//
// Created by alkaid on 2020/2/7.
//

#include "Epoll.h"

extern pthread_mutex_t qlock;
extern pthread_cond_t cont;
extern priority_queue<timer*, deque<timer*>, timerCmp> myTimerQueue;
extern unordered_map<int,timer*> fd2time;

void SetNonBlocking(int fd){
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option|O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
}

void *Check_Time(void *args){
    while(true){
        pthread_mutex_lock(&qlock);
        if(myTimerQueue.empty()){
            pthread_cond_wait(&cont,&qlock);
        }
        if(myTimerQueue.top()->deleted==true||time(NULL)-myTimerQueue.top()->used_time > TIMELIMIT){
            cout<<"超时删除，队列大小："<<myTimerQueue.size()<<endl;
            timer* cur = myTimerQueue.top();
            myTimerQueue.pop();
            delete cur; //这里进行了二次删除
        }
        pthread_mutex_unlock(&qlock);
    }
}

void HandleRequest(void *args){
    RequestData *req_data = (RequestData*)args;
    if(req_data->Handle_Read()) {
        req_data->Handle_Write();
    }
    return ;
}

void Epoll(int listenfd){
    int epollfd;
    struct epoll_event event[EPOLLEVENTS];
    int ret;
    epollfd = epoll_create(FDSIZE); //建立一颗红黑树存储需要监视的socket以及对应的事件，建立双向链表存储准备就绪的事件
    //setnonblocking(epollfd);
    pthread_mutex_init(&qlock, NULL);
    pthread_cond_init(&cont,NULL);
    pthread_t *check_thread = (pthread_t *)malloc(sizeof(pthread_t)); //检查线程
    pthread_create(check_thread,NULL, Check_Time, NULL);
    RequestData *data = new RequestData(epollfd,listenfd);
    SetNonBlocking(listenfd);
    threadpool_t* pool = threadpool_create(8, 65535);
    add_event(epollfd,listenfd,data,EPOLLIN); //为监听套接字标记上EPOLLIN，确保可以执行ACCEPT(),不能使用EPOLLET！
    while(true){
        ret = epoll_wait(epollfd,event,EPOLLEVENTS,-1); //观察双向链表，等待事件的就绪，成功时返回就绪事件数目。
        Handle_Events(epollfd,event,ret,listenfd, pool);
    }
    close(epollfd);
}

void Handle_Events(int epollfd, struct epoll_event *events, int num, int listenfd, threadpool_t *pool){
    for(int i=0;i<num;i++){
        RequestData *request = (RequestData*)(events[i].data.ptr);
        int fd = request->Get_fd();
        SetNonBlocking(fd);
        if((fd == listenfd)&&(events[i].events&EPOLLIN)) {Handle_Accept(epollfd, fd);} //如果是监听套接字，则进行accpet()
        else if(events[i].events&EPOLLIN){
            threadpool_add(pool,HandleRequest,events[i].data.ptr);
        }
    }
}

void Handle_Accept(int epollfd,int listenfd)
{
    struct sockaddr_in cliaddr;
    socklen_t  cliaddrlen=sizeof(cliaddr);
    int clifd = Accept(listenfd,(struct sockaddr*)&cliaddr,&cliaddrlen); //连接成功返回
    if (clifd == -1)
        perror("accpet error:");
    else
    {
        cout<<"accept a new client"<<endl;
        RequestData *data = new RequestData(epollfd,clifd);
        add_event(epollfd,clifd,data,EPOLLIN|EPOLLET);
    }
}

//epoll_ctl的添加事件情况
int add_event(int epollfd,int fd,void *data,int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.ptr = data;
    if(epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&ev)==-1){
        //cout<<"增加失败"<<endl;
        perror("epoll_add");
        return -1;
    }
    return 1;
}

//epoll_ctl的删除事件情况
int del_event(int epollfd,int fd,void *data, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.ptr = data;
    if(epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,&ev)==-1) {
        //cout<<"删除失败"<<endl;
        perror("epoll_del");
        return -1;
    }
    return 1;
}

//epoll_ctl的修改事件情况
int mod_event(int epollfd,int fd,void *data,int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.ptr = data;
    if(epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&ev)==-1) {
        //cout<<"修改失败"<<endl;
        perror("epoll_mod");
        return -1;
    }
    return 1;
}
#include "epollIO.h"

using namespace std;


pthread_mutex_t qlock;
pthread_cond_t cont;
priority_queue<timer*, deque<timer*>, timerCmp> myTimerQueue;

void setnonblocking(int fd){
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option|O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
}

void *check_time(void *args){
    while(true){
        pthread_mutex_lock(&qlock);
        if(myTimerQueue.empty()){
            pthread_cond_wait(&cont,&qlock);
        }
        if(myTimerQueue.top()->deleted==true||time(NULL)-myTimerQueue.top()->used_time > TIMELIMIT){
            cout<<myTimerQueue.size()<<endl;
            timer* cur = myTimerQueue.top();
            myTimerQueue.pop();
            delete cur; //这里进行了二次删除
        }
        pthread_mutex_unlock(&qlock);
    }
}
void handle(void *args){
    RequestData *req_data = (RequestData*)args;
    //req_data->updatetimer();
    req_data->do_read();
    req_data->do_write();
    //else{
        //delete req_data;
        //cout<<"read error"<<endl;
    //}
    return ;
}

void my_epoll(int listenfd){
    int epollfd;
    struct epoll_event event[EPOLLEVENTS];
    int ret;
    epollfd = epoll_create(FDSIZE); //建立一颗红黑树存储需要监视的socket以及对应的事件，建立双向链表存储准备就绪的事件
    pthread_mutex_init(&qlock, NULL);
    pthread_cond_init(&cont,NULL);
    pthread_t *check_thread = (pthread_t *)malloc(sizeof(pthread_t)); //检查线程
    pthread_create(check_thread,NULL, check_time, NULL);
    RequestData *data = new RequestData();
    setnonblocking(listenfd);
    data->setfd(epollfd, listenfd);
    threadpool_t* pool = threadpool_create(8, 65535);
    add_event(epollfd,listenfd,data,EPOLLIN); //为监听套接字标记上EPOLLIN，确保可以执行ACCEPT()
    while(true){
        ret = epoll_wait(epollfd,event,EPOLLEVENTS,-1); //观察双向链表，等待事件的就绪，成功时返回就绪事件数目。
        handle_events(epollfd,event,ret,listenfd, pool);
        // check_time();
    }
    close(epollfd);
}

void handle_events(int epollfd, struct epoll_event *events, int num, int listenfd, threadpool_t *pool){
    for(int i=0;i<num;i++){
        RequestData *request = (RequestData*)(events[i].data.ptr);
        int fd = request->getfd();
        setnonblocking(fd);
        if((fd == listenfd)&&(events[i].events&EPOLLIN)) {handle_accpet(epollfd, fd);} //如果是监听套接字，则进行accpet()
        else if(events[i].events&EPOLLIN){
            threadpool_add(pool,handle,events[i].data.ptr);
        }
    }
}

void handle_accpet(int epollfd,int listenfd)
{
    struct sockaddr_in cliaddr;
    socklen_t  cliaddrlen=sizeof(cliaddr);
    int clifd = Accept(listenfd,(struct sockaddr*)&cliaddr,&cliaddrlen); //连接成功返回
    if (clifd == -1)
        perror("accpet error:");
    else
    {
        cout<<"accept a new client"<<endl;
        RequestData *data = new RequestData();
        data->setfd(epollfd,clifd);
        add_event(epollfd,clifd,data,EPOLLIN|EPOLLET);
    }
}

//epoll_ctl的添加事件情况
int add_event(int epollfd,int fd,void *data,int state)
{
    setnonblocking(fd);
    struct epoll_event ev;
    ev.events = state;
    ev.data.ptr = (void*)data;
    if(epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&ev)==-1){
        //cout<<"增加失败"<<endl;
        perror("epoll_add");
        return -1;
    }
    return 1;
}

//epoll_ctl的删除事件情况
int delete_event(int epollfd,int fd,void *data, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.ptr = (void*)data;
    if(epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,&ev)==-1) {
        //cout<<"删除失败"<<endl;
        perror("epoll_del");
        return -1;
    }
    return 1;
}

//epoll_ctl的修改事件情况
int modify_event(int epollfd,int fd,void *data,int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.ptr = (void*)data;
    if(epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&ev)==-1) {
        //cout<<"修改失败"<<endl;
        perror("epoll_mod");
        return -1;
    }
    return 1;
}



int RequestData::getfd() {
    return fd;
}

void RequestData::setfd(int epoll_fd,int _fd) {
    epollfd = epoll_fd;
    fd = _fd;
}
void RequestData::updatetimer() {
    if(mytimer!=NULL){
        //cout<<"更新时间"<<endl;
        mytimer->used_time = time(NULL);
    }
}
void RequestData::addtimer(timer* mytime) {
    cout<<"添加计时器"<<endl;
    this->mytimer = mytime;

}

void RequestData::getRequestHead(char *buffer,int msgLen)
{
    stringstream ss(string(buffer,buffer+msgLen));
    re_content = ss.str();
    if(re_content.find("keep-alive")!=re_content.npos)
    {
        keep_alive=1; // 设置长连接
    }
    ss>>method;
    ss>>uri;
}

//根据不同的请求方法执行
void RequestData::do_response()
{
    if(method=="POST")
    {
        this->do_post_response();
    }
    else if(method=="HEAD")
    {
        //跟get类似仅仅返回响应头不返回内容
        //  do_head_response();
    }
    else //default GET
    {
        this->do_get_response();
    }
}

//发送POST请求响应
void RequestData::do_post_response(){
    string statusCode;
    content = this->getContent_Post(statusCode);
    responseheader=this->constructHeader();
}

string RequestData::getContent_Post(string &statusCode)
{
}

//发送GET请求响应
void RequestData::do_get_response()
{
    string statusCode;
    content=this->getContent(statusCode);
    responseheader=this->constructHeader();
}

//获取content
string RequestData::getContent(string &statusCode)
{
    string fileName=this->parseURI();
    //解析content-type
    this->parseContentType(fileName);
    ifstream f;//打开文件
    f.open(fileName);
    if(f.fail())//no such file
    {
        status="404 Not Found";
        return errorContent();
    }
    string tmp;
    string content;
    while(f){
        getline(f,tmp);
        content += tmp;
    }
     status="200 OK";
    f.close();
    return content;
}


//打开一个errorpage
string RequestData::errorContent()
{
    ifstream f("error.html");
    string tmp;
    string content;
    while(f>>tmp)
    {
        content+=tmp;
    }
    f.close();
    return content;
}

//解析uri
string RequestData::parseURI()
{
    if(uri=="/")
        uri+="BookMark.html";
    string webroot="/data/C++_Test/cmake-build-debug/";
    return webroot+uri;
}

//解析Content-type
void RequestData::parseContentType(string fileName)
{
    string tmpExtension=this->getExtent(fileName); //获取文件扩展名
    ifstream mimeFile("mime.types");
    string line;
    while(getline(mimeFile,line))
    {
        if(line[0]!='#')
        {
            stringstream lineStream(line);
            contentType.clear();
            lineStream>>contentType;
            vector<string>extensions;
            string extension;
            while(lineStream>>extension)
            {
                extensions.push_back(extension);
            }
            for(int i=0;i<extensions.size();++i)
            {
                if(tmpExtension==extensions[i])
                {
                    mimeFile.close();
                    return ;
                }
            }

        }
    }
    //如果都不匹配就默认为text/plain
    contentType="text/plain";
    mimeFile.close();
}

//获取文件扩展名
string RequestData::getExtent(string fileName)
{
    int i;
    for(i=fileName.size()-1;;--i)
    {
        if(fileName[i]=='.')
            break;
    }
    return string(fileName.begin()+i+1,fileName.end());
}

//构造响应头
string RequestData::constructHeader()
{
    string contentSize(std::to_string(content.size()));
    string head("\r\nHTTP/1.1 ");
    string ContentType("\r\nContent-Type: ");
    string ServerHead("\r\nServer: localhost");
    string ContentLength("\r\nContent-Length: ");
    string Date("\r\nDate: ");
    string Newline("\r\n");
    time_t rawtime;
    time(&rawtime);
    string message;
    message+=head;
    message+=status;
    message+=ContentType;
    message+=contentType;
    message+=ServerHead;
    message+=ContentLength;
    message+=contentSize;
    message+=Date;
    message+=(string)ctime(&rawtime);
    message+=Newline;
    return message;
}

bool RequestData::do_read()
{
    int msgLen = 0;
    char buffer[BUFFER_SIZE];
    memset(buffer,'\0',BUFFER_SIZE);
    if((msgLen = recv(fd, buffer, BUFFER_SIZE,0)) == -1)
    {
        cout<<"Error handling incoming request"<<endl;
        close(fd);
        delete_event(epollfd,fd,this,EPOLLIN|EPOLLET);
        return false;
    }
    else if(msgLen==0)
    {
        cout<<"client closed\r\n"<<endl;
        close(fd);
        delete_event(epollfd,fd,this,EPOLLIN|EPOLLET);
        return false;
    }
    else
    {
        this->getRequestHead(buffer,msgLen); //获取请求头(method和uri)
        this->do_response(); //制作响应报文
        modify_event(epollfd,fd,this,EPOLLOUT); // 更新事件
        return true;
    }
}

//发送数据
void RequestData::do_write()
{
    send(fd,responseheader.c_str(),responseheader.size(),0);
    send(fd,content.c_str(),content.size(),0);
    if(keep_alive)
    {
        cout<<"keep alive ok"<<endl;
        modify_event(epollfd,fd,this,EPOLLIN|EPOLLET);
        if(mytimer==NULL) {
            pthread_mutex_lock(&qlock);
            timer *cur = new timer(this);
            this->addtimer(cur);
            myTimerQueue.push(cur);
            if (myTimerQueue.size() == 1) {
                pthread_cond_signal(&cont);
            }
            pthread_mutex_unlock(&qlock);
        }
        this->reset();
    }
    else{
        //this->mytimer->setDeleted();
        delete_event(epollfd,fd,this,EPOLLOUT);
        //delete this;
        close(fd);
        return;
    }
}

void RequestData::reset() {
    content.clear();
    method.clear();
    uri.clear();
    status.clear();
    contentType.clear();
    responseheader.clear();
    keep_alive = false;
}

RequestData::~RequestData() {
    //delete_event(epollfd,fd,this,EPOLLOUT);
    if(mytimer!=NULL){
        cout<<"分离计时器中的请求对象"<<endl;
        mytimer->clearReq();
        cout<<"分离成功"<<endl;
        mytimer = NULL;
    }
    perror("~RequestData");
    close(fd);
}
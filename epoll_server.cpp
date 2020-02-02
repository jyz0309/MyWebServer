//
// Created by alkaid on 2019/12/9.
//
//
// Created by alkaid on 2019/12/7.
//

#include "epoll_server.h"
using namespace std;
//创建socket套接字
int Socket(int family, int type, int protocol)
{
    int n;
    if ((n=socket(family,type,protocol))<0){
        cout<<"socket error\r\n"<<endl;
        return -1;
    }
    return n;
}
//绑定套接字
void Bind(int fd,const struct sockaddr *sa, socklen_t salen){
    if(bind(fd, sa, salen) < 0){
        cout<<"bind error\r\n"<<endl;
        exit(-1);
    }
}
//将套接字类型转变为监听套接字
void Listen(int fd, int backlog){
    char *ptr;
    if((ptr=getenv("LISTENQ"))!= NULL) backlog = atoi(ptr);
    if (listen(fd, backlog)<0){
        cout<<"listen error"<<endl;
        return;
    }
}
//等待接收连接
int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
    int n;
    again:
    if((n=accept(fd, sa, salenptr)) < 0){
#ifdef  EPROTO
        if (errno == EPROTO || errno == ECONNABORTED)
#else
            if (errno == ECONNABORTED)
#endif
            goto again;
        else
        {
            printf("accept error\r\n");
            return -1;
        }
    }
    return n;
}


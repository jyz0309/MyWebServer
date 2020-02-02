#include "epollIO.h"

using namespace std;

//忽略SIGPIPE信号
void addsig(int sig,void(handler)(int), bool restart=true){
    struct sigaction sa;
    memset(&sa,'\0', sizeof(sa));
    sa.sa_handler=handler;
    if(restart){
        sa.sa_flags |= SA_RESTART;
    }
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig,&sa,NULL)!=-1);
}

int main(int argc, char **argv)
{
    const int port = 8080;
    addsig(SIGPIPE, SIG_IGN);
    int listenfd=Socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(port);
    int opt = 1;
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,(const void*)&opt,sizeof(opt));
    Bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    Listen(listenfd, 5);
    setnonblocking(listenfd);
    my_epoll(listenfd);
    return 0;
}

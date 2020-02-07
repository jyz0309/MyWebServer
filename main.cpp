#include "Epoll.h"
#include "Socket.h"
#include <assert.h>
#include "UtilFunc.h"

using namespace std;

//忽略SIGPIPE信号


int main(int argc, char **argv)
{
    const int port = 1024;
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
    SetNonBlocking(listenfd);
    Epoll(listenfd);
    return 0;
}

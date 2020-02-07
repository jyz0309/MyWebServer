//
// Created by alkaid on 2020/2/7.
//

#ifndef C___TEST_SOCKET_H
#define C___TEST_SOCKET_H

#include <sys/socket.h>
#include <iostream>
#include <sys/epoll.h>

#define BUFFER_SIZE 4096

int Socket(int, int, int);
void Bind(int,const struct sockaddr* sa,socklen_t salen);
void Listen(int, int);
int Accept(int, struct sockaddr*, socklen_t*);


#endif //C___TEST_SOCKET_H

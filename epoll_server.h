//
// Created by alkaid on 2019/12/2.
//

#ifndef CLIONPROJECTS_EPOLL_SERVER_H
#define CLIONPROJECTS_EPOLL_SERVER_H

#endif //CLIONPROJECTS_EPOLL_SERVER_H
//
// Created by alkaid on 2019/11/28.
//
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <cerrno>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <sys/epoll.h>
#include <fcntl.h>
#include<cassert>
#include <malloc.h>

#define BUFFER_SIZE 4096

int Socket(int, int, int);
void Bind(int,const struct sockaddr* sa,socklen_t salen);
void Listen(int, int);
int Accept(int, struct sockaddr*, socklen_t*);


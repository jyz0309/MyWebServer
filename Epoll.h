//
// Created by alkaid on 2020/2/7.
//

#ifndef C___TEST_EPOLL_H
#define C___TEST_EPOLL_H

#include "Socket.h"
#include "RequestData.h"
#include <unordered_map>
#include <queue>
#include <sys/epoll.h>
#include <fcntl.h>
#include <pthread.h>
#include <cerrno>
#include <unistd.h>
#include <ctime>
#include <netinet/in.h>

using namespace std;

#define FDSIZE 1000
#define EPOLLEVENTS 100
#define TIMELIMIT 5

class RequestData;
class timer;
struct timerCmp;

void Epoll(int listenfd);
void Handle_Events(int epollfd, epoll_event *events, int num, int listenfd,
                   threadpool_t *pThreadpool);
void Handle_Accept(int epollfd,int listenfd);
int add_event(int epollfd, int fd,void *data ,int state);
int del_event(int epollfd,int fd,void *data,int state);
int mod_event(int epollfd,int fd,void *data,int state);
void SetNonBlocking(int fd);
void *Check_Time(void *args);
void HandleRequest(void *args);
#endif //C___TEST_EPOLL_H

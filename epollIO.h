//
// Created by alkaid on 2019/12/9.
//

#ifndef C___TEST_EPOLLIO_H
#include "threadpool.h"
#include "epoll_server.h"
#include <unordered_map>
#include <queue>
#include <unistd.h>
#include <ctime>

using namespace std;
#define FDSIZE 1000
#define EPOLLEVENTS 100
#define TIMELIMIT 5
class RequestData;
class timer;
struct timerCmp;

class RequestData{
private:
    string method;
    string uri;
    string content;
    string re_content;
    string responseheader;
    string status;
    string contentType; //以上都是构造请求时的内容
    int fd;
    int epollfd;
    int keep_alive=0;
public:
    timer *mytimer = NULL;
    ~RequestData();
    void updatetimer();
    void addtimer(timer* mytime);
    int getfd();
    void setfd(int epoll_fd, int fd_);
    void getRequestHead(char *buffer,int msgLen);
    void do_response();
    void do_get_response();
    void do_post_response();
    string getContent_Post(string &statusCode);
    string getContent(string &statusCode);
    string errorContent();
    string parseURI();
    void parseContentType(string fileName);
    string getExtent(string fileName);
    string constructHeader();
    bool do_read();
    void do_write();
    void reset();
};

class timer{
public:
    bool deleted;
    RequestData *data;
    time_t used_time;
    timer(RequestData *_data):data(_data),deleted(false){
        used_time = time(NULL);
    }
    ~timer(){
        if(data!=NULL){
            cout<<"销毁data"<<endl;
            delete data;
        }
        perror("~timer");
    }
    void clearReq(){
        data = NULL;
        this->setDeleted();
    }
    void setDeleted(){
        deleted = true;
    }
};

struct timerCmp
{
    bool operator()(const timer *a, const timer *b) const
    {
        return a->used_time > b->used_time;
    }
};


void my_epoll(int listenfd);
void handle_events(int epollfd, epoll_event *events, int num, int listenfd,
                   threadpool_t *pThreadpool);
void handle_accpet(int epollfd,int listenfd);
int add_event(int epollfd, int fd,void *data ,int state);
int delete_event(int epollfd,int fd,void *data,int state);
int modify_event(int epollfd,int fd,void *data,int state);
void setnonblocking(int fd);
#endif //C___TEST_EPOLLIO_H
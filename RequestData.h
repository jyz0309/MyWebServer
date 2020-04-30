//
// Created by alkaid on 2020/2/7.
//

#ifndef C___TEST_REQUESTDATA_H
#define C___TEST_REQUESTDATA_H

#include <iostream>
#include <vector>
#include <ctime>
#include <cstring>
#include <sstream>
#include <json/json.h>
#include <fstream>
#include <string>
#include <unordered_map>
#include <queue>
#include <unistd.h>
#include "Socket.h"
#include "ThreadPool.h"
#include "Epoll.h"

using namespace std;
class timer;

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
    int keep_alive;
public:
    timer *mytimer = NULL;
    RequestData(int epollfd_, int fd_);
    ~RequestData();
    int Get_fd();
    void getRequestHead(char *buffer,int msgLen);
    void do_response();
    void do_get_response();
    void do_post_response();
    Json::Value handle_post_req(Json::Value data,string uri_name);
    string getContent(string &statusCode);
    string errorContent();
    string parseURI();
    void parseContentType(string fileName);
    string getExtent(string fileName);
    string constructHeader();
    bool Handle_Read();
    void Handle_Write();
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
            data = NULL;
        }
        perror("~timer");
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

#endif //C___TEST_REQUESTDATA_H

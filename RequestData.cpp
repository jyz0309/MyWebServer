//
// Created by alkaid on 2020/2/7.
//

#include "RequestData.h"

pthread_mutex_t qlock;
pthread_cond_t cont;
priority_queue<timer*, deque<timer*>, timerCmp> myTimerQueue;
unordered_map<int,timer*> fd2time;

RequestData::RequestData(int epollfd_, int fd_):epollfd(epollfd_),fd(fd_),keep_alive(0) {}

RequestData::~RequestData() {
    perror("~RequestData");
    close(fd);
}

bool RequestData::Handle_Read()
{
    int msgLen = 0;
    char buffer[BUFFER_SIZE];
    memset(buffer,'\0',BUFFER_SIZE);
    if((msgLen = recv(fd, buffer, BUFFER_SIZE,0)) == -1)
    {
        cout<<"Error handling incoming request\r\n"<<endl;
        del_event(epollfd,fd,this,EPOLLIN|EPOLLET);
        auto iter = fd2time.find(fd);
        if(iter!=fd2time.end()){
            fd2time[fd]->deleted=true;
            fd2time.erase(iter);
        }
        return false;
    }
    else if(msgLen==0)
    {
        cout<<"client closed\r\n"<<endl;
        close(fd);
        del_event(epollfd,fd,this,EPOLLIN|EPOLLET);
        //close(fd);
        auto iter = fd2time.find(fd);
        if(iter!=fd2time.end()){
            fd2time[fd]->deleted=true;
            fd2time.erase(iter);
        }
        return false;
    }
    else
    {
        this->getRequestHead(buffer,msgLen); //获取请求头(method和uri)
        this->do_response(); //制作响应报文
        return true;
    }
}

//发送数据
void RequestData::Handle_Write()
{
    send(fd,responseheader.c_str(),responseheader.size(),0);
    send(fd,content.c_str(),content.size(),0);
    if(keep_alive)
    {
        cout<<"keep alive ok"<<endl;
        if(mytimer==NULL) {
            pthread_mutex_lock(&qlock);
            cout<<"添加计时器"<<endl;
            timer *cur = new timer(this);
            fd2time[fd] = cur;
            this->mytimer = cur;
            myTimerQueue.push(cur);
            if (myTimerQueue.size() == 1) {
                pthread_cond_signal(&cont);
            }
            pthread_mutex_unlock(&qlock);
        }
        this->reset();
    }
    else{
        del_event(epollfd,fd,this,EPOLLIN|EPOLLET);
        delete this;
        return;
    }
}

int RequestData::Get_fd() {
    return fd;
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

void RequestData::do_post_response() {
    string statusCode;
    content = this->getContent(statusCode);
    string uri_name = this->parseURI();
    Json::Reader reader;
    Json::Value data;
    reader.parse(content, data);
    // 根据data进行重定向
    responseheader=this->constructHeader();
}

Json::Value RequestData::handle_post_req(Json::Value data, string uri_name){
    //根据DATA的值进行一系列处理
    return data;
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

void RequestData::reset() {
    content.clear();
    method.clear();
    uri.clear();
    re_content.clear();
    method.clear();
    status.clear();
    contentType.clear();
    responseheader.clear();
    keep_alive = false;
}

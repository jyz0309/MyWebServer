//
// Created by alkaid on 2020/2/7.
//

#include "log.h"

#include <errno.h>
#include <unistd.h>//access, getpid
#include <assert.h>//assert
#include <stdarg.h>//va_list
#include <sys/stat.h>//mkdir
#include <sys/syscall.h>

#define MEM_USE_LIMIT (3u * 1024 * 1024 * 1024)//3GB
#define LOG_USE_LIMIT (1u * 1024 * 1024 * 1024)//1GB
#define LOG_LEN_LIMIT (4 * 1024)//4K
#define RELOG_THRESOLD 5
#define BUFF_WAIT_TIME 1

uint32_t One_Buffer_Size = 10*1024*1024;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;



Log::Log(int size):Buffer_size(size),Producer(NULL),Consumer(NULL),t() {
    Buffer_Node *head = new Buffer_Node(One_Buffer_Size);
    Buffer_Node* p = head;
    Buffer_Node *cur;
    Producer = head;
    Consumer = head;
    for(int i = 1;i<size;i++){
        cur = new Buffer_Node(One_Buffer_Size);
        head->next = cur;
        cur->prev = head;
        head = cur;
    }
    head->next = p;
    p->prev = head;
}

void Log::Init_Path(const char* log_dir, const char* prog_name, int level)
{
    pthread_mutex_lock(&lock);

    strncpy(dir, log_dir, 256);
    //name format:  name_year-mon-day-t[tid].log.n
    strncpy(file_name, prog_name, 128);
    mkdir(dir, 0777);
    //查看是否存在此目录、目录下是否允许创建文件
    if (access(dir, F_OK | W_OK) == -1)
    {
        fprintf(stderr, "logdir: %s error: %s\n", dir, strerror(errno));
    }
    else{
        printf("创建文件失败!");
    }
    if (level > TRACE)
        level = TRACE;
    if (level < FATAL)
        level = FATAL;
    log_level = level;
    pthread_mutex_unlock(&lock);
}

void Log::persist(){
    while(true){
        pthread_mutex_lock(&lock);
        if(Consumer->status == Buffer_Node::Free){
            struct timespec tsp;
            struct timeval now;
            gettimeofday(&now, NULL);
            tsp.tv_sec = now.tv_sec;
            tsp.tv_nsec = now.tv_usec * 1000;//nanoseconds
            tsp.tv_sec += 1;//wait for 1 seconds
            pthread_cond_timedwait(&cond, &lock, &tsp); //等待一秒
        }
        if(Consumer->empty()){
            pthread_mutex_unlock(&lock);
            continue;
        }
        if(Consumer->status == Buffer_Node::Free){
            Producer->status = Buffer_Node::Full;
            Producer = Producer->next;
        }
        int year = t.year, mon = t.mon, day = t.day;
        pthread_mutex_unlock(&lock);
        if (!choose_file(year, mon, day))
            continue;
        //write
        Consumer->write(fp_);
        fflush(fp_);
        pthread_mutex_lock(&lock);
        Consumer->clear();
        Consumer = Consumer->next;
        pthread_mutex_unlock(&lock);
    }
}

void Log::append(const char* lvl, const char* format, ...)
{
    int ms;
    uint64_t curr_sec = t.get_curr_time(&ms);
    char log_line[LOG_LEN_LIMIT];
    int prev_len = snprintf(log_line, LOG_LEN_LIMIT, "%s[%s.%03d]", lvl, t.utc_fmt, ms);
    va_list arg_ptr;
    va_start(arg_ptr, format);
    //TO OPTIMIZE IN THE FUTURE: performance too low here!
    int main_len = vsnprintf(log_line + prev_len, LOG_LEN_LIMIT - prev_len, format, arg_ptr);
    va_end(arg_ptr);
    uint32_t len = prev_len + main_len;
    bool tell_back = false;
    pthread_mutex_lock(&lock);
    if (Consumer->status == Buffer_Node::Free && Consumer->availble_len() >= len)
    {
        Consumer->append(log_line, len);
    }
    else
    {
        if (Consumer->status == Buffer_Node::Free)
        {
            Consumer->status = Buffer_Node::Full;//set to FULL
            Buffer_Node* next_buf = Consumer->next;
            //tell backend thread
            tell_back = true;

            //it suggest that this buffer is under the persist job
            if (next_buf->status == Buffer_Node::Full)
            {
                //if mem use < MEM_USE_LIMIT, allocate new cell_buffer
                if (30*1024*1024 * (Buffer_size + 1) > MEM_USE_LIMIT)
                {
                    fprintf(stderr, "no more log space can use\n");
                    Consumer = next_buf;
                }
                else
                {
                    Buffer_Node* new_buffer = new Buffer_Node(30*1024*1024);
                    Buffer_size += 1;
                    new_buffer->prev = Consumer;
                    Consumer->next = new_buffer;
                    new_buffer->next = next_buf;
                    next_buf->prev = new_buffer;
                    Consumer = new_buffer;
                }
            }
            else
            {
                //next buffer is free, we can use it
                Consumer = next_buf;
            }
            Consumer->append(log_line, len);
        }
    }
    pthread_mutex_unlock(&lock);
    if (tell_back)
    {
        pthread_cond_signal(&cond);
    }
}

bool Log::choose_file(int y, int m, int d){
    if(!fp_){
        year = y,mon = m, day = d;
        char log_path[512] = {};
        sprintf(log_path,"%s/%s.%d%02d%02d.log", dir, file_name, year, mon, day);
        fp_ = fopen(log_path,"w");
    }
    else if(d != day){
        fclose(fp_);
        year = y,mon = m, day = d;
        char log_path[512] = {};
        sprintf(log_path,"%s/%s.%d%02d%02d.log", dir, file_name, year, mon, day);
        fp_ = fopen(log_path,"w");
    }
    else if(ftell(fp_)>=1u * 1024 * 1024 * 1024){
        fclose(fp_);
        //mv xxx.log.[i] xxx.log.[i + 1]
        year = y,mon = m, day = d;
        char log_path[512] = {};
        sprintf(log_path,"%s/%s.%d%02d%02d.log.1", dir, file_name, year, mon, day);
        fp_ = fopen(log_path,"w");
    }
    return fp_ != NULL;
}
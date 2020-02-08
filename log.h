//
// Created by alkaid on 2020/2/7.
//

#ifndef OS_LOG_H
#define OS_LOG_H

#include <cstdio>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>

enum LOG_LEVEL{
    FATAL = 1,
    ERROR,
    WARN,
    INFO,
    DEBUG,
    TRACE,
};

extern pid_t gettid();

class Time{
private:
    void reset_utc_fmt()
    {
        snprintf(utc_fmt, 20, "%d-%02d-%02d %02d:%02d:%02d", year, mon, day, hour, min, sec);
    }

    void reset_utc_fmt_sec()
    {
        snprintf(utc_fmt + 17, 3, "%02d", sec);
    }

    uint64_t _sys_acc_min;
    uint64_t _sys_acc_sec;
public:
    int year,mon,day,hour,min,sec;
    char utc_fmt[20];
    Time(){
        struct timeval tv;
        gettimeofday(&tv,NULL);
        _sys_acc_sec = tv.tv_sec;
        _sys_acc_min = _sys_acc_sec/60;
        struct tm cur_tm;
        localtime_r((time_t*)&_sys_acc_sec, &cur_tm);
        year = cur_tm.tm_year+1900;
        mon  = cur_tm.tm_mon + 1;
        day  = cur_tm.tm_mday;
        hour  = cur_tm.tm_hour;
        min  = cur_tm.tm_min;
        sec  = cur_tm.tm_sec;
        reset_utc_fmt();
    }
    uint64_t get_curr_time(int* p_msec = NULL)
    {
        struct timeval tv;
        //get current ts
        gettimeofday(&tv, NULL);
        if (p_msec)
            *p_msec = tv.tv_usec / 1000;
        //if not in same seconds
        if ((uint32_t)tv.tv_sec != _sys_acc_sec)
        {
            sec = tv.tv_sec % 60;
            _sys_acc_sec = tv.tv_sec;
            //or if not in same minutes
            if (_sys_acc_sec / 60 != _sys_acc_min)
            {
                //use _sys_acc_sec update year, mon, day, hour, min, sec
                _sys_acc_min = _sys_acc_sec / 60;
                struct tm cur_tm;
                localtime_r((time_t*)&_sys_acc_sec, &cur_tm);
                year = cur_tm.tm_year + 1900;
                mon  = cur_tm.tm_mon + 1;
                day  = cur_tm.tm_mday;
                hour = cur_tm.tm_hour;
                min  = cur_tm.tm_min;
                //reformat utc format
                reset_utc_fmt();
            }
            else
            {
                //reformat utc format only sec
                reset_utc_fmt_sec();
            }
        }
        return tv.tv_sec;
    }
};
class Buffer_Node{
public:
    enum buffer_status{
        Free,
        Full
    };
    buffer_status status;
    Buffer_Node* next;
    Buffer_Node* prev;
    Buffer_Node(uint32_t len_):
    status(Free),prev(NULL),next(NULL),len(len_),used_len(0){
        data = new char[len];
        if(!data){
            fprintf(stderr,"The space not enough to allocate");
            exit(1);
        }
    }
    uint32_t availble_len(){return len-used_len;}
    bool empty(){return used_len==0;}
    void append(const char* line, uint32_t len){
        if(availble_len()<len){
            return;
        }
        memcpy(data+used_len,line,len);
        used_len+=len;
    }
    void clear(){
        used_len = 0;
        status = Free;
    }
    void write(FILE* fp){
        uint32_t len = fwrite(data,1,used_len,fp);
        if(len!=used_len){
            fprintf(stderr,"write log to disk error");
        }
    }
private:
    Buffer_Node(const Buffer_Node&);
    Buffer_Node& operator=(const Buffer_Node&);
    uint32_t len;
    uint32_t used_len;
    char* data;
};


class Log
{
public:
    static Log* Sample()
    {
        pthread_once(&once, Log::init);
        return sample;
    }

    static void init()
    {
        while (!sample) sample = new Log(3);
    }
    Log(int size);
    void Init_Path(const char* log_dir, const char* prog_name, int level);
    void persist();
    bool choose_file(int year, int mon, int day);
    void append(const char* lvl, const char* format, ...);
private:
    int Buffer_size;
    Buffer_Node* Producer;
    Buffer_Node* Consumer;
    char dir[256];
    char file_name[128];
    int year,mon,day;
    int log_level;
    Time t;
    FILE *fp_;
    static Log* sample;
    static pthread_once_t once;
};


#define LOG_MEM_SET(mem_lmt) \
    do \
    { \
        if (mem_lmt < 90 * 1024 * 1024) \
        { \
            mem_lmt = 90 * 1024 * 1024; \
        } \
        else if (mem_lmt > 1024 * 1024 * 1024) \
        { \
            mem_lmt = 1024 * 1024 * 1024; \
        } \
        ring_log::_one_buff_len = mem_lmt; \
    } while (0)

#define LOG_INIT(log_dir, prog_name, level) \
    do \
    { \
        ring_log::ins()->init_path(log_dir, prog_name, level); \
        pthread_t tid; \
        pthread_create(&tid, NULL, be_thdo, NULL); \
        pthread_detach(tid); \
    } while (0)

//format: [LEVEL][yy-mm-dd h:m:s.ms][tid]file_name:line_no(func_name):content
#define LOG_TRACE(fmt, args...) \
    do \
    { \
        if (ring_log::ins()->get_level() >= TRACE) \
        { \
            ring_log::ins()->try_append("[TRACE]", "[%u]%s:%d(%s): " fmt "\n", \
                    gettid(), __FILE__, __LINE__, __FUNCTION__, ##args); \
        } \
    } while (0)

#define LOG_DEBUG(fmt, args...) \
    do \
    { \
        if (ring_log::ins()->get_level() >= DEBUG) \
        { \
            ring_log::ins()->try_append("[DEBUG]", "[%u]%s:%d(%s): " fmt "\n", \
                    gettid(), __FILE__, __LINE__, __FUNCTION__, ##args); \
        } \
    } while (0)

#define LOG_INFO(fmt, args...) \
    do \
    { \
        if (ring_log::ins()->get_level() >= INFO) \
        { \
            ring_log::ins()->try_append("[INFO]", "[%u]%s:%d(%s): " fmt "\n", \
                    gettid(), __FILE__, __LINE__, __FUNCTION__, ##args); \
        } \
    } while (0)

#define LOG_NORMAL(fmt, args...) \
    do \
    { \
        if (ring_log::ins()->get_level() >= INFO) \
        { \
            ring_log::ins()->try_append("[INFO]", "[%u]%s:%d(%s): " fmt "\n", \
                    gettid(), __FILE__, __LINE__, __FUNCTION__, ##args); \
        } \
    } while (0)

#define LOG_WARN(fmt, args...) \
    do \
    { \
        if (ring_log::ins()->get_level() >= WARN) \
        { \
            ring_log::ins()->try_append("[WARN]", "[%u]%s:%d(%s): " fmt "\n", \
                    gettid(), __FILE__, __LINE__, __FUNCTION__, ##args); \
        } \
    } while (0)

#define LOG_ERROR(fmt, args...) \
    do \
    { \
        if (ring_log::ins()->get_level() >= ERROR) \
        { \
            ring_log::ins()->try_append("[ERROR]", "[%u]%s:%d(%s): " fmt "\n", \
                gettid(), __FILE__, __LINE__, __FUNCTION__, ##args); \
        } \
    } while (0)

#define LOG_FATAL(fmt, args...) \
    do \
    { \
        ring_log::ins()->try_append("[FATAL]", "[%u]%s:%d(%s): " fmt "\n", \
            gettid(), __FILE__, __LINE__, __FUNCTION__, ##args); \
    } while (0)

#define TRACE(fmt, args...) \
    do \
    { \
        if (ring_log::ins()->get_level() >= TRACE) \
        { \
            ring_log::ins()->try_append("[TRACE]", "[%u]%s:%d(%s): " fmt "\n", \
                    gettid(), __FILE__, __LINE__, __FUNCTION__, ##args); \
        } \
    } while (0)

#define DEBUG(fmt, args...) \
    do \
    { \
        if (ring_log::ins()->get_level() >= DEBUG) \
        { \
            ring_log::ins()->try_append("[DEBUG]", "[%u]%s:%d(%s): " fmt "\n", \
                    gettid(), __FILE__, __LINE__, __FUNCTION__, ##args); \
        } \
    } while (0)

#define INFO(fmt, args...) \
    do \
    { \
        if (ring_log::ins()->get_level() >= INFO) \
        { \
            ring_log::ins()->try_append("[INFO]", "[%u]%s:%d(%s): " fmt "\n", \
                    gettid(), __FILE__, __LINE__, __FUNCTION__, ##args); \
        } \
    } while (0)

#define NORMAL(fmt, args...) \
    do \
    { \
        if (ring_log::ins()->get_level() >= INFO) \
        { \
            ring_log::ins()->try_append("[INFO]", "[%u]%s:%d(%s): " fmt "\n", \
                    gettid(), __FILE__, __LINE__, __FUNCTION__, ##args); \
        } \
    } while (0)

#define WARN(fmt, args...) \
    do \
    { \
        if (ring_log::ins()->get_level() >= WARN) \
        { \
            ring_log::ins()->try_append("[WARN]", "[%u]%s:%d(%s): " fmt "\n", \
                    gettid(), __FILE__, __LINE__, __FUNCTION__, ##args); \
        } \
    } while (0)

#define ERROR(fmt, args...) \
    do \
    { \
        if (ring_log::ins()->get_level() >= ERROR) \
        { \
            ring_log::ins()->try_append("[ERROR]", "[%u]%s:%d(%s): " fmt "\n", \
                gettid(), __FILE__, __LINE__, __FUNCTION__, ##args); \
        } \
    } while (0)

#define FATAL(fmt, args...) \
    do \
    { \
        ring_log::ins()->try_append("[FATAL]", "[%u]%s:%d(%s): " fmt "\n", \
            gettid(), __FILE__, __LINE__, __FUNCTION__, ##args); \
    } while (0)

#endif //OS_ASIC_LOG_H

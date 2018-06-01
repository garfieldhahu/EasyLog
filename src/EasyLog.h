#ifndef EASYLOG_EASYLOG_H
#define EASYLOG_EASYLOG_H

#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdarg.h>

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#define gettid() syscall(__NR_gettid) 

namespace easylog
{

enum LogLevel
{
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    LOG_LEVEL_NUM
};

const int kMessageLenMax = 8192;
// head eg:
// "20180525 08:01:45.342651 DEBUG "
const int kHeadLen = 31;
const int kTimeLen = 24;
const char* kTimeFormat = "%4d%2d%2d %2d:%2d:%2d.%6d";


class EasyLog
{
public:
    static void set_log_level(LogLevel level)
    {
        level_ = level;
        return;
    }

    static EasyLog* Instance()
    {
        pthread_once(&once_, EasyLog::init);
        return ins_;
    }

    static void init()
    {
        if(!ins_)
            ins_ = new EasyLog();
    }

private:
    //void format_head()
    //{
    //    snprintf(str_time_, kTimeLen, kTimeFormat, year_, month_, day_, hour_, minute_, second_, usecond_);
    //}

    //void format_head_usec()
    //{
    //    snprintf(str_time_ + 18, 6, "%6d", usecond_);
    //}

    EasyLog();

    void format_head();

private:
    char str_head_[kHeadLen] ;
    int last_second_;
    int buf_num_;
    struct timeval tv_;
    struct tm tm_;

    static LogLevel level_;
    static pthread_mutex_t mutex_;
    static pthread_cond_t cond_;
    static EasyLog* ins_;
    static pthread_once_t once_;
    static __thread pid_t tid_;
};



#define LOG_TRACE(fmt, args...) \
    do \
    { \
        if(EasyLog::Instance()->get_log_level() >= TRACE) \
            EasyLog::Instance()->append("%s:%s(%d) - " fmt "\n", \
                                        __FILE__, __FUNCTION__, __LINE__, ##args); \
    } \
    while(0) \

#define LOG_DEBUG(fmt, args...) \
    do \
    { \
        if(EasyLog::Instance()->get_log_level() >= DEBUG) \
            EasyLog::Instance()->append("%s:%s(%d) - " fmt "\n", \
                                        __FILE__, __FUNCTION__, __LINE__, ##args); \
    } \
    while(0) \


#define LOG_INFO(fmt, args...) \
    do \
    { \
        if(EasyLog::Instance()->get_log_level() >= INFO) \
            EasyLog::Instance()->append("%s:%s(%d) - " fmt "\n", \
                                        __FILE__, __FUNCTION__, __LINE__, ##args); \
    } \
    while(0) \
} //namespace easylog 


#define LOG_WARN(fmt, args...) \
    do \
    { \
        if(EasyLog::Instance()->get_log_level() >= WARN) \
            EasyLog::Instance()->append("%s:%s(%d) - " fmt "\n", \
                                        __FILE__, __FUNCTION__, __LINE__, ##args); \
    } \
    while(0) \

#define LOG_ERROR(fmt, args...) \
    do \
    { \
        if(EasyLog::Instance()->get_log_level() >= ERROR) \
            EasyLog::Instance()->append("%s:%s(%d) - " fmt "\n", \
                                        __FILE__, __FUNCTION__, __LINE__, ##args); \
    } \
    while(0) \

#define LOG_FATAL(fmt, args...) \
    do \
    { \
        if(EasyLog::Instance()->get_log_level() >= FATAL) \
            EasyLog::Instance()->append("%s:%s(%d) - " fmt "\n", \
                                        __FILE__, __FUNCTION__, __LINE__, ##args); \
    } \
    while(0) \


#endif // EASH_EASYLOG_H

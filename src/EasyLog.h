#ifndef EASYLOG_EASYLOG_H
#define EASYLOG_EASYLOG_H

#include <LogBuffer.h>
#include <pthread.h>
#include <unistd.h>

namespace easylog
{


class EasyLog
{
public:
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

    static set_log_level(LogLevel log_level)
    {
        log_level_ = log_level;
    }

    staitc LogLevel get_log_level() const
    {
        return log_level_;
    }

    void Log(const char* file, const char* function, int line, const char* fmt, ...);

private:
    EasyLog();
    void format_head();
    void init_tid()     {tid_ = gettid();}

private:
    int buf_num_;
    LogBuffer* head_;
    LogBuffer* tail_;
    char log_path_[512];
    char log_name_[128];
    char str_[kMessageMsgLenMax];
    time_t last_second_;
    struct timeval tv_;
    struct tm tm_;

    static LogLevel log_level_;
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
            EasyLog::Instance()->Log( \
                                        __FILE__, __FUNCTION__, __LINE__, fmt, ##args); \
    } \
    while(0) \

#define LOG_DEBUG(fmt, args...) \
    do \
    { \
        if(EasyLog::Instance()->get_log_level() >= DEBUG) \
            EasyLog::Instance()->Log( \
                                        __FILE__, __FUNCTION__, __LINE__, fmt, ##args); \
    } \
    while(0) \


#define LOG_INFO(fmt, args...) \
    do \
    { \
        if(EasyLog::Instance()->get_log_level() >= INFO) \
            EasyLog::Instance()->Log( \
                                        __FILE__, __FUNCTION__, __LINE__, fmt, ##args); \
    } \
    while(0) \

#define LOG_WARN(fmt, args...) \
    do \
    { \
        if(EasyLog::Instance()->get_log_level() >= WARN) \
            EasyLog::Instance()->Log( \
                                        __FILE__, __FUNCTION__, __LINE__, fmt, ##args); \
    } \
    while(0) \

#define LOG_ERROR(fmt, args...) \
    do \
    { \
        if(EasyLog::Instance()->get_log_level() >= ERROR) \
            EasyLog::Instance()->Log( \
                                        __FILE__, __FUNCTION__, __LINE__, fmt, ##args); \
    } \
    while(0) \

#define LOG_FATAL(fmt, args...) \
    do \
    { \
        if(EasyLog::Instance()->get_log_level() >= FATAL) \
            EasyLog::Instance()->Log( \
                                        __FILE__, __FUNCTION__, __LINE__, fmt, ##args); \
    } \
    while(0) \

} //namespace easylog 

#endif // EASH_EASYLOG_H

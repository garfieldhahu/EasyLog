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

    static LogLevel get_log_level() const
    {
        return log_level_;
    }

    void Log(const char* file, const char* function, int line, const char* fmt, ...);
    void start();

private:
    EasyLog();
    void init();
    void init_log_file(const char* log_path, const char* log_name);
    void dump_background();//开启后台线程，异步dump日志
    void roll_log();
    void open_file()
    {
        strncat(log_full_name_, log_path_, sizeof(log_full_name_));
        strncat(log_full_name_, "/", sizeof(log_full_name_));
        strncat(log_full_name_, log_name_, sizeof(log_full_name_));
        log_fd_ = open(log_full_name_, O_CREAT | O_APPEND | O_RDWR, 0640);
    }
    void roll_log(struct tm* s_tm)
    {
        close(log_fd_);
        log_roll_hour_ = s_tm.tm_hour;
        sprintf(log_name_+3, "%d%d%d%d", s_tm.tm_year + 1900, s_tm.tm_mon + 1, s_tm.tm_mday, s_tm.tm_hour);
        open_file();
    }
    void init_tid()     {tid_ = gettid();}


private:
    int buf_num_;
    int buf_size_;
    LogBuffer* head_;
    LogBuffer* tail_;
    char log_full_name_[kLogPathLenMax];
    char log_path_[kLogPathLenMax];
    char log_name_[kLogNameLenMax];
    char str_[kMessageMsgLenMax];
    int log_fd_;
    int log_roll_hour_;
    time_t last_second_;

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

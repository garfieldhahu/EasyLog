#ifndef EASYLOG_EASYLOG_H
#define EASYLOG_EASYLOG_H

#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include "Util.h"
#include "LogBuffer.h"

namespace easylog
{


class EasyLog
{
public:
    void stop();
    void log(LogLevel level, const char* file, const char* function, int line, const char* fmt, ...);

    static EasyLog* Instance()
    {
        pthread_once(&once_, EasyLog::new_instance);
        return ins_;
    }
    static void new_instance()
    {
        if(!ins_)
        {
            ins_ = new EasyLog();
            ins_->init();
            ins_->start();
        }
    }
    static void set_log_level(LogLevel log_level)    {log_level_ = log_level;}
    static void set_log_path(const char* log_path)   {strncpy(log_path_, log_path, sizeof(log_path_));};
    static void set_log_name(const char* log_name)   {strncpy(log_name_, log_name, sizeof(log_name_));};
    static void set_buf_num(int buf_num)             {buf_num_ = buf_num;}
    static void set_buf_size(int buf_size)           {buf_size_ = buf_size;}
    static LogLevel get_log_level()                  {return log_level_;}
private:
    void init();
    void start();
    void init_log_file();
    void roll_log();
    void open_file();
    void dump_tail();
    void dump_all();
    void dump_background();// start a background thread to dump logs asynchronously
    void dump_before_exit();

    EasyLog():running_(false),head_(NULL),tail_(NULL),log_roll_hour_(25)
    {
    }
    void init_tid()         {tid_ = gettid();}
    int get_tid() const     {return tid_;}
    static void* background_function(void* arg)
    {
        EasyLog::Instance()->dump_background(); 
    }
    static void signal_exit(int sig)
    {
        EasyLog::Instance()->dump_before_exit();
    }


private:
    LogBuffer* head_;
    LogBuffer* tail_;
    bool running_;
    pthread_t dump_thread_tid_;
    int log_fd_;
    int log_roll_hour_;

    //static char log_path_[kLogPathLenMax];
    //static char log_name_[kLogNameLenMax];
    static char log_path_[512];
    static char log_name_[128];
    static char log_full_name_[512];
    static int buf_num_;
    static int buf_size_;
    static LogLevel log_level_;
    static pthread_mutex_t mutex_;
    static pthread_cond_t cond_;
    static EasyLog* ins_;
    static pthread_once_t once_;
    static __thread pid_t tid_;
    static __thread char* str_;
    static __thread time_t last_second_;
};


#define LOG_TRACE(fmt, args...) \
    do \
    { \
        if(easylog::EasyLog::Instance()->get_log_level() <= easylog::TRACE) \
            easylog::EasyLog::Instance()->log( \
                                        easylog::TRACE, __FILE__, __FUNCTION__, __LINE__, fmt, ##args); \
    } \
    while(0)

#define LOG_DEBUG(fmt, args...) \
    do \
    { \
        if(easylog::EasyLog::Instance()->get_log_level() <= easylog::DEBUG) \
            easylog::EasyLog::Instance()->log( \
                                        easylog::DEBUG, __FILE__, __FUNCTION__, __LINE__, fmt, ##args); \
    } \
    while(0)


#define LOG_INFO(fmt, args...) \
    do \
    { \
        if(easylog::EasyLog::Instance()->get_log_level() <= easylog::INFO) \
            easylog::EasyLog::Instance()->log( \
                                        easylog::INFO, __FILE__, __FUNCTION__, __LINE__, fmt, ##args); \
    } \
    while(0)

#define LOG_WARN(fmt, args...) \
    do \
    { \
        if(easylog::EasyLog::Instance()->get_log_level() <= easylog::WARN) \
            easylog::EasyLog::Instance()->log( \
                                        easylog::WARN, __FILE__, __FUNCTION__, __LINE__, fmt, ##args); \
    } \
    while(0)

#define LOG_ERROR(fmt, args...) \
    do \
    { \
        if(easylog::EasyLog::Instance()->get_log_level() <= easylog::ERROR) \
            easylog::EasyLog::Instance()->log( \
                                        easylog::ERROR, __FILE__, __FUNCTION__, __LINE__, fmt, ##args); \
    } \
    while(0)

#define LOG_FATAL(fmt, args...) \
    do \
    { \
        if(easylog::EasyLog::Instance()->get_log_level() <= easylog::FATAL) \
            easylog::EasyLog::Instance()->log( \
                                        easylog::FATAL, __FILE__, __FUNCTION__, __LINE__, fmt, ##args); \
    } \
    while(0)

#define LOG_STOP() \
    do \
    { \
        easylog::EasyLog::Instance()->stop(); \
    } \
    while(0)

} //namespace easylog 

#endif // EASH_EASYLOG_H

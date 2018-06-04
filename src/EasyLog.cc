#include <EasyLog.h>

namespace easylog
{

LogLevel EasyLog::log_level_ = INFO;
pthread_mutex_t EasyLog::mutex_ = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t EasyLog::cond_ = PTHREAD_COND_INITIALIZER;
EasyLog* EasyLog::ins_ = NULL;
pthread_once_t EasyLog::once_ = PTHREAD_ONCE_INIT;
__thread pid_t EasyLog::tid_ = 0;

void LogBuffer::format_head()
{
    gettimeofday(&tv_, NULL);
    time_t second = tv_.tv_sec;
    int usecond = tv_.tv_usec; 
    if(likely(second = last_second_))
    {
        snprintf(str_ + 24, kUtimeLen, "%6d", usecond);
    }
    else
    {
        last_second_ = second;
        localtime_r(&last_second_, &tm_);
        int year = tm_.tm_year + 1900;
        int month = tm_.tm_mon + 1;
        int day = tm_.tm_mday;
        int hour = tm_.tm_hour;
        int minute = tm_.tm_min;
        int second = tm_.tm_sec;
        snprintf(str_ + 6, kTimeLen, kTimeFormat, year, month, day, hour, minute, second, usecond);
    }
    return;
}

void EasyLog::Log(const char* file, const char* function, int line, const char* fmt, ...)
{
    //todo: Buffer长度充足时，考虑可以直接在Buffer上组装，减少memcpy
    // if(likely(head_->avail_size() > kMessageMsgLenMax))
    // {
    //   **  format_head()无法解决
    //     int pre_len = sprintf(head_->current_pos(), "%s:%s(%d) - ", file, function, line);
    //     int remain_len = kMessageBodyLenMax - pre_len;
    //     va_list args;
    //     va_start(args, fmt);
    //     int len_to_write = vsnprintf(head_->current_pos() + kHeadLen + pre_len, remain_len, fmt, args);
    //     va_end(args);
    //     //单条日志过长时截断
    //     if(remain_len == len_to_write)
    //         str_[kMessageMsgLenMax - 2] = '\n';
    // }

    format_head();
    int pre_len = sprintf(str_ + kHeadLen, "%s:%s(%d) - ", file, function, line);
    int base_len = kHeadLen + pre_len;
    int remain_len = kMessageMsgLenMax - base_len;
    va_list args;
    va_start(args, fmt);
    int body_len = vsnprintf(str_ + kHeadLen + pre_len, remain_len, fmt, args);
    va_end(args);
    //单条日志过长时截断
    if(remain_len == body_len)
        str_[kMessageMsgLenMax - 1] = '\n';

    int log_len = base_len + body_len;
    if(unlikely(log_len > head_->avail_size())
    {
        //todo: head_ may not have enough space
        //...
    }
    else
    {
        head_->append(str_, body_len + remain_len);
    }
    
    return;
}



}// namespace easylog

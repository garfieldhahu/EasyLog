#include <EasyLog.h>

namespace easylog
{

LogLevel EasyLog::log_level_ = INFO;
pthread_mutex_t EasyLog::mutex_ = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t EasyLog::cond_ = PTHREAD_COND_INITIALIZER;
EasyLog* EasyLog::ins_ = NULL;
pthread_once_t EasyLog::once_ = PTHREAD_ONCE_INIT;
__thread pid_t EasyLog::tid_ = 0;

EasyLog::EasyLog(const char* log_path, const char* log_name, LogLevel log_level, int buf_num, int buf_size)
    :buff_num_(buff_num), buff_size_(buff_size)
{
    init_log_file(log_path, log_name);
    if(log_level > FATAL)
        log_level = FATAL;
    log_level_ = log_level;

    head_ = new LogBuffer(buff_size_);
    if(!head_)
    {
        fprintf(stderr, "no space to allocate LogBuffer\n");
        exit(1);
    }
    head_->set_log_ins(this);
    LogBuffer* cur;
    LogBuffer* pre = head_;
    for(int i = 1; i < buf_num_; ++i)
    {
        cur = new LogBuffer(buff_size_);
        if(!cur)
        {
            fprintf(stderr, "no space to allocate LogBuffer\n");
            exit(1);
        }
        cur->set_log_ins(this);
        cur->set_pre(pre);
        pre->set_next(cur);
        pre = cur;
    }
    pre->set_next(head_);
    head_->set_pre(pre);
    tail_ = head_;
    init_log_file();
}

void EasyLog::init_log_file(const char* log_path, const char* log_name)
{
    struct timeval s_tv;
    gettimeofday(&s_tv, NULL);
    time_t second = s_tv.tv_sec;
    struct tm s_tm;
    localtime_r(&second, &s_tm);
    log_roll_hour_ = s_tm.tm_hour;
    if(NULL == log_name)
    {
        char buf[kLogNameLenMax];
        int len = readlink("/proc/self/exe", buf, sizeof(buf));
        buf[len] = '\0';
        char* index = strrchr(buf, '\/');
        if(unlikely(NULL == index))
            sprintf(log_name_, "log%d%d%d%d%s.txt", s_tm.tm_year + 1900, s_tm.tm_mon + 1, s_tm.tm_mday, s_tm.tm_hour, buf);
        else
            sprintf(log_name_, "log%d%d%d%d%s.txt", s_tm.tm_year + 1900, s_tm.tm_mon + 1, s_tm.tm_mday, s_tm.tm_hour, index + 1);
    }
    else
    {
        sprintf(log_name_, "log%d%d%d%d%s.txt", s_tm.tm_year + 1900, s_tm.tm_mon + 1, s_tm.tm_mday, s_tm.tm_hour, log_name);
    }
    if(NULL == log_path)
        log_path_ = "/tmp/EasyLog";
    else
        strncpy(log_path_, log_path, sizeof(log_path_));
    mkdir(log_path_, 0777);
    if(access(log_path_, F_OK | W_OK) == -1)
    {
        fprintf(stderr, "EasyLog ins create fail, log path: %s, error: %s\n", log_path_, strerror(errno));
        exit(1);
    }
    open_file();
}


void EasyLog::Log(const char* file, const char* function, int line, const char* fmt, ...)
{
    char log_format_buffer[kMessageMsgLenMax];
    struct timeval s_tv;
    gettimeofday(&s_tv, NULL);
    time_t second = s_tv.tv_sec;
    int usecond = s_tv.tv_usec;
    struct tm s_tm;
    if(likely(second == last_second_))
    {
        snprintf(log_format_buffer + 24, kUtimeLen, "%6d", usecond);
    }
    else
    {
        last_second_ = second;
        localtime_r(&last_second_, &s_tm);
        snprintf(log_format_buffer + 6, kTimeLen, kTimeFormat, s_tm.tm_year + 1900, s_tm.tm_mon + 1, s_tm.tm_mday, s_tm.tm_hour, s_tm.tm_min, s_tm.tm_sec, usecond);
        if(unlikely(s_tm.tm_hour != log_roll_hour_))
            roll_log(&s_tm);
    }

    int pre_len = sprintf(log_format_buffer + kHeadLen, "%s:%s(%d) - ", file, function, line);
    int base_len = kHeadLen + pre_len;
    int remain_len = kMessageMsgLenMax - base_len;
    va_list args;
    va_start(args, fmt);
    int body_len = vsnprintf(log_format_buffer + kHeadLen + pre_len, remain_len, fmt, args);
    va_end(args);
    //单条日志过长时截断
    if(remain_len == body_len)
        log_format_buffer[kMessageMsgLenMax - 1] = '\n';
    int log_len = base_len + body_len;
    bool raise_signal = false;
    pthread_mutex_lock(&mutex_);
    if(unlikely(log_len > head_->avail_size())
    {
        //todo: head_ may not have enough space
        //...
    }
    else
    {
        head_->append(log_format_buffer, body_len + remain_len);
    }
    pthread_mutex_unlock(&mutex_);
    if(raise_signal)
        pthread_cond_signal(&cond_);
}



}// namespace easylog

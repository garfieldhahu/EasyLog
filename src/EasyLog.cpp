#include <stdarg.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h> 
#include "EasyLog.h"

namespace easylog
{

LogLevel EasyLog::log_level_ = INFO;
pthread_mutex_t EasyLog::mutex_ = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t EasyLog::cond_ = PTHREAD_COND_INITIALIZER;
EasyLog* EasyLog::ins_ = NULL;
pthread_once_t EasyLog::once_ = PTHREAD_ONCE_INIT;
__thread pid_t EasyLog::tid_ = 0;
__thread char* EasyLog::str_ = NULL;
__thread time_t EasyLog::last_second_ = 0;
//char EasyLog::log_path_[kLogPathLenMax] = "/tmp/EasyLog";
//char EasyLog::log_name_[kLogNameLenMax];
char EasyLog::log_path_[512] = "/tmp/EasyLog";
char EasyLog::log_name_[128];
char EasyLog::log_full_name_[512];
int EasyLog::buf_num_ = kBufferNum;
int EasyLog::buf_size_ = kBufferLen;


void EasyLog::init()
{
    head_ = new LogBuffer(buf_size_);
    if(!head_)
    {
        fprintf(stderr, "no space to allocate LogBuffer\n");
        exit(1);
    }
    LogBuffer* cur;
    LogBuffer* pre = head_;
    for(int i = 1; i < buf_num_; ++i)
    {
        cur = new LogBuffer(buf_size_);
        if(!cur)
        {
            fprintf(stderr, "no space to allocate LogBuffer\n");
            exit(1);
        }
        cur->set_pre(pre);
        pre->set_next(cur);
        pre = cur;
    }
    pre->set_next(head_);
    head_->set_pre(pre);
    tail_ = head_;
    init_log_file();
    if(signal(SIGSEGV, signal_exit))
    {
        fprintf(stderr, "redirect SIGSEGV signal failed\n");
        exit(1);
    }
}

void EasyLog::start()
{
    if(pthread_create(&dump_thread_tid_, NULL, background_function, NULL))
    {
        fprintf(stderr, "EasyLog backgroud dmup thrad create failed\n");
        exit(1);
    }
    running_ = true;
}


void EasyLog::dump_background()
{
    while(running_)
    {
        pthread_mutex_lock(&mutex_);
        if(tail_->is_empty(THIS_HOUR) && tail_->is_empty(NEXT_HOUR))
        {
            struct timespec abstime;
            struct timeval now;
            gettimeofday(&now, NULL);
            abstime.tv_sec = now.tv_sec;
            abstime.tv_nsec = now.tv_usec * 1000;//nanoseconds
            abstime.tv_sec += kDumpLoopMaxTime;
            pthread_cond_timedwait(&cond_, &mutex_, &abstime);
        }

        if(tail_->get_buffer_stat() == FREE)
        {
            //assert(tail_ == head_);
            head_ = head_->get_next();
        }
        //pthread_mutex_unlock(&mutex_);
        tail_->dump(log_fd_, THIS_HOUR);
        if(unlikely(!tail_->is_empty(NEXT_HOUR)))
        {
            roll_log();
            tail_->dump(log_fd_, NEXT_HOUR);
        }
        //pthread_mutex_lock(&mutex_);
        tail_->clear();
        tail_ = tail_->get_next();
        pthread_mutex_unlock(&mutex_);

    }   
}

void EasyLog::stop()
{
    running_ = false;
    pthread_mutex_lock(&mutex_);
    dump_all();
    pthread_mutex_unlock(&mutex_);
    pthread_join(dump_thread_tid_, NULL);
}

void EasyLog::roll_log()
{
    struct timeval s_tv;
    gettimeofday(&s_tv, NULL);
    time_t second = s_tv.tv_sec;
    struct tm s_tm;
    localtime_r(&second, &s_tm);
    log_roll_hour_ = s_tm.tm_hour;
    char file_time[10];
    sprintf(file_time, "%04d%02d%02d%02d", s_tm.tm_year + 1900, s_tm.tm_mon + 1, s_tm.tm_mday, s_tm.tm_hour);
    memcpy(log_name_ + 3, file_time, 10);
    close(log_fd_);
    open_file();
}

void EasyLog::dump_tail()
{
    tail_->dump(log_fd_, THIS_HOUR);
    if(unlikely(!tail_->is_empty(NEXT_HOUR)))
    {
        roll_log();
        tail_->dump(log_fd_, NEXT_HOUR);
        tail_ = tail_->get_next();
    }
}

void EasyLog::dump_all()
{
    while(tail_ != head_)
        dump_tail();
}

void EasyLog::open_file()
{
    memset(log_full_name_, '\0', 512);
    strncat(log_full_name_, log_path_, sizeof(log_full_name_));
    strncat(log_full_name_, "/", sizeof(log_full_name_));
    strncat(log_full_name_, log_name_, sizeof(log_full_name_));
    log_fd_ = open(log_full_name_, O_CREAT | O_APPEND | O_RDWR, 0640);
    if(-1 == log_fd_)
    {
        fprintf(stderr, "EasyLog open fail, file name: %s, error: %s\n", log_full_name_, strerror(errno));
        exit(1);
    }
}


void EasyLog::init_log_file()
{
    mkdir(log_path_, 0777);
    if(access(log_path_, F_OK | W_OK) == -1)
    {
        fprintf(stderr, "EasyLog ins create fail, log path: %s, error: %s\n", log_path_, strerror(errno));
        exit(1);
    }
    struct timeval s_tv;
    gettimeofday(&s_tv, NULL);
    time_t second = s_tv.tv_sec;
    struct tm s_tm;
    localtime_r(&second, &s_tm);
    log_roll_hour_ = s_tm.tm_hour;
    char buf[kLogNameLenMax];
    if(!strlen(log_name_))
    {
        int len = readlink("/proc/self/exe", buf, sizeof(buf));
        buf[len] = '\0';
        char* index = strrchr(buf, '/');
        if(unlikely(NULL == index))
            sprintf(log_name_, "log%04d%02d%02d%02d-%s.txt", s_tm.tm_year + 1900, s_tm.tm_mon + 1, s_tm.tm_mday, s_tm.tm_hour, buf);
        else
            sprintf(log_name_, "log%04d%02d%02d%02d-%s.txt", s_tm.tm_year + 1900, s_tm.tm_mon + 1, s_tm.tm_mday, s_tm.tm_hour, index + 1);
    }
    else
    {
        strncpy(buf, log_name_, kLogNameLenMax);
        sprintf(log_name_, "log%04d%02d%02d%02d-%s.txt", s_tm.tm_year + 1900, s_tm.tm_mon + 1, s_tm.tm_mday, s_tm.tm_hour, buf);
    }
    open_file();
}


void EasyLog::dump_before_exit()
{
    LOG_FATAL("program got SIGSEGV, will exit after log dump\n");
    stop();
    exit(1);
}
void EasyLog::log(LogLevel level,const char* file, const char* function, int line, const char* fmt, ...)
{
    if(unlikely(0 == get_tid()))
        init_tid();
    if(unlikely(NULL == str_))
        str_ = new char[kMessageMsgLenMax];
    strncpy(str_, LevelMap[level], 6);
    struct timeval s_tv;
    gettimeofday(&s_tv, NULL);
    time_t second = s_tv.tv_sec;
    int usecond = s_tv.tv_usec;
    struct tm s_tm;
    if(likely(second == last_second_))
    {
        sprintf(str_ + 24, "%06d", usecond);
    }
    else
    {
        last_second_ = second;
        localtime_r(&last_second_, &s_tm);
        snprintf(str_ + 6, kTimeLen, kTimeFormat, s_tm.tm_year + 1900, s_tm.tm_mon + 1, s_tm.tm_mday, s_tm.tm_hour, s_tm.tm_min, s_tm.tm_sec, usecond);
    }

    int pre_len = sprintf(str_ + kHeadLen, " %ld %s:%s(%d) - ", get_tid(), file, function, line);
    int base_len = kHeadLen + pre_len;
    int remain_len = kMessageMsgLenMax - base_len;
    va_list args;
    va_start(args, fmt);
    int body_len = vsnprintf(str_ + kHeadLen + pre_len, remain_len, fmt, args);
    va_end(args);
    // truncate if the single log is too long
    if(remain_len == body_len)
        str_[kMessageMsgLenMax - 1] = '\n';
    int log_len = base_len + body_len;
    bool raise_signal = false;
    int  buf_index = THIS_HOUR;
    pthread_mutex_lock(&mutex_);
    //if it comes into another hour ,roll the log
    if(unlikely(s_tm.tm_hour != log_roll_hour_))
        buf_index = NEXT_HOUR;
    //head_ may not have enough space
    if(unlikely(log_len > head_->avail_size(buf_index)))
    {
        raise_signal = true;
        head_->set_buffer_stat(FULL);
        if(unlikely(head_->get_next() == tail_))
        {
            LogBuffer* tmp = new LogBuffer(head_, tail_, buf_size_);
            ++buf_num_;
            head_ = tmp;
            LOG_WARN("EasyLog new another buffer, current buffer num:%d, total size:%ldMB\n", buf_num_, (kBufferLen >> 20) * buf_num_ * BUFFER_INDEX_NUM);
        }
        else
        {
            head_ = head_->get_next();
        }
    }
    head_->append(str_, body_len + base_len, buf_index);
    pthread_mutex_unlock(&mutex_);
    if(raise_signal)
        pthread_cond_signal(&cond_);
}



}// namespace easylog

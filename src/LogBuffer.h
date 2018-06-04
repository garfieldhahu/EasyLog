#ifndef EASYLOG_LOGBUFFER_H
#define EASYLOG_LOGBUFFER_H

#include <unistd.h>

namespace easylog
{


class LogBuffer
{
public:
    // friend class EasyLog;
    enum BufferStat
    {
        CLEAN,
        DIRTY,
    };
    int avail_size()                {return size_ - used_len_;}
    char* current_pos()             {return &buff_[used_len_];}
    bool is_empty()                 {return used_len_ == 0;}
    void dump(int fd)               {::write(fd, buff_, used_len_);}
    LogBuffer* get_pre()            {return pre_;}
    void set_pre(LogBuffer* pre)    {pre_ = pre;}
    LogBuffer* get_next()           {return next_;}
    void set_next(LogBuffer* next)  {next_ = next;}
    void append(const char* str, int len)
    {
        memcpy(buff_ + used_len_, str, len);
        used_len_ += len;
        return;
    }

    LogBuffer(LogBuffer* pre, LogBuffer* next, int size);
    LogBuffer();
    
private:
    char* buff_;
    int size_;
    int used_len_;
    BufferStat buffer_stat_;
    LogBuffer* pre_;
    LogBuffer* next_;
};



} //namespace easylog


#endif // EASYLOG_LOGBUFFER_H

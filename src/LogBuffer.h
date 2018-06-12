#ifndef EASYLOG_LOGBUFFER_H
#define EASYLOG_LOGBUFFER_H

#include <unistd.h>
#include "Util.h"

namespace easylog
{


class LogBuffer
{
public:
    int avail_size(int index)               {return size_[index] - used_len_[index];}
    bool is_empty(int index)                {return used_len_[index] == 0;}
    LogBuffer* get_pre()                    {return pre_;}
    void set_pre(LogBuffer* pre)            {pre_ = pre;}
    LogBuffer* get_next()                   {return next_;}
    void set_next(LogBuffer* next)          {next_ = next;}
    BufferStat get_buffer_stat()            {return buffer_stat_;}
    void set_buffer_stat(BufferStat stat)   {buffer_stat_ = stat;}
    void clear()                            {buffer_stat_ = FREE;used_len_[THIS_HOUR] = 0;used_len_[NEXT_HOUR] = 0;}
    void dump(int fd, int index)            {::write(fd, buff_[index], used_len_[index]);} 
    void append(const char* str, int len, int index)
    {
        memcpy(buff_[index] + used_len_[index], str, len);
        used_len_[index] += len;
        return;
    }
    LogBuffer(LogBuffer* pre, LogBuffer* next, int size);
    LogBuffer(int size);
    
private:
    char** buff_;
    int size_[BUFFER_INDEX_NUM];
    int used_len_[BUFFER_INDEX_NUM];
    BufferStat buffer_stat_;
    LogBuffer* pre_;
    LogBuffer* next_;
};



} //namespace easylog


#endif // EASYLOG_LOGBUFFER_H

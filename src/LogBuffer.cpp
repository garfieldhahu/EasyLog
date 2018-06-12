#include "LogBuffer.h"


namespace easylog
{

LogBuffer::LogBuffer(LogBuffer* pre, LogBuffer* next, int size)
    :pre_(pre), next_(next)
{
    size_[THIS_HOUR] = size;
    size_[NEXT_HOUR] = size; 
    buff_ = new char*[BUFFER_INDEX_NUM];
    buff_[THIS_HOUR] = new char[size_[THIS_HOUR]];
    buff_[NEXT_HOUR] = new char[size_[NEXT_HOUR]];
    buffer_stat_ = FREE;
}

LogBuffer::LogBuffer(int size)
    :pre_(NULL), next_(NULL)
{
    size_[THIS_HOUR] = size;
    size_[NEXT_HOUR] = size; 
    buff_ = new char*[BUFFER_INDEX_NUM];
    buff_[THIS_HOUR] = new char[size_[THIS_HOUR]];
    buff_[NEXT_HOUR] = new char[size_[NEXT_HOUR]];
    buffer_stat_ = FREE;
}


} // namespace easylog

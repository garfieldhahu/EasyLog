#include <LogBuffer.h>
#include <util.h>


namespace easylog
{


LogBuffer::LogBuffer(LogBuffer* pre, LogBuffer* next, int size)
    :pre_(pre), next_(next), size_(size)
{
    buff_ = new char[size_];
    buff_stat_ = EMPTY;
}

LogBuffer::LogBuffer(int size)
    :pre_(NULL), next_(NULL), size_(size)
{
    buff_ = new char[size_];
    buff_stat_ = EMPTY;
}

LogBuffer::LogBuffer()
    :pre_(NULL), next_(NULL), size_(kBufferLen)
{
    buff_ = new char[size_];
    buff_stat_ = EMPTY;
}


} // namespace easylog

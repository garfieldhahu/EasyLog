#ifndef EASYLOG_UTIL_H
#define EASYLOG_UTIL_H

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

namespace easylog
{

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#define gettid() syscall(__NR_gettid)

enum LogLevel
{
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    LOG_LEVEL_NUM,
};


enum BufferStat
{
    FREE,
    FULL,
};

enum BufferIndex
{
    THIS_HOUR,
    NEXT_HOUR,
    BUFFER_INDEX_NUM,
};

extern const char* LevelMap[LOG_LEVEL_NUM]; 
extern const int kHeadLen;
extern const char* kTimeFormat;
extern const int kTimeLen;
extern const int kMessageMsgLenMax;
extern const int kMessageBodyLenMax; // kMessageMsgLenMax - kHeadLen;
extern const int kBufferNum;
extern const int kBufferLen;
extern const int kLogPathLenMax;
extern const int kLogNameLenMax;
extern const int kDumpLoopMaxTime;


} //namespace easylog

#endif // EASYLOG_UTIL_H

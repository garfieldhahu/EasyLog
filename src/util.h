#ifndef EASYLOG_UTIL_H
#define EASYLOG_UTIL_H

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

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
    EMPTY,
    FULL,
};

const char* LevelMap[LOG_LEVEL_NUM] = 
{
    "TRACE ",
    "DEBUG ",
    "INFO  ",
    "WARN  ",
    "ERROR ",
    "FATAL ",
};

// head eg:
// "DEBUG 20180525 08:01:45.342651 "
const int kHeadLen = 31;
const char* kTimeFormat = "%4d%2d%2d %2d:%2d:%2d.%6d ";
const int kTimeLen = 25;
const int kUtimeLen = 6;

const int kMessageMsgLenMax = 4* 1024;
const int kMessageBodyLenMax = 4 * 1024 - 31; // kMessageMsgLenMax - kHeadLen;
const int kBufferLen = 4 * 1024 * 1024;

const int kLogPathLenMax = 512;
const int kLogNameLenMax = 128;




} //namespace easylog

#endif // EASYLOG_UTIL_H

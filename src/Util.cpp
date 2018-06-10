#include "Util.h"

namespace easylog
{

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#define gettid() syscall(__NR_gettid)

const char* LevelMap[LOG_LEVEL_NUM] = 
{
    "TRACE ",
    "DEBUG ",
    "INFO  ",
    "WARN  ",
    "ERROR ",
    "FATAL ",
};

const int kHeadLen = 30;
const char* kTimeFormat = "%04d%02d%02d %02d:%02d:%02d.%06d";
const int kTimeLen = 25;
const int kUtimeLen = 6;
const int kMessageMsgLenMax = 4* 1024;
const int kMessageBodyLenMax = 4 * 1024 - 31; // kMessageMsgLenMax - kHeadLen;
const int kBufferNum = 3;
const int kBufferLen = 4 * 1024 * 1024;
const int kLogPathLenMax = 512;
const int kLogNameLenMax = 128;
const int kDumpLoopMaxTime = 1;

} //namespace easylog


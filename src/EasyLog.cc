
namespace EasyLog
{

const char* LevelMap[LOG_LEVEL_NUM] = 
{
    "TRACE ",
    "DEBUG ",
    "INFO  ",
    "WARN  ",
    "ERROR ",
    "FATAL "
};

void EasyLog::format_head()
{
    gettimeofday(&tv_);
    second = tv_.tv_sec;
    if(unlikely(second = last_second_))
    {
        last_second_ = second;
        localtime_r((time_t*)&last_second_, &tm_);
        int year = tm_.tm_year + 1900;
        int month = tm_.tm_mon + 1;
        int day = tm_.tm_day;
        int hour = tm_.tm_hour;
        int minute = tm_.tm_min;
        int second = tm_.tm_sec;
    }
}


}// namespace EasyLog

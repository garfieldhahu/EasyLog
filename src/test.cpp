#include "EasyLog.h"

const int loop_num = 1000000;

void* child1(void* arg)
{
    for(int i = 0; i < loop_num; ++i)
    {
        LOG_DEBUG("child1 loop %d\n", i);
        LOG_INFO("in child1:%ld%s\n", 1357986, "abcdefghijklmn");
    }
}

void* child2(void* arg)
{
    for(int i = 0; i < loop_num; ++i)
    {
        LOG_DEBUG("in child2:%s%d\n", "HELLO COLEWANG", 88888);
        LOG_INFO("in child2:%ld%s\n", 2468097, "opqrstuvwxyz");
    }
}


int main()
{
    struct timespec ts1;
    struct timespec ts2;
    clock_gettime(CLOCK_MONOTONIC, &ts1);
    pthread_t t1, t2;
    pthread_create(&t1, NULL, &child1, NULL);
    pthread_create(&t2, NULL, &child2, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    LOG_EXIT();
    clock_gettime(CLOCK_MONOTONIC, &ts2);
    uint64_t nsec = ts2.tv_nsec<ts1.tv_nsec? 1000000000+ts2.tv_nsec-ts1.tv_nsec: ts2.tv_nsec - ts1.tv_nsec;
    uint64_t sec = ts2.tv_nsec<ts1.tv_nsec? ts2.tv_sec-1-ts1.tv_sec : ts2.tv_sec - ts1.tv_sec; 
    double consume = sec + nsec / 1000000000;
    double average = loop_num * 4 / consume;
    printf("consume time: %lfs, %ld.%09lds\n", consume, sec, nsec);
    printf("total log num:%d\n", loop_num * 4);
    printf("average %.lfw/s\n", average);
    return 0;

}

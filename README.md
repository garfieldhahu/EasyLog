# EasyLog
EasyLog是一个轻量高效的多线程异步日志库，每个工作线程输出的日志会非阻塞地写入提前申请好的Buffer中并立即返回，库工具会自动启动另一后台线程用文件追加写的方式将日志内容Dump磁盘的操作，提升落盘效率。日志组织方式参考如下样例：
> INFO  20180610 21:52:08.425329 test.cpp:child1(10) - in child1:1357986abcdefghijklmn

日志共分为共6个级别，可以在调用日志库时设置程序的程序日志输出级别：
1. TRACE
2. DEBUG
3. INFO
4. WARN
5. ERROR
6. FATAL

日志会以小时为单位自动rolling， 默认输出路径为/tmp/EasyLog/，默认以程序可执行文件名为名称，类似"log2018061021test.txt"。当程序发生SegmentFault时，日志库可以保证将Buffer内数据Dump到磁盘，关键日志信息不会丢失。经测试，日志输出效率在100w～150w/s左右，完全可以满足大多数后台服务的日志输出功能。

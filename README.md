# EasyLog
EasyLog是一个轻量高效的多线程异步日志库，每个工作线程输出的日志会非阻塞地写入提前申请好的Buffer中并立即返回，库工具会自动启动另一后台线程用文件追加写的方式将日志内容Dump磁盘的操作，提升落盘效率。日志组织方式参考如下样例：
> 日志级别 日期 时间(微妙级别) tid 文件名:函数名(代码行数) - 日志正文
```C++
INFO  20180611 20:48:09.077633 8070 test.cpp:child1(10) - in child1:1357986abcdefghijklmn
DEBUG 20180611 20:48:09.077634 8070 test.cpp:child1(9) - child1 loop 2
DEBUG 20180611 20:48:09.077655 8071 test.cpp:child2(18) - in child2:HELLO COLEWANG88888
INFO  20180611 20:48:09.077660 8071 test.cpp:child2(19) - in child2:2468097opqrstuvwxyz
```
日志共分为共6个级别，可以在调用日志库时设置程序的程序日志输出级别：
1. TRACE
2. DEBUG
3. INFO
4. WARN
5. ERROR
6. FATAL

日志会以小时为单位自动rolling， 默认输出路径为/tmp/EasyLog/，默认以程序可执行文件名为名称，最终日志名为
> log{小时级别时间}-{程序名}.txt（如"log2018061021-test.txt"）。

当程序发生SegmentFault时，日志库可以保证将Buffer内数据Dump到磁盘，关键日志信息不会丢失。经测试，日志输出效率稳定在200w+条/s以上，测试程序可以瞬时跑满全部磁盘IO，完全可以满足大多数后台服务的日志输出功能。

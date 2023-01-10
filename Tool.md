## perf 

[系统级性能分析工具perf的介绍与使用_ITPUB博客](http://blog.itpub.net/24585765/viewspace-2564885/)

进程
`perf record -p ID1,ID2 sleep seconds`

线程
`perf record -t ID1,ID2 sleep seconds`

输出调用栈
`-g = --call-graph=fp`

top
`perf top -p/t`


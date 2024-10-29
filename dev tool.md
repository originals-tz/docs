## GDB

带参数运行 `run 参数`

查看的pretty printer
```
(gdb) : info pretty-printer
```

c++默认自带格式化脚本,位置 : `/usr/share/gcc-*/python/libstdcxx/`
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

## 内存记录

[Procpath — Procpath documentation](https://procpath.readthedocs.io/en/latest/index.html)

记录

procpath record -i 1 -d [名字].sqlite '$..children[?(@.stat.pid == [进程pid])]'

可视化

[Analysing records — Procpath documentation](https://procpath.readthedocs.io/en/latest/analysis.html)

## 显示GBK

cat FC220628.001 | iconv -f GBK -t UTF-8

## ebpf

cat /boot/config-`uname -r` | grep CONFIG_DEBUG_INFO_BTF

[内存泄漏（增长）火焰图 | HeapDump性能社区](https://heapdump.cn/article/1661654?from=pc)

## nc文件传输

发送端 nc -l 12312 > target.tar.gz

接收端 nc ip 12312 < target.tar.gz

## 查看csv


```
column -s, -t < somefile.csv | less -#2 -N -S
```

## 查看进程
pgrep themesrv | xargs -I % pwdx %
ps -C themesrv -o cmd,rss
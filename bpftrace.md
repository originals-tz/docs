bpftrace踩坑

1.包管理安装的bpftrace可能并不是完全体

例如缺少libdw支持，无法解析DWARF(DWARF是ELF最常用的调试信息格式)

没有调试信息就无法在bpftrace中获取函数的返回值之类的数据

因此可以使用`bpftrace info`查看构建信息，如果缺少支持，在进行源码安装

2.调试信息的安装

对于ubuntu，参考

https://wiki.ubuntu.com/Debug%20Symbol%20Packages

调试信息的包格式为 `libxxx-dbgsym` 使用apt安装

需要注意的是, bpftrace也有自己的调试信息，需要安装 `bpftrace-dbgsym`

3.获取二进制中的符号

使用`nm -D lib.so` 或者 `objdump -tT a.out`

4.程序结束后无法解析对应的符号

如果使用了ustack，应在程序退出前结束bpftrace，程序退出后无法根据地址解析出具体的栈，只能打印出地址

5.牛刀小试

```
#!/usr/bin/env bpftrace
uprobe:libtcmalloc:malloc
/ comm == "a.out" /
{
    @add = arg0;
}

uretprobe:libtcmalloc:malloc
/ comm == "a.out"/
{
    @memory[uptr(retval)] += @add;
    printf("malloc %p...\n", retval);
    @sk[uptr(retval)] = ustack;
}


uprobe:libtcmalloc:free
/ comm == "a.out" /
{
    printf("free %p...\n", arg0);
    delete(@memory[uptr(arg0)]);
    delete(@sk[uptr(arg0)]);
}


uprobe:libc:malloc
/ comm == "a.out" /
{
    @add = arg0;
}

uretprobe:libc:malloc
/ comm == "a.out"/
{
    @memory[uptr(retval)] += @add;
    printf("malloc %p...\n", retval);
    @sk[uptr(retval)] = ustack;
}


uprobe:libc:free
/ comm == "a.out" /
{
    printf("free %p...\n", arg0);
    delete(@memory[uptr(arg0)]);
    delete(@sk[uptr(arg0)]);
}
```

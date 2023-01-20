[深入浅出 eBPF | 专注于 Linux 内核技术eBPF (Linux/Kernel/XDP/BCC/BPFTrace/Cilium)](https://www.ebpf.top/)

- bpftrace追踪内存分配
http://mysqlentomologist.blogspot.com/2021/05/dynamic-tracing-of-memory-allocations.html

查看so符号 nm -D lib.so

objdump -tT a.out

```
sudo bpftrace -e 'uprobe:libtcmalloc:tc_new /comm == "a.out"/ { printf("All│
ocated %d bytes\n", arg0); }'
```

libtcmalloc : tc_new tc_delete

ubuntu debuginfo安装
https://wiki.ubuntu.com/Debug%20Symbol%20Packages

bpftrace BEGIN无法执行
https://zhuanlan.zhihu.com/p/576186351

bpftrace libdw support 
bpftrace info 查看构建信息，如果没有需要手动编译

如果使用了ustack，应在程序退出前结束bpftrace，程序退出后无法根据地址解析出具体的栈，只能打印出地址

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
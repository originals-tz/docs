## pyclang无法正确解析STL对象

安装pyclang后无法正确解析STL对象，可能是链接的clang库出现问题

使用pip list查看安装了那些与clang有关的包

卸载pip安装的libclang，pip中的libclang自带了一个clang的so，后续安装的包可能link到上面去了

使用系统包管理安装libclang

然后使用pip重新安装clang和pyclang

## ssh 配置

```
Host *
    # 默认转发密钥
    ForwardAgent yes
    #不自动接收公钥，如果设置为 yes，ssh 就不会自动把计算机的密钥加入到 .ssh/known_hosts 文件，并且一旦计算机的密钥发生了变化，就拒绝连接。
    StrictHostKeyChecking no
    # 将访问过的 hosts 以 hash 的形式存放，降低被入侵后信息暴露的风险
    HashKnownHosts yes
    # 压缩传输，取值on/off/force。在 scp 等传输文件的情况下提高传输速度
    Compression yes
    ServerAliveInterval 60
    ServerAliveCountMAx 5
    ControlMaster auto
    ControlPersist yes
    ControlPath ~/.ssh/socks/%r@%h:%p
```

## Clion GDB remote Error during pretty printers setup

首先查看clion日志 Help/Show Log In Explorer

```
2023-01-28 14:23:25,824 [ 260107]   WARN - #c.j.c.e.debugger - [UNEXPECTED] >python import sys; sys.dont_write_bytecode = True; sys.path.insert(0, "/tmp/1697b084-1d48-4dd1-9197-638fd5a7e500/.clion.resources/pretty_printers/gdb"); from default.printers import register_default_printers; register_default_printers(None); from libstdcxx.v6.printers import register_libstdcxx_printers; register_libstdcxx_printers(None); from default.libstdcxx_printers import patch_libstdcxx_printers_module; patch_libstdcxx_printers_module(); 

```
可以看到引用了`sys.path.insert(0, "/tmp/1697b084-1d48-4dd1-9197-638fd5a7e500/.clion.resources/pretty_printers/gdb")`

将其删除

## 4tb以上的硬盘挂载

```
fdisk /dev/sda

----------- in fdisk
g : 创建gpt分区表
n : 创建分区，使用默认就行
------------ end

mkfs.ext4 /dev/sda1 在分区上建立文件系统
vim /etc/fstab 设置挂载路径
moutn -a 挂载
```
## 硬盘间同步

https://explainshell.com/explain?cmd=rsync+-avzH

```
rsync  --progress -avzH hdd1 hdd2/
--progress 显示进度
-z 压缩
-H 保持硬链接
```
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
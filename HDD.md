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
### 硬盘间同步

https://explainshell.com/explain?cmd=rsync+-avzH

```
rsync  --progress -avzH hdd1 hdd2/
--progress 显示进度
-z 压缩
-H 保持硬链接
```
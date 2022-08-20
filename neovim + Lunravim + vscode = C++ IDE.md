## Clion 远程开发之痛
当下，Jetbrain IDE大行其道，自己配置一套开发工具链已经成为一件投产比很低的事情
几年前，为了享受服务器的资源，我毅然而然的掏钱购入了Jetbrian全家桶
不得不说，Jetbrain的产品体验还是挺不错的，作为一个非常讨厌点点点来配置工程的用户(因为记不住要怎么点)，Clion将cmake作为配置深得我心，同时也很好的结合了其他开源工具链
唯一的痛点就是，Clion的远程开发真的太弱了
为了支持文件索引，Clion要在服务器上生成对应的索引文件，然后打包下载到本地
频繁的网络IO，在某些情况下，就会出现打开一个文件要渲染几秒甚至十几秒的情况
同时，Jetbrian全家桶吃内存也是令人诟病已久的
开几个工程，分分钟吃完32G内存
可能Jetbrain已经意识到这些问题的存在，因此在最近又推出了全新的远程开发模式Getway,说白了就是vscode那套
然而，Gateway的bug实在是太多了，网络IO日常打满，文件编辑更是bug百出，可以说，可用度基本为0
这样看来，Clion其实已经满足不了我了，所以，我又把目光移到了Vim/Emacs等编辑器上
Emacs其实我个人感觉是坑比较多的，Vim我又不是很习惯VimScript这种奇葩脚本语言
恰巧最近看到了Neovim势头很猛，而且是用Lua作为配置语言，于是便抱着试一试的态度上了车
## Neovim + LunarVim 全新的开始
vim和emacs在前几年的时候也折腾过，但当时可能是没什么耐心，也可能是需求不明确, 到后面都放弃了
而这次可能是想明白了，很多东西做起来也很有目的性，到后面也是达到了预期，所以写了这篇文章大概介绍下配置过程

### 1.准备好你的代理
由于大部分配置都是托管在github上的开源项目，所以有一个好的代理就比较重要了

下面说一下命令行配置代理的方式，我订阅的是Torjan的代理，在windows上是用一个clash的软件去跑的，所以linux下也一样，唯一的不同就是没有UI

```
下载clash

https://github.com/Dreamacro/clash

导出你的代理配置 conf.yml

启动 ./clash -f conf.yml
```

这样子就能跑起来了，然后设置下http，https和git的代理, ip和端口就看你配置文件里面怎么配的了

```
export http_proxy=http://127.0.0.1:7890

export https_proxy=http://127.0.0.1:7890

git config --global https.proxy https://127.0.0.1:7890

git config --global http.proxy http://127.0.0.1:7890
```
### 2.开始配置Neovim和LunarVim
作为一个没有root权限的弟弟，我只能使用源码编译的方式，然后安装到 `~/.local`下
首先设置下搜素路径 `.bashrc`,保证`.local`下的数据可以被搜到

```
export PATH="/home/use/.local:$PATH"
```
然后就按照文档配置下
```
neovim

https://github.com/neovim/neovim.git

https://github.com/LunarVim/LunarVim.git

```

配置好后执行下`:checkhealth`看下有哪些问题
一般有问题的大多数就是一些需要额外执行命令来安装的东西
例如LSP后端的安装，基本看下文档就可以解决
### 3.vscode作为gdb前端
vscode可以说是最好的gdb前端，前提是要配置好gdb的打印

gcc默认提供了对应的python脚本，首先确保gdb有python支持，然后在`~/.gdbinit`里进行配置，一个超棒的gdb前端就完成了
```
python
import sys
sys.path.insert(0, "/usr/share/gcc/python")
from libstdcxx.v6.printers import register_libstdcxx_printers
register_libstdcxx_printers(None)
end
```

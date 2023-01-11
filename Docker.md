```
docker run 
--name hdfsdev 
-p 5000:22 # 开启端口
-v [宿主路径]:[docker路径] #挂载目录
-itd [镜像]
```

```
docer exec -it 
-w /root #设置工作路径
[docker名字]
/bin/bash
```

```
# 解压
docker exec -it hdfsdev tar -xvf /root/$DEPLOY_FILE -C /root
docker exec -it hdfsdev rm /root/$DEPLOY_FILE
docker exec -it hdfsdev ls /root/
# 编译
docker exec -it hdfsdev rm -rf $Build
docker exec -it hdfsdev mkdir $Build
docker exec -w $Build -it hdfsdev cmake -DCMAKE_BUILD_TYPE=Release ..
docker exec -w $Build -it hdfsdev make -j
docker exec -it hdfsdev cp $Build/$SERVER $Release/$SERVER
docker exec -it -w $Release hdfsdev /bin/bash
```
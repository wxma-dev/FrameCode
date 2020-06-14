#!/bin/bash

#shell脚本实现本地文件与服务器文件同步

read -t 30 -p "请输入项目名:" name
echo -e "\n"
echo "项目名为:$name"
case $name in
   cn)
   rsync -r ~/project1 root@192.168.18.2:/project1 --exclude="test.css" --progress
   ;;
   cnb2b)
   rsync -r ~/project2 root@192.168.18.2:/web/project2 --exclude="node_modules" --progress
   ;;
   *)
   echo "Ignorant"
   ;;
 esac
 echo "同步完成"

#name表示要同步的文件名
#  --exculde="node_modules"表示过滤的文件夹
#  --progress 显示文件同步进程



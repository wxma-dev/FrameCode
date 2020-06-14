#!/bin/bash

for ip in 192.168.56.{1..254}
do
 (
 ping $ip -c 4 &>/dev/null;
 if [ $? -eq 0 ]; then
  echo $ip is alive
 fi
 ) &
done
wait


#重点是在于()和&，把for里面需要执行的命令当作一个组合并在后台运行。
#wait等待所有后台子程序执行完毕


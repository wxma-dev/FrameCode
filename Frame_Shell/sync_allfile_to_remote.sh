#!/bin/bash  

#遍历本地文件夹下的所有文件名，分别压缩后ftp传送到远程服务器上保存

local_ip=$(ifconfig | grep "inet addr" | grep -v 127.0.0.1 | awk '{print $2}' | awk -F ':' '{print $2}') 
backup_dir="/var/web_backup/data"
date=`date +%Y%m%d` 
deldate=`date -d -14day +%Y%m%d` 
mkdir -p $backup_dir 
web_path="/mysite"
web_dir=$(ls -l /mysite/ |awk '/^d/ {print $NF}') 
for web_name in $web_dir 
do
    if [ "$web_name" != "lost+found" ] 
    then
    tar zcvf $backup_dir/$web_name.dump_$date.tar.gz $web_path/$web_name 
    fi
done
ftp -n<<! 
open 10.XX.XX.XX 
user ftpuser ****** 
binary 
hash
cd backups/web/$local_ip/ 
lcd /var/web_backup/data
prompt 
mput *.tar.gz 
close 
bye 
! 
rm -rf $backup_dir/*



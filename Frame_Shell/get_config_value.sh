#!/bin/bash

#举例，配置文件如下：
#[TESTFTP]
#host=127.0.0.1
#name=my
#pass=mylove
#type=ftp
#[TESTSSH]
#host=127.0.0.1
#name=my
#pass=mylove
#type=ssh
#[END]


initConf()
{
HOSTID=$1
CONF=$2
echo "----cat----"
cat $CONF \\
    | grep -v ^$ \\
    | sed -n "s/\\s\\+//;/\\[${HOSTID}\\]/,/^\\[/p" \\
    | grep -v ^'\\['
echo "----INFO----"

INFO=`cat $CONF \\
    | grep -v ^$ \\
    | sed -n "s/\\s\\+//;/\\[${HOSTID}\\]/,/^\\[/p" \\
    | grep -v ^'\\[' ` && eval "$INFO"

echo $host
echo $name
echo $pass
echo $type
}



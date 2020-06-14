#!/bin/bash

cd $1
ls -trl | grep ^-  | awk '{print $9}' | while read line
do
	iconv -f utf8 -t gb2312 ${line} > ${line}.tmp;
	cp ${line}.tmp ${line};
	rm ${line}.tmp;
done




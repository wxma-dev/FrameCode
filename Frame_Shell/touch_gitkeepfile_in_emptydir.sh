#!/bin/bash

if [ $# -ne 1 ] ; then
	echo "args error"
fi

workdir=$1


#touch file .gitkeep in emtpy dir
find $workdir -type d -empty | xargs du -a | awk '{if ($1 == 0 ) {print $2}}' | while read line
do
	echo "empty dir:$line"
	touch ${line}/.gitkeep
done

#check empty dir
count="0"
find $workdir -type d -empty | xargs du -a | awk '{if ($1 == 0 ) {print $2}}' | while read line
do
	count=`expr $count + 1`;
	echo "check, hava empty dir :$line"
done

if [ "$count" -eq "0" ] ; then
	echo "check, not have empty dir"
fi


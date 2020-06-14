#!/bin/bash

if [ $# -ne 1 ] ; then
	echo "args error"
fi

workdir=$1


#echo in emtpy dir
echo "----------------------------start----list empty dir--------------------------------"
find $workdir -type d -empty | while read line
do
	echo "$line"
done
echo "-----------------------------end-----list empty dir--------------------------------"

icount=`find $workdir -type d -empty | wc -l`
echo "icount=[$icount]"
if [ "$icount" -eq "0" ] ; then
	echo "not have any empty dir"
	exit
fi


#touch file .gitkeep in emtpy dir
printf "\n\n\n"
echo -n "are you sure touch .gitkeep in the empty dir(y/n)?"
read line
if [ "$line" == "y" ] ; then
	find $workdir -type d -empty | while read line
	do
		touch ${line}/.gitkeep && echo "touch ${line}/.gitkeep success"
	done
else
	exit
fi

#check empty dir
find $workdir -type d -empty | while read line
do
	echo "check, hava empty dir :$line"
done

count=`find $workdir -type d -empty | wc -l`
echo "count=[$count]"
if [ "$count" -eq "0" ] ; then
	echo "check,count=[$count] not have empty dir"
fi






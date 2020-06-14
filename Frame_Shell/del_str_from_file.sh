#!/bin/sh


# 功能:    删除文件中含"指定内容"的行
# 运行方式: ./dline.sh c.log  ==> 产生输出文件: c.log0
array=(
  "rm -f lvr_3531_pf_new"
  "arm-hisiv100-linux-gcc "
  "In function "
  "excess elements in array initializer"
  "warning: multi-line comment"
  "embedded '\\0' in format"
  "__NR_SYSCALL_BASE"
  "this is the location of the previous definition"
  "dereferencing type-punned pointer will break strict-aliasing rules"
  "differ in signedness"
  "but argument is of type"
  "implicit declaration of"
)
if [ $# -lt 1 ]; then
  echo "Usage: $0 <logfile>"
  exit
fi
file="$1"0
cp -f $1 $file
function deleteLine()
{
  sed "/$1/d" $file > tmp
  mv -f tmp $file
}
wc -l $file
for line in "${array[@]}"
do
  if [ ${#line} -gt 0 ] && [ ${line:0:1} != "#" ]; then
    deleteLine "$line"
  fi
done
wc -l $file

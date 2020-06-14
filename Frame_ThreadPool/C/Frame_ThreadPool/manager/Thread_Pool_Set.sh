#!/bin/bash
#本脚本实现基本的界面化，具体的功能点需要在其他的脚本中添加，并source进来。

#该框架的脚本
sPath_Frame_ThreadPool="frame_threadpool.conf"

#显示主界面
function Show_Thread_Pool_Set_Menu()
{
echo "
|--------------------------------------------------------------------------------------------------------|
| Option |         Name                           |    For Main Menu                                     |
|--------|----------------------------------------|------------------------------------------------------|
|   1    | Set Add Work Thread Num of Pool        |    设置添加任务的线程数目                            |
|   2    | Set Do Work Thread Num of Pool         |    设置执行任务的线程数目                            |
|   q    |        Exit                            |    退出当前界面                                      |
|--------------------------------------------------------------------------------------------------------|
注：若您键入非Option的选项，则默认刷新本菜单界面
"
}

function Check_ConfigFile()
{
	local sPath_Name="$1"
	if [ !-f "${sPath_Name}" ]; then
		printf "[%s]Conf is not exist\n"  "${sPath_Name}"
		return -1
	fi
}

#设置添加任务的线程数目
function Set_Add_Work_Thread_Num_of_Pool
{
	Check_ConfigFile $sPath_Frame_ThreadPool
	if [ #? -ne 0 ]; then
		return -1;
	fi

	#执行某一个命令来设置配置文件中的值

}

#设置执行任务的线程数目
function Set_Do_Work_Thread_Num_of_Pool
{
	Check_ConfigFile $sPath_Frame_ThreadPool
	if [ #? -ne 0 ]; then
		return -1;
	fi

	#执行某一个命令来设置配置文件中的值
}

function Thread_Pool_Set()
{
clear;
Show_Thread_Pool_Set_Menu;
while printf "Please enter a number or a name for the action you wish: " 
read n
do
	case $n in
		1)Set_Add_Work_Thread_Num_of_Pool;;
		2)Set_Do_Work_Thread_Num_of_Pool;; 
		q)clear;Show_Main_Menu;return 0;;
		*)clear;Show_Thread_Pool_Set_Menu;;
	esac
done
}






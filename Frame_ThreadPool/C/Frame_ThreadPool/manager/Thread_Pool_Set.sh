#!/bin/bash
#���ű�ʵ�ֻ����Ľ��滯������Ĺ��ܵ���Ҫ�������Ľű�����ӣ���source������

#�ÿ�ܵĽű�
sPath_Frame_ThreadPool="frame_threadpool.conf"

#��ʾ������
function Show_Thread_Pool_Set_Menu()
{
echo "
|--------------------------------------------------------------------------------------------------------|
| Option |         Name                           |    For Main Menu                                     |
|--------|----------------------------------------|------------------------------------------------------|
|   1    | Set Add Work Thread Num of Pool        |    �������������߳���Ŀ                            |
|   2    | Set Do Work Thread Num of Pool         |    ����ִ��������߳���Ŀ                            |
|   q    |        Exit                            |    �˳���ǰ����                                      |
|--------------------------------------------------------------------------------------------------------|
ע�����������Option��ѡ���Ĭ��ˢ�±��˵�����
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

#�������������߳���Ŀ
function Set_Add_Work_Thread_Num_of_Pool
{
	Check_ConfigFile $sPath_Frame_ThreadPool
	if [ #? -ne 0 ]; then
		return -1;
	fi

	#ִ��ĳһ�����������������ļ��е�ֵ

}

#����ִ��������߳���Ŀ
function Set_Do_Work_Thread_Num_of_Pool
{
	Check_ConfigFile $sPath_Frame_ThreadPool
	if [ #? -ne 0 ]; then
		return -1;
	fi

	#ִ��ĳһ�����������������ļ��е�ֵ
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






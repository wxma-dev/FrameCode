




/*
Ӧ�ò㣬��ܲ㣬��ʹ���ļ�
*/
#ifndef __COMMON_H__
#define __COMMON_H__






/* �������͵�ָ��Ĵ�С��Ϊ sizeof( void * ) */
#define POINT_SIZE ( sizeof( void * ) )

/*
�������ڴ��ĵ�ַ��ӵ�������
*/
void Into_Queue( void * pPoint_Task );


/*
����Ƿ���ϱ�׼
*/
int Check_Task_Mem( void * const pPoint );

/*
��ȡ����־λ��ָ��
*/
void * Get_Mark_Addr( void * const pPoint_Addr );


/*
���㡱��ŵ�ֵַ���ڴ�顰���׵�ַ
*/
void * Get_Start_Addr( void * const pPoint_Addr );


/*
��ȡ����ŵ�ֵַ���ڴ�顰�е�ֵ
��Σ�pPoint_Start_Addr   ��ʼ�ĵ�ַ
���Σ�pPoint_Task_Addr    ����ŵ�ֵַ���ڴ�顰�е�ֵ����ָ��
*/
void Get_Task_Addr( void * const pPoint_Start_Addr, void ** pPoint_Task_Addr );


/*
�ͷš���ŵ�ֵַ���ڴ�顰ָ����ڴ�����
*/
void Free_Task_Addr( void * pPoint_Task_Addr );



#endif


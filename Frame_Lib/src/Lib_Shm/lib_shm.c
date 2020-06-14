/*************************************************
*  Copyright (C) 2011 HICOM
*  All rights reserved
*
*  This is unpublished proprietary source code of HICOM Ltd.
*  The copyright notice above does not evidence any actual
*  or intended publication of such source code.
*
*  NOTICE: UNAUTHORIZED DISTRIBUTION, ADAPTATION OR USE MAY BE
*  SUBJECT TO CIVIL AND CRIMINAL PENALTIES.
*
*  File name:      lib_shm.c
*
*  Author:       Version:        Date: 
*  mawx           1.0             2018-05-08
*
*  Description: 
*  Others:       
*  Function List: 
*      1. Init_Shm()           ��ʼ�������ڴ�
*  History:        
*                  
* NO  Date        Author       reason     Modification
* 1   2018-05-08  mawx                    ����
* 2   2018-05-08  mawx                    ���Ӻ�����
************************************************/
/* Unix Head File */
#include <sys/ipc.h>
#include <sys/shm.h>

#if 0

/*��������������������������������������������������������������������������������������Ҫ���ݾ���ҵ����������޸ġ�������������������������������������������������������������������*/
/*************************************************
*  Function:       How_To_Update
*  Description:    ��ҵ�����ݸ��µ������ڴ���
*  Table Accessed: 
*  Table Updated:  
*  Input:          pPoint_In            ����ĵ�ַ
*                  iFlag_Open_or_Close  ͨ��ͨ�ı�ʶ
*                  iIndex               �����±�
*  Output:         
*  Return:         0  �ɹ�
*                 -1  ʧ��
*  Error:          
*  Others:         
*************************************************/
void How_To_Update( void * pPoint_In , int iFlag_Open_or_Close, int iIndex )
{
    MQMGRARR * pPoint;
    
    pPoint = NULL;

    pPoint = (MQMGRARR *)pPoint_In;

    /* �޸�ǰ��ӡ���� */

    /* �޸ı�ʶ */
    ( *( (*pPoint).que_info_+iIndex) ).que_open_succ_flag_ = iFlag_Open_or_Close;
    pPoint = NULL;

    /* �޸ĺ��ӡ���� */
}

/*************************************************
*  Function:       What_Is_Flag
*  Description:    ��ȡ��ͨ��״̬
*  Table Accessed: 
*  Table Updated:  
*  Input:          pPoint_In            ����ĵ�ַ
*                  iIndex               �����±�
*  Output:         
*  Return:         ��ͨ��״̬
*                  
*  Error:          
*  Others:         
*************************************************/
int What_Is_Flag(void * pPoint_In , int iIndex)
{
    int iFlag;
    
    iFlag = 0;
    
    iFlag = ( *( (*pPoint).que_info_+iIndex) ).que_open_succ_flag_;
    
    return iFlag;
}

/*************************************************
*  Function:       Update_ConnFlag_Shm
*  Description:    MQ��ͨ״̬ д�뵽�����ڴ���
*  Table Accessed: 
*  Table Updated:  
*  Input:          kKey        String���͵Ľ�ֵ
*                  pPoint_Out  ��ȡ����ӳ���ַ
*  Output:         
*  Return:         0  �ɹ�
*                 -1  ʧ��
*  Error:          
*  Others:         
*************************************************/
int Update_ConnFlag_Shm( key_t kKey, int iFlag_Conn, int iIndex )
{
    int       iShmId;
    void    * pcLocShmAddr;
    
    iShmId = -1;
    pcLocShmAddr = NULL;
    
    /* ��ȡ�����ڴ�ID */
    iShmId = shmget( (key_t)kKey, 0, 0660 );
    if( iShmId == -1 )
    {
       return -1;
    }
    
    /* ��ȡ�����ڴ�ӳ���ַ */
    pcLocShmAddr = shmat( iShmId, 0, 0 );
    if( pcLocShmAddr == ( char * ) -1 )
    {
       return -1;
    }

    How_To_Update( pcLocShmAddr, iFlag_Conn, iIndex );
    
    shmdt(pcLocShmAddr);

    return 0;
}

/*************************************************
*  Function:       Update_ConnFlag_Shm_ByKeyStr
*  Description:    MQ��ͨ״̬ д�뵽�����ڴ��У���ֵ���ַ���ת����
*  Table Accessed: 
*  Table Updated:  
*  Input:          kKey        String���͵Ľ�ֵ
*                  pPoint_Out  ��ȡ����ӳ���ַ
*  Output:         
*  Return:         0  �ɹ�
*                 -1  ʧ��
*  Error:          
*  Others:         
*************************************************/
int Update_ConnFlag_Shm_ByKeyStr( char * sKey, int iFlag_Conn, int iIndex )
{
    int       iShmId;
    int       iRet;
    void    * pcLocShmAddr;
    int       kKey;

    iShmId = -1;
    iRet = -1;
    pcLocShmAddr = NULL;
    kKey = -1;
    
    /* ��ȡkey_t���ͽ�ֵ */
    kKey = ftok( sKey, 0 );
    if( -1 == kKey )
    {
        return -1;
    }
    
    iRet = Update_ConnFlag_Shm(kKey, iFlag_Conn, iIndex);
    if( iRet == -1 )
    {
       return -1;
    }

    return 0;
}


/*************************************************
*  Function:       Get_ConnFlag_Shm
*  Description:    �ӹ����ڴ��л�ȡMQ��ͨ״̬
*  Table Accessed: 
*  Table Updated:  
*  Input:          kKey        key_t���͵Ľ�ֵ
*                  iIndex      �����±�
*  Output:         
*  Return:         ��ͨ��״̬
*                  -1 ʧ��
*  Error:          
*  Others:         
*************************************************/
int Get_ConnFlag_Shm( key_t kKey, int iIndex )
{
    int       iShmId;
    void    * pcLocShmAddr;
    int       iFlag_Conn;
    
    iFlag_Conn = 0;
    iShmId = -1;
    pcLocShmAddr = NULL;
    
    /* ��ȡ�����ڴ�ID */
    iShmId = shmget( (key_t)kKey, 0, 0660 );
    if( iShmId == -1 )
    {
       return -1;
    }
    
    /* ��ȡ�����ڴ�ӳ���ַ */
    pcLocShmAddr = shmat( iShmId, 0, 0 );
    if( pcLocShmAddr == ( char * ) -1 )
    {
       return -1;
    }

    iFlag_Conn = What_Is_Flag( pcLocShmAddr , iIndex );
    
    shmdt(pcLocShmAddr);

    return iFlag_Conn;
}


/*************************************************
*  Function:       Get_ConnFlag_Shm_ByKeyStr
*  Description:    �ӹ����ڴ��л�ȡMQ��ͨ״̬����ֵ���ַ���ת����
*  Table Accessed: 
*  Table Updated:  
*  Input:          sKey        String���͵Ľ�ֵ
*                  iIndex      �����±�
*  Output:         
*  Return:         ��ͨ��״̬
*                  -1 ʧ��
*  Error:          
*  Others:         
*************************************************/
int Get_ConnFlag_Shm_ByKeyStr( char * sKey, int iIndex )
{
    int       iShmId;
    int       iRet;
    void    * pcLocShmAddr;
    int       kKey;
    int       iFlag_Conn;

    iShmId = -1;
    iRet = -1;
    pcLocShmAddr = NULL;
    kKey = -1;
    iFlag_Conn = -1;
    
    /* ��ȡkey_t���ͽ�ֵ */
    kKey = ftok( sKey, 0 );
    if( -1 == kKey )
    {
        return -1;
    }

    iFlag_Conn = Get_ConnFlag_Shm( kKey, iIndex );
    if( iFlag_Conn == -1 )
    {
       return -1;
    }

    return iFlag_Conn;
}
/*��������������������������������������������������������������������������Ҫ���ݾ���ҵ����������޸ġ���������������������������������������������������������������������*/





/*************************************************
*  Function:       Create_Shm
*  Description:    ���������ڴ�
*  Table Accessed: 
*  Table Updated:  
*  Input:          kKey        key_t���͵Ľ�ֵ
*                  iShmSize    �����ڴ�Ĵ�С
*  Output:         
*  Return:         0  �ɹ�
*                 -1  ʧ��
*  Error:          
*  Others:         �����Ӧ�Ĺ����ڴ���ڣ�����
*                  �������������򴴽�
*************************************************/
int Create_Shm( key_t kKey, int iShmSize )
{
    int       iShmId;                /* �����ڴ�ID */
    void    * pcSHM;                 /* �����ڴ�ӳ�䴦��ĵ�ַ */

    /* ��ʼ�� */
    iShmId = -1;
    pcSHM = NULL;

    /* ��������ڴ治����, �ȴ��� */
    iShmId = shmget( kKey, 0, 0660 );
    if( iShmId < 0 )
    {
        /* ���������ڴ� */
        iShmId = shmget( kKey, (size_t)iShmSize, IPC_CREAT | 0777 );
        if( iShmId == -1 )
        {
            shmctl( iShmId, IPC_RMID, NULL );
            return -1;
        }
    }

    /* ��ȡ�����ڴ�ĵ�ַ */
    pcSHM = shmat( iShmId, 0, 0 );
    if( -1 == (int)pcSHM )
    {
        shmctl( iShmId, IPC_RMID, NULL );
        return -1;
    }

    memcpy( pcSHM, pPoint, iPoint_Size );

    shmdt( pcSHM );

    return 0;
} /* end of Create_Shm() */

/*************************************************
*  Function:       Create_Shm_MustNULL
*  Description:    ���������ڴ�
*  Table Accessed: 
*  Table Updated:  
*  Input:          kKey        key_t���͵Ľ�ֵ
*                  iShmSize    �����ڴ�Ĵ�С
*  Output:         
*  Return:         0  �ɹ�
*                 -1  ʧ��
*  Error:          
*  Others:         �����Ӧ�Ĺ����ڴ���ڣ���ɾ��
*                  �ٴ������������򴴽�
*************************************************/
int Create_Shm_MustNULL( key_t kKey, int iShmSize )
{
    int       iShmId;                /* �����ڴ�ID */
    void    * pcSHM;                 /* �����ڴ�ӳ�䴦��ĵ�ַ */

    /* ��ʼ�� */
    iShmId = -1;
    pcSHM = NULL;

    /* ��������ڴ治����, �ȴ��� */
    iShmId = shmget( kKey, 0, 0660 );
    if( iShmId < 0 )  //������
    {
        /* ���������ڴ� */
        iShmId = shmget( kKey, (size_t)iShmSize, IPC_CREAT | 0777 );
        if( iShmId == -1 )
        {
            shmctl( iShmId, IPC_RMID, NULL );
            return -1;
        }
    }
    else //����
    {
        /* ɾ�������ڴ� */
        if( -1 == shmctl( iShmId, IPC_RMID, NULL ) )
        {
            return -1;
        }

        /* ���������ڴ� */
        iShmId = shmget( kKey, (size_t)iShmSize, IPC_CREAT | 0777 );
        if( iShmId == -1 )
        {
            shmctl( iShmId, IPC_RMID, NULL );
            return -1;
        }
    }

    /* ��ȡ�����ڴ�ĵ�ַ */
    pcSHM = shmat( iShmId, 0, 0 );
    if( -1 == (int)pcSHM )
    {
        shmctl( iShmId, IPC_RMID, NULL );
        return -1;
    }

    memcpy( pcSHM, pPoint, iPoint_Size );

    shmdt( pcSHM );

    return 0;
} /* end of Create_Shm_MustNULL() */

/*************************************************
*  Function:       Create_Shm_ByKeyStr
*  Description:    ���������ڴ�
*  Table Accessed: 
*  Table Updated:  
*  Input:          kKey        key_t���͵Ľ�ֵ
*                  iShmSize    �����ڴ�Ĵ�С
*  Output:         
*  Return:         0  �ɹ�
*                 -1  ʧ��
*  Error:          
*  Others:         
*************************************************/
int Create_Shm_ByKeyStr( char * sKey, int iShmSize )
{
    int       iShmId;                /* �����ڴ�ID */
    void    * pcSHM;                 /* �����ڴ�ӳ�䴦��ĵ�ַ */
    key_t     kKey;                  /* key_t���͵Ľ�ֵ */
    int       iRet;

    /* ��ʼ�� */
    iShmId = -1;
    pcSHM = NULL;
    iRet = -1;

    /* ��ȡkey_t���ͽ�ֵ */
    kKey = ftok( sKey, 0 );
    if( -1 == kKey )
    {
        return -1;
    }

    iRet = Create_Shm( kKey, iShmSize );
    if ( 0 != iRet )
    {
        return -1;
    }

    return 0;
} /* end of Create_Shm_ByKeyStr() */


/*************************************************
*  Function:       Init_Shm
*  Description:    ��ʼ�������ڴ�
*  Table Accessed: 
*  Table Updated:  
*  Input:          kKey        key_t���͵Ľ�ֵ
*                  iShmSize    �����ڴ�Ĵ�С
*                  pPoint      ���õ�ַ��ʼ���ڴ�ռ串���������ڴ�
*                  iPoint_Size ��Ҫ���ǵ��ڴ��С
*  Output:         
*  Return:         0  �ɹ�
*                 -1  ʧ��
*  Error:          
*  Others:         
*************************************************/
int Init_Shm( key_t kKey, int iShmSize, void * pPoint, int iPoint_Size )
{
    int iRet ;
    void    * pcSHM;                 /* �����ڴ�ӳ�䴦��ĵ�ַ */
    
    iRet = -1;
    pcSHM = NULL;
    
    if ( iShmSize < iPoint_Size || iShmSize < 0 || iPoint_Size < 0)
    {
        return -1;
    }

    /* ���������ڴ� */
    iRet = Create_Shm( kKey, iShmSize );
    if( 0 != iRet )
    {
        return -1;
    }

    /* ��ȡ�����ڴ�ID */
    iShmId = shmget( kKey, 0, 0660 );
    if( iShmId < 0 )
    {
        return -1;
    }

    /* ��ȡ�����ڴ�ĵ�ַ */
    pcSHM = shmat( iShmId, 0, 0 );
    if( -1 == (int)pcSHM )
    {
        shmctl( iShmId, IPC_RMID, NULL );
        return -1;
    }

    /* ��ʼ�� */
    memcpy( pcSHM, pPoint, iPoint_Size );

    shmdt( pcSHM );

    return 0;
} /* end of Init_Shm() */


/*************************************************
*  Function:       Init_Shm_ByKeyStr
*  Description:    ��ʼ�������ڴ棨��ֵ���ַ���ת����
*  Table Accessed: 
*  Table Updated:  
*  Input:          sKey        String���͵Ľ�ֵ
*                  iShmSize    �����ڴ�Ĵ�С
*                  pPoint      ���õ�ַ��ʼ���ڴ�ռ串���������ڴ�
*                  iPoint_Size ��Ҫ���ǵ��ڴ��С
*  Output:         
*  Return:         0  �ɹ�
*                 -1  ʧ��
*  Error:          
*  Others:         
*************************************************/
int Init_Shm_ByKeyStr( char * sKey, int iShmSize, void * pPoint, int iPoint_Size )
{
    int iRet;
    key_t kKey;

    iRet = -1;
    kKey = -1;

    /* ��ȡkey_t���ͽ�ֵ */
    kKey = ftok( sKey, 0 );
    if( -1 == kKey )
    {
        return -1;
    }

    iRet = Init_Shm( kKey, iShmSize, pPoint, iPoint_Size );
    if( 0 != iRet )
    {
        return -1;
    }

    return 0;
} /* end of Init_Shm_ByKeyStr() */


/*************************************************
*  Function:       Get_Addr_Shm
*  Description:    ��ȡ�����ڴ�ӳ������ĵ�ַ
*  Table Accessed: 
*  Table Updated:  
*  Input:          kKey        key_t���͵Ľ�ֵ
*  Output:         
*  Return:         �����ڴ�ӳ������ĵ�ַ
*                  NULL ʧ��
*  Error:          
*  Others:         ���ʹ�ú󣬻���Ҫshmdt����ж�ص���
*                  ��Dt_Addr_Shm�ɶ�ʹ��
*************************************************/
void * Get_Addr_Shm( key_t kKey )
{
    int       iShmId;
    void    * pcLocShmAddr;
    void    * pPoint_Return;
    
    iShmId = -1;
    pcLocShmAddr = NULL;
    pPoint_Return = NULL;
    
    /* ��ȡ�����ڴ�ID */
    iShmId = shmget( (key_t)kKey, 0, 0660 );
    if( iShmId == -1 )
    {
       return NULL;
    }
    
    /* ��ȡ�����ڴ�ӳ���ַ */
    pcLocShmAddr = shmat( iShmId, 0, 0 );
    if( pcLocShmAddr == ( char * ) -1 )
    {
       return NULL;
    }

    /* ֱ��ָ�븳ֵ */
    pPoint_Return = pcLocShmAddr;
    
#if 0
    /* ����ж�ص�ַ��ʹ��Dt_Addr_Shm �ڵ���Get_Addr_Shm������ж�� */
    shmdt(pcLocShmAddr);
#endif

    return pPoint_Return;
}

/*************************************************
*  Function:       Get_Addr_Shm_ByKeyStr
*  Description:    ��ȡ�����ڴ�ӳ������ĵ�ַ����ֵ���ַ���ת����
*  Table Accessed: 
*  Table Updated:  
*  Input:          sKey        String���͵Ľ�ֵ
*  Output:         
*  Return:         �����ڴ�ӳ������ĵ�ַ
*                  NULL ʧ��
*  Error:          
*  Others:         ���ʹ�ú󣬻���Ҫshmdt����ж�ص���
*                  ��Dt_Addr_Shm�ɶ�ʹ��
*************************************************/
void * Get_Addr_Shm_ByKeyStr( char * sKey )
{

    void    * pcLocShmAddr;
    void    * pPoint_Return;
    key_t     kKey;
    
    pcLocShmAddr = NULL;
    pPoint_Return = NULL;
    kKey = -1;
    
    /* ��ȡkey_t���ͽ�ֵ */
    kKey = ftok( sKey, 0 );
    if( -1 == kKey )
    {
        return NULL;
    }

    pPoint_Return = Get_Addr_Shm( kKey );
    if ( NULL == pPoint_Return )
    {
        return NULL;
    }

    return pPoint_Return;
}

/*************************************************
*  Function:       Dt_Addr_Shm
*  Description:    ж�� �����ڴ�ӳ������ĵ�ַ
*  Table Accessed: 
*  Table Updated:  
*  Input:          kKey        key_t���͵Ľ�ֵ
*  Output:         
*  Return:         0 �ɹ�
*                  -1 ʧ��
*  Error:          
*  Others:         
*************************************************/
int Dt_Addr_Shm( void * pPoint_Dt )
{
    if ( NULL == pPoint_Dt )
    {
        return -1;
    }

    shmdt(pPoint_Dt);

    return 0;
}

/*************************************************
*  Function:       Del_Shm
*  Description:    ɾ�������ڴ�
*  Table Accessed: 
*  Table Updated:  
*  Input:          kKey        key_t���͵Ľ�ֵ
*  Output:         
*  Return:         0  �ɹ�
*                  -1 ʧ��
*  Error:          
*  Others:         
*************************************************/
int Del_Shm( key_t kKey )
{
    int iRet;
    int iShmId;

    iRet = -1;
    iShmId = -1;

    /* ��ȡ�����ڴ�ID */
    iShmId = shmget( (key_t)kKey, 0, 0660 );
    if( -1 == iShmId )
    {
       return -1;
    }

    /* ɾ�������ڴ� */
    iRet = shmctl( iShmId, IPC_RMID, NULL );
    if( 0 != iRet )
    {
        return -1;
    }

    return 0;
}


/*************************************************
*  Function:       Del_Shm_ByKeyStr
*  Description:    ɾ�������ڴ棨��ֵ���ַ���ת����
*  Table Accessed: 
*  Table Updated:  
*  Input:          kKey        key_t���͵Ľ�ֵ
*  Output:         
*  Return:         0  �ɹ�
*                  -1 ʧ��
*  Error:          
*  Others:         
*************************************************/
int Del_Shm_ByKeyStr( char * sKey )
{
    key_t  kKey;
    int    iRet;
    
    kKey = -1;
    iRet = -1;

    /* ��ȡkey_t���ͽ�ֵ */
    kKey = ftok( sKey, 0 );
    if( -1 == kKey )
    {
        return -1;
    }

    /* ɾ�������ڴ� */
    iRet = Del_Shm( kKey );
    if( -1 == iRet )
    {
        return -1;
    }

    return 0;
}

#endif

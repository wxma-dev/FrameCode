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
*      1. Init_Shm()           初始化共享内存
*  History:        
*                  
* NO  Date        Author       reason     Modification
* 1   2018-05-08  mawx                    建立
* 2   2018-05-08  mawx                    增加函数：
************************************************/
/* Unix Head File */
#include <sys/ipc.h>
#include <sys/shm.h>

#if 0

/*↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓此线以下需要根据具体业务需求进行修改↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓*/
/*************************************************
*  Function:       How_To_Update
*  Description:    将业务数据更新到共享内存中
*  Table Accessed: 
*  Table Updated:  
*  Input:          pPoint_In            输入的地址
*                  iFlag_Open_or_Close  通不通的标识
*                  iIndex               数组下标
*  Output:         
*  Return:         0  成功
*                 -1  失败
*  Error:          
*  Others:         
*************************************************/
void How_To_Update( void * pPoint_In , int iFlag_Open_or_Close, int iIndex )
{
    MQMGRARR * pPoint;
    
    pPoint = NULL;

    pPoint = (MQMGRARR *)pPoint_In;

    /* 修改前打印出来 */

    /* 修改标识 */
    ( *( (*pPoint).que_info_+iIndex) ).que_open_succ_flag_ = iFlag_Open_or_Close;
    pPoint = NULL;

    /* 修改后打印出来 */
}

/*************************************************
*  Function:       What_Is_Flag
*  Description:    获取联通的状态
*  Table Accessed: 
*  Table Updated:  
*  Input:          pPoint_In            输入的地址
*                  iIndex               数组下标
*  Output:         
*  Return:         联通的状态
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
*  Description:    MQ联通状态 写入到共享内存中
*  Table Accessed: 
*  Table Updated:  
*  Input:          kKey        String类型的健值
*                  pPoint_Out  获取到的映射地址
*  Output:         
*  Return:         0  成功
*                 -1  失败
*  Error:          
*  Others:         
*************************************************/
int Update_ConnFlag_Shm( key_t kKey, int iFlag_Conn, int iIndex )
{
    int       iShmId;
    void    * pcLocShmAddr;
    
    iShmId = -1;
    pcLocShmAddr = NULL;
    
    /* 获取共享内存ID */
    iShmId = shmget( (key_t)kKey, 0, 0660 );
    if( iShmId == -1 )
    {
       return -1;
    }
    
    /* 获取共享内存映射地址 */
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
*  Description:    MQ联通状态 写入到共享内存中（健值由字符串转换）
*  Table Accessed: 
*  Table Updated:  
*  Input:          kKey        String类型的健值
*                  pPoint_Out  获取到的映射地址
*  Output:         
*  Return:         0  成功
*                 -1  失败
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
    
    /* 获取key_t类型健值 */
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
*  Description:    从共享内存中获取MQ联通状态
*  Table Accessed: 
*  Table Updated:  
*  Input:          kKey        key_t类型的健值
*                  iIndex      数组下标
*  Output:         
*  Return:         联通的状态
*                  -1 失败
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
    
    /* 获取共享内存ID */
    iShmId = shmget( (key_t)kKey, 0, 0660 );
    if( iShmId == -1 )
    {
       return -1;
    }
    
    /* 获取共享内存映射地址 */
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
*  Description:    从共享内存中获取MQ联通状态（健值由字符串转换）
*  Table Accessed: 
*  Table Updated:  
*  Input:          sKey        String类型的健值
*                  iIndex      数组下标
*  Output:         
*  Return:         联通的状态
*                  -1 失败
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
    
    /* 获取key_t类型健值 */
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
/*↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑此线以上需要根据具体业务需求进行修改↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑*/





/*************************************************
*  Function:       Create_Shm
*  Description:    创建共享内存
*  Table Accessed: 
*  Table Updated:  
*  Input:          kKey        key_t类型的健值
*                  iShmSize    共享内存的大小
*  Output:         
*  Return:         0  成功
*                 -1  失败
*  Error:          
*  Others:         如果对应的共享内存存在，则不用
*                  创建；不存在则创建
*************************************************/
int Create_Shm( key_t kKey, int iShmSize )
{
    int       iShmId;                /* 共享内存ID */
    void    * pcSHM;                 /* 共享内存映射处理的地址 */

    /* 初始化 */
    iShmId = -1;
    pcSHM = NULL;

    /* 如果共享内存不存在, 先创建 */
    iShmId = shmget( kKey, 0, 0660 );
    if( iShmId < 0 )
    {
        /* 创建共享内存 */
        iShmId = shmget( kKey, (size_t)iShmSize, IPC_CREAT | 0777 );
        if( iShmId == -1 )
        {
            shmctl( iShmId, IPC_RMID, NULL );
            return -1;
        }
    }

    /* 获取共享内存的地址 */
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
*  Description:    创建共享内存
*  Table Accessed: 
*  Table Updated:  
*  Input:          kKey        key_t类型的健值
*                  iShmSize    共享内存的大小
*  Output:         
*  Return:         0  成功
*                 -1  失败
*  Error:          
*  Others:         如果对应的共享内存存在，则删除
*                  再创建；不存在则创建
*************************************************/
int Create_Shm_MustNULL( key_t kKey, int iShmSize )
{
    int       iShmId;                /* 共享内存ID */
    void    * pcSHM;                 /* 共享内存映射处理的地址 */

    /* 初始化 */
    iShmId = -1;
    pcSHM = NULL;

    /* 如果共享内存不存在, 先创建 */
    iShmId = shmget( kKey, 0, 0660 );
    if( iShmId < 0 )  //不存在
    {
        /* 创建共享内存 */
        iShmId = shmget( kKey, (size_t)iShmSize, IPC_CREAT | 0777 );
        if( iShmId == -1 )
        {
            shmctl( iShmId, IPC_RMID, NULL );
            return -1;
        }
    }
    else //存在
    {
        /* 删除共享内存 */
        if( -1 == shmctl( iShmId, IPC_RMID, NULL ) )
        {
            return -1;
        }

        /* 创建共享内存 */
        iShmId = shmget( kKey, (size_t)iShmSize, IPC_CREAT | 0777 );
        if( iShmId == -1 )
        {
            shmctl( iShmId, IPC_RMID, NULL );
            return -1;
        }
    }

    /* 获取共享内存的地址 */
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
*  Description:    创建共享内存
*  Table Accessed: 
*  Table Updated:  
*  Input:          kKey        key_t类型的健值
*                  iShmSize    共享内存的大小
*  Output:         
*  Return:         0  成功
*                 -1  失败
*  Error:          
*  Others:         
*************************************************/
int Create_Shm_ByKeyStr( char * sKey, int iShmSize )
{
    int       iShmId;                /* 共享内存ID */
    void    * pcSHM;                 /* 共享内存映射处理的地址 */
    key_t     kKey;                  /* key_t类型的健值 */
    int       iRet;

    /* 初始化 */
    iShmId = -1;
    pcSHM = NULL;
    iRet = -1;

    /* 获取key_t类型健值 */
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
*  Description:    初始化共享内存
*  Table Accessed: 
*  Table Updated:  
*  Input:          kKey        key_t类型的健值
*                  iShmSize    共享内存的大小
*                  pPoint      将该地址开始的内存空间覆盖至共享内存
*                  iPoint_Size 需要覆盖的内存大小
*  Output:         
*  Return:         0  成功
*                 -1  失败
*  Error:          
*  Others:         
*************************************************/
int Init_Shm( key_t kKey, int iShmSize, void * pPoint, int iPoint_Size )
{
    int iRet ;
    void    * pcSHM;                 /* 共享内存映射处理的地址 */
    
    iRet = -1;
    pcSHM = NULL;
    
    if ( iShmSize < iPoint_Size || iShmSize < 0 || iPoint_Size < 0)
    {
        return -1;
    }

    /* 创建共享内存 */
    iRet = Create_Shm( kKey, iShmSize );
    if( 0 != iRet )
    {
        return -1;
    }

    /* 获取共享内存ID */
    iShmId = shmget( kKey, 0, 0660 );
    if( iShmId < 0 )
    {
        return -1;
    }

    /* 获取共享内存的地址 */
    pcSHM = shmat( iShmId, 0, 0 );
    if( -1 == (int)pcSHM )
    {
        shmctl( iShmId, IPC_RMID, NULL );
        return -1;
    }

    /* 初始化 */
    memcpy( pcSHM, pPoint, iPoint_Size );

    shmdt( pcSHM );

    return 0;
} /* end of Init_Shm() */


/*************************************************
*  Function:       Init_Shm_ByKeyStr
*  Description:    初始化共享内存（健值由字符串转换）
*  Table Accessed: 
*  Table Updated:  
*  Input:          sKey        String类型的健值
*                  iShmSize    共享内存的大小
*                  pPoint      将该地址开始的内存空间覆盖至共享内存
*                  iPoint_Size 需要覆盖的内存大小
*  Output:         
*  Return:         0  成功
*                 -1  失败
*  Error:          
*  Others:         
*************************************************/
int Init_Shm_ByKeyStr( char * sKey, int iShmSize, void * pPoint, int iPoint_Size )
{
    int iRet;
    key_t kKey;

    iRet = -1;
    kKey = -1;

    /* 获取key_t类型健值 */
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
*  Description:    获取共享内存映射出来的地址
*  Table Accessed: 
*  Table Updated:  
*  Input:          kKey        key_t类型的健值
*  Output:         
*  Return:         共享内存映射出来的地址
*                  NULL 失败
*  Error:          
*  Others:         这个使用后，还需要shmdt函数卸载掉的
*                  与Dt_Addr_Shm成对使用
*************************************************/
void * Get_Addr_Shm( key_t kKey )
{
    int       iShmId;
    void    * pcLocShmAddr;
    void    * pPoint_Return;
    
    iShmId = -1;
    pcLocShmAddr = NULL;
    pPoint_Return = NULL;
    
    /* 获取共享内存ID */
    iShmId = shmget( (key_t)kKey, 0, 0660 );
    if( iShmId == -1 )
    {
       return NULL;
    }
    
    /* 获取共享内存映射地址 */
    pcLocShmAddr = shmat( iShmId, 0, 0 );
    if( pcLocShmAddr == ( char * ) -1 )
    {
       return NULL;
    }

    /* 直接指针赋值 */
    pPoint_Return = pcLocShmAddr;
    
#if 0
    /* 不能卸载地址，使用Dt_Addr_Shm 在调用Get_Addr_Shm处进行卸载 */
    shmdt(pcLocShmAddr);
#endif

    return pPoint_Return;
}

/*************************************************
*  Function:       Get_Addr_Shm_ByKeyStr
*  Description:    获取共享内存映射出来的地址（健值由字符串转换）
*  Table Accessed: 
*  Table Updated:  
*  Input:          sKey        String类型的健值
*  Output:         
*  Return:         共享内存映射出来的地址
*                  NULL 失败
*  Error:          
*  Others:         这个使用后，还需要shmdt函数卸载掉的
*                  与Dt_Addr_Shm成对使用
*************************************************/
void * Get_Addr_Shm_ByKeyStr( char * sKey )
{

    void    * pcLocShmAddr;
    void    * pPoint_Return;
    key_t     kKey;
    
    pcLocShmAddr = NULL;
    pPoint_Return = NULL;
    kKey = -1;
    
    /* 获取key_t类型健值 */
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
*  Description:    卸载 共享内存映射出来的地址
*  Table Accessed: 
*  Table Updated:  
*  Input:          kKey        key_t类型的健值
*  Output:         
*  Return:         0 成功
*                  -1 失败
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
*  Description:    删除共享内存
*  Table Accessed: 
*  Table Updated:  
*  Input:          kKey        key_t类型的健值
*  Output:         
*  Return:         0  成功
*                  -1 失败
*  Error:          
*  Others:         
*************************************************/
int Del_Shm( key_t kKey )
{
    int iRet;
    int iShmId;

    iRet = -1;
    iShmId = -1;

    /* 获取共享内存ID */
    iShmId = shmget( (key_t)kKey, 0, 0660 );
    if( -1 == iShmId )
    {
       return -1;
    }

    /* 删除共享内存 */
    iRet = shmctl( iShmId, IPC_RMID, NULL );
    if( 0 != iRet )
    {
        return -1;
    }

    return 0;
}


/*************************************************
*  Function:       Del_Shm_ByKeyStr
*  Description:    删除共享内存（健值由字符串转换）
*  Table Accessed: 
*  Table Updated:  
*  Input:          kKey        key_t类型的健值
*  Output:         
*  Return:         0  成功
*                  -1 失败
*  Error:          
*  Others:         
*************************************************/
int Del_Shm_ByKeyStr( char * sKey )
{
    key_t  kKey;
    int    iRet;
    
    kKey = -1;
    iRet = -1;

    /* 获取key_t类型健值 */
    kKey = ftok( sKey, 0 );
    if( -1 == kKey )
    {
        return -1;
    }

    /* 删除共享内存 */
    iRet = Del_Shm( kKey );
    if( -1 == iRet )
    {
        return -1;
    }

    return 0;
}

#endif


#include "lib_jzzh.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

/* ����ת�� */

/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    convert_charto8bit                        *
* Description:    char����ת����8bit��string����            *
*                 ���磺����2 ת���ɵĽ������00000010      *
*                                                           *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             char c ��Ҫת����char                     *
*                                                           *
* Out:            sStr_Point ת���ú󱣴��ڸõ�ַ��         *
*                                                           *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
//char --> 8bit
void convert_charto8bit(char c, char * sStr_Point)
{
    int i = 0;
    char result[8]={0};
    for( i=0;i<8;i++)
    result[i]=((c&(0x80>>i))&&0xff)+0x30;
    //printf( "%s\n", result);
    memcpy( sStr_Point, result, 8 );
}


// char�����ַ� ת���� 8λ��2����λ���ַ�����ʽ��
// �����out_str_bin�ĵ�ַ������Ҫ���ڻ��ߵ���9�����������ڴ����
void Char_To_StrBin( char src_char, char * out_str_bin )
{
    char iTmp = src_char;
    char iTmp_0 = 1;   /* ������ 00000001 */
    char iTmp_1 = 2;   /* ������ 00000010 */
    char iTmp_2 = 4;   /* ������ 00000100 */
    char iTmp_3 = 8;   /* ������ 00001000 */
    char iTmp_4 = 16;  /* ������ 00010000 */
    char iTmp_5 = 32;  /* ������ 00100000 */
    char iTmp_6 = 64;  /* ������ 01000000 */
    char iTmp_7 = 128; /* ������ 10000000 */

    strcpy( out_str_bin + 0, (iTmp&iTmp_7) == 128 ? "1" : "0" );
    strcpy( out_str_bin + 1, (iTmp&iTmp_6) == 64  ? "1" : "0" );
    strcpy( out_str_bin + 2, (iTmp&iTmp_5) == 32  ? "1" : "0" );
    strcpy( out_str_bin + 3, (iTmp&iTmp_4) == 16  ? "1" : "0" );
    strcpy( out_str_bin + 4, (iTmp&iTmp_3) == 8   ? "1" : "0" );
    strcpy( out_str_bin + 5, (iTmp&iTmp_2) == 4   ? "1" : "0" );
    strcpy( out_str_bin + 6, (iTmp&iTmp_1) == 2   ? "1" : "0" );
    strcpy( out_str_bin + 7, (iTmp&iTmp_0) == 1   ? "1" : "0" );
}


//ȡ���char�� ת���� �ַ�����ʽ��2����λ
//iNum_Char    ȥ����char��ת��
void MultipleChar_To_StrBin( char * src_char, char * out_str_bin, int iNum_Char )
{
    char  cTmp = '\0';
    int   iFor = 0;

    for( iFor = 0 ; iFor < iNum_Char; iFor++ )
    {
        cTmp = src_char[iFor];
        Char_To_StrBin( cTmp, out_str_bin + iFor * 8 );
    }
}

// 2����ת����10����
int From_2Jz_To_10Jz( char * s2Jz_Str_In )
{
    char cTmp       = 0;
    int  iOut       = 0;
    char * pPoint_Tmp = NULL;
    int iFor_Num    = 0;
    int ilength_Src = 0;

    ilength_Src = strlen( s2Jz_Str_In );
    pPoint_Tmp = (char *)malloc( ilength_Src + 1 );
    memcpy( pPoint_Tmp, s2Jz_Str_In, ilength_Src );

    for ( iFor_Num = 0; iFor_Num < ilength_Src; iFor_Num++ )
    {
        cTmp = pPoint_Tmp[iFor_Num];
        iOut = iOut*2 + (cTmp-'0');
    }
    return iOut;
}

//8����ת����10����
int  From_8Jz_To_10Jz( char * s8Jz_Str_In )
{
    int r=0;
    while(*s8Jz_Str_In)
    {
        r=r*8+(*s8Jz_Str_In++ -'0');
    }
    return r;
}


//16����ת����10����
//sTmp_Out �ַ�����ʽ��16������
int From_16Jz_To_10Jz( char * sTmp_Out  )
{
    int iFor,iLength_Str_In,iReturn=0,iTmp;

    //ʮ�������ǰ��ַ����������ģ�����Ҫ������ĳ���
    iLength_Str_In = strlen(sTmp_Out);

    for( iFor = 0; iFor < iLength_Str_In; iFor++ )
    {
        //ʮ�����ƻ�Ҫ�ж����ǲ�����A-F����a-f֮��a=10����
        if( sTmp_Out[iFor] >= 'A' && sTmp_Out[iFor] <= 'F' )
        {
            iTmp = sTmp_Out[iFor] - 'A' + 10;
        }
        else if( sTmp_Out[iFor] >= 'a' && sTmp_Out[iFor] <= 'f' )
        {
            iTmp = sTmp_Out[iFor] - 'a' + 10;
        }
        else
        {
            iTmp = sTmp_Out[iFor]-'0';
        }
        iReturn = iReturn * 16 + iTmp;
    }
    return iReturn;
}


// 10����ת����2����
// sBin_Outָ����ڴ泤����Ҫ����32
void From_10Jz_To_2Jz( int iInt_In, char * sBin_Out )
{
    /*
    ����һ��
    void convert(int n)
    {
        if(n > 1)
        {
            convert(n/2);
        } 
        printf("%d",n%2);
    }
    */

    /* �������� */
    char sBin_Tmp[ 8*4 + 1] = {'\0'};
    int  iInt_Tmp  = iInt_In;
    int  iInt_Tmp1 = 0;
    int  iInt_Tmp2 = 1;               /* ������Ϊ 00000000 00000000 00000000 00000001 */
    int  iFor_Count = 0;

    for( iFor_Count = 0; iFor_Count < 32; iFor_Count++ )
    {
        iInt_Tmp1 = iInt_Tmp >> iFor_Count;
        iInt_Tmp1 = iInt_Tmp1 & iInt_Tmp2;
        if ( 0 == iInt_Tmp1 )
        {
            memcpy( &( sBin_Tmp[ 32 - 1 - iFor_Count ] ), "0",1 );
        }
        else
        {
            memcpy( &( sBin_Tmp[ 32 - 1 - iFor_Count ] ), "1",1 );
        }
    }
    memcpy( sBin_Out, sBin_Tmp, strlen( sBin_Tmp ) );
}

//sTmp_Out ����
//i10Jz_Num ��Ҫת���Ľ�������10��������ʾ)
//iJz��Ҫת���Ľ���
//����0�Ļ�����˵����ת��������0
int func_10_Jzzh( char *sTmp_Out, int i10Jz_Num, int iJz )
{
    char sBit[]={"0123456789ABCDEF"};
    int  iLen;
    
    if ( i10Jz_Num == 0 )
    {
        strcpy( sTmp_Out, "" );
        return 0;
    }
    
    func_10_Jzzh( sTmp_Out, i10Jz_Num/iJz, iJz );
    iLen = strlen( sTmp_Out );
    sTmp_Out[iLen] = sBit[i10Jz_Num%iJz];
    sTmp_Out[iLen+1]='\0';
}

//10����ת����8����
void From_10Jz_To_8Jz( int iInt_In, char * s8Jz_Out )
{
    func_10_Jzzh( s8Jz_Out, iInt_In, 8 );
}

//10����ת����16����
void From_10Jz_To_16Jz( int iInt_In, char * s16Jz_Out )
{
    func_10_Jzzh( s16Jz_Out, iInt_In, 16 );
}



#if 0
int main()
{

    char cTmp = 'a';
    char sTmp[100]={'\0'};

    Char_To_StrBin( cTmp, sTmp );
    printf( "%s\n", sTmp );

    char sTmp1[ 100 ];
    memset( sTmp1, 0x00, sizeof(sTmp1) );

    int i = 0;
    for ( i = 0 ;  i < 100 ; i++ )
    {
        memset( sTmp1, 0x00, sizeof(sTmp1) );
        From_10Jz_To_2Jz( i, sTmp1 );
        printf( "%s\n", sTmp1 );
    }

    memset( sTmp1, 0x00, sizeof(sTmp1) );
    From_10Jz_To_2Jz( 4294967294, sTmp1 );
    printf( "%s\n", sTmp1 );

    printf( "%d\n", From_2Jz_To_10Jz( "01111111111111111111111111111111" ) );
    printf( "%d\n", From_2Jz_To_10Jz( "00000000000000000000000000000011" ) );


    char sTmp3[ 100 ];
    memset( sTmp3, 0x00, sizeof( sTmp3 ) );
    From_10Jz_To_16Jz( 35, sTmp3 ); 
    printf( "%s\n", sTmp3 );
    
    memset( sTmp3, 0x00, sizeof( sTmp3 ) );
    From_10Jz_To_8Jz( 35, sTmp3 );
    printf( "%s\n", sTmp3 );


    printf( "%d\n", From_16Jz_To_10Jz( "5F" ) );
    printf( "%d\n", From_8Jz_To_10Jz( "2000" ) );

}
#endif


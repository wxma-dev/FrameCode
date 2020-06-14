/*****************************************************************************/
/*                           TOPCARD                                         */
/*****************************************************************************/
/* PROGRAM NAME: bas_enc_string.c                                            */
/* DESCRIPTIONS: 字符串加/解密                                               */
/* AUTHOR      : Ruan Chao                                                   */
/* CREATE DATE : 2016-02-29                                                  */
/*****************************************************************************/

/* 包涵系统头文件*/
#include <errno.h>
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include "bas/bas_global.h"

/* 包涵公用头文件*/

/* 包含自定义头文件*/

/* 定义标准全局变量*/

/* 定义宏*/
#define   ENCSTRING_FLAG_YES            1
#define   ENCSTRING_FLAG_NO             0

/* 定义模块需要的全局变量*/
const char base[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/="; 

/* 声明一些函数*/
static int encPlsfilInitial(int argc, char *argv[]);
static int encPlsfilProcess(int argc, char *argv[]);
static void encPlsfilEnd(int iFlag);
static int DES_Encrypt(char *caInString, char *keyStr, char *caOutString);
static int DES_Decrypt(char *caInString, char *keyStr, char *caOutString);
static int IsSpace(const char *pcaBuf, int iLen);
int  basEncString(char *caInString, char *caOutString, int iFlag);
void ByteToBit(char ch, char bit[8]);
void BitToByte(char bit[8], char *ch);
void Ch8ToBit64(char ch[8], char bit[64]);
void Bit64ToCh8(char bit[64], char ch[8]);
void DES_SubKey(char key[64], char subKey[16][48]);
void DES_PC1(char key[64], char temp[56]);
void DES_PC2(char key[56], char temp[48]);
void DES_ROL(char data[56], int count);
void DES_IP(char data[64]);
void DES_IP_1(char data[64]);
void DES_E(char data[48]);
void DES_P(char data[32]);
void DES_SBOX(char data[48]);
void DES_XOR(char right[48], char left[48],int count);
void DES_Switch(char left[32], char right[32]);
void DES_EncryptBlock(char plainBlock[8], char subKey[16][48], char cipherBlock[8]);
void DES_DecryptBlock(char cipherBlock[8], char subKey[16][48], char plainBlock[8]);
char *base64_encode(const char* data, int data_len); 
char *base64_decode(const char* data, int data_len); 
static char find_pos(char ch); 

/*****************************************************************************/
/* FUNC:   int basEncString(char *caInString, char *caOutString, int iFlag)  */
/* PARAMS: iFlag  ENCSTRING_FLAG_NO     解密                                 */
/*         iFlag  ENCSTRING_FLAG_YES    加密                                 */
/* RETURN: 0 成功                                                            */
/*          -1 失败                                                          */
/* DESC  : 加解密字符串                                                      */
/*****************************************************************************/
BAS_EXPORT int basEncString(char *caInString, char *caOutString, int iFlag)
{
    int  iEncFlag = ENCSTRING_FLAG_NO; /*加解密标志*/
    int  iRet = 0;
    char caKey[16 + 1];//密钥

    if (strlen(getenv("DBKEY")) == 0)
    { 
        BAS_LOG(BAS_LOGINFO, 0, 0, "encPlsfilProcess:密钥为空");
        return -1;
    }
    char *key = (char *)base64_decode(getenv("DBKEY"), strlen(getenv("DBKEY")));
    memset(caKey, 0, sizeof(caKey));
    sprintf(caKey, "%*.*s", 16, 16, key);

    if(IsSpace(caKey, sizeof(caKey)-1))
    {
        //printf("encPlsfilProcess:密钥为空\n");
        BAS_LOG(BAS_LOGINFO, 0, 0, "encPlsfilProcess:密钥为空");
        return -1;
    }

    iEncFlag = iFlag;

    if(iEncFlag == ENCSTRING_FLAG_YES)
    {
        iRet=DES_Encrypt(caInString, caKey, caOutString);
        if(iRet != 0)
        {
            BAS_LOG(BAS_LOGINFO, 0, 0, "encPlsfil:DES_Encrypt()加密文件出错,caInString=[%s], caInString");
            return -1;
        }
        char *enc = (char *)base64_encode(caOutString, strlen(caOutString));
        memcpy(caOutString, enc, sizeof(enc));
        free(enc);
    }
    else
    {
        char *dec = (char *)base64_decode(caInString, strlen(caInString));
        iRet=DES_Decrypt(dec, caKey, caOutString);
        if(iRet != 0)
        {
            BAS_LOG(BAS_LOGINFO, 0, 0, "encPlsfil:DES_Encrypt()解密文件出错");
            return -1;
        }
        free(dec);
    }

    return 0;
}

/* 加密字符串  */
static int DES_Encrypt(char *caInString, char *keyStr, char *caOutString)
{
    int   count = 0;
    long  len;
    long  iBlock;    /*文件分块数*/
    long  l;
    int   iOdd = 0;  /*末块补余数  */
    char  plainBlock[8], cipherBlock[8], tmpBlock[8], keyLBlock[8], keyRBlock[8];
    char  bitLKey[64], bitRKey[64];
    char  subLKey[16][48], subRKey[16][48];
    char  caSize[14 + 1];

    /* 设置密钥  */
    memcpy(keyLBlock, keyStr, 8);
    memcpy(keyRBlock, keyStr + 8, 8);
    /* 将密钥转换为二进制流  */
    Ch8ToBit64(keyLBlock, bitLKey);
    Ch8ToBit64(keyRBlock, bitRKey);
    /* 生成子密钥  */
    DES_SubKey(bitLKey, subLKey);
    DES_SubKey(bitRKey, subRKey);
    
    len = strlen(caInString);    /* 取长度为len字节的字符串 */
    iBlock = len/8;
    iOdd = len%8;
    sprintf(caSize, "%014d", len);

    for (l=0; l < iBlock; l++)
    {
        /* 每次读8个字节 */
        memcpy(plainBlock, caInString + 8*count, 8);
        DES_EncryptBlock(plainBlock, subLKey, cipherBlock);
        DES_DecryptBlock(cipherBlock, subRKey, tmpBlock);
        DES_EncryptBlock(tmpBlock, subLKey, cipherBlock);
        memcpy(caOutString + 8*count, cipherBlock, 8);
        count++;
    }
    if(iOdd != 0) /* 如果读取的字节数不等于8字节（64bit）*/
    {
        memcpy(plainBlock, caInString + 8*iBlock, 8);
        /* 将剩余的字节填充为'\0'  */
        memset(plainBlock + iOdd, '\0', 8 - iOdd);
        /* 最后一个字节保存填充字节（包括plainBlock[7]）的个数 */
        //plainBlock[7] = 8 - iOdd;
        /* 并进行加密*/
        DES_EncryptBlock(plainBlock, subLKey, cipherBlock);
        DES_DecryptBlock(cipherBlock, subRKey, tmpBlock);
        DES_EncryptBlock(tmpBlock, subLKey, cipherBlock);
        memcpy(caOutString + 8*iBlock, cipherBlock, 8);
    }

    return 0;
}

/* 解密字符串  */
static int DES_Decrypt(char *caInString, char *keyStr, char *caOutString)
{
    int  count = 0;
    int  num = 0;
    long len;
    long iBlock;    /*文件分块数*/
    long l;
    int  iOdd = 0;  /*末块补余数  */
    char plainBlock[8], cipherBlock[8], tmpBlock[8], keyLBlock[8], keyRBlock[8];
    char bitLKey[64], bitRKey[64];
    char subLKey[16][48], subRKey[16][48];
    char caSize[14 + 1];

    /* 设置密钥  */
    memcpy(keyLBlock, keyStr, 8);
    memcpy(keyRBlock, keyStr + 8, 8);
    /* 将密钥转换为二进制流  */
    Ch8ToBit64(keyLBlock, bitLKey);
    Ch8ToBit64(keyRBlock, bitRKey);
    /* 生成子密钥  */
    DES_SubKey(bitLKey, subLKey);
    DES_SubKey(bitRKey, subRKey);

    len = strlen(caInString);    /* 取长度为len字节的字符串 */
    iBlock = len/8;
    iOdd = len%8;
    sprintf(caSize, "%014d", len);

    for (l=0; l < iBlock; l++)
    {
        /* 解密读取的密文和加密的明文相对应，都是8字节*/
        memcpy(cipherBlock, caInString + 8*count, 8);
        DES_DecryptBlock(cipherBlock, subLKey, plainBlock);
        DES_EncryptBlock(plainBlock, subRKey, tmpBlock);
        DES_DecryptBlock(tmpBlock, subLKey, plainBlock);
        memcpy(caOutString + 8*count, plainBlock, 8);
        count++;
    }
    if (iOdd != 0)/*判断密文末尾是否被填充  */
    {
        memcpy(cipherBlock, caInString + 8*count, 8);
        DES_DecryptBlock(cipherBlock, subLKey, plainBlock);
        DES_EncryptBlock(plainBlock, subRKey, tmpBlock);
        DES_DecryptBlock(tmpBlock, subLKey, plainBlock);
        memcpy(caOutString + 8*iBlock, plainBlock, 8);
    }

    return 0;
}



static int IsSpace(const char *pcaBuf, int iLen)
{
    int i;

    if (strlen(pcaBuf) == 0)
    {
        return 1;
    }
    else
    {
        if (strlen(pcaBuf) < iLen )
        {
            iLen = strlen(pcaBuf);
        }
    }

    for (i = 0; i < iLen; i++)
    {
        if (pcaBuf[i] != ' ')
        {
            return 0;
        }
    }
    return 1;
}

// DES加密算法源代码

// 初始置换表IP
int IP[64] = 
{ 57,49,41,33,25,17,9,1,
  59,51,43,35,27,19,11,3,
  61,53,45,37,29,21,13,5,
  63,55,47,39,31,23,15,7,
  56,48,40,32,24,16,8,0,
  58,50,42,34,26,18,10,2,
  60,52,44,36,28,20,12,4,
  62,54,46,38,30,22,14,6 };

// 逆初始置换表IP^-1
int IP_1[64] =
{ 39,7,47,15,55,23,63,31,
  38,6,46,14,54,22,62,30,
  37,5,45,13,53,21,61,29,
  36,4,44,12,52,20,60,28,
  35,3,43,11,51,19,59,27,
  34,2,42,10,50,18,58,26,
  33,1,41,9,49,17,57,25,
  32,0,40,8,48,16,56,24 };

// 扩充置换表E
int E[48] =
{ 31,0,1,2,3,4,
  3,4,5,6,7,8,
  7,8,9,10,11,12,
  11,12,13,14,15,16,
  15,16,17,18,19,20,
  19,20,21,22,23,24,
  23,24,25,26,27,28,
  27,28,29,30,31,0 };

// 置换函数P
int P[32] =
{ 15,6,19,20,28,11,27,16,
  0,14,22,25,4,17,30,9,
  1,7,23,13,31,26,2,8,
  18,12,29,5,21,10,3,24 };

// S盒 
int S[8][4][16] = // 用3维数组保存S1到S8，共8个S盒，每个S盒有4行16列
{
// S1
{{14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
{0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
{4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
{15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}},
// S2
{{15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
{3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
{0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
{13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}},
// S3
{{10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
{13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
{13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
{1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}},
// S4
{{7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
{13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
{10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
{3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}},
// S5
{{2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
{14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
{4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
{11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}},
// S6  
{{12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
{10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
{9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
{4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}},
// S7
{{4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
{13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
{1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
{6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}},
// S8
{{13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
{1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
{7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
{2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}}
};

// 置换选择 PC_1
int PC_1[56] =
{ 56,48,40,32,24,16,8,
  0,57,49,41,33,25,17,
  9,1,58,50,42,34,26,
  18,10,2,59,51,43,35,
  62,54,46,38,30,22,14,
  6,61,53,45,37,29,21,
  13,5,60,52,44,36,28,
  20,12,4,27,19,11,3 };

// 置换选择 PC_2
int PC_2[48] = 
{ 13,16,10,23,0,4,2,27,
  14,5,20,9,22,18,11,3,
  25,7,15,6,26,19,12,1,
  40,51,30,36,46,54,29,39,
  50,44,32,46,43,48,38,55,
  33,52,45,41,49,35,28,31 };

// 对左移次数的规定 LS
int LS[16] = {1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};

// 字节转换成二进制
void ByteToBit(char ch,char bit[8])
{
    int i = 0;
    for(i = 0;i < 8;i++)
    {
        *(bit+7-i) = (ch>>i)&1; // 将字符ch先右移i位，再和00000001相与，得到ch相应的二进制位，分别存放到大小为8的数组bit中
    }
}

// 二进制转换成字节
void BitToByte(char bit[8],char *ch)
{
    int i = 0;
    for(i = 0;i < 8;i++)
    {
        *ch |= *(bit+7-i)<<i;
    }
}

// 将长度为8的字符串转为二进制位串  
void Ch8ToBit64(char ch[8],char bit[64])
{
    int i = 0;
    for(i = 0;i < 8;i++)
    {
        ByteToBit(*(ch+i),bit+(i<<3));  // 将8个字节的字符数组ch[8]变为64位的比特数组bit[64]
    }
}

// 将二进制位串转为长度为8的字符串  
void Bit64ToCh8(char bit[64],char ch[8])
{  
    int i = 0;
    memset(ch,0,8);  // 将想要进行变换后的8字节字符串ch[8]初始化为0
    for(i = 0;i < 8;i++)
    {
        BitToByte(bit+(i<<3),ch+i);  // 将64位的比特数组bit[64]变为8个字节的字符数组ch[8]
    }
}

// 生成子密钥
void DES_SubKey(char key[64],char subKey[16][48])
{
    char temp[56];
    int i = 0;
    DES_PC1(key,temp); // PC1置换
    for(i = 0;i < 16;i++)
    { // 16轮迭代，产生16个子密钥
        DES_ROL(temp,LS[i]); // 循环左移  
        DES_PC2(temp,subKey[i]); // PC2置换，产生子密钥
    }
}

// 密钥置换PC1
void DES_PC1(char key[64],char temp[56])
{
    int i = 0;
    for(i = 0;i < 56;i++)
    {
        temp[i] = key[PC_1[i]];
    }
}

// 密钥置换PC2
void DES_PC2(char key[56],char temp[48])
{
    int i = 0;
    for(i = 0;i < 48;i++)
    {
        temp[i] = key[PC_2[i]];
    }
}

// 循环左移
void DES_ROL(char data[56], int count)
{
    char temp[56];

    // 保存将要循环移动到右边的位
    memcpy(temp,data,count);
    memcpy(temp+count,data+28,count);

    // 左边28位移动
    memcpy(data,data+count,28-count);
    memcpy(data+28-count,temp,count);

    // 右边28位移动
    memcpy(data+28,data+28+count,28-count);
    memcpy(data+56-count,temp+count,count);
}

// IP置换
void DES_IP(char data[64])
{
    int i = 0;
    char temp[64];
    for(i = 0;i < 64;i++)
    {
        temp[i] = data[IP[i]];
    }
    memcpy(data,temp,64);
}

// IP逆置换
void DES_IP_1(char data[64])
{  
    int i = 0;
    char temp[64];
    for(i = 0;i < 64;i++)
    {
        temp[i] = data[IP_1[i]];
    }
    memcpy(data,temp,64);
}

// 扩展置换
void DES_E(char data[48])
{
    int i = 0;
    char temp[48];
    for(i = 0;i < 48;i++)
    {
        temp[i] = data[E[i]];
    }
    memcpy(data,temp,48);
}

// P置换
void DES_P(char data[32])
{
    int i = 0;
    char temp[32];
    for(i = 0;i < 32;i++)
    {  
        temp[i] = data[P[i]];
    }
    memcpy(data,temp,32);
}

// 左部与右部异或
void DES_XOR(char right[48],char left[48] ,int count)
{  
    int i = 0;
    for(i = 0;i < count;i++)
    {
        right[i] ^= left[i];
    }
}

// S盒置换  
void DES_SBOX(char data[48])
{
    int i = 0;
    int j = 0;
    int k = 0;
    int row,column,output;  // S盒中数据的行，列，和输出数据
    for(i = 0;i < 8;i++) // 将48位比特数组data分为8个6位的data分组分别进行S盒置换
    {
        j = i*6;
        k = i<<2;

        // 通过位运算计算在S盒中的行与列
        row = (data[j]<<1)+data[j+5];
        column = (data[j+1]<<3)+(data[j+2]<<2)+(data[j+3]<<1)+data[j+4];
        output = S[i][row][column];

        // 通过位运算将S盒中的数据output化为2进制，存在数组data中
        data[k+0] = (output&8)>>3;
        data[k+1] = (output&4)>>2;
        data[k+2] = (output&2)>>1;
        data[k+3] = (output&1)>>0;
    }
}

// 左部和右部进行交换  
void DES_Switch(char left[32],char right[32])
{  
    char temp[32];
    memcpy(temp,left,32);
    memcpy(left,right,32);
    memcpy(right,temp,32);
}

// 加密单个分组
void DES_EncryptBlock(char plainBlock[8],char subKey[16][48],char cipherBlock[8])
{  
    char plainBits[64];
    char copyRight[48];
    int i = 0;

    Ch8ToBit64(plainBlock,plainBits);
    // 初始置换（IP置换）
    DES_IP(plainBits);

    // 16轮迭代
    for(i = 0;i < 16;i++)
    {
        memcpy(copyRight,plainBits+32,32);  
        // 将右半部分进行扩展置换，从32位扩展到48位
        DES_E(copyRight);  
        // 将右半部分与子密钥进行异或操作
        DES_XOR(copyRight,subKey[i],48);
        // 异或结果进入S盒，输出32位结果
        DES_SBOX(copyRight);
        // P置换
        DES_P(copyRight);  
        // 将明文左半部分与右半部分进行异或
        DES_XOR(plainBits,copyRight,32);
        DES_Switch(plainBits,plainBits+32);  // 利用左右部交换，将R[i-1]赋给L[i]，R[i-1]进行一系列上述操作的结果赋给R[i]
        if(i == 15)
        {
            // 16轮迭代结束后进行最后一次左右部的交换
            DES_Switch(plainBits,plainBits+32);
        }
    }
    // 逆初始置换（IP^1置换）
    DES_IP_1(plainBits);
    Bit64ToCh8(plainBits,cipherBlock);
}

// 解密单个分组
void DES_DecryptBlock(char cipherBlock[8],char subKey[16][48],char plainBlock[8])
{
    char cipherBits[64];
    char copyRight[48];
    int i = 0;  
    
    Ch8ToBit64(cipherBlock,cipherBits);
    // 初始置换（IP置换）
    DES_IP(cipherBits);

    // 16轮迭代（加密的逆操作）
    for(i = 15;i >= 0; i--)
    {
        memcpy(copyRight,cipherBits+32,32);  
        // 将右半部分进行扩展置换，从32位扩展到48位
        DES_E(copyRight);  
        // 将右半部分与子密钥进行异或操作
        DES_XOR(copyRight,subKey[i],48);
        // 异或结果进入S盒，输出32位结果
        DES_SBOX(copyRight);
        // P置换
        DES_P(copyRight);
        // 将明文左半部分与右半部分进行异或
        DES_XOR(cipherBits,copyRight,32);
        DES_Switch(cipherBits,cipherBits+32); // 利用左右部交换，将R[i-1]赋给L[i]，R[i-1]进行一系列上述操作的结果赋给R[i]
        if(i == 0)
        {
            // 16轮迭代结束后进行最后一次左右部的交换
            DES_Switch(cipherBits,cipherBits+32);
        }
    }  
    // 逆初始置换（IP^1置换）
    DES_IP_1(cipherBits);
    Bit64ToCh8(cipherBits,plainBlock);
}
//转码
char *base64_encode(const char* data, int data_len) 
{ 
    int prepare = 0; 
    int ret_len; 
    int temp = 0; 
    char *ret = NULL; 
    char *f = NULL; 
    int tmp = 0; 
    char changed[4]; 
    int i = 0; 
    ret_len = data_len / 3; 
    temp = data_len % 3; 
    if (temp > 0) 
    { 
        ret_len += 1; 
    } 
    ret_len = ret_len*4 + 1; 
    ret = (char *)malloc(ret_len); 
      
    if ( ret == NULL) 
    { 
        printf("No enough memory.\n"); 
        exit(0); 
    } 
    memset(ret, 0, ret_len); 
    f = ret; 
    while (tmp < data_len) 
    { 
        temp = 0; 
        prepare = 0; 
        memset(changed, '\0', 4); 
        while (temp < 3) 
        { 
            if (tmp >= data_len) 
            { 
                break; 
            } 
            prepare = ((prepare << 8) | (data[tmp] & 0xFF)); 
            tmp++; 
            temp++; 
        } 
        prepare = (prepare<<((3-temp)*8)); 
        for (i = 0; i < 4 ;i++ ) 
        { 
            if (temp < i) 
            { 
                changed[i] = 0x40; 
            } 
            else 
            { 
                changed[i] = (prepare>>((3-i)*6)) & 0x3F; 
            } 
            *f = base[changed[i]]; 
            f++; 
        } 
    } 
    *f = '\0'; 
    return ret; 
} 
/* */ 
static char find_pos(char ch)   
{ 
    char *ptr = (char*)strrchr(base, ch);
    return (ptr - base); 
} 
//解码
char *base64_decode(const char *data, int data_len) 
{ 
    int ret_len = (data_len / 4) * 3; 
    int equal_count = 0; 
    char *ret = NULL; 
    char *f = NULL; 
    int tmp = 0; 
    int temp = 0; 
    char need[3]; 
    int prepare = 0; 
    int i = 0; 
    if (*(data + data_len - 1) == '=') 
    { 
        equal_count += 1; 
    } 
    if (*(data + data_len - 2) == '=') 
    { 
        equal_count += 1; 
    } 
    if (*(data + data_len - 3) == '=') 
    {
        equal_count += 1; 
    } 
    switch (equal_count) 
    { 
    case 0: 
        ret_len += 4;
        break; 
    case 1: 
        ret_len += 4;
        break; 
    case 2: 
        ret_len += 3;
        break; 
    case 3: 
        ret_len += 2;
        break; 
    } 
    ret = (char *)malloc(ret_len); 
    if (ret == NULL) 
    { 
        printf("No enough memory.\n"); 
        exit(0); 
    } 
    memset(ret, 0, ret_len); 
    f = ret; 
    while (tmp < (data_len - equal_count)) 
    { 
        temp = 0; 
        prepare = 0; 
        memset(need, 0, 4); 
        while (temp < 4) 
        { 
            if (tmp >= (data_len - equal_count)) 
            { 
                break; 
            } 
            prepare = (prepare << 6) | (find_pos(data[tmp])); 
            temp++; 
            tmp++; 
        } 
        prepare = prepare << ((4-temp) * 6); 
        for (i=0; i<3 ;i++ ) 
        { 
            if (i == temp) 
            { 
                break; 
            } 
            *f = (char)((prepare>>((2-i)*8)) & 0xFF); 
            f++; 
        } 
    } 
    *f = '\0'; 
    return ret; 
}

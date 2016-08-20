#ifndef __MPRSYMMCRYPT_H__
#define __MPRSYMMCRYPT_H__
#include "MPRDeviceEnum.h"

#ifdef __cplusplus
extern "C" {
#endif

#if WIN32
#define	INT_64	__int64
#else
#define	INT_64	long long
#endif
/*
功能：初始化对称加解密
返回值：
        0:           成功
        BAD_PARAM:   传入的参数错误
        BAD_CALL:    错误的调用
*/
int MPRSymmCrypt_Init(const unsigned char *p_key,      //[IN]对称密钥首地址
                      const int            key_len,    //[IN]对称密钥长度
                      const unsigned char *p_iv,       //[IN]初始向量首地址, buff长度16字节, 仅CBC模式有效, 其他模式传NULL
                      const int            is_series,  //[IN]是否连续加解密, 仅ECB模式有效, 1连续加解密, 自动padding; 0非连续加解密, 只能传16的倍数块进行加解密
                      EncryptAlgorithm     alg);       //[IN]对称加密算法及模式, 仅ECB非连续和CTR模式支持随机读写加解密, 其他模式必须从头开始加解密

/*
功能：对称加密
返回值：
        0:           成功
        BAD_PARAM:   传入的参数错误
        BAD_CALL:    未初始化
*/
int MPRSymmCrypt_Encrypt(const unsigned char *p_in_data,      //[IN]需要加密明文首地址
                         const unsigned int   in_len,         //[IN]需要加密明文长度, 除CTR和ECB非连续加密以外, 其余模式都需要在加密最后传入in_len为0, 获取剩余加密数据段;ECB非连续加密只能传16的整数倍
                         const INT_64         offset,         //[IN]首地址在整个需加密的明文中的位置, 仅CTR和ECB模式有效, 其他模式传0, ECB模式offset必须为16的整数倍
                         unsigned char       *p_out_data,     //[OUT]加密后密文首地址, p_out_data指向的buff调用者分配释放, 建议大小为in_len + 16
                         unsigned int        *out_len);       //[IN/OUT]输入buff长度, 输出实际加密后密文长度

/*
功能：对称解密
返回值：
        0:           成功
        BAD_PARAM:   传入的参数错误
        BAD_CALL:    未初始化
*/                       
int MPRSymmCrypt_Decrypt(const unsigned char *p_in_data,      //[IN]需要解密密文首地址
                         const unsigned int   in_len,         //[IN]需要解密密文长度, 除CTR和ECB非连续加密以外, 其余模式都需要在加密最后传入in_len为0, 获取剩余加密数据段;ECB非连续加密只能传16的整数倍
                         const INT_64         offset,         //[IN]首地址在整个需解密密文中的位置, 仅CTR和ECB模式有效, 其他模式传0, ECB模式offset必须为16的整数倍 
                         unsigned char       *p_out_data,     //[OUT]解密后的明文首地址, p_out_data指向的buff调用者分配释放, 建议大小为in_len + 16
                         unsigned int        *out_len);       //[IN/OUT]输入buff长度, 输出实际解密后明文长度

/*
功能：加解密完成清除加解密信息
返回值：
        无
*/ 
void MPRSymmCrypt_Uninit();

/*
功能：初始化对称加解密, 支持多组加解密同时进行
返回值：
        0:           成功
        BAD_PARAM:   传入的参数错误
        BAD_CALL:    错误的调用
*/
int MPRSymmCrypt_InitEx(const unsigned char *p_key,      //[IN]对称密钥首地址
                        const int            key_len,    //[IN]对称密钥长度
                        const unsigned char *p_iv,       //[IN]初始向量首地址, buff长度16字节, 仅CBC模式有效, 其他模式传NULL
                        const int            is_series,  //[IN]是否连续加解密, 仅ECB模式有效, 1连续加解密, 自动padding; 0非连续加解密, 只能传16的倍数块进行加解密
                        EncryptAlgorithm     alg,        //[IN]对称加密算法及模式, 仅ECB非连续和CTR模式支持随机读写加解密, 其他模式必须从头开始加解密
                        void                **p_context);//[OUT]加解密上下文, 用于接下来加解密时传入
/*
功能：对称加密, 支持多组加解密同时进行
返回值：
        0:           成功
        BAD_PARAM:   传入的参数错误
        BAD_CALL:    未初始化
*/
int MPRSymmCrypt_EncryptEx(void                *p_context,      //[IN]加解密上下文
                           const unsigned char *p_in_data,      //[IN]需要加密明文首地址
                           const unsigned int   in_len,         //[IN]需要加密明文长度, 除CTR和ECB非连续加密以外, 其余模式都需要在加密最后传入in_len为0, 获取剩余加密数据段;ECB非连续加密只能传16的整数倍
                           const INT_64         offset,         //[IN]首地址在整个需加密的明文中的位置, 仅CTR和ECB模式有效, 其他模式传0, ECB模式offset必须为16的整数倍
                           unsigned char       *p_out_data,     //[OUT]加密后密文首地址, p_out_data指向的buff调用者分配释放, 建议大小为in_len + 16
                           unsigned int        *out_len);       //[IN/OUT]输入buff长度, 输出实际加密后密文长度

/*
功能：对称解密, 支持多组加解密同时进行
返回值：
        0:           成功
        BAD_PARAM:   传入的参数错误
        BAD_CALL:    未初始化
*/                       
int MPRSymmCrypt_DecryptEx(void                *p_context,      //[IN]加解密上下文
                           const unsigned char *p_in_data,      //[IN]需要解密密文首地址
                           const unsigned int   in_len,         //[IN]需要解密密文长度, 除CTR和ECB非连续加密以外, 其余模式都需要在加密最后传入in_len为0, 获取剩余加密数据段;ECB非连续加密只能传16的整数倍
                           const INT_64         offset,         //[IN]首地址在整个需解密密文中的位置, 仅CTR和ECB模式有效, 其他模式传0, ECB模式offset必须为16的整数倍 
                           unsigned char       *p_out_data,     //[OUT]解密后的明文首地址, p_out_data指向的buff调用者分配释放, 建议大小为in_len + 16
                           unsigned int        *out_len);       //[IN/OUT]输入buff长度, 输出实际解密后明文长度

/*
功能：加解密完成清除加解密信息, 支持多组加解密同时进行
返回值：
        无
*/ 
void MPRSymmCrypt_UninitEx(void **p_context);                      //[IN]加解密上下文

#ifdef __cplusplus
}
#endif

#endif

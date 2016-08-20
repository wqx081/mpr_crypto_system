#ifndef __MPRASYMMCRYPT_H__
#define __MPRASYMMCRYPT_H__
#include "MPRDeviceEnum.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
功能：初始化非对称加解密
返回值：
        0:           成功
*/
int MPRAsymmCrypt_Init();       
/*
功能：生成非对称密钥对
返回值：
        0:           成功
        BAD_PARAM:   传入的参数错误
        BAD_CALL:    未初始化
*/

int MPRAsymmCrypt_GenerateKeyPair(PKI_Algorithm        alg,            //[IN]非对称加密算法
                                  const unsigned int   key_bits,       //[IN]加密算法长度, SM2算法192或256, RSA算法1024或2048                                 const unsigned char *p_in_data,      //[IN]需要加密明文首地址
                                        unsigned char  *p_pubkey,      //[IN]公钥首地址
                                        unsigned int   *pubkey_len,    //[IN/OUT]输入buff长度, 返回公钥长度,建议key_bits/8
                                        unsigned char  *p_prvkey,      //[IN]私钥首地址
                                        unsigned int   *prvkey_len);   //[OUT]输入buff长度, 返回私钥长度,建议key_bits/16


int MPRAsymmCrypt_PubEncrypt(PKI_Algorithm        alg,            //[IN]非对称加密算法
                             const unsigned char *p_pubkey,       //[IN]公钥首地址
                             const unsigned int   pubkey_len,     //[IN]公钥长度
                             const unsigned char *p_in_data,      //[IN]需要加密明文首地址
                             const unsigned int   in_len,         //[IN]需要加密明文长度, 长度1到32字节
                             unsigned char       *p_out_data,     //[OUT]加密后密文首地址, p_out_data指向的buff调用者分配释放, 建议大小为in_len + 16
                             unsigned int        *out_len);       //[IN/OUT]输入buff长度, 输出实际加密后密文长度

/*
功能：非对称私钥解密
返回值：
        0:           成功
        BAD_PARAM:   传入的参数错误
        BAD_CALL:    未初始化
*/                       
int MPRAsymmCrypt_PrvDecrypt(PKI_Algorithm        alg,            //[IN]非对称加密算法
                             const unsigned char *p_prvkey,       //[IN]私钥首地址
                             const unsigned int   prvkey_len,     //[IN]私钥长度
                             const unsigned char *p_in_data,      //[IN]需要解密密文首地址
                             const unsigned int   in_len,         //[IN]需要解密密文长度
                             unsigned char       *p_out_data,     //[OUT]解密后的明文首地址, p_out_data指向的buff调用者分配释放, 建议大小为in_len + 16
                             unsigned int        *out_len);       //[IN/OUT]输入buff长度, 输出实际解密后明文长度
                                                                  //对128字节密文解密后的结果为32字节, 112字节密文解密后的结果为16字节,

/*
功能：签名
返回值：
        0:           成功
        BAD_PARAM:   传入的参数错误
        BAD_CALL:    未初始化
*/                       
int MPRAsymmCrypt_Sign(PKI_Algorithm        alg,            //[IN]非对称加密算法
                       const unsigned char *p_pubkey,       //[IN]公钥首地址
                       const unsigned int   pubkey_len,     //[IN]公钥长度
                       const unsigned char *p_prvkey,       //[IN]私钥首地址
                       const unsigned int   prvkey_len,     //[IN]私钥长度
                       const unsigned char *p_in_data,      //[IN]签名MSG首地址
                       const unsigned int   in_len,         //[IN]签名MSG长度
                       const unsigned char *p_sign_id,      //[IN]签名ID首地址
                       const unsigned int   sign_len,       //[IN]签名ID长度
                       unsigned char       *p_signature,    //[OUT]签名数据, p_out_data指向的buff调用者分配释放, 建议大小为64
                       unsigned int        *signature_len); //[IN/OUT]输入buff长度, 输出实际签名长度

/*
功能：验签
返回值：
        0:           成功
        BAD_PARAM:   传入的参数错误
        BAD_CALL:    未初始化
*/                       
int MPRAsymmCrypt_Verify(PKI_Algorithm        alg,            //[IN]非对称加密算法
                          const unsigned char *p_pubkey,       //[IN]公钥首地址
                          const unsigned int   pubkey_len,     //[IN]公钥长度
                          const unsigned char *p_in_data,      //[IN]签名MSG首地址
                          const unsigned int   in_len,         //[IN]签名MSG长度
                          const unsigned char *p_sign_id,      //[IN]签名ID首地址
                          const unsigned int   sign_len,       //[IN]签名ID长度
                          unsigned char       *p_signature,    //[IN]签名数据
                          unsigned int         signature_len); //[IN]输入buff长度
    
/*
功能：加解密完成清除加解密信息
返回值：
        无
*/ 
int MPRAsymmCrypt_Uninit();

#ifdef __cplusplus
}
#endif

#endif
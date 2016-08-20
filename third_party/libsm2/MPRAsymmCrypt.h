#ifndef __MPRASYMMCRYPT_H__
#define __MPRASYMMCRYPT_H__
#include "MPRDeviceEnum.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
���ܣ���ʼ���ǶԳƼӽ���
����ֵ��
        0:           �ɹ�
*/
int MPRAsymmCrypt_Init();       
/*
���ܣ����ɷǶԳ���Կ��
����ֵ��
        0:           �ɹ�
        BAD_PARAM:   ����Ĳ�������
        BAD_CALL:    δ��ʼ��
*/

int MPRAsymmCrypt_GenerateKeyPair(PKI_Algorithm        alg,            //[IN]�ǶԳƼ����㷨
                                  const unsigned int   key_bits,       //[IN]�����㷨����, SM2�㷨192��256, RSA�㷨1024��2048                                 const unsigned char *p_in_data,      //[IN]��Ҫ���������׵�ַ
                                        unsigned char  *p_pubkey,      //[IN]��Կ�׵�ַ
                                        unsigned int   *pubkey_len,    //[IN/OUT]����buff����, ���ع�Կ����,����key_bits/8
                                        unsigned char  *p_prvkey,      //[IN]˽Կ�׵�ַ
                                        unsigned int   *prvkey_len);   //[OUT]����buff����, ����˽Կ����,����key_bits/16


int MPRAsymmCrypt_PubEncrypt(PKI_Algorithm        alg,            //[IN]�ǶԳƼ����㷨
                             const unsigned char *p_pubkey,       //[IN]��Կ�׵�ַ
                             const unsigned int   pubkey_len,     //[IN]��Կ����
                             const unsigned char *p_in_data,      //[IN]��Ҫ���������׵�ַ
                             const unsigned int   in_len,         //[IN]��Ҫ�������ĳ���, ����1��32�ֽ�
                             unsigned char       *p_out_data,     //[OUT]���ܺ������׵�ַ, p_out_dataָ���buff�����߷����ͷ�, �����СΪin_len + 16
                             unsigned int        *out_len);       //[IN/OUT]����buff����, ���ʵ�ʼ��ܺ����ĳ���

/*
���ܣ��ǶԳ�˽Կ����
����ֵ��
        0:           �ɹ�
        BAD_PARAM:   ����Ĳ�������
        BAD_CALL:    δ��ʼ��
*/                       
int MPRAsymmCrypt_PrvDecrypt(PKI_Algorithm        alg,            //[IN]�ǶԳƼ����㷨
                             const unsigned char *p_prvkey,       //[IN]˽Կ�׵�ַ
                             const unsigned int   prvkey_len,     //[IN]˽Կ����
                             const unsigned char *p_in_data,      //[IN]��Ҫ���������׵�ַ
                             const unsigned int   in_len,         //[IN]��Ҫ�������ĳ���
                             unsigned char       *p_out_data,     //[OUT]���ܺ�������׵�ַ, p_out_dataָ���buff�����߷����ͷ�, �����СΪin_len + 16
                             unsigned int        *out_len);       //[IN/OUT]����buff����, ���ʵ�ʽ��ܺ����ĳ���
                                                                  //��128�ֽ����Ľ��ܺ�Ľ��Ϊ32�ֽ�, 112�ֽ����Ľ��ܺ�Ľ��Ϊ16�ֽ�,

/*
���ܣ�ǩ��
����ֵ��
        0:           �ɹ�
        BAD_PARAM:   ����Ĳ�������
        BAD_CALL:    δ��ʼ��
*/                       
int MPRAsymmCrypt_Sign(PKI_Algorithm        alg,            //[IN]�ǶԳƼ����㷨
                       const unsigned char *p_pubkey,       //[IN]��Կ�׵�ַ
                       const unsigned int   pubkey_len,     //[IN]��Կ����
                       const unsigned char *p_prvkey,       //[IN]˽Կ�׵�ַ
                       const unsigned int   prvkey_len,     //[IN]˽Կ����
                       const unsigned char *p_in_data,      //[IN]ǩ��MSG�׵�ַ
                       const unsigned int   in_len,         //[IN]ǩ��MSG����
                       const unsigned char *p_sign_id,      //[IN]ǩ��ID�׵�ַ
                       const unsigned int   sign_len,       //[IN]ǩ��ID����
                       unsigned char       *p_signature,    //[OUT]ǩ������, p_out_dataָ���buff�����߷����ͷ�, �����СΪ64
                       unsigned int        *signature_len); //[IN/OUT]����buff����, ���ʵ��ǩ������

/*
���ܣ���ǩ
����ֵ��
        0:           �ɹ�
        BAD_PARAM:   ����Ĳ�������
        BAD_CALL:    δ��ʼ��
*/                       
int MPRAsymmCrypt_Verify(PKI_Algorithm        alg,            //[IN]�ǶԳƼ����㷨
                          const unsigned char *p_pubkey,       //[IN]��Կ�׵�ַ
                          const unsigned int   pubkey_len,     //[IN]��Կ����
                          const unsigned char *p_in_data,      //[IN]ǩ��MSG�׵�ַ
                          const unsigned int   in_len,         //[IN]ǩ��MSG����
                          const unsigned char *p_sign_id,      //[IN]ǩ��ID�׵�ַ
                          const unsigned int   sign_len,       //[IN]ǩ��ID����
                          unsigned char       *p_signature,    //[IN]ǩ������
                          unsigned int         signature_len); //[IN]����buff����
    
/*
���ܣ��ӽ����������ӽ�����Ϣ
����ֵ��
        ��
*/ 
int MPRAsymmCrypt_Uninit();

#ifdef __cplusplus
}
#endif

#endif
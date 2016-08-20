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
���ܣ���ʼ���ԳƼӽ���
����ֵ��
        0:           �ɹ�
        BAD_PARAM:   ����Ĳ�������
        BAD_CALL:    ����ĵ���
*/
int MPRSymmCrypt_Init(const unsigned char *p_key,      //[IN]�Գ���Կ�׵�ַ
                      const int            key_len,    //[IN]�Գ���Կ����
                      const unsigned char *p_iv,       //[IN]��ʼ�����׵�ַ, buff����16�ֽ�, ��CBCģʽ��Ч, ����ģʽ��NULL
                      const int            is_series,  //[IN]�Ƿ������ӽ���, ��ECBģʽ��Ч, 1�����ӽ���, �Զ�padding; 0�������ӽ���, ֻ�ܴ�16�ı�������мӽ���
                      EncryptAlgorithm     alg);       //[IN]�ԳƼ����㷨��ģʽ, ��ECB��������CTRģʽ֧�������д�ӽ���, ����ģʽ�����ͷ��ʼ�ӽ���

/*
���ܣ��ԳƼ���
����ֵ��
        0:           �ɹ�
        BAD_PARAM:   ����Ĳ�������
        BAD_CALL:    δ��ʼ��
*/
int MPRSymmCrypt_Encrypt(const unsigned char *p_in_data,      //[IN]��Ҫ���������׵�ַ
                         const unsigned int   in_len,         //[IN]��Ҫ�������ĳ���, ��CTR��ECB��������������, ����ģʽ����Ҫ�ڼ��������in_lenΪ0, ��ȡʣ��������ݶ�;ECB����������ֻ�ܴ�16��������
                         const INT_64         offset,         //[IN]�׵�ַ����������ܵ������е�λ��, ��CTR��ECBģʽ��Ч, ����ģʽ��0, ECBģʽoffset����Ϊ16��������
                         unsigned char       *p_out_data,     //[OUT]���ܺ������׵�ַ, p_out_dataָ���buff�����߷����ͷ�, �����СΪin_len + 16
                         unsigned int        *out_len);       //[IN/OUT]����buff����, ���ʵ�ʼ��ܺ����ĳ���

/*
���ܣ��Գƽ���
����ֵ��
        0:           �ɹ�
        BAD_PARAM:   ����Ĳ�������
        BAD_CALL:    δ��ʼ��
*/                       
int MPRSymmCrypt_Decrypt(const unsigned char *p_in_data,      //[IN]��Ҫ���������׵�ַ
                         const unsigned int   in_len,         //[IN]��Ҫ�������ĳ���, ��CTR��ECB��������������, ����ģʽ����Ҫ�ڼ��������in_lenΪ0, ��ȡʣ��������ݶ�;ECB����������ֻ�ܴ�16��������
                         const INT_64         offset,         //[IN]�׵�ַ����������������е�λ��, ��CTR��ECBģʽ��Ч, ����ģʽ��0, ECBģʽoffset����Ϊ16�������� 
                         unsigned char       *p_out_data,     //[OUT]���ܺ�������׵�ַ, p_out_dataָ���buff�����߷����ͷ�, �����СΪin_len + 16
                         unsigned int        *out_len);       //[IN/OUT]����buff����, ���ʵ�ʽ��ܺ����ĳ���

/*
���ܣ��ӽ����������ӽ�����Ϣ
����ֵ��
        ��
*/ 
void MPRSymmCrypt_Uninit();

/*
���ܣ���ʼ���ԳƼӽ���, ֧�ֶ���ӽ���ͬʱ����
����ֵ��
        0:           �ɹ�
        BAD_PARAM:   ����Ĳ�������
        BAD_CALL:    ����ĵ���
*/
int MPRSymmCrypt_InitEx(const unsigned char *p_key,      //[IN]�Գ���Կ�׵�ַ
                        const int            key_len,    //[IN]�Գ���Կ����
                        const unsigned char *p_iv,       //[IN]��ʼ�����׵�ַ, buff����16�ֽ�, ��CBCģʽ��Ч, ����ģʽ��NULL
                        const int            is_series,  //[IN]�Ƿ������ӽ���, ��ECBģʽ��Ч, 1�����ӽ���, �Զ�padding; 0�������ӽ���, ֻ�ܴ�16�ı�������мӽ���
                        EncryptAlgorithm     alg,        //[IN]�ԳƼ����㷨��ģʽ, ��ECB��������CTRģʽ֧�������д�ӽ���, ����ģʽ�����ͷ��ʼ�ӽ���
                        void                **p_context);//[OUT]�ӽ���������, ���ڽ������ӽ���ʱ����
/*
���ܣ��ԳƼ���, ֧�ֶ���ӽ���ͬʱ����
����ֵ��
        0:           �ɹ�
        BAD_PARAM:   ����Ĳ�������
        BAD_CALL:    δ��ʼ��
*/
int MPRSymmCrypt_EncryptEx(void                *p_context,      //[IN]�ӽ���������
                           const unsigned char *p_in_data,      //[IN]��Ҫ���������׵�ַ
                           const unsigned int   in_len,         //[IN]��Ҫ�������ĳ���, ��CTR��ECB��������������, ����ģʽ����Ҫ�ڼ��������in_lenΪ0, ��ȡʣ��������ݶ�;ECB����������ֻ�ܴ�16��������
                           const INT_64         offset,         //[IN]�׵�ַ����������ܵ������е�λ��, ��CTR��ECBģʽ��Ч, ����ģʽ��0, ECBģʽoffset����Ϊ16��������
                           unsigned char       *p_out_data,     //[OUT]���ܺ������׵�ַ, p_out_dataָ���buff�����߷����ͷ�, �����СΪin_len + 16
                           unsigned int        *out_len);       //[IN/OUT]����buff����, ���ʵ�ʼ��ܺ����ĳ���

/*
���ܣ��Գƽ���, ֧�ֶ���ӽ���ͬʱ����
����ֵ��
        0:           �ɹ�
        BAD_PARAM:   ����Ĳ�������
        BAD_CALL:    δ��ʼ��
*/                       
int MPRSymmCrypt_DecryptEx(void                *p_context,      //[IN]�ӽ���������
                           const unsigned char *p_in_data,      //[IN]��Ҫ���������׵�ַ
                           const unsigned int   in_len,         //[IN]��Ҫ�������ĳ���, ��CTR��ECB��������������, ����ģʽ����Ҫ�ڼ��������in_lenΪ0, ��ȡʣ��������ݶ�;ECB����������ֻ�ܴ�16��������
                           const INT_64         offset,         //[IN]�׵�ַ����������������е�λ��, ��CTR��ECBģʽ��Ч, ����ģʽ��0, ECBģʽoffset����Ϊ16�������� 
                           unsigned char       *p_out_data,     //[OUT]���ܺ�������׵�ַ, p_out_dataָ���buff�����߷����ͷ�, �����СΪin_len + 16
                           unsigned int        *out_len);       //[IN/OUT]����buff����, ���ʵ�ʽ��ܺ����ĳ���

/*
���ܣ��ӽ����������ӽ�����Ϣ, ֧�ֶ���ӽ���ͬʱ����
����ֵ��
        ��
*/ 
void MPRSymmCrypt_UninitEx(void **p_context);                      //[IN]�ӽ���������

#ifdef __cplusplus
}
#endif

#endif

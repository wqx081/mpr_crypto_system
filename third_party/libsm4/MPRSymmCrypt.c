#include <stdlib.h>
#include "MPRSymmCrypt.h"
#include "cryptlib.h"

static cipher_context ctx_encypt;
static cipher_context ctx_decypt;
static int __is_series = 0;

static int SymmAlgConvert(EncryptAlgorithm alg)
{
    switch(alg)
    {
    case AES_CBC:
        return ALG_AES_CBC;
    case AES_CTR:
        return ALG_AES_CTR;
    case AES_ECB:
        return ALG_AES_ECB;
    case SM4_CBC:
        return ALG_SM4_CBC;
    case SM4_CTR:
        return ALG_SM4_CTR;
    case SM4_ECB:
        return ALG_SM4_ECB;
    }
    return ALG_AES_ECB;
}

int MPRSymmCrypt_Init(const unsigned char *p_key,       //[IN]�Գ���Կ�׵�ַ
                      const int            key_len,     //[IN]�Գ���Կ����
                      const unsigned char *p_iv,        //[IN]��ʼ�����׵�ַ, buff����16�ֽ�, ��CBCģʽ��Ч, ����ģʽ��NULL
                      const int            is_series,   //[IN]�Ƿ������ӽ���, ��ECBģʽ��Ч, 1�����ӽ���, �Զ�padding; 0�������ӽ���, ֻ�ܴ�16�ı�������мӽ���
                      EncryptAlgorithm     alg)         //[IN]�ԳƼ����㷨��ģʽ, ��ECB��������CTRģʽ֧�������д�ӽ���, ����ģʽ�����ͷ��ʼ�ӽ���

{
    if(p_key == NULL || key_len > MAX_KEY_LENGTH){
        return BAD_PARAM;
    }

    if(alg == AES_CBC || alg == SM4_CBC){
        if(p_iv == NULL){
            return BAD_PARAM;
        }
    }

    if(alg == AES_ECB || alg == SM4_ECB)
    {
        if(is_series == 0 || is_series == 1)
        {
            __is_series = is_series;
        }
        else 
        {
            return BAD_PARAM;
        }
    }

    if(!crypt_enc_init(&ctx_encypt, (uint8 *)p_key, (int)key_len,(uint8 *)p_iv, SymmAlgConvert(alg))){
        return BAD_CALL;
    }
    if(!crypt_dec_init(&ctx_decypt, (uint8 *)p_key, (int)key_len,(uint8 *)p_iv, SymmAlgConvert(alg))){
        return BAD_CALL;
    }

    return 0;
}

int MPRSymmCrypt_Encrypt(const unsigned char *p_in_data,      //[IN]��Ҫ���������׵�ַ
                         const unsigned int   in_len,         //[IN]��Ҫ�������ĳ���, ��CTR��ECB��������������, ����ģʽ����Ҫ�ڼ��������in_lenΪ0, ��ȡʣ��������ݶ�;ECB����������ֻ�ܴ�16��������
                         const INT_64         offset,         //[IN]�׵�ַ����������ܵ������е�λ��, ��CTR��ECBģʽ��Ч, ����ģʽ��0, ECBģʽoffset����Ϊ16��������
                         unsigned char       *p_out_data,     //[OUT]���ܺ������׵�ַ, p_out_dataָ���buff�����߷����ͷ�, �����СΪin_len + 16
                         unsigned int        *out_len)        //[IN/OUT]����outbuff����, ���ʵ�ʼ��ܺ����ĳ���
{
    if(p_out_data == NULL || out_len == NULL){
        return BAD_PARAM;
    }

    if(p_in_data == NULL && in_len != 0){
        return BAD_PARAM;
    }

    if(ctx_encypt.alg == 0)
    {
        return BAD_CALL;
    }

    if(in_len > *out_len)
    {
        return BAD_PARAM;
    }

    switch(ctx_encypt.alg)
    {
    case ALG_SM4_CTR:
    case ALG_AES_CTR:
        if(crypt_ctr_encrypt(&ctx_encypt, (unsigned char*)p_in_data, p_out_data, in_len, offset))
        {
            *out_len = in_len;
            return 0;
        }
        break;
    case ALG_AES_CBC:
    case ALG_SM4_CBC:
update_enc:
        if(in_len > 0)
        {
            if(crypt_enc_update(&ctx_encypt, (unsigned char*)p_in_data, in_len, p_out_data, out_len))
            {
                return 0;
            }
        }
        else
        {
            if(*out_len < 16)
            {
                return BAD_PARAM;
            }
            if(crypt_enc_final(&ctx_encypt, p_out_data, (int *)out_len))
            {
                return 0;
            }
        }

        break;
    case ALG_AES_ECB:
    case ALG_SM4_ECB:
        if(__is_series == 0)
        {
            if(in_len % 16 != 0 || offset % 16 != 0)
            {
                return BAD_PARAM;
            }
            if(crypt_enc_block(&ctx_encypt, (unsigned char *)p_in_data, p_out_data, in_len))
            {
                *out_len = in_len;
                return 0;
            }
        }
        else
        {
            goto update_enc;
        }
        break;
    }

    return BAD_CALL;
}

int MPRSymmCrypt_Decrypt(const unsigned char *p_in_data,      //[IN]��Ҫ���������׵�ַ
                         const unsigned int   in_len,         //[IN]��Ҫ�������ĳ���, ����ECBģʽ, ���ȱ���Ϊ16��������
                         const INT_64         offset,         //[IN]�׵�ַ����������������е�λ��, ��CTRģʽ��Ч, ����ģʽ��0
                         unsigned char       *p_out_data,     //[OUT]���ܺ�������׵�ַ
                         unsigned int        *out_len)        //[OUT]���ܺ�����ĳ���
{
    if(p_out_data == NULL || out_len == NULL){
        return BAD_PARAM;
    }

    if(p_in_data == NULL && in_len != 0){
        return BAD_PARAM;
    }

    if(ctx_decypt.alg == 0)
    {
        return BAD_CALL;
    }
    if(in_len > *out_len)
    {
        return BAD_PARAM;
    }

    switch(ctx_decypt.alg)
    {
    case ALG_AES_CTR:
    case ALG_SM4_CTR:
        if(crypt_ctr_decrypt(&ctx_decypt, (unsigned char*)p_in_data, p_out_data, in_len, offset))
        {
            *out_len = in_len;
            return 0;
        }
        break;
    case ALG_AES_CBC:
    case ALG_SM4_CBC:
update_dec:
        if(in_len > 0)
        {
            if(crypt_dec_update(&ctx_decypt, (unsigned char*)p_in_data, in_len, p_out_data, out_len))
            {
                return 0;
            }
        }
        else
        {
            if(*out_len < 16)
            {
                return BAD_PARAM;
            }
            if(crypt_dec_final(&ctx_decypt, p_out_data, (int *)out_len))
            {
                return 0;
            }
        }

        break;
    case ALG_AES_ECB:
    case ALG_SM4_ECB:
        if(__is_series == 0)
        {
            if(in_len % 16 != 0 || offset % 16 != 0)
            {
                return BAD_PARAM;
            }
            if(crypt_dec_block(&ctx_decypt, (unsigned char *)p_in_data, p_out_data, in_len))
            {
                *out_len = in_len;
                return 0;
            }
        }
        else
        {
            goto update_dec;
        }
        break;
    }

    return BAD_CALL;
}

void MPRSymmCrypt_Uninit()
{
    crypt_enc_end(&ctx_encypt);
    crypt_dec_end(&ctx_decypt);
    
}

typedef struct
{
    cipher_context ctx_encypt;
    cipher_context ctx_decypt;
    int is_series;
}MPRSymm_Context;

int MPRSymmCrypt_InitEx(const unsigned char *p_key,      //[IN]�Գ���Կ�׵�ַ
                        const int            key_len,    //[IN]�Գ���Կ����
                        const unsigned char *p_iv,       //[IN]��ʼ�����׵�ַ, buff����16�ֽ�, ��CBCģʽ��Ч, ����ģʽ��NULL
                        const int            is_series,  //[IN]�Ƿ������ӽ���, ��ECBģʽ��Ч, 1�����ӽ���, �Զ�padding; 0�������ӽ���, ֻ�ܴ�16�ı�������мӽ���
                        EncryptAlgorithm     alg,        //[IN]�ԳƼ����㷨��ģʽ, ��ECB��������CTRģʽ֧�������д�ӽ���, ����ģʽ�����ͷ��ʼ�ӽ���
                        void                **p_context) //[OUT]�ӽ���������, ���ڽ������ӽ���ʱ����
{
    MPRSymm_Context *ctx_tmp = NULL;
    if(p_key == NULL || key_len > MAX_KEY_LENGTH || p_context == NULL){
        return BAD_PARAM;
    }

    if(alg == AES_CBC || alg == SM4_CBC){
        if(p_iv == NULL){
            return BAD_PARAM;
        }
    }

    if(alg == AES_ECB || alg == SM4_ECB)
    {
        if(is_series != 0 && is_series != 1)
        {
            return BAD_PARAM;
        }
    }

    *p_context = malloc(sizeof(MPRSymm_Context));
    if(*p_context == NULL)
    {
        return SYS_ERROR;
    }

    ctx_tmp = *p_context;
    ctx_tmp->is_series = is_series;

    if(!crypt_enc_init(&ctx_tmp->ctx_encypt, (uint8 *)p_key, (int)key_len,(uint8 *)p_iv, SymmAlgConvert(alg))){
        free(*p_context);
        *p_context = NULL;
        return BAD_CALL;
    }
    if(!crypt_dec_init(&ctx_tmp->ctx_decypt, (uint8 *)p_key, (int)key_len,(uint8 *)p_iv, SymmAlgConvert(alg))){
        free(*p_context);
        *p_context = NULL;
        return BAD_CALL;
    }

    return 0;
}

int MPRSymmCrypt_EncryptEx(void                *p_context,        //[IN]�ӽ���������
                           const unsigned char *p_in_data,        //[IN]��Ҫ���������׵�ַ
                           const unsigned int   in_len,           //[IN]��Ҫ�������ĳ���, ��CTR��ECB��������������, ����ģʽ����Ҫ�ڼ��������in_lenΪ0, ��ȡʣ��������ݶ�;ECB����������ֻ�ܴ�16��������
                           const INT_64         offset,           //[IN]�׵�ַ����������ܵ������е�λ��, ��CTR��ECBģʽ��Ч, ����ģʽ��0, ECBģʽoffset����Ϊ16��������
                           unsigned char       *p_out_data,       //[OUT]���ܺ������׵�ַ, p_out_dataָ���buff�����߷����ͷ�, �����СΪin_len + 16
                           unsigned int        *out_len)          //[IN/OUT]����buff����, ���ʵ�ʼ��ܺ����ĳ���
{
    MPRSymm_Context *ctx_tmp = NULL;
    if(p_out_data == NULL || out_len == NULL || p_context == NULL){
        return BAD_PARAM;
    }

    if(p_in_data == NULL && in_len != 0){
        return BAD_PARAM;
    }
    ctx_tmp = (MPRSymm_Context *)p_context;
    if(ctx_tmp->ctx_encypt.alg == 0)
    {
        return BAD_CALL;
    }

    if(in_len > *out_len)
    {
        return BAD_PARAM;
    }

    switch(ctx_tmp->ctx_encypt.alg)
    {
    case ALG_SM4_CTR:
    case ALG_AES_CTR:
        if(crypt_ctr_encrypt(&ctx_tmp->ctx_encypt, (unsigned char*)p_in_data, p_out_data, in_len, offset))
        {
            *out_len = in_len;
            return 0;
        }
        break;
    case ALG_AES_CBC:
    case ALG_SM4_CBC:
update_enc_ex:
        if(in_len > 0)
        {
            if(crypt_enc_update(&ctx_tmp->ctx_encypt, (unsigned char*)p_in_data, in_len, p_out_data, out_len))
            {
                return 0;
            }
        }
        else
        {
            if(*out_len < 16)
            {
                return BAD_PARAM;
            }
            if(crypt_enc_final(&ctx_tmp->ctx_encypt, p_out_data, (int *)out_len))
            {
                return 0;
            }
        }

        break;
    case ALG_AES_ECB:
    case ALG_SM4_ECB:
        if(ctx_tmp->is_series == 0)
        {
            if(in_len % 16 != 0 || offset % 16 != 0)
            {
                return BAD_PARAM;
            }
            if(crypt_enc_block(&ctx_tmp->ctx_encypt, (unsigned char *)p_in_data, p_out_data, in_len))
            {
                *out_len = in_len;
                return 0;
            }
        }
        else
        {
            goto update_enc_ex;
        }
        break;
    }

    return BAD_CALL;
}

int MPRSymmCrypt_DecryptEx(void                *p_context,        //[IN]�ӽ���������
                           const unsigned char *p_in_data,        //[IN]��Ҫ���������׵�ַ
                           const unsigned int   in_len,           //[IN]��Ҫ�������ĳ���, ��CTR��ECB��������������, ����ģʽ����Ҫ�ڼ��������in_lenΪ0, ��ȡʣ��������ݶ�;ECB����������ֻ�ܴ�16��������
                           const INT_64         offset,           //[IN]�׵�ַ����������������е�λ��, ��CTR��ECBģʽ��Ч, ����ģʽ��0, ECBģʽoffset����Ϊ16�������� 
                           unsigned char       *p_out_data,       //[OUT]���ܺ�������׵�ַ, p_out_dataָ���buff�����߷����ͷ�, �����СΪin_len + 16
                           unsigned int        *out_len)          //[IN/OUT]����buff����, ���ʵ�ʽ��ܺ����ĳ���
{
    MPRSymm_Context *ctx_tmp = NULL;
    if(p_out_data == NULL || out_len == NULL || p_context == NULL){
        return BAD_PARAM;
    }

    if(p_in_data == NULL && in_len != 0){
        return BAD_PARAM;
    }

    ctx_tmp = (MPRSymm_Context *)p_context;
    if(ctx_tmp->ctx_decypt.alg == 0)
    {
        return BAD_CALL;
    }
    if(in_len > *out_len)
    {
        return BAD_PARAM;
    }

    switch(ctx_tmp->ctx_decypt.alg)
    {
    case ALG_AES_CTR:
    case ALG_SM4_CTR:
        if(crypt_ctr_decrypt(&ctx_tmp->ctx_decypt, (unsigned char*)p_in_data, p_out_data, in_len, offset))
        {
            *out_len = in_len;
            return 0;
        }
        break;
    case ALG_AES_CBC:
    case ALG_SM4_CBC:
update_dec_ex:
        if(in_len > 0)
        {
            if(crypt_dec_update(&ctx_tmp->ctx_decypt, (unsigned char*)p_in_data, in_len, p_out_data, out_len))
            {
                return 0;
            }
        }
        else
        {
            if(*out_len < 16)
            {
                return BAD_PARAM;
            }
            if(crypt_dec_final(&ctx_tmp->ctx_decypt, p_out_data, (int *)out_len))
            {
                return 0;
            }
        }

        break;
    case ALG_AES_ECB:
    case ALG_SM4_ECB:
        if(ctx_tmp->is_series == 0)
        {
            if(in_len % 16 != 0 || offset % 16 != 0)
            {
                return BAD_PARAM;
            }
            if(crypt_dec_block(&ctx_tmp->ctx_decypt, (unsigned char *)p_in_data, p_out_data, in_len))
            {
                *out_len = in_len;
                return 0;
            }
        }
        else
        {
            goto update_dec_ex;
        }
        break;
    }

    return BAD_CALL;
}

void MPRSymmCrypt_UninitEx(void **p_context)                       //[IN]�ӽ���������
{
    MPRSymm_Context *ctx_tmp = NULL;
    if(*p_context == NULL || p_context == NULL)
    {
        return;
    }

    ctx_tmp = (MPRSymm_Context *)*p_context;
    crypt_enc_end(&ctx_tmp->ctx_encypt);
    crypt_dec_end(&ctx_tmp->ctx_decypt);
    free (*p_context);
    *p_context = NULL;
}

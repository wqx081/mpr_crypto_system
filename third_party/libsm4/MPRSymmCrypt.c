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

int MPRSymmCrypt_Init(const unsigned char *p_key,       //[IN]对称密钥首地址
                      const int            key_len,     //[IN]对称密钥长度
                      const unsigned char *p_iv,        //[IN]初始向量首地址, buff长度16字节, 仅CBC模式有效, 其他模式传NULL
                      const int            is_series,   //[IN]是否连续加解密, 仅ECB模式有效, 1连续加解密, 自动padding; 0非连续加解密, 只能传16的倍数块进行加解密
                      EncryptAlgorithm     alg)         //[IN]对称加密算法及模式, 仅ECB非连续和CTR模式支持随机读写加解密, 其他模式必须从头开始加解密

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

int MPRSymmCrypt_Encrypt(const unsigned char *p_in_data,      //[IN]需要加密明文首地址
                         const unsigned int   in_len,         //[IN]需要加密明文长度, 除CTR和ECB非连续加密以外, 其余模式都需要在加密最后传入in_len为0, 获取剩余加密数据段;ECB非连续加密只能传16的整数倍
                         const INT_64         offset,         //[IN]首地址在整个需加密的明文中的位置, 仅CTR和ECB模式有效, 其他模式传0, ECB模式offset必须为16的整数倍
                         unsigned char       *p_out_data,     //[OUT]加密后密文首地址, p_out_data指向的buff调用者分配释放, 建议大小为in_len + 16
                         unsigned int        *out_len)        //[IN/OUT]输入outbuff长度, 输出实际加密后密文长度
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

int MPRSymmCrypt_Decrypt(const unsigned char *p_in_data,      //[IN]需要解密密文首地址
                         const unsigned int   in_len,         //[IN]需要解密密文长度, 对于ECB模式, 长度必须为16的整数倍
                         const INT_64         offset,         //[IN]首地址在整个需解密密文中的位置, 仅CTR模式有效, 其他模式传0
                         unsigned char       *p_out_data,     //[OUT]解密后的明文首地址
                         unsigned int        *out_len)        //[OUT]解密后的明文长度
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

int MPRSymmCrypt_InitEx(const unsigned char *p_key,      //[IN]对称密钥首地址
                        const int            key_len,    //[IN]对称密钥长度
                        const unsigned char *p_iv,       //[IN]初始向量首地址, buff长度16字节, 仅CBC模式有效, 其他模式传NULL
                        const int            is_series,  //[IN]是否连续加解密, 仅ECB模式有效, 1连续加解密, 自动padding; 0非连续加解密, 只能传16的倍数块进行加解密
                        EncryptAlgorithm     alg,        //[IN]对称加密算法及模式, 仅ECB非连续和CTR模式支持随机读写加解密, 其他模式必须从头开始加解密
                        void                **p_context) //[OUT]加解密上下文, 用于接下来加解密时传入
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

int MPRSymmCrypt_EncryptEx(void                *p_context,        //[IN]加解密上下文
                           const unsigned char *p_in_data,        //[IN]需要加密明文首地址
                           const unsigned int   in_len,           //[IN]需要加密明文长度, 除CTR和ECB非连续加密以外, 其余模式都需要在加密最后传入in_len为0, 获取剩余加密数据段;ECB非连续加密只能传16的整数倍
                           const INT_64         offset,           //[IN]首地址在整个需加密的明文中的位置, 仅CTR和ECB模式有效, 其他模式传0, ECB模式offset必须为16的整数倍
                           unsigned char       *p_out_data,       //[OUT]加密后密文首地址, p_out_data指向的buff调用者分配释放, 建议大小为in_len + 16
                           unsigned int        *out_len)          //[IN/OUT]输入buff长度, 输出实际加密后密文长度
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

int MPRSymmCrypt_DecryptEx(void                *p_context,        //[IN]加解密上下文
                           const unsigned char *p_in_data,        //[IN]需要解密密文首地址
                           const unsigned int   in_len,           //[IN]需要解密密文长度, 除CTR和ECB非连续加密以外, 其余模式都需要在加密最后传入in_len为0, 获取剩余加密数据段;ECB非连续加密只能传16的整数倍
                           const INT_64         offset,           //[IN]首地址在整个需解密密文中的位置, 仅CTR和ECB模式有效, 其他模式传0, ECB模式offset必须为16的整数倍 
                           unsigned char       *p_out_data,       //[OUT]解密后的明文首地址, p_out_data指向的buff调用者分配释放, 建议大小为in_len + 16
                           unsigned int        *out_len)          //[IN/OUT]输入buff长度, 输出实际解密后明文长度
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

void MPRSymmCrypt_UninitEx(void **p_context)                       //[IN]加解密上下文
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

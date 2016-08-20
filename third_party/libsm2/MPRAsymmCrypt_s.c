#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "native_sm2.h"
//#include "rsa_api.h"
#include "MPRAsymmCrypt.h"
#include <stdio.h>

//XEngineSessionCtx NULL = NULL;

static int readFile(char *fileName, unsigned char *pBuffer, int *bufferLen)
{
    FILE *pFile;

    pFile = fopen(fileName, "rb");
    if( pFile != NULL)
    {
        *bufferLen = fread(pBuffer, 1, *bufferLen, pFile);
        fclose(pFile);
        return 1;
    }

    return -1;
}

static int loadPubKey(const unsigned char *pBuffer, int len, ECCrefPublicKey *pubKey)
{
    if( len != 48 && len != 64 )
        return -2;
    memset(pubKey, 0, sizeof(ECCrefPublicKey));
    pubKey->bits = (len*8)/2;
    memcpy(pubKey->x, pBuffer, len/2);
    memcpy(pubKey->y, pBuffer+len/2, len/2);

    return 0;
}

static int loadPrvKey(const unsigned char *pBuffer, int len, ECCrefPrivateKey *prvKey)
{
    if( len != 24 && len != 32 )
        return -2;

    memset(prvKey, 0, sizeof(ECCrefPrivateKey));
    prvKey->bits = len*8;
    memcpy(prvKey->D, pBuffer, len);

    return 0;
}

static int SizeOfPackedCipher(const ECCCipherEx cipher)
{
    return ((cipher.bits / 8) * 2) //x & y
        + sizeof(cipher.C)       //C
        + (cipher.mbits / 8);    //M
}


static int PackCipher(const ECCCipherEx cipher, unsigned char* p_buf, int* buf_len)
{
    int ofst = 0;
    if (*buf_len < SizeOfPackedCipher(cipher))
    {
        return -1;
    }


    memcpy(p_buf + ofst, cipher.x, cipher.bits / 8);
    ofst += cipher.bits / 8;

    memcpy(p_buf + ofst, cipher.y, cipher.bits / 8);
    ofst += cipher.bits / 8;

    memcpy(p_buf + ofst, cipher.C, sizeof(cipher.C));
    ofst += sizeof(cipher.C);

    memcpy(p_buf + ofst, cipher.M, cipher.mbits / 8);
    ofst += cipher.mbits / 8;

    *buf_len = ofst;

    return 0;
}


static int UnpackCipher(const unsigned char* p_buf, int buf_len, int bits, ECCCipherEx *cipher)
{
    int ofst = 0;

    cipher->bits = bits;
    cipher->mbits = (buf_len - cipher->bits / 4 - sizeof(cipher->C)) * 8;
    memcpy(cipher->x, p_buf + ofst, cipher->bits / 8);
    ofst += cipher->bits / 8;

    memcpy(cipher->y, p_buf + ofst, cipher->bits / 8);
    ofst += cipher->bits / 8;

    memcpy(cipher->C, p_buf + ofst, sizeof(cipher->C));
    ofst += sizeof(cipher->C);

    memcpy(cipher->M, p_buf + ofst, cipher->mbits / 8);
    ofst += cipher->mbits / 8;

    return 0;
}

int MPRAsymmCrypt_Init()
{
    return 0;
}

int MPRAsymmCrypt_GenerateKeyPair(PKI_Algorithm        alg,            //[IN]�ǶԳƼ����㷨
                                  const unsigned int   key_bits,       //[IN]�����㷨����, SM2�㷨192��256, RSA�㷨1024��2048                                 const unsigned char *p_in_data,      //[IN]��Ҫ���������׵�ַ
                                  unsigned char        *p_pubkey,      //[IN]��Կ�׵�ַ
                                  unsigned int         *pubkey_len,    //[IN/OUT]����buff����, ���ع�Կ����,����key_bits/8
                                  unsigned char        *p_prvkey,      //[IN]˽Կ�׵�ַ
                                  unsigned int         *prvkey_len)    //[OUT]����buff����, ����˽Կ����,����key_bits/16
{
    int rt = 0;
    
    if(p_pubkey == NULL || pubkey_len == NULL || p_prvkey == NULL || prvkey_len == NULL|| key_bits == 0){
        return BAD_PARAM;
    }
    switch(alg)
    {
        case SM2:
            rt = SDF_GenerateKeyPair_ECC(key_bits, (char *)p_pubkey, (int *)pubkey_len, (char *)p_prvkey, (int *)prvkey_len);
            if(SDR_OK != rt){
                return rt;
            }
            return 0;
        case RSA:
            return BAD_CALL;
            break;
        default:
            return BAD_PARAM;
            break;
    }
    
    return BAD_PARAM;
}

int MPRAsymmCrypt_PubEncrypt(PKI_Algorithm        alg,            //[IN]�ǶԳƼ����㷨
    const unsigned char *p_pubkey,       //[IN]��Կ�׵�ַ
    const unsigned int   pubkey_len,     //[IN]��Կ����
    const unsigned char *p_in_data,      //[IN]��Ҫ���������׵�ַ
    const unsigned int   in_len,         //[IN]��Ҫ�������ĳ���
    unsigned char       *p_out_data,     //[OUT]���ܺ������׵�ַ, p_out_dataָ���buff�����߷����ͷ�, �����СΪin_len + 16
    unsigned int        *out_len)        //[IN/OUT]����buff����, ���ʵ�ʼ��ܺ����ĳ���
{
    ECCrefPublicKey pucPublicKey;
    ECCCipherEx cipher = {0};
    int rt = 0;
    if(p_pubkey == NULL || pubkey_len == 0 || p_in_data == NULL || in_len == 0|| p_out_data == NULL || out_len == NULL){
        return BAD_PARAM;
    }

    switch(alg)
    {
    case SM2:
        if(0 != loadPubKey(p_pubkey, pubkey_len, &pucPublicKey))
        {
            return BAD_PARAM;
        }
        rt = SNF_ExternalEncrypt_ECC(NULL, SGD_SM2_3, &pucPublicKey, (uint8*)p_in_data, in_len, &cipher);
        if(SDR_OK != rt){
            return rt;
        }
        if(0 !=PackCipher(cipher, p_out_data, (int *)out_len))
        {
            return BAD_PARAM;
        }
        return 0;
    case RSA:
//        rt = rsa_pubkey_encrypt(p_in_data, in_len, p_pubkey, pubkey_len, p_out_data, (int*)out_len);
//        return rt;
        break;
    default:
        return BAD_PARAM;
        break;
    }

    return BAD_CALL;
}

int MPRAsymmCrypt_PrvDecrypt(PKI_Algorithm        alg,            //[IN]�ǶԳƼ����㷨
    const unsigned char *p_prvkey,       //[IN]˽Կ�׵�ַ
    const unsigned int   prvkey_len,     //[IN]˽Կ����
    const unsigned char *p_in_data,      //[IN]��Ҫ���������׵�ַ
    const unsigned int   in_len,         //[IN]��Ҫ�������ĳ���
    unsigned char       *p_out_data,     //[OUT]���ܺ�������׵�ַ, p_out_dataָ���buff�����߷����ͷ�, �����СΪin_len + 16
    unsigned int        *out_len)        //[IN/OUT]����buff����, ���ʵ�ʽ��ܺ����ĳ���
{
    ECCrefPrivateKey pucPrivateKey;
    ECCCipherEx decipher = {0};
    int rt = 0;
    if(p_prvkey == NULL || prvkey_len == 0 || p_in_data == NULL || in_len == 0|| p_out_data == NULL || out_len == NULL){
        return BAD_PARAM;
    }

    switch(alg)
    {
    case SM2:
        if(0 !=loadPrvKey(p_prvkey, prvkey_len, &pucPrivateKey))
        {
            return BAD_PARAM;
        }
            
        UnpackCipher(p_in_data, in_len, pucPrivateKey.bits, &decipher);
            
        unsigned char* p = (unsigned char*)&pucPrivateKey;
#if 0
        int i = 0;
        for(i = 0 ;i<sizeof(pucPrivateKey);i++){
            fprintf(stderr,"%02x",*p++);
        }
        fprintf(stderr,"\n");
        fprintf(stderr,"decipher :");
        unsigned char* p1 = (unsigned char*)&decipher;
        for(i = 0 ;i<sizeof(decipher);i++){
            fprintf(stderr,"%02x",*p1++);
        }
        fprintf(stderr,"\n");
            
#endif
        rt = SNF_ExternalDecrypt_ECC(NULL, SGD_SM2_3, &pucPrivateKey, &decipher, (uint8*)p_out_data, out_len);
        if(SDR_OK != rt){
            return rt;
        }

        return 0;
    case RSA:
//        rt = rsa_prvkey_decrypt(p_in_data, in_len, p_prvkey, prvkey_len, p_out_data, (int*)out_len);
//        return rt;
        break;
    default:
        return BAD_PARAM;
        break;
    }
    return BAD_CALL;
}

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
    unsigned int        *signature_len)  //[IN/OUT]����buff����, ���ʵ��ǩ������
{
    ECCrefPublicKey pucPublicKey = {0};
    ECCrefPrivateKey pucPrivateKey = {0};
    int rt = 0;
    ECCSignatureEx pucSignature = {0};

    if(p_pubkey == NULL || pubkey_len == 0 || p_prvkey == NULL || prvkey_len == 0 || p_in_data == NULL || in_len == 0|| p_sign_id == NULL || sign_len == 0|| p_signature == NULL || signature_len == NULL){
        return BAD_PARAM;
    }

    switch(alg)
    {
    case SM2:
        if(0 !=loadPrvKey(p_prvkey, prvkey_len, &pucPrivateKey))
        {
            return BAD_PARAM;
        }

        if(0 != loadPubKey(p_pubkey, pubkey_len, &pucPublicKey))
        {
            return BAD_PARAM;
        }

        rt = SNF_ExternalSign_ECC(NULL, SGD_SM2_1, &pucPrivateKey, (unsigned char *)p_sign_id, sign_len, (unsigned char *)p_in_data, in_len, &pucSignature);
        if(SDR_OK != rt){
            return rt;
        }
        *signature_len = pucSignature.bits / 4;
        memcpy(p_signature, pucSignature.r, *signature_len / 2);
        memcpy(p_signature + *signature_len / 2, pucSignature.s, *signature_len / 2);
        return 0;
    case RSA:
//        rt = rsa_prvkey_encrypt(p_in_data, in_len, p_prvkey, prvkey_len, p_signature, (int*)signature_len);
//        return rt;
        break;
    default:
        return BAD_PARAM;
        break;
    }
    return BAD_PARAM;
}

int MPRAsymmCrypt_Verify(PKI_Algorithm        alg,            //[IN]�ǶԳƼ����㷨
    const unsigned char *p_pubkey,       //[IN]��Կ�׵�ַ
    const unsigned int   pubkey_len,     //[IN]��Կ����
    const unsigned char *p_in_data,      //[IN]ǩ��MSG�׵�ַ
    const unsigned int   in_len,         //[IN]ǩ��MSG����
    const unsigned char *p_sign_id,      //[IN]ǩ��ID�׵�ַ
    const unsigned int   sign_len,       //[IN]ǩ��ID����
    unsigned char       *p_signature,    //[IN]ǩ������
    unsigned int         signature_len)  //[IN]����buff����
{
    ECCrefPublicKey pucPublicKey = {0};
    int rt = 0;
    ECCSignatureEx pucSignature = {0};
    unsigned char decrypt_data[1024] = {0};
    int decrypt_len = 1024;

    if(p_pubkey == NULL || pubkey_len == 0 || p_in_data == NULL || in_len == 0|| p_sign_id == NULL || sign_len == 0|| p_signature == NULL || signature_len == 0){
        return BAD_PARAM;
    }

    switch(alg)
    {
    case SM2:
        if(0 != loadPubKey(p_pubkey, pubkey_len, &pucPublicKey))
        {
            return BAD_PARAM;
        }

        pucSignature.bits = pucPublicKey.bits;
        memcpy(pucSignature.r, p_signature, pucSignature.bits / 8);
        memcpy(pucSignature.s, p_signature + pucSignature.bits / 8, pucSignature.bits / 8);

        rt = SNF_ExternalVerify_ECC(NULL, SGD_SM2_1, &pucPublicKey, (unsigned char *)p_sign_id, sign_len, (unsigned char *)p_in_data, in_len, &pucSignature);
        if(SDR_OK != rt){
            return rt;
        }
        return 0;
    case RSA:
//        rt = rsa_pubkey_decrypt(p_signature, signature_len, p_pubkey, pubkey_len, decrypt_data, &decrypt_len);
//        if(rt != 0){
//            return rt;
//        }
//
//        return memcmp(p_in_data, decrypt_data, in_len);
        break;
    default:
        return BAD_PARAM;
        break;
    }
    return BAD_PARAM;
}

int MPRAsymmCrypt_Uninit()
{
    return 0;
}

#if 0
int main()
{
    const unsigned int   key_bits     = 1024;       //[IN]�����㷨����, SM2�㷨192��256, RSA�㷨1024��2048                                 const unsigned char *p_in_data,      //[IN]��Ҫ���������׵�ַ
    unsigned char        p_pubkey[1024] = {0};      //[IN]��Կ�׵�ַ
    unsigned int         pubkey_len   = 1024;    //[IN/OUT]����buff����, ���ع�Կ����,����key_bits/8
    unsigned char        p_prvkey[1024] = {0};      //[IN]˽Կ�׵�ַ
    unsigned int         prvkey_len  = 1024;
    unsigned char        cipher[256] = {0};
    int                  cipher_len = 256;
    unsigned char        decipher[256] = {0};
    int                  decipher_len = 256;
    unsigned char        p_sign[64] = {0};
    unsigned int         sign_len = 64;
    int rt = 0;

    MPRAsymmCrypt_Init();
    readFile("pubkey_256(5).sm2", p_pubkey, &pubkey_len);
    readFile("prvkey_256(5).sm2", p_prvkey, &prvkey_len);
    MPRAsymmCrypt_PubEncrypt(SM2, p_pubkey, pubkey_len, (const unsigned char *)"34564564563453535312312312312312364646345", 17, cipher, &cipher_len);
    MPRAsymmCrypt_PrvDecrypt(SM2, p_prvkey, prvkey_len, cipher, cipher_len, decipher, &decipher_len);
    MPRAsymmCrypt_Sign(SM2, p_pubkey, pubkey_len, p_prvkey, prvkey_len, "12236546974656", 12, "2013120100087", 13, p_sign, &sign_len);
    MPRAsymmCrypt_Verify(SM2, p_pubkey, pubkey_len, "12236546974656", 12, "2013120100087", 13, p_sign, sign_len);

    pubkey_len = 1024;
    prvkey_len = 1024;
    //rt = rsa_generate_keypair(key_bits, p_pubkey, &pubkey_len, p_prvkey, &prvkey_len);
    MPRAsymmCrypt_PubEncrypt(RSA, p_pubkey, pubkey_len, (const unsigned char *)"34564564563453535312312312312312364646345", 17, cipher, &cipher_len);
    MPRAsymmCrypt_PrvDecrypt(RSA, p_prvkey, prvkey_len, cipher, cipher_len, decipher, &decipher_len);
    MPRAsymmCrypt_Sign(RSA, p_pubkey, pubkey_len, p_prvkey, prvkey_len, "12236546974656", 12, "2013120100087", 13, p_sign, &sign_len);
    MPRAsymmCrypt_Verify(RSA, p_pubkey, pubkey_len, "12236546974656", 12, "2013120100087", 13, p_sign, sign_len);
}
#endif

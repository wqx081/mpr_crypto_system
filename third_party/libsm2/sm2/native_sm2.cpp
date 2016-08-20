#include "stdafx.h"
#include "native_sm2.h"
#include "sm2.h"

#define NULL 0
#include <memory.h>

int32 SDF_GenerateKeyPair_ECC(uint32 keyBits,
                              char *pubKey,
                              int *pubKeyLen,
                              char *prvKey,
                              int *prvKeyLen
                              )
{
    sm2_pubkey pub_key_s;
    sm2_prvkey prv_key_s;
    
    if (keyBits != 192 && keyBits != 256) {
        return SDR_KEYERR;
    }
    
    sm2_init_param(keyBits);
    return sm2_key_gen_ext(&pub_key_s, &prv_key_s, prvKey, prvKeyLen, pubKey, pubKeyLen);
}

int32 SNF_ExternalSign_ECC(
	void* sessionCtx,
	uint32 uiAlgID,
	ECCrefPrivateKey *pucPrivateKey,
	uint8 *pid,
	uint32 ilen,
	uint8 *message,
	uint32 mlen,
	ECCSignatureEx *pucSignature
)
{
	sm2_prvkey prvkey;
	char signature[512];
	unsigned int slen = sizeof(signature);

	if (sessionCtx != NULL)
		return SDR_OPENDEVICE;

	if (uiAlgID != SGD_SM2_1)
		return SDR_ALGMODNOTSUPPORT;

	if (pucPrivateKey->bits != 192 && pucPrivateKey->bits != 256)
		return SDR_KEYERR;

	sm2_init_param(pucPrivateKey->bits);
	sm2_load_prvkey(&prvkey , pucPrivateKey->D , pucPrivateKey->bits / 8);

	if (sm2_sign(&prvkey , pid , ilen , message , mlen , (unsigned char*)signature , &slen))
		return SDR_SIGNERR;
	
	pucSignature->bits = pucPrivateKey->bits;
	memcpy(pucSignature->r, signature , slen / 2);
	memcpy(pucSignature->s , signature + slen / 2, slen / 2);
	return SDR_OK;
}

int32 SNF_ExternalVerify_ECC(
	void* sessionCtx,
	uint32 uiAlgID,
	ECCrefPublicKey *pucPublicKey,
	uint8 *pid,
	uint32 ilen,
	uint8 *message,
	uint32 mlen,
	ECCSignatureEx *pucSignature
)
{
	sm2_pubkey pubkey;
	char key[65] = {0};
	char signature[512] = {0};
	unsigned int slen = sizeof(signature);

	if (sessionCtx != NULL)
		return SDR_OPENDEVICE;

	if (uiAlgID != SGD_SM2_1)
		return SDR_ALGMODNOTSUPPORT;

	if (pucPublicKey->bits != 192 && pucPublicKey->bits != 256)
		return SDR_KEYERR;

	sm2_init_param(pucPublicKey->bits);
	key[0] = 0x04;
	memcpy(key + 1, pucPublicKey->x , pucPublicKey->bits / 8);
	memcpy(key + 1 + pucPublicKey->bits / 8 , pucPublicKey->y ,pucPublicKey->bits / 8 );

	sm2_load_pubkey(&pubkey , (unsigned char*)key , (pucPublicKey->bits / 4) + 1);

	slen = pucSignature->bits / 4 ;
	memcpy(signature , pucSignature->r , slen / 2);
	memcpy(signature + slen / 2, pucSignature->s , slen / 2);

	if (sm2_verify(&pubkey , pid , ilen , message , mlen , (unsigned char*)signature , slen))
		return SDR_VERIFYERR;

	return SDR_OK;
}

int32 SNF_ExternalEncrypt_ECC(
	 void*  sessionCtx,
	 uint32 uiAlgID,
	 ECCrefPublicKey *pucPublicKey,
	 uint8 *pucData,
	 uint32 uiDataLength,
	 ECCCipherEx *pucEncData
 )
{
	sm2_pubkey pubkey;
	char key[65] = {0};
	char cipher[512] = {0};
	unsigned int scip = sizeof(cipher);
	int len = 0 ;

	if (sessionCtx != NULL)
		return SDR_OPENDEVICE;

	if (uiAlgID != SGD_SM2_3)
		return SDR_ALGMODNOTSUPPORT;

	if (pucPublicKey->bits != 192 && pucPublicKey->bits != 256)
		return SDR_KEYERR;


	sm2_init_param(pucPublicKey->bits);
	key[0] = 0x04;
	memcpy(key + 1, pucPublicKey->x , pucPublicKey->bits / 8);
	memcpy(key + 1 + pucPublicKey->bits / 8 , pucPublicKey->y ,pucPublicKey->bits / 8 );

	sm2_load_pubkey(&pubkey , (unsigned char*)key , (pucPublicKey->bits / 4) + 1);

	if (sm2_enc(&pubkey ,pucData , uiDataLength  , (unsigned char*)cipher ,  &scip))
	{
		return SDR_PKOPERR;
	}
	
	pucEncData->bits = pucPublicKey->bits;
	memcpy(pucEncData->x , cipher + 1 , pucPublicKey->bits / 8);
	memcpy(pucEncData->y , cipher + 1 + pucPublicKey->bits / 8, pucPublicKey->bits / 8);
	memcpy(pucEncData->C , cipher + 1 + pucPublicKey->bits / 4 , 0x20);
	len = (scip - 1 - 0x20 - pucPublicKey->bits / 4 ) ;
	memcpy(pucEncData->M , cipher + 1 + pucPublicKey->bits / 4 + 0x20 , len );
	pucEncData->mbits = len * 8 ;
	return SDR_OK;
}

int32 SNF_ExternalDecrypt_ECC(
	 void*  sessionCtx,
	 uint32 uiAlgID,
	 ECCrefPrivateKey *pucPrivateKey,
	 ECCCipherEx *pucEncData,
	 uint8 *pucData,
	 uint32 *puiDataLength
 )
{
	sm2_prvkey prvkey;
	char cipher[512];
	unsigned int clen = sizeof(cipher);
	int len = 0 ;

	if (sessionCtx != NULL)
		return SDR_OPENDEVICE;

	if (uiAlgID != SGD_SM2_3)
		return SDR_ALGMODNOTSUPPORT;

	if (pucPrivateKey->bits != 192 && pucPrivateKey->bits != 256)
		return SDR_KEYERR;

	sm2_init_param(pucPrivateKey->bits);
	sm2_load_prvkey(&prvkey , pucPrivateKey->D , pucPrivateKey->bits / 8);

	cipher[0] = 0x04;
	memcpy(cipher + 1 , pucEncData->x , pucEncData->bits / 8);
	memcpy(cipher + 1 + pucEncData->bits / 8 , pucEncData->y , pucEncData->bits / 8);
	memcpy(cipher + 1 + pucEncData->bits / 4  , pucEncData->C , 0x20);
	len = pucEncData->mbits / 8 ;
	memcpy(cipher + 1 + pucEncData->bits /4 + 0x20 , pucEncData->M , len);

	len = 1 + pucEncData->bits / 4 + 0x20  + pucEncData->mbits / 8 ;

	if (sm2_dec(&prvkey , (unsigned char*)cipher , len , pucData ,  puiDataLength))
	{
		return SDR_SKOPERR;
	}


	return SDR_OK;
}


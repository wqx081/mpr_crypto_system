
#ifndef __CRYPT_LIB_H__
#define __CRYPT_LIB_H__

#if WIN32
 #include <windows.h>
#endif

#ifndef uint8
#define uint8  unsigned char
#endif

#ifndef int32
#define int32 unsigned int
#endif

#ifndef ulong
#define ulong unsigned long
#endif

#ifndef int64
 #if WIN32
  #define	int64	__int64
 #else
  #define	int64	long long
 #endif
#endif

#define SM4_KEY_LENGTH			16
#define AES_KEY_LENGTH			16
#define MAX_KEY_LENGTH			16

#define SM4_BLOCK_SIZE			16
#define AES_BLOCK_SIZE			16
#define MAX_CIPHER_BLOCK_SIZE	16

#define ALG_SM4				0x00000001
#define ALG_AES				0x00000002

#define MODE_ECB			0x00010000
#define MODE_CBC			0x00020000
#define MODE_CTR			0x00100000

#define ALG_SM4_ECB			(ALG_SM4 | MODE_ECB)
#define ALG_SM4_CBC			(ALG_SM4 | MODE_CBC)
#define ALG_SM4_CTR			(ALG_SM4 | MODE_CTR)
#define ALG_AES_ECB			(ALG_AES | MODE_ECB)
#define ALG_AES_CBC			(ALG_AES | MODE_CBC)
#define ALG_AES_CTR			(ALG_AES | MODE_CTR)

#define	MASK_ALG			0x0000FFFF
#define MASK_MODE			0xFFFF0000

typedef struct{
	void *enc_ctx;
	int alg;
	int block_size;
	int count;
	int flag;
	uint8 iv[MAX_CIPHER_BLOCK_SIZE];
	uint8 buf[MAX_CIPHER_BLOCK_SIZE];
	uint8 final[MAX_CIPHER_BLOCK_SIZE];
}cipher_context;

#ifdef __cplusplus
extern "C" {
#endif

int crypt_enc_init(cipher_context *cctx, uint8 *ck, int ckl, uint8 *iv, int alg);
int crypt_dec_init(cipher_context *cctx, uint8 *ck, int ckl, uint8 *iv, int alg);

int crypt_enc_block(cipher_context *cctx, uint8 *in, uint8 *out, int len);
int crypt_dec_block(cipher_context *cctx, uint8 *in, uint8 *out, int len);

int crypt_enc_update(cipher_context *cctx, uint8 *in, int32 inl, uint8 *out, int32 *outl);
int crypt_enc_final(cipher_context *cctx, uint8 *out, int *outl);

int crypt_dec_update(cipher_context *cctx, uint8 *in, int32 inl, uint8 *out, int32 *outl);
int crypt_dec_final(cipher_context *cctx, uint8 *out, int *outl);

void crypt_enc_end(cipher_context *cctx);
void crypt_dec_end(cipher_context *cctx);

int crypt_ctr_encrypt(cipher_context *cctx, uint8 *in, uint8 *out, ulong length, int64 offset);
int crypt_ctr_decrypt(cipher_context *cctx, uint8 *in, uint8 *out, ulong length, int64 offset);

#ifdef __cplusplus
}
#endif

#endif

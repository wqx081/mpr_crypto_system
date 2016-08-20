
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cryptlib.h"

#include "sm4.h"
#include "aes_fast.h"

int crypt_enc_init(cipher_context *cctx, uint8 *ck, int ckl, uint8 *iv, int alg)
{
	switch(alg&MASK_ALG){
	case ALG_AES:
		cctx->enc_ctx = (AES_KEY *)malloc(sizeof(AES_KEY));
		cctx->block_size=AES_BLOCK_SIZE;
		if(ckl<AES_KEY_LENGTH)
			return 0;
		AES_set_encrypt_key(ck, ckl*8, cctx->enc_ctx);
		if(iv)
			memcpy(cctx->iv, iv, cctx->block_size);
		break;
	case ALG_SM4:
		cctx->enc_ctx = (sm4_context *)malloc(sizeof(sm4_context));
		cctx->block_size=SM4_BLOCK_SIZE;
		if(ckl<SM4_KEY_LENGTH)
			return 0;
		sm4_setkey_enc(cctx->enc_ctx, ck);
		if(iv)
			memcpy(cctx->iv, iv, cctx->block_size);
		break;
	default:
		return 0;
	}

	cctx->alg=alg;
	cctx->count=0;
	cctx->flag=0;

	return 1;
}

int crypt_dec_init(cipher_context *cctx, uint8 *ck, int ckl, uint8 *iv, int alg)
{
	switch(alg&MASK_ALG){
	case ALG_AES:
		cctx->enc_ctx = (AES_KEY *)malloc(sizeof(AES_KEY));
		cctx->block_size=AES_BLOCK_SIZE;
		if(ckl<AES_KEY_LENGTH)
			return 0;
		if( alg == ALG_AES_CTR )
			AES_set_encrypt_key(ck, ckl*8, cctx->enc_ctx);
		else
			AES_set_decrypt_key(ck, ckl*8, cctx->enc_ctx);
		if(iv)
			memcpy(cctx->iv, iv, cctx->block_size);
		break;
	case ALG_SM4:
		cctx->enc_ctx = (sm4_context *)malloc(sizeof(sm4_context));
		cctx->block_size=SM4_BLOCK_SIZE;
		if(ckl<SM4_KEY_LENGTH)
			return 0;
		if( alg == ALG_SM4_CTR )
			sm4_setkey_enc(cctx->enc_ctx, ck);
		else
			sm4_setkey_dec(cctx->enc_ctx, ck);
		if(iv)
			memcpy(cctx->iv, iv, cctx->block_size);
		break;
	default:
		return 0;
	}

	cctx->alg=alg;
	cctx->count=0;
	cctx->flag=0;
	
	return 1;
}

int crypt_enc_block(cipher_context *cctx, uint8 *in, uint8 *out, int len)
{
	if(len%cctx->block_size)
		return 0;
		
	switch(cctx->alg){
	case ALG_AES_ECB:
	case ALG_AES_CTR:
	{
		int j;

		for(j=0; j<len; j+=cctx->block_size){
			AES_encrypt(in+j, out+j, cctx->enc_ctx);
		}
		return 1;
	}
	case ALG_AES_CBC:
	{
		int i, j;
		uint8* piv;
		uint8 temp[MAX_CIPHER_BLOCK_SIZE];

		piv=cctx->iv;
		for(j=0; j<len; j+=cctx->block_size){
			for(i=0; i<cctx->block_size; i++)
				temp[i]=in[j+i]^piv[i];
			AES_encrypt(temp, out+j, cctx->enc_ctx);
			piv=out+j;
		}

		memcpy(cctx->iv, out+j-cctx->block_size, cctx->block_size);
		return 1;
	}
	case ALG_SM4_ECB:
	case ALG_SM4_CTR:
		sm4_crypt_ecb(cctx->enc_ctx, SM4_ENCRYPT, len, in, out);
		return 1;
	case ALG_SM4_CBC:
		sm4_crypt_cbc(cctx->enc_ctx, SM4_ENCRYPT, len, cctx->iv, in, out);
		return 1;
	}
	
	return 0;
}

int crypt_dec_block(cipher_context *cctx, uint8 *in, uint8 *out, int len)
{
	if(len%cctx->block_size)
		return 0;
		
	switch(cctx->alg){
	case ALG_AES_ECB:
	case ALG_AES_CTR:
	{
		int j;

		for(j=0; j<len; j+=cctx->block_size){
			AES_decrypt(in+j, out+j, cctx->enc_ctx);
		}
		return 1;
	}
	case ALG_AES_CBC:
	{
		int i, j;
		uint8 temp[MAX_CIPHER_BLOCK_SIZE];

		for(j=0; j<len; j+=cctx->block_size){
			memcpy(temp, in+j, cctx->block_size);
			AES_decrypt(temp, out+j, cctx->enc_ctx);
			for(i=0; i<cctx->block_size; i++){
				out[j+i]^=cctx->iv[i];
			}
			
			memcpy(cctx->iv, temp, cctx->block_size);
		}
		return 1;
	}
	case ALG_SM4_ECB:
	case ALG_SM4_CTR:
		sm4_crypt_ecb(cctx->enc_ctx, SM4_DECRYPT, len, in, out);
		return 1;
		break;
	case ALG_SM4_CBC:
		sm4_crypt_cbc(cctx->enc_ctx, SM4_DECRYPT, len, cctx->iv, in, out);
		return 1;
		break;
	}
	
	return 0;
}

#include "aes_locl.h"

void ctr_init_iv(uint8 *iv, int64 counter)
{
	int64 val = counter;
	unsigned int ll = (unsigned int)counter;
	unsigned int hh = (unsigned int)(counter >> 32);

	memset(iv, 0, 16);
	PUTU32(iv+8, hh);
	PUTU32(iv+12, ll);
}

int crypt_ctr_encrypt(cipher_context *cctx, uint8 *in, uint8 *out, ulong length, int64 offset)
{
	int64 alignment;
	unsigned char ivec[MAX_CIPHER_BLOCK_SIZE];
	unsigned char ecount_buf[MAX_CIPHER_BLOCK_SIZE];
	unsigned int n;
	unsigned long l=length;
	int i = 0;

	if(offset%MAX_CIPHER_BLOCK_SIZE)
	{
		alignment = offset/MAX_CIPHER_BLOCK_SIZE;
		ctr_init_iv(ivec, alignment);
		crypt_enc_block(cctx, ivec, ecount_buf, MAX_CIPHER_BLOCK_SIZE);
 		AES_ctr128_inc(ivec);
		n = offset%MAX_CIPHER_BLOCK_SIZE;
	}
	else
	{
		ctr_init_iv(ivec, offset/MAX_CIPHER_BLOCK_SIZE);
		n = 0;
	}

	while (l) {
		if (n == 0) {
			crypt_enc_block(cctx, ivec, ecount_buf, MAX_CIPHER_BLOCK_SIZE);
 			AES_ctr128_inc(ivec);
		}
		*(out++) = *(in++) ^ ecount_buf[n];
		n = (n+1) % MAX_CIPHER_BLOCK_SIZE;
		l--;
		i++;
	}

	return 1;
}

int crypt_ctr_decrypt(cipher_context *cctx, uint8 *in, uint8 *out, ulong length, int64 offset)
{
	int64 alignment;
	unsigned char ivec[MAX_CIPHER_BLOCK_SIZE];
	unsigned char ecount_buf[MAX_CIPHER_BLOCK_SIZE];
	unsigned int n;
	unsigned long l=length;

	if(offset%MAX_CIPHER_BLOCK_SIZE)
	{
		alignment = offset/MAX_CIPHER_BLOCK_SIZE;
		ctr_init_iv(ivec, alignment);
		crypt_enc_block(cctx, ivec, ecount_buf, MAX_CIPHER_BLOCK_SIZE);
 		AES_ctr128_inc(ivec);
		n = offset%MAX_CIPHER_BLOCK_SIZE;
	}
	else
	{
		ctr_init_iv(ivec, offset/MAX_CIPHER_BLOCK_SIZE);
		n = 0;
	}

	while (l) {
		if (n == 0) {
			crypt_enc_block(cctx, ivec, ecount_buf, MAX_CIPHER_BLOCK_SIZE);
 			AES_ctr128_inc(ivec);
		}
		*(out++) = *(in++) ^ ecount_buf[n];
		n = (n+1) % MAX_CIPHER_BLOCK_SIZE;
		l--;
	}

	return 1;
}

int crypt_enc_update(cipher_context *cctx, uint8 *in, int32 inl, uint8 *out, int32 *outl)
{
#if 0
	int tlen=cctx->count+inl;
	int block_size=cctx->block_size;
	int blocks=tlen/block_size;
	int xl, ret;
	uint8 *temp;

	if(blocks==0){ //tlen<CIPHER_BLOCK_SIZE
		memcpy(cctx->buf+cctx->count, in, inl);
		cctx->count+=inl;
		*outl=0;
		return 1;
	}

	xl=block_size*blocks;
	
	if(cctx->count){
		temp=(uint8 *)malloc(xl);
		if(!temp)
			return 0;
		memcpy(temp, cctx->buf, cctx->count);
		memcpy(temp+cctx->count, in, xl-cctx->count);
		ret=crypt_enc_block(cctx, temp, out, xl);
		free(temp);
	}
	else
		ret=crypt_enc_block(cctx, in, out, xl);

	if(ret!=1)
		return ret;

	cctx->count=tlen%block_size;
	if(cctx->count){//AT LEAST THERE IS ONE MORE BLOCK
		memcpy(cctx->buf, in+inl-cctx->count, cctx->count);
	}

	*outl=xl;

	return 1;
#else
	int tlen=cctx->count+inl;
	int block_size=cctx->block_size;
	int blocks=tlen/block_size;
	int nfill;
	int ret;

	uint8 first[MAX_CIPHER_BLOCK_SIZE];
	uint8 *vout;

	*outl=0;
	vout=out;

	if(blocks==0){ //tlen<CIPHER_BLOCK_SIZE
		memcpy(cctx->buf+cctx->count, in, inl);
		cctx->count+=inl;
		return 1;
	}
	
	nfill=block_size-cctx->count;
	memcpy(cctx->buf+cctx->count, in, nfill);

	ret=crypt_enc_block(cctx, cctx->buf, first, block_size);	//1 block encryption
	if(ret!=1)
		return ret;

	in+=nfill;
	vout+=nfill;

	*outl+=block_size;

	cctx->count=0;
	blocks--;

	if(blocks){
		ret=crypt_enc_block(cctx, in, vout, blocks*block_size);
		if(ret!=1)
			return ret;

		in+=blocks*block_size;
		vout+=blocks*block_size;
		*outl+=blocks*block_size;
	}

	cctx->count=tlen%block_size;
	if(cctx->count){//AT LEAST THERE IS ONE MORE BLOCK
		memcpy(cctx->buf, in, cctx->count);
	}

	if(nfill!=block_size){//if inl%16=0, then this will never happen
		memmove(out+block_size, out+nfill, *outl-block_size);
	}

	memcpy(out, first, block_size);

	return 1;
#endif
}

int crypt_enc_final(cipher_context *cctx, uint8 *out, int *outl)
{
	int i, ret;
	int n=cctx->block_size-cctx->count;

	switch(cctx->alg & MASK_MODE){
	case MODE_ECB:
		for(i=cctx->count; i<cctx->block_size; i++)
			cctx->buf[i]=n;
					
		ret=crypt_enc_block(cctx, cctx->buf, out, cctx->block_size);	//the last block encryption with padding
		if(ret!=1)
			return ret;
					
		*outl=cctx->block_size;

		return 1;
		break;
	case MODE_CBC:
		for(i=cctx->count; i<cctx->block_size; i++)
			cctx->buf[i]=n;

		ret=crypt_enc_block(cctx, cctx->buf, out, cctx->block_size);	//the last block encryption with padding
		if(ret!=1)
			return ret;

		*outl=cctx->block_size;

		return 1;
	default:
		return 0;
	}
}

int crypt_dec_update(cipher_context *cctx, uint8 *in, int32 inl, uint8 *out, int32 *outl)
{
#if 0
	int tlen=cctx->count+inl;
	int block_size=cctx->block_size;
	int blocks=tlen/block_size;
	int xl, ret;
	uint8 *temp, fi[MAX_CIPHER_BLOCK_SIZE];

	*outl=0;
	if(blocks==0){ //tlen<CIPHER_BLOCK_SIZE
		memcpy(cctx->buf+cctx->count, in, inl);
		cctx->count+=inl;

		if(cctx->flag==1){//Block in final is not the FINAL BLOCK
			memcpy(out, cctx->final, block_size);
			*outl=block_size;
			cctx->flag=0;
		}
		return 1;
	}

	xl=blocks*block_size;

	if(cctx->count){
		temp=(uint8 *)malloc(xl);
		if(!temp)
			return 0;
		memcpy(temp, cctx->buf, cctx->count);
		memcpy(temp+cctx->count, in, xl-cctx->count);
		ret=crypt_dec_block(cctx, temp, out, xl);
		free(temp);
	}
	else
		ret=crypt_dec_block(cctx, in, out, xl);
	if(ret!=1)
		return ret;

	cctx->count=tlen%block_size;

	if(cctx->count){//AT LEAST THERE IS ONE MORE BLOCK
		memcpy(cctx->buf, in+inl-cctx->count, cctx->count);
	}

	if(!cctx->count){//have to backup the last block in case it is the final block
		if(!cctx->flag){
			memcpy(cctx->final, out+xl-block_size, block_size);
			cctx->flag=1;
			*outl=xl-block_size;
		}
		else{
			memcpy(fi, out+xl-block_size, block_size);
			memmove(out+block_size, out, xl-block_size);
			memcpy(out, cctx->final, block_size);
			memcpy(cctx->final, fi, block_size);
			*outl=xl;
		}
	}
	else{
		if(cctx->flag){
			memmove(out+block_size, out, xl);
			memcpy(out, cctx->final, block_size);
			*outl=xl+block_size;
			cctx->flag=0;
		}
		else
			*outl=xl;
	}

	return 1;
#else
	int tlen=cctx->count+inl;
	int block_size=cctx->block_size;
	int blocks=tlen/block_size;
	int nfill, ret;

	uint8 first[MAX_CIPHER_BLOCK_SIZE];
	uint8 *vout;

	*outl=0;
	vout=out;

	if(inl<0)
		return 0;

	if(inl==0)
		return 1;
	
	if(blocks==0){ //tlen<CIPHER_BLOCK_SIZE
		memcpy(cctx->buf+cctx->count, in, inl);
		cctx->count+=inl;

		if(cctx->flag==1){//Block in final is not the FINAL BLOCK
			memcpy(vout, cctx->final, block_size);
			*outl=block_size;
			cctx->flag=0;
		}
		return 1;
	}

	nfill=block_size-cctx->count;
	memcpy(cctx->buf+cctx->count, in, nfill);

	ret=crypt_dec_block(cctx, cctx->buf, first, block_size);
	if(ret!=1)
		return ret;

	vout+=nfill;
	in+=nfill;

	//now IN and OUT are aligned

	*outl+=block_size; //include the first block
	cctx->count=0;
	blocks--;

	if(blocks){
		ret=crypt_dec_block(cctx, in, vout, blocks*block_size);
		if(ret!=1)
			return ret;

		vout+=blocks*block_size;
		in+=blocks*block_size;
		*outl+=blocks*block_size;
	}

	cctx->count=tlen%block_size;

	if(cctx->count){//AT LEAST THERE IS ONE MORE BLOCK
		memcpy(cctx->buf, in, cctx->count); //save the left input to cctx->buf
	}

	if(cctx->flag==0){//insert only the first block
		if(nfill!=block_size)
			memmove(out+block_size, out+nfill, *outl-block_size);//the first block is not in buf out yet

		memcpy(out, first, block_size);
	}
	else{//insert the last final block + the first block
		memmove(out+2*block_size, out+nfill, *outl-block_size);
		memcpy(out, cctx->final, block_size); //insert the last final block
		memcpy(out+block_size, first, block_size); //insert the first block
		*outl+=block_size;//including the last final block
		cctx->flag=0;
	}

	if(!cctx->count){//have to backup the last block in case it is the final block
		memcpy(cctx->final, out+*outl-block_size, block_size);
		cctx->flag=1;
		*outl-=block_size;
	}

	return 1;
#endif
}

int crypt_dec_final(cipher_context *cctx, uint8 *out, int *outl)
{
	int n, b, i;
	*outl=0;

	if(cctx->count)
		return 0;

	if(cctx->flag==0) //this could not happen
		return 0;

	b=cctx->block_size;

	switch(cctx->alg & MASK_MODE){
	case MODE_ECB:
		//*outl = b;
		//memcpy(out, cctx->final, b);
		n=cctx->final[cctx->block_size-1];
		if(n>cctx->block_size)
			return 0;
		
		for (i=0; i<n; i++)
		{
			if (cctx->final[--b] != n)
			{
				return 0;
			}
		}
		
		n=cctx->block_size-n;
		for (i=0; i<n; i++)
			out[i]=cctx->final[i];
		*outl=n;
		return 1;
		break;
	case MODE_CBC:
		n=cctx->final[cctx->block_size-1];
		if(n>cctx->block_size)
			return 0;

		for (i=0; i<n; i++)
		{
			if (cctx->final[--b] != n)
			{
				return 0;
			}
		}

		n=cctx->block_size-n;
		for (i=0; i<n; i++)
			out[i]=cctx->final[i];
		*outl=n;
		return 1;
	default:
		return 0;
	}
}

void crypt_enc_end(cipher_context *cctx)
{
	if( cctx->enc_ctx )
		free( cctx->enc_ctx );
}

void crypt_dec_end(cipher_context *cctx)
{
	if( cctx->enc_ctx )
		free( cctx->enc_ctx );
}

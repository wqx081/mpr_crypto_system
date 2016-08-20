#ifndef __PBC_SM2_H__
#define __PBC_SM2_H__


#include "mpz_t.h"


struct point_proj_s
{
    mpz_t x, y, z;
};

typedef struct point_proj_s point_proj_t[1];
typedef struct point_proj_s *point_proj_ptr;

struct point_affn_s
{
	mpz_t x,y;
	char infinity;
};

typedef struct point_affn_s point_affn_t[1];
typedef struct point_affn_s * point_affn_ptr;

typedef struct
{
	point_affn_t sG;
	unsigned int  ecp;
}sm2_pubkey;

typedef struct
{
	point_affn_t sG;
	mpz_t s;
}sm2_prvkey;

#ifdef __cplusplus
extern "C" {
#endif

int sm2_enc
(
	sm2_pubkey *pubkey,
	unsigned char *plaintext, 
	unsigned int plen, 
	unsigned char *ciphertext, 
	unsigned int *clen
);

int sm2_dec
(
	sm2_prvkey *prvkey,
	unsigned char *ciphertext, 
	unsigned int clen,
	unsigned char *plaintext, 
	unsigned int *plen 
);

int sm2_sign
(
	sm2_prvkey *prvkey,
	unsigned char *id,
	unsigned int ilen,
	unsigned char *message, 
	unsigned int mlen,
	unsigned char *signature, 
	unsigned int *slen 	
);

int sm2_verify
(
	sm2_pubkey *pubkey,
	unsigned char *id,
	unsigned int ilen,
	unsigned char *message, 
	unsigned int mlen,
	unsigned char *signature, 
	unsigned int slen 	
);



int sm2_load_pubkey
(
	sm2_pubkey *pubkey,
	unsigned char *data,
	unsigned int n
);

void sm2_clear_pubkey(
    sm2_pubkey *pubkey
);

int sm2_load_prvkey
(
	sm2_prvkey *prvkey ,
	unsigned char *data,
	unsigned int n
);

void sm2_clear_prvkey(
    sm2_prvkey *prvkey
);

void sm2_init_param(
    int bitlen
);

int sm2_key_gen(
	sm2_pubkey *pubkey,
	sm2_prvkey *prvkey,
	char* pubkeydata,
	int*  puLen
);
    
int sm2_key_gen_ext(
    sm2_pubkey *pubkey,
    sm2_prvkey *prvkey,
    char* prvkeydata,
    int*  prvLen,
    char* pubkeydata,
    int*  puLen
);

#ifdef __cplusplus
}
#endif

#endif

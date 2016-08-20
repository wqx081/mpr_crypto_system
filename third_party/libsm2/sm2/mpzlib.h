#ifndef _MPZLIB_H
#define _MPZLIB_H
#include "mpz_t.h"

#include "NN.h"
#include "eicl_mul.h"

#ifdef __cplusplus
extern "C" {
#endif

void  mpz_init(
    BIGINT *a
);

void  mpz_set(
    BIGINT *c, 
    BIGINT *a
);

// c = a + b
void  mpz_add(
    BIGINT *c, 
    BIGINT *a, 
    BIGINT *b
);

// c = a - b
void  mpz_sub(
    BIGINT *c, 
    BIGINT *a,
    BIGINT *b
);

// c = a * b
void  mpz_mul(
    BIGINT *c, 
    BIGINT *a,
    BIGINT *b
);

// c = a mod b
void  mpz_mod(
    BIGINT *c, 
    BIGINT *a, 
    BIGINT *b
);

// c = (a + b) mod p
void  mpz_add_mod(
    BIGINT *c, 
    BIGINT *a, 
    BIGINT *b,
    BIGINT *p
);

// c = (a - b) mod p
void  mpz_sub_mod(
    BIGINT *c,
    BIGINT *a, 
    BIGINT *b,
    BIGINT *p
);

// c = (a * b) mod p
void  mpz_mul_mod(
    BIGINT *c,
    BIGINT *a,
    BIGINT *b,
    BIGINT *p
);

// c = (a ^ b) mod p
void  mpz_exp_mod(
    BIGINT *c, 
    BIGINT *a, 
    BIGINT *b, 
    BIGINT *p
);

// c = 1/a mod p
void  mpz_invert(
    BIGINT *c, 
    BIGINT *a, 
    BIGINT *p
);

void  mpz_mod_BarrettInit(
    BIGINT *c, 
    TBarrett *pbuf
);


void  mpz_BarrettSetBuf(
    TBarrett *pbuf
);

// u cmp v
S8  mpz_cmp(
    BIGINT *u, 
    BIGINT *v
);

// u cmp val
S8  mpz_cmp_ui(
    BIGINT *u,
    unsigned long val
);

U8  mpz_tstbit(
    BIGINT *u, 
    unsigned long bit_index
);

void  mpz_clrbit(
    BIGINT *u, 
    unsigned long bit_index
);

void  mpz_set_ui(
    BIGINT *u,
    unsigned long d
);

U8  mpz_odd_p(
    BIGINT *b
);

U32  mpz_get_ui(
    BIGINT *u
);

void  mpz_add_ui(
    BIGINT *c,
    BIGINT *a, 
    unsigned long b
);
void  mpz_add_ui_mod(
    BIGINT *c, 
    BIGINT *a,
    unsigned long b, 
    BIGINT *p
);

void  mpz_sub_ui(
    BIGINT *c,
    BIGINT *a,
    unsigned long b
);

void  mpz_sub_ui_mod(
    BIGINT *c, 
    BIGINT *a,
    unsigned long b,
    BIGINT *p
);

// c = a*2 mod p
void  mpz_mul_2_mod(
    BIGINT *c, 
    BIGINT *a,
    BIGINT *p
);

unsigned short  mpz_sizeinbase(
    BIGINT *u,
    unsigned long b
);

// c = a / 2^b
void  mpz_fdiv_q_2exp(
    BIGINT *c,
    BIGINT *a,
    U32 b
);

// c = a * 2^b
void  mpz_mul_2exp(
    BIGINT *c,
    BIGINT *a,
    U32 b
);

// c = a^b mod p
void  mpz_pow_ui(
    BIGINT *c,
    BIGINT *a,
    U32 b,
    BIGINT *p
);

//int  gmp_printf(const char *fmt, void *data);
int  gmp_printf (
    const char *fmt, ...
);

void  mpz_clear(
    BIGINT *a
);

void  mpz_tdiv_q_2exp(
    BIGINT *c,
    BIGINT *a,
    U32 b
);

void  mpz_powm(
    BIGINT *c, 
    BIGINT *a, 
    BIGINT *b, 
    BIGINT *p
);

void  mpz_set_str(
    BIGINT *c, 
    unsigned char *str ,
    char temp
);

#ifdef __cplusplus
}
#endif

#endif
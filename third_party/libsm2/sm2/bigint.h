#ifndef __BIGINT_H__
#define __BIGINT_H__

//#include "const.h"
#include "mpz_t.h"
#include "mpzlib.h"

#define zp_add_mod(x, a, b) mpz_add_mod(x, a, b, FIELD_P)
#define zp_sub_mod(x, a, b) mpz_sub_mod(x, a, b, FIELD_P)
#define zp_add_mod_p(x, a, b, p)  mpz_add_mod(x, a, b, p)
#define zp_sub_mod_p(x, a, b, p)  mpz_sub_mod(x, a, b, p)
#define zp_mul_mod_p(x, a, b, p) mpz_mul_mod(x, a, b, p)
#define zp_mul_mod(x, a, b) mpz_mul_mod(x, a, b, FIELD_P)
#define zp_pow_ui(x, a, b) mpz_pow_ui(x, a, b, FIELD_P)
#define zp_add_ui_mod(x, a, b) mpz_add_ui_mod(x, a, b, FIELD_P)
#define zp_sub_ui_mod(x, a, b) mpz_sub_ui_mod(x, a, b, FIELD_P)
	
#define I2OSP(a, b, c)			I2BSP(a, b*8, c)


#ifdef __cplusplus
extern "C" {
#endif


void BS2IP(
    mpz_ptr z,
    U8 * c,
    int n
);

U8 I2BSP(
    U8 * data,
    U16 n, 
    mpz_ptr z
);

#ifdef __cplusplus
}
#endif
#endif
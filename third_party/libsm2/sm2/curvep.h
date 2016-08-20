#ifndef __CURVE_H__
#define __CURVE_H__
#include "sm2.h"
#include "const.h"
#include "bigint.h"

#ifdef __cplusplus
extern "C" {
#endif

void point_proj_printf(
    point_proj_ptr P
);

void point_affn_printf(
    point_affn_ptr P
);

void point_proj_init(
    point_proj_ptr P
);

void point_proj_clear(
    point_proj_ptr P
);

void point_proj_set_0(
    point_proj_ptr P
);

void point_affn_set(
    point_affn_ptr R, 
    point_affn_ptr P
);

void point_proj_set_affn(
    point_proj_ptr R, 
    point_affn_ptr P
);

void point_affn_init(
    point_affn_ptr P
);

void point_affn_clear(
    point_affn_ptr P
);

char point_affn_cmp(
    point_affn_ptr P,
    point_affn_ptr Q
);

void point_proj_double(
    point_proj_ptr Q
);
void point_mix_add(
    point_proj_ptr Q, 
    point_affn_ptr P
);

void point_proj_mul(
    point_proj_ptr R, 
    point_affn_ptr P, 
    mpz_ptr n
);

void point_proj2affn(
    point_affn_ptr R,
    point_proj_ptr P
);

void point_affn_mul(
    point_affn_ptr R, 
    point_affn_ptr P,
    mpz_ptr n
);

void point_affn_add(
    point_affn_ptr R, 
    point_affn_ptr P, 
    point_affn_ptr Q
);

S32 OS2ECP_XY(
    point_affn_ptr P, 
    U8 *data, int len
);

S32 OS2ECP(
	point_affn_ptr P,
	U8 *data,
	int len
);

S32 OS2IP(
	mpz_ptr z, 
	U8 *str,
	int n
);

U32 EC2OSP(
	U8 *data, 
	S32 *len, 
	point_affn_ptr P,
	int ecp
);

U32 EC2OSP_XY(
	U8 *data, 
	S32 *len, 
	point_affn_ptr P
);

U32 EC2OSP_XY(
	U8 *data, 
	S32 *len, 
	point_affn_ptr P
);

U32 EC2OSP_XY(
	U8 *data, 
	S32 *len, 
	point_affn_ptr P
);

#ifdef __cplusplus
}
#endif
#endif


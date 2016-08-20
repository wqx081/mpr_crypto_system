#ifndef _EICL_MUL_H
#define _EICL_MUL_H

#define EICL
//#define NEW_MUL
#include "mpzlib.h"
#include "NN.h"
#include "NNMLib.h"

#ifdef __cplusplus
extern "C" {
#endif

void eicl_mul(
    BIGINT *c, 
    BIGINT *a, 
    BIGINT *b
);

#ifdef NO_SUPPOST64
void NN_DigitMult(
    NN_DIGIT a, 
    NN_DIGIT b , 
    NN_DIGIT* ret
);
#else
NN_DOUBLE_DIGIT  NN_DigitMult(
    NN_DIGIT a, 
    NN_DIGIT b
);
#endif

void	eicl_add(
    BIGINT *c,
    BIGINT *a, 
    BIGINT *b
);

void	eicl_sub(
    BIGINT *c, 
    BIGINT *a,
    BIGINT *b
);

int		eicl_cmp(
    BIGINT *a, 
    BIGINT *b
);

void	eicl_mul_mod(
    BIGINT *c, 
    BIGINT *a,
    BIGINT *b,
    BIGINT *n
);

void	eicl_add_mod(
    BIGINT *c, 
    BIGINT *a, 
    BIGINT *b,
    BIGINT *n
);

void	eicl_sub_mod(
    BIGINT *c, 
    BIGINT *a, 
    BIGINT *b, 
    BIGINT *n
);

#ifdef __cplusplus
}
#endif

#endif
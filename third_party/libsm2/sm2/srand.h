#ifndef __RAND_GEN_H__
#define __RAND_GEN_H__
#include "const.h"
#include "bigint.h"

#ifdef __cplusplus
extern "C" {
#endif

U8 random_string(
    U8 * rand, 
    int len
);

void random_limit (
    mpz_ptr x, 
    mpz_ptr p
);

#ifdef __cplusplus
}
#endif


#endif


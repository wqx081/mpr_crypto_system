#ifndef __CONSTANT_H__
#define __CONSTANT_H__
#include "mpz_t.h"



//#define GMP_SUPPORTED		1		
#define NM_SUPPORTED		2	
#define OLYM_TIANLANG		4

#define MUL_BITS

#ifndef MUL_BITS
//#define USE_ECC_256
/*
#define INDEX S16
#ifdef USE_ECC_256
#define FIELD_SIZE_BITS			256
#else
#define FIELD_SIZE_BITS			192
#endif
#define FIELD_SIZE_IN_BYTES		((FIELD_SIZE_BITS+7)/8)
*/
#else
extern int eccbits;
#define FIELD_SIZE_BITS			eccbits
#define FIELD_SIZE_IN_BYTES		((FIELD_SIZE_BITS+7)/8)
#endif


#ifndef NULL
#define NULL ((void *)0)
#endif




#endif

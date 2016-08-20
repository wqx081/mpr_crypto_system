#ifndef _NN_H_
#define _NN_H_
#include "mpz_t.h"
#define MODINVOPT

#define KEY_BIT_LEN 256
#define HYBRID_MUL_WIDTH5
#define BARRETT_REDUCTION



typedef U32 NN_DIGIT;

typedef U8 NN_UINT;
typedef U16 NN_UINT2;

/* Length of digit in bits */
#define NN_DIGIT_BITS 32

/* Length of digit in bytes */
#define NN_DIGIT_LEN (NN_DIGIT_BITS/8)

/* Maximum value of digit */
#define MAX_NN_DIGIT 0xffffffff


#define KEYDIGITS (KEY_BIT_LEN/NN_DIGIT_BITS) //5

/* Maximum length in digits */
#define MAX_NN_DIGITS (KEYDIGITS+1)

/* buffer size
 *should be large enough to hold order of base point
 */
#define NUMWORDS MAX_NN_DIGITS


#define Barrett TBarrett


#endif

#ifndef _MPZ_T_H
#define _MPZ_T_H

#ifndef ECC_CONST
#define ECC_CONST
#define ECC_WORDSIZE	32
#define ECC_NUMBITS		256
#define	ECC_NUMWORD		(ECC_NUMBITS/ECC_WORDSIZE)
#define ECC_MAXLONG		(ECC_NUMWORD+1)
#define ECC_BYTENUM		(ECC_NUMBITS/8)
#define ECC_LSWORD      ECC_NUMWORD      
#endif

#define MAX_FIELD_BYTES ECC_BYTENUM


#ifndef S8
#define S8 signed char
#endif
#ifndef U8
#define U8 unsigned char
#endif
#ifndef S16
#define S16 short
#endif

#ifndef U16
#define U16 unsigned short
#endif

#ifndef S32
#define S32 int
#endif
#ifndef U32
#define U32 unsigned int
#endif

#ifndef NO_SUPPOST64
#ifndef S64
#define S64 long long 
#endif
#ifndef U64
#define U64 unsigned long long
#endif
#endif

typedef U32 NN_DIGIT ;

#ifndef NO_SUPPOST64
typedef U64 NN_DOUBLE_DIGIT;
#endif
typedef struct {
	U32 data[ECC_MAXLONG];
}BIGINT;

typedef BIGINT BIGINT_t[1];
typedef BIGINT*  BIGINT_ptr;

typedef struct{
	U32 mu[2*ECC_MAXLONG+1];
	U8 mu_len;
	U8 km;
} TBarrett;

#define mpz_t	BIGINT_t
#define mpz_ptr	BIGINT_ptr

#define _mpz_t     BIGINT_t
#define _mpz_p     BIGINT_ptr
#endif

#include "stdafx.h"
#include "const.h"
#include "bigint.h"
#include "paramp.h"


void HCMD_CALC(
    U32 *n, 
    U32 nlen
)
{
	mpz_mod_BarrettInit((BIGINT *)n, 0);
}


 S32 OS2IP(
    mpz_ptr z, 
    U8 *str,
    int n
)
{
	BS2IP(z, str, n*8);
	return 0;
}


 void BS2IP(
    mpz_ptr z,
    U8 * c,
    int n
)
{
	U16 i = 0;
	U16 bs= n/8;
	U8 ls= n%8;
	U8 j;
	
	mpz_set_ui(z, 0);
	while(i < bs) {
		mpz_mul_2exp(z, z, 8);
		mpz_add_ui(z, z, (c[i]));
		i++;
	}
	
	if(ls){
		mpz_mul_2exp(z, z, ls);
		j=c[i];
		j=j>>(8-ls);
		mpz_add_ui(z, z, j);
	}
}

U8 I2BSP(
    U8 * data,
    U16 n,
    mpz_ptr z
)
{
	S16 bytes, bpad;
	U32 l;
	U8 * c=data;
	S16 i;
#ifdef USE_GLOBAL_MEM
	mpz_ptr x=(mpz_ptr)(mIBC->pBuffer[mIBC->index++]);
#else
	mpz_t x;
#endif
	bytes=mpz_sizeinbase(z, 2);
	if(n<bytes)
		return 1;
	bytes=(bytes+7)/8;
	n=(n+7)/8;
	bpad=n-bytes;
	
	mpz_init(x);
	mpz_set(x, z);
	i=n-1;
	while(i >= bpad) {
		l = mpz_get_ui(x);
		c[i] = (U8) l;
		mpz_tdiv_q_2exp(x, x, 8);
		i--;
	}
	mpz_clear(x);
	for(i=0; i<bpad; i++)
		c[i]=(U8) 0;
	
#ifdef USE_GLOBAL_MEM
	mIBC->index--;
#endif
	return 0;
}

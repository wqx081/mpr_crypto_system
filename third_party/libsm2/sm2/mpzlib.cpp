#include "stdafx.h"
#include "mpzlib.h"
#include "NN.h"
#include "NNMLib.h"
#include "eicl_mul.h"
#include "dbg_print.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
void mpz_init(
    BIGINT *a
)
{
	DBG_DEF("mpz_init Enter.");
	NN_AssignZero((NN_DIGIT *)a, ECC_MAXLONG);
	DBG_DEF("mpz_init Exit.");
}

void  mpz_set(
    BIGINT *c,
    BIGINT *a
)
{
	DBG_DEF("mpz_set Enter.");
	NN_Assign((NN_DIGIT *)c, (NN_DIGIT *)a, ECC_MAXLONG);
	DBG_DEF("mpz_set Exit.");
}

// c = a + b
void  mpz_add(
    BIGINT *c,
    BIGINT *a,
    BIGINT *b
)
{
	DBG_DEF("mpz_add Enter.");
#ifdef EICL
	eicl_add(c, a, b);
#else
	NN_Add((NN_DIGIT *)c, (NN_DIGIT *)a, (NN_DIGIT *)b, ECC_MAXLONG);
#endif
	DBG_DEF("mpz_add Exit.");
}

// c = a - b
void  mpz_sub(
    BIGINT *c, 
    BIGINT *a,
    BIGINT *b
)
{
	DBG_DEF("mpz_sub Enter.");
#ifdef EICL
	eicl_sub(c, a, b);
#else
	NN_Sub((NN_DIGIT *)c, (NN_DIGIT *)a, (NN_DIGIT *)b, ECC_MAXLONG);
#endif
	DBG_DEF("mpz_sub Exit.");
}

// c = a * b
void  mpz_mul(
    BIGINT *c,
    BIGINT *a,
    BIGINT *b
)
{
	DBG_DEF("mpz_mul Enter.");
//	BIGINT_t d ;
//	mpz_clear(d);
#ifdef EICL
	eicl_mul(c, a, b);
#else
	NN_Mult((NN_DIGIT *)c, (NN_DIGIT *)a, (NN_DIGIT *)b, ECC_MAXLONG);
#endif
//	mpz_set(c ,d);

	DBG_DEF("mpz_mul Exit.");
}

// c = a mod b
void  mpz_mod(
    BIGINT *c, 
    BIGINT *a,
    BIGINT *b
)
{
	NN_UINT adigits, bdigits;

	DBG_DEF("mpz_mod Enter.");
	adigits = (NN_UINT)NN_Digits((NN_DIGIT *)a, ECC_MAXLONG);
	bdigits = (NN_UINT)NN_Digits((NN_DIGIT *)b, ECC_MAXLONG);

	if(adigits < bdigits)
	{
		NN_Assign((NN_DIGIT *)c, (NN_DIGIT *)a, adigits);
		DBG_DEF("mpz_mod Exit.");
		return;
	}

	NN_Mod((NN_DIGIT *)c, (NN_DIGIT *)a, adigits, (NN_DIGIT *)b, bdigits);
	DBG_DEF("mpz_mod Exit.");
}

#if 1
// c = (a + b) mod p
void  mpz_add_mod(
    BIGINT *c,
    BIGINT *a,
    BIGINT *b,
    BIGINT *p
)
{
	DBG_DEF("mpz_add_mod Enter.");
	DBG_INF("the input data 'a' is: ", a);
	DBG_INF("the input data 'b' is: ", b);
	DBG_INF("the input data 'p' is: ", p);
#ifdef EICL
	eicl_add_mod(c, a, b, p);
#else
	NN_ModAdd((NN_DIGIT *)c, (NN_DIGIT *)a, (NN_DIGIT *)b, (NN_DIGIT *)p, ECC_MAXLONG);
#endif
	DBG_INF("the output data 'c' is: ", c);
	DBG_DEF("mpz_add_mod Exit.");
}
#endif

// c = (a - b) mod p
void  mpz_sub_mod(
    BIGINT *c,
    BIGINT *a,
    BIGINT *b,
    BIGINT *p
)
{
	DBG_DEF("mpz_sub_mod Enter.");
	DBG_INF("the input data 'a' is: ", a);
	DBG_INF("the input data 'b' is: ", b);
	DBG_INF("the input data 'p' is: ", p);
#ifdef EICL
	eicl_sub_mod(c, a, b, p);
#else
	NN_ModSub((NN_DIGIT *)c, (NN_DIGIT *)a, (NN_DIGIT *)b, (NN_DIGIT *)p, ECC_MAXLONG);
#endif
	DBG_INF("the output data 'c' is: ", c);
	DBG_DEF("mpz_sub_mod Exit.");
}

// c = (a * b) mod p

void  mpz_mul_mod(
    BIGINT *c,
    BIGINT *a,
    BIGINT *b, 
    BIGINT *p
)
{
	DBG_DEF("mpz_mul_mod Enter.");
	DBG_INF("the input data 'a' is: ", a);
	DBG_INF("the input data 'b' is: ", b);
	DBG_INF("the input data 'p' is: ", p);
	NN_ModMult((NN_DIGIT *)c, (NN_DIGIT *)a, (NN_DIGIT *)b, (NN_DIGIT *)p, ECC_MAXLONG);
	//eicl_mul_mod(c, a, b, p);
	DBG_INF("the output data 'c' is: ", c);
	DBG_DEF("mpz_mul_mod Exit.");
}


// c = (a^b) mod p
// mpz_powm
void  mpz_exp_mod(
    BIGINT *c, 
    BIGINT *a,
    BIGINT *b,
    BIGINT *p
)
{
	DBG_DEF("mpz_exp_mod Enter.");
	DBG_INF("the input data 'a' is: ", a);
	DBG_INF("the input data 'b' is: ", b);
	DBG_INF("the input data 'p' is: ", p);
	NN_ModExp((NN_DIGIT *)c, (NN_DIGIT *)a, (NN_DIGIT *)b, ECC_MAXLONG, (NN_DIGIT *)p, ECC_MAXLONG);
	DBG_INF("the output data 'c' is: ", c);
	DBG_DEF("mpz_exp_mod Exit.");
}

// c = 1/a mod p
void  mpz_invert(
    BIGINT *c,
    BIGINT *a,
    BIGINT *p
)
{
	DBG_DEF("mpz_invert Enter.");
	DBG_INF("the input data 'a' is: ", a);
	DBG_INF("the input data 'p' is: ", p);
	NN_ModInv((NN_DIGIT *)c, (NN_DIGIT *)a, (NN_DIGIT *)p, ECC_MAXLONG);
	DBG_INF("the output data 'c' is: ", c);
	DBG_DEF("mpz_invert Exit.");
}

void  mpz_mod_BarrettInit(
    BIGINT *c, 
    TBarrett *pbuf
)
{
	NN_ModBarrettInit((NN_DIGIT *)c, ECC_MAXLONG, (Barrett *)pbuf);
}

void  mpz_BarrettSetBuf(
    TBarrett *pbuf
)
{
	NN_BarrettSetBuf((Barrett *)pbuf);
}

S8  mpz_cmp(
    BIGINT *u,
    BIGINT *v
)
{
#ifdef EICL
	return eicl_cmp(u, v);
#else
	return NN_Cmp((NN_DIGIT *)u, (NN_DIGIT *)v, ECC_MAXLONG);
#endif
}

S8  mpz_cmp_ui(
    BIGINT *u,
    unsigned long val
)
{
	U8 i;

	if(u->data[0] > val)
		return 1;
	
	for(i=1; i<ECC_MAXLONG; i++)
	{
		if(u->data[i] != 0)
			return 1;
	}

	if(u->data[0] == val)
		return 0;
	else
		return -1;
}

U8  mpz_tstbit(
    BIGINT *u, 
    unsigned long bit_index
)
{
	U32 i;
	i =	(u->data[bit_index/ECC_WORDSIZE] & ((U32)1 << (bit_index % ECC_WORDSIZE)));
	if(i)
		return 1;
	else
		return 0;
}

void  mpz_clrbit(
    BIGINT *u,
    unsigned long bit_index
)
{
	u->data[bit_index/ECC_WORDSIZE] &= ~(1 << (bit_index % ECC_WORDSIZE));
}

void  mpz_set_ui(
    BIGINT *u, 
    unsigned long d
)
{
	mpz_init(u);
	u->data[0] = d;
}

U8  mpz_odd_p(
    BIGINT *b
)
{
    return (b->data[0] & (U32)1);
}

U32  mpz_get_ui(
    BIGINT *u
)
{
	return u->data[0];
}

void  mpz_add_ui(
    BIGINT *c, 
    BIGINT *a, 
    unsigned long b
)
{
	U32 carry, ai;
	U8 i;

	DBG_DEF("mpz_add_ui Enter.");

	ai = a->data[0] + b;
	carry = (ai < b ? 1 : 0);
	c->data[0] = ai;

	for(i=1; i<ECC_MAXLONG; i++)
	{
		ai = a->data[i] + carry;
		carry = (ai < carry ? 1 : 0);
		c->data[i] = ai;
	}

	DBG_DEF("mpz_add_ui Exit.");
	return;
}

void  mpz_add_ui_mod(
    BIGINT *c,
    BIGINT *a, 
    unsigned long b, 
    BIGINT *p
)
{
	mpz_add_ui(c, a, b);
	if(mpz_cmp(c, p) >= 0)
		mpz_sub(c, c, p);
	return;
}

// c = a - b
void  mpz_sub_ui(
    BIGINT *c, 
    BIGINT *a, 
    unsigned long b
)
{
	U32 borrow, ai;
	U8 i;

	DBG_DEF("mpz_sub_ui Enter.");

	ai = a->data[0] - b;
	borrow = ((ai > (MAX_NN_DIGIT - b)) ? 1 : 0);
	c->data[0] = ai;

	for(i=1; i<ECC_MAXLONG; i++)
	{
		ai = a->data[i] - borrow;
		borrow = ((ai > (MAX_NN_DIGIT - borrow)) ? 1 : 0);
		c->data[i] = ai;
	}

	DBG_DEF("mpz_sub_ui Exit.");

}

void  mpz_sub_ui_mod(BIGINT *c, BIGINT *a, unsigned long b, BIGINT *p)
{
	if(mpz_cmp_ui(a, b) >= 0)
		mpz_sub_ui(c, a, b);
	else
	{
		mpz_add(c, a, p);
		mpz_sub_ui(c, c, b);
	}
	return;
}

void  mpz_mul_2_mod(
    BIGINT *c, 
    BIGINT *a, 
    BIGINT *p
)
{
	DBG_DEF("mpz_mul_2_mod Enter.");

	mpz_add(c, a, a);
	mpz_mod(c, c, p);

	DBG_DEF("mpz_mul_2_mod Exit.");
}

U16  mpz_sizeinbase(
    BIGINT *u,
    unsigned long b
)
{
	U32 i;

	if(b == 2) {
		i = NN_Digits((NN_DIGIT *)u, ECC_MAXLONG);
		return NN_Bits((NN_DIGIT *)u, i);
	}
	else
		return 0;
}

// c = a / 2^b
// assume the length of a is < ECC_MAXLONG
void  mpz_fdiv_q_2exp(
    BIGINT *c,
    BIGINT *a,
    U32 b
)
{
	//NN_RShift((NN_DIGIT *)c, (NN_DIGIT *)a, b, ECC_MAXLONG);
	U32 i, temp, carry;
	S32 j;
	U8 t;

	DBG_DEF("mpz_fdiv_q_2exp Enter.");
	DBG_INF("the input data 'a' is: ", a);
	//DBG_INF("the input data 'b' is: ", (unsigned char *)b);

	i = b/ECC_WORDSIZE;	// 要移动的字数
	b %= ECC_WORDSIZE;	// 要移动的位数
	t = ECC_WORDSIZE - b;
	carry = 0;

	if(i)
	{
		j = 0;
		if(i < ECC_MAXLONG)
		{
			for(j=0; j<(S32)(ECC_MAXLONG-i); j++)
				c->data[j] = a->data[j+i];
		}
		while(j < ECC_MAXLONG)
			c->data[j++] = 0;
	}
	else
		mpz_set(c, a);

	for(j=(S32)(ECC_MAXLONG-1); j>=0; j--)
	{
		temp = c->data[j];
		c->data[j] = (temp>>b) | carry;
		carry = b?(temp<<t):0;
	}

	DBG_INF("the output data 'c' is: ", c);
	DBG_DEF("mpz_fdiv_q_2exp Exit.");

}

// c = a * 2^b
// assume the length of a is < ECC_MAXLONG
void  mpz_mul_2exp(
    BIGINT *c, 
    BIGINT *a,
    U32 b
)
{
	//NN_LShift((NN_DIGIT *)c, (NN_DIGIT *)a, b, ECC_MAXLONG);
	U32 i, j, temp, carry;
	U8 t;

	DBG_DEF("mpz_mul_2exp Enter.");
	DBG_INF("the input data 'a' is: ", a);

	i = b/ECC_WORDSIZE;	// 要移动的字数
	b %= ECC_WORDSIZE;	// 要移动的位数
	temp = 0;

	if(i)
	{
		j = ECC_MAXLONG - 1;
		if(i < ECC_MAXLONG)
		{
			for(j=(ECC_MAXLONG-1); j>=i; j--)
				c->data[j] = a->data[j-i];
		}

		for(i=0; i<j+1; i++)
			c->data[i] = 0;
	}
	
	t = ECC_WORDSIZE - b;
	carry = 0;
	for(j=0; j<ECC_MAXLONG; j++)
	{
		temp = c->data[j];
		c->data[j] = (temp << b) | carry;
		carry = b?(temp>>t):0;
	}

	DBG_INF("the output data 'c' is: ", c);
	DBG_DEF("mpz_mul_2exp Exit.");

}

// c = a^b mod p
void  mpz_pow_ui(
    BIGINT *c, 
    BIGINT *a, 
    U32 b,
    BIGINT *p
)
{
//	DBG_DEF("mpz_pow_ui Enter.");
//	DBG_INF("the input data 'a' is: ", a);
//	DBG_INF("the input data 'p' is: ", p);

	NN_DIGIT bi = (NN_DIGIT) b;
	NN_ModExp((NN_DIGIT *)c, (NN_DIGIT *)a, (NN_DIGIT *)&bi, 1, (NN_DIGIT *)p, ECC_MAXLONG);

	DBG_INF("the output data 'c' is: ", c);
	DBG_DEF("mpz_pow_ui Exit.");
}

void  mpz_clear(
    BIGINT *a
)
{
	DBG_DEF("mpz_clear Enter.");
	mpz_init(a);
	DBG_DEF("mpz_clear Exit.");
}

void  mpz_tdiv_q_2exp(
    BIGINT *c,
    BIGINT *a,
    U32 b
)
{
//	DBG_DEF("mpz_tdiv_q_2exp Enter.");
	mpz_fdiv_q_2exp(c, a, b);
//	DBG_DEF("mpz_tdiv_q_2exp Exit.");
}

void  mpz_powm(BIGINT *c, BIGINT *a, BIGINT *b, BIGINT *p)
{
//	DBG_DEF("mpz_powm Enter.");
	mpz_exp_mod(c, a, b, p);
//	DBG_DEF("mpz_powm Exit.");
}

// input must be '0'~'9' || 'a'~'f' || 'A'~'F'
char asc2hex_byte(
    char asc
)
{
	if(asc>='0' && asc<='9')
		return (asc - 0x30);
	else if(asc>='a' && asc<='f')
		return (asc - 0x57);
	else
		return (asc - 0x37);
}

void asc2hex(
    char *hex,
    int *hexlen,
    char *asc, 
    int asclen
)
{
	int i, sl, dl;
	char *p, *q, t;

	p = hex, q = asc;
	sl = asclen;
	i = 0;

	t = asc2hex_byte(*q);

	q++; i++;

	if(sl%2) 
	{
		dl = sl/2 + 1;
		*p = t;
	}
	else
	{
		dl = sl / 2;
		*p = (t<<4) | asc2hex_byte(*q);
		q++;
		i++;
	}
	p++;

	for(; i<sl; i+=2)
	{
		t = asc2hex_byte(*q);
		q++;
		*p = (t<<4) | asc2hex_byte(*q);
		q++;
		p++;
	}
	*hexlen = dl;
}

void hex2asc(
    char *asc, 
    int *asclen,
    char *hex, 
    int hexlen
)
{
	char *ph, *pa;
	char a, b;

	ph = hex; pa = asc;
	ph += (hexlen-1);

	for(;ph>=hex;ph--)
	{
		a = ((*ph & 0xF0)>>4);
		if(a>=0 && a<=9)
			a += 0x30;
		else
			a += 0x37;
		b = (*ph & 0x0F);
		if(b>=0 && b<=9)
			b += 0x30;
		else
			b += 0x37;

		*pa++ = a;
		*pa++ = b;
	}
	*asclen = hexlen*2;

}

int  gmp_printf (
    const char *fmt, ...
)
{
	va_list ap;
	int i, al;
	BIGINT *bp;

	va_start(ap, fmt);
	bp = va_arg(ap, BIGINT *);
	va_end(ap);

	i = mpz_sizeinbase(bp, 2);
	al = i / 8;
	if(i%8)
		al++;
	
	printf("%s", fmt);
	for(i=al-1; i>=0; i--)
		printf("%02x", (unsigned char)*((unsigned char *)bp+i));
	printf("\n");

	return 1;
}



void  mpz_set_str(
    BIGINT *c, 
    unsigned char *str, 
    char temp
)
{
	int i, j, str_size;
	unsigned char *p, *q;

	str_size = strlen((const char *)str);

	p = str + str_size - 1;
	q = (unsigned char *)c;

	do
	{
		i = asc2hex_byte(*p); p--;
		if(p < str)
		{
			*q = i;
			break;
		}
		j = asc2hex_byte(*p); p--;
		*q = (j<<4) | i; q++;

	}while(p>=str);

	return;
}
#include "stdafx.h"
#include "eicl_mul.h"
#include "string.h"

// WORDMASK = 0xFFFFFFFF
#define WORDMASK ( ~(U32)0 )
// HALFWORDMASK = 0x0000FFFF
#define HALFWORDMASK     (WORDMASK >> (ECC_WORDSIZE/2))

int  eicl_cmp(
    BIGINT *a, 
    BIGINT *b
)
{
	register U16 alen, blen;
	alen = NN_Digits((NN_DIGIT *)a, ECC_MAXLONG);
	blen = NN_Digits((NN_DIGIT *)b, ECC_MAXLONG);
    
	if(alen > blen) 
        return 1;
	if(alen < blen) 
        return -1;
    
	while(alen-->0)
	{
		if(a->data[alen] > b->data[alen]) return 1;
		if(a->data[alen] < b->data[alen]) return -1;
	}
	return 0;
}

// c = a + b
void  eicl_add(
    BIGINT *c, 
    BIGINT *a, 
    BIGINT *b
)
{
	register int pos, len, al, bl;
	unsigned int sum, carry, msb;
	U32 *p;

	// perform addition by using the smaller's length, then ....
	al = NN_Digits((NN_DIGIT *)a, ECC_MAXLONG);
	bl = NN_Digits((NN_DIGIT *)b, ECC_MAXLONG);
	len = (al > bl) ? bl : al;
	carry = 0;
    
	for(pos=0; pos<len; ++pos)
	{
		sum = (a->data[pos] & (WORDMASK >> 1)) + (b->data[pos] & (WORDMASK >> 1)) + carry;
		msb = (a->data[pos] >> (ECC_WORDSIZE - 1)) + (b->data[pos] >> (ECC_WORDSIZE - 1)) + (sum >> (ECC_WORDSIZE-1));
		carry = (msb & 2) != 0;
		if(msb & 1)
			c->data[pos] = sum | ((U32)1 << (ECC_WORDSIZE-1));
		else
			c->data[pos] = sum & (WORDMASK >> 1);
	}
	// just add the carry-out (0/1) for the residue, if any
	if(al > bl)
	{
		len = al;
		p = a->data;
	}
	else
	{
		len = bl;
		p = b->data;
	}
	for(; pos < len; ++pos)
	{
		if(carry == 0)
			c->data[pos] = p[pos];
		else
		{
			sum = (p[pos] & (WORDMASK >> 1)) + carry;
			msb = (p[pos] >> (ECC_WORDSIZE - 1)) + (sum >> (ECC_WORDSIZE - 1));
			carry = (msb & 2) != 0;
			if(msb & 1)
				c->data[pos] = sum | ((U32)1 << (ECC_WORDSIZE - 1));
			else
				c->data[pos] = sum & (WORDMASK >> 1);
		}
	}
	c->data[pos++] = carry;

	for(; pos<ECC_MAXLONG; pos++)
		c->data[pos] = 0;

}

// c = a - b;
void  eicl_sub(
    BIGINT *c,
    BIGINT *a,
    BIGINT *b
)
{
	int pos;
	register U32 msb, sum;
	U32 aneg, carry;
	long len;

	len = NN_Digits((NN_DIGIT *)b, ECC_MAXLONG);

	carry = 1;
	for(pos=0; pos<len; ++pos)
	{
		sum = (a->data[pos] & 0x7FFFFFFF) + ((aneg=~b->data[pos]) & 0x7FFFFFFF) + carry;
		msb = (a->data[pos] >> 31) + (aneg >> 31) + (sum >> 31);
		carry = ((msb & 2) != 0);
		if(msb & 1)
			c->data[pos] = sum | 0x80000000;
		else
			c->data[pos] = sum & 0x7FFFFFFF;
	}
	// just add the carry-out (0/1) for the residue, if any
	len = NN_Digits((NN_DIGIT *)a, ECC_MAXLONG);
	for(; pos<len; ++pos)
	{
		sum = (a->data[pos] & 0x7FFFFFFF) + carry + 0x7FFFFFFF;
		msb = (a->data[pos] >> 31) + (sum >> 31) + 1;
		carry = ((msb & 2) != 0);
		if(msb & 1)
			c->data[pos] = sum | 0x80000000;
		else
			c->data[pos] = sum & 0x7FFFFFFF;
	}

	for(; pos<ECC_MAXLONG; pos++)
		c->data[pos] = 0;
}
#ifdef NO_SUPPOST64

void  eicl_mul(
    BIGINT *c, 
    BIGINT *a,
    BIGINT *b
)
{
	U32 C, S, sum, carry;
	U32 curA, curB, curC;
	U32 a0b0, a0b1, a1b0, a1b1, cAand, cAshift, cBand, cBshift;
	U32 *cval, *aval, *bval;
	int i, j, alen, blen;

	cval = (U32 *)c;
	aval = (U32 *)a;
	bval = (U32 *)b;

	alen = NN_Digits((NN_DIGIT *)a, ECC_MAXLONG);
	blen = NN_Digits((NN_DIGIT *)b, ECC_MAXLONG);
	for(i=0; i<blen; i++)
	{
		C = 0;
		curB = bval[i];
		cBand = curB & HALFWORDMASK;
		cBshift = curB >> (ECC_WORDSIZE/2);
		for(j=0; j<alen; j++)
		{
			//(C,S) = a[j]*b[j] + c[i+j] + C
			curA = aval[j];
			curC = cval[i+j];

			cAand = curA & HALFWORDMASK;
			cAshift = curA >> (ECC_WORDSIZE/2);
			a0b0 = cAand * cBand;
			a1b0 = cAshift * cBand;
			a0b1 = cAand *cBshift;
			a1b1 = cAshift * cBshift;

			S = (a0b0 & HALFWORDMASK) + (curC & HALFWORDMASK) + (C & HALFWORDMASK);
			carry = S >> (ECC_WORDSIZE/2);
			S = S & 0x0000FFFF;
			sum = (a0b1 & HALFWORDMASK) + (a1b0 & HALFWORDMASK) + (a0b0 >> (ECC_WORDSIZE/2))
				+ (curC >> (ECC_WORDSIZE/2)) + (C >> (ECC_WORDSIZE/2)) + carry;
			carry = sum >> (ECC_WORDSIZE/2);
			S = S | (sum << (ECC_WORDSIZE/2));

			sum = (a0b1 >> (ECC_WORDSIZE/2)) + (a1b0 >> (ECC_WORDSIZE/2)) +
				(a1b1 & HALFWORDMASK) + carry;
			carry = sum >> (ECC_WORDSIZE/2);
			C = sum & HALFWORDMASK;
			
			sum = (a1b1 >> (ECC_WORDSIZE/2)) + carry;
			C = C | (sum << (ECC_WORDSIZE/2));

			cval[i+j] = S;
		}
		cval[i+j] = C;
	}
}

#else
void  eicl_mul(
    BIGINT *c, 
    BIGINT *a,
    BIGINT *b
)
{
	U32 carry;
	U32 curA, curB, curC;
	U32 *cval, *aval, *bval;
	int i, j, alen, blen;
	U64 T;
	U32 TH, TL, TT;
	
	cval = (U32 *)c;
	aval = (U32 *)a;
	bval = (U32 *)b;
	
	alen = NN_Digits((NN_DIGIT *)a, ECC_MAXLONG);
	blen = NN_Digits((NN_DIGIT *)b, ECC_MAXLONG);
	for(i=0; i<blen; i++)
	{
		TH = 0;
		curB = bval[i];
		for(j=0; j<alen; j++)
		{
			carry = 0;
			
			//(C,S) = a[j]*b[j] + c[i+j] + C
			curA = aval[j];
			curC = cval[i+j];
			
			//DOUBLE_MULL(TT ,TL ,curA ,  curB);
			T = (U64)curB * (U64)curA;
			TL = (U32)T;
			TT = (U32)(T >> 32);	
			
			curC += TH;
			carry += curC < cval[i+j];

			cval[i+j] = curC;
			curC += TL;
			carry += curC < cval[i+j];

			cval[i+j] = curC;
			TH = TT+ carry;
		}

		cval[i+j] = TH;
	}
}
#endif



#ifdef NO_SUPPOST64

void NN_DigitMult(
    NN_DIGIT a, 
    NN_DIGIT b ,
    NN_DIGIT* ret
)
{
	U32 C, S, sum, carry;
	U32 curA, curB, curC;
	U32 a0b0, a0b1, a1b0, a1b1, cAand, cAshift, cBand, cBshift;

	C = 0;
	curB = b;
	cBand = curB & HALFWORDMASK;
	cBshift = curB >> (ECC_WORDSIZE/2);
	
	curA = a;
	curC = 0;
	
	cAand = curA & HALFWORDMASK;
	cAshift = curA >> (ECC_WORDSIZE/2);
	a0b0 = cAand * cBand;
	a1b0 = cAshift * cBand;
	a0b1 = cAand *cBshift;
	a1b1 = cAshift * cBshift;
	
	S = (a0b0 & HALFWORDMASK) + (curC & HALFWORDMASK) + (C & HALFWORDMASK);
	carry = S >> (ECC_WORDSIZE/2);
	S = S & 0x0000FFFF;
	sum = (a0b1 & HALFWORDMASK) + (a1b0 & HALFWORDMASK) + (a0b0 >> (ECC_WORDSIZE/2))
		+ (curC >> (ECC_WORDSIZE/2)) + (C >> (ECC_WORDSIZE/2)) + carry;
	carry = sum >> (ECC_WORDSIZE/2);
	S = S | (sum << (ECC_WORDSIZE/2));
	
	sum = (a0b1 >> (ECC_WORDSIZE/2)) + (a1b0 >> (ECC_WORDSIZE/2)) +
		(a1b1 & HALFWORDMASK) + carry;
	carry = sum >> (ECC_WORDSIZE/2);
	C = sum & HALFWORDMASK;
	
	sum = (a1b1 >> (ECC_WORDSIZE/2)) + carry;
	C = C | (sum << (ECC_WORDSIZE/2));
	
	ret[0] = C ;
	ret[1] = S ;
}
#else
NN_DOUBLE_DIGIT  NN_DigitMult(
    NN_DIGIT a, 
    NN_DIGIT b
)
{
	U32 C, S, sum, carry;
	U32 curA, curB, curC;
	U32 a0b0, a0b1, a1b0, a1b1, cAand, cAshift, cBand, cBshift;
	NN_DOUBLE_DIGIT ret;

	C = 0;
	curB = b;
	cBand = curB & HALFWORDMASK;
	cBshift = curB >> (ECC_WORDSIZE/2);

	curA = a;
	curC = 0;

	cAand = curA & HALFWORDMASK;
	cAshift = curA >> (ECC_WORDSIZE/2);
	a0b0 = cAand * cBand;
	a1b0 = cAshift * cBand;
	a0b1 = cAand *cBshift;
	a1b1 = cAshift * cBshift;

	S = (a0b0 & HALFWORDMASK) + (curC & HALFWORDMASK) + (C & HALFWORDMASK);
	carry = S >> (ECC_WORDSIZE/2);
	S = S & 0x0000FFFF;
	sum = (a0b1 & HALFWORDMASK) + (a1b0 & HALFWORDMASK) + (a0b0 >> (ECC_WORDSIZE/2))
		+ (curC >> (ECC_WORDSIZE/2)) + (C >> (ECC_WORDSIZE/2)) + carry;
	carry = sum >> (ECC_WORDSIZE/2);
	S = S | (sum << (ECC_WORDSIZE/2));

	sum = (a0b1 >> (ECC_WORDSIZE/2)) + (a1b0 >> (ECC_WORDSIZE/2)) +
		(a1b1 & HALFWORDMASK) + carry;
	carry = sum >> (ECC_WORDSIZE/2);
	C = sum & HALFWORDMASK;
			
	sum = (a1b1 >> (ECC_WORDSIZE/2)) + carry;
	C = C | (sum << (ECC_WORDSIZE/2));

	ret = C;
	ret <<= ECC_WORDSIZE;
	ret |= S;
	return ret;
}
#endif


// c = a*b mod n
void  eicl_mul_mod(
    BIGINT *c, 
    BIGINT *a, 
    BIGINT *b, 
    BIGINT *n
)
{
	register int cnt, k, blen, ul;
	U32 bi, bitmask;
	U32 uarray[ECC_MAXLONG];

	NN_AssignZero((NN_DIGIT *)uarray, ECC_MAXLONG);

	ul = 1;
	blen = NN_Digits((NN_DIGIT *)b, ECC_MAXLONG);
	for(cnt=blen-1; cnt>=0; --cnt)
	{
		bi = b->data[cnt];
		for(bitmask=(U32)1<<(ECC_WORDSIZE-1); bitmask!=0; bitmask>>=1)
		{
			// shift left u, 1
			uarray[ul] = 0;
			for(k=ul-1; k>=0; --k)
			{
				uarray[k+1] |= uarray[k] >> (ECC_WORDSIZE-1);
				uarray[k] <<= 1;
			}
			if(uarray[ul])
				++ul;
			// if u>=n then u=u-n
			if(eicl_cmp((BIGINT *)uarray, n)>=0)
			{
				eicl_sub((BIGINT *)uarray, (BIGINT *)uarray, n);
				ul = NN_Digits((NN_DIGIT *)uarray, ECC_MAXLONG);
			}
			// u = u+bi*a
			if(bitmask & bi)
			{
				eicl_add((BIGINT *)uarray, a, (BIGINT *)uarray);
				ul = NN_Digits((NN_DIGIT *)uarray, ECC_MAXLONG);
				while(ul>1 && uarray[ul-1]==0)
					--ul;
			}
			// if u>=n then u=u-n
			if(eicl_cmp((BIGINT *)uarray, n)>=0)
			{
				eicl_sub((BIGINT *)uarray, (BIGINT *)uarray, n);
				ul = NN_Digits((NN_DIGIT *)uarray, ECC_MAXLONG);
			}
		}
	}
	// copy u to c and adjust length
	NN_Assign((NN_DIGIT *)c->data, (NN_DIGIT *)uarray, ul);

	for(cnt=ul; cnt<ECC_MAXLONG; cnt++)
		c->data[cnt] = 0;

	return;
}

void  eicl_add_mod(
    BIGINT *c, 
    BIGINT *a, 
    BIGINT *b, 
    BIGINT *n
)
{
	eicl_add(c, a, b);
	if(eicl_cmp(c, n) >= 0)
		eicl_sub(c, c, n);
	return;
}

void  eicl_sub_mod(
    BIGINT *c, 
    BIGINT *a, 
    BIGINT *b, 
    BIGINT *n
)
{
	if(eicl_cmp(a, b) >= 0)
		eicl_sub(c, a, b);
	else
	{
		eicl_add(c, a, n);
		eicl_sub(c, c, b);
	}
	return;
}
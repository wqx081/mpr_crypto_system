#include "stdafx.h"
#include <stdio.h>
#include "NN.h"
#include "NNMLib.h"
#include "eicl_mul.h"
#include "mpz_t.h"

#define MAX(a,b) ((a) < (b) ? (b) : (a))
#define DIGIT_MSB(x) (NN_DIGIT)(((x) >> (NN_DIGIT_BITS - 1)) & 1)
#define DIGIT_2MSB(x) (NN_DIGIT)(((x) >> (NN_DIGIT_BITS - 2)) & 3)

#define NN_ASSIGN_DIGIT(a, b, digits) {NN_AssignZero (a, digits); a[0] = b;}
#define NN_EVEN(a, digits) (((digits) == 0) || ! (a[0] & 1))

static NN_DIGIT  NN_AddDigitMult (
    NN_DIGIT *a, 
    NN_DIGIT *b,
    NN_DIGIT c,
    NN_DIGIT *d,
    NN_UINT digits
);

static NN_DIGIT  NN_SubDigitMult (
    NN_DIGIT *a,
    NN_DIGIT *b,
    NN_DIGIT c,
    NN_DIGIT *d,
    NN_UINT digits
);

static unsigned int  NN_DigitBits (
    NN_DIGIT a
);

Barrett * pBarrett;

/* Return b * c, where b and c are NN_DIGITs. The result is a NN_DOUBLE_DIGIT
*/
//#define NN_DigitMult(b, c) (NN_DOUBLE_DIGIT)(b) * (c)

// test whether the ith bit in a is one
NN_DIGIT b_testbit(
    NN_DIGIT * a, 
    S16 i
)
{
	return (*(a + (i / NN_DIGIT_BITS)) & ((NN_DIGIT)1 << (i % NN_DIGIT_BITS)));
}
  
/* Sets a = b / c, where a and c are digits.

	 Lengths: b[2].
	 Assumes b[1] < c and HIGH_HALF (c) > 0. For efficiency, c should be
	 normalized.
	 from digit.c
*/

NN_DIGIT _DivEx(
    NN_DIGIT  AHiVal, 
    NN_DIGIT  ALoVal, 
    NN_DIGIT ADivisor,  
    NN_DIGIT* AHiRet,
    NN_DIGIT* ARemainder 
)  
{  	
	// 初始化  
	NN_DIGIT result = 0;  
		char byteBit;
	
	// 判断除数是否为 0, 及高 32 位是否为 0  
	if (ADivisor == 0)  {
		return 0 ;
	}
		 
	else if (AHiVal == 0)  
	{  
		AHiRet = 0;  
		result = ALoVal / ADivisor;  
		if (ARemainder != NULL)  
			*ARemainder = ALoVal % ADivisor;  
	}  
	else  
	{  
		// 初始化  
		NN_DIGIT dwMod;  
		
		// 取高 32 位结果  
		*AHiRet   = AHiVal / ADivisor;  
		dwMod    = AHiVal % ADivisor;  
		
		// 判断最高位是否为 1  
		if (ADivisor >= 0x80000000)  
		{  
			// 初始化  
	
			char     boolBit = (dwMod >= 0x80000000);  
			NN_DIGIT dwValue = ADivisor & 0x7FFFFFFF;  
			
			// 循环操作  
			for (byteBit = 0; byteBit < 32; byteBit++)  
			{  
				result = result << 1;  
				dwMod  = (dwMod << 1) + ((ALoVal >> (31 - byteBit)) & 1);  
				//dwMod = (dwMod << 1) + ((ALoVal & _Bit32_[byteBit]) != 0);  
				
				if (boolBit)  
				{  
					result++;  
					dwMod  ^= 0x80000000;  
					dwMod  -= dwValue;  
					boolBit = (dwMod >= 0x80000000);  
				}  
				else if (dwMod >= ADivisor)  
				{  
					result++;  
					dwMod  -= ADivisor;  
				}  
				else  
					boolBit = (dwMod >= 0x80000000);  
			}  
		}  
		else  
			for (byteBit = 0; byteBit < 32; byteBit++)  
			{  
				result = result << 1;  
				dwMod  = (dwMod << 1) + ((ALoVal >> (31 - byteBit)) & 1);  
				//dwMod = (dwMod << 1) + ((ALoVal & _Bit32_[byteBit]) != 0);  
				
				if (dwMod >= ADivisor)  
				{  
					result++;  
					dwMod -= ADivisor;  
				}  
			}  
			
			// 判断是否需要余数  
			if (ARemainder != NULL)  
				*ARemainder = dwMod;  
	}  
	
	// 返回结果  
	return result;  
} 

void  NN_DigitDiv (
    NN_DIGIT *a, 
    NN_DIGIT b[2], 
    NN_DIGIT c)
{
#ifdef NO_SUPPOST64
	NN_DIGIT d , mod ,ret;
	*a = _DivEx(b[1] , b[0] , c , &d , &mod);
#else
	NN_DOUBLE_DIGIT t;
	t = (((NN_DOUBLE_DIGIT)b[1]) << NN_DIGIT_BITS) ^ ((NN_DOUBLE_DIGIT)b[0]);
	*a = (NN_DIGIT)(t/c);
#endif
}

/* Decodes character string b into a, where character string is ordered
   from most to least significant.

  Lengths: a[digits], b[len].
   Assumes b[i] = 0 for i < len - digits * NN_DIGIT_LEN. (Otherwise most
   significant bytes are truncated.)
*/
void  NN_Decode (
    NN_DIGIT *a, 
    NN_UINT digits, 
    unsigned char *b, 
    NN_UINT len)
{
    NN_DIGIT t;
    int j;
    unsigned int i, u;

    for (i = 0, j = len - 1; i < digits && j >= 0; i++) {
        t = 0;
        for (u = 0; j >= 0 && u < NN_DIGIT_BITS; j--, u += 8)
            t |= ((NN_DIGIT)b[j]) << u;
        a[i] = t;
    }

    for (; i < digits; i++)
        a[i] = 0;
}

/* Encodes b into character string a, where character string is ordered
   from most to least significant.

   Lengths: a[len], b[digits].
   Assumes NN_Bits (b, digits) <= 8 * len. (Otherwise most significant
   digits are truncated.)
 */
void  NN_Encode (
    unsigned char *a, 
    NN_UINT len, 
    NN_DIGIT *b, 
    NN_UINT digits)
{
    NN_DIGIT t;
    int j;
    unsigned int i, u;

    for (i = 0, j = len - 1; i < digits && j >= 0; i++) {
        t = b[i];
        for (u = 0; j >= 0 && u < NN_DIGIT_BITS; j--, u += 8)
            a[j] = (unsigned char)(t >> u);
    }

    for (; j >= 0; j--)
        a[j] = 0;
}

/* Assigns a = b.

   Lengths: a[digits], b[digits].
 */
void  NN_Assign (
    NN_DIGIT *a,
    NN_DIGIT *b, 
    NN_UINT digits)
{
	register NN_UINT i;
	for(i=0; i<digits; i++)
		a[i] = b[i];
	//memcpy(a, b, digits*NN_DIGIT_LEN);
}

/* Assigns a = 0.

   Lengths: a[digits].
 */
void  NN_AssignZero (
    NN_DIGIT *a, 
    NN_UINT digits
)
{
	U8 i;
	for (i = 0; i < digits; i++)
		a[i] = 0;
}

/* Computes a = b + c. Returns carry.
   a, b ,c can be same
   Lengths: a[digits], b[digits], c[digits].
 */
NN_DIGIT  NN_Add (
    NN_DIGIT *a, 
    NN_DIGIT *b, 
    NN_DIGIT *c, 
    NN_UINT digits
)  
{
	NN_DIGIT carry, ai;
	NN_UINT i;
	
	carry = 0;
	
	for (i = 0; i < digits; i++) {
		if ((ai = b[i] + carry) < carry)
			ai = c[i];
		else if ((ai += c[i]) < c[i])
			carry = 1;
		else
			carry = 0;
		a[i] = ai;
	}
	return carry;
}

/* Computes a = b - c. Returns borrow.
   a, b, c can be same
   Lengths: a[digits], b[digits], c[digits].
 */
NN_DIGIT  NN_Sub (
    NN_DIGIT *a, 
    NN_DIGIT *b, 
    NN_DIGIT *c, 
    NN_UINT digits
)  
{
	NN_DIGIT ai, borrow;
	NN_UINT i;
	NN_UINT blen;
	
	borrow = 0;
	blen = (NN_UINT)NN_Digits((NN_DIGIT *)b, ECC_MAXLONG);
	digits = blen > digits ? blen: digits ;
	
	for (i = 0; i < digits; i++) {
		if ((ai = b[i] - borrow) > (MAX_NN_DIGIT - borrow))
			ai = MAX_NN_DIGIT - c[i];
		else if ((ai -= c[i]) > (MAX_NN_DIGIT - c[i]))
			borrow = 1;
		else
			borrow = 0;
		a[i] = ai;
  }
  return borrow;
}

/* Computes a = b * c.
   a, b, c can be same
   Lengths: a[2*digits], b[digits], c[digits].
   Assumes digits < MAX_NN_DIGITS.
 */
void  NN_Mult (
    NN_DIGIT *a,
    NN_DIGIT *b, 
    NN_DIGIT *c, 
    NN_UINT digits
)  
{
	NN_DIGIT t[2*MAX_NN_DIGITS+2];
	NN_UINT bDigits, cDigits, i;
	
	NN_AssignZero (t, 2 * digits);
	
	bDigits = (NN_UINT)NN_Digits (b, digits);
	cDigits = (NN_UINT)NN_Digits (c, digits);

	for (i = 0; i < bDigits; i++)
		t[i+cDigits] += NN_AddDigitMult (&t[i], &t[i], b[i], c, cDigits);

	NN_Assign (a, t, 2 * digits);
}

/* Computes a = b * 2^c (i.e., shifts left c bits), returning carry.
   a, b can be same
   Lengths: a[digits], b[digits].
   Requires c < NN_DIGIT_BITS.
 */
NN_DIGIT  NN_LShift (
    NN_DIGIT *a, 
    NN_DIGIT *b, 
    NN_UINT c, 
    NN_UINT digits
)
{
	NN_DIGIT bi, carry;
	NN_UINT i, t;
	
	if (c >= NN_DIGIT_BITS)
		return (0);

	t = NN_DIGIT_BITS - c;

	carry = 0;

	for (i = 0; i < digits; i++) {
		bi = b[i];
		a[i] = (bi << c) | carry;
		carry = c ? (bi >> t) : 0;
	}

	return (carry);
}

/* Computes a = b div 2^c (i.e., shifts right c bits), returning carry.
   a, b can be same
   Lengths: a[digits], b[digits].
   Requires: c < NN_DIGIT_BITS.
 */
NN_DIGIT  NN_RShift (
    NN_DIGIT *a,
    NN_DIGIT *b, 
    NN_UINT c, 
    NN_UINT digits
)
{
  NN_DIGIT bi, carry;
  int i;
  NN_UINT t;

  if (c >= NN_DIGIT_BITS)
    return (0);

  t = NN_DIGIT_BITS - c;

  carry = 0;

  for (i = digits - 1; i >= 0; i--) {
    bi = b[i];
    a[i] = (bi >> c) | carry;
    carry = c ? (bi << t) : 0;
  }

  return (carry);
}

/* Computes a = c div d and b = c mod d.
   a, c, d can be same
   b, c, d can be same
   Lengths: a[cDigits], b[dDigits], c[cDigits], d[dDigits].
   Assumes d > 0, cDigits < 2 * MAX_NN_DIGITS,
           dDigits < MAX_NN_DIGITS.
 */
void  NN_Div (
    NN_DIGIT *a,
    NN_DIGIT *b,
    NN_DIGIT *c, 
    NN_UINT cDigits, 
    NN_DIGIT *d, 
    NN_UINT dDigits
)
{
     NN_DIGIT ai, cc[2*MAX_NN_DIGITS+2], dd[1*MAX_NN_DIGITS+2], t;

    int i;
    int ddDigits, shift;

    ddDigits = NN_Digits (d, dDigits);
    if (ddDigits == 0)
        return;

    /* Normalize operands.
    */
    shift = NN_DIGIT_BITS - NN_DigitBits (d[ddDigits-1]);
    NN_AssignZero (cc, 2*MAX_NN_DIGITS+2);
    NN_AssignZero (dd, 1*MAX_NN_DIGITS+2);
    cc[cDigits] = NN_LShift (cc, c, shift, cDigits); // cc = c * 2^shift
    NN_LShift (dd, d, shift, ddDigits); // dd = d * 2^shift
    t = dd[ddDigits-1];

    if (a != NULL)
  	    NN_AssignZero (a, cDigits);

    for (i = cDigits-ddDigits; i >= 0; i--) {
        /* Underestimate quotient digit and subtract.
            */
        if (t == MAX_NN_DIGIT)
            ai = cc[i+ddDigits];
        else
            NN_DigitDiv (&ai, &cc[i+ddDigits-1], t + 1);
        cc[i+ddDigits] -= NN_SubDigitMult (&cc[i], &cc[i], ai, dd, ddDigits);
	
        /* Correct estimate.
            */
        while (cc[i+ddDigits] || (NN_Cmp (&cc[i], dd, ddDigits) >= 0)) {
            ai++;
            cc[i+ddDigits] -= NN_Sub (&cc[i], &cc[i], dd, ddDigits);
        }
        
        if (a != NULL)
        a[i] = ai;
    }  
    /* Restore result.
        */
    NN_AssignZero (b, dDigits);
    NN_RShift (b, cc, shift, ddDigits);
}

/* Computes a = b mod c.

   Lengths: a[cDigits], b[bDigits], c[cDigits].
   Assumes c > 0, bDigits < 2 * MAX_NN_DIGITS, cDigits < MAX_NN_DIGITS.
 */
void  NN_Mod (
    NN_DIGIT *a,
    NN_DIGIT *b, 
    NN_UINT bDigits, 
    NN_DIGIT *c, 
    NN_UINT cDigits
)
{  
#ifdef BARRETT_REDUCTION
    NN_ModBarrett(a, b, bDigits, c, cDigits);
#else
    NN_Div (NULL, a, b, bDigits, c, cDigits);
#endif
}

/* Computes a = b * c mod d.
   a, b, c can be same lengths: a[digits], b[digits], c[digits], d[digits].
   Assumes d > 0, digits < MAX_NN_DIGITS.
 */
void  NN_ModMult (
    NN_DIGIT *a,
    NN_DIGIT *b,
    NN_DIGIT *c,
    NN_DIGIT *d,
    NN_UINT digits
)
{
#ifdef BARRETT_REDUCTION
	NN_DIGIT q2[2*MAX_NN_DIGITS+8], x[2*MAX_NN_DIGITS+8], r2[2*MAX_NN_DIGITS+8]/*, m[MAX_NN_DIGITS+4]*/;
#ifdef EICL
	NN_AssignZero(x, 2*MAX_NN_DIGITS+8);
	NN_AssignZero(q2, 2*MAX_NN_DIGITS+8);
	NN_AssignZero(r2, 2*MAX_NN_DIGITS+8);
	//NN_AssignZero(m, MAX_NN_DIGITS+4);
#else
	if(digits > 4)
		NN_AssignZero(x+2*digits-8, 2*MAX_NN_DIGITS-2*digits+16);
	else
		NN_AssignZero(x, 2*MAX_NN_DIGITS+8);
#endif

	//b*c
#ifdef EICL
	eicl_mul((BIGINT *)x, (BIGINT *)b, (BIGINT *)c);
#else
	NN_Mult(x, b, c, digits);
#endif
	//NN_Assign(m, d, digits);
	//NN_AssignZero(m+digits, MAX_NN_DIGITS+4-digits);

	//q_2=q_1*mu
#ifdef EICL
	eicl_mul((BIGINT *)q2, (BIGINT *)(x+pBarrett->km-1), (BIGINT *)(pBarrett->mu));	
#else
	NN_Mult(q2, x+pBarrett->km-1, pBarrett->mu, pBarrett->mu_len);
#endif

	//q_3*m
#ifdef EICL
	//eicl_mul((BIGINT *)r2, (BIGINT *)(q2+pBarrett->km+1), (BIGINT *)m);
	eicl_mul((BIGINT *)r2, (BIGINT *)(q2+pBarrett->km+1), (BIGINT *)d);
#else
	//NN_Mult(r2, q2+pBarrett->km+1, m, pBarrett->mu_len);
	NN_Mult(r2, q2+pBarrett->km+1, d, ->mu_len);
#endif

	NN_AssignZero(r2+pBarrett->km+1, 2*MAX_NN_DIGITS+8-pBarrett->km-1);
	NN_AssignZero(x+pBarrett->km+1, 2*MAX_NN_DIGITS+8-pBarrett->km-1);

#ifdef EICL
	if(eicl_cmp((BIGINT *)x, (BIGINT *)r2) < 0)
		x[pBarrett->km+1] = 1;
	eicl_sub((BIGINT *)x, (BIGINT *)x, (BIGINT *)r2);
	while(eicl_cmp((BIGINT *)x, (BIGINT *)d) >= 0)
		eicl_sub((BIGINT *)x, (BIGINT *)x, (BIGINT *)d);
#else
	if (NN_Cmp(x, r2, pBarrett->km+1) < 0)
		x[pBarrett->km+1] = 1;
	NN_Sub(x, x, r2, pBarrett->km+2);
	while(NN_Cmp(x, /*m*/d, digits) >= 0)
		NN_Sub(x, x, /*m*/d, digits);
#endif
	
	NN_Assign(a, x, digits);

#else
	NN_DIGIT t[2*MAX_NN_DIGITS];
	
	//memset(t, 0, 2*MAX_NN_DIGITS*NN_DIGIT_LEN);
	t[2*MAX_NN_DIGITS-1]=0;
	t[2*MAX_NN_DIGITS-2]=0;
	NN_Mult (t, b, c, digits);
	NN_Mod (a, t, 2 * digits, d, digits);
#endif
}

/* Computes a = b^c mod d.

   Lengths: a[dDigits], b[dDigits], c[cDigits], d[dDigits].
   Assumes d > 0, cDigits > 0, dDigits < MAX_NN_DIGITS.
 */
void  NN_ModExp (
    NN_DIGIT *a,
    NN_DIGIT *b, 
    NN_DIGIT *c, 
    NN_UINT cDigits, 
    NN_DIGIT *d, 
    NN_UINT dDigits
)
{
    NN_DIGIT bPower[3][MAX_NN_DIGITS], ci, t[MAX_NN_DIGITS];
    int i;
    U8 ciBits, j, s;

    /* Store b, b^2 mod d, and b^3 mod d.
        */
    NN_Assign (bPower[0], b, dDigits);
    NN_ModMult (bPower[1], bPower[0], b, d, dDigits);
    NN_ModMult (bPower[2], bPower[1], b, d, dDigits);

    NN_ASSIGN_DIGIT (t, 1, dDigits);

    cDigits = (NN_UINT)NN_Digits (c, cDigits);
    for (i = cDigits - 1; i >= 0; i--) {
        ci = c[i];
        ciBits = NN_DIGIT_BITS;
    
        /* Scan past leading zero bits of most significant digit.
            */
        if (i == (int)(cDigits - 1)) {
            while (! DIGIT_2MSB (ci)) {
                ci <<= 2;
                ciBits -= 2;
            }
        }

        for (j = 0; j < ciBits; j += 2, ci <<= 2) {
        /* Compute t = t^4 * b^s mod d, where s = two MSB's of ci.
                */
        NN_ModMult (t, t, t, d, dDigits);
        NN_ModMult (t, t, t, d, dDigits);
        
        if ((s = DIGIT_2MSB (ci)) != 0)
            NN_ModMult (t, t, bPower[s-1], d, dDigits);
        }
    }

    NN_Assign (a, t, dDigits);
}

/* Compute a = 1/b mod c, assuming inverse exists.
   a, b, c can be same
   Lengths: a[digits], b[digits], c[digits].
   Assumes gcd (b, c) = 1, digits < MAX_NN_DIGITS.
 */
void  NN_ModInv (
    NN_DIGIT *a, 
    NN_DIGIT *b, 
    NN_DIGIT *c, 
    NN_UINT digits
)
{
    NN_DIGIT q[MAX_NN_DIGITS], t1[MAX_NN_DIGITS], t3[MAX_NN_DIGITS],
        u1[MAX_NN_DIGITS], u3[MAX_NN_DIGITS], v1[MAX_NN_DIGITS],
        v3[MAX_NN_DIGITS], w[2*MAX_NN_DIGITS];

    int u1Sign;

    /* Apply extended Euclidean algorithm, modified to avoid negative
        numbers.
        */
    NN_ASSIGN_DIGIT (u1, 1, digits);
    NN_AssignZero (v1, digits);
    NN_Assign (u3, b, digits);
    NN_Assign (v3, c, digits);
    u1Sign = 1;

    while (! NN_Zero (v3, digits)) {
        NN_Div (q, t3, u3, digits, v3, digits);
        NN_Mult (w, q, v1, digits);
        NN_Add (t1, u1, w, digits);
        NN_Assign (u1, v1, digits);
        NN_Assign (v1, t1, digits);
        NN_Assign (u3, v3, digits);
        NN_Assign (v3, t3, digits);
        u1Sign = -u1Sign;
    }

    /* Negate result if sign is negative.
        */
    if (u1Sign < 0)
        NN_Sub (a, c, u1, digits);
    else
        NN_Assign (a, u1, digits);
}

/*
 * a= b/c mod d
 * algorithm in "From Euclid's GCD to Montgomery Multiplication to the Great Divide"
 * 
 *
 */
void  NN_ModDivOpt (
    NN_DIGIT *a, 
    NN_DIGIT *b, 
    NN_DIGIT *c, 
    NN_DIGIT *d, 
    NN_UINT digits
)
{
    NN_DIGIT A[MAX_NN_DIGITS], B[MAX_NN_DIGITS], U[MAX_NN_DIGITS], V[MAX_NN_DIGITS];
    int tmp_even;

    NN_Assign(A, c, digits);
    NN_Assign(B, d, digits);
    NN_Assign(U, b, digits);
    NN_AssignZero(V, digits);
  
    while ((tmp_even = NN_Cmp(A, B, digits)) != 0){
        if (NN_EVEN(A, digits)){
	        NN_RShift(A, A, 1, digits);
	        if (NN_EVEN(U, digits)){
	            NN_RShift(U, U, 1, digits);
	        }else{
	            NN_Add(U, U, d, digits);
	            NN_RShift(U, U, 1, digits);
	        }
        }else if (NN_EVEN(B, digits)){
	        NN_RShift(B, B, 1, digits);
	        if (NN_EVEN(V, digits)){
	            NN_RShift(V, V, 1, digits);
	        }else{
	            NN_Add(V, V, d, digits);
	            NN_RShift(V, V, 1, digits);
	        }
        }else if (tmp_even > 0){
	        NN_Sub(A, A, B, digits);
	        NN_RShift(A, A, 1, digits);
	        if (NN_Cmp(U, V, digits) < 0){
	            NN_Add(U, U, d, digits);
	        }
	        NN_Sub(U, U, V, digits);
	        if (NN_EVEN(U, digits)){
	            NN_RShift(U, U, 1, digits);
	        }else{
	            NN_Add(U, U, d, digits);
	            NN_RShift(U, U, 1, digits);
	        }
        }else{
            NN_Sub(B, B, A, digits);
	        NN_RShift(B, B, 1, digits);
	        if (NN_Cmp(V, U, digits) < 0){
	            NN_Add(V, V, d, digits);
	        }
	        NN_Sub(V, V, U, digits);
	        if (NN_EVEN(V, digits)){
	            NN_RShift(V, V, 1, digits);
	        }else{
	            NN_Add(V, V, d, digits);
	            NN_RShift(V, V, 1, digits);
	        }
        }
  }

  NN_Assign(a, U, digits);
}

/* Returns sign of a - b.

   Lengths: a[digits], b[digits].
 */
S32  NN_Cmp (
    NN_DIGIT *a, 
    NN_DIGIT *b, 
    NN_UINT digits
)
{
    int i;
    register U16 alen, blen;
    alen = NN_Digits((NN_DIGIT *)a, ECC_MAXLONG); 
    blen = NN_Digits((NN_DIGIT *)b, ECC_MAXLONG);
    if(alen > blen) return 1;
    if(alen < blen) return -1;

    for (i = digits - 1; i >= 0; i--) { 
        if (a[i] > b[i])
            return (1);
    /* else added by Panos Kampankis*/
        else if (a[i] < b[i])
            return (-1); 
    }

    return (0);
}

/* Returns nonzero iff a is zero.

   Lengths: a[digits].
 */
S32  NN_Zero (
    NN_DIGIT *a,
    NN_UINT digits)
{
    NN_UINT i;

    for (i = 0; i < digits; i++)
        if (a[i])
            return (0);
  
    return (1);
}

/* Returns the significant length of a in bits.

   Lengths: a[digits].
 */
U16  NN_Bits (
    NN_DIGIT *a, 
    NN_UINT digits
)
{
    if ((digits = (NN_UINT)NN_Digits (a, digits)) == 0)
        return (0);

    return ((digits - 1) * NN_DIGIT_BITS + NN_DigitBits (a[digits-1]));
}

/* Returns the significant length of a in digits.

  Lengths: a[digits].
 */


U16  NN_Digits (
    NN_DIGIT *a,
    NN_UINT digits
)
{
    S16 i;

    for (i = digits - 1; i >= 0; i--)
        if (a[i])
        break;

    return (i + 1);
}

/* Computes a = b + c*d, where c is a digit. Returns carry.
   a, b, c can be same
   Lengths: a[digits], b[digits], d[digits].
 */
static NN_DIGIT NN_AddDigitMult (
    NN_DIGIT *a, 
    NN_DIGIT *b, 
    NN_DIGIT c, 
    NN_DIGIT *d, 
    NN_UINT digits
)
{
#ifdef NO_SUPPOST64
	NN_DIGIT carry;
	unsigned int i;
	NN_DIGIT t[2];
	
	//Should copy b to a
	if (c == 0)
		return (0);
	carry = 0;
	for (i = 0; i < digits; i++) {
		NN_DigitMult (c, d[i] , t);
		if ((a[i] = b[i] + carry) < carry)
			carry = 1;
		else
			carry = 0;
		if ((a[i] += (NN_DIGIT)(t[1])) < (t[1]))
			carry++;
		carry += t[0];
	}
    
    return (carry);
#else
    NN_DIGIT carry;
    unsigned int i;
    NN_DOUBLE_DIGIT t;

    //Should copy b to a
    if (c == 0)
    return (0);
    
    carry = 0;
    for (i = 0; i < digits; i++) {
        t = NN_DigitMult (c, d[i]);
        if ((a[i] = b[i] + carry) < carry)
            carry = 1;
        else
            carry = 0;
        
        if ((a[i] += (NN_DIGIT)(t & MAX_NN_DIGIT)) < (t & MAX_NN_DIGIT))
            carry++;
        
        carry += (NN_DIGIT)(t >> NN_DIGIT_BITS);
    }
    
    return (carry);
#endif
}

/* Computes a = b - c*d, where c is a digit. Returns borrow.
   a, b, d can be same
   Lengths: a[digits], b[digits], d[digits].
 */
static NN_DIGIT NN_SubDigitMult (
    NN_DIGIT *a, 
    NN_DIGIT *b,
    NN_DIGIT c,
    NN_DIGIT *d, 
    NN_UINT digits
)
{
#ifdef NO_SUPPOST64
	NN_DIGIT borrow;
	unsigned int i;
	NN_DIGIT t[2];
	
	if (c == 0)
		return (0);
	borrow = 0;
	for (i = 0; i < digits; i++) {
		NN_DigitMult (c, d[i] , t);
		if ((a[i] = b[i] - borrow) > (MAX_NN_DIGIT - borrow))
			borrow = 1;
		else
			borrow = 0;
		if ((a[i] -= (NN_DIGIT)(t[1])) > (MAX_NN_DIGIT - (t[1])))
			borrow++;
		borrow += (NN_DIGIT)(t[0]);
	}
  return (borrow); 

#else
    NN_DIGIT borrow;
    unsigned int i;
    NN_DOUBLE_DIGIT t;

    if (c == 0)
        return (0);
    
    borrow = 0;
    
    for (i = 0; i < digits; i++) {
        t = NN_DigitMult (c, d[i]);
        if ((a[i] = b[i] - borrow) > (MAX_NN_DIGIT - borrow))
            borrow = 1;
        else
            borrow = 0;
        
        if ((a[i] -= (NN_DIGIT)(t & MAX_NN_DIGIT)) > (MAX_NN_DIGIT - (t & MAX_NN_DIGIT)))
            borrow++;
        borrow += (NN_DIGIT)(t >> NN_DIGIT_BITS);
    }
    
    return (borrow);    
#endif
}

/* Returns the significant length of a in bits, where a is a digit.
 */
static unsigned int NN_DigitBits (
    NN_DIGIT a
)
{
    unsigned int i;

    for (i = 0; i < NN_DIGIT_BITS; i++, a >>= 1)
        if (a == 0)
        break;
  
    return (i);
}

//init \mu , b = wordsize, from Handbook of Applied Cryptography Algorithm 14.42
void  NN_ModBarrettInit(
    NN_DIGIT *c, 
    NN_UINT cDigits,
    Barrett *pbuf
)
{
	NN_DIGIT tmp[2*MAX_NN_DIGITS+1], residue[MAX_NN_DIGITS];
	
	pBarrett = pbuf;
	pBarrett->km = (unsigned char)NN_Digits(c, cDigits);
	
	NN_AssignZero(tmp, 2*MAX_NN_DIGITS+1);
	NN_AssignZero(pBarrett->mu, 2*MAX_NN_DIGITS+1);
	tmp[2*pBarrett->km] = 1;
	
	NN_Div(pBarrett->mu, residue, tmp, 2*pBarrett->km+1, c, cDigits);
	pBarrett->mu_len = (unsigned char)NN_Digits(pBarrett->mu, 2*MAX_NN_DIGITS+1);
  
#ifdef HYBRID_MULT
#ifdef PLATFORM_MICAZ
#ifdef HYBRID_MUL_WIDTH4
	while ((pBarrett->mu_len % 4) != 0)
		pBarrett->mu_len++;
#else
	while ((pBarrett->mu_len % 5) != 0)
		pBarrett->mu_len++;
#endif
#endif
#endif
}

//set the buffer for barrett reduction
void  NN_BarrettSetBuf(
    Barrett *pbuf
)
{
    pBarrett = pbuf;
}

//Barrett modular reduction, from Handbook of Applied Cryptography Algorithm 14.42
//compute a = b mod c
//cDigits must be smaller than MAX_NN_DIGITS
void  NN_ModBarrett(
    NN_DIGIT *a, 
    NN_DIGIT *b,
    NN_UINT bDigits,
    NN_DIGIT *c,
    NN_UINT cDigits
)
{
	NN_DIGIT q2[2*MAX_NN_DIGITS+6], q1[MAX_NN_DIGITS+4], r2[2*MAX_NN_DIGITS+8]/*, r1[MAX_NN_DIGITS+4], m[MAX_NN_DIGITS+4]*/;
	
	NN_AssignZero(q2, 2*MAX_NN_DIGITS+6);
	NN_AssignZero(r2, 2*MAX_NN_DIGITS+8);
	
	// q1 = b / ( (2^32) ^ (km-1) )
	NN_Assign(q1, b+pBarrett->km-1, bDigits-pBarrett->km+1);
	NN_AssignZero(q1+bDigits-pBarrett->km+1, MAX_NN_DIGITS+4-bDigits+pBarrett->km-1);
	
	// r1 = b mode ( (2^32) ^ (km+1) )
	//NN_Assign(r1, b, pBarrett->km+1);
	//NN_AssignZero(r1+pBarrett->km+1, MAX_NN_DIGITS+4-pBarrett->km-1);
	
	//NN_Assign(m, c, cDigits);
	//NN_AssignZero(m+cDigits, MAX_NN_DIGITS+4-cDigits);
	
	// q_2 = q_1 * mu
#ifdef EICL
	eicl_mul((BIGINT *)q2, (BIGINT *)q1, (BIGINT *)(pBarrett->mu));
#else
	NN_Mult(q2, q1, pBarrett->mu, pBarrett->mu_len);
#endif
	
	// r2 = (q_3 * m) mod ((2^32) ^ (km+1)) = (q2 / ((2^32) ^ (km+1)) ) * m mod ((2^32) ^ (km+1))
#ifdef EICL
	eicl_mul((BIGINT *)r2, (BIGINT *)(q2+pBarrett->km+1), (BIGINT *)/*m*/c);
#else
	NN_Mult(r2, q2+pBarrett->km+1, /*m*/c, pBarrett->mu_len);
#endif
	NN_AssignZero(r2+pBarrett->km+1, 2*MAX_NN_DIGITS+8-pBarrett->km-1);
	
	// r = r1 - r2
	// if r < 0; r = r + (2^32) ^ (km+1)
	// r1 = b mode ( (2^32) ^ (km+1) )

	NN_Assign(q1, b, pBarrett->km+1);
	NN_AssignZero(q1+pBarrett->km+1, MAX_NN_DIGITS+4-pBarrett->km-1);

#ifdef EICL
	if(eicl_cmp((BIGINT *)q1, (BIGINT *)r2) < 0)
		q1[pBarrett->km+1] = 1;
	eicl_sub((BIGINT *)q1, (BIGINT *)q1, (BIGINT *)r2);
	while(eicl_cmp((BIGINT *)q1, (BIGINT *)c) >= 0)
		eicl_sub((BIGINT *)q1, (BIGINT *)q1, (BIGINT *)c);
#else
	if (NN_Cmp(q1, r2, pBarrett->km+1) < 0)
		q1[pBarrett->km+1] = 1;
	NN_Sub(q1, q1, r2, pBarrett->km+2);

	while(NN_Cmp(q1, c, cDigits) >= 0)
		NN_Sub(q1, q1, c, cDigits);
#endif

	NN_Assign(a, q1, cDigits);
	NN_AssignZero(a+cDigits, MAX_NN_DIGITS-cDigits);
}

//Computes a = (b + c) mod d.
//a, b, c can be same
//Assumption: b,c is in [0, d)
void  NN_ModAdd(
    NN_DIGIT * a,
    NN_DIGIT * b,
    NN_DIGIT * c, 
    NN_DIGIT * d, 
    NN_UINT digits
)
{
    NN_DIGIT tmp[MAX_NN_DIGITS];
    NN_DIGIT carry;
  
    carry = NN_Add(tmp, b, c, digits);
    if (carry)
        NN_Sub(a, tmp, d, digits);
    else if (NN_Cmp(tmp, d, digits) >= 0)
        NN_Sub(a, tmp, d, digits);
    else
        NN_Assign(a, tmp, digits);
  
}
   
//Computes a = (b - c) mod d.
//Assume b and c are all smaller than d
//always return positive value
void  NN_ModSub(
    NN_DIGIT * a, 
    NN_DIGIT * b, 
    NN_DIGIT * c,
    NN_DIGIT * d,
    NN_UINT digits
)
{
	NN_DIGIT tmp[MAX_NN_DIGITS];
	NN_DIGIT borrow;
	borrow = NN_Sub(tmp, b, c, digits);
	if (borrow) 
		NN_Add(a, tmp, d, digits); 
	else 
		NN_Assign(a, tmp, digits);
	
}

//Computes a = -b mod c
void NN_ModNeg(
    NN_DIGIT * a,
    NN_DIGIT * b,
    NN_DIGIT * c, 
    NN_UINT digits
){
    NN_ModSub(a,c,b,c,digits);
}
   
//Computes a = (b / c) mod d
void NN_ModDiv(
    NN_DIGIT * a,
    NN_DIGIT * b,
    NN_DIGIT * c,
    NN_DIGIT * d, 
    NN_UINT digits
)
{
#ifndef MODINVOPT
    NN_ModDivOpt (a, b, c, digits);
#else
    NN_DIGIT y[MAX_NN_DIGITS];
    NN_ASSIGN_DIGIT(y, 1, digits);
    NN_ModDivOpt(a, y, b, c, digits);
#endif
}

// test whether the ith bit in a is one
NN_DIGIT  NN_TestBit(
    NN_DIGIT * a, 
    S16 i
)
{  
    return (b_testbit(a,i));
}

/* Computes a = b + c*d, where c is a digit. Returns carry.
   a, b, c can be same
   Lengths: a[digits], b[digits], d[digits].
 */
NN_DIGIT  NN_Add_DigitMult (
    NN_DIGIT *a,
    NN_DIGIT *b,
    NN_DIGIT c, 
    NN_DIGIT *d,
    NN_UINT digits
)
{
	return NN_AddDigitMult(a, b, c, d, digits);
}

/* Computes a = b - c*d, where c is a digit. Returns borrow.
   a, b, d can be same
   Lengths: a[digits], b[digits], d[digits].
 */
NN_DIGIT  NN_Sub_DigitMult (
    NN_DIGIT *a,
    NN_DIGIT *b, 
    NN_DIGIT c,
    NN_DIGIT *d,
    NN_UINT digits
)
{
	return NN_SubDigitMult(a, b, c, d, digits);
}

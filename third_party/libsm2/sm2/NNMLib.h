#ifndef _NNMLIB_H
#define _NNMLIB_H

#ifdef __cplusplus
extern "C"{
#endif

void  NN_DigitDiv (
    NN_DIGIT *a, 
    NN_DIGIT b[2], 
    NN_DIGIT c
);

void  NN_Decode (
    NN_DIGIT *a,
    NN_UINT digits,
    unsigned char *b,
    NN_UINT len
);

void  NN_Encode (
    unsigned char *a, 
    NN_UINT len, 
    NN_DIGIT *b,
    NN_UINT digits
);

void  NN_Assign (
    NN_DIGIT *a,
    NN_DIGIT *b,
    NN_UINT digits
);

void  NN_AssignZero (
    NN_DIGIT *a, 
    NN_UINT digits
);

NN_DIGIT  NN_Add (
    NN_DIGIT *a, 
    NN_DIGIT *b, 
    NN_DIGIT *c,
    NN_UINT digits
);

NN_DIGIT  NN_Sub (
    NN_DIGIT *a,
    NN_DIGIT *b,
    NN_DIGIT *c,
    NN_UINT digits
);

void  NN_Mult (
    NN_DIGIT *a,
    NN_DIGIT *b,
    NN_DIGIT *c, 
    NN_UINT digits
);

void  NN_Sqr(
    NN_DIGIT *a, 
    NN_DIGIT *b, 
    NN_UINT digits
);

NN_DIGIT  NN_LShift (
    NN_DIGIT *a, 
    NN_DIGIT *b, 
    NN_UINT c, 
    NN_UINT digits
);

NN_DIGIT  NN_RShift (
    NN_DIGIT *a, 
    NN_DIGIT *b, 
    NN_UINT c, 
    NN_UINT digits
);

void  NN_Div (
    NN_DIGIT *a, 
    NN_DIGIT *b, 
    NN_DIGIT *c, 
    NN_UINT cDigits, 
    NN_DIGIT *d, 
    NN_UINT dDigits
);

void  NN_Mod (
    NN_DIGIT *a, 
    NN_DIGIT *b,
    NN_UINT bDigits, 
    NN_DIGIT *c,
    NN_UINT cDigits
);

void  NN_ModMult (
    NN_DIGIT *a, 
    NN_DIGIT *b,
    NN_DIGIT *c,
    NN_DIGIT *d, 
    NN_UINT digits
);

void  NN_ModExp (
    NN_DIGIT *a,
    NN_DIGIT *b,
    NN_DIGIT *c, 
    NN_UINT cDigits,
    NN_DIGIT *d,
    NN_UINT dDigits
);

void  NN_ModInv (
    NN_DIGIT *a,
    NN_DIGIT *b,
    NN_DIGIT *c,
    NN_UINT digits
);

S32  NN_Cmp (
    NN_DIGIT *a, 
    NN_DIGIT *b,
    NN_UINT digits
);

S32  NN_Zero (
    NN_DIGIT *a,
    NN_UINT digits
);

U16  NN_Bits (
    NN_DIGIT *a, 
    NN_UINT digits
);
U16  NN_Digits (
    NN_DIGIT *a,
    NN_UINT digits
);

void  NN_ModBarrettInit(
    NN_DIGIT *c, 
    NN_UINT cDigits, 
    Barrett *pbuf
);

void  NN_BarrettSetBuf(
    Barrett *pbuf
);

void  NN_ModBarrett(
    NN_DIGIT *a, 
    NN_DIGIT *b, 
    NN_UINT bDigits, 
    NN_DIGIT *c, 
    NN_UINT cDigits
);

NN_DIGIT  NN_TestBit(
    NN_DIGIT * a, 
    S16 i
);

void  NN_ModAdd(
    NN_DIGIT * a, 
    NN_DIGIT * b,
    NN_DIGIT * c, 
    NN_DIGIT * d,
    NN_UINT digits
);

void  NN_ModSub(
    NN_DIGIT * a, 
    NN_DIGIT * b,
    NN_DIGIT * c,
    NN_DIGIT * d,
    NN_UINT digits
);

NN_DIGIT  NN_Add_DigitMult (
    NN_DIGIT *a,
    NN_DIGIT *b, 
    NN_DIGIT c, 
    NN_DIGIT *d,
    NN_UINT digits
);

NN_DIGIT  NN_Sub_DigitMult (
    NN_DIGIT *a, 
    NN_DIGIT *b,
    NN_DIGIT c, 
    NN_DIGIT *d,
    NN_UINT digits
);

#ifdef __cplusplus
}
#endif

#endif
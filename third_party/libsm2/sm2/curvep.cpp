#include "stdafx.h"
#include "const.h"
#include "curvep.h"
#include "paramp.h"

void dump_point(
    point_affn_ptr P,
    char *t
)
{
    //AK_DEBUG_OUTPUT("---%s---\n", t);


	//gmp_printf("%Zx\n", P->x);
    //	gmp_printf("%Zx\n", P->y);
}

void dump_point_proj(
    point_proj_ptr X, 
    char *t
)
{
	//printf("---------- %s-------- \n", t);
	//gmp_printf("x: %Zx\n", X->x);
	//gmp_printf("y: %Zx\n", X->y);
	//gmp_printf("z: %Zx\n", X->z);
	//printf("--------end of %s----------\n", t);
}



void point_proj_init(
    point_proj_ptr P
)
{
	mpz_init(P->x);
	mpz_init(P->y);
	mpz_init(P->z);
}

void point_proj_clear(
    point_proj_ptr P
)
{
	mpz_clear(P->x);
	mpz_clear(P->y);
	mpz_clear(P->z);
}

void point_proj_set_0(
    point_proj_ptr P
)
{	
	mpz_set_ui(P->x, 1);
	mpz_set_ui(P->y, 1);
	mpz_set_ui(P->z, 0);
}

void point_affn_set(
    point_affn_ptr R,
    point_affn_ptr P
)
{
	mpz_set(R->x, P->x);
	mpz_set(R->y, P->y);
	R->infinity=P->infinity;
}

void point_proj_set_affn(
    point_proj_ptr R, 
    point_affn_ptr P
)
{
	mpz_set(R->x, P->x);
	mpz_set(R->y, P->y);
	mpz_set_ui(R->z,1);
}

void point_affn_init(
    point_affn_ptr P
)
{
	mpz_init(P->x);
	mpz_init(P->y);
	P->infinity=0;
}

void point_affn_clear(
    point_affn_ptr P
)
{
	mpz_clear(P->x);
	mpz_clear(P->y);
	P->infinity=0;
}

char point_affn_cmp(
    point_affn_ptr P,
    point_affn_ptr Q
)
{
	if(P->infinity!=Q->infinity)
		return 1;

	if(mpz_cmp(P->x, Q->x)||mpz_cmp(P->y, Q->y))
		return 1;

	return 0;
}

//curve y^2=x^3+ax+b
 void point_proj_double(
    point_proj_ptr Q
)
//project operation
//Q=2Q
{
#ifndef USE_GLOBAL_MEM
	mpz_t ld1, ld2, ld3;
#else
    mpz_ptr ld1,ld2,ld3;

    ld1 = (mpz_ptr)(mECC->pBuffer[mECC->index++]);
    ld2 = (mpz_ptr)(mECC->pBuffer[mECC->index++]);
    ld3 = (mpz_ptr)(mECC->pBuffer[mECC->index++]);
	GET_GMEM_INDEX
#endif

	if(mpz_cmp_ui(Q->z, 0)==0)   //Q is infinity, then does nothing
	{
 	    goto end;
    }

	mpz_init(ld1);
	mpz_init(ld2);
	mpz_init(ld3);

	zp_mul_mod(ld1, Q->z, Q->z); //ld1=Z_1^2
 	zp_mul_mod(ld2, Q->x, Q->x); //X_1^2
	zp_add_mod(ld3, ld2, ld2);
    zp_add_mod(ld2, ld3, ld2); //3X_1^2
	zp_mul_mod(ld3, ld1, ld1);
	zp_mul_mod(ld3, ld3, COA);	//a*ld1^2
	zp_add_mod(ld2, ld2, ld3); //3X_1^2+a*ld1^2

	zp_mul_mod(Q->z, Q->y, Q->z);
	zp_add_mod(Q->z, Q->z, Q->z);//2Y_1*Z_1

	zp_mul_mod(ld3, Q->y, Q->y);
	zp_add_mod(ld3, ld3, ld3); //2Y_1^2

	zp_mul_mod(ld1, Q->x, ld3);
	zp_add_mod(ld1, ld1, ld1); //2X_1*ld3 (ld4)

	zp_mul_mod(Q->x, ld2, ld2);
	zp_sub_mod(Q->x, Q->x, ld1);
	zp_sub_mod(Q->x, Q->x, ld1); //ld2^2-2ld4 

	zp_mul_mod(ld3, ld3, ld3);
	zp_add_mod(ld3, ld3, ld3); //2*ld3^2 (ld5)

	zp_sub_mod(Q->y, ld1, Q->x);
	zp_mul_mod(Q->y, ld2, Q->y);
	zp_sub_mod(Q->y, Q->y, ld3);
	
	mpz_clear(ld1);
	mpz_clear(ld2);
	mpz_clear(ld3);
end:
#ifdef USE_GLOBAL_MEM
    mECC->index-=3;
#endif
	return;
}

 void point_mix_add(
    point_proj_ptr Q, 
    point_affn_ptr P
)
//mix operation
//Q+=P
{
#ifndef USE_GLOBAL_MEM
	mpz_t ld1, ld2, ld3,ld4;
#else
    mpz_ptr ld1,ld2,ld3,ld4;

    ld1 = (mpz_ptr)(mECC->pBuffer[mECC->index++]);
    ld2 = (mpz_ptr)(mECC->pBuffer[mECC->index++]);
    ld3 = (mpz_ptr)(mECC->pBuffer[mECC->index++]);
    ld4 = (mpz_ptr)(mECC->pBuffer[mECC->index++]);
	GET_GMEM_INDEX
#endif

	if(mpz_cmp_ui(Q->z, 0)==0)   //Q is infinity
    {
		if(P->infinity)          //P is infinity
			goto end;             //does nothing
		mpz_set(Q->x, P->x);    //Q=P
		mpz_set(Q->y, P->y);
		mpz_set_ui(Q->z, 1);
		goto end;
	}

	mpz_init(ld1);
	mpz_init(ld2);
	mpz_init(ld3);
	mpz_init(ld4);

	//point addition, P is fixed point with Pz=1
	zp_mul_mod(ld4, Q->z, Q->z);//Z_2^2
	zp_mul_mod(ld1, P->x, ld4); //ld1=X_1*Z_2^2
	zp_sub_mod(ld2, ld1, Q->x); //ld2=ld1-X_2
	zp_mul_mod(ld4, ld4, Q->z); //Z_2^3
	zp_mul_mod(ld3, P->y, ld4); //ld3=Y_1*Z_2^3
	zp_sub_mod(ld4, ld3, Q->y); //ld4=ld3-Y_2
	zp_add_mod(ld1, ld1, Q->x); //ld5=ld1+X_2-->ld1
	zp_add_mod(ld3, ld3, Q->y); //ld6=ld3+Y_2-->ld3
	zp_mul_mod(Q->z, Q->z, ld2); //Z_3=Z_2*ld2
	zp_mul_mod(Q->y, ld2, ld2); //ld7=ld2^2-->Q_y
	zp_mul_mod(ld3, ld3, Q->y); //ld6*ld7
	zp_mul_mod(ld3, ld3, ld2);  //ld3=ld6*ld7*ld2
	zp_mul_mod(ld1, ld1, Q->y); //ld1=ld5*ld7
	zp_mul_mod(Q->x, ld4, ld4); //ld4^2
	zp_sub_mod(Q->x, Q->x, ld1);//X_3=ld4^2-ld5*ld7
	zp_sub_mod(ld2, ld1, Q->x);	//ld5*ld7-X_3
	zp_sub_mod(ld1, ld2, Q->x);	//ld8=ld5*ld7-2X_3-->ld1
	zp_mul_mod(ld1, ld1, ld4);	//ld8*ld4
	zp_sub_mod(Q->y, ld1, ld3);	//ld8*ld4-ld6*ld7*ld2
	if(mpz_odd_p(Q->y)){
		mpz_add(Q->y, Q->y, FIELD_P);
	}
	mpz_fdiv_q_2exp(Q->y, Q->y, 1); //Y3=(ld8*ld4-ld6*ld7*ld2)/2

	mpz_clear(ld1);
	mpz_clear(ld2);
	mpz_clear(ld3);
	mpz_clear(ld4);
end:
#ifdef USE_GLOBAL_MEM
    mECC->index-=4;
#endif
	return;
}

 void point_proj_mul(
    point_proj_ptr R, 
    point_affn_ptr P, 
    mpz_ptr n
)
{
	short m;

    m = mpz_sizeinbase(n, 2) - 1;
    
	point_proj_init(R);
    point_proj_set_0(R);

	while(!mpz_tstbit(n, m)) 
		m--;

    while(m>=0) {
		point_proj_double(R);
		//dump_point_proj(R,"R1");	
        	
		if (mpz_tstbit(n, m)) {
			point_mix_add(R, P);
//			dump_point_proj(R, "R2");
		}
		m--;
    }
}

void point_proj2affn(
    point_affn_ptr R, 
    point_proj_ptr P
)
{
#ifndef USE_GLOBAL_MEM
	mpz_t t1,t2;
#else
    mpz_ptr t1,t2;

    t1 = (mpz_ptr)(mECC->pBuffer[mECC->index++]);
    t2 = (mpz_ptr)(mECC->pBuffer[mECC->index++]);
	GET_GMEM_INDEX
#endif

	mpz_init(t1);
	mpz_init(t2);

	if(mpz_cmp_ui(P->z, 0)){
		mpz_invert(t1, P->z, FIELD_P); //t1=1/z
//	printf("----------------------mpz_invert %d\n", mECC->index);

		//x[1], y[1] no longer needed
		zp_mul_mod(t2, t1, t1); //t2=t1*t1=1/z^2
		zp_mul_mod(R->x, P->x, t2);	//x/z^2
		zp_mul_mod(t2, t2, t1); //t2=t1^3=1/z^3
		zp_mul_mod(R->y, P->y, t2);//y/z^3

		R->infinity = 0;
	}
	else{
		R->infinity=1;
	}

	mpz_clear(t1);
	mpz_clear(t2);

#ifdef USE_GLOBAL_MEM
    mECC->index-=2;
#endif	
}

 void point_affn_mul(
    point_affn_ptr R, 
    point_affn_ptr P, 
    mpz_ptr n
)
//R=[n]P
{
#ifndef USE_GLOBAL_MEM
	point_proj_t nP;
	
#else

	point_proj_ptr nP;
	nP=(point_proj_ptr)mECC->pBuffer[mECC->index];
	mECC->index+=3;
	GET_GMEM_INDEX
#endif

// 	printf("-------in point_affn_mul current_mem_index %d\n", mECC->index);

	point_proj_mul(nP, P, n);
//	dump_point(nP, " nP");
	point_proj2affn(R, nP);
#ifdef USE_GLOBAL_MEM
	mECC->index-=3;
#endif
}

 void point_affn_add(
    point_affn_ptr R, 
    point_affn_ptr P,
    point_affn_ptr Q
 )
//R = P + Q
//R != P
//working on y^2=x^3+ax+b
//Assume P!=-Q, P!=Q
{
#ifndef USE_GLOBAL_MEM
	mpz_t ld, temp1;
#else
    mpz_ptr ld,temp1;

    ld = (mpz_ptr)(mECC->pBuffer[mECC->index++]);
    temp1 = (mpz_ptr)(mECC->pBuffer[mECC->index++]);
	GET_GMEM_INDEX
#endif

	if(P->infinity){
		point_affn_set(R, Q);
		goto end;
	}
		
	if(Q->infinity){
		point_affn_set(R, P);
		goto end;
	}

	mpz_init(ld);
	mpz_init(temp1);

	//compute lambda
	zp_sub_mod(temp1, Q->x, P->x);	//X2-X1
  	
	mpz_invert(temp1, temp1, FIELD_P);		//1/X2-X1
//	printf("----------------------mpz_invert %d\n", mECC->index);
//	gmp_printf("temp1 %Zx\n", temp1);
 	
	zp_sub_mod(ld, Q->y, P->y);		//Y2-Y1
	zp_mul_mod(ld, ld, temp1);		//Y2-Y1/X2-X1
//	gmp_printf("ld %Zx\n", ld);
	//compute X_3
	zp_mul_mod(temp1, ld, ld);		//ld^2
//	gmp_printf("temp1 %Zx\n", temp1);
	zp_sub_mod(temp1, temp1, P->x);	//ld^2-X_1
// 	gmp_printf("temp1 %Zx\n", temp1);
	
	zp_sub_mod(R->x, temp1, Q->x);	//X_3=ld^2-X_1-X_2
// 	gmp_printf("R->x %Zx\n", R->x);
	
	//compute Y_3
	zp_sub_mod(temp1, P->x, R->x);	//X_1-X_3
	zp_mul_mod(temp1, temp1, ld);	//ld(X_1-X_3)
	zp_sub_mod(R->y, temp1, P->y);	//Y_3=ld(X_1-X_3)-Y_1
	
	mpz_clear(ld);
	mpz_clear(temp1);
end:
#ifdef USE_GLOBAL_MEM
    mECC->index-=2;
#endif	
	return;
}

 U32 EC2OSP_XY(
    U8 *data, 
    S32 *len, 
    point_affn_ptr P
 )
{
	U32 n=(FIELD_SIZE_BITS+7)/8;

	I2BSP(data, n*8, P->x);
	I2BSP(data+n, n*8, P->y);

	*len=2*n;
	return 0;
}

 U32 EC2OSP(
    U8 *data, 
    S32 *len, 
    point_affn_ptr P,
    int ecp
)
{
	EC2OSP_XY(data+1, len, P);

	data[0] = 0x04;
	*len = (*len) + 1;
	return 0;
}

 S32 OS2ECP_XY(
    point_affn_ptr P, 
    U8 *data,
    int len
 )
{
	U32 n=(FIELD_SIZE_BITS+7)/8;

    BS2IP(P->x, data, n*8); 
    BS2IP(P->y, data+n, n*8); 
	return 2*n;
}

 S32 OS2ECP(
    point_affn_ptr P,
    U8 *data,
    int len
 )
{
	if(data[0] != 0x04)
		return -1;

	return OS2ECP_XY(P, data+1, len-1)+1;
}

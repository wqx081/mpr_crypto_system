#include "stdafx.h"
#include "string.h"
#include "sm2.h"
#include "sm3.h"
#include "paramp.h"
#include "const.h"
#include "srand.h"
#include "curvep.h"

#define SM3_HASH_LEN			32
#define ALG_HASH_SM3			0x30
#define ECPUCMP					0x04

#define LT_ENDIAN 

#ifdef MUL_BITS
int eccbits;
#endif
void dump_mem(unsigned char *ptr, int len, char *title)
{
/*
	int i;

	AK_DEBUG_OUTPUT("----begin %s dump---\n", title);
	for(i=0; i<len; i++)
		AK_DEBUG_OUTPUT("%02x",(unsigned char)(ptr[i]));

	AK_DEBUG_OUTPUT("\n----dump %s end---\n", title);*/
}


ecc_param sysparam;
ecc_param *paramptr = &sysparam;
TBarrett pXbufFieldPrime;
TBarrett pXbufSubgrpOrder;

void sm2_init_param(int bits_len)
{
#ifdef MUL_BITS
	eccbits = bits_len;
#endif
	if(bits_len == 256){
		mpz_init(FIELD_P);
		mpz_set_str(FIELD_P, (unsigned char*)"FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFF", 16);

		mpz_init(COA);
		mpz_set_str(COA, (unsigned char*)"FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFC", 16);
		mpz_init(COB);
		mpz_set_str(COB, (unsigned char*)"28E9FA9E9D9F5E344D5A9E4BCF6509A7F39789F515AB8F92DDBCBD414D940E93", 16);

		mpz_init(SUBGRP_ORDER);
		mpz_set_str(SUBGRP_ORDER, (unsigned char*)"FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFF7203DF6B21C6052B53BBF40939D54123", 16);

		point_affn_init(GENERATOR);
		GENERATOR->infinity = 0;
		mpz_init(GENERATOR->x);
		mpz_set_str(GENERATOR->x, (unsigned char*)"32C4AE2C1F1981195F9904466A39C9948FE30BBFF2660BE1715A4589334C74C7", 16);
		mpz_init(GENERATOR->y);
		mpz_set_str(GENERATOR->y, (unsigned char*)"BC3736A2F4F6779C59BDCEE36B692153D0A9877CC62A474002DF32E52139F0A0", 16);

#define pXbufSubgrpOrder256		"\xA0\x49\x51\xF1\x61\x63\xAC\x12\x01\x3C\x32\xFA\x96\x20\xFC\x8D\x01\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x09\x08\x00\x00"
#define pXbufFieldPrime256		"\x03\x00\x00\x00\x02\x00\x00\x00\x02\x00\x00\x00\x02\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x09\x08\x00\x00"
		memcpy( &pXbufSubgrpOrder , pXbufSubgrpOrder256 , 0x50);
		memcpy( &pXbufFieldPrime , pXbufFieldPrime256 , 0x50);
		
		NN_BarrettSetBuf(&pXbufFieldPrime);
	}else{
		mpz_init(FIELD_P);
		mpz_set_str(FIELD_P, (unsigned char*)"BDB6F4FE3E8B1D9E0DA8C0D46F4C318CEFE4AFE3B6B8551F", 16);

		mpz_init(COA);
		mpz_set_str(COA, (unsigned char*)"BB8E5E8FBC115E139FE6A814FE48AAA6F0ADA1AA5DF91985", 16);
		mpz_init(COB);
		mpz_set_str(COB, (unsigned char*)"1854BEBDC31B21B7AEFC80AB0ECD10D5B1B3308E6DBF11C1", 16);

		mpz_init(SUBGRP_ORDER);
		mpz_set_str(SUBGRP_ORDER, (unsigned char*)"BDB6F4FE3E8B1D9E0DA8C0D40FC962195DFAE76F56564677", 16);

		point_affn_init(GENERATOR);
		GENERATOR->infinity = 0;
		mpz_init(GENERATOR->x);
		mpz_set_str(GENERATOR->x, (unsigned char*)"4AD5F7048DE709AD51236DE65E4D4B482C836DC6E4106640", 16);
		mpz_init(GENERATOR->y);
		mpz_set_str(GENERATOR->y,(unsigned char*) "02BB3A02D4AAADACAE24817A4CA3A1B014B5270432DB27D2", 16);

#define pXbufSubgrpOrder192		"\xB4\x00\x85\xC1\x06\xD7\x2F\xC0\xC5\x14\x3B\x79\xA6\x36\x39\x22\xE0\x05\x1C\x2A\x4B\xF0\x71\x59\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x07\x06\x00\x00"
#define pXbufFieldPrime192		"\xAC\xB2\xEC\x58\x1F\xC2\xB4\x53\x41\x6C\x51\xCB\xA5\x36\x39\x22\xE0\x05\x1C\x2A\x4B\xF0\x71\x59\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x07\x06\x00\x00"
		memcpy( &pXbufSubgrpOrder , pXbufSubgrpOrder192 ,0x50);
		memcpy( &pXbufFieldPrime , pXbufFieldPrime192 , 0x50);
		NN_BarrettSetBuf(&pXbufFieldPrime);
	}
	//	mpz_mod_BarrettInit(SUBGRP_ORDER, &pXbufSubgrpOrder);
	//	mpz_mod_BarrettInit(FIELD_P, &pXbufFieldPrime);
}

void get_data_from_pubkey(point_affn_ptr P, char * data, unsigned int* dlen)
{
	char* px;
	char* py;
	int i = 0;
	px = (char *)P->x;
	py = (char *)P->y;

	data[0] = 0x04;

	i = 0;
	while(i < eccbits/8){
		data[1 + i] = *(px + eccbits/8 - i -1) ;
		i++;
	}

	while(i < eccbits/4){
		data[1 + i] = *(py + eccbits/4 - i - 1) ;
		i++;
	}

	*dlen = 1 + eccbits/4;
}

void get_data_from_prvkey(unsigned int* prvkey, char * data, unsigned int* dlen)
{
	char* ps;
	int i = 0;
	ps = (char *)prvkey;
	
	i = 0;
	while(i < eccbits/8){
		data[i] = *(ps + eccbits/8 - i -1) ;
		i++;
	}
    
	*dlen = eccbits/8;
}

int sm2_key_gen_ext(
                    sm2_pubkey *pubkey,
                    sm2_prvkey *prvkey,
                    char* prvkeydata,
                    int*  prvLen,
                    char* pubkeydata,
                    int*  puLen
                    )
{
	mpz_init(prvkey->s);
	random_limit(prvkey->s, SUBGRP_ORDER);
	mpz_mod_BarrettInit(FIELD_P, &pXbufFieldPrime);
    
	point_affn_init(prvkey->sG);
	point_affn_mul(prvkey->sG, GENERATOR, prvkey->s);
	
	//init pubkey
	memset(pubkey, 0, sizeof(sm2_pubkey));
	point_affn_init(pubkey->sG);
	point_affn_mul(pubkey->sG, GENERATOR, prvkey->s);
	pubkey->ecp = ECPUCMP;
	
	if(*puLen < 0x41){
		*puLen = 0x41;
		return -1;
	}
	get_data_from_pubkey(pubkey->sG , pubkeydata, (unsigned int *)puLen );
    
	if(*prvLen < 0x20){
		*puLen = 0x20;
		return -1;
	}
    get_data_from_prvkey((unsigned int *)prvkey->s, prvkeydata, (unsigned int *)prvLen);
    
    //去掉最前的0x04
    if (pubkeydata[0] == 0x04) {
        for (int i = 0; i < *puLen; ++ i) {
            pubkeydata[i] = pubkeydata[i + 1];
        }
        *puLen -= 1;
    }

	return 0;
}

int sm2_key_gen(
	sm2_pubkey *pubkey,
	sm2_prvkey *prvkey,
	char* pubkeydata,
	int*  puLen
)
{
	mpz_init(prvkey->s);
	random_limit(prvkey->s, SUBGRP_ORDER);
	mpz_mod_BarrettInit(FIELD_P, &pXbufFieldPrime);
		
	point_affn_init(prvkey->sG);
	point_affn_mul(prvkey->sG, GENERATOR, prvkey->s);

	//init pubkey
	memset(pubkey, 0, sizeof(sm2_pubkey));
	point_affn_init(pubkey->sG);
	point_affn_mul(pubkey->sG, GENERATOR, prvkey->s);
	pubkey->ecp = ECPUCMP;

    
	if(*puLen < 0x41){
	     *puLen = 0x41;
	     return 0;
	}
	get_data_from_pubkey(pubkey->sG , pubkeydata, (unsigned int *)puLen );
	
	return 0;
}


int sm2_load_pubkey
(
	sm2_pubkey *pubkey,
	unsigned char *data,
	unsigned int n
)
{
	
	if( n != 48 && n != 64 && n != 49 && n != 65 )
		return 1;

	point_affn_init(pubkey->sG);
	pubkey->sG->infinity = 0;
	if(n%2 == 1)
		OS2ECP(pubkey->sG, data, n);
	else
		OS2ECP_XY(pubkey->sG, data, n);
    
	pubkey->ecp = ECPUCMP;
//	dump_point(pubkey->sG, "sG");
	return 0;
}


int sm2_load_prvkey
(
	sm2_prvkey *prvkey ,
	unsigned char *data,
	unsigned int n
)
{

	if( n != 24 && n != 32 )
		return 1;
	mpz_init(prvkey->s);
	OS2IP(prvkey->s, data, FIELD_SIZE_IN_BYTES);

	point_affn_init(prvkey->sG);
	prvkey->sG->infinity = 0;
	OS2ECP(prvkey->sG, data+FIELD_SIZE_IN_BYTES, n-FIELD_SIZE_IN_BYTES);

	point_affn_init(prvkey->sG);
	point_affn_mul(prvkey->sG, GENERATOR, prvkey->s);
//	dump_point(prvkey->sG, "sG");

	return 0;
}



void sm2_clear_pubkey
(
	sm2_pubkey *pubkey
)
{
	point_affn_clear(pubkey->sG);
}

void sm2_clear_prvkey
(
	sm2_prvkey *prvkey
)
{
	point_affn_clear(prvkey->sG);
	mpz_clear(prvkey->s);
}



void sm2_hash
(
	unsigned char *x,
	int xlen,
	unsigned char *m,
	int mlen,
	unsigned char *y,
	int ylen,
	unsigned char *hash
)
{
    sm3_context ctx;

    sm3_starts( &ctx );
    sm3_update( &ctx, x, xlen );
	if(x && xlen)
	    sm3_update( &ctx, m, mlen );
	if(y && ylen)
	    sm3_update( &ctx, y, ylen );
    sm3_finish( &ctx, hash );

    memset( &ctx, 0, sizeof( sm3_context ) );
}

int sm2_KDF1_ex
(
	unsigned char *fn, 
	unsigned int rlen, 
	unsigned char *message, 
	unsigned int mlen, 
	unsigned int halg,
	unsigned int counter
)
{
	unsigned char md_value[32];
	unsigned char ibuf[4];
	unsigned int pos;
	unsigned int md_length = sizeof(md_value);

	pos=0;

	while(pos<rlen){
#ifdef LT_ENDIAN
		ibuf[0]= ((unsigned char *)&counter)[3];
		ibuf[1]= ((unsigned char *)&counter)[2];
		ibuf[2]= ((unsigned char *)&counter)[1];
		ibuf[3]= ((unsigned char *)&counter)[0];
#else
		ibuf[0]= ((unsigned char *)&counter)[0];
		ibuf[1]= ((unsigned char *)&counter)[1];
		ibuf[2]= ((unsigned char *)&counter)[2];
		ibuf[3]= ((unsigned char *)&counter)[3];
#endif
		sm2_hash(message, mlen, ibuf, 4, NULL, 0, md_value);
		
		if(pos+md_length<rlen){
			memcpy(fn+pos, (unsigned char *)md_value, md_length);
			pos+=md_length;
		}
		else{
			memcpy(fn+pos, (unsigned char *)md_value, rlen-pos);
			pos=rlen;		
		}

		counter++;
	}

	return 1;
}

int  sm2_enc
(
	sm2_pubkey *pubkey,
	unsigned char *plaintext, 
	unsigned int  plen, 
	unsigned char *ciphertext, 
	unsigned int  *clen
)
{
	mpz_t k;
	point_affn_t kG;
	unsigned int c1len, xylen;
#ifndef MUL_BITS
	unsigned char tmp[FIELD_SIZE_IN_BYTES*2];
#else
	unsigned char tmp[32 * 2];
#endif
	unsigned int i;

	if( !ciphertext )
	{
		*clen = FIELD_SIZE_IN_BYTES+1+plen+SM3_HASH_LEN;
		return 0;
	}
	else
	{
		if( *clen < (FIELD_SIZE_IN_BYTES+1+plen+SM3_HASH_LEN) )
			return 1;
	}

	mpz_init(k);
	NN_BarrettSetBuf(&pXbufSubgrpOrder);
	random_limit(k, SUBGRP_ORDER);
	NN_BarrettSetBuf(&pXbufFieldPrime);
#ifdef USE_ECC_256
	//mpz_set_str(k, "4C62EEFD6ECFC2B95B92FD6C3D9575148AFA17425546D49018E5388D49DD7B4F", 16);
#else
	//mpz_set_str(k, "ADB6F4FE3E8B1D9E0DA8C0D40FC962195DFAE76F5656467", 16);
#endif

	point_affn_init(kG);
	point_affn_mul(kG, GENERATOR, k);
//	dump_point(kG, "kG"); 
	c1len = *clen;
	EC2OSP(ciphertext,(int *) &c1len, kG, pubkey->ecp);

	point_affn_clear(kG);
	point_affn_init(kG);
	point_affn_mul(kG, pubkey->sG, k);
//	dump_point(kG, "kG"); 
	mpz_clear(k);

	xylen = sizeof(tmp);
	EC2OSP_XY(tmp, (int *)&xylen, kG);
	point_affn_clear(kG);
		dump_mem(tmp, 0x30, "tmp");
	sm2_hash(tmp, xylen/2, plaintext, plen, tmp+xylen/2, xylen/2, ciphertext+c1len);

	sm2_KDF1_ex(ciphertext+c1len+SM3_HASH_LEN, plen, tmp, xylen, ALG_HASH_SM3, 1);

	for(i=0; i<plen; i++)
		ciphertext[c1len+SM3_HASH_LEN+i] ^= plaintext[i];

	*clen = c1len+plen+SM3_HASH_LEN;


	return 0;
}

int  sm2_dec
(
	sm2_prvkey *prvkey,
	unsigned char *ciphertext, 
	unsigned int clen,
	unsigned char*plaintext, 
	unsigned int *plen 
)
{
	point_affn_t ksG;
#ifndef MUL_BITS
	unsigned char tmp[FIELD_SIZE_IN_BYTES*2];
#else
	unsigned char tmp[32*2];
#endif
	unsigned char *hash = tmp;
	int c1len, xylen;
	unsigned int i;
	int ret;

	point_affn_init(ksG);

	c1len = OS2ECP(ksG, ciphertext, clen);

	if(c1len < 0 || (clen - c1len <= SM3_HASH_LEN) )
	{
		point_affn_clear(ksG);
		return 1;
	}
	mpz_BarrettSetBuf(&pXbufFieldPrime);
	point_affn_mul(ksG, ksG, prvkey->s);

	xylen = sizeof(tmp);
	EC2OSP_XY(tmp, &xylen, ksG);
	point_affn_clear(ksG);

	*plen = clen-c1len-SM3_HASH_LEN;
	sm2_KDF1_ex(plaintext, *plen, tmp, xylen, ALG_HASH_SM3, 1);
	for(i=0; i<*plen; i++)
		plaintext[i] = ciphertext[c1len+SM3_HASH_LEN+i]^plaintext[i];

	sm2_hash(tmp, xylen/2, plaintext, *plen, tmp+xylen/2, xylen/2, hash);
	if( !memcmp(hash, ciphertext+c1len, SM3_HASH_LEN) )
		ret = 0;
	else
	{
		memset(plaintext, 0, *plen);
		ret = 1;
	}

	return ret;
}

void sm2_z
(
	unsigned char  *id,
	unsigned int ilen,
	point_affn_ptr G,
	point_affn_ptr sG,
	unsigned char  *hash
)
{
    sm3_context ctx;
#ifndef MUL_BITS
	unsigned char xt[FIELD_SIZE_IN_BYTES*2];
#else
	unsigned char xt[32*2];
#endif
	unsigned short nlen = ilen*8;
	int tlen;

    sm3_starts( &ctx );

#ifdef LT_ENDIAN
	xt[0]= ((unsigned char *)&nlen)[1];
	xt[1]= ((unsigned char *)&nlen)[0];
#else
	xt[0]= ((unsigned char *)&nlen)[0];
	xt[1]= ((unsigned char *)&nlen)[1];
#endif
    sm3_update( &ctx, xt, 2 );
//	dump_mem(xt, 2, "idlen");

	if(id && ilen)
		sm3_update( &ctx, id, ilen );
//	dump_mem(id, ilen, "id");

	tlen = FIELD_SIZE_IN_BYTES;
	I2OSP(xt, tlen, COA);
	sm3_update( &ctx, xt, tlen );
//	dump_mem(xt, tlen, "a");

	tlen = FIELD_SIZE_IN_BYTES;
	I2OSP(xt, tlen, COB);
	sm3_update( &ctx, xt, tlen );
//	dump_mem(xt, tlen, "b");

	tlen = sizeof(xt);
	EC2OSP_XY(xt, &tlen, G);
	sm3_update( &ctx, xt, tlen );
//	dump_mem(xt, tlen, "G");

	tlen = sizeof(xt);
	EC2OSP_XY(xt, &tlen, sG);
	sm3_update( &ctx, xt, tlen );
//	dump_mem(xt, tlen, "sG");

    sm3_finish( &ctx, hash );

    memset( &ctx, 0, sizeof( sm3_context ) );
}

int  sm2_sign
(
	sm2_prvkey *prvkey,
	unsigned char *id,
	unsigned int ilen,
	unsigned char *message, 
	unsigned int mlen,
	unsigned char *signature, 
	unsigned int *slen 	
)
{
#ifndef MUL_BITS
	unsigned char xt[FIELD_SIZE_IN_BYTES*2];
#else
	unsigned char xt[32*2];
#endif
	mpz_t e, k, s;
	point_affn_t kG;
	//unsigned int xlen = sizeof(xt);

	if( !signature )
	{
		*slen = FIELD_SIZE_IN_BYTES*2;
		return 0;
	}

	if(*slen < (unsigned int)(FIELD_SIZE_IN_BYTES*2))
		return 1;

	sm2_z(id, ilen, GENERATOR, prvkey->sG, xt);
//	dump_mem(z, sizeof(z), "z");
	sm2_hash(xt, SM3_HASH_LEN, message, mlen, NULL, 0, xt);
//	dump_mem(h, sizeof(h), "h");
	mpz_init(e);
	OS2IP(e, xt, SM3_HASH_LEN);	//e
	NN_BarrettSetBuf(&pXbufSubgrpOrder);
	mpz_mod(e, e, SUBGRP_ORDER);

	mpz_init(k);

	random_limit(k, SUBGRP_ORDER);
	NN_BarrettSetBuf(&pXbufFieldPrime);

#ifdef USE_ECC_256
	//mpz_set_str(k, "6CB28D99385C175C94F94E934817663FC176D925DD72B727260DBAAE1FB2F96F", 16);
#else
	//mpz_set_str(k, "ADB6F4FE3E8B1D9E0DA8C0D40FC962195DFAE76F5656460", 16);
#endif

	point_affn_init(kG);
	point_affn_mul(kG, GENERATOR, k);
//	dump_point(kG, "kG");

//	gmp_printf("x: %Zx\n", x);
	mpz_BarrettSetBuf(&pXbufSubgrpOrder);
	//mpz_mod_BarrettInit(SUBGRP_ORDER, &pXbuf);
	zp_add_mod_p(e, e, kG->x, SUBGRP_ORDER);

	I2OSP(signature, FIELD_SIZE_IN_BYTES, e);
	point_affn_clear(kG);

	mpz_init(s);
	mpz_add_ui(s, prvkey->s, 1);	//1+d_A
	mpz_invert(s, s, SUBGRP_ORDER);	//(1+d_A)^-1
	zp_mul_mod_p(e, e, prvkey->s, SUBGRP_ORDER);
	zp_sub_mod_p(k, k, e, SUBGRP_ORDER);
	zp_mul_mod_p(s, s, k, SUBGRP_ORDER);

	//mpz_mod_BarrettInit(FIELD_P, &pXbuf);
	mpz_BarrettSetBuf(&pXbufFieldPrime);

	I2OSP(signature+FIELD_SIZE_IN_BYTES, FIELD_SIZE_IN_BYTES, s);
	*slen = FIELD_SIZE_IN_BYTES*2;

	mpz_clear(e);
	mpz_clear(k);
	mpz_clear(s);

	return 0;
}

int sm2_verify
(
	sm2_pubkey *pubkey,
	unsigned char *id,
	unsigned int ilen,
	unsigned char *message, 
	unsigned int mlen,
	unsigned char *signature, 
	unsigned int slen 	
)
{
#ifndef MUL_BITS
	char xt[FIELD_SIZE_IN_BYTES>SM3_HASH_LEN?FIELD_SIZE_IN_BYTES:SM3_HASH_LEN];
#else
	char xt[SM3_HASH_LEN];
#endif
	mpz_t e, t, s, r;
	point_affn_t sG, tP;
	unsigned int xlen = sizeof(xt);
	int ret;

	if(slen < (unsigned int)(FIELD_SIZE_IN_BYTES*2) )
		return 1;

	sm2_z(id, ilen, GENERATOR, pubkey->sG, (unsigned char*)xt);
	sm2_hash((unsigned char*)xt, SM3_HASH_LEN, message, mlen, NULL, 0, (unsigned char*)xt);
	mpz_init(e);
	OS2IP(e, (unsigned char*)xt, SM3_HASH_LEN);	//e
	mpz_BarrettSetBuf(&pXbufSubgrpOrder);
	mpz_mod(e, e, SUBGRP_ORDER);
//	gmp_printf("e: %Zx\n", e);

	mpz_init(r);
	OS2IP(r, signature, FIELD_SIZE_IN_BYTES);
	mpz_init(s);
	OS2IP(s, signature+FIELD_SIZE_IN_BYTES, FIELD_SIZE_IN_BYTES);

	mpz_init(t);
	//mpz_mod_BarrettInit(SUBGRP_ORDER, &pXbuf);

	zp_add_mod_p(t, r, s, SUBGRP_ORDER);

	//mpz_mod_BarrettInit(FIELD_P, &pXbuf);
	mpz_BarrettSetBuf(&pXbufFieldPrime);
	point_affn_init(sG);
	point_affn_mul(sG, GENERATOR, s);
//	dump_point(sG, "sG");

	point_affn_init(tP);
	point_affn_mul(tP, pubkey->sG, t);
//	dump_point(tP, "tP");

	point_affn_add(sG, sG, tP);
//	dump_point(sG, "sG");
	point_affn_clear(tP);
	zp_add_mod_p(t, e, sG->x, SUBGRP_ORDER);

	ret = mpz_cmp(t, r);
	if( ret )
		ret = 1;

	mpz_clear(e);
	mpz_clear(t);
	mpz_clear(s);
	mpz_clear(r);
	point_affn_clear(sG);

	//mpz_mod_BarrettInit(FIELD_P, &pXbuf);
	mpz_BarrettSetBuf(&pXbufFieldPrime);

	return ret;
}

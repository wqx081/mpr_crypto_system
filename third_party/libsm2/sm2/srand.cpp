#include "stdafx.h"
#include "const.h"
#include "srand.h"
#include "bigint.h"
#include "paramp.h"
#include "stdlib.h"
#include "time.h"

#ifdef WIN32
#define _WIN32_WINNT 0x0600

#include <windows.h>
#include <Wincrypt.h>


static int WRNG_random_bits(char *bytes, int bits, void *data)
{

    /*
	LPCSTR provname = (LPCSTR) data;
	HCRYPTPROV hProv;
	int bytecount, ret;

	ret = CryptAcquireContext(&hProv, (LPCSTR)NULL, provname, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);

	bytecount = (bits + 7) / 8;
	CryptGenRandom(hProv, bytecount, (BYTE *)bytes);
	CryptReleaseContext(hProv, 0);

	return ret;
     */
}

#endif

#include <stdio.h>

U8 random_string(
    unsigned char *rand, 
    int rlen
)
{
    FILE *fp = fopen("/dev/urandom", "rb");
    if (fp)
    {
        fread(rand, 1, rlen, fp);
        fclose(fp);
        
    }
    /*
    for (int i=0; i<rlen; i++)
    {
        printf("%x", rand[i]);
    }
    printf("\n");
     */
    return 1;
    /*
#if 0
	int i;
	srand((unsigned int)time((time_t *)NULL)); 
	for(i=0;i<rlen;i++)
	{	
		
		r[i]=rand() ;
	}
	return 1;
#else

#ifdef UNICODE
	WRNG_random_bits(rand, rlen*8, _T("Microsoft Base Cryptographic Provider v1.0"));
#else
	WRNG_random_bits((char *)rand, rlen*8, "Microsoft Base Cryptographic Provider v1.0");
#endif
	return 1;

#endif
     */

}

void random_limit (
    mpz_ptr x, 
    mpz_ptr p
)
{
	U16 n=(mpz_sizeinbase(p,2)+7)/8;
	//U8 *ptr=((U8 *)(x->data))+4;
	unsigned long size = 0;

   // random_string(ptr+MAX_FIELD_BYTES-n,n);
	U8 *ptr=((U8 *)(x->data));
	 random_string(ptr, n);
     
	while (mpz_cmp(x, p) > 0) {
		size = mpz_sizeinbase(x, 2) - 1;
		mpz_clrbit(x, size);
    }    

}



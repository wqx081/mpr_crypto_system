#include "stdafx.h"
#include "stdlib.h"
#include "time.h"

unsigned char tlRAND_bytes(
    unsigned char *r,
    int rlen
){
    int i;
	for(i=0;i<rlen;i++)
	{	srand( time(NULL) ); 
		r[i]=rand() ;
		//r[i]=(unsigned char)Fwl_GetRand(0xff);;
	}
	return 1;
}



#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "MPRAsymmCrypt.h"

void dump_hexstring(unsigned char* buf, unsigned int len)
{
	static char str[1024] = {0};
	memset(str, 0, 1024);

    unsigned char * pin = buf;
    const char * hex = "0123456789ABCDEF";
    char * pout = str;
    int i = 0;
    //for(; i < sizeof(buf)-1; ++i){
    for(; i < len-1; ++i){
        *pout++ = hex[(*pin>>4)&0xF];
        *pout++ = hex[(*pin++)&0xF];
        *pout++ = ':';
    }
    *pout++ = hex[(*pin>>4)&0xF];
    *pout++ = hex[(*pin)&0xF];
    *pout = 0;

    printf("%s\n", str);
}

void hexstring_to_ascii(unsigned char* buf, unsigned int len)
{
	static unsigned char str[1024] = {0};
	memset(str, 0, 1024);

	unsigned char* pin = buf;
	unsigned char* pout= str;

	int i=0;
	for (; i < len-1; ++i) {
		*pout++ = (char)*pin++;
	}
	printf("%s\n", str);
}

int main(int argc, char **argv)
{
	int ret;
	const int KEY_BITS = 256;

	unsigned char pubkey[128] = {0};
	unsigned int  pubkey_len = 128;
	unsigned char prvkey[128] = {0};
	unsigned int  prvkey_len = 128;
	
	
	unsigned char sbuf[1024] = {0};

	 
	unsigned char text[] = "Hello World";
	unsigned char cipherLicense[256] = {0};
	unsigned int  cipher_len = 256;
	unsigned char decipherLicense[256] = {0};
	unsigned int  decipher_len = 256;
	
	FILE	*fp = NULL;

	if (argc != 2) {
		fprintf(stderr, "USAGE: %s [INPUT FILE]\n", argv[0]);
		return 1;
	}

	fp = fopen(argv[1], "w+");
	if (fp == NULL) {
		fprintf(stderr, "open %s error\n", argv[1]);
		return 1;
	}
	
	ret = MPRAsymmCrypt_Init();
	if (ret != 0) 	
		return -1;
	ret = MPRAsymmCrypt_GenerateKeyPair( SM2, KEY_BITS,
                                         pubkey, &pubkey_len,
                                         prvkey, &prvkey_len);
	if (ret != 0) {
		fprintf(stderr, "generate sm2 key pair error:%d\n", ret);
		goto end;
	}	
	fprintf(stderr, "pub_key: ");
	dump_hexstring(pubkey, pubkey_len);
	fprintf(stderr, "prv_key: ");
	dump_hexstring(prvkey, prvkey_len);	

    ret = MPRAsymmCrypt_PubEncrypt(SM2, pubkey, pubkey_len, 
		(const unsigned char *)text, strlen((char*)text), cipherLicense, &cipher_len); 
	if (ret != 0) {
		fprintf(stderr, "Pub encrypt error:%d\n", ret);
		goto end;
	}
	dump_hexstring(cipherLicense, cipher_len);

	fwrite(cipherLicense, sizeof(unsigned char), cipher_len, fp);
	rewind(fp);
	fread(sbuf, sizeof(unsigned char), cipher_len, fp);
	dump_hexstring(sbuf, cipher_len);

	//fprintf(stderr, "cipher_len=%d\n", cipher_len);

	//dump_key(cipherLicense, cipher_len);

#if 0
    ret = MPRAsymmCrypt_PrvDecrypt(SM2, prvkey, prvkey_len, 
		cipherLicense, cipher_len, decipherLicense, &decipher_len);
	if (ret != 0) {
		fprintf(stderr, "Prv Decrypt error:%d\n", ret);
		goto end;
	}

	fprintf(stderr, "decipher_len=%d\n", decipher_len);
	dump_hexstring(decipherLicense, decipher_len);
	hexstring_to_ascii(decipherLicense, decipher_len);
#endif
	
	ret = MPRAsymmCrypt_PrvDecrypt(SM2, prvkey, prvkey_len,
		sbuf, cipher_len, decipherLicense, &decipher_len);


	if (ret != 0) {
		fprintf(stderr, "Prv Decrypt error:%d \n", ret);
		goto end;
	}
	hexstring_to_ascii(decipherLicense, decipher_len);

	
end:
	fclose(fp);
	MPRAsymmCrypt_Uninit();
	return ret;
}

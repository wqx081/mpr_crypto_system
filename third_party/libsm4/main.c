#include <stdio.h>
#include <string.h>
#include "cryptlib.h"

int test_sm4()
{
	unsigned char key[16] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};
	char input1[] = "this is a test string|";
	char input2[] = "hello world!";
	unsigned char cipher[128];
	unsigned char decipher[128];
	cipher_context ctx;
	unsigned char iv[16];
	int i, ret, buflen, cipherlen, decipherlen;
	unsigned char* pbuf;

	memset(iv, 0x02, 16);

	// sm4 encryption
	ret = crypt_enc_init(&ctx, key, 16, iv, ALG_SM4_CBC);
	if( !ret )
	{
		printf("crypt_enc_init() failed!\n");
		return -1;
	}
	buflen = 128;
	pbuf = cipher;
	// feed segment 1
	ret = crypt_enc_update(&ctx, (uint8 *)input1, strlen(input1), pbuf, &buflen);
	if( !ret )
	{
		printf("crypt_enc_update() failed!\n");
		return -1;
	}
	pbuf += buflen;
	buflen = cipher+128-pbuf;
	// feed segment 2
	ret = crypt_enc_update(&ctx, (uint8 *)input2, strlen(input2), pbuf, &buflen);
	if( !ret )
	{
		printf("crypt_enc_update() failed!\n");
		return -1;
	}
	pbuf += buflen;
	buflen = cipher+128-pbuf;
	ret = crypt_enc_final(&ctx, pbuf, &buflen);
	if( !ret )
	{
		printf("crypt_enc_final() failed!\n");
		return -1;
	}
	pbuf += buflen;
	cipherlen = pbuf-cipher;
	printf("cipher len : %d\n", cipherlen);
	for(i=0; i<cipherlen; i++)
		printf("%02x ", cipher[i]);
	printf("\n");

	crypt_enc_end(&ctx);

	// sm4 decryption
	ret = crypt_dec_init(&ctx, key, 16, iv, ALG_SM4_CBC);
	if( !ret )
	{
		printf("crypt_dec_init() failed!\n");
		return -1;
	}
	buflen = 128;
	pbuf = decipher;
	// feed segment 1
	ret = crypt_dec_update(&ctx, cipher, 11, pbuf, &buflen);
	if( !ret )
	{
		printf("crypt_dec_update() failed!\n");
		return -1;
	}
	pbuf += buflen;
	buflen = decipher+128-pbuf;
	// feed segment 2
	ret = crypt_dec_update(&ctx, cipher+11, cipherlen-11, pbuf, &buflen);
	if( !ret )
	{
		printf("crypt_dec_update() failed!\n");
		return -1;
	}
	pbuf += buflen;
	buflen = decipher+128-pbuf;
	ret = crypt_dec_final(&ctx, pbuf, &buflen);
	if( !ret )
	{
		printf("crypt_dec_final() failed!\n");
		return -1;
	}
	pbuf += buflen;

	decipherlen = pbuf-decipher;
	decipher[decipherlen] = 0;
	printf("decipher len : %d\n", decipherlen);
	printf("%s\n", (char *)decipher);

	crypt_dec_end(&ctx);

	return 0;
}

int test_aes()
{
	unsigned char key[16] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};
	char input1[] = "this is a test string|";
	char input2[] = "hello world!";
	unsigned char cipher[128];
	unsigned char decipher[128];
	cipher_context ctx;
	unsigned char iv[16];
	int i, ret, buflen, cipherlen, decipherlen;
	unsigned char* pbuf;

	memset(iv, 0x02, 16);

	// sm4 encryption
	ret = crypt_enc_init(&ctx, key, 16, iv, ALG_AES_ECB);
	if( !ret )
	{
		printf("crypt_enc_init() failed!\n");
		return -1;
	}
	buflen = 128;
	pbuf = cipher;
	// feed segment 1
	ret = crypt_enc_update(&ctx, (uint8 *)input1, strlen(input1), pbuf, &buflen);
	if( !ret )
	{
		printf("crypt_enc_update() failed!\n");
		return -1;
	}
	pbuf += buflen;
	buflen = cipher+128-pbuf;
	// feed segment 2
	ret = crypt_enc_update(&ctx, (uint8 *)input2, strlen(input2), pbuf, &buflen);
	if( !ret )
	{
		printf("crypt_enc_update() failed!\n");
		return -1;
	}
	pbuf += buflen;
	buflen = cipher+128-pbuf;
	ret = crypt_enc_final(&ctx, pbuf, &buflen);
	if( !ret )
	{
		printf("crypt_enc_final() failed!\n");
		return -1;
	}
	pbuf += buflen;
	cipherlen = pbuf-cipher;
	printf("cipher len : %d\n", cipherlen);
	for(i=0; i<cipherlen; i++)
		printf("%02x ", cipher[i]);
	printf("\n");

	crypt_enc_end(&ctx);

	// sm4 decryption
	ret = crypt_dec_init(&ctx, key, 16, iv, ALG_AES_ECB);
	if( !ret )
	{
		printf("crypt_dec_init() failed!\n");
		return -1;
	}
	buflen = 128;
	pbuf = decipher;
	// feed segment 1
	ret = crypt_dec_update(&ctx, cipher, 11, pbuf, &buflen);
	if( !ret )
	{
		printf("crypt_dec_update() failed!\n");
		return -1;
	}
	pbuf += buflen;
	buflen = decipher+128-pbuf;
	// feed segment 2
	ret = crypt_dec_update(&ctx, cipher+11, cipherlen-11, pbuf, &buflen);
	if( !ret )
	{
		printf("crypt_dec_update() failed!\n");
		return -1;
	}
	pbuf += buflen;
	buflen = decipher+128-pbuf;
	ret = crypt_dec_final(&ctx, pbuf, &buflen);
	if( !ret )
	{
		printf("crypt_dec_final() failed!\n");
		return -1;
	}
	pbuf += buflen;

	decipherlen = pbuf-decipher;
	decipher[decipherlen] = 0;
	printf("decipher len : %d\n", decipherlen);
	printf("%s\n", (char *)decipher);

	crypt_dec_end(&ctx);

	return 0;
}

int test_ctr()
{
	unsigned char key[16] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};
	char input1[] = "Super Smack is a database benchmarking tool. It currently supports"
				"MySQL and PosgreSQL, and has some initial infrastructure for"
				"Oracle. Refer to the INSTALL file for installation instructions. To"
				"learn how to write your own tests, see TUTORIAL, and also study the"
				"examples in the smack directory, and read the comments.";
	char input2[] = "hello world!";
	unsigned char cipher[1024];
	unsigned char decipher[1024];
	cipher_context ctx;
	int i, ret, cipherlen, decipherlen;
	unsigned char* pbuf;
	int offset;

	ret = crypt_enc_init(&ctx, key, 16, NULL, ALG_SM4_CTR);
	if( !ret )
	{
		printf("crypt_enc_init() failed!\n");
		return -1;
	}
	pbuf = cipher;

	// 加密原文input1
	offset = 0;
	ret = crypt_ctr_encrypt(&ctx, (uint8 *)input1, pbuf, strlen(input1), offset);
	if( !ret )
	{
		printf("crypt_ctr_encrypt() failed!\n");
		return -1;
	}
	pbuf += strlen(input1);
	offset += strlen(input1);
	// 加密原文input2
	ret = crypt_ctr_encrypt(&ctx, (uint8 *)input2, pbuf, strlen(input2), offset);
	if( !ret )
	{
		printf("crypt_ctr_encrypt() failed!\n");
		return -1;
	}
	pbuf += strlen(input2);
	cipherlen = pbuf-cipher;
	printf("cipher len : %d\n", cipherlen);
	for(i=0; i<cipherlen; i++)
		printf("%02x ", cipher[i]);
	printf("\n");

	crypt_enc_end(&ctx);

	ret = crypt_dec_init(&ctx, key, 16, NULL, ALG_SM4_CTR);
	if( !ret )
	{
		printf("crypt_dec_init() failed!\n");
		return -1;
	}

	pbuf = decipher;
	offset = 0;
	decipherlen = 35;
	// 解密密文区间0至35字节的数据
	ret = crypt_ctr_decrypt(&ctx, cipher+offset, pbuf, decipherlen, offset);
	if( !ret )
	{
		printf("crypt_dec_update() failed!\n");
		return -1;
	}
	pbuf[decipherlen] = 0;
	printf("%s\n", pbuf);

	// 解密密文区间40至110字节的数据
	offset = 40;
	decipherlen = 70;
	ret = crypt_ctr_decrypt(&ctx, cipher+offset, pbuf, decipherlen, offset);
	if( !ret )
	{
		printf("crypt_dec_update() failed!\n");
		return -1;
	}
	pbuf[decipherlen] = 0;
	printf("%s\n", pbuf);

	crypt_dec_end(&ctx);

	return 0;
}

void main(void)
{
//	test_sm4();
	test_aes();
//	test_ctr();
}

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <libgen.h>
#include <errno.h>

#include "MPRSymmCrypt.h"


#define AJUST_LEN(x)    (((x)+((16)-1))&(~((16)-1)))

long 
file_size(const char *fname)
{
    FILE *fp = fopen(fname, "rb");
    long len;

    if (fp == NULL)
        return -1;
    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fclose(fp);
    return len;
}

int
one_file_encrypt(const char *src, const char *dst)
{
	FILE	*ofp, *ifp;
	uint8_t	buf[1024];
	uint8_t	obuf[1024];
	uint32_t in_len;
	uint32_t out_len;
	int	ret;
	int	t_len;

    uint32_t encrypted_offset = 0;

    int len  = file_size(src);


    ifp = fopen(src, "rb");

	ofp = fopen(dst, "wb");


    while (len > 0) {

        in_len = fread(buf, sizeof(char), 1024, ifp);
        if (in_len == 0 && feof(ifp))
            break;

        if (in_len % 16 != 0) {
            t_len = AJUST_LEN(in_len);
            memset(buf + in_len, 0, t_len - in_len);
        }
        else
            t_len = in_len;

        out_len = t_len;

        if ((ret = MPRSymmCrypt_Encrypt(buf, t_len, encrypted_offset, obuf, &out_len)) != 0) {
            if (ret == BAD_PARAM) {
                fprintf(stderr, "----- MPRSymmCrypt_Encrypt PARAM bad\n\n");
            }
            else {
                fprintf(stderr, "----- MPRSymmCrypt_Encrypt Encrypt error\n\n");
            }
            fclose(ifp);
            fclose(ofp);
            ret = 1;
            goto end;
        }

        encrypted_offset += out_len;
        fwrite(obuf, sizeof(char), t_len, ofp);
        len -= t_len;
    }

    fclose(ifp);
    fclose(ofp);
end:
	return ret;
}

int
one_file_decrypt(const char* src, const char *dst)
{
    FILE    *ofp;
	FILE	*ifp;
    uint8_t buf[1024];
    uint8_t obuf[1024];
    uint32_t in_len;
    uint32_t out_len;
    uint32_t expect_len;
    uint32_t len;
    int     ret;
    uint32_t tlen;

    uint32_t decrypted_offset = 0;

    ifp = fopen(src, "rb");
	len = file_size(src);
    ofp = fopen(dst, "wb");
	
	
    while (len > 0) {

    	expect_len = len >= 1024 ? 1024 : len;

        //fprintf(stderr, "----------- len: %d expect_len: %d", len, expect_len);

        in_len = fread(buf, sizeof(char), expect_len, ifp);
        if (ferror(ifp) || feof(ifp)) {
        	fprintf(stderr, "fread error:%s\n", strerror(errno));
            //exit(1);
        }
        if (in_len == 0)
        	break;

        if (in_len % 16 != 0) {
            tlen = AJUST_LEN(in_len);
            memset(buf + in_len, 0, tlen - in_len);
        }
        else
        	tlen = in_len;
                //fprintf(stderr, " tlen:%d\n", tlen);
        out_len = tlen;

                //if ((ret = MPRSymmCrypt_Decrypt(buf, tlen, 0, obuf, &out_len)) != 0) {
        if ((ret = MPRSymmCrypt_Decrypt(buf, tlen, decrypted_offset, obuf, &out_len)) != 0) {
        	if (ret == BAD_PARAM) {
            // LOG
            	fprintf(stderr, "----- PARAM bad\n\n");
            } else {
                fprintf(stderr, "----- Decrypt ERROR\n\n");
            }
            return 1;
        }

        decrypted_offset += out_len;

        fwrite(obuf, sizeof(char), out_len, ofp);
        len -= out_len;
	}

	fclose(ifp);
	fclose(ofp);
    return 0;

}

int main(int argc, char **argv)
{
	int	ret;
	
	unsigned char key[] = {
    0xa6, 0xcb, 0xf2, 0x5f,
    0x0d, 0x17, 0x4d, 0x0e,
    0x8e, 0x6c, 0x55, 0x8c,
    0x52, 0x9d, 0xbb, 0x60
	};
	int key_len = 16;

	ret = MPRSymmCrypt_Init(key, key_len, 0, 0, AES_ECB);
	if (ret != 0) {
		fprintf(stderr, "MPRSymmCrypt_Init: ERROR code:%d\n", ret);
		return ret;
	}

	one_file_encrypt(argv[1], argv[2]);
	
	one_file_decrypt(argv[2], argv[3]);

	MPRSymmCrypt_Uninit();
	return ret;
}

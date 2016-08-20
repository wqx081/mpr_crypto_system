
#ifndef XYSSL_SM3_H
#define XYSSL_SM3_H


typedef struct
{
    unsigned int total[2];     /*!< number of bytes processed  */
    unsigned int state[8];     /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */

    unsigned char ipad[64];     /*!< HMAC: inner padding        */
    unsigned char opad[64];     /*!< HMAC: outer padding        */
}sm3_context;

#ifdef __cplusplus
extern "C" {
#endif

void sm3_starts( 
    sm3_context *ctx
);

void sm3_update( 
    sm3_context *ctx,
    unsigned char *input, 
    int ilen 
);

void sm3_finish( 
    sm3_context *ctx,
    unsigned char *output 
);

void sm3( 
    unsigned char *input, 
    int ilen,
    unsigned char *output
);

int sm3_file( 
    char *path, 
    unsigned char *output 
);

void sm3_hmac_starts( 
    sm3_context *ctx, 
    unsigned char *key, 
    int keylen
);

void sm3_hmac_update(
    sm3_context *ctx, 
    unsigned char *input,
    int ilen 
);

void sm3_hmac_finish( 
    sm3_context *ctx, 
    unsigned char *output 
);

void sm3_hmac( 
    unsigned char *key, 
    int keylen,
    unsigned char *input, 
    int ilen,
    unsigned char *output
);

#ifdef __cplusplus
}
#endif

#endif /* sm3.h */

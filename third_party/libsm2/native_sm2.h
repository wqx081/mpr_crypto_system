#ifndef __NATIVE_SM2_H__
#define __NATIVE_SM2_H__

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifdef LINUX
#include <sys/socket.h>
#endif

#ifndef uint8
#define uint8	unsigned char
#endif

#ifndef uint16
#define uint16	unsigned short
#endif

#ifndef uint32
#define uint32	unsigned int
#endif

#ifndef int8
#define int8	char
#endif

#ifndef int16
#define int16	short
#endif

#ifndef int32
#define int32	int
#endif

typedef void* ObjectPointer;




#define SDR_OK					0x0			//操作成功
#define SDR_UNKNOWERR			0x00000001	//未知错误
#define SDR_NOTSUPPORT			0x00000002	//不支持的接口调用
#define SDR_COMMFAIL			0x00000003	//与设备通信失败
#define SDR_HARDFAIL			0x00000004	//运算模块无响应
#define SDR_OPENDEVICE			0x00000005	//打开设备失败
#define SDR_OPENSESSION			0x00000006	//创建会话失败
#define SDR_PARDENY				0x00000007	//无私钥使用权限
#define SDR_KEYNOTEXIST			0x00000008	//不存在的密钥调用
#define SDR_ALGNOTSUPPORT		0x00000009	//不支持的算法调用
#define SDR_ALGMODNOTSUPPORT	0x0000000A	//不支持的算法模式调用
#define SDR_PKOPERR				0x0000000B	//公钥运算失败
#define SDR_SKOPERR				0x0000000C	//私钥运算失败
#define SDR_SIGNERR				0x0000000D	//签名运算失败
#define SDR_VERIFYERR			0x0000000E	//验证签名失败
#define SDR_SYMOPERR			0x0000000F	//对称算法运算失败
#define SDR_STEPERR				0x00000010	//多步运算步骤错误
#define SDR_FILESIZEERR			0x00000011	//文件长度超出限制
#define SDR_FILENOEXIST			0x00000012	//指定的文件不存在
#define SDR_FILEOFSERR			0x00000013	//文件起始位置错误
#define SDR_KEYTYPEERR			0x00000014	//密钥类型错误
#define SDR_KEYERR				0x00000015	//密钥错误
#define SDR_DATASIZEERR			0x00000020	//数据长度超出限制
#define SDR_FILEOPERR			0x00000021
#define SDR_MEMERR				0x00000022
#define SDR_PARAMERR			0x00000023


#define SGD_RSA					0x00010000	
#define SGD_SM2_1				0x00020100		//椭圆曲线签名算法
#define SGD_SM2_2				0x00020200		//椭圆曲线密钥交换协议
#define SGD_SM2_3				0x00020400		//椭圆曲线加密算法

#define SGD_SM3		0x00000001
#define SGD_SHA1	0x00000002
#define SGD_SHA256	0x00000004

#ifdef __cplusplus
extern "C" {
#endif

extern int32 precom;



#define XENGINE_DEVATTR_MAINAUTHINFO			1
#define XENGINE_DEVATTR_MAINNETWORKINFO			2
#define XENGINE_DEVATTR_BACKAUTHINFO			3
#define XENGINE_DEVATTR_BACKNETWORKINFO			4
#define XENGINE_DEVATTR_MAINADDRESS				5
#define XENGINE_DEVATTR_BACKADDRESS				6



#define SM3_HASH_LEN			32
#define ECCref_MAX_BITS			256 
#define ECCref_MAX_LEN			((ECCref_MAX_BITS+7) / 8)

typedef struct ECCrefPublicKey_st
{
	uint32 bits;
	uint8 x[ECCref_MAX_LEN]; 
	uint8 y[ECCref_MAX_LEN]; 
}ECCrefPublicKey;

typedef struct ECCrefPrivateKey_st
{
    uint32 bits;
    uint8 D[ECCref_MAX_LEN];
}ECCrefPrivateKey;

typedef struct ECCSignatureEx_st
{
	uint32 bits;
	uint8 r[ECCref_MAX_LEN];	
	uint8 s[ECCref_MAX_LEN];	
} ECCSignatureEx;

typedef struct ECCCipherEx_st
{
	uint32 bits;
	uint32 mbits;
	uint8 x[ECCref_MAX_LEN]; 
	uint8 y[ECCref_MAX_LEN]; 
	uint8 C[ECCref_MAX_LEN];
    uint8 M[ECCref_MAX_LEN];
}ECCCipherEx;


int32 SNF_ExternalSign_ECC(
	void* sessionCtx,
	uint32 uiAlgID,
	ECCrefPrivateKey *pucPrivateKey,
	uint8 *pid,
	uint32 ilen,
	uint8 *message,
	uint32 mlen,
	ECCSignatureEx *pucSignature
);

int32 SNF_ExternalVerify_ECC(
	void* sessionCtx,
	uint32 uiAlgID,
	ECCrefPublicKey *pucPublicKey,
	uint8 *pid,
	uint32 ilen,
	uint8 *message,
	uint32 mlen,
	ECCSignatureEx *pucSignature
);

int32 SNF_ExternalEncrypt_ECC(
	void* sessionCtx,
	uint32 uiAlgID,
	ECCrefPublicKey *pucPublicKey,
	uint8 *pucData,
	uint32 uiDataLength,
	ECCCipherEx *pucEncData
);

int32 SNF_ExternalDecrypt_ECC(
	void* sessionCtx,
	uint32 uiAlgID,
	ECCrefPrivateKey *pucPrivateKey,
	ECCCipherEx *pucEncData,
	uint8 *pucData,
	uint32 *puiDataLength
);




#ifdef __cplusplus
}
#endif

#endif
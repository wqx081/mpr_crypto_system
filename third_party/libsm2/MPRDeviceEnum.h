#ifndef __MPRDEVICEENUM_H__
#define __MPRDEVICEENUM_H__

//MPR设备类型枚举
typedef enum {
    MPRDevice_Invalid,     //无效的MPR设备
    MPRDevice_PenReader,   //MPR识读器（点读笔）
    MPRDevice_BookDisk,    //MRR书盘（UKey）
    MPRDevice_Dongle       //USB dongle,无媒体存储能力
} MPRDeviceType;

//接口函数返回的错误代码
typedef enum {
    DEVICE_OK               = 0,  //接口函数调用成功
    BAD_PARAM               = -1, //传入的接口函数参数错误
    BAD_CALL                = -2, //接口函数调用顺序错误
    DEVICE_NOT_EXIST        = -3, //设备不存在
    COMM_FAIL               = -4, //下位机通讯错误
    LOW_PRIORITY            = -5, //其他程序正在通讯且优先级高
    ONE_EXIST               = -6, //只有一个程序存在
    SYS_ERROR               = -7, //其它错误
    VERIFY_SIGNATURE_FAIL   = -8, //验证license签名失败
    NO_THIS_LINE            = -9, //downloadlist中没有该行
    DECRYPT_FAIL            = -10 //下位机解密失败
} MPRDeviceErrorCode;

//识读器支持的音频格式
typedef enum {
    WAV = 0x00000001L,     //非压缩wave格式
    MP3 = 0x00000002L,     //MP3压缩格式
    OGG = 0x00000004L      //OGG压缩格式
} MPRAudioFormat;

//设备支持的对称加密算法类型
typedef enum {
    AES_ECB = 0x0001,     //AES ECB算法
    AES_CBC = 0x0002,     //AES CBC算法
    AES_CTR = 0x0004,     //AES CTR算法
    SM4_ECB = 0x0100,     //国密4 ECB算法
    SM4_CBC = 0x0200,     //国密4 CBC算法
    SM4_CTR = 0x0400      //国密4 CTR算法
} EncryptAlgorithm;

//设备支持的非对称加密算法类型
typedef enum {
    RSA = 0x01,                   //RSA算法
    SM2 = 0x02                    //国密2算法
} PKI_Algorithm;

//MPR设备事件枚举
typedef enum {
    PLUGED_IN,           //设备插入
    PLUGED_OUT,          //设备设备拔出
    MPR_CODE_CAPTURED,   //点读了一个MPR码
    POWER_KEY_PRESSED,   //POWER键被按下
    UP_KEY_PRESSED,      //音量+键被按下
    DOWN_KEY_PRESSED,    //音量-键被按下
    PAIR_SUCCESS         //无线识读器配对成功
} MPRDeviceEvent;

#endif//__MPRDEVICEENUM_H__
#ifndef __MFLERRORCODE_H__
#define __MFLERRORCODE_H__

// MFL库函数返回的错误代码，兼容MPRDeviceErrorCode
// 0：成功
// -1 至 -1000：标准的错误代码
// -1000以下：各库函数自定义的错误代码
typedef enum {
    MFL_OK             			= 0,  //接口函数调用成功
    MFL_BAD_PARAM               = -1, //传入的接口函数参数错误
    MFL_BAD_CALL                = -2, //接口函数调用顺序错误
    MFL_DEVICE_NOT_EXIST        = -3, //设备不存在
    MFL_COMM_FAIL               = -4, //下位机通讯错误
    MFL_LOW_PRIORITY            = -5, //其他程序正在通讯且优先级高
    MFL_ONE_EXIST               = -6, //只有一个程序存在
    MFL_SYS_ERROR               = -7, //其它错误
    MFL_VERIFY_SIGNATURE_FAIL   = -8, //验证license签名失败
    MFL_NO_THIS_LINE            = -9, //downloadlist中没有该行
    MFL_FILE_IOERROR            = -11,//文件IO错误
    MFL_INVALID_MPR             = -12 //无效的MPR文件 
} MFLErrorCode;

#endif //__MFLERRORCODE_H__
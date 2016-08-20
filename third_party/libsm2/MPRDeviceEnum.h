#ifndef __MPRDEVICEENUM_H__
#define __MPRDEVICEENUM_H__

//MPR�豸����ö��
typedef enum {
    MPRDevice_Invalid,     //��Ч��MPR�豸
    MPRDevice_PenReader,   //MPRʶ����������ʣ�
    MPRDevice_BookDisk,    //MRR���̣�UKey��
    MPRDevice_Dongle       //USB dongle,��ý��洢����
} MPRDeviceType;

//�ӿں������صĴ������
typedef enum {
    DEVICE_OK               = 0,  //�ӿں������óɹ�
    BAD_PARAM               = -1, //����Ľӿں�����������
    BAD_CALL                = -2, //�ӿں�������˳�����
    DEVICE_NOT_EXIST        = -3, //�豸������
    COMM_FAIL               = -4, //��λ��ͨѶ����
    LOW_PRIORITY            = -5, //������������ͨѶ�����ȼ���
    ONE_EXIST               = -6, //ֻ��һ���������
    SYS_ERROR               = -7, //��������
    VERIFY_SIGNATURE_FAIL   = -8, //��֤licenseǩ��ʧ��
    NO_THIS_LINE            = -9, //downloadlist��û�и���
    DECRYPT_FAIL            = -10 //��λ������ʧ��
} MPRDeviceErrorCode;

//ʶ����֧�ֵ���Ƶ��ʽ
typedef enum {
    WAV = 0x00000001L,     //��ѹ��wave��ʽ
    MP3 = 0x00000002L,     //MP3ѹ����ʽ
    OGG = 0x00000004L      //OGGѹ����ʽ
} MPRAudioFormat;

//�豸֧�ֵĶԳƼ����㷨����
typedef enum {
    AES_ECB = 0x0001,     //AES ECB�㷨
    AES_CBC = 0x0002,     //AES CBC�㷨
    AES_CTR = 0x0004,     //AES CTR�㷨
    SM4_ECB = 0x0100,     //����4 ECB�㷨
    SM4_CBC = 0x0200,     //����4 CBC�㷨
    SM4_CTR = 0x0400      //����4 CTR�㷨
} EncryptAlgorithm;

//�豸֧�ֵķǶԳƼ����㷨����
typedef enum {
    RSA = 0x01,                   //RSA�㷨
    SM2 = 0x02                    //����2�㷨
} PKI_Algorithm;

//MPR�豸�¼�ö��
typedef enum {
    PLUGED_IN,           //�豸����
    PLUGED_OUT,          //�豸�豸�γ�
    MPR_CODE_CAPTURED,   //�����һ��MPR��
    POWER_KEY_PRESSED,   //POWER��������
    UP_KEY_PRESSED,      //����+��������
    DOWN_KEY_PRESSED,    //����-��������
    PAIR_SUCCESS         //����ʶ������Գɹ�
} MPRDeviceEvent;

#endif//__MPRDEVICEENUM_H__
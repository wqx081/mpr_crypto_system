#ifndef __MFLERRORCODE_H__
#define __MFLERRORCODE_H__

// MFL�⺯�����صĴ�����룬����MPRDeviceErrorCode
// 0���ɹ�
// -1 �� -1000����׼�Ĵ������
// -1000���£����⺯���Զ���Ĵ������
typedef enum {
    MFL_OK             			= 0,  //�ӿں������óɹ�
    MFL_BAD_PARAM               = -1, //����Ľӿں�����������
    MFL_BAD_CALL                = -2, //�ӿں�������˳�����
    MFL_DEVICE_NOT_EXIST        = -3, //�豸������
    MFL_COMM_FAIL               = -4, //��λ��ͨѶ����
    MFL_LOW_PRIORITY            = -5, //������������ͨѶ�����ȼ���
    MFL_ONE_EXIST               = -6, //ֻ��һ���������
    MFL_SYS_ERROR               = -7, //��������
    MFL_VERIFY_SIGNATURE_FAIL   = -8, //��֤licenseǩ��ʧ��
    MFL_NO_THIS_LINE            = -9, //downloadlist��û�и���
    MFL_FILE_IOERROR            = -11,//�ļ�IO����
    MFL_INVALID_MPR             = -12 //��Ч��MPR�ļ� 
} MFLErrorCode;

#endif //__MFLERRORCODE_H__
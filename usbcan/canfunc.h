#ifndef CANFUNC_H
#define CANFUNC_H

#include <windows.h>

#include <string>
using std::string;

//1.ZLGCAN系列接口卡信息的数据类型。
typedef  struct  _VCI_BOARD_INFO{
        USHORT	hw_Version;
        USHORT	fw_Version;
        USHORT	dr_Version;
        USHORT	in_Version;
        USHORT	irq_Num;
        BYTE	can_Num;
        CHAR	str_Serial_Num[20];
        CHAR	str_hw_Type[40];
        USHORT	Reserved[4];
} VCI_BOARD_INFO,*PVCI_BOARD_INFO;

//2.定义CAN信息帧的数据类型。
typedef  struct  _VCI_CAN_OBJ{
    UINT	ID;
    UINT	TimeStamp;
    BYTE	TimeFlag;
    BYTE	SendType;
    BYTE	RemoteFlag;//是否是远程帧
    BYTE	ExternFlag;//是否是扩展帧
    BYTE	DataLen;
    BYTE	Data[8];
    BYTE	Reserved[3];
}VCI_CAN_OBJ,*PVCI_CAN_OBJ;

//3.定义初始化CAN的数据类型
typedef struct _VCI_INIT_CONFIG{
    DWORD	AccCode;
    DWORD	AccMask;
    DWORD	Reserved;
    UCHAR	Filter;
    UCHAR	Timing0;
    UCHAR	Timing1;
    UCHAR	Mode;
}VCI_INIT_CONFIG,*PVCI_INIT_CONFIG;

// Can消息结构体
typedef struct _CANMSG{
    unsigned long id;
    unsigned int datalen;
    unsigned char data[8];
}CanMsg, *PCanMsg;

class CanFunc
{
public:
    virtual ~CanFunc();
    virtual bool OpenAndInitDevice() = 0;
    virtual bool Transmit(PCanMsg data) = 0;
    virtual unsigned long GetReceiveNum() = 0;
    virtual bool ReceiveData(PCanMsg data) = 0;

    virtual string GetErrorMsg() = 0;
};

#endif // CANFUNC_H

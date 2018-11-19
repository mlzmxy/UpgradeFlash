#ifndef CANFUNC_CHUANGXIN_H
#define CANFUNC_CHUANGXIN_H

#include <string>
using std::string;

#include "canFunc.h"

//1.ZLGCAN系列接口卡信息的数据类型。
typedef  struct  _VCI_BOARD_INFO_CX {
    USHORT	hw_Version;
    USHORT	fw_Version;
    USHORT	dr_Version;
    USHORT	in_Version;
    USHORT	irq_Num;
    BYTE	can_Num;
    CHAR	str_Serial_Num[20];
    CHAR	str_hw_Type[40];
    USHORT	Reserved[4];
} VCI_BOARD_INFO_CX, *PVCI_BOARD_INFO_CX;

//2.定义CAN信息帧的数据类型。
typedef  struct  _VCI_CAN_OBJ_CX {
    UINT	ID;
    UINT	TimeStamp;
    BYTE	TimeFlag;
    BYTE	SendType;
    BYTE	RemoteFlag;//是否是远程帧
    BYTE	ExternFlag;//是否是扩展帧
    BYTE	DataLen;
    BYTE	Data[8];
    BYTE	Reserved[3];
}VCI_CAN_OBJ_CX, *PVCI_CAN_OBJ_CX;

//3.定义初始化CAN的数据类型
typedef struct _VCI_INIT_CONFIG_CX {
    DWORD	AccCode;
    DWORD	AccMask;
    DWORD	Reserved;
    UCHAR	Filter;
    UCHAR	Timing0;
    UCHAR	Timing1;
    UCHAR	Mode;
}VCI_INIT_CONFIG_CX, *PVCI_INIT_CONFIG_CX;


typedef DWORD(__stdcall* LPVCI_OpenDevice)(DWORD, DWORD, DWORD);
typedef DWORD(__stdcall* LPVCI_CloseDevice)(DWORD, DWORD);
typedef DWORD(__stdcall* LPVCI_InitCan)(DWORD, DWORD, DWORD, PVCI_INIT_CONFIG_CX);

typedef ULONG(__stdcall* LPVCI_GetReceiveNum)(DWORD, DWORD, DWORD);
typedef DWORD(__stdcall* LPVCI_ClearBuffer)(DWORD, DWORD, DWORD);

typedef DWORD(__stdcall* LPVCI_StartCAN)(DWORD, DWORD, DWORD);
typedef DWORD(__stdcall* LPVCI_ResetCAN)(DWORD, DWORD, DWORD);

typedef ULONG(__stdcall* LPVCI_Transmit)(DWORD, DWORD, DWORD, PVCI_CAN_OBJ_CX, ULONG);
typedef ULONG(__stdcall* LPVCI_Receive)(DWORD, DWORD, DWORD, PVCI_CAN_OBJ_CX, ULONG, INT);


class CanFunc_ChuangXin : public CanFunc
{
public:
	CanFunc_ChuangXin();
	~CanFunc_ChuangXin();

	virtual bool OpenAndInitDevice();
	virtual bool Transmit(PCanMsg data);
	virtual unsigned long GetReceiveNum();
	virtual bool ReceiveData(PCanMsg data);
	virtual string GetErrorMsg();

private:
    VCI_INIT_CONFIG_CX init_config;  //初始化参数
    VCI_CAN_OBJ_CX m_candata_struct[500];  //CAN数据结构
	HMODULE hMod;

	LPVCI_OpenDevice VCI_OpenDevice;
	LPVCI_CloseDevice VCI_CloseDevice;
	LPVCI_ResetCAN VCI_ResetCan;
	LPVCI_InitCan VCI_InitCan;
	LPVCI_StartCAN VCI_StartCAN;
	LPVCI_Transmit VCI_Transmit;
	LPVCI_Receive VCI_Receive;
    LPVCI_GetReceiveNum VCI_GetReceiveNum;
	LPVCI_ClearBuffer VCI_ClearBuffer;

	unsigned int m_error_code;
};

#endif // CANFUNC_CHUANGXIN_H


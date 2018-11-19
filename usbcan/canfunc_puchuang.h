#ifndef CANFUNC_PUCHUANG_H
#define CANFUNC_PUCHUANG_H

#include <string>
using std::string;

#include "canFunc.h"

//1.定义初始化CAN的数据类型
typedef struct _VCI_INIT_CONFIG_PC
{
    DWORD	AccCode;	//对应SJA1000中的四个寄存器ACR0，ACR1，ACR2，ACR3
    DWORD	AccMask;	//对应SJA1000中的四个寄存器AMR0，AMR1，AMR2，AMR3
    DWORD   Reserved;
    UCHAR   Filter;
    UCHAR	Timing0;	//定时器0
    UCHAR	Timing1;	//定时器1
    UCHAR   Mode;       //模式
}VCI_INIT_CONFIG_PC,*P_VCI_INIT_CONFIG_PC;


//2.定义CAN信息帧的数据类型
typedef struct _VCI_CAN_OBJ_PC
{
    DWORD	ID;
    UCHAR   SendType;       //0：正常发送，1：自发自收
    UCHAR	ExternFlag;	    //0：标准帧，  1：扩展帧
    UCHAR	RemoteFlag;	    //0：数据帧，  1：远程帧
    UCHAR	DataLen;
    UCHAR	Data[8];
}VCI_CAN_OBJ_PC,*P_VCI_CAN_OBJ_PC;


// CAN_TO_USB
typedef bool(__stdcall* Func_PuChuang_1)(DWORD);
typedef bool(__stdcall* Func_PuChuang_2)(DWORD, DWORD);
typedef bool(__stdcall* Func_PuChuang_3)(DWORD, DWORD, P_VCI_INIT_CONFIG_PC);
typedef bool(__stdcall* Func_PuChuang_4)(DWORD, DWORD, P_VCI_CAN_OBJ_PC);
typedef bool(__stdcall* Func_PuChuang_5)(DWORD, PCHAR);
typedef DWORD(__stdcall* Func_PuChuang_6)(DWORD, DWORD, P_VCI_CAN_OBJ_PC, DWORD, INT);
typedef DWORD(__stdcall* Func_PuChuang_7)(DWORD, DWORD);

class CanFunc_PuChuang : public CanFunc
{
public:
	CanFunc_PuChuang();
	~CanFunc_PuChuang();

	virtual bool OpenAndInitDevice();
	virtual bool Transmit(PCanMsg data);
	virtual unsigned long GetReceiveNum();
	virtual bool ReceiveData(PCanMsg data);
	virtual string GetErrorMsg();

private:
    VCI_INIT_CONFIG_PC init_config;  //初始化参数
    VCI_CAN_OBJ_PC m_candata_struct[500];  //CAN数据结构
	HMODULE hMod;

	Func_PuChuang_1 _OpenDevice;
	Func_PuChuang_1 _CloseDevice;
	Func_PuChuang_2 _ResetCan;
	Func_PuChuang_3 _InitCan;
	Func_PuChuang_4 _Transmit;
	Func_PuChuang_5 _ReadDevSn;
	Func_PuChuang_6 _Receive;
	Func_PuChuang_7 _GetReceiveNum;
	Func_PuChuang_2 _ClearBuffer;

	int m_error_code;
};

#endif // CANFUNC_PUCHUANG_H


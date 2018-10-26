#ifndef CANFUNC_CHUANGXIN_H
#define CANFUNC_CHUANGXIN_H

#include <string>
using std::string;

#include "canFunc.h"

typedef DWORD(__stdcall* LPVCI_OpenDevice)(DWORD, DWORD, DWORD);
typedef DWORD(__stdcall* LPVCI_CloseDevice)(DWORD, DWORD);
typedef DWORD(__stdcall* LPVCI_InitCan)(DWORD, DWORD, DWORD, PVCI_INIT_CONFIG);

typedef ULONG(__stdcall* LPVCI_GetReceiveNum)(DWORD, DWORD, DWORD);
typedef DWORD(__stdcall* LPVCI_ClearBuffer)(DWORD, DWORD, DWORD);

typedef DWORD(__stdcall* LPVCI_StartCAN)(DWORD, DWORD, DWORD);
typedef DWORD(__stdcall* LPVCI_ResetCAN)(DWORD, DWORD, DWORD);

typedef ULONG(__stdcall* LPVCI_Transmit)(DWORD, DWORD, DWORD, PVCI_CAN_OBJ, ULONG);
typedef ULONG(__stdcall* LPVCI_Receive)(DWORD, DWORD, DWORD, PVCI_CAN_OBJ, ULONG, INT);


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
	VCI_INIT_CONFIG init_config;  //初始化参数
	VCI_CAN_OBJ m_candata_struct;  //CAN数据结构
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


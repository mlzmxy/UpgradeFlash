#ifndef CANFUNC_CHUANGXIN_H
#define CANFUNC_CHUANGXIN_H

#include <string>
using std::string;

#include "canfunc.h"
/*
typedef DWORD (*Func_ChuangXin_1)(DWORD, DWORD, DWORD);
typedef DWORD (*Func_ChuangXin_2)(DWORD, DWORD);
typedef ULONG (*Func_ChuangXin_3)(DWORD, DWORD, DWORD);
typedef DWORD (*Func_ChuangXin_4)(DWORD, DWORD, DWORD, PVCI_INIT_CONFIG);
typedef ULONG (*Func_ChuangXin_5)(DWORD, DWORD, DWORD, PVCI_CAN_OBJ, ULONG);
typedef ULONG (*Func_ChuangXin_6)(DWORD, DWORD, DWORD, PVCI_CAN_OBJ, ULONG, INT);
*/

typedef DWORD (*LPVCI_OpenDevice)(DWORD,DWORD,DWORD);
typedef DWORD (*LPVCI_CloseDevice)(DWORD,DWORD);
typedef DWORD (*LPVCI_InitCan)(DWORD,DWORD,DWORD,PVCI_INIT_CONFIG);

typedef ULONG (*LPVCI_GetReceiveNum)(DWORD,DWORD,DWORD);
typedef DWORD (*LPVCI_ClearBuffer)(DWORD,DWORD,DWORD);

typedef DWORD (*LPVCI_StartCAN)(DWORD,DWORD,DWORD);
typedef DWORD (*LPVCI_ResetCAN)(DWORD,DWORD,DWORD);

typedef ULONG (*LPVCI_Transmit)(DWORD,DWORD,DWORD,PVCI_CAN_OBJ,ULONG);
typedef ULONG (*LPVCI_Receive)(DWORD,DWORD,DWORD,PVCI_CAN_OBJ,ULONG,INT);


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

#ifndef CANFUNC_PUCHUANG_H
#define CANFUNC_PUCHUANG_H

#include <string>
using std::string;

#include "./usbcan/canfunc.h"

// CAN_TO_USB
typedef bool  (*Func_PuChuang_1)(DWORD);
typedef bool  (*Func_PuChuang_2)(DWORD, DWORD);
typedef bool  (*Func_PuChuang_3)(DWORD, DWORD, PVCI_INIT_CONFIG);
typedef bool  (*Func_PuChuang_4)(DWORD, DWORD, PVCI_CAN_OBJ);
typedef bool  (*Func_PuChuang_5)(DWORD, PCHAR);
typedef DWORD (*Func_PuChuang_6)(DWORD, DWORD, PVCI_CAN_OBJ, DWORD, INT);
typedef DWORD (*Func_PuChuang_7)(DWORD, DWORD);

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
    VCI_INIT_CONFIG init_config;  //初始化参数
    VCI_CAN_OBJ m_candata_struct;  //CAN数据结构
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

#endif // CANFUNC_H

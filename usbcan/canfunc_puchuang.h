#ifndef CANFUNC_PUCHUANG_H
#define CANFUNC_PUCHUANG_H

#include <string>
using std::string;

#include "canfunc.h"
#include "puchuangdianzi/CAN_TO_USB.h"

// CAN_TO_USB
typedef bool  (*Func1)(DWORD);
typedef bool  (*Func2)(DWORD, DWORD);
typedef bool  (*Func3)(DWORD, DWORD, P_VCI_INIT_CONFIG);
typedef bool  (*Func4)(DWORD, DWORD, P_VCI_CAN_OBJ);
typedef bool  (*Func5)(DWORD, PCHAR);
typedef DWORD (*Func6)(DWORD, DWORD, P_VCI_CAN_OBJ, DWORD, INT);
typedef DWORD (*Func7)(DWORD, DWORD);

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

    Func1 _OpenDevice;
    Func1 _CloseDevice;
    Func2 _ResetCan;
    Func3 _InitCan;
    Func4 _Transmit;
    Func5 _ReadDevSn;
    Func6 _Receive;
    Func7 _GetReceiveNum;
    Func2 _ClearBuffer;

    int m_error_code;
};

// CAN
#define Dev_Index 0  //设备索引号
#define Can_Index 0  //第0路CAN
#define Can_Index_1 1  //第1路CAN

#define ERROR_LOAD_DLL 0x1        //DLL加载失败
#define ERROR_LINK_DLL_FUNCS 0x2  //DLL函数链接失败
#define ERROR_OPENDEVICE 0x3      //打开设备失败
#define ERROR_InitCan 0x4         //CAN初始化失败
#define ERROR_SENDDATA  0x6       //发送失败

#endif // CANFUNC_H

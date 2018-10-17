#ifndef CANFUNC_H
#define CANFUNC_H

#include <string>
using std::string;

#include "CAN_TO_USB.h"

// CAN_TO_USB
typedef bool  (*Func1)(DWORD);
typedef bool  (*Func2)(DWORD, DWORD);
typedef bool  (*Func3)(DWORD, DWORD, P_VCI_INIT_CONFIG);
typedef bool  (*Func4)(DWORD, DWORD, P_VCI_CAN_OBJ);
typedef bool  (*Func5)(DWORD, PCHAR);
typedef DWORD (*Func6)(DWORD, DWORD, P_VCI_CAN_OBJ, DWORD, INT);
typedef DWORD (*Func7)(DWORD, DWORD);

class CanFunc
{
public:
    CanFunc();
    ~CanFunc();

    void SetInitConfig();
    bool OpenAndInitDevice();

    bool OpenDevice(DWORD DevIndex);
    bool CloseDevice(DWORD DevIndex);
    bool ResetCan(DWORD DevIndex,DWORD CANIndex);
    bool InitCan(DWORD DevIndex,DWORD CANIndex,P_VCI_INIT_CONFIG pInitConfig);
    bool Transmit(DWORD DevIndex,DWORD CANIndex,P_VCI_CAN_OBJ pSend);
    bool ReadDevSn(DWORD DevIndex, PCHAR DevSn); //获得序列号
    DWORD Receive(DWORD DevIndex,DWORD CANIndex,P_VCI_CAN_OBJ pReceive,DWORD Len,INT WaitTime);
    DWORD GetReceiveNum(DWORD DevIndex,DWORD CANIndex);  //获取缓冲区尚未读取的帧数
    bool ClearBuffer(DWORD DevIndex,DWORD CANIndex);

    string GetErrorMsg();

private:
    int error_code;
    VCI_INIT_CONFIG init_config;  //初始化参数

    Func1 _OpenDevice;
    Func1 _CloseDevice;
    Func2 _ResetCan;
    Func3 _InitCan;
    Func4 _Transmit;
    Func5 _ReadDevSn;
    Func6 _Receive;
    Func7 _GetReceiveNum;
    Func2 _ClearBuffer;
};

// CAN
#define Dev_Index 0  //设备索引号
#define Can_Index_0 0  //第0路CAN
#define Can_Index_1 1  //第1路CAN

#define ERROR_LOAD_DLL 0x1        //DLL加载失败
#define ERROR_LINK_DLL_FUNCS 0x2  //DLL函数链接失败
#define ERROR_OPENDEVICE 0x3      //打开设备失败
#define ERROR_InitCan 0x4         //CAN初始化失败

#endif // CANFUNC_H

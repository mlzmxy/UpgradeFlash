#ifndef CANFUNC_CHUANGXIN_H
#define CANFUNC_CHUANGXIN_H

#include <string>
using std::string;

#include <QLibrary>

#include "canfunc.h"

typedef DWORD (*Func_ChuangXin_1)(DWORD, DWORD, DWORD);
typedef DWORD (*Func_ChuangXin_2)(DWORD, DWORD);
typedef ULONG (*Func_ChuangXin_3)(DWORD, DWORD, DWORD);
typedef DWORD (*Func_ChuangXin_4)(DWORD, DWORD, DWORD, PVCI_INIT_CONFIG);
typedef ULONG (*Func_ChuangXin_5)(DWORD, DWORD, DWORD, PVCI_CAN_OBJ, ULONG);
typedef ULONG (*Func_ChuangXin_6)(DWORD, DWORD, DWORD, PVCI_CAN_OBJ, ULONG, INT);

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

    bool IsSucceed();

private:
    VCI_INIT_CONFIG init_config;  //初始化参数
    VCI_CAN_OBJ m_candata_struct;  //CAN数据结构
    QLibrary *can_dll;

    Func_ChuangXin_1 _OpenDevice;
    Func_ChuangXin_2 _CloseDevice;
    Func_ChuangXin_1 _ResetCan;
    Func_ChuangXin_4 _InitCan;
    Func_ChuangXin_1 _StartCAN;
    Func_ChuangXin_5 _Transmit;
    Func_ChuangXin_6 _Receive;
    Func_ChuangXin_3 _GetReceiveNum;
    Func_ChuangXin_1 _ClearBuffer;

    unsigned int m_error_code;
};

// CAN
#define DeviceType 4  //设备类型 CANalyst-II
#define DeviceInd 0   //设备索引号
#define CANInd 0      //第0路CAN
#define Reserved 0    //保留

#define ERROR_LOAD_DLL 0x1        //DLL加载失败
#define ERROR_LINK_DLL_FUNCS 0x2  //DLL函数链接失败
#define ERROR_OPENDEVICE 0x3      //打开设备失败
#define ERROR_InitCan 0x4         //CAN初始化失败
#define ERROR_StartCan 0x5        //CAN启动失败
#define ERROR_SENDDATA  0x6       //发送失败

#endif // CANFUNC_CHUANGXIN_H

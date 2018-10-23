#ifndef CANFUNC_CHUANGXIN_H
#define CANFUNC_CHUANGXIN_H

#include <string>
using std::string;

#include "canfunc.h"
#include "usbcan/chuangxinkeji/ControlCAN.h"

typedef DWORD (*Func1)(DWORD, DWORD, DWORD);
typedef DWORD (*Func2)(DWORD, DWORD);
typedef ULONG (*Func3)(DWORD, DWORD, DWORD);
typedef DWORD (*Func4)(DWORD, DWORD, DWORD, PVCI_INIT_CONFIG);
typedef ULONG (*Func5)(DWORD, DWORD, DWORD, PVCI_CAN_OBJ, ULONG);
typedef ULONG (*Func6)(DWORD, DWORD, DWORD, PVCI_CAN_OBJ, ULONG, INT);

class CanFunc_ChuangXin : public CanFunc
{
public:
    typedef enum _TIMING {
        TIMING_1000K=0,
        TIMING_800K,
        TIMING_500K,
        TIMING_250K,
        TIMING_125K,
        TIMING_100K,
        TIMING_50K,
        TIMING_20K,
        TIMING_10K
    }TIMING;
public:
    CanFunc_ChuangXin();
    virtual bool OpenAndInitDevice();
    virtual bool Transmit(PCanMsg data);
    virtual unsigned long GetReceiveNum();
    virtual bool ReceiveData(PCanMsg data);
    virtual string GetErrorMsg();

    void SetCanConfig(unsigned long dev_ind, unsigned long can_ind, TIMING timing_ind);

private:
    VCI_INIT_CONFIG init_config;  //初始化参数
    VCI_CAN_OBJ m_candata_struct;  //CAN数据结构

    unsigned long m_device_type;  //设备类型
    unsigned long m_device_ind;   //设备索引号
    unsigned long m_can_ind;      //can通道

    Func1 _OpenDevice;
    Func2 _CloseDevice;
    Func1 _ResetCan;
    Func4 _InitCan;
    Func1 _StartCAN;
    Func5 _Transmit;
    Func6 _Receive;
    Func3 _GetReceiveNum;
    Func1 _ClearBuffer;

    int m_error_code;
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

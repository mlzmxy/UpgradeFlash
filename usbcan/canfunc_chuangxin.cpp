
#include "canfunc_chuangxin.h"

#include <QDebug>

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


CanFunc_ChuangXin::CanFunc_ChuangXin() :
    VCI_OpenDevice(nullptr),
    VCI_CloseDevice(nullptr),
    VCI_ResetCan(nullptr),
    VCI_InitCan(nullptr),
    VCI_StartCAN(nullptr),
    VCI_Transmit(nullptr),
    VCI_Receive(nullptr),
    VCI_GetReceiveNum(nullptr),
    VCI_ClearBuffer(nullptr),
    m_error_code(0)
{
    //加载动态链接库ControlCAN.dll
    hMod = LoadLibraryA("ControlCAN.dll");
    if (hMod) {
        VCI_OpenDevice = (LPVCI_OpenDevice)GetProcAddress(hMod, "VCI_OpenDevice");
        VCI_CloseDevice = (LPVCI_CloseDevice)GetProcAddress(hMod, "VCI_CloseDevice");
        VCI_ResetCan = (LPVCI_ResetCAN)GetProcAddress(hMod, "VCI_ResetCAN");
        VCI_InitCan = (LPVCI_InitCan)GetProcAddress(hMod, "VCI_InitCAN");
        VCI_StartCAN = (LPVCI_StartCAN)GetProcAddress(hMod, "VCI_StartCAN");
        VCI_Transmit = (LPVCI_Transmit)GetProcAddress(hMod, "VCI_Transmit");
        VCI_Receive = (LPVCI_Receive)GetProcAddress(hMod, "VCI_Receive");
        VCI_GetReceiveNum = (LPVCI_GetReceiveNum)GetProcAddress(hMod, "VCI_GetReceiveNum");
        VCI_ClearBuffer = (LPVCI_ClearBuffer)GetProcAddress(hMod, "VCI_ClearBuffer");
    } else {
        m_error_code = ERROR_LOAD_DLL;
    }
}

CanFunc_ChuangXin::~CanFunc_ChuangXin()
{
    if(VCI_CloseDevice) {
        VCI_CloseDevice(DeviceType, DeviceInd);
    }
}

bool CanFunc_ChuangXin::OpenAndInitDevice()
{
    //can初始化设置
    //VCI_INIT_CONFIG init_config;  //初始化参数
    init_config.AccCode = 0x00000000;
    init_config.AccMask = 0xFFFFFFFF;
    init_config.Filter = 0;
    init_config.Timing0 = 0x01;  //250Kbps
    init_config.Timing1 = 0x1C;
    init_config.Mode = 0;
    if(m_error_code == 0){
        if(1 == VCI_OpenDevice(DeviceType, DeviceInd, Reserved)) {
            if(1 == VCI_InitCan(DeviceType, DeviceInd, CANInd, &init_config)) {
                if(1 == VCI_StartCAN(DeviceType, DeviceInd, CANInd)) {
                    return true;
                } else {
                    m_error_code = ERROR_StartCan;
                }
            } else {
                m_error_code = ERROR_InitCan;
            }
        } else {
            m_error_code = ERROR_OPENDEVICE;
        }
    }
    return false;
}

bool CanFunc_ChuangXin::Transmit(PCanMsg data)
{
    //VCI_CAN_OBJ m_candata_struct;  //CAN数据结构
    m_candata_struct.ID = data->id;
    m_candata_struct.SendType = 1;  //单次发送
    m_candata_struct.RemoteFlag = 0;
    m_candata_struct.ExternFlag = 1;
    m_candata_struct.DataLen = 8;
    m_candata_struct.Data[0] = data->data[0];
    m_candata_struct.Data[1] = data->data[1];
    m_candata_struct.Data[2] = data->data[2];
    m_candata_struct.Data[3] = data->data[3];
    m_candata_struct.Data[4] = data->data[4];
    m_candata_struct.Data[5] = data->data[5];
    m_candata_struct.Data[6] = data->data[6];
    m_candata_struct.Data[7] = data->data[7];

    if(VCI_Transmit) {
        if(1 == VCI_Transmit(DeviceType, DeviceInd, CANInd, &m_candata_struct, 1)) {
            return true;
        } else {
            m_error_code = ERROR_SENDDATA;
        }
    } else {
        m_error_code = ERROR_LOAD_DLL;
    }
    return false;
}

unsigned long CanFunc_ChuangXin::GetReceiveNum()
{
    return VCI_GetReceiveNum(DeviceType, DeviceInd, CANInd);
}

bool CanFunc_ChuangXin::ReceiveData(PCanMsg data)
{
    if(VCI_Receive) {
        if(1 == VCI_Receive(DeviceType, DeviceInd, CANInd, &m_candata_struct, 1, 5)) {
            data->id = m_candata_struct.ID;
            data->datalen = m_candata_struct.DataLen;
            data->data[0] = m_candata_struct.Data[0];
            data->data[1] = m_candata_struct.Data[1];
            data->data[2] = m_candata_struct.Data[2];
            data->data[3] = m_candata_struct.Data[3];
            data->data[4] = m_candata_struct.Data[4];
            data->data[5] = m_candata_struct.Data[5];
            data->data[6] = m_candata_struct.Data[6];
            data->data[7] = m_candata_struct.Data[7];
            return true;
        }
    } else {
        m_error_code = ERROR_LOAD_DLL;
    }
    return false;
}

std::string CanFunc_ChuangXin::GetErrorMsg()
{
    string msg = "";
    switch(m_error_code)
    {
    case ERROR_LOAD_DLL:
        msg = "ControlCAN.dll 加载失败!";
        break;
    case ERROR_LINK_DLL_FUNCS:
        msg = "DLL函数链接失败";
        break;
    case ERROR_OPENDEVICE:
        msg = "打开CAN设备失败";
        break;
    case ERROR_InitCan:
        msg = "CAN初始化失败";
        break;
    case ERROR_StartCan:
        msg = "CAN启动失败";
        break;
    case ERROR_SENDDATA:
        msg = "CAN发送失败";
        break;
    default:
        break;
    }
    return msg;
}


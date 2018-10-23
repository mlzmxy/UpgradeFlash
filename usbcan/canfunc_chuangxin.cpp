#include "canfunc_chuangxin.h"

#include <QDebug>

CanFunc_ChuangXin::CanFunc_ChuangXin() :
    can_dll(new QLibrary("ControlCAN.dll")),
    _OpenDevice(nullptr),
    _CloseDevice(nullptr),
    _ResetCan(nullptr),
    _InitCan(nullptr),
    _StartCAN(nullptr),
    _Transmit(nullptr),
    _Receive(nullptr),
    _GetReceiveNum(nullptr),
    _ClearBuffer(nullptr),
    m_error_code(0)
{
    //can初始化设置
    init_config.AccCode = 0x00000000;
    init_config.AccMask = 0xFFFFFFFF;
    init_config.Filter = 0;
    init_config.Timing0 = 0x01;  //250Kbps
    init_config.Timing1 = 0x1C;
    init_config.Mode = 0;
/*
    if(can_dll->load()) {
        _OpenDevice = (Func_ChuangXin_1)can_dll->resolve("VCI_OpenDevice");
        _CloseDevice = (Func_ChuangXin_2)can_dll->resolve("VCI_CloseDevice");
        //_ResetCan = (Func_ChuangXin_1)can_dll->resolve("VCI_ResetCAN");
        _InitCan = (Func_ChuangXin_4)can_dll->resolve("VCI_InitCAN");
        _StartCAN = (Func_ChuangXin_1)can_dll->resolve("VCI_StartCAN");
        _Transmit = (Func_ChuangXin_5)can_dll->resolve("VCI_Transmit");
        _Receive = (Func_ChuangXin_6)can_dll->resolve("VCI_Receive");
        //_GetReceiveNum = (Func_ChuangXin_3)can_dll->resolve("VCI_GetReceiveNum");
        //_ClearBuffer = (Func_ChuangXin_1)can_dll->resolve("VCI_ClearBuffer");

        DWORD rel =  _OpenDevice(DeviceType, DeviceInd, Reserved);
        if(rel == 1) {
            rel = _InitCan(DeviceType, DeviceInd, CANInd, &init_config);
            if(rel == 1) {
                rel = _StartCAN(DeviceType, DeviceInd, CANInd);
                if(rel == 1) {
                } else {
                    m_error_code = ERROR_StartCan;
                }
            } else {
                m_error_code = ERROR_InitCan;
            }
        } else {
            m_error_code = ERROR_OPENDEVICE;
        }
    } else {
        m_error_code = ERROR_LOAD_DLL;
    }
    */
}

CanFunc_ChuangXin::~CanFunc_ChuangXin()
{

}

bool CanFunc_ChuangXin::OpenAndInitDevice()
{
    //加载动态链接库ControlCAN.dll


    //HMODULE hMod = LoadLibraryA("ControlCAN.dll");
    if(can_dll->load()) {
        _OpenDevice = (Func_ChuangXin_1)can_dll->resolve("VCI_OpenDevice");
        _CloseDevice = (Func_ChuangXin_2)can_dll->resolve("VCI_CloseDevice");
        _ResetCan = (Func_ChuangXin_1)can_dll->resolve("VCI_ResetCAN");
        _InitCan = (Func_ChuangXin_4)can_dll->resolve("VCI_InitCAN");
        _StartCAN = (Func_ChuangXin_1)can_dll->resolve("VCI_StartCAN");
        _Transmit = (Func_ChuangXin_5)can_dll->resolve("VCI_Transmit");
        _Receive = (Func_ChuangXin_6)can_dll->resolve("VCI_Receive");
        _GetReceiveNum = (Func_ChuangXin_3)can_dll->resolve("VCI_GetReceiveNum");
        _ClearBuffer = (Func_ChuangXin_1)can_dll->resolve("VCI_ClearBuffer");
//    if (hMod) {
//        _OpenDevice = (Func_ChuangXin_1)GetProcAddress(hMod, "VCI_OpenDevice");
//        _CloseDevice = (Func_ChuangXin_2)GetProcAddress(hMod, "VCI_CloseDevice");
//        _ResetCan = (Func_ChuangXin_1)GetProcAddress(hMod, "VCI_ResetCAN");
//        _InitCan = (Func_ChuangXin_4)GetProcAddress(hMod, "VCI_InitCAN");
//        _StartCAN = (Func_ChuangXin_1)GetProcAddress(hMod, "VCI_StartCAN");
//        _Transmit = (Func_ChuangXin_5)GetProcAddress(hMod, "VCI_Transmit");
//        _Receive = (Func_ChuangXin_6)GetProcAddress(hMod, "VCI_Receive");
//        _GetReceiveNum = (Func_ChuangXin_3)GetProcAddress(hMod, "VCI_GetReceiveNum");
//        _ClearBuffer = (Func_ChuangXin_1)GetProcAddress(hMod, "VCI_ClearBuffer");

        DWORD rel =  _OpenDevice(DeviceType, DeviceInd, Reserved);
        if(rel == 1) {
            rel = _InitCan(DeviceType, DeviceInd, CANInd, &init_config);
            if(rel == 1) {
                rel = _StartCAN(DeviceType, DeviceInd, CANInd);
                if(rel == 1) {
                    //qDebug() << 1;
                    return true;
                }
            } else {
                m_error_code = ERROR_InitCan;
                return false;
            }
        } else {
            m_error_code = ERROR_OPENDEVICE;
            return false;
        }
    } else {
        m_error_code = ERROR_LOAD_DLL;
        return false;
    }
    m_error_code = ERROR_StartCan;
    return false;

//    if(m_error_code == 0) {
//        return true;
//    } else {
//        return false;
//    }
}

bool CanFunc_ChuangXin::Transmit(PCanMsg data)
{
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

    if(_Transmit(DeviceType, DeviceInd, CANInd, &m_candata_struct, 1)) {
        return true;
    } else {
        m_error_code = ERROR_SENDDATA;
        return false;
    }
}

unsigned long CanFunc_ChuangXin::GetReceiveNum()
{
    return _GetReceiveNum(DeviceType, DeviceInd, CANInd);
}

bool CanFunc_ChuangXin::ReceiveData(PCanMsg data)
{
    if(_Receive) {
        if(1 == _Receive(DeviceType, DeviceInd, CANInd, &m_candata_struct, 1, 5)) {
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
    }
    //qDebug() << "off";
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

#include "canfunc_chuangxin.h"

CanFunc_ChuangXin::CanFunc_ChuangXin() :
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
}

bool CanFunc_ChuangXin::OpenAndInitDevice()
{
    //加载动态链接库ControlCAN.dll
    HMODULE hMod = LoadLibraryA("ControlCAN.dll");
    if (hMod) {
        _OpenDevice = (Func1)GetProcAddress(hMod, "VCI_OpenDevice");
        _CloseDevice = (Func2)GetProcAddress(hMod, "VCI_CloseDevice");
        _ResetCan = (Func1)GetProcAddress(hMod, "VCI_ResetCAN");
        _InitCan = (Func4)GetProcAddress(hMod, "VCI_InitCAN");
        _StartCAN = (Func1)GetProcAddress(hMod, "VCI_StartCAN");
        _Transmit = (Func5)GetProcAddress(hMod, "VCI_Transmit");
        _Receive = (Func6)GetProcAddress(hMod, "VCI_Receive");
        _GetReceiveNum = (Func3)GetProcAddress(hMod, "VCI_GetReceiveNum");
        _ClearBuffer = (Func1)GetProcAddress(hMod, "VCI_ClearBuffer");

        if(_OpenDevice && _CloseDevice && _ResetCan && _InitCan && _Transmit
           && _Receive && _GetReceiveNum && _ClearBuffer) {
            if(_OpenDevice(DeviceType, DeviceInd, Reserved) == 1) {
                if(_InitCan(DeviceType, DeviceInd, CANInd, &init_config) == 1) {
                    if(_StartCAN(DeviceType, DeviceInd, CANInd) != 1) {
                        m_error_code = ERROR_StartCan;
                        return false;
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
            m_error_code = ERROR_LINK_DLL_FUNCS;
            return false;
        }

    } else {
        m_error_code = ERROR_LOAD_DLL;
        return false;
    }
    return true;
}

bool CanFunc_ChuangXin::Transmit(PCanMsg data)
{
    m_candata_struct.ID = data->id;
    m_candata_struct.SendType = 0;
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

void CanFunc_ChuangXin::ReceiveData(PCanMsg data)
{
    if(_Receive(DeviceType, DeviceInd, CANInd, &m_candata_struct, 1, 5)) {
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
    }
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

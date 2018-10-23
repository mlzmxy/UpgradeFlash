#include "canfunc_chuangxin.h"

CanFunc_ChuangXin::CanFunc_ChuangXin() :
    m_device_type(4),
    m_device_ind(0),
    m_can_ind(0),
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
    //can初始化默认设置
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
            if(_OpenDevice(m_device_type, m_device_ind, 0) == 1) {
                if(_InitCan(m_device_type, m_device_ind, m_can_ind, &init_config) == 1) {
                    if(_StartCAN(m_device_type, m_device_ind, m_can_ind) != 1) {
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

    if(_Transmit(m_device_type, m_device_ind, m_can_ind, &m_candata_struct, 1)) {
        return true;
    } else {
        m_error_code = ERROR_SENDDATA;
        return false;
    }
}

unsigned long CanFunc_ChuangXin::GetReceiveNum()
{
    return _GetReceiveNum(m_device_type, m_device_ind, m_can_ind);
}

bool CanFunc_ChuangXin::ReceiveData(PCanMsg data)
{
    if(_Receive(m_device_type, m_device_ind, m_can_ind, &m_candata_struct, 1, 5)) {
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

void CanFunc_ChuangXin::SetCanConfig(unsigned long dev_ind, unsigned long can_ind, TIMING timing_ind)
{
    m_device_ind = dev_ind;
    m_can_ind = can_ind;
    switch(timing_ind) {
    case TIMING_1000K:
        init_config.Timing0 = 0x00;
        init_config.Timing1 = 0x14;
        break;
    case TIMING_800K:
        init_config.Timing0 = 0x00;
        init_config.Timing1 = 0x16;
        break;
    case TIMING_500K:
        init_config.Timing0 = 0x00;
        init_config.Timing1 = 0x1C;
        break;
    case TIMING_250K:
        init_config.Timing0 = 0x01;
        init_config.Timing1 = 0x1C;
        break;
    case TIMING_125K:
        init_config.Timing0 = 0x03;
        init_config.Timing1 = 0x1C;
        break;
    case TIMING_100K:
        init_config.Timing0 = 0x04;
        init_config.Timing1 = 0x1C;
        break;
    case TIMING_50K:
        init_config.Timing0 = 0x09;
        init_config.Timing1 = 0x1C;
        break;
    case TIMING_20K:
        init_config.Timing0 = 0x18;
        init_config.Timing1 = 0x1C;
        break;
    case TIMING_10K:
        init_config.Timing0 = 0x31;
        init_config.Timing1 = 0x1C;
        break;
    default:
        init_config.Timing0 = 0x01;
        init_config.Timing1 = 0x1C;
        break;
    }
}

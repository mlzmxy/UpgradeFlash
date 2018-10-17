#include "CAN_TO_USB.h"
#include "canfunc.h"

CanFunc::CanFunc()
    :error_code(0),
     _OpenDevice(nullptr),
     _CloseDevice(nullptr),
     _ResetCan(nullptr),
     _InitCan(nullptr),
     _Transmit(nullptr),
     _ReadDevSn(nullptr),
     _Receive(nullptr),
     _GetReceiveNum(nullptr),
     _ClearBuffer(nullptr)
{

}

CanFunc::~CanFunc()
{
}

bool CanFunc::OpenAndInitDevice()
{
    //can初始化设置
    init_config.AccCode = 0x00000000;
    init_config.AccMask = 0xFFFFFFFF;
    init_config.Filter = 0;
    init_config.Timing0 = 0x01;  //250Kbps
    init_config.Timing1 = 0x1C;
    init_config.Mode = 0;

    //加载动态链接库CAN_TO_USB.dll
    HMODULE hMod = LoadLibraryA("CAN_TO_USB.dll");
    if (hMod) {
        _OpenDevice = (Func1)GetProcAddress(hMod, "VCI_OpenDevice");
        _CloseDevice = (Func1)GetProcAddress(hMod, "VCI_CloseDevice");
        _ResetCan = (Func2)GetProcAddress(hMod, "VCI_ResetCan");
        _InitCan = (Func3)GetProcAddress(hMod, "VCI_InitCan");
        _Transmit = (Func4)GetProcAddress(hMod, "VCI_Transmit");
        _ReadDevSn = (Func5)GetProcAddress(hMod, "VCI_ReadDevSn");
        _Receive = (Func6)GetProcAddress(hMod, "VCI_Receive");
        _GetReceiveNum = (Func7)GetProcAddress(hMod, "VCI_GetReceiveNum");
        _ClearBuffer = (Func2)GetProcAddress(hMod, "VCI_ClearBuffer");

        if(_OpenDevice && _CloseDevice && _ResetCan && _InitCan && _Transmit
                && _ReadDevSn && _Receive && _GetReceiveNum && _ClearBuffer) {
            if(_OpenDevice(Dev_Index)) {
                if(!_InitCan(Dev_Index, Can_Index_1, &init_config)) {
                    error_code = ERROR_InitCan;
                    return false;
                }
            } else {
                error_code = ERROR_OPENDEVICE;
                return false;
            }
        } else {
            error_code = ERROR_LINK_DLL_FUNCS;
            return false;
        }

    } else {
        error_code = ERROR_LOAD_DLL;
        return false;
    }
    return true;
}

bool CanFunc::OpenDevice(DWORD DevIndex)
{
    return _OpenDevice(DevIndex);
}

bool CanFunc::CloseDevice(DWORD DevIndex)
{
    return _CloseDevice(DevIndex);
}

bool CanFunc::ResetCan(DWORD DevIndex, DWORD CANIndex)
{
    return _ResetCan(DevIndex, CANIndex);
}

bool CanFunc::InitCan(DWORD DevIndex, DWORD CANIndex, P_VCI_INIT_CONFIG pInitConfig)
{
    return _InitCan(DevIndex, CANIndex, pInitConfig);
}

bool CanFunc::Transmit(DWORD DevIndex, DWORD CANIndex, P_VCI_CAN_OBJ pSend)
{
    return _Transmit(DevIndex, CANIndex, pSend);
}

bool CanFunc::ReadDevSn(DWORD DevIndex, PCHAR DevSn)
{
    return _ReadDevSn(DevIndex, DevSn);
}

DWORD CanFunc::Receive(DWORD DevIndex, DWORD CANIndex, P_VCI_CAN_OBJ pReceive, DWORD Len, INT WaitTime)
{
    return _Receive(DevIndex, CANIndex, pReceive, Len, WaitTime);
}

DWORD CanFunc::GetReceiveNum(DWORD DevIndex, DWORD CANIndex)
{
    return _GetReceiveNum(DevIndex, CANIndex);
}

bool CanFunc::ClearBuffer(DWORD DevIndex, DWORD CANIndex)
{
    return _ClearBuffer(DevIndex, CANIndex);
}

std::string CanFunc::GetErrorMsg()
{
    string msg = "";
    switch(error_code)
    {
    case ERROR_LOAD_DLL:
        msg = "CAN_TO_USB.dll 加载失败!";
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
    }
    return msg;
}

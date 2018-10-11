#include <QLibrary>
#include <QMessageBox>
#include <QDebug>

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

bool CanFunc::LoadDllFuncs()
{
    //加载动态链接库CAN_TO_USB.dll
    QLibrary can_to_usb_lib("CAN_TO_USB");
    if (can_to_usb_lib.load()) {
        _OpenDevice = (Func1)can_to_usb_lib.resolve("VCI_OpenDevice");
        _CloseDevice = (Func1)can_to_usb_lib.resolve("VCI_CloseDevice");
        _ResetCan = (Func2)can_to_usb_lib.resolve("VCI_ResetCan");
        _InitCan = (Func3)can_to_usb_lib.resolve("VCI_InitCan");
        _Transmit = (Func4)can_to_usb_lib.resolve("VCI_Transmit");
        _ReadDevSn = (Func5)can_to_usb_lib.resolve("VCI_ReadDevSn");
        _Receive = (Func6)can_to_usb_lib.resolve("VCI_Receive");
        _GetReceiveNum = (Func7)can_to_usb_lib.resolve("VCI_GetReceiveNum");
        _ClearBuffer = (Func2)can_to_usb_lib.resolve("VCI_ClearBuffer");

        if(!(_OpenDevice || _CloseDevice || _ResetCan || _InitCan || _Transmit
                || _ReadDevSn || _Receive || _GetReceiveNum || _ClearBuffer)) {
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
    default:
        msg = "未知错误";
    }
    return msg;
}

#include "canfunc_puchuang.h"

// CAN
#define Dev_Index 0  //设备索引号
#define Can_Index 1  //第1路CAN

#define ERROR_LOAD_DLL 0x1        //DLL加载失败
#define ERROR_LINK_DLL_FUNCS 0x2  //DLL函数链接失败
#define ERROR_OPENDEVICE 0x3      //打开设备失败
#define ERROR_InitCan 0x4         //CAN初始化失败
#define ERROR_SENDDATA  0x6       //发送失败

CanFunc_PuChuang::CanFunc_PuChuang()
	:_OpenDevice(nullptr),
	_CloseDevice(nullptr),
	_ResetCan(nullptr),
	_InitCan(nullptr),
	_Transmit(nullptr),
	_ReadDevSn(nullptr),
	_Receive(nullptr),
	_GetReceiveNum(nullptr),
	_ClearBuffer(nullptr),
	m_error_code(0)
{
	//加载动态链接库CAN_TO_USB.dll
	HMODULE hMod = LoadLibraryA("CAN_TO_USB.dll");
	if (hMod) {
		_OpenDevice = (Func_PuChuang_1)GetProcAddress(hMod, "VCI_OpenDevice");
		_CloseDevice = (Func_PuChuang_1)GetProcAddress(hMod, "VCI_CloseDevice");
		_ResetCan = (Func_PuChuang_2)GetProcAddress(hMod, "VCI_ResetCan");
		_InitCan = (Func_PuChuang_3)GetProcAddress(hMod, "VCI_InitCan");
		_Transmit = (Func_PuChuang_4)GetProcAddress(hMod, "VCI_Transmit");
		_ReadDevSn = (Func_PuChuang_5)GetProcAddress(hMod, "VCI_ReadDevSn");
		_Receive = (Func_PuChuang_6)GetProcAddress(hMod, "VCI_Receive");
		_GetReceiveNum = (Func_PuChuang_7)GetProcAddress(hMod, "VCI_GetReceiveNum");
		_ClearBuffer = (Func_PuChuang_2)GetProcAddress(hMod, "VCI_ClearBuffer");
	}
	else {
		m_error_code = ERROR_LOAD_DLL;
	}
}

CanFunc_PuChuang::~CanFunc_PuChuang()
{
	if (_CloseDevice) {
		_CloseDevice(Dev_Index);
	}
}

bool CanFunc_PuChuang::OpenAndInitDevice()
{
	//can初始化设置
	init_config.AccCode = 0x00000000;
	init_config.AccMask = 0xFFFFFFFF;
	init_config.Filter = 0;
	init_config.Timing0 = 0x01;  //250Kbps
	init_config.Timing1 = 0x1C;
	init_config.Mode = 0;

	if (m_error_code == 0) {
		if (_OpenDevice(Dev_Index)) {
			if (_InitCan(Dev_Index, Can_Index, &init_config)) {
				return true;
			}
			else {
				m_error_code = ERROR_InitCan;
			}
		}
		else {
			m_error_code = ERROR_OPENDEVICE;
		}
	}
	return false;
}

bool CanFunc_PuChuang::Transmit(PCanMsg data)
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

	if (_Transmit(Dev_Index, Can_Index, &m_candata_struct)) {
		return true;
	}
	else {
		m_error_code = ERROR_SENDDATA;
		return false;
	}
}

unsigned long CanFunc_PuChuang::GetReceiveNum()
{
	return _GetReceiveNum(Dev_Index, Can_Index);
}

bool CanFunc_PuChuang::ReceiveData(PCanMsg data)
{
    if (_Receive(Dev_Index, Can_Index, &m_candata_struct, 1, 5)) {
        if((m_candata_struct.ID == data->id) && (m_candata_struct.DataLen == 8)) {
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
	return false;
}

std::string CanFunc_PuChuang::GetErrorMsg()
{
	string msg = "";
	switch (m_error_code)
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
	case ERROR_SENDDATA:
		msg = "CAN发送失败";
		break;
	default:
		break;
	}
	return msg;
}

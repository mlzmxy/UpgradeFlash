/*
*******************************************************************************************************
*                                                                   								  *
*                                                                                                     * 
*                                                                                                     *                                        
*                                                                                                     *
*                                  Copyright (C), 2007-2008,				 			              *
*                       Department of Instrument Science and Engineering，		                      *
*                             Southeast University， Nanjing 210096                                   *         
*                                                                                                     *
*                                                                                                     *
*                                                                                                     *
*                                                                                                     *                             
*                                                                                                     *           
*                                                                                                     *  
-------------------------------------------------------------------------------------------------------
File Name     :  
Author        :  
Version       :  
					
Create Date   :   
Description:  :   
                  
Function List :   
History       :  
-----------------------------------------------------------------------------------------------------*/
#ifndef CAN_TO_USB_H
#define CAN_TO_USB_H

#include <windows.h>

//1.定义初始化CAN的数据类型
typedef struct _VCI_INIT_CONFIG
{
	DWORD	AccCode;	//对应SJA1000中的四个寄存器ACR0，ACR1，ACR2，ACR3
	DWORD	AccMask;	//对应SJA1000中的四个寄存器AMR0，AMR1，AMR2，AMR3
    DWORD   Reserved;
    UCHAR   Filter;
	UCHAR	Timing0;	//定时器0
	UCHAR	Timing1;	//定时器1 		
	UCHAR   Mode;       //模式   
}VCI_INIT_CONFIG,*P_VCI_INIT_CONFIG;


//2.定义CAN信息帧的数据类型
typedef struct _VCI_CAN_OBJ
{
    DWORD	ID;
    UCHAR   SendType;       //0：正常发送，1：自发自收
	UCHAR	ExternFlag;	    //0：标准帧，  1：扩展帧
	UCHAR	RemoteFlag;	    //0：数据帧，  1：远程帧
	UCHAR	DataLen;
	UCHAR	Data[8];		
}VCI_CAN_OBJ,*P_VCI_CAN_OBJ;

//3.函数接口定义
/*****************************************************************************************************
* Function Name:VCI_OpenDevice
* Function     :打开设备
* Input        :
* Output       :
******************************************************************************************************
*/
extern "C" __declspec(dllimport) BOOL __stdcall VCI_OpenDevice(DWORD DevIndex);

/*****************************************************************************************************
* Function Name:VCI_CloseDevice
* Function     :关闭设备
* Input        :
* Output       :
******************************************************************************************************
*/
extern "C" __declspec(dllimport) BOOL __stdcall  VCI_CloseDevice(DWORD DevIndex);

/*****************************************************************************************************
* Function Name:VCI_ResetCAN
* Function     :复位设备
* Input        :
* Output       :
******************************************************************************************************
*/
extern "C" __declspec(dllimport) BOOL __stdcall VCI_ResetCan(DWORD DevIndex,DWORD CANIndex);

/*****************************************************************************************************
* Function Name:VCI_InitCan
* Function     :设备初始化
* Input        :
* Output       :
******************************************************************************************************
*/
extern "C" __declspec(dllimport) BOOL __stdcall VCI_InitCan(DWORD DevIndex,DWORD CANIndex,P_VCI_INIT_CONFIG pInitConfig);


/*****************************************************************************************************
* Function Name:VCI_Transmit
* Function     :发送数据
* Input        :
* Output       :
******************************************************************************************************
*/
extern "C" __declspec(dllimport) BOOL __stdcall VCI_Transmit(DWORD DevIndex,DWORD CANIndex,P_VCI_CAN_OBJ pSend);

/*****************************************************************************************************
* Function Name:VCI_ReadDevSn
* Function     :获得序列号
* Input        :
* Output       :
******************************************************************************************************
*/
extern "C" __declspec(dllimport) BOOL __stdcall VCI_ReadDevSn(DWORD DevIndex, PCHAR DevSn); //获得序列号

/*****************************************************************************************************
* Function Name:接受数据
* Function     :VCI_Receive
* Input        :
* Output       :
******************************************************************************************************
*/
extern "C" __declspec(dllimport) DWORD __stdcall VCI_Receive(DWORD DevIndex,DWORD CANIndex,P_VCI_CAN_OBJ pReceive,DWORD Len,INT WaitTime);

/*****************************************************************************************************
* Function Name:VCI_GetReceiveNum
* Function     :获取缓冲区尚未读取的帧数
* Input        :
* Output       :
******************************************************************************************************
*/
extern "C" __declspec(dllimport) DWORD __stdcall  VCI_GetReceiveNum(DWORD DevIndex,DWORD CANIndex);

/*****************************************************************************************************
* Function Name:VCI_ClearBuffer
* Function     :清除缓冲区中的数据
* Input        :
* Output       :
******************************************************************************************************
*/
extern "C" __declspec(dllimport) BOOL __stdcall VCI_ClearBuffer(DWORD DevIndex,DWORD CANIndex);

#endif

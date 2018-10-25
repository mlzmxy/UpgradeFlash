/*
*******************************************************************************************************
*                                                                   								  *
*                                                                                                     * 
*                                                                                                     *                                        
*                                                                                                     *
*                                  Copyright (C), 2007-2008,				 			              *
*                       Department of Instrument Science and Engineering��		                      *
*                             Southeast University�� Nanjing 210096                                   *         
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

//1.�����ʼ��CAN����������
typedef struct _VCI_INIT_CONFIG
{
	DWORD	AccCode;	//��ӦSJA1000�е��ĸ��Ĵ���ACR0��ACR1��ACR2��ACR3
	DWORD	AccMask;	//��ӦSJA1000�е��ĸ��Ĵ���AMR0��AMR1��AMR2��AMR3
    DWORD   Reserved;
    UCHAR   Filter;
	UCHAR	Timing0;	//��ʱ��0
	UCHAR	Timing1;	//��ʱ��1 		
	UCHAR   Mode;       //ģʽ   
}VCI_INIT_CONFIG,*P_VCI_INIT_CONFIG;;


//2.����CAN��Ϣ֡����������
typedef struct _VCI_CAN_OBJ
{
    DWORD	ID;
    UCHAR   SendType;       //0���������ͣ�1���Է�����
	UCHAR	ExternFlag;	    //0����׼֡��  1����չ֡
	UCHAR	RemoteFlag;	    //0������֡��  1��Զ��֡
	UCHAR	DataLen;
	UCHAR	Data[8];		
}VCI_CAN_OBJ,*P_VCI_CAN_OBJ;

//3.�����ӿڶ���
/*****************************************************************************************************
* Function Name:VCI_OpenDevice
* Function     :���豸
* Input        :
* Output       :
******************************************************************************************************
*/
extern "C" __declspec(dllimport) BOOL __stdcall VCI_OpenDevice(DWORD DevIndex);

/*****************************************************************************************************
* Function Name:VCI_CloseDevice
* Function     :�ر��豸
* Input        :
* Output       :
******************************************************************************************************
*/
extern "C" __declspec(dllimport) BOOL __stdcall  VCI_CloseDevice(DWORD DevIndex);

/*****************************************************************************************************
* Function Name:VCI_ResetCAN
* Function     :��λ�豸
* Input        :
* Output       :
******************************************************************************************************
*/
extern "C" __declspec(dllimport) BOOL __stdcall VCI_ResetCan(DWORD DevIndex,DWORD CANIndex);

/*****************************************************************************************************
* Function Name:VCI_InitCan
* Function     :�豸��ʼ��
* Input        :
* Output       :
******************************************************************************************************
*/
extern "C" __declspec(dllimport) BOOL __stdcall VCI_InitCan(DWORD DevIndex,DWORD CANIndex,P_VCI_INIT_CONFIG pInitConfig);


/*****************************************************************************************************
* Function Name:VCI_Transmit
* Function     :��������
* Input        :
* Output       :
******************************************************************************************************
*/
extern "C" __declspec(dllimport) BOOL __stdcall VCI_Transmit(DWORD DevIndex,DWORD CANIndex,P_VCI_CAN_OBJ pSend);

/*****************************************************************************************************
* Function Name:VCI_ReadDevSn
* Function     :������к�
* Input        :
* Output       :
******************************************************************************************************
*/
extern "C" __declspec(dllimport) BOOL __stdcall VCI_ReadDevSn(DWORD DevIndex, PCHAR DevSn); //������к�

/*****************************************************************************************************
* Function Name:��������
* Function     :VCI_Receive
* Input        :
* Output       :
******************************************************************************************************
*/
extern "C" __declspec(dllimport) DWORD __stdcall VCI_Receive(DWORD DevIndex,DWORD CANIndex,P_VCI_CAN_OBJ pReceive,DWORD Len,INT WaitTime);

/*****************************************************************************************************
* Function Name:VCI_GetReceiveNum
* Function     :��ȡ��������δ��ȡ��֡��
* Input        :
* Output       :
******************************************************************************************************
*/
extern "C" __declspec(dllimport) DWORD __stdcall  VCI_GetReceiveNum(DWORD DevIndex,DWORD CANIndex);

/*****************************************************************************************************
* Function Name:VCI_ClearBuffer
* Function     :����������е�����
* Input        :
* Output       :
******************************************************************************************************
*/
extern "C" __declspec(dllimport) BOOL __stdcall VCI_ClearBuffer(DWORD DevIndex,DWORD CANIndex);

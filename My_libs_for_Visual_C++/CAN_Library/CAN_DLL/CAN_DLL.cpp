// CAN_DLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "CAN_DLL.h"


/***********************************************************************
*			�������������� ����������
***********************************************************************/

/**********************************************************************
*			������������� ����������
***********************************************************************/
extern HWND	g_hPseudoWindow;	//����� ����� ������-���� �� dllmain.cpp

/**********************************************************************
*			���������� ���������� �������
**********************************************************************/
// ���������, ������������ �������� CAN
// ����������� � ��������� �������� ����� ������ � �������� �� PIC18F4585 ����������
struct CAN_DLL_API CCAN_Speed
{
	//������������
	CCAN_Speed(){};
	CCAN_Speed(BYTE brp, BYTE propseg, BYTE phseg1, BYTE phseg2, BYTE sjw);
	BYTE BRP;		//�� 0 �� 63 - �������� �� ����� ����
	BYTE PROPSEG;	//|
	BYTE PHSEG1;	//| �� 0 �� 7 - � ����� ���� ���������� ����� � ����
	BYTE PHSEG2;	//|
	BYTE SJW;		// �� 0 �� 3 - �� ���������� � �������� ��������, � ���..., 
					// � ���������� SJW==1
};

/**********************************************************************
*			���������� ���������� ����������
**********************************************************************/
//��� ������
CSerialPort		g_COM_Port;
//����� ����, �������� � ���� �������� ��������� ��� ������ CAN �������
HWND g_MesReceiveWindow;

//��������� �������
HANDLE g_hg_ReceiveErrorsCountMes;	//�������� ��������� � ����������� ������ ��� ������
HANDLE g_hg_TranceiveErrorsCountMes;	//			>>					>>		  ��� ��������
HANDLE g_hBusOffMessage;			//	>>		>>		 � ��������� "Bus-Off"
HANDLE g_hCAN_SpeedMessage;			//	>>		>>		 �� ��������� CAN
HANDLE g_hFilterSetOK;				//	>>		>>		 � �������������� ��������� �������
HANDLE g_hMaskSetOK;				//	>>		>>		 � �������������� ��������� �����
HANDLE g_hSpeedSetOK;				//	>>		>>		 � �������������� ��������� ��������
HANDLE g_hRegimeSetOK;				//	>>		>>		 � �������������� ��������� ������ ��������� ��������
HANDLE g_hCAN_MesSended;			//	>>		>>		 ��� CAN ��������� ����������
HANDLE g_hFilterEnDisOK;			//	>>		>>		 � �������������� ���������/���������� �������
HANDLE g_hFilterRequestOK;			//	>>		>>		 �� ��������� �������
HANDLE g_hMaskRequestOK;			//	>>		>>		 �� ��������� �����
HANDLE g_hRegimeRequestOK;			//	>>		>>		 �� ��������� ������ ������ ���������������

//����� ��������� ����������, ����� ������� � ���� ���������� ��������� ������ ��������� �� RS-232
//����� ���� �� �������� ����� ������� ����������� ���������, �� � ����� �� ������ �����, � �������
//��� ������� ��������� ��� ����� ����������� ����������.
volatile INT8U		g_ReceiveErrorsCount;
volatile INT8U		g_TranceiveErrorsCount;
volatile bool		g_Bus_Off;
volatile CCAN_Speed g_CAN_SpeedResult;
volatile INT8U		g_FilterSetResult;
volatile INT8U		g_MaskSetResult;
volatile INT8U		g_SpeedSetResult;
volatile INT8U		g_RegimeSetResult;
volatile INT8U		g_SendMesCAN_Result;
volatile INT8U		g_FilterEnDisResult;
volatile INT32U		g_FilterValue;
volatile bool		g_IsExtended;
volatile bool		g_IsEnable;
volatile INT32U		g_MaskValue;
volatile INT8U		g_RegimeGetResult;

const INT8U COMPUTER_ID			= 0x51;				// ������������� ���������� � ���� RS-232
const INT8U	CONTROLLER_ID		= 0x50;				//		>>		>>		�����������
const WORD	CONTROLLER_VID		= 0x10C4;			//| 
const WORD	CONTROLLER_PID		= 0xEA60;			//| �������������� CP210x � ���� USB
const TCHAR	CONTROLLER_SERIAL[]	= _TEXT("1234\0");	//  ������ ��������� ������ ��������������� � ���� USB
const INT32U F_OSC = 40000000;						//������� ������ ����������������

bool g_bConnected=false;			//��������� �� ���������������
									// true - ��
									// false - ���
bool g_bCOM_Forsibly;				//��� �� ������ ��������������� ����� ��� ����
									// true - ��
									// false - ���

DWORD	g_dTimeOut=1000;			//����� � ��, � ������� �������� � ���� ����� 
									//������ �� ���������������

/**********************************************************************
*			���������� ���������� �������
***********************************************************************/
//�������������� �������������
void InitCAN_DLL(void);
//��� ������� ����� ��������� ��������� �� Serial Port � 
//����������, ��� ������ ������
void CAN_Supervisor(UINT Msg, WPARAM wParam, LPARAM lParam);
//���������� ���������� �� RS-232 ���������
bool CAN_RS232_Verify(BYTE *pReceiveMessage, WORD MesageLength);
//��������� ������ �� RS-232
bool ReceiveChar(INT8U ch);
//���������� ��������� WM_DEVICECHANGE
void OnDeviceChange(WPARAM wParam, LPARAM lParam);
//��������� ��������� �� ��������������� � ����������
void UpdateConnected();
//�������������� COM ���� �� ���������������
CAN_RESULT InitializeCOM();
//��������� �� ������������ CAN ���������
bool VerifyCAN_Message(CCAN_Message* pCAN_Message);

//��������� ��� ��������� ��������
CAN_RESULT	CAN_SpeedRegFind(DWORD BaudRate,CCAN_Speed *pCAN_Speed=NULL, float* pError=NULL, DWORD* pActualSpeed=NULL);
bool		CAN_CheckSpeedRight (const CCAN_Speed *pCAN_Speed);
DWORD		CAN_CalculateSpeed	(CCAN_Speed *pCAN_Speed);
CAN_RESULT	CAN_GetSpeed(CCAN_Speed *pCAN_Speed);
CAN_RESULT	CAN_SetSpeed(const CCAN_Speed *pCAN_Speed);

/***********************************************************************
*				������ ��������� CCAN_Speed
***********************************************************************/
//�����������
CCAN_Speed::CCAN_Speed(BYTE brp, BYTE propseg, BYTE phseg1, BYTE phseg2, BYTE sjw)
{
	BRP=brp; PROPSEG=propseg; PHSEG1=phseg1; PHSEG2=phseg2; SJW=sjw;
}

/************************************************************************
*					���������� �������
************************************************************************/
/**********************    InitCAN_DLL      *****************************
*		����� ������� ���������������� ��������� ���������� ������� 
*	dllmain() � ����� dllmain.cpp, ������, �.�. � ���� ���� ���������
*	����������, �������������� ������� �� �������, �� � ������ ������
*	������ ��� �������.
************************************************************************/
void InitCAN_DLL(void)
{
	g_MesReceiveWindow=NULL;

	//������ �������, ������� ����� ��������������� � ������� ���� 
	//��� ����� ��������� �� ���� RS-232
	g_hg_ReceiveErrorsCountMes	= CreateEvent(NULL, TRUE, TRUE, NULL);	//�������� ��������� � ����������� ������ ��� ������
	g_hg_TranceiveErrorsCountMes	= CreateEvent(NULL, TRUE, TRUE, NULL);	//			>>					>>		  ��� ��������
	g_hBusOffMessage			= CreateEvent(NULL, TRUE, TRUE, NULL);	//	>>		>>		 � ��������� "Bus-Off"
	g_hCAN_SpeedMessage			= CreateEvent(NULL, TRUE, TRUE, NULL);	//	>>		>>		 �� ��������� CAN
	g_hFilterSetOK				= CreateEvent(NULL, TRUE, TRUE, NULL);	//	>>		>>		 � �������������� ��������� �������
	g_hMaskSetOK				= CreateEvent(NULL, TRUE, TRUE, NULL);	//	>>		>>		 � �������������� ��������� �����
	g_hSpeedSetOK				= CreateEvent(NULL, TRUE, TRUE, NULL);	//	>>		>>		 � �������������� ��������� ��������
	g_hRegimeSetOK				= CreateEvent(NULL, TRUE, TRUE, NULL);	//	>>		>>		 � �������������� ��������� ������ ��������� ��������
	g_hCAN_MesSended			= CreateEvent(NULL, TRUE, TRUE, NULL);	//	>>		>>		 ��� CAN ��������� ����������
	g_hFilterEnDisOK			= CreateEvent(NULL, TRUE, TRUE, NULL);	//	>>		>>		 � �������������� ���������/���������� �������
	g_hFilterRequestOK			= CreateEvent(NULL, TRUE, TRUE, NULL);	//	>>		>>		 �� ��������� �������
	g_hMaskRequestOK			= CreateEvent(NULL, TRUE, TRUE, NULL);	//	>>		>>		 �� ��������� �����
	g_hRegimeRequestOK			= CreateEvent(NULL, TRUE, TRUE, NULL);	//	>>		>>		 �� ��������� ������ ������ ���������������

	g_bCOM_Forsibly=false;		//������������� ����� ���������������� ���� �� ������������

	//�����, ��������� �� ���������������
	UpdateConnected();
	//�, ���� ���������, �� ������� ����
	InitializeCOM();
}
/**********************    OnDeviceChange     ***************************
*		��� ���������� ��������� WM_DEVICECHANGE, ������� ����������� 
*	����� ���������� ���� � ����� dllmain.cpp ����� �������� ������
*	������������ � ������� ���������. �� ��� � ���������, ����� �������� ���
*	�������� ���������������.
*************************************************************************/
void OnDeviceChange(WPARAM wParam, LPARAM lParam)
{
	if(g_bCOM_Forsibly)	return;		//���� ������ ������������� ����� COM ����

	switch(wParam)
	{
	case DBT_DEVICEARRIVAL:			//���������� �������� � ��� ������ � ������
		//���� ��������������� ���
		//���������, �� ����� ��������� ��� ����
		if(CAN_IsMonitorConnected()==true) return;	
		break;
	case DBT_DEVICEREMOVECOMPLETE:	//���������� �������� �� ����������
		//���� ��������������� ��� � � ������, �� ��� ����������
		//�����-�� ����� �� ���� ��������� ������
		if(CAN_IsMonitorConnected()==false) return;
		break;
	default:						//��� ���� ��� ��������� �������� ���������� ��������
		return;    
	}
	//���� � ������ ���� switch, �� ������ � ���� ��������, ��� ���������� 
	//��� ��������� ���������������
	
	bool lOldConnected=CAN_IsMonitorConnected();
	UpdateConnected();		//������ ��������� ������ ������������ ���������
	if(lOldConnected==false && CAN_IsMonitorConnected()==true)
	{//��������������� ������-��� ����������
		if(InitializeCOM()==CR_ALLOK)
		{//������������ ���������
			PostMessage(g_MesReceiveWindow,MSG_NEW_RECEIVE,(WPARAM)NULL,CTF_STATE_CHANGED);
		}
	}
	else if(lOldConnected==true  && CAN_IsMonitorConnected()==false)
	{//��������������� ������-��� ���������
		PostMessage(g_MesReceiveWindow,MSG_NEW_RECEIVE,(WPARAM)NULL,CTF_STATE_CHANGED);
	}
}

/**************************    UpdateConnected    ***********************************
*		��������� ��������� �� ��������������� � ����������. ������ �����������
*	��� ������������ � ����� CP210x ����������, � ���� ����� ��� ��� ���������������
*	- � �������� ���������� ���������� g_bConnected. ��� ����������� ��������������� -
*	� �������� ��� ���������� ������ ������������ g_bConnected, ������� � ������ ������
*	������� �������� �������������� ��������.
*************************************************************************************/
void UpdateConnected()
{
	DWORD lCP210xCount;
	CP210x_STATUS lRes=CP210x_GetNumDevices(&lCP210xCount);
	if(lRes!=CP210x_SUCCESS)
	{//���-�� � �� ��� � ��������
		g_bConnected=false;	//���� �������, ��� ������ �� ����������
		return;
	}
	for(DWORD i=0; i<lCP210xCount;i++)
	{//���� �� ���� ������������ CP210x
		CP210x_DEVICE_STRING lDevString;
		lRes=CP210x_GetProductString(i,lDevString,CP210x_RETURN_FULL_PATH);
		if(lRes==CP210x_SUCCESS)
		{//������� ���������� ���������
			CHAR lBuf[5];			//|
			lBuf[0]=lDevString[26];	//|
			lBuf[1]=lDevString[27];	//| �������� ������ �� ����� ���������� ������, � ������� ���������� 
			lBuf[2]=lDevString[28];	//| �������� �����
			lBuf[3]=lDevString[29];	//|
			lBuf[4]=0;				//|
			//��������� ��� � UNICODE
			WCHAR lSerStringW[256];
			wsprintf(lSerStringW,L"%S\0",lBuf);
			if(lstrcmp(lSerStringW,CONTROLLER_SERIAL)==0)
			{//������ �����, ��� ���������������!
				g_bConnected=true;	//|
				return;				//| ��������������� ���������!
			}
		}
	}
	g_bConnected=false;			//|
	return;						//| ��� � �� ����� ��������� ����������
}


/************************    InitializeCOM    *********************************
*		���� ��������������� ��������� � ����������, �� ��� ����� ��������� 
*	���������������� ���� �� ������ � ���. ���� � ���������� �������.
******************************************************************************/
CAN_RESULT InitializeCOM()
{
	//�����, ��������� �� ��������������� � ����������
	if(!CAN_IsMonitorConnected())
	{//��������������� �� ���������
		return CR_NOTCONNECTED;
	}

	//����� ����� ����������������� ����� ���������������
	int lPortNum=GetPortNumXP2000Vista(CONTROLLER_VID, CONTROLLER_PID, (TCHAR*)CONTROLLER_SERIAL);
	if(lPortNum==-1)
	{//� ��������������� ��� ������ � �������, ���� �� ���������: �������-�������?
		g_bConnected=false;
		return CR_OTHERERROR;
	}

	//������������� ����� �����
	if(g_COM_Port.InitPort(NULL,				//����, �������� ����� ������������ ��� ���������
						lPortNum,				//����, ������� ����� ���������
						115200,					//�������� ��������
						'N',					//�������� ��������
						8,						//��������
						1))						//�������� �����
	{//���� ������ �������
		//������ ��������� �� �������-����������, ������� ����� �����������
		//������� �� �����
		if(!g_COM_Port.SetParentSupervisor(CAN_Supervisor))
		{//������
			g_bConnected=false;
			return CR_OTHERERROR;
		}
	}
	else
	{//�� ����� ������
		g_bConnected=false;
		return CR_OTHERERROR;
	}

	return CR_ALLOK;
}

/**************************    VerifyCAN_Message    *******************************
*		��������� �� ������������ CAN ���������. ���� ����������� ������ �����
*	������, �.�. �������� �������� � ���� ���� ����� ������� ������ ����� ��������
*	��������.
***********************************************************************************/
bool VerifyCAN_Message(CCAN_Message* pCAN_Message)
{
	if(pCAN_Message==NULL) return false;
	if(pCAN_Message->DataLen>8) return false;
	return true;
}

/**********************     CAN_COM_Open    ****************************************
*		�� ������ ��������������� ���� ����� ������� ���������������� �����, ���� 
*	������� CP2102 �� ����� (��� ������ ������ ��� �������), �� ��� ������� ������������� 
*	�������������� ���������� ����� ���������������� ����
***********************************************************************************/
CAN_DLL_API CAN_RESULT	CAN_COM_Open(unsigned char port_num)
{
	//������������� ����� �����
	if(g_COM_Port.InitPort(NULL,					//����, �������� ����� ������������ ��� ���������
						port_num,				//����, ������� ����� ���������
						115200,					//�������� ��������
						'N',					//�������� ��������
						8,						//��������
						1))						//�������� �����
	{//���� ������ �������
		//������ ��������� �� �������-����������, ������� ����� �����������
		//������� �� �����
		if(!g_COM_Port.SetParentSupervisor(CAN_Supervisor))
		{//������
			g_bCOM_Forsibly=false;
			return CR_OTHERERROR;
		}
	}
	else
	{//�� ����� ������
		g_bCOM_Forsibly=false;
		return CR_OTHERERROR;
	}
	g_bCOM_Forsibly=true;		//� ����������� ����� ���������������� ����
	return CR_ALLOK;
}
/**************************    CAN_IsCOM_Opened    *********************************
*		���� COM ���� ��� ������ ������������� - �� ������������ ������, � ���������
*	������ - ����.� ����������� ������� ��� ������ �� �����, ��� ����� ������������, 
*	����� �������� ������������ - ��������� �� ��������������� � COM �����.
***********************************************************************************/
CAN_DLL_API bool CAN_IsCOM_Opened(void)
{
	return g_bCOM_Forsibly;
}

/**************************    CAN_IsMonitorConnected   ****************************
*		������ ������� ���������� - ��������� �� ��������������� � ����������
*	��� ���.
*		true - ��������������� ��������� � CP2102 ����������������, ���� ���� 
*				�������������� ������������� ����� ������� CAN_COM_Open
*		false - �� ���������
***********************************************************************************/
CAN_DLL_API bool CAN_IsMonitorConnected()
{
	if(g_bCOM_Forsibly) return true;	//������� ������ ����� COM ����, ������� ������������
										//��������������� ���������
	//��������� �� �� ����� USB
	return g_bConnected;
}

/*************************    CAN_SetMesReceiveWindow    ***************************
*		������������� ��������� �� ����, ������� ����� ��������� ��������� �� CAN
*	hWnd - ����� ����
*	���� ��� ���������, �� ���������� CR_ALLOK. ���� ���-�� �� �� � �������, ��
*	CR_BADARGUMENTS
************************************************************************************/
CAN_RESULT	CAN_SetMesReceiveWindow(HWND hWnd)
{
	if(hWnd==NULL || hWnd==INVALID_HANDLE_VALUE )return CR_BADARGUMENTS;
	g_MesReceiveWindow=hWnd;
	return CR_ALLOK;
}

/*************************    CAN_SetTimeOut    ***********************************
*		������������� ����-��� �������� ������ ���������������.
*	time_ms - ����� ����-���� � �������������. ������ ���� �� ������ 10 ��.
*	���� ��� ���������, �� ���������� CR_ALLOK. ���� ���-�� �� �� � time_ms, ��
*	CR_BADARGUMENTS
***********************************************************************************/
CAN_DLL_API CAN_RESULT CAN_SetTimeOut(DWORD time_ms)
{
	if(time_ms<10) return CR_BADARGUMENTS;
	g_dTimeOut=time_ms;							//������ �����
	return CR_ALLOK;
}

/************************    CAN_GetTimeOut    *************************************
*		���������� ������������� ����-��� �������� ������ ���������������.
***********************************************************************************/
CAN_DLL_API DWORD	CAN_GetTimeOut(void)
{
	return g_dTimeOut;
}

/*************************     CAN_CheckSpeedRight     *************************************
*	������� ��������� ������������ �������� �������� CAN �������� �������� PIC18F4585
*********************************************************************************************/
bool CAN_CheckSpeedRight (const CCAN_Speed *pCAN_Speed)
{
	if(pCAN_Speed==NULL)	return false;

	if(pCAN_Speed->BRP    <0 || pCAN_Speed->BRP    >63){return false;}
	if(pCAN_Speed->PROPSEG<0 || pCAN_Speed->PROPSEG>7) {return false;}
	if(pCAN_Speed->PHSEG1 <0 || pCAN_Speed->PHSEG1 >7) {return false;}
	if(pCAN_Speed->PHSEG2 <0 || pCAN_Speed->PHSEG2 >7) {return false;}
	if(pCAN_Speed->SJW    <0 || pCAN_Speed->SJW    >3) {return false;}
	if(pCAN_Speed->PHSEG2 > (pCAN_Speed->PHSEG1+pCAN_Speed->PROPSEG)){return false;}
	if(pCAN_Speed->PHSEG2 < pCAN_Speed->SJW)		   {return false;}
	return true;
}

/*********************     CAN_CalculateSpeed     ***************************************
*	����������� ����������� �������� ��������� ���������������� � �������� � �����.
*	����� ������ �������� ������ ��. ������� PIC18F4585
*		0 - ������� ���������� � ��������
****************************************************************************************/
DWORD	CAN_CalculateSpeed	(CCAN_Speed *pCAN_Speed)
{
	if(!CAN_CheckSpeedRight (pCAN_Speed)) return 0;

	DWORD temp=F_OSC/(2*(pCAN_Speed->BRP+1));					 //1 ������ �� ����� ������ ������ (����)
	DWORD lBaud=temp/(1+pCAN_Speed->PROPSEG+pCAN_Speed->PHSEG1+pCAN_Speed->PHSEG2+3);	//����� ����� ����

	return lBaud;
}

/*********************    CAN_SpeedRegFind   ******************************************
*		������� ���������� ��������� ��� �������� �������� ������ ������� ������.
*	� ���� ������� � ��������� ���������������� ���� �������.
*		BaudRate - ��������� �������� � ����� (�������� 500 000 ���/�)
*		pCAN_Speed - ��������� �� ��������� ��������, ������� ����� ��������� ��������� (�.�. NULL)
*		pError  - ��������� �� ����������, ������� ����� ��������� ������ (�.�. NULL)
*		pActualSpeed - ��������� �� ����������, ������� ����� ��������� �������������� �������� (�.�. NULL)
**************************************************************************************/
CAN_RESULT	CAN_SpeedRegFind(DWORD BaudRate,CCAN_Speed *pCAN_Speed, float* pError, DWORD* pActualSpeed)
{
	INT32S lMaxMatch=100000000;
	CCAN_Speed lSpeed(0,0,0,0,1);
	
	for(lSpeed.BRP=0; lSpeed.BRP<63; lSpeed.BRP++)
		for(lSpeed.PROPSEG=0; lSpeed.PROPSEG<7; lSpeed.PROPSEG++)
			for(lSpeed.PHSEG1=0; lSpeed.PHSEG1<7; lSpeed.PHSEG1++)
				for(lSpeed.PHSEG2=0; lSpeed.PHSEG2<7; lSpeed.PHSEG2++)
				{
					//���� �������� ����������, �� �� ���� �� �������������
					if(!CAN_CheckSpeedRight(&lSpeed)) continue;
					INT32S lCurSpeed=CAN_CalculateSpeed	(&lSpeed);
					if(abs(lCurSpeed-(INT32S)BaudRate)<abs(lMaxMatch-(INT32S)BaudRate))
					{//���� ����� �������� �����
						lMaxMatch=lCurSpeed;
						if(pCAN_Speed!=NULL)
						{
							pCAN_Speed->BRP=lSpeed.BRP;
							pCAN_Speed->PROPSEG=lSpeed.PROPSEG;
							pCAN_Speed->PHSEG1=lSpeed.PHSEG1;
							pCAN_Speed->PHSEG2=lSpeed.PHSEG2;
						}
					}

				}
	//��������� ������ � ���������
	if(pError!=NULL) *pError=(FP32)abs(lMaxMatch-(INT32S)BaudRate)/(FP32)BaudRate*100.f;
	//������ �������� ���������� ��������
	if(pActualSpeed!=NULL) *pActualSpeed=lMaxMatch;
	//�� � �����������, �� �� �������� �� �������� SJW
	if(pCAN_Speed!=NULL) pCAN_Speed->SJW=1;

	return CR_ALLOK;
}

/**************************     CAN_CalcActualSpeed    **************************************
*		��� ��������� ������ CAN_SpeedRegFind, ��������� ����� DLL
********************************************************************************************/
CAN_DLL_API CAN_RESULT	CAN_CalcActualSpeed(DWORD BaudRate, float* pError, DWORD* pActualSpeed)
{
	return CAN_SpeedRegFind(BaudRate,NULL, pError, pActualSpeed);
}

/**************************    CAN_SendMessage     **********************************
*		���������� �� ���� CAN ���������.
*	pCAN_Message - ��������� �� ������������ ���������
*		������������ ��������:
*	CR_ALLOK		- ��������� ������� ����������
*	CR_NOTCONNECTED - ��������������� �� ���������
*	CR_BADARGUMENTS - pCAN_Message ����� NULL ��� ��������� �����������
*	CR_DEVICEBUSY	- ��������������� �� ���� ��������� ���������, ����� �� �� ����-
*						����� � CAN ���� ��� �������� �� �� �� ��������
*	CR_NOANSWER		- ��������������� �� �������� 
*	CR_OTHERERROR	- ������ ���������� ������
************************************************************************************/
CAN_DLL_API CAN_RESULT CAN_SendMessage(CCAN_Message* pCAN_Message)
{
	if(!CAN_IsMonitorConnected())		return CR_NOTCONNECTED;	//�������� �� �����������
	if(!VerifyCAN_Message(pCAN_Message))return CR_BADARGUMENTS;	//�������� ����������

	/*******************************************************************
	*	�������� �� CAN ��������� � ����������
	********************************************************************/
	static INT8U message[35];
	INT8U i=0; 

	//����� ����� ���������
	message[i++]='$';						//������ ������ �������
	message[i++]=COMPUTER_ID;				//������������� �����������
	message[i++]=0x25;						//Message Type
	message[i++]=pCAN_Message->DataLen+6;	//Data Length
	
	//����� ������
	*((INT32U*)&message[i++])=pCAN_Message->ID; i++;i++;i++;	//�������������
	message[i++]=pCAN_Message->DataLen;							//����� ���� ������
	message[i++]=pCAN_Message->Flags;							//�������������� �����
	for(INT8U k=0; k<pCAN_Message->DataLen; k++)
	{//����������� ������ ������ (���������� ����� ��� 8 ����)
		message[i++]=pCAN_Message->Data[k];
	}
	
	//����� ����������� �����
	INT8U sum=CRC_8_polinom_8541(message,1,i);					//������ ���� � ����� �� ������
	message[i]=sum;
	message[i+1]=0x00;											//�� ������ ������
	
	//�������� ���������
	if(!g_COM_Port.WriteToPort(message,i+1))
	{//���-�� ��� �� ��������
		return CR_OTHERERROR;
	}

	//��������� �� �������� ������ �� �����������
	if(!ResetEvent(g_hCAN_MesSended))
	{
		return CR_OTHERERROR;
	}

	//���� ����� ������
	DWORD tmp=WaitForSingleObject(g_hCAN_MesSended, CAN_GetTimeOut()/*INFINITE*/);
	if(tmp==WAIT_OBJECT_0)
	{//���������� �������, ��� � �������
		//����� ������ ������
		if(g_SendMesCAN_Result!=0)
			return CR_DEVICEBUSY;
	}
	else if(tmp==WAIT_TIMEOUT)
	{//���������� �� �������
		SetEvent(g_hCAN_MesSended);
		return CR_NOANSWER;
	}
	else
	{//�� ����� Windows
		return CR_OTHERERROR;
	}

	return CR_ALLOK;
}

/************************    CAN_Getg_ReceiveErrorsCount    ************************
*		���������� ���������� ������ ��� ������ (�� ������������ CAN 2.0B).
*		pCount - ��������� �� ����������, ������� ����� ��������� ���������.
*		������������ ��������:
*	���� ��� ������ ���������			- CR_ALLOK
*	pCount ����� NULL					- CR_BADARGUMENTS
*	���� ��������������� �� ���������	- CR_NOTCONNECTED
*	���� ��������������� �� ��������	- CR_NOANSWER
*	������ ���������� ������			- CR_OTHERERROR
************************************************************************************/
CAN_DLL_API CAN_RESULT CAN_GetReceiveErrorsCount(unsigned char* pCount)
{
	if(pCount==NULL)				return CR_BADARGUMENTS;	//������ ������ ���������
	if(!CAN_IsMonitorConnected())	return CR_NOTCONNECTED;	//�������� �� �����������

	static INT8U message[15];
	INT8U i=0; 

	//����� ����� ���������
	message[i++]='$';					//������ ������ �������
	message[i++]=COMPUTER_ID;			//������������� �����������
	message[i++]=0x10;					//Message Type
	message[i++]=0x00;					//Data Length
	//����� ������
	
	//����� ����������� �����
	INT8U sum=CRC_8_polinom_8541(message,1,i);				//������ ���� � ����� �� ������
	message[i]=sum;
	message[i+1]=0x00;										//�� ������ ������
	
	//�������� ���������
	if(!g_COM_Port.WriteToPort(message,i+1))
	{//���-�� ��� �� ��������
		return CR_OTHERERROR;
	}

	//��������� �� �������� ������ �� �����������
	if(!ResetEvent(g_hg_ReceiveErrorsCountMes))
	{
		return CR_OTHERERROR;
	}

	//���� ����� ������
	DWORD tmp=WaitForSingleObject(g_hg_ReceiveErrorsCountMes, CAN_GetTimeOut());
	if(tmp==WAIT_OBJECT_0)
	{//���������� �������, ��� � �������
		*pCount=g_ReceiveErrorsCount;
	}
	else if(tmp==WAIT_TIMEOUT)
	{//���������� �� �������
		SetEvent(g_hg_ReceiveErrorsCountMes);
		return CR_NOANSWER;
	}
	else
	{//�� ����� Windows
		return CR_OTHERERROR;
	}

	return CR_ALLOK;
}

/************************    CAN_Getg_ReceiveErrorsCount    ************************
*		���������� ���������� ������ ��� �������� (�� ������������ CAN 2.0B).
*		pCount - ��������� �� ����������, ������� ����� ��������� ���������.
*		������������ ��������:
*	���� ��� ������ ���������			- CR_ALLOK
*	pCount ����� NULL					- CR_BADARGUMENTS
*	���� ��������������� �� ���������	- CR_NOTCONNECTED
*	���� ��������������� �� ��������	- CR_NOANSWER
*	������ ���������� ������			- CR_OTHERERROR
************************************************************************************/
CAN_DLL_API CAN_RESULT CAN_GetTranceiveErrorsCount(unsigned char* pCount)
{
	if(pCount==NULL)				return CR_BADARGUMENTS;	//������ ������ ���������
	if(!CAN_IsMonitorConnected())	return CR_NOTCONNECTED;	//�������� �� �����������

	static INT8U message[15];
	INT8U i=0; 

	//����� ����� ���������
	message[i++]='$';					//������ ������ �������
	message[i++]=COMPUTER_ID;			//������������� �����������
	message[i++]=0x11;					//Message Type
	message[i++]=0x00;					//Data Length
	//����� ������
	
	//����� ����������� �����
	INT8U sum=CRC_8_polinom_8541(message,1,i);		//������ ���� � ����� �� ������
	message[i]=sum;
	message[i+1]=0x00;										//�� ������ ������
	
	//�������� ���������
	if(!g_COM_Port.WriteToPort(message,i+1))
	{//���-�� ��� �� ��������
		return CR_OTHERERROR;
	}

	//��������� �� �������� ������ �� �����������
	if(!ResetEvent(g_hg_TranceiveErrorsCountMes))
	{
		return CR_OTHERERROR;
	}

	//���� ����� ������
	DWORD tmp=WaitForSingleObject(g_hg_TranceiveErrorsCountMes, CAN_GetTimeOut());
	if(tmp==WAIT_OBJECT_0)
	{//���������� �������, ��� � �������
		*pCount=g_TranceiveErrorsCount;
	}
	else if(tmp==WAIT_TIMEOUT)
	{//���������� �� �������
		SetEvent(g_hg_TranceiveErrorsCountMes);
		return CR_NOANSWER;
	}
	else
	{//�� ����� Windows
		return CR_OTHERERROR;
	}

	return CR_ALLOK;
}

/***********************    CAN_GetBusOff     ***************************************
*	��������� ��������������� �� ��������� "Bus-Off" (�� ������������ CAN)
*		pBusOff - ��������� �� ����������, ������� ����� ��������� ���������
*	������������ ��������:
*		CR_ALLOK		- ������� ������� ����������
*		CR_NOTCONNECTED - ��������������� �� ��������� � ����������
*		CR_NOANSWER		- ��������������� �� �������� �� �������
*		CR_OTHERERROR	- ������ ������
*		*pBusOff ==	true  - ��������������� ��������� � ��������� Bus-Off
*					false - � �������� ���������
************************************************************************************/
CAN_DLL_API CAN_RESULT CAN_GetBusOff(bool *pBusOff)
{
	if(pBusOff==NULL)				return CR_BADARGUMENTS;	//������ ������ ���������
	if(!CAN_IsMonitorConnected())	return CR_NOTCONNECTED;	//�������� �� �����������

	static INT8U message[15];
	INT8U i=0; 

	//����� ����� ���������
	message[i++]='$';					//������ ������ �������
	message[i++]=COMPUTER_ID;			//������������� �����������
	message[i++]=0x12;					//Message Type
	message[i++]=0x00;					//Data Length
	//����� ������
	
	//����� ����������� �����
	INT8U sum=CRC_8_polinom_8541(message,1,i);				//������ ���� � ����� �� ������
	message[i]=sum;
	message[i+1]=0x00;										//�� ������ ������
	
	//�������� ���������
	if(!g_COM_Port.WriteToPort(message,i+1))
	{//���-�� ��� �� ��������
		return CR_OTHERERROR;
	}

	//��������� �� �������� ������ �� �����������
	if(!ResetEvent(g_hBusOffMessage))
	{
		return CR_OTHERERROR;
	}

	//���� ����� ������
	DWORD tmp=WaitForSingleObject(g_hBusOffMessage, CAN_GetTimeOut());
	if(tmp==WAIT_OBJECT_0)
	{//���������� �������, ��� � �������
		*pBusOff=g_Bus_Off;
	}
	else if(tmp==WAIT_TIMEOUT)
	{//���������� �� �������
		SetEvent(g_hBusOffMessage);
		return CR_NOANSWER;
	}
	else
	{//�� ����� Windows
		return CR_OTHERERROR;
	}

	return CR_ALLOK;//������������ ���������
}

/**************************    CAN_GetSpeed    **************************************
*		������ ��������, �� ������� �������� ���������������.
************************************************************************************/
CAN_RESULT CAN_GetSpeed(CCAN_Speed *pCAN_Speed)
{
	if(pCAN_Speed==NULL)			return CR_BADARGUMENTS;	//������ ������ ���������
	if(!CAN_IsMonitorConnected())	return CR_NOTCONNECTED;	//�������� �� �����������

	static INT8U message[15];
	INT8U i=0; 

	//����� ����� ���������
	message[i++]='$';					//������ ������ �������
	message[i++]=COMPUTER_ID;			//������������� �����������
	message[i++]=0x13;					//Message Type
	message[i++]=0x00;					//Data Length
	//����� ������
	
	//����� ����������� �����
	INT8U sum=CRC_8_polinom_8541(message,1,i);				//������ ���� � ����� �� ������
	message[i]=sum;
	message[i+1]=0x00;										//�� ������ ������
	
	//�������� ���������
	if(!g_COM_Port.WriteToPort(message,i+1))
	{//���-�� ��� �� ��������
		return CR_OTHERERROR;
	}

	//��������� �� �������� ������ �� �����������
	if(!ResetEvent(g_hCAN_SpeedMessage))
	{
		return CR_OTHERERROR;
	}

	//���� ����� ������
	DWORD tmp=WaitForSingleObject(g_hCAN_SpeedMessage, CAN_GetTimeOut());
	if(tmp==WAIT_OBJECT_0)
	{//���������� �������, ��� � �������
		pCAN_Speed->BRP		= g_CAN_SpeedResult.BRP;
		pCAN_Speed->PROPSEG	= g_CAN_SpeedResult.PROPSEG;
		pCAN_Speed->PHSEG1	= g_CAN_SpeedResult.PHSEG1;
		pCAN_Speed->PHSEG2	= g_CAN_SpeedResult.PHSEG2;
		pCAN_Speed->SJW		= g_CAN_SpeedResult.SJW;
	}
	else if(tmp==WAIT_TIMEOUT)
	{//���������� �� �������
		SetEvent(g_hCAN_SpeedMessage);
		return CR_NOANSWER;
	}
	else
	{//�� ����� Windows
		return CR_OTHERERROR;
	}

	return CR_ALLOK;//������������ ���������
}

/***********************   CAN_GetSpeed    ****************************************
*		�������������� �� ������� ��� ����� CAN_GetSpeed(CCAN_Speed *pCAN_Speed)
***********************************************************************************/
CAN_DLL_API CAN_RESULT	CAN_GetSpeed(DWORD* pBaudRate)
{
	if(pBaudRate==NULL) return CR_BADARGUMENTS;

	//������� � ���������������� � ����� ��������� ���������
	CCAN_Speed lCAN_Speed;
	CAN_RESULT lRes=CAN_GetSpeed(&lCAN_Speed);
	if(lRes!=CR_ALLOK)	return lRes;

	*pBaudRate=CAN_CalculateSpeed(&lCAN_Speed);
	return CR_ALLOK;
}

/***********************    CAN_SetSpeed     ****************************************
*		���������� ����� �������� CAN �� ���������������.
************************************************************************************/
CAN_RESULT CAN_SetSpeed(const CCAN_Speed *pCAN_Speed)
{
	if(!CAN_IsMonitorConnected()) return CR_NOTCONNECTED;		//�������� �� �����������
	if(!CAN_CheckSpeedRight(pCAN_Speed)) return CR_BADARGUMENTS;//�������� �����������
	
	static INT8U message[25];
	INT8U i=0; 

	//����� ����� ���������
	message[i++]='$';					//������ ������ �������
	message[i++]=COMPUTER_ID;			//������������� �����������
	message[i++]=0x23;					//Message Type
	message[i++]=0x05;					//Data Length
	//����� ������
	message[i++]=pCAN_Speed->BRP;
	message[i++]=pCAN_Speed->PROPSEG;
	message[i++]=pCAN_Speed->PHSEG1;
	message[i++]=pCAN_Speed->PHSEG2;
	message[i++]=pCAN_Speed->SJW;

	//����� ����������� �����
	INT8U sum=CRC_8_polinom_8541(message,1,i);				//������ ���� � ����� �� ������
	message[i]=sum;
	message[i+1]=0x00;										//�� ������ ������
	
	//�������� ���������
	if(!g_COM_Port.WriteToPort(message,i+1))
	{//���-�� ��� �� ��������
		return CR_OTHERERROR;
	}

	//��������� �� �������� ������ �� �����������
	if(!ResetEvent(g_hSpeedSetOK))
	{
		return CR_OTHERERROR;
	}

	//���� ����� ������
	DWORD tmp=WaitForSingleObject(g_hSpeedSetOK, CAN_GetTimeOut());
	if(tmp==WAIT_OBJECT_0)
	{//���������� �������, ��� � �������
		if(g_SpeedSetResult==0)
		{//�������� CAN �������� �������
			//���� �� ����, ��� ���� ��������
		}
		else
		{//��������������� �� ���� �������� ���� �������� CAN
			return CR_DEVICEBUSY;
		}
	}
	else if(tmp==WAIT_TIMEOUT)
	{//���������� �� �������
		SetEvent(g_hSpeedSetOK);
		return CR_NOANSWER;
	}
	else
	{//�� ����� Windows
		return CR_OTHERERROR;
	}

	return CR_ALLOK;//������������ ���������
}

/**********************    CAN_SetSpeed   *******************************************
*		��� ��������� ����� ������ ������� CAN_SetSpeed(const CCAN_Speed *pCAN_Speed)
************************************************************************************/
CAN_DLL_API CAN_RESULT	CAN_SetSpeed(DWORD BaudRate)
{
	CCAN_Speed lCAN_Speed;
	//��������� ��������
	CAN_RESULT lRes=CAN_SpeedRegFind(BaudRate,&lCAN_Speed);
	if(lRes!=CR_ALLOK)	return lRes;

	//���������� ����� �������� CAN � ���������������
	lRes=CAN_SetSpeed(&lCAN_Speed);
	return lRes;
}

/**********************    CAN_SetRegime    ************************************
*		������������� ����� ������ ���������������. (��. ����� ��������)
*		Regime - ��������� ����� ������. ���� �� ��� �������� 
*					RG_NORMAL, RG_SPEEDFIND, RG_AUTOSEND
*	������������ ��������:
*		���� ��� ������ ���������			- CR_ALLOK
*		����������� Regime					- CR_BADARGUMENTS
*		���� ��������������� �� ���������	- CR_NOTCONNECTED
*		���� ��������������� �� ��������	- CR_NOANSWER
*		������ ���������� ������			- CR_OTHERERROR
**********************************************************************************/
CAN_DLL_API CAN_RESULT CAN_SetRegime(BYTE Regime)
{
	if(Regime!=RG_NORMAL && Regime!=RG_SPEEDFIND && Regime!=RG_AUTOSEND)
									return CR_BADARGUMENTS;	//������������ �����
	if(!CAN_IsMonitorConnected())	return CR_NOTCONNECTED;	//�������� �� �����������

	static INT8U message[15];
	INT8U i=0; 

	//����� ����� ���������
	message[i++]='$';					//������ ������ �������
	message[i++]=COMPUTER_ID;			//������������� �����������
	message[i++]=0x22;					//Message Type
	message[i++]=0x01;					//Data Length
	//����� ������
	message[i++]=Regime;
	//����� ����������� �����
	INT8U sum=CRC_8_polinom_8541(message,1,i);				//������ ���� � ����� �� ������
	message[i]=sum;
	message[i+1]=0x00;										//�� ������ ������
	
	//�������� ���������
	if(!g_COM_Port.WriteToPort(message,i+1))
	{//���-�� ��� �� ��������
		return CR_OTHERERROR;
	}

	//��������� �� �������� ������ �� �����������
	if(!ResetEvent(g_hRegimeSetOK))
	{
		return CR_OTHERERROR;
	}

	//���� ����� ������
	DWORD tmp=WaitForSingleObject(g_hRegimeSetOK, CAN_GetTimeOut());
	if(tmp==WAIT_OBJECT_0)
	{//���������� �������, ��� � �������
		if(g_RegimeSetResult==0)
		{//����� ��������� �������� ��� ������� �������
			//�� ����, ��� ���� ������
		}
		else
		{//��������������� �� ���� ��������� ��������
			return CR_DEVICEBUSY;
		}
	}
	else if(tmp==WAIT_TIMEOUT)
	{//���������� �� �������
		SetEvent(g_hRegimeSetOK);
		return CR_NOANSWER;
	}
	else
	{//�� ����� Windows
		return CR_OTHERERROR;
	}

	return CR_ALLOK;//������������ ���������
}

/************************     CAN_GetRegime     ***********************************
*		���������� ����� ������ ���������������.
*		pRegime	- ��������� �� ����������, ������� ����� ��������� ������� �����.
*	������������ ��������:
*		���� ��� ������ ���������			- CR_ALLOK
*		pRegime==NULL						- CR_BADARGUMENTS
*		���� ��������������� �� ���������	- CR_NOTCONNECTED
*		���� ��������������� �� ��������	- CR_NOANSWER
*		������ ���������� ������			- CR_OTHERERROR
***********************************************************************************/
CAN_DLL_API CAN_RESULT	CAN_GetRegime(BYTE* pRegime)
{
	if(pRegime==NULL)				return false;			//������ ������ ���������
	if(!CAN_IsMonitorConnected())	return CR_NOTCONNECTED;	//�������� �� �����������

	static INT8U message[15];
	INT8U i=0; 

	//����� ����� ���������
	message[i++]='$';					//������ ������ �������
	message[i++]=COMPUTER_ID;			//������������� �����������
	message[i++]=0x16;					//Message Type
	message[i++]=0x00;					//Data Length
	//����� ������
	//����� ����������� �����
	INT8U sum=CRC_8_polinom_8541(message,1,i);				//������ ���� � ����� �� ������
	message[i]=sum;
	message[i+1]=0x00;										//�� ������ ������
	
	//�������� ���������
	if(!g_COM_Port.WriteToPort(message,i+1))
	{//���-�� ��� �� ��������
		return CR_OTHERERROR;
	}

	//��������� �� �������� ������ �� �����������
	if(!ResetEvent(g_hRegimeRequestOK))
	{
		return CR_OTHERERROR;
	}

	//���� ����� ������
	DWORD tmp=WaitForSingleObject(g_hRegimeRequestOK, CAN_GetTimeOut());
	if(tmp==WAIT_OBJECT_0)
	{//���������� �������, ��� � �������
		*pRegime=g_RegimeGetResult;							//��������� �������
	}
	else if(tmp==WAIT_TIMEOUT)
	{//���������� �� �������
		SetEvent(g_hRegimeRequestOK);
		return CR_NOANSWER;
	}
	else
	{//�� ����� Windows
		return CR_OTHERERROR;
	}

	return CR_ALLOK;//������������ ���������
}

/*****************************     CAN_SetFilter     *******************************
*		�������� �������� ������� �������� ��������� �� ���������������.
*		FilterNum			- ����� ������� �� 0 �� 15
*		FilterValue			- �������� �������, ������� ����� ����������
*								�� 0 �� (2^11)-1 ���� ������ �����������
*								�� 0 �� (2^29)-1 ���� ������ �����������
*							���� �������� �������� �� ���������� �����, �� 
*							��� ������ ����������.
*		IsFilterExtended	-	true - ������ ��� ����������� ���������
*								false - ��� �����������
*	������������ ��������:
*		���� ��� ������ ���������			- CR_ALLOK
*		������������ ����� �������			- CR_BADARGUMENTS
*		���� ��������������� �� ���������	- CR_NOTCONNECTED
*		���� ��������������� �� ��������	- CR_NOANSWER
*		������ ���������� ������			- CR_OTHERERROR
************************************************************************************/
CAN_DLL_API CAN_RESULT CAN_SetFilter(BYTE FilterNum, DWORD FilterValue, bool IsFilterExtended)
{
	if(FilterNum>15)				return CR_BADARGUMENTS;	//������������ ����� �������
	if(!CAN_IsMonitorConnected())	return CR_NOTCONNECTED;	//�������� �� �����������

	static INT8U message[15];
	INT8U i=0; 

	//����� ����� ���������
	message[i++]='$';					//������ ������ �������
	message[i++]=COMPUTER_ID;			//������������� �����������
	message[i++]=0x20;					//Message Type
	if(IsFilterExtended)
		message[i++]=0x05;				//����� � ����������� ���������������
	else
		message[i++]=0x03;				//����� � ������� ���������������
	//����� ������
	message[i++]=FilterNum;				//����� �������
	INT8U* mas=(INT8U*)&FilterValue;	//���������
	message[i++]=mas[0];	//|
	message[i++]=mas[1];	//|	��������� ���� ��� ����������� ��������� ����� ������
	if(IsFilterExtended)	
	{//������ ��� ����������� ���������
		message[i++]=mas[2];
		message[i++]=mas[3];
	}
	//����� ����������� �����
	INT8U sum=CRC_8_polinom_8541(message,1,i);				//������ ���� � ����� �� ������
	message[i]=sum;
	message[i+1]=0x00;										//�� ������ ������

	//�������� ���������
	if(!g_COM_Port.WriteToPort(message,i+1))
	{//���-�� ��� �� ��������
		return CR_OTHERERROR;
	}

	//��������� �� �������� ������ �� �����������
	if(!ResetEvent(g_hFilterSetOK))
	{
		return CR_OTHERERROR;
	}

	//���� ����� ������
	DWORD tmp=WaitForSingleObject(g_hFilterSetOK, CAN_GetTimeOut());
	if(tmp==WAIT_OBJECT_0)
	{//���������� �������, ��� � �������
		if(g_FilterSetResult==0)
		{//������ ��� ������� �������
			//�� ����, ��� ���� ������
		}
		else
		{//��������������� �� ���� ��������� ��������
			return CR_DEVICEBUSY;
		}
	}
	else if(tmp==WAIT_TIMEOUT)
	{//���������� �� �������
		SetEvent(g_hFilterSetOK);
		return CR_NOANSWER;
	}
	else
	{//�� ����� Windows
		return CR_OTHERERROR;
	}

	return CR_ALLOK;//������������ ���������
}

/*****************************    CAN_GetFilter     **************************************
*		���������� ��������, ����� � ������������/������������� �������.
*	FilterNum - ����� ������� �� 0 �� 15
*	pFilterValue		- ���� ����� �������� �������� �������
*	pIsFilterExtended	- ���� ����� �������� �������� �� ������ �����������
*							true	- ����������� �������������
*							false	- ����������� �������������
*	pIsFilterEnable		- ���� ����� ��������������� �� ������
*							true	- ��
*							false	- ���
*	pFilterValue,pIsFilterExtended,pIsFilterEnabled �.�. ��������, � ���� ������ � 
*	��������������� ������� ��������� �� �������.
*	������������ ��������:
*		���� ��� ������ ���������			- CR_ALLOK
*		������������ ����� �������			- CR_BADARGUMENTS
*		���� ��������������� �� ���������	- CR_NOTCONNECTED
*		���� ��������������� �� ��������	- CR_NOANSWER
*		������ ���������� ������			- CR_OTHERERROR
*****************************************************************************************/
CAN_DLL_API CAN_RESULT	CAN_GetFilter(BYTE FilterNum, DWORD *pFilterValue, bool* pIsFilterExtended, bool* pIsFilterEnabled)
{
	if(!CAN_IsMonitorConnected()) return CR_NOTCONNECTED;	//�������� �� �����������
	if(FilterNum>15)	return CR_BADARGUMENTS;				//��� ������ �������

	static INT8U message[10];
	INT8U i=0; 

	//����� ����� ���������
	message[i++]='$';					//������ ������ �������
	message[i++]=COMPUTER_ID;			//������������� �����������
	message[i++]=0x14;					//Message Type
	message[i++]=0x01;					//����� ���� ������
	//����� ������
	message[i++]=FilterNum;				//����� �������
	//����� ����������� �����
	INT8U sum=CRC_8_polinom_8541(message,1,i);				//������ ���� � ����� �� ������
	message[i]=sum;
	message[i+1]=0x00;										//�� ������ ������

	//�������� ���������
	if(!g_COM_Port.WriteToPort(message,i+1))
	{//���-�� ��� �� ��������
		return CR_OTHERERROR;
	}

	//��������� �� �������� ������ �� �����������
	if(!ResetEvent(g_hFilterRequestOK))
	{
		return CR_OTHERERROR;
	}

	//���� ����� ������
	DWORD tmp=WaitForSingleObject(g_hFilterRequestOK, CAN_GetTimeOut());
	if(tmp==WAIT_OBJECT_0)
	{//���������� �������, ��� � �������
		//���������� �������
		if(pFilterValue!=NULL)		*pFilterValue		= g_FilterValue;
		if(pIsFilterExtended!=NULL) *pIsFilterExtended	= g_IsExtended;
		if(pIsFilterEnabled!=NULL)	*pIsFilterEnabled	= g_IsEnable;
	}
	else if(tmp==WAIT_TIMEOUT)
	{//���������� �� �������
		SetEvent(g_hFilterRequestOK);
		return CR_NOANSWER;
	}
	else
	{//�� ����� Windows
		return CR_OTHERERROR;
	}

	return CR_ALLOK;//������������ ���������
}


/*****************************    CAN_EnableFilter    ************************************
*		��������/��������� ������ ������� ��������������� ���� CAN �� ���������������
*	FilterNum	- ����� ������� �� 0 �� 15
*	IsEnable	- true  - ������ ����� ��������
*				  false - ������ ����� ���������
*	������������ ��������:
*		���� ��� ������ ���������			- CR_ALLOK
*		������������ ����� �������			- CR_BADARGUMENTS
*		���� ��������������� �� ���������	- CR_NOTCONNECTED
*		���� ��������������� �� ��������	- CR_NOANSWER
*		������ ���������� ������			- CR_OTHERERROR
*****************************************************************************************/
CAN_DLL_API CAN_RESULT	CAN_EnableFilter(BYTE FilterNum, bool IsEnable)
{
	if(FilterNum>15)				return CR_BADARGUMENTS;	//��� ������ �������
	if(!CAN_IsMonitorConnected())	return CR_NOTCONNECTED;	//�������� �� �����������

	static INT8U message[10];
	INT8U i=0; 

	//����� ����� ���������
	message[i++]='$';					//������ ������ �������
	message[i++]=COMPUTER_ID;			//������������� �����������
	message[i++]=0x26;					//Message Type
	message[i++]=0x02;					//����� ���� ������
	//����� ������
	message[i++]=FilterNum;				//����� �������
	message[i++]=IsEnable?1:0;			//�������/��������
	
	//����� ����������� �����
	INT8U sum=CRC_8_polinom_8541(message,1,i);				//������ ���� � ����� �� ������
	message[i]=sum;
	message[i+1]=0x00;										//�� ������ ������

	//�������� ���������
	if(!g_COM_Port.WriteToPort(message,i+1))
	{//���-�� ��� �� ��������
		return CR_OTHERERROR;
	}

	//��������� �� �������� ������ �� �����������
	if(!ResetEvent(g_hFilterEnDisOK))
	{
		return CR_OTHERERROR;
	}

	//���� ����� ������
	DWORD tmp=WaitForSingleObject(g_hFilterEnDisOK, CAN_GetTimeOut());
	if(tmp==WAIT_OBJECT_0)
	{//���������� �������, ��� � �������
		if(g_FilterEnDisResult==0)
		{//������ ��� ������� �������
			//�� ����, ��� ���� ������
		}
		else
		{//��������������� �� ���� ��������� ��������
			return CR_DEVICEBUSY;
		}
	}
	else if(tmp==WAIT_TIMEOUT)
	{//���������� �� �������
		SetEvent(g_hFilterEnDisOK);
		return CR_NOANSWER;
	}
	else
	{//�� ����� Windows
		return CR_OTHERERROR;
	}

	return CR_ALLOK;//������������ ���������
}

/********************    CAN_SetMask    **********************************************
*		�������� �������� ����� ��� �������� �������� ��������� �� ���������������.
*		MaskValue - ����� �������� ����� �� 0 �� (2^29)-1
*					���� �������� ��������� ���������� �������, �� ��� ������ ����������.
*	������������ ��������:
*		���� ��� ������ ���������			- CR_ALLOK
*		���� ��������������� �� ���������	- CR_NOTCONNECTED
*		���� ��������������� �� ��������	- CR_NOANSWER
*		������ ���������� ������			- CR_OTHERERROR
*************************************************************************************/
CAN_DLL_API CAN_RESULT CAN_SetMask(DWORD MaskValue)
{
	if(!CAN_IsMonitorConnected()) return CR_NOTCONNECTED;	//�������� �� �����������

	static INT8U message[15];
	INT8U i=0; 

	//����� ����� ���������
	message[i++]='$';					//������ ������ �������
	message[i++]=COMPUTER_ID;			//������������� �����������
	message[i++]=0x21;					//Message Type
	message[i++]=0x04;					//����� ���� ������
	//����� ������
	INT8U* mas=(INT8U*)&MaskValue;		//���������
	message[i++]=mas[0];				
	message[i++]=mas[1];				
	message[i++]=mas[2];
	message[i++]=mas[3];
	
	//����� ����������� �����
	INT8U sum=CRC_8_polinom_8541(message,1,i);				//������ ���� � ����� �� ������
	message[i]=sum;
	message[i+1]=0x00;										//�� ������ ������

	//�������� ���������
	if(!g_COM_Port.WriteToPort(message,i+1))
	{//���-�� ��� �� ��������
		return CR_OTHERERROR;
	}

	//��������� �� �������� ������ �� �����������
	if(!ResetEvent(g_hMaskSetOK))
	{
		return CR_OTHERERROR;
	}

	//���� ����� ������
	DWORD tmp=WaitForSingleObject(g_hMaskSetOK, CAN_GetTimeOut());
	if(tmp==WAIT_OBJECT_0)
	{//���������� �������, ��� � �������
		if(g_MaskSetResult==0)
		{//������ ��� ������� �������
			//�� ����, ��� ���� ������
		}
		else
		{//��������������� �� ���� ��������� ��������
			return CR_DEVICEBUSY;
		}
	}
	else if(tmp==WAIT_TIMEOUT)
	{//���������� �� �������
		SetEvent(g_hMaskSetOK);
		return CR_NOANSWER;
	}
	else
	{//�� ����� Windows
		return CR_OTHERERROR;
	}

	return CR_ALLOK;
}

/***********************     CAN_GetMask     ****************************************
*		���������� �������� ����� ���� CAN �� ����������������.
*		pMaskValue - ��������� �� ����������, ������� ����� ��������� �������� �����
*	������������ ��������:
*		���� ��� ������ ���������			- CR_ALLOK
*		pMaskValue == NULL					- CR_BADARGUMENTS
*		���� ��������������� �� ���������	- CR_NOTCONNECTED
*		���� ��������������� �� ��������	- CR_NOANSWER
*		������ ���������� ������			- CR_OTHERERROR
************************************************************************************/
CAN_DLL_API CAN_RESULT	CAN_GetMask(DWORD* pMaskValue)
{
	if(pMaskValue==NULL)			return CR_BADARGUMENTS;	//������ ������ ���������
	if(!CAN_IsMonitorConnected())	return CR_NOTCONNECTED;	//�������� �� �����������

	static INT8U message[10];
	INT8U i=0; 

	//����� ����� ���������
	message[i++]='$';					//������ ������ �������
	message[i++]=COMPUTER_ID;			//������������� �����������
	message[i++]=0x15;					//Message Type
	message[i++]=0x00;					//����� ���� ������
	//����� ������
	//����� ����������� �����
	INT8U sum=CRC_8_polinom_8541(message,1,i);				//������ ���� � ����� �� ������
	message[i]=sum;
	message[i+1]=0x00;										//�� ������ ������

	//�������� ���������
	if(!g_COM_Port.WriteToPort(message,i+1))
	{//���-�� ��� �� ��������
		return CR_OTHERERROR;
	}

	//��������� �� �������� ������ �� �����������
	if(!ResetEvent(g_hMaskRequestOK))
	{
		return CR_OTHERERROR;
	}

	//���� ����� ������
	DWORD tmp=WaitForSingleObject(g_hMaskRequestOK, CAN_GetTimeOut());
	if(tmp==WAIT_OBJECT_0)
	{//���������� �������, ��� � �������
		*pMaskValue=g_MaskValue;						//���������� �������
	}
	else if(tmp==WAIT_TIMEOUT)
	{//���������� �� �������
		SetEvent(g_hMaskRequestOK);
		return CR_NOANSWER;
	}
	else
	{//�� ����� Windows
		return CR_OTHERERROR;
	}

	return CR_ALLOK;//������������ ���������
}

/***********************    CAN_Supervisor     ***************************************
*		��� ������� ����� ��������� ��������� �� Serial Port � 
*	����������, ��� ������ ������. !������� ����������� � ������ Serial Port � 
*	����� ���������� �������!.
*************************************************************************************/
void CAN_Supervisor(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
	case WM_COMM_RXCHAR:
		{//������ ������
			ReceiveChar((INT8U)wParam);
			break;
		}
	case WM_COMM_ACCESS_DENIED:
		{//������ � ����� ������
			//������ ��������������� ��������� �� USB �������
			g_COM_Port.StopMonitoring();	//��������� �������� � ������
			g_bConnected=false;				//���� ������� ���������� �����������
			break;
		}
	case WM_COMM_TXEMPTY_DETECTED:
		{//��������� ����������
			break;
		}
	}
}

/*************************   ReceiveChar   ************************************
*	���������� ����������  ������, ��������� ���-�����, ������������ �����,
*	��������� ������� � ������ ������ ������. ������ ������������ ����.
*	���� ��� ������� ������� ����� ��� ������ - ����� �������������, ��� �����
*	������������� ������� ���������. � ��� ���������� �� ������ ���� ������
*	������ ������� CAN_RS232_Verify()
*	ch - ��������� �������� ������
********************************************************************************/
bool ReceiveChar(INT8U ch)
{
	const INT16U	lMAX_LEN=50;						//������������ ����� ���������
	static INT8U	ReceiveMessage[lMAX_LEN];			//����� ������������ ���������
	const  INT8U	lLEN_ADD		= 0x05;
	const  INT8U	lFIRST_BYTE		= 0x24;
	const  INT8U	lLEN_STATE		= 0x03;
	static INT8U	pred_char		= 0;				//���������� ������
	static INT16U	char_count		= 0;				//���������� �������� ��������
	
	char_count++;										//������� ��� ���� ������
	
	/*
	�������� � ������������� ����� �����, ������ ��������� ����� ���� ����� �������,
	����� ������������� ����� ������ (�� �����).
	*/

	static bool verifi_on=0;			//���� 1, �� ����� ��������� � ������������
										//� ������ �����
	
	static bool FirstByteReady=false;	//��������, ��������� ��� �� ���������� (�� ����� ����
										//������, � � ����� �������� ����� ������ ������.
										//���� ���� �������� ����������� ��������� ������ ������
										//���������

	static INT16U paket_length=0;		//����� ���������� ����� ������������� ����� ������
	static INT16U input_count=0;		//���������� �������� ����
	
	INT8U RC_register=ch;				//������ ���

	//��������� ����� ������ ������ �������� ��������� ������, �� �� ������������ � ����
	static bool Chepuha=true;		//����� ���������� ������ ��������� ��������� ��� �� ������ �� ���� ��������� ������
	static bool LenCorrect=false;	//�������� ���� ����� ����� ���������� ��������	
	static bool TheEnd=false;		//������ ��� ������� ����� ���������, ��� ����� ���������� �� ������
	static bool SumCorrect=false;

	if(input_count>lMAX_LEN)		//������������ ������, � �� ���� ������� ����
	{//������ �������
		input_count=0;
	}

	if(TheEnd)
	{//� ������� ��� ��������� ��������� ���������, � � ���� �������� ������� � ���������� ������
		TheEnd=false;
		Chepuha=true;
	}

	//������� ��������� �������� ������
	ReceiveMessage[input_count]=RC_register;	//���������� � ����� ������
	input_count++;								//�������� ���������� �������� ��������

	//���� ����� �� �������, � ��� �� ����������� $ (�����, �����, ������)
	if(RC_register==lFIRST_BYTE && FirstByteReady==false)
	{
		input_count=0;			//������ ����� ��������� - �������� �������
		ReceiveMessage[0]=RC_register;	//������� ���� ������ ��� �� ���� �����
		input_count=1;			//������ ����� ��������� - �������� �������
		FirstByteReady=true;	//������ ������ ���� ��� �������

		Chepuha=false;		//��� ��� ����������� �����
		LenCorrect=false;	//���� ��� ������ �� ����� � �����
		SumCorrect=false;	//����������� ����� ��� �� �������
	}
	else if(Chepuha==false)
	{//����� �������
		if(input_count==lLEN_STATE+1)//��������� �������� ������
		{//����� ��� ���������, � �� ���������, � ���� ����� ����� ����� ������ � ������
			if(RC_register>lMAX_LEN-lLEN_ADD)
			{//���� ����� ������� ������� �������
				paket_length=lMAX_LEN;
				FirstByteReady=false;	//������ �������� ����� �������
				MyTRACE("CAN_DLL ���� ����� � ������������� �� RS232 ��������� ������� �������");
			}
			else
			{
				//��������, ����� ���-�� �������� ��� ������� �������
				paket_length=RC_register+lLEN_ADD;

				LenCorrect=true;	//���� ����� ���� ���������
			}
		}
		else if(input_count==paket_length)
		{//��������� ������� �������, ���� ��������� ����������� �����
			if(LenCorrect)
			{//���� ����� ���� ���������, �� ����� ����������� � �������������������� �����
				unsigned char sim=ch;
				unsigned char sim2=CRC_8_polinom_8541(ReceiveMessage,1,input_count-1);
				if(sim!=sim2)
				{//������ � ����������� �����
					MyTRACE("CAN_DLL ������ ����������� ����� � ������������� �� RS232 ���������");
				}
				else
				{//����������� ����� �����
					SumCorrect=true;
					CAN_RS232_Verify(ReceiveMessage, input_count);
				}
			}
			FirstByteReady=false;	//����� ������ ��������� ����� ���������

			TheEnd=true;			//�������� ���� ����� ���������
		}
	}
	else
	{
 		MyTRACE("CAN_DLL ����� ����������� �� RS-232 ������� ����� ������");
	}
	
	pred_char=ch; //����� ����� ��� ���������� ������
	return true;
}

/***********************    CAN_RS232_Verify    ***************************************
*		���������� ���������� �� RS-232 ���������. � � ������������ � ���� ����������
*	��������������� �������.
**************************************************************************************/
bool CAN_RS232_Verify(BYTE *pReceiveMessage, WORD MesageLength)
{
	if(pReceiveMessage==NULL || MesageLength==0)
	{//������������ ���������
		return false;
	}

	const INT8U MT_STATE=0x02;							//���� ���� ���������
	const INT8U lMesType=pReceiveMessage[MT_STATE];		//��� ��������� ���������
	const INT8U ID_STATE=0x01;							//���� �������������� �����������
	const INT8U lMesID=pReceiveMessage[ID_STATE];		//������������� �����������
	const INT8U DAT_STATE=0x04;							//������ ���� ������
	const INT8U LEN_STATE=0x03;							//��������� ����� ���� ������
	const INT8U lDatLen=pReceiveMessage[LEN_STATE];		//����� ���� ������

	if(lMesID==COMPUTER_ID)
	{/*��� ��������� � �������� ��� ����*/
	}
	else if (lMesID==CONTROLLER_ID)
	{/*��� ��������� �������� ����������*/
		if(lMesType==0x01)
		{//������� ��������� CAN ���������
			const INT8U lcMasLength=100;							//|
			static INT8U lsMasCounter=0;							//|
			static CCAN_Message ReceivedCAN_MessageMas[lcMasLength];//| ��. ��������� ����
			//�������� �������������
			ReceivedCAN_MessageMas[lsMasCounter].ID=*((DWORD *)&pReceiveMessage[DAT_STATE]);	
			//���������� ���� ������
			ReceivedCAN_MessageMas[lsMasCounter].DataLen=pReceiveMessage[DAT_STATE+4];
			//�������� �����
			ReceivedCAN_MessageMas[lsMasCounter].Flags=pReceiveMessage[DAT_STATE+5];
			//�������� ���� ������
			INT8U i=0;
			for(; i<ReceivedCAN_MessageMas[lsMasCounter].DataLen; i++)
			{
				ReceivedCAN_MessageMas[lsMasCounter].Data[i]=pReceiveMessage[DAT_STATE+6+i];
			}
			//������ ��������� ������
			for(; i<8;i++) ReceivedCAN_MessageMas[lsMasCounter].Data[i]=0;

			if(g_MesReceiveWindow!=INVALID_HANDLE_VALUE)
			{//������ ������� ����������
				//����� � ����� PostMessage, � � ��� ����� ��������
				//��� ���� ���������, �� ��� �������� ����������� ���������
				//ReceivedCAN_Message. SendMessage ������ ����� ������, �.�.
				//����� ����� COM ����� ������� � ����� ���� � �������� ����!
				//������� � ������ ����� ����������� ������, ������� �����
				//����������� �� �����. � �������, ������� ��������� ����� ��������
				//�������� � �������������� ���������, ���� � �� ����� �� �����.
				PostMessage(g_MesReceiveWindow,MSG_NEW_RECEIVE,(WPARAM)&ReceivedCAN_MessageMas[lsMasCounter],0);
				if(lsMasCounter>=lcMasLength)
					lsMasCounter=0;
				else
					lsMasCounter++;
			}
			else
			{//����� ������������ �������� ���������
				return false;
			}
		}
		else if(lMesType==0x10)
		{//���������� ������ ��� ������
			if(WaitForSingleObject(g_hg_ReceiveErrorsCountMes,0)!=WAIT_TIMEOUT)
				MessageBox(NULL,_TEXT("��������������� ������� ��������� � ����������� ������ �� ������ ��� �������"),_TEXT("������ ���������������"), MB_ICONWARNING);
			g_ReceiveErrorsCount=pReceiveMessage[DAT_STATE];
			if(!SetEvent(g_hg_ReceiveErrorsCountMes)) return false;
		}
		else if(lMesType==0x11)
		{//���������� ������ ��� ��������
			if(WaitForSingleObject(g_hg_TranceiveErrorsCountMes,0)!=WAIT_TIMEOUT)
				MessageBox(NULL,_TEXT("��������������� ������� ��������� � ����������� ������ �� �������� ��� �������"),_TEXT("������ ���������������"), MB_ICONWARNING);
			g_TranceiveErrorsCount=pReceiveMessage[4];
			if(!SetEvent(g_hg_TranceiveErrorsCountMes)) return false;
		}
		else if(lMesType==0x12)
		{//��������� BUS-OFF
			if(WaitForSingleObject(g_hBusOffMessage,0)!=WAIT_TIMEOUT)
				MessageBox(NULL,_TEXT("��������������� ������� ��������� � ���������� Bus-Off ��� �������"),_TEXT("������ ���������������"), MB_ICONWARNING);
			if(pReceiveMessage[4])
				g_Bus_Off=true;			//"��������� �� ����"
			else
				g_Bus_Off=false;			//� �������� ������
			if(!SetEvent(g_hBusOffMessage)) return false;

		}
		else if(lMesType==0x13)
		{//���������� � �������� CAN
			if(WaitForSingleObject(g_hCAN_SpeedMessage,0)!=WAIT_TIMEOUT)
				MessageBox(NULL,_TEXT("��������������� ������� ��������� �� ��������� CAN ��� �������"),_TEXT("������ ���������������"), MB_ICONWARNING);
			g_CAN_SpeedResult.BRP		= pReceiveMessage[DAT_STATE];
			g_CAN_SpeedResult.PROPSEG	= pReceiveMessage[DAT_STATE+1];
			g_CAN_SpeedResult.PHSEG1	= pReceiveMessage[DAT_STATE+2];
			g_CAN_SpeedResult.PHSEG2	= pReceiveMessage[DAT_STATE+3];
			g_CAN_SpeedResult.SJW		= pReceiveMessage[DAT_STATE+4];
			
			if(!SetEvent(g_hCAN_SpeedMessage)) return false;
		}
		else if(lMesType==0x14)
		{//���������� ������� �������� �������
			if(WaitForSingleObject(g_hFilterRequestOK,0)!=WAIT_TIMEOUT)
				MessageBox(NULL,_TEXT("��������������� ������� ��������� �� ��������� ������� ��� �������"),_TEXT("������ ���������������"), MB_ICONWARNING);
			if(lDatLen==4)
			{//� ������� 2 �����, ������ ������������� �����������
				g_IsExtended=false;
				g_FilterValue=*((INT16U*)&pReceiveMessage[DAT_STATE+2]);
			}
			else if(lDatLen==6)
			{//� ������� 4 �����, ������ ������������� �����������
				g_IsExtended=true;
				g_FilterValue=*((INT32U*)&pReceiveMessage[DAT_STATE+2]);
			}
			else
				MessageBox(NULL,_TEXT("�������� ������ ��������� �� ��������� �������"),_TEXT("������ ���������"), MB_ICONWARNING);
			g_IsEnable=pReceiveMessage[DAT_STATE+1]==0?false:true;	//������� �� ������?
			if(!SetEvent(g_hFilterRequestOK)) return false;
		}
		else if(lMesType==0x15)
		{//���������� ������� �������� �����
			if(WaitForSingleObject(g_hMaskRequestOK,0)!=WAIT_TIMEOUT)
				MessageBox(NULL,_TEXT("��������������� ������� ��������� �� ��������� ����� ��� �������"),_TEXT("������ ���������������"), MB_ICONWARNING);
			g_MaskValue=*((INT32U*)&pReceiveMessage[DAT_STATE]);
			if(!SetEvent(g_hMaskRequestOK)) return false;

		}
		else if(lMesType==0x16)
		{//���������� ������� ���� ����� ������
			if(WaitForSingleObject(g_hRegimeRequestOK,0)!=WAIT_TIMEOUT)
				MessageBox(NULL,_TEXT("��������������� ������� ��������� � ������� ������ ��� �������"),_TEXT("������ ���������������"), MB_ICONWARNING);
			g_RegimeGetResult=pReceiveMessage[DAT_STATE];
			if(!SetEvent(g_hRegimeRequestOK)) return false;
		}
		else if(lMesType==0x20)
		{//������������� ��������� �������
			if(WaitForSingleObject(g_hFilterSetOK,0)!=WAIT_TIMEOUT)
				MessageBox(NULL,_TEXT("��������������� ������� ��������� � ������������� ��������� ������� CAN ��� �������"),_TEXT("������ ���������������"), MB_ICONWARNING);
			g_FilterSetResult=pReceiveMessage[DAT_STATE];
			if(!SetEvent(g_hFilterSetOK)) return false;
		}
		else if(lMesType==0x21)
		{//������������� ��������� �����
			if(WaitForSingleObject(g_hMaskSetOK,0)!=WAIT_TIMEOUT)
				MessageBox(NULL,_TEXT("��������������� ������� ��������� � ������������� ��������� ����� CAN ��� �������"),_TEXT("������ ���������������"), MB_ICONWARNING);
			g_MaskSetResult=pReceiveMessage[DAT_STATE];
			if(!SetEvent(g_hMaskSetOK)) return false;
		}
		else if(lMesType==0x22)
		{//������������� ��������� ������ ������ ���������������
			if(WaitForSingleObject(g_hRegimeSetOK,0)!=WAIT_TIMEOUT)
				MessageBox(NULL,_TEXT("��������������� ������� ��������� � ������������� ��������� ������ ��������� �������� CAN ��� �������"),_TEXT("������ ���������������"), MB_ICONWARNING);
			g_RegimeSetResult=pReceiveMessage[DAT_STATE];
			if(!SetEvent(g_hRegimeSetOK)) return false;
		}
		else if(lMesType==0x23)
		{//������������� ��������� �������� CAN
			if(WaitForSingleObject(g_hSpeedSetOK,0)!=WAIT_TIMEOUT)
				MessageBox(NULL,_TEXT("��������������� ������� ��������� � ������������� ��������� �������� CAN ��� �������"),_TEXT("������ ���������������"), MB_ICONWARNING);
			g_SpeedSetResult=pReceiveMessage[DAT_STATE];
			if(!SetEvent(g_hSpeedSetOK)) return false;
		}
		else if(lMesType==0x25)
		{//CAN ��������� ����������
			if(WaitForSingleObject(g_hCAN_MesSended,0)!=WAIT_TIMEOUT)
			{
				//++++ MessageBox(NULL,_TEXT("��������������� ������� �� �������� CAN ��������� ��� �������"),_TEXT("������ ���������������"), MB_ICONWARNING);
				//�� ���� ���������� MessageBox, �.�. ����� �������� ��������, ���� ��� ������ 2 ����� � ���� ���������� ����� �� �����������
				//� ��������� �� �� ����� �������� ����������� �����������, ��� �� ������, ������� �������� ���������, � ��� �� �����
				//�����������
				PostMessage(g_MesReceiveWindow,MSG_NEW_RECEIVE,(WPARAM)NULL,CTF_UNCOWN_CONFIRM);
			}
			g_SendMesCAN_Result=pReceiveMessage[DAT_STATE];
			if(!SetEvent(g_hCAN_MesSended)) return false;
		}
		else if(lMesType==0x26)
		{//������������� ���������/���������� �������
			if(WaitForSingleObject(g_hFilterEnDisOK,0)!=WAIT_TIMEOUT)
				MessageBox(NULL,_TEXT("��������������� ������� ��������� � ������������� ���������//���������� ������� ��� �������"),_TEXT("������ ���������������"), MB_ICONWARNING);
			g_FilterEnDisResult=pReceiveMessage[DAT_STATE];
			if(!SetEvent(g_hFilterEnDisOK)) return false;
		}
		else
		{//����������� �����
			return false;
		}
		
	}//����� ��������� �� �����������
	else
	{//����������� �����������
		return false;
	}

	return true;
}
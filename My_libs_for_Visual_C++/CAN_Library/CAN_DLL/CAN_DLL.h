#ifdef CAN_DLL_EXPORTS
#define CAN_DLL_API __declspec(dllexport)
#else
#define CAN_DLL_API __declspec(dllimport)
#endif

/******************************************************************************
*						���� ����������� ���������
*	06.04.2009 - ������ ���� � ���������������� ��������� �����-�� �������, 
*		�� ����-�������� ������������ ��������� � ����� ����� �������.
******************************************************************************/

// ���������, ������������ ��������� CAN
struct CAN_DLL_API CCAN_Message 
{
	DWORD ID;				// ������������� CAN ���������
 
//����� ��� ��� ��������, ��� � ��� ������
#define CF_XTD_FRAME		0x20 // 0b00100000 ����������� �������������
#define CF_RTR_FRAME		0x40 // 0b01000000 ��������� ������ (��� ���� ������)

//�����, ������������ ������ ��� ������
#define CF_RX_DBL_BUFFERED  0x80 // 0b10000000 ������� �����������
#define CF_RX_OVERFLOW      0x08 // 0b00001000 ������������ ��������� ������
#define CF_RX_INVALID_MSG   0x10 // 0b00010000 �������� ���������
	BYTE Flags;				// �������������� �����
	BYTE DataLen;			// ���������� ���� ������
	BYTE Data[8];			// ������
};

//���������� ������ �������
typedef unsigned char CAN_RESULT;
#define CR_ALLOK			0x00	//�������� ��������� �������
#define CR_BADARGUMENTS		0x01	//� ������� �������� ������������ ���������
#define CR_NOANSWER			0x02	//��������������� �� ��������
#define CR_OTHERERROR		0x03	//������ ������
#define CR_NOTCONNECTED		0x04	//��������������� �� ��������� � ����������
#define CR_DEVICEBUSY		0x05	//��������������� �������, ��� �� ���� ���������
									//����������� ��������

//������ ������ ���������������
#define RG_NOTINITIALIZED	0x00	//������ � ���������������
#define RG_NORMAL			0x01	//���������� ����� ������
#define RG_SPEEDFIND		0x02	//��������� �������� CAN
#define RG_AUTOSEND			0x03	//������������ ������� (��� ������� PC)

/*******************************************************************************
*		��� ��������� ���������� CAN ���������, ���� �� ������ ������� �������,
*	��������� �������� ����, ����� �������� ����� ��� ����� �������� CAN_SetMesReceiveWindow, ���������.
*******************************************************************************/
#define MSG_NEW_RECEIVE		(WM_USER+26)	//��� ��������� ������������, ����� ��������� ��������� ������
//���� WPARAM==NULL, �� LPARAM ���������� �������
#define CTF_STATE_CHANGED	0x00			//���������� ��������� ���������/�������� ��������������� CAN-USB
#define CTF_UNCOWN_CONFIRM	0x01			//��������������� �������� ������������� �������� ���������������� ���������


//�������� ������� ��������� � ����� ABOUT_CAN_DLL.TXT
CAN_DLL_API CAN_RESULT	CAN_SetMesReceiveWindow(HWND hWnd);
CAN_DLL_API bool		CAN_IsMonitorConnected();
CAN_DLL_API CAN_RESULT	CAN_SetTimeOut(DWORD time_ms);
CAN_DLL_API DWORD		CAN_GetTimeOut(void);

CAN_DLL_API CAN_RESULT	CAN_GetReceiveErrorsCount(unsigned char* pCount);
CAN_DLL_API CAN_RESULT	CAN_GetTranceiveErrorsCount(unsigned char* pCount);
CAN_DLL_API CAN_RESULT	CAN_GetBusOff(bool *pBusOff);

CAN_DLL_API CAN_RESULT	CAN_CalcActualSpeed(DWORD BaudRate, float* pError=NULL, DWORD* pActualSpeed=NULL);
CAN_DLL_API CAN_RESULT	CAN_GetSpeed(DWORD* pBaudRate);
CAN_DLL_API CAN_RESULT	CAN_SetSpeed(DWORD BaudRate); 

CAN_DLL_API CAN_RESULT	CAN_SetRegime(BYTE Regime);
CAN_DLL_API CAN_RESULT	CAN_GetRegime(BYTE* pRegime);
CAN_DLL_API CAN_RESULT	CAN_SendMessage(CCAN_Message* pCAN_Message);

CAN_DLL_API CAN_RESULT	CAN_SetFilter(BYTE FilterNum, DWORD FilterValue, bool IsFilterExtended=false);
CAN_DLL_API CAN_RESULT	CAN_EnableFilter(BYTE FilterNum, bool IsEnable);
CAN_DLL_API CAN_RESULT	CAN_GetFilter(BYTE FilterNum, DWORD *pFilterValue, bool* pIsFilterExtended, bool* pIsFilterEnabled);

CAN_DLL_API CAN_RESULT	CAN_SetMask(DWORD MaskValue);
CAN_DLL_API CAN_RESULT	CAN_GetMask(DWORD* pMaskValue);

CAN_DLL_API CAN_RESULT	CAN_COM_Open(unsigned char port_num);
CAN_DLL_API bool		CAN_IsCOM_Opened(void);

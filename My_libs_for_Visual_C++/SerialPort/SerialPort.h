/*************************************************************************************
***************************************************************************************
**		���� ������ �������� � ���������������� ������ �� ����� ������ ������.
**	�� ���������� ������� ������ ��� ������.
*****************************************************************************************
***************************************************************************************/

/********************	���� ����������� ���������	*********************************
*		12.10,2006 - + ������ ���� ���� ���� ���������� � ��������� � ����� ����� ��� 
*	���� ��������� ����������
*		26.10.2006 - + ��������� ������������ ����, �� �������� ������� EV_TXEMPTY
*	��������� ������ ����� ������ ������-���� �������. ������� ������������ ��������� 
*	������ ��������� ������ ��� ������.
*		30.10.2006 - + ��������� ���� � ������������� �� ����� ����, ����� ���������� ������ 
*	�������.
*					+ ��������� ��������� � ����������
*		22.01.2007 - ������ ��������� warningi
*		14.02.2007 - ��������� ������������ ������ ERROR_OPERATION_ABORTED
*		14.04.2007 - � ����� �� ������� ���������� ���� COMSTAT ��������� �����������,
*						����� ���������� ������
*		24.04.2007 - ���������� ���������!!! ������� ����� ���� ������� � ������, ���� ������
*					������������� ������� ������������ ������ � ���������� ����������
*		14.09.2007 - ������������� ����������� ������ ���� � �����������, �.�. �� ��������
*						��������� � �����������, ��� ��������� ��������� ���������, ���� 
*						InitPort �� ���� �������
*		29.02.2008 -  � ������� CreateFile ������� �������� FILE_ATTRIBUTE_NORMAL
*		14.04.2008 - ���� �� MFC � ������� Windows API, ��������� ����������� �� 
*						���������� ��������� �������� ����, � ������������ ���������
*						�� ������� �������������� �������.
*		17.04.2008 - �������� ������� WriteToPort
*		3.08.2008	- ��������� ������� ClosePort
*		01.09.2008 - ��������� ���������� ������� ReceiveChar, ������� ����� ��������� �����
*					�����, � �� �� ������ �����, ��-�� ���� ������ ����������� ������ � 
*					CP2102
*		6.10.2008 - ���������� ������ � ������� ReceivePortLoLevel
*		08.04.2009 - � ������� ClosePort ��� �������� ������ ��������� �������� � 100 ��,
*					����� ����� ���� ��������� ����������������� �� ��� �� ����� �����.
*************************************************************************************/
#pragma once

// ��������� �������� � �������� �� �����
#define	WM_COMM_BREAK_DETECTED			(WM_USER + 1)
#define	WM_COMM_CTS_DETECTED			(WM_USER + 2)
#define	WM_COMM_DSR_DETECTED			(WM_USER + 3)
#define	WM_COMM_ERR_DETECTED			(WM_USER + 4)
#define	WM_COMM_EVENT1_DETECTED			(WM_USER + 5)
#define	WM_COMM_EVENT2_DETECTED			(WM_USER + 6)
#define	WM_COMM_PERR_DETECTED			(WM_USER + 7)
#define	WM_COMM_RING_DETECTED			(WM_USER + 8)
#define	WM_COMM_RLSD_DETECTED			(WM_USER + 9)
#define	WM_COMM_RX80FULL_DETECTED		(WM_USER + 10)
#define	WM_COMM_RXFLAG_DETECTED			(WM_USER + 11)
#define	WM_COMM_TXEMPTY_DETECTED		(WM_USER + 12)
#define	WM_COMM_RXCHAR					(WM_USER + 13)
#define WM_COMM_ACCESS_DENIED			(WM_USER + 14)	//������ � ����� �������� (���������� ��� �����)
#define WM_COMM_UNCOWN_ERROR			(WM_USER + 15)	//����������� ������

#define PURGE_FLAGS             PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR 
#define PURGE_RX				PURGE_RXABORT | PURGE_RXCLEAR 
#define PURGE_TX				PURGE_TXABORT | PURGE_TXCLEAR 
#define EVENTFLAGS_DEFAULT      EV_BREAK | EV_CTS | EV_DSR | EV_ERR | EV_RING | EV_RLSD
#define FLAGCHAR_DEFAULT        '\n'

// ������� ����������� ������� ������
DWORD WINAPI CommThread(PVOID pParam);

class CSerialPort
{														 
public:
	//����������� � ����������
	CSerialPort();
	virtual		~CSerialPort();

	//������������� �����
	BOOL		InitPort(HWND ParentHWND, UINT portnr = 1, UINT baud = 19200, char parity = 'N', UINT databits = 8, UINT stopbits = 2, DWORD dwCommEvents = EV_RXCHAR | EV_TXEMPTY | EV_ERR, UINT writebuffersize = 512);
	//��������� ������ � ������
	void		ClosePort();
	//������� �����������
	bool		SetParentSupervisor(void (*parent_supervisor)(UINT Msg, WPARAM wParam, LPARAM lParam));

	void ClearRXBuff();
	void ClearTXBuff();
	void SetRTS(bool rts);

	// ������ ������ ������ ���������� �� ������
	BOOL		StartMonitoring();		//������������ ������ ���� ��� ��� ������ �����
	// ��������������� ���������� �� ������
	BOOL		StopMonitoring();		//��������� ������ ����������
	// ������������� ���������� �� ������� ������
	BOOL		ContinueMonitoring();

	// ������ ������� ������� ������
	DWORD		GetWriteBufferSize();
	// ������ ������� ������������� �����
	DWORD		GetCommEvents();
	// 
	DCB			GetDCB();

	//���������� ������� �������� ������
	bool		WriteToPort(const BYTE* pBufToSend, DWORD NumBytes);

	// ����� �����
	UINT				m_nPortNr;
	// ������ ����� (�����)
	HANDLE				m_hComm;
protected:

	// ���������� �����-�������
	void		ProcessErrorMessage(TCHAR* ErrorText);

	// ����������� ������� ������ ������� (�����)
	static bool	ReceivePortLoLevel(CSerialPort* port);
	// ����������� ������� �������� ������� (�����)
	static bool	WritePortLoLevel(CSerialPort* port);

	// �����
	HANDLE				m_hThread;

	// ������� �������������
	// ����������� ������
//	CRITICAL_SECTION	m_csCommunicationSync;
	// ���� ������������� ������
	BOOL				m_bThreadAlive;
	// ���� �������� ������
	BOOL				m_bThreadSuspended;

	// ������ �� �������
	// ������ ������� �������� ������
	HANDLE				m_hShutdownEvent;

	// ������ ������� ������
	HANDLE				m_hWriteEvent;

	// ������ �������. 
	// ������ ������� ������� �������� ����� �������.
	// 0 - ������� ������� ������ - ������� ������ ��� �������� �����.
	// ���� ���������� ��� ������ �������:
	// 1 - ������� ��� ��������� �������, ������� ����������� � ����������� ��������� (m_ov.hEvent).
	// 2 - ������� ������ m_hWriteEvent 
	HANDLE				m_hEventArray[3];

	// ���������
	// ����������� ����/�����
	// ��� ��������� ������ ������ ���� ���������������� ����� ����� �������������� � �������.
	// ����� ������� �������� ������ � ����� �������� ERROR_INVALID_PARAMETER
public:
	OVERLAPPED			m_ov;
	// ��������� ����-�����
	COMMTIMEOUTS		m_CommTimeouts;
	// ����������� ��������� �����
	DCB					m_dcb;

	//����� ������������� ����
	HWND				m_ParentHWND;
	//��������� �� ���������� �������� (���� ��� �� ����)
	void (*ParentSupervisor)(UINT Msg, WPARAM wParam, LPARAM lParam);

	//���������� ��������, ���� ������� ��� ������������� �������, ���� 
	//������� ��� ���������
	void NotifyParent(UINT Msg, WPARAM wParam, LPARAM lParam);

	// ������

	// ������ ��������
	BYTE *				m_szWriteBuffer;
	// ������������ ������ ������� ��������
	DWORD				m_nWriteBufferSize;
	// ����� ������� �����
	DWORD				m_dwCommEvents;
	// ���������� ����, ������� ���������� �������� �� ������� �������� �������� WriteFile
	DWORD				dwBytesToWrite;

	//������� ������ ������ ���� �������� ��� �����������
	friend DWORD WINAPI CommThread(PVOID pParam);

};




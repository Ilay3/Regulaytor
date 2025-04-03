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
*************************************************************************************/
#pragma once

// ��������� �������� � �������� ��������� ��� ��������� �����.
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

#define PURGE_FLAGS             PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR 
#define PURGE_RX				PURGE_RXABORT | PURGE_RXCLEAR 
#define PURGE_TX				PURGE_TXABORT | PURGE_TXCLEAR 
#define EVENTFLAGS_DEFAULT      EV_BREAK | EV_CTS | EV_DSR | EV_ERR | EV_RING | EV_RLSD
#define FLAGCHAR_DEFAULT        '\n'

class CSerialPort
{														 
public:
	//����������� � ����������
	CSerialPort();
	virtual		~CSerialPort();

	//������������� �����
	BOOL		InitPort(CWnd* pPortOwner, UINT portnr = 1, UINT baud = 19200, char parity = 'N', UINT databits = 8, UINT stopsbits = 2, DWORD dwCommEvents = EV_RXCHAR | EV_TXEMPTY | EV_ERR, UINT nBufferSize = 512);
	void ClearRXBuff();
	void ClearTXBuff();
	void SetRTS(bool rts);

	// ������ ������ ������ ���������� �� ������
	BOOL		StartMonitoring();		//������������ ������ ���� ��� ��� ������ �����
	// ��������������� ���������� �� ������
	BOOL		StopMonitoring();		//��������� ������ ����������
	// ������������� ���������� �� ������� ������
	BOOL		RestartMonitoring();

	// ������ ������� ������� ������
	DWORD		GetWriteBufferSize();
	// ������ ������� ������������� �����
	DWORD		GetCommEvents();
	// 
	DCB			GetDCB();

	void		WriteToPort(unsigned char* string, unsigned int num);

	// ����� �����
	UINT				m_nPortNr;
	// ������ ����� (�����)
	HANDLE				m_hComm;
protected:

	// ���������� �����-�������
	void		ProcessErrorMessage(char* ErrorText);

	// ������� ����������� ������� ������
	static UINT	CommThread(LPVOID pParam);
	// ����������� ������� ������ ������� (�����)
	static void	ReceiveChar(CSerialPort* port, COMSTAT comstat);
	// ����������� ������� �������� ������� (�����)
	static void	WriteChar(CSerialPort* port);

	// �����
	CWinThread*			m_Thread;

	// ������� �������������
	// ����������� ������
	CRITICAL_SECTION	m_csCommunicationSync;
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
	OVERLAPPED			m_ov;
	// ��������� ����-�����
	COMMTIMEOUTS		m_CommTimeouts;
	// ����������� ��������� �����
	DCB					m_dcb;

	// ��������� �� ������������ ����� ��� ����
	CWnd*		m_pOwner;

	// ������

	// ������ ��������
	unsigned char *		m_szWriteBuffer;
	// ������������ ������ ������� ��������
	DWORD				m_nWriteBufferSize;
	// ����� ������� �����
	DWORD				m_dwCommEvents;
	// ���������� ����, ������� ���������� �������� �� ������� �������� �������� WriteFile
	DWORD				dwBytesToWrite;

//
// ��������� �������� �����
//
	struct sCOMM
	{
		HANDLE  hCommPort;				// ����� �����
		BYTE    bPortnr;				// ����� �����
		DWORD	dwBaud;					// �������� ������ �������
		BYTE	bDatabits;				// ����� ���� ������ � �����
		BYTE	bParity;				// ��������
		BYTE	bStopbits;				// ����� �������� ���
		DWORD   dwCommevents;			// ����� ������� �� ������ �����
		DWORD	dwWritebuffersize;		// ����� ����������� �������

		CHAR    chFlag;					// 
		CHAR	chXON;					// XON ������ 
		CHAR	chXOFF;					// XOFF ������ 
		WORD    wXONLimit;				// ����������� ����� ���� � ������� ��� �������� XON
		WORD	wXOFFLimit;				// ������������ ����� ���� � ������� ��� �������� XOFF
		DWORD   fRTSControl;			// ���������� ������ RTS
		DWORD   fDTRControl;			// ���������� ������ DTR
		BOOL    fCTSOutFlow;			// ���������� �������� ������� �� ��������� CTS
		BOOL	fDSROutFlow;			// ���������� �������� ������� �� ��������� DSR
		BOOL	fDSRInFlow;				// ���������� ������� ������� �� ��������� DSR
		BOOL	fXonXoffOutFlow;		//
		BOOL	fXonXoffInFlow;			//
		BOOL	fTXafterXoffSent;		//
		BOOL	fNull;					// ���������� ������� �������� ����� ( 0x00 )
	} COMM;

	struct sFlowControl
	{
		struct sBaud
		{
			DWORD _CBR_110;
			DWORD _CBR_300;
			DWORD _CBR_600;
			DWORD _CBR_1200;
			DWORD _CBR_2400;
			DWORD _CBR_4800;
			DWORD _CBR_9600;
			DWORD _CBR_14400;
			DWORD _CBR_19200;
			DWORD _CBR_38400;
			DWORD _CBR_57600;
			DWORD _CBR_115200;
			DWORD _CBR_128000;
			DWORD _CBR_256000;
		} Baud;

		struct sDatabits
		{
				
		} Databits;

		struct sStopbits
		{
			BYTE _ONESTOPBIT;
			BYTE _ONE5STOPBITS;
			BYTE _TWOSTOPBITS;
		} Stopbits;

		struct sParity
		{
			BYTE _EVENPARITY;
			BYTE _MARKPARITY;
			BYTE _NOPARITY; 
			BYTE _ODDPARITY; 
			BYTE _SPACEPARITY; 
		} Parity;

		struct sRTS
		{
			BOOL _RTS_CONTROL_DISABLE;	
			BOOL _RTS_CONTROL_ENABLE;	
			BOOL _RTS_CONTROL_HANDSHAKE;	
			BOOL _RTS_CONTROL_TOGGLE;	
		} RTS;

		struct sDTR
		{
			BOOL _DTR_CONTROL_DISABLE;
			BOOL _DTR_CONTROL_ENABLE;
			BOOL _DTR_CONTROL_HANDSHAKE;
		} DTR;

	} FlowControl;


/*#define COMPORT( x )        (x.hCommPort)
#define PORT( x )           (x.bPortnr)
#define BAUDRATE( x )       (x.dwBaud)
#define BYTESIZE( x )       (x.bDatabits)
#define PARITY( x )         (x.bParity)
#define STOPBITS( x )       (x.bStopbits)
#define EVENTFLAGS( x )     (x.dwCommevents)
#define BUFFSIZE( x )       (x.dwWritebuffersize)

#define FLAGCHAR( x )       (x.chFlag)

#define DTRCONTROL( x )     (x.fDTRControl)
#define RTSCONTROL( x )     (x.fRTSControl)
#define XONCHAR( x )        (x.chXON)
#define XOFFCHAR( x )       (x.chXOFF)
#define XONLIMIT( x )       (x.wXONLimit)
#define XOFFLIMIT( x )      (x.wXOFFLimit)
#define CTSOUTFLOW( x )     (x.fCTSOutFlow)
#define DSROUTFLOW( x )     (x.fDSROutFlow)
#define DSRINFLOW( x )      (x.fDSRInFlow)
#define XONXOFFOUTFLOW( x ) (x.fXonXoffOutFlow)
#define XONXOFFINFLOW( x )  (x.fXonXoffInFlow)
#define TXAFTERXOFFSENT(x)  (x.fTXafterXoffSent)
#define NULLENABLE(x)		(x.fNull)

*/
/*
�������� baudrate:
����� ��������� ����� ���������� �������� ��� ���� �� ������:
CBR_110
CBR_300
CBR_600
CBR_1200
CBR_2400
CBR_4800
CBR_9600
CBR_14400
CBR_19200
CBR_38400
CBR_57600
CBR_115200
CBR_128000
CBR_256000
*/

/*
��������� �������� ������:
ONESTOPBIT
ONE5STOPBITS
TWOSTOPBITS
*/
/*
��������� �������� ��������:
EVENPARITY 
MARKPARITY 
NOPARITY 
ODDPARITY 
SPACEPARITY 
*/

// �������� fOutxDsrFlow - ���� ���������� � ������, ���������� ���������� ������� ������:  
// ���� TRUE, �� �������������� ����� DSR (data-set-ready) ��� ���������� �������� �������.
// ���� TRUE � DSR=0, �������� ������������������ �� ��������� DSR=1.

// �������� fDsrSensitivity:  
// ���� TRUE, �� �������������� ����� DSR (data-set-ready) ��� ���������� ������� �������.
// ���� TRUE � DSR=0, ������������ ����� ����� ������ �� ��������� DSR=1.

/*
// ���������� ���������� ������� ������ RTS / CTS:
	�������� RTS (request-to-send) ����� ������� �� ��������:
RTS_CONTROL_DISABLE		- ��������� ����� RTS ��� �������� ����� � ����� ��������� �����������.
RTS_CONTROL_ENABLE		- ��������� ����� RTS ��� �������� ����� � ����� ��������� �����������. 
RTS_CONTROL_HANDSHAKE	- ��������� ������������� �� ����� RTS. ������� ��������� RTS � 1,
	����� ������ �������� ������ ������ ��������, � � 0, ����� ������ ���������� ������ 3/4.
	���� ������������� ���������, ��������� ������ �� ������� ���������� ������ ��������� ����� ��������� ������� EscapeCommFunction. 
RTS_CONTROL_TOGGLE		- ������� ������������� RTS � 1, ���� ���� ������ ��� ��������.
	����� �������� ���� ����������������� ������ ����� RTS ��������������� � 0.
	Windows Me/98/95:  ��� �������� �� ��������������. 

	�������� fOutxCtsFlow - ���� ���������� ��������: 
���� TRUE, �� �������������� ����� CTS (clear-to-send) ��� ���������� �������� �������.
���� TRUE � CTS=0, �������� ��������� ������������������ �� ��������� CTS=1.
*/

/*
�������� DTR (data-terminal-ready) ����� ���������� � ������ ������:
DTR_CONTROL_DISABLE		- ��������� ����� DTR ��� �������� ����� � ����� ��������� �����������. 
DTR_CONTROL_ENABLE		- ��������� ����� DTR ��� �������� ����� � ����� ��������� �����������. 
DTR_CONTROL_HANDSHAKE	- ��������� ������������� �� ����� DTR. ���� ������������� ���������, 
	��������� ������ �� ������� ���������� ������ ��������� ����� ��������� ������� EscapeCommFunction. 
*/

// �����!!!
// �������� fNull:
// ���� TRUE, ������� ����� (0x00) �� �������������� ��� ���������.

/*
��������� XonChar / XoffChar - ����������� ���������� �������:
������� (�����) ���������� � ���������� �������� ������. 
���������� ���������� ��� ������������� ������. �������������� ���������� ��������.
��������� XonLim / XoffLim:
�����. ����� ���� �� ������� �������, ��� ������� ���������� ������ XON.
����.  ����� ���� �� ������� �������, ��� ������� ���������� ������ XOFF.
*/


/*

char * szBaud[] = {
	    "110", "300", "600", "1200", "2400", 
	    "4800", "9600", "14400", "19200",
	    "38400", "56000", "57600", "115200", 
	    "128000", "256000"
	};

DWORD   BaudTable[] =  {
	    CBR_110, CBR_300, CBR_600, CBR_1200, CBR_2400,
	    CBR_4800, CBR_9600, CBR_14400, CBR_19200, CBR_38400,
	    CBR_56000, CBR_57600, CBR_115200, CBR_128000, CBR_256000
	} ;

char * szParity[] =   {   "None", "Even", "Odd", "Mark", "Space" };

DWORD   ParityTable[] = {  NOPARITY, EVENPARITY, ODDPARITY, MARKPARITY, SPACEPARITY  } ;

char * szStopBits[] =  {  "1", "1.5", "2"  };

DWORD   StopBitsTable[] =  { ONESTOPBIT, ONE5STOPBITS, TWOSTOPBITS } ;

char * szDTRControlStrings[] = { "Enable", "Disable", "Handshake" };

DWORD   DTRControlTable[] = { DTR_CONTROL_ENABLE, DTR_CONTROL_DISABLE, DTR_CONTROL_HANDSHAKE };

char * szRTSControlStrings[] = { "Enable", "Disable", "Handshake", "Toggle" };

DWORD   RTSControlTable[] = {   RTS_CONTROL_ENABLE, RTS_CONTROL_DISABLE, 
				RTS_CONTROL_HANDSHAKE, RTS_CONTROL_TOGGLE };

DWORD   EventFlagsTable[] = {
	    EV_BREAK, EV_CTS, EV_DSR, EV_ERR, EV_RING,
	    EV_RLSD, EV_RXCHAR, EV_RXFLAG, EV_TXEMPTY

*/


};





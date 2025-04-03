/*************************************************************************************
***************************************************************************************
**		Этот модуль работает с последовательным портом на самом низком уровне.
**	Не использует никакие другие мои модули.
*****************************************************************************************
***************************************************************************************/

/********************	ЛИСТ РЕГИСТРАЦИИ ИЗМЕНЕНИЙ	*********************************
*		12.10,2006 - + Теперь этот файл стал глобальным и находится в одной овщей для 
*	всех библиотек директории
*		26.10.2006 - + Исправлен серьёзнейший глюк, по которому событие EV_TXEMPTY
*	наступало только после приема какого-либо символа. Поэтому окончательно работющей 
*	отныне считается только эта версия.
*		30.10.2006 - + Исправлен глюк с переключением на новый порт, после ошибочного выбора 
*	старого.
*					+ Добавлено изменение в деструктор
*		22.01.2007 - убраны некоторые warningi
*		14.02.2007 - добавлено отслеживание ошибки ERROR_OPERATION_ABORTED
*		14.04.2007 - в одной из функция переменная типа COMSTAT объявлена статической,
*						иначе компилятор глючит
*************************************************************************************/

#ifndef __SERIALPORT_H__
#define __SERIALPORT_H__

#define WM_COMM_BREAK_DETECTED		WM_USER+1	// A break was detected on input.
#define WM_COMM_CTS_DETECTED		WM_USER+2	// The CTS (clear-to-send) signal changed state. 
#define WM_COMM_DSR_DETECTED		WM_USER+3	// The DSR (data-set-ready) signal changed state. 
#define WM_COMM_ERR_DETECTED		WM_USER+4	// A line-status error occurred. Line-status errors are CE_FRAME, CE_OVERRUN, and CE_RXPARITY. 
#define WM_COMM_RING_DETECTED		WM_USER+5	// A ring indicator was detected. 
#define WM_COMM_RLSD_DETECTED		WM_USER+6	// The RLSD (receive-line-signal-detect) signal changed state. 
#define WM_COMM_RXCHAR				WM_USER+7	// A character was received and placed in the input buffer. 
#define WM_COMM_RXFLAG_DETECTED		WM_USER+8	// The event character was received and placed in the input buffer.  
#define WM_COMM_TXEMPTY_DETECTED	WM_USER+9	// The last character in the output buffer was sent.  

#define WM_COMM_CLEAR				WM_USER+10	// The RX buffer was cleared. //Моё!!! ;)

class CSerialPort
{														 
public:
	void ClearRXBuff();
	void ClearTXBuff();
	void SetRTS(bool rts);
	// contruction and destruction
	CSerialPort();
	virtual		~CSerialPort();

	// port initialisation											
	BOOL		InitPort(CWnd* pPortOwner, UINT portnr = 1, UINT baud = 19200, char parity = 'N', UINT databits = 8, UINT stopsbits = 2, DWORD dwCommEvents = EV_RXCHAR | EV_TXEMPTY | EV_ERR, UINT nBufferSize = 512);

	// start/stop comm watching
	BOOL		StartMonitoring();
	BOOL		RestartMonitoring();
	BOOL		StopMonitoring();

	DWORD		GetWriteBufferSize();
	DWORD		GetCommEvents();
	DCB			GetDCB();

	void		WriteToPort(unsigned char* string, unsigned int num);
	HANDLE				m_hComm;
	UINT				m_nPortNr;
protected:
	// protected memberfunctions
	void		ProcessErrorMessage(char* ErrorText);
	static UINT	CommThread(LPVOID pParam);
	static void	ReceiveChar(CSerialPort* port, COMSTAT comstat);
	static void	WriteChar(CSerialPort* port);

	// thread
	CWinThread*			m_Thread;

	// synchronisation objects
	CRITICAL_SECTION	m_csCommunicationSync;
	BOOL				m_bThreadAlive;

	// handles
	HANDLE				m_hShutdownEvent;
	
	HANDLE				m_hWriteEvent;

	// Event array. 
	// One element is used for each event. There are two event handles for each port.
	// A Write event and a receive character event which is located in the overlapped structure (m_ov.hEvent).
	// There is a general shutdown when the port is closed. 
	HANDLE				m_hEventArray[3];

	// structures
	OVERLAPPED			m_ov;
	COMMTIMEOUTS		m_CommTimeouts;
	DCB					m_dcb;

	// owner window
	CWnd*				m_pOwner;

	// misc
	
	unsigned char*		m_szWriteBuffer;
	DWORD				m_dwCommEvents;
	DWORD				m_nWriteBufferSize;
	DWORD				dwBytesToWrite;

};

#endif __SERIALPORT_H__



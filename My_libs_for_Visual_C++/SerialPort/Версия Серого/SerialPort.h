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
#pragma once

// Сообщения родителю о событиях возникших при ПОЛУЧЕНИИ байта.
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
	//конструктор и деструктор
	CSerialPort();
	virtual		~CSerialPort();

	//инициализация порта
	BOOL		InitPort(CWnd* pPortOwner, UINT portnr = 1, UINT baud = 19200, char parity = 'N', UINT databits = 8, UINT stopsbits = 2, DWORD dwCommEvents = EV_RXCHAR | EV_TXEMPTY | EV_ERR, UINT nBufferSize = 512);
	void ClearRXBuff();
	void ClearTXBuff();
	void SetRTS(bool rts);

	// запуск нового потока наблюдения за портом
	BOOL		StartMonitoring();		//использовать только один раз для нового порта
	// приостановление наблюдения за портом
	BOOL		StopMonitoring();		//остановка потока наблюдения
	// возобновления наблюдения за текущим портом
	BOOL		RestartMonitoring();

	// запрос размера буффера записи
	DWORD		GetWriteBufferSize();
	// запрос событий определяющиих связь
	DWORD		GetCommEvents();
	// 
	DCB			GetDCB();

	void		WriteToPort(unsigned char* string, unsigned int num);

	// номер порта
	UINT				m_nPortNr;
	// Объект файла (порта)
	HANDLE				m_hComm;
protected:

	// защищенные члены-функции
	void		ProcessErrorMessage(char* ErrorText);

	// Главная статическая функция потока
	static UINT	CommThread(LPVOID pParam);
	// статическая функция приема символа (байта)
	static void	ReceiveChar(CSerialPort* port, COMSTAT comstat);
	// статическая функция передачи символа (байта)
	static void	WriteChar(CSerialPort* port);

	// Поток
	CWinThread*			m_Thread;

	// Объекты синхронизации
	// Критическая секция
	CRITICAL_SECTION	m_csCommunicationSync;
	// Флаг существования потока
	BOOL				m_bThreadAlive;
	// Флаг останова потока
	BOOL				m_bThreadSuspended;

	// Хэндлы на объекты
	// Объект события останова потока
	HANDLE				m_hShutdownEvent;

	// Объект события записи
	HANDLE				m_hWriteEvent;

	// Массив событий. 
	// Каждый элемент массива содержит хэндл события.
	// 0 - Главное событие потока - останов потока при закрытии порта.
	// Порт использует два хэндла событий:
	// 1 - событие при получении символа, которые расположены в асинхронной структуре (m_ov.hEvent).
	// 2 - событие записи m_hWriteEvent 
	HANDLE				m_hEventArray[3];

	// Структуры
	// Асинхронный ввод/вывод
	// Эта структура должна всегда быть инициализирована нулем перед использованием в функции.
	// Иначе функция совершит ошибку с кодом возврата ERROR_INVALID_PARAMETER
	OVERLAPPED			m_ov;
	// Параметры тайм-аутов
	COMMTIMEOUTS		m_CommTimeouts;
	// Управляющие настройки порта
	DCB					m_dcb;

	// Указатель на родительский класс или окно
	CWnd*		m_pOwner;

	// прочее

	// буффер передачи
	unsigned char *		m_szWriteBuffer;
	// максимальный размер буффера передачи
	DWORD				m_nWriteBufferSize;
	// маска событий связи
	DWORD				m_dwCommEvents;
	// количество байт, которое необходимо передать из буффера передачи функцией WriteFile
	DWORD				dwBytesToWrite;

//
// структура настроек порта
//
	struct sCOMM
	{
		HANDLE  hCommPort;				// хэндл порта
		BYTE    bPortnr;				// номер порта
		DWORD	dwBaud;					// скорость обмена данными
		BYTE	bDatabits;				// длина поля данных в битах
		BYTE	bParity;				// четность
		BYTE	bStopbits;				// число стоповых бит
		DWORD   dwCommevents;			// маска событий по приему байта
		DWORD	dwWritebuffersize;		// длина передающего буффера

		CHAR    chFlag;					// 
		CHAR	chXON;					// XON символ 
		CHAR	chXOFF;					// XOFF символ 
		WORD    wXONLimit;				// минимальное число байт в буффере для передачи XON
		WORD	wXOFFLimit;				// максимальное число байт в буффере для передачи XOFF
		DWORD   fRTSControl;			// управление линией RTS
		DWORD   fDTRControl;			// управление линией DTR
		BOOL    fCTSOutFlow;			// управление выходным потоком по состоянию CTS
		BOOL	fDSROutFlow;			// управление выходным потоком по состоянию DSR
		BOOL	fDSRInFlow;				// управление входным потоком по состоянию DSR
		BOOL	fXonXoffOutFlow;		//
		BOOL	fXonXoffInFlow;			//
		BOOL	fTXafterXoffSent;		//
		BOOL	fNull;					// управление приемом нулевого байта ( 0x00 )
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
параметр baudrate:
может принимать любое приминимое значение или взят из списка:
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
возможные значения стопов:
ONESTOPBIT
ONE5STOPBITS
TWOSTOPBITS
*/
/*
возможные значения четности:
EVENPARITY 
MARKPARITY 
NOPARITY 
ODDPARITY 
SPACEPARITY 
*/

// параметр fOutxDsrFlow - вход готовности к обмену, аппаратное управление потоком данных:  
// Если TRUE, то контролируется линия DSR (data-set-ready) для управления выходным потоком.
// Если TRUE и DSR=0, передача приостанавливается до появления DSR=1.

// параметр fDsrSensitivity:  
// Если TRUE, то контролируется линия DSR (data-set-ready) для управления входным потоком.
// Если TRUE и DSR=0, прекращается прием любых данных до появления DSR=1.

/*
// аппаратное управление потоком данных RTS / CTS:
	параметр RTS (request-to-send) выход запроса на передачу:
RTS_CONTROL_DISABLE		- Запрещает линию RTS при открытии порта и после оставляет запрещенной.
RTS_CONTROL_ENABLE		- Разрешает линию RTS при открытии порта и после оставляет разрешенной. 
RTS_CONTROL_HANDSHAKE	- Разрешает подтверждения по линии RTS. Драйвер переводит RTS в 1,
	когда размер входного буффер меньше половины, и в 0, когда буффер заполнился больше 3/4.
	Если подтверждение разрешено, возникает ошибка по которой приложение должно настроить линию используя функцию EscapeCommFunction. 
RTS_CONTROL_TOGGLE		- Драйвер устанавливает RTS в 1, если есть данные для передачи.
	После передачи всех буфферизированных данных линия RTS устанавливается в 0.
	Windows Me/98/95:  Эта величина не поддерживается. 

	параметр fOutxCtsFlow - вход разрешения передачи: 
Если TRUE, то контролируется линия CTS (clear-to-send) для управления выходным потоком.
Если TRUE и CTS=0, передача аппаратно приостанавливается до появления CTS=1.
*/

/*
параметр DTR (data-terminal-ready) выход готовности к обмену данных:
DTR_CONTROL_DISABLE		- Запрещает линию DTR при открытии порта и после оставляет запрещенной. 
DTR_CONTROL_ENABLE		- Разрешает линию DTR при открытии порта и после оставляет разрешенной. 
DTR_CONTROL_HANDSHAKE	- Разрешает подтверждения по линии DTR. Если подтверждение разрешено, 
	возникает ошибка по которой приложение должно настроить линию используя функцию EscapeCommFunction. 
*/

// ВАЖНО!!!
// параметр fNull:
// Если TRUE, нулевый байты (0x00) НЕ обрабатываются при получении.

/*
параметры XonChar / XoffChar - программное управление потоком:
Символы (байты) разрешения и запрещения передачи данных. 
Передаются приемником при возникновении ошибок. Обрабатываются программно системой.
параметры XonLim / XoffLim:
миним. число байт во входном буффере, при котором передается символ XON.
макс.  число байт во входном буффере, при котором передается символ XOFF.
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





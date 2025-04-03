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
*		24.04.2007 - КОНКРЕТНАЯ ПЕРЕДЕЛКА!!! большая часть кода содрана у Серого, зато теперь
*					гарантировано горячее переключение портов и корректное выполнение
*		14.09.2007 - инициализация критической секции ушла в конструктор, т.к. ее удаление
*						находится в деструкторе, что устранило вылетание программы, если 
*						InitPort не была вызвана
*		29.02.2008 -  в функции CreateFile добавил параметр FILE_ATTRIBUTE_NORMAL
*		14.04.2008 - ушел от MFC к чистому Windows API, появилась возможность не 
*						отправлять сообщение внешнему окну, а использовать указатель
*						на внешнюю обрабатывающую функцию.
*		17.04.2008 - улучшена функция WriteToPort
*		3.08.2008	- появилась функция ClosePort
*		01.09.2008 - Полностью перепахана функция ReceiveChar, которая стала принимать целые
*					блоки, а не по одному байту, из-за чего сильно тормозилась работа с 
*					CP2102
*		6.10.2008 - исправлена ошибка в фукнции ReceivePortLoLevel
*		08.04.2009 - в функции ClosePort при закрытии хэндла добавлена задержка в 100 мс,
*					чтобы можно было безглючно перенастраиваться на тот же номер порта.
*************************************************************************************/
#pragma once

// Сообщения родителю о событиях на порте
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
#define WM_COMM_ACCESS_DENIED			(WM_USER + 14)	//Доступ к порту запрещен (посылается без маски)
#define WM_COMM_UNCOWN_ERROR			(WM_USER + 15)	//неизвестная ошибка

#define PURGE_FLAGS             PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR 
#define PURGE_RX				PURGE_RXABORT | PURGE_RXCLEAR 
#define PURGE_TX				PURGE_TXABORT | PURGE_TXCLEAR 
#define EVENTFLAGS_DEFAULT      EV_BREAK | EV_CTS | EV_DSR | EV_ERR | EV_RING | EV_RLSD
#define FLAGCHAR_DEFAULT        '\n'

// Главная статическая функция потока
DWORD WINAPI CommThread(PVOID pParam);

class CSerialPort
{														 
public:
	//конструктор и деструктор
	CSerialPort();
	virtual		~CSerialPort();

	//инициализация порта
	BOOL		InitPort(HWND ParentHWND, UINT portnr = 1, UINT baud = 19200, char parity = 'N', UINT databits = 8, UINT stopbits = 2, DWORD dwCommEvents = EV_RXCHAR | EV_TXEMPTY | EV_ERR, UINT writebuffersize = 512);
	//завершаем работу с портом
	void		ClosePort();
	//задание обработчика
	bool		SetParentSupervisor(void (*parent_supervisor)(UINT Msg, WPARAM wParam, LPARAM lParam));

	void ClearRXBuff();
	void ClearTXBuff();
	void SetRTS(bool rts);

	// запуск нового потока наблюдения за портом
	BOOL		StartMonitoring();		//использовать только один раз для нового порта
	// приостановление наблюдения за портом
	BOOL		StopMonitoring();		//остановка потока наблюдения
	// возобновления наблюдения за текущим портом
	BOOL		ContinueMonitoring();

	// запрос размера буффера записи
	DWORD		GetWriteBufferSize();
	// запрос событий определяющиих связь
	DWORD		GetCommEvents();
	// 
	DCB			GetDCB();

	//собственно функция отправки данных
	bool		WriteToPort(const BYTE* pBufToSend, DWORD NumBytes);

	// номер порта
	UINT				m_nPortNr;
	// Объект файла (порта)
	HANDLE				m_hComm;
protected:

	// защищенные члены-функции
	void		ProcessErrorMessage(TCHAR* ErrorText);

	// статическая функция приема символа (байта)
	static bool	ReceivePortLoLevel(CSerialPort* port);
	// статическая функция передачи символа (байта)
	static bool	WritePortLoLevel(CSerialPort* port);

	// Поток
	HANDLE				m_hThread;

	// Объекты синхронизации
	// Критическая секция
//	CRITICAL_SECTION	m_csCommunicationSync;
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
public:
	OVERLAPPED			m_ov;
	// Параметры тайм-аутов
	COMMTIMEOUTS		m_CommTimeouts;
	// Управляющие настройки порта
	DCB					m_dcb;

	//Хэндл родительского окна
	HWND				m_ParentHWND;
	//Указатель на супервизор родителя (если это не окно)
	void (*ParentSupervisor)(UINT Msg, WPARAM wParam, LPARAM lParam);

	//Уведомляет родителя, либо вызывая его супервизорную функцию, либо 
	//посылая ему сообщение
	void NotifyParent(UINT Msg, WPARAM wParam, LPARAM lParam);

	// прочее

	// буффер передачи
	BYTE *				m_szWriteBuffer;
	// максимальный размер буффера передачи
	DWORD				m_nWriteBufferSize;
	// маска событий связи
	DWORD				m_dwCommEvents;
	// количество байт, которое необходимо передать из буффера передачи функцией WriteFile
	DWORD				dwBytesToWrite;

	//Функции потока должны быть доступны все поднаготные
	friend DWORD WINAPI CommThread(PVOID pParam);

};




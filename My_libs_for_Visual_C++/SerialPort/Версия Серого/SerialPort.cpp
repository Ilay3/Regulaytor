/*
**	FILENAME			CSerialPort.cpp
**
**	PURPOSE				This class can read, write and watch one serial port.
**						It sends messages to its owner when something happends on the port
**						The class creates a thread for reading and writing so the main
**						program is not blocked.
**
*/

#include "stdafx.h"
#include "SerialPort.h"
 
#include <assert.h>
//#include <memory.h>

/*
Эта опция заставляет компилятор не выдавать предупреждения, когда используются
старые функции, работающие со строками без ограничения на их длину. 
Например strcut вместо strcut_s. 
*/
#define _CRT_SECURE_NO_DEPRECATE 


CSerialPort::CSerialPort()
{
	//инициализируем хэндл создаваемого файла (порта)
	m_hComm				= NULL;

	// инициализируем асинхронную структуру нулем (обязательно)
	m_ov.Offset			= 0;
	m_ov.OffsetHigh		= 0;

	// инициализируем хэндлы событий
	m_ov.hEvent			= NULL;
	m_hWriteEvent		= NULL;
	m_hShutdownEvent	= NULL;

	// инициализируем буффер передачи
	m_szWriteBuffer		= NULL;

	// сброс флага существования потока
	m_bThreadAlive		= FALSE;

	// сброс флага останова потока
	m_bThreadSuspended	= FALSE;
}


//
// Динамческое удаление памяти
//
CSerialPort::~CSerialPort()
{
	// если поток существует, то убиваем - посылаем событие m_hShutdownEvent
	if (m_bThreadAlive)
	{
		// проверка на останов потока
		if(m_bThreadSuspended)
			RestartMonitoring();
	
		// посылаем событие m_hShutdownEvent, пока поток не убьется
		do
		{
			SetEvent(m_hShutdownEvent);
		} while (m_bThreadAlive == TRUE);

		// очищаем указатель убитого потока
		m_Thread = NULL;
		TRACE("Serial Thread shutdowned.\n");
	}

	// сбрасываем все события
	if (m_ov.hEvent != NULL)
		ResetEvent(m_ov.hEvent);
	if (m_hWriteEvent != NULL)
		ResetEvent(m_hWriteEvent);
	if (m_hShutdownEvent != NULL)
		ResetEvent(m_hShutdownEvent);

	// удаляем буффер
	if (m_szWriteBuffer != NULL)
	{
		delete [] m_szWriteBuffer;
		m_szWriteBuffer=NULL;
	}

	// закрываем открытый ранее порт
	if (m_hComm != NULL)
	{
		CloseHandle(m_hComm);
		m_hComm = NULL;
		TRACE("Serial Port closed.\n");
	}

	// удаляем критическую секцию
	DeleteCriticalSection(&m_csCommunicationSync);

	TRACE("Serial Communication ended.\n");
}

//
// Инициализация порта. Номер порта должен находиться в диапазоне 0 - 255.
//
BOOL CSerialPort::InitPort(CWnd* pPortOwner,	// родитель порта
						   UINT  portnr,				// номер порта (1..255)
						   UINT  baud,					// скорость порта
						   char  parity,				// четность 
						   UINT  databits,				// число бит в поле данных 
						   UINT  stopbits,				// число стоповых бит 
						   DWORD dwCommEvents,			// события по приему EV_RXCHAR, EV_CTS и т.д.
						   UINT  writebuffersize)		// размер буффера передачи

{
	// проверка номера порта 
	assert(portnr > 0 && portnr < 256);
	// проверка на существование родителя
	assert(pPortOwner != NULL);

	// если поток существует: убиваем
	if (m_bThreadAlive)
	{
		// проверка на останов потока
		if(m_bThreadSuspended)
			RestartMonitoring();
	
		// посылаем событие m_hShutdownEvent, пока поток не убьется
		do
		{
			SetEvent(m_hShutdownEvent);
		} while (m_bThreadAlive == TRUE);
		// очищаем указатель убитого потока
		m_Thread = NULL;
		TRACE("Serial Thread shutdowned while init\n");
	}

	// создаем события, предварительно сбросив
	if (m_ov.hEvent != NULL)
		ResetEvent(m_ov.hEvent);
	m_ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (m_hWriteEvent != NULL)
		ResetEvent(m_hWriteEvent);
	m_hWriteEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	
	if (m_hShutdownEvent != NULL)
		ResetEvent(m_hShutdownEvent);
	m_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// инициализация объектов событий
	m_hEventArray[0] = m_hShutdownEvent;	// наивысший приоритет
	m_hEventArray[1] = m_ov.hEvent;
	m_hEventArray[2] = m_hWriteEvent;

	// инициализация критической секции
	InitializeCriticalSection(&m_csCommunicationSync);
	
	// сохраняем родителя
	m_pOwner = pPortOwner;

	// если буффер уже выделен, удаляем его
	if (m_szWriteBuffer != NULL)
	{
		delete [] m_szWriteBuffer;
		m_szWriteBuffer = NULL;
	}
	// выделяем буффер для записи в порт
	m_szWriteBuffer = new unsigned char[writebuffersize];

	// сохраняем номер порта
	m_nPortNr = portnr;

	// установка размера буффера для записи
	m_nWriteBufferSize = writebuffersize;

	// сохраняем маску событий по приему
	m_dwCommEvents = dwCommEvents;

//	BOOL bResult = FALSE;
	
	// временные символьные буфферы
	char *szPort = new char[50];
	char *szBaud = new char[50];

	// теперь критично!
	EnterCriticalSection(&m_csCommunicationSync);

	// если порт уже открыт, то закрываем его
	if (m_hComm != NULL)
	{
		CloseHandle(m_hComm);
		m_hComm = NULL;
		TRACE("Serial Port closed while init\n");
	}

	// символьное описания параметров порта (используется синтаксис команды MODE в DOS)
	// имя порта
	sprintf_s(szPort, 50, "COM%d", portnr);
	// строка параметров
	sprintf_s(szBaud, 50, "baud=%d parity=%c data=%d stop=%d", baud, parity, databits, stopbits);

	// открываем порт и сохраняем его хэндл
	m_hComm = CreateFile(szPort,						// строка параметров порта (COMX)
					     GENERIC_READ | GENERIC_WRITE,	// чтение/запись
					     0,								// Устройство Последовательный порт должен быть открыт с параметром 0 и означает, что доступ к объекту и повторное его открытие невозможно пока хэндл не будет закрыт - Исключительный доступ
					     NULL,							// без аттрибутов безопасности
					     OPEN_EXISTING,					// Устройство Последовательный порт должен быть открыт с флагом OPEN_EXISTING (открыть существующий файл)
					     FILE_FLAG_OVERLAPPED,			// Асинхронный ввод/вывод
					     NULL);							// template должен быть NULL для устройств связи

	// проверка хэндла порта 
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		// порт не найден
		// удаляем временные буфферы
		delete [] szPort;
		delete [] szBaud;
		szPort = NULL;
		szBaud = NULL;

		// удаляем буффер передачи
		delete [] m_szWriteBuffer;
		m_szWriteBuffer = NULL;

		// освобождаем критическую секцию
		LeaveCriticalSection(&m_csCommunicationSync);
		TRACE("Serial Port invalid while init\n");
		return FALSE;
	}

	// установка тайм-аутов связи
	// ReadIntervalTimeout
	// максимальное время ожидания прихода следующего байта, мсек
	// отсчет начинается при получении байта во время работы ReadFile.
	// ReadFile возвращает любой принятый буффер, если интервал превышает указанный
	m_CommTimeouts.ReadIntervalTimeout			= 1000;
	// не понятно пока
	m_CommTimeouts.ReadTotalTimeoutMultiplier	= 1000;
	m_CommTimeouts.ReadTotalTimeoutConstant		= 1000;
	m_CommTimeouts.WriteTotalTimeoutMultiplier	= 1000;
	m_CommTimeouts.WriteTotalTimeoutConstant	= 1000;

	// настройка порта
	// установка тайм-аутов
	if (SetCommTimeouts(m_hComm, &m_CommTimeouts))
	{						
		// установка маски разрешенных событий по приему байта
		if (SetCommMask(m_hComm, dwCommEvents))
		{
			// заполнение структуры DCB текущими настройками порта
			if (GetCommState(m_hComm, &m_dcb))
			{
				// изменение управляющих настроек
				// Управление битом RTS:
				// RTS_CONTROL_ENABLE - ра
				m_dcb.fRtsControl	= RTS_CONTROL_ENABLE;
				// RTS_CONTROL_TOGGLE - устанавливает бит RTS в 1, если имеются байты для передачи.
				// После отправки всего буфера, бит RTS переходит в 0. 
				// Только для Windows 2000/XP/2003
				//m_dcb.fRtsControl	= RTS_CONTROL_TOGGLE;		

				// ДЛЯ ПРИЕМА НУЛЯ!!!
				m_dcb.fNull			= FALSE;	

				// дополнение структуры DCB параметрами из строки настроек порта 
				if (BuildCommDCB(szBaud, &m_dcb))
				{
					// конфигурирование порта в соответствии с настройками в DCB
					if (SetCommState(m_hComm, &m_dcb))
						; // нормальная операция... продолжаем
					else
						ProcessErrorMessage("SetCommState()");
				}
				else
					ProcessErrorMessage("BuildCommDCB()");
			}
			else
				ProcessErrorMessage("GetCommState()");
		}
		else
			ProcessErrorMessage("SetCommMask()");
	}
	else
		ProcessErrorMessage("SetCommTimeouts()");

	delete [] szPort;
	delete [] szBaud;
	szPort = NULL;
	szBaud = NULL;

	// очистка порта
	PurgeComm(m_hComm, PURGE_FLAGS);

	// освобождение критической секции
	LeaveCriticalSection(&m_csCommunicationSync);

	TRACE("Initialisation for communicationport %d completed.\nUse Startmonitor to communicate.\n", portnr);


	return TRUE;
}

//
//  Главная статическсая функция потока CommThread.
//
UINT CSerialPort::CommThread(LPVOID pParam)
{
	// Приведение указателя на тип void переданный в поток (AfxBeginThread(..., !this!))
	// назад к указателю на класс CSerialPort
	CSerialPort *port = (CSerialPort*)pParam;
	
	// установка флага состояния потока (информация для родителя)  
	// TRUE - для индикации, что поток запущен.
	port->m_bThreadAlive = TRUE;	
		
	// Прочие переменные
	// байт передано
//	DWORD BytesTransfered = 0; 
	// маска события
	DWORD Event		= 0;
	DWORD CommEvent = 0;
	// маска ошибки
	DWORD dwError	= 0;
	// структура, содержащая информацию по порту 
	COMSTAT comstat;
	// флаг резудьтата, предполагаем что все будет ОК!
	BOOL  bResult	= TRUE;
	// добавлено для 2005vc++, ругался на неинициализированную comstat
	// заполняем структуру COMSTAT
	ClearCommError(port->m_hComm, &dwError, &comstat);

		
	// Очищаем буффер перед запуском
	if (port->m_hComm)		// проверяем открыт ли порт
		PurgeComm(port->m_hComm, PURGE_FLAGS);

	// начинаем вечный цикл.  Цикл будет существовать пока работает поток.
	for (;;) 
	{ 

		// WaitCommEvent ожидает появление события из установленных для данного порта.
		// Вызываем WaitCommEvent(). Этот вызов возвратиться немедленно,
		// потому что наш порт создан как асинхронный порт (FILE_FLAG_OVERLAPPED
		// и используется асинхронная структура m_ov). Функция установит элемент m_ov.hEvent,
		// который является частью массива событий m_hEventArray, в несигнальное состояние,  
		// если нет ни одного байта для чтения или в сигнальное состояние,  
		// если есть что прочитать из порта. Если этот хэндл события установлен в несигнальное состояние,  
		// то по приходу символа (байта) в порт он будет установлен в сигнальное состояние.

		bResult = WaitCommEvent(port->m_hComm, &Event, &port->m_ov);

		// Если асинхронная операция не может быть завершена немедленно, то вернется FALSE
		if (!bResult)  
		{ 
			// Если WaitCommEvent() возвратит FALSE, то обработаем последнюю ошибку
			// для определения причины..
			switch (dwError = GetLastError()) 
			{ 
			case ERROR_IO_PENDING: 	
				{ 
					// Это нормальный код возврата - операция работает в фоновом режиме.
					// Система устанавливает m_ov.hEvent в несигнальное состояние до возврата из WaitCommEvent.
					// Затем система устанавливает m_ov.hEvent в сигнальное состояние 
					// при появлении события или возникновении ошибки.
					// Поэтому ничего не делаем и продолжаем ожидание события в GetOverlappedResult!
					break;
				}
			case 87:
				{
					// В Windows NT, этот код возвращается по нескольким причинам.
					// И это тоже нормальный ответ
					// Также ничего не делаем и продолжаем ожидание события в GetOverlappedResult!
					break;
				}
			default:
				{
					// Все другие коды ошибок обозначают появление серьезной ошибки.
					// Обработаем эту ошибку.
					port->ProcessErrorMessage("WaitCommEvent()");
					break;
				}
			}
		}
		else
		{
			// Если WaitCommEvent() возвратит TRUE, проверим для уверенности,
			// что в буффере приема есть байты для чтения. 
			// Если мы прочитаем больше одного байта за раз из буффера (что не делает эта программа)
			// у нас появится ситуация, где первый пришедший байт станет причиной, того
			// что WaitForMultipleObjects() прекратит ожидание. WaitForMultipleObjects()
			// сбросит хэндл события m_ov.hEvent в несигнальное состояние как только возвратиться.
			//
			// Если за время между сбросом этого события и вызовом ReadFile() появились еще байты 
			// хэндл m_ov.hEvent будет установлен в сигнальное состояние.
			// Когда произойдет вызов ReadFile(), будут считаны все байты из буффера и программа
			// возвратиться в цикл ожидания события от WaitCommEvent().
			// 
			// С этого момента вы находитtcm в ситуации, когда m_ov.hEvent установлен,
			// но нет ни одного байта для чтения.  Если вы продолжите и вызовите ReadFile(),
			// то она возвратиться немедленно несмотря что установлен асинхронный режим, а
			// GetOverlappedResults() не возвратиться пока не придет следующий байт.
			//
			// Такое состояние для функции GetOverlappedResults() не желанно. 
			// Событие прекращения потока (Событие 0) и WriteFile() событие (Событие 1)
			// не будут работать если поток заблокирован функцией GetOverlappedResults().
			//
			// Решением этому является проверка буффера вызовом ClearCommError().
			// Этот вызов сбросит хэндл события, и если нет ни одного байта для чтения
			// мы сможем вернутся к функции WaitCommEvent() снова, а затем продолжить.
			// Если имеются байты для чтения, то ничего не делаем и продолжаем.
		
			bResult = ClearCommError(port->m_hComm, &dwError, &comstat);

			// проверка количества байт в очереди
			if (comstat.cbInQue == 0)
				continue;
		}	// конец если bResult

		// Главная функция ожидания.  Эта функция блокирует поток пока не появиться
		// одно из девяти событий.

		Event = WaitForMultipleObjects(3, port->m_hEventArray, FALSE, INFINITE);

		switch (Event)
		{
		case 0:
			{
				// Событие прекращения потока. Номер этого события в массиве событий нулевой,
				// поэтому имеет наивысший приоритет и будет обслужен первым.

			 	port->m_bThreadAlive = FALSE;
				
				// Убиваем этот поток.  break не требуется, но оставим для правильности.
				AfxEndThread(100, TRUE);
				break;
			}
		case 1:	// события по приходу байта в порт
			{
				// определяем маску событий
				GetCommMask(port->m_hComm, &CommEvent);

				if (CommEvent & EV_BREAK)
					::SendMessage(port->m_pOwner->m_hWnd, WM_COMM_BREAK_DETECTED, (WPARAM) 0, (LPARAM) port->m_nPortNr);

				if (CommEvent & EV_CTS)
					::SendMessage(port->m_pOwner->m_hWnd, WM_COMM_CTS_DETECTED, (WPARAM) 0, (LPARAM) port->m_nPortNr);
			
				if (CommEvent & EV_DSR)
					::SendMessage(port->m_pOwner->m_hWnd, WM_COMM_DSR_DETECTED, (WPARAM) 0, (LPARAM) port->m_nPortNr);
			
				if (CommEvent & EV_ERR)
					::SendMessage(port->m_pOwner->m_hWnd, WM_COMM_ERR_DETECTED, (WPARAM) 0, (LPARAM) port->m_nPortNr);
			
				if (CommEvent & EV_EVENT1)
					::SendMessage(port->m_pOwner->m_hWnd, WM_COMM_EVENT1_DETECTED, (WPARAM) 0, (LPARAM) port->m_nPortNr);
			
				if (CommEvent & EV_EVENT2)
					::SendMessage(port->m_pOwner->m_hWnd, WM_COMM_EVENT2_DETECTED, (WPARAM) 0, (LPARAM) port->m_nPortNr);
			
				if (CommEvent & EV_PERR)
					::SendMessage(port->m_pOwner->m_hWnd, WM_COMM_PERR_DETECTED, (WPARAM) 0, (LPARAM) port->m_nPortNr);
			
				if (CommEvent & EV_RING)
					::SendMessage(port->m_pOwner->m_hWnd, WM_COMM_RING_DETECTED, (WPARAM) 0, (LPARAM) port->m_nPortNr);
				
				if (CommEvent & EV_RLSD)
					::SendMessage(port->m_pOwner->m_hWnd, WM_COMM_RLSD_DETECTED, (WPARAM) 0, (LPARAM) port->m_nPortNr);
			
				if (CommEvent & EV_RX80FULL)
					::SendMessage(port->m_pOwner->m_hWnd, WM_COMM_RX80FULL_DETECTED, (WPARAM) 0, (LPARAM) port->m_nPortNr);
			
				if (CommEvent & EV_RXFLAG)
					::SendMessage(port->m_pOwner->m_hWnd, WM_COMM_RXFLAG_DETECTED, (WPARAM) 0, (LPARAM) port->m_nPortNr);
				
				if (CommEvent & EV_TXEMPTY)
					::SendMessage(port->m_pOwner->m_hWnd, WM_COMM_TXEMPTY_DETECTED, (WPARAM) 0, (LPARAM) port->m_nPortNr);

				// событие на считывание байта из порта
				if (CommEvent & EV_RXCHAR)
					ReceiveChar(port, comstat);
				break;
			}  
		case 2: // событие записи
			{
				// событие записи байта в порт.
				WriteChar(port);
				break;
			}

		} // конец switch
				
	} // закрываем вечный цикл

	return 0;
}

//
// запуск потока мониторинга за последовательным портом
//
BOOL CSerialPort::StartMonitoring()
{
	// поверка на существование потока
	if(m_bThreadAlive == TRUE)
		return FALSE;

	// создаем новый поток
	m_Thread = AfxBeginThread(CommThread, this);
	if (m_Thread == NULL)
		return FALSE;
	TRACE("Serial Thread started\n");
	return TRUE;	
}

//
// вывод потока из состояния приостанова
//
BOOL CSerialPort::RestartMonitoring()
{
	// поверка на существование потока
	if(m_bThreadAlive == FALSE)
		return FALSE;

	TRACE("Serial Thread resumed\n");
	// вывод потока
	m_Thread->ResumeThread();
	m_bThreadSuspended = FALSE;
	return TRUE;	
}

//
// ввод потока в состояние приостанова
//
BOOL CSerialPort::StopMonitoring()
{
	// поверка на существование потока
	if(m_bThreadAlive == FALSE)
		return FALSE;

	TRACE("Serial Thread suspended\n");
	// ввод потока
	m_Thread->SuspendThread();
	m_bThreadSuspended = TRUE;
	return TRUE;	
}



//
// определение настоящей причины ошибки
//
void CSerialPort::ProcessErrorMessage(char* ErrorText)
{
	char *Temp = new char[200];
	
	LPVOID lpMsgBuf;

	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	);

	sprintf_s(Temp, 200, "WARNING:  %s Failed with the following error: \n%s\nPort: %d\n", (char*)ErrorText, lpMsgBuf, m_nPortNr); 
	MessageBox(NULL, Temp, "Application Error", MB_ICONSTOP);

	LocalFree(lpMsgBuf);
	delete[] Temp;
}

//
// Запись байта.
//
void CSerialPort::WriteChar(CSerialPort* port)
{
	BOOL bWrite = TRUE;
	BOOL bResult = TRUE;

	DWORD BytesSent = 0;

	ResetEvent(port->m_hWriteEvent);

	// вход в критическую секцию
	EnterCriticalSection(&port->m_csCommunicationSync);

	if (bWrite)
	{
		// Инициализация переменных
		port->m_ov.Offset = 0;		// всегда 0, если последовательный порт
		port->m_ov.OffsetHigh = 0;	// всегда 0, если последовательный порт

		// Очистка буфферов приема, передачи и останов приема, передачи
		PurgeComm(port->m_hComm, PURGE_FLAGS);

		bResult = WriteFile(port->m_hComm,							// Хэндл последовательного порта
							port->m_szWriteBuffer,					// указатель на буффер, переданный в функцию
							port->dwBytesToWrite,					// длина отправляемого сообщения 
							&BytesSent,								// адрес возврата числа отправленных байт
							&port->m_ov);							// асинхронная структура
		
		
		// работаем по всем кодам ошибок
		if (!bResult)  
		{
			// получаем код последней ошибки
			DWORD dwError = GetLastError();
			switch (dwError)
			{
				// незавершенная асинхронная операция
				case ERROR_IO_PENDING:
					{
						// продолжим в GetOverlappedResults()
						// сбрасываем число отправленных байтов
						BytesSent = 0;
						// сбрасывем флаг передачи
						bWrite = FALSE;
						break;
					}
				// все остальные
				default:
					{
						// все остальные ошибки
						port->ProcessErrorMessage("WriteFile()");
					}
			}
		} 
		else
		{
			LeaveCriticalSection(&port->m_csCommunicationSync);
		}
	} // конец if(bWrite)

	if (!bWrite)
	{
		bWrite = TRUE;
		bResult = GetOverlappedResult(port->m_hComm,	// Хэндл последовательного порта 
									  &port->m_ov,		// асинхронная структура
									  &BytesSent,		// адрес возврата числа отправленных байт
									  TRUE); 			// флаг ожидания (TRUE - ожидание окончания операции)
		LeaveCriticalSection(&port->m_csCommunicationSync);

		// обработка кодов ошибок
		if (!bResult)  
		{
			port->ProcessErrorMessage("GetOverlappedResults() in WriteFile()");
		}	
	} // конец if (!bWrite)

	// проверка на равенство числа отправленных и отправляемых байт
	if (BytesSent != (port->dwBytesToWrite))
	{
		TRACE("WARNING: WriteFile() error.. Bytes Sent: %d; Message Length: %d\n", BytesSent, port->dwBytesToWrite/*strlen((char*)port->m_szWriteBuffer)*/);
	}
}

//
// Прием байта. сообщаем родителю. 
//
void CSerialPort::ReceiveChar(CSerialPort* port, COMSTAT comstat)
{
	BOOL  bRead = TRUE; 
	BOOL  bResult = TRUE;
	DWORD dwError = 0;
	DWORD BytesRead = 0;
	unsigned char RXBuff = 0;

	for (;;) 
	{ 
		// Gain ownership of the comm port critical section.
		// This process guarantees no other part of this program 
		// is using the port object. 
		
		EnterCriticalSection(&port->m_csCommunicationSync);

		// ClearCommError() will update the COMSTAT structure and
		// clear any other errors.
		
		bResult = ClearCommError(port->m_hComm, &dwError, &comstat);

		LeaveCriticalSection(&port->m_csCommunicationSync);

		// start forever loop.  I use this type of loop because I
		// do not know at runtime how many loops this will have to
		// run. My solution is to start a forever loop and to
		// break out of it when I have processed all of the
		// data available.  Be careful with this approach and
		// be sure your loop will exit.
		// My reasons for this are not as clear in this sample 
		// as it is in my production code, but I have found this 
		// solutiion to be the most efficient way to do this.
		
		if (comstat.cbInQue == 0)
		{
			// break out when all bytes have been read
//				::SendMessage((port->m_pOwner)->m_hWnd, WM_COMM_CLEAR, (WPARAM) 0, (LPARAM) port->m_nPortNr);

			break;
		}
						
		EnterCriticalSection(&port->m_csCommunicationSync);

		if (bRead)
		{
			bResult = ReadFile(port->m_hComm,		// Handle to COMM port 
							   &RXBuff,				// RX Buffer Pointer
							   1,					// Read one byte
							   &BytesRead,			// Stores number of bytes read
							   &port->m_ov);		// pointer to the m_ov structure
			// deal with the error code 

			if (!bResult)  
			{ 
				switch (dwError = GetLastError()) 
				{ 
					case ERROR_IO_PENDING: 	
						{ 
							// asynchronous i/o is still in progress 
							// Proceed on to GetOverlappedResults();
							bRead = FALSE;
							break;
						}
					default:
						{
							// Another error has occured.  Process this error.
							port->ProcessErrorMessage("ReadFile()");
							break;
						} 
				}
			}
			else
			{
				// ReadFile() returned complete. It is not necessary to call GetOverlappedResults()
				bRead = TRUE;
			}
		}  // close if (bRead)

		if (!bRead)
		{
			bRead = TRUE;
			bResult = GetOverlappedResult(port->m_hComm,	// Handle to COMM port 
										  &port->m_ov,		// Overlapped structure
										  &BytesRead,		// Stores number of bytes read
										  TRUE); 			// Wait flag

			// deal with the error code 
			if (!bResult)  
			{
				port->ProcessErrorMessage("GetOverlappedResults() in ReadFile()");
			}	
		}  // close if (!bRead)
				
		LeaveCriticalSection(&port->m_csCommunicationSync);


		// notify parent that a byte was received
		::SendMessage((port->m_pOwner)->m_hWnd, WM_COMM_RXCHAR, (WPARAM) RXBuff, (LPARAM) port->m_nPortNr);
		

	} // end forever loop
	

}

//
// Write a string to the port
//
void CSerialPort::WriteToPort(unsigned char* string, unsigned int num)
{		
	assert(m_hComm != 0);

	if(string == NULL || num <= 0)
		return;

	EnterCriticalSection(&m_csCommunicationSync);

		memset(m_szWriteBuffer, 0, m_nWriteBufferSize);

		memcpy(m_szWriteBuffer, string, num);

		dwBytesToWrite = (DWORD) num;

	LeaveCriticalSection(&m_csCommunicationSync);

	SetEvent(m_hWriteEvent);
}

//
// Return the device control block
//
DCB CSerialPort::GetDCB()
{
	return m_dcb;
}

//
// Return the communication event masks
//
DWORD CSerialPort::GetCommEvents()
{
	return m_dwCommEvents;
}

//
// Return the output buffer size
//
DWORD CSerialPort::GetWriteBufferSize()
{
	return m_nWriteBufferSize;
}




//Ручная установка бита RTS. 
//Может использован для задания направления передачи данных по RS-485
//
void CSerialPort::SetRTS(bool rts)
{
	if(!rts)
		EscapeCommFunction(m_hComm, SETRTS); 
	else if(rts)
		EscapeCommFunction(m_hComm, CLRRTS);  
}
/*************************************************************************************/

void CSerialPort::ClearRXBuff()
{
	PurgeComm(m_hComm, PURGE_RX);
}

void CSerialPort::ClearTXBuff()
{
	PurgeComm(m_hComm, PURGE_TX);
}

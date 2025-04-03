#include "stdafx.h"
#include "SerialPort.h"
#include <MyTrace_visual_c++.h>				//для trace без MFC
#include <assert.h>							//для assert без MFC

CSerialPort::CSerialPort()
{
	//сброшу номер порта
	m_nPortNr=0;

	//инициализируем хэндл создаваемого файла порта
	m_hComm				= NULL;

	// инициализируем асинхронную структуру нулем (обязательно)
	m_ov.Offset			= 0;
	m_ov.OffsetHigh		= 0;

	// инициализируем хэндлы событий
	m_ov.hEvent			= CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hWriteEvent		= CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hShutdownEvent	= CreateEvent(NULL, TRUE, FALSE, NULL);

	// инициализируем буффер передачи
	m_szWriteBuffer		= NULL;

	// сброс флага существования потока
	m_bThreadAlive		= FALSE;

	// сброс флага останова потока
	m_bThreadSuspended	= FALSE;

	// инициализация критической секции
	//InitializeCriticalSection(&m_csCommunicationSync);

	ParentSupervisor=NULL;	//|
	m_ParentHWND=NULL;		//| о родителе еще ничего не известно
}


//
// Динамческое удаление памяти
//
CSerialPort::~CSerialPort()
{
	//Закрываю порт
	ClosePort();

	// сбрасываем все события
	CloseHandle(m_ov.hEvent);
	CloseHandle(m_hWriteEvent);
	CloseHandle(m_hShutdownEvent);

	// удаляем критическую секцию
	//DeleteCriticalSection(&m_csCommunicationSync);
}

//
// Инициализация порта. Номер порта должен находиться в диапазоне 0 - 255.
//
BOOL CSerialPort::InitPort(HWND  ParentHWND,			// родитель порта
						   UINT  portnr,				// номер порта (1..255)
						   UINT  baud,					// скорость порта
						   char  parity,				// четность 
						   UINT  databits,				// число бит в поле данных 
						   UINT  stopbits,				// число стоповых бит 
						   DWORD dwCommEvents,			// события по приему EV_RXCHAR, EV_CTS и т.д.
						   UINT  writebuffersize)		// размер буффера передачи

{
	//Если это не первая инициализация, то уберу старую
	ClosePort();
	
	// проверка номера порта 
	if(portnr == 0 || portnr > 255) return false;
	//проверка размера буфера для отправки
	if(writebuffersize==0)			return false;

	// сброшу все события
	ResetEvent(m_ov.hEvent);
	ResetEvent(m_hWriteEvent);
	ResetEvent(m_hShutdownEvent);

	// инициализация объектов событий
	m_hEventArray[0] = m_hShutdownEvent;
	m_hEventArray[1] = m_ov.hEvent;
	m_hEventArray[2] = m_hWriteEvent;

	// сохраняем родителя
	m_ParentHWND = ParentHWND;

	// выделяем буффер для записи в порт
	m_szWriteBuffer = new BYTE[writebuffersize];

	// сохраняем номер порта
	m_nPortNr = portnr;

	// установка размера буффера для записи
	m_nWriteBufferSize = writebuffersize;

	// сохраняем маску событий по приему
	m_dwCommEvents = dwCommEvents;

	// временные символьные буфферы
	TCHAR szPort[30];
	TCHAR szBaud[50];
	// символьное описания параметров порта (используется синтаксис команды MODE в DOS)
	// имя порта
	_stprintf_s(szPort, sizeof(szPort)/sizeof(TCHAR), _TEXT("\\\\.\\COM%d\0"), portnr);
	// строка параметров
	_stprintf_s(szBaud, sizeof(szBaud)/sizeof(TCHAR), _TEXT("baud=%d parity=%c data=%d stop=%d\0"), baud, parity, databits, stopbits);

	// открываем порт и сохраняем его хэндл
	m_hComm = CreateFile(szPort,										// строка параметров порта (COMX)
					     GENERIC_READ | GENERIC_WRITE,					// чтение/запись
					     0,												// Устройство Последовательный порт должен быть открыт с параметром 0 и означает, что доступ к объекту и повторное его открытие невозможно пока хэндл не будет закрыт - Исключительный доступ
					     NULL,											// без аттрибутов безопасности
					     OPEN_EXISTING,									// Устройство Последовательный порт должен быть открыт с флагом OPEN_EXISTING (открыть существующий файл)
					     FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,	// Асинхронный ввод/вывод
					     NULL);											// template должен быть NULL для устройств связи

	// проверка хэндла порта 
	if (m_hComm == INVALID_HANDLE_VALUE)
	{// порт не найден
		// удаляем буффер передачи
		delete [] m_szWriteBuffer;
		m_szWriteBuffer = NULL;

		#ifdef _DEBUG
		DWORD lErrCode=GetLastError();			//для отладочных целей узнаю причину сбоя
		TCHAR lErrString[255];
		_stprintf_s(lErrString,sizeof(lErrString)/sizeof(TCHAR),_TEXT("Serial Port %s invalid while init. GetLastError()==%u\n"),szPort,lErrCode);
		TracePrint(lErrString);
		#endif
		return FALSE;
	}

	// установка тайм-аутов связи
	// ReadIntervalTimeout
	// максимальное время ожидания прихода следующего байта, мсек
	// отсчет начинается при получении байта во время работы ReadFile.
	// ReadFile возвращает любой принятый буффер, если интервал превышает указанный
	m_CommTimeouts.ReadIntervalTimeout			= 0;
	// не понятно пока
	m_CommTimeouts.ReadTotalTimeoutMultiplier	= 0;
	m_CommTimeouts.ReadTotalTimeoutConstant		= 0;
	m_CommTimeouts.WriteTotalTimeoutMultiplier	= 0;
	m_CommTimeouts.WriteTotalTimeoutConstant	= 0;

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
						Sleep(60); //так рекомендуется в MSDN, чтобы изменения вступили в силу
					else
						ProcessErrorMessage(_TEXT("SetCommState()"));
				}
				else
					ProcessErrorMessage(_TEXT("BuildCommDCB()"));
			}
			else
				ProcessErrorMessage(_TEXT("GetCommState()"));
		}
		else
			ProcessErrorMessage(_TEXT("SetCommMask()"));
	}
	else
		ProcessErrorMessage(_TEXT("SetCommTimeouts()"));

	// очистка порта
	PurgeComm(m_hComm, PURGE_FLAGS);

	//начну выполнение потока, обслуживающего порт
	if(!StartMonitoring()) return FALSE;

	MyTRACE(_TEXT("Initialisation for communication port %d completed.\n"), portnr);

	return TRUE;
}

/**********************    ClosePort    ****************************************************
*		Эта функция заканчивает работу с портом: прекращает потоки, очищает всю дина-
*	мически выделенную память и т.д. Иными словами, после вызова этой функции с портом
*	можно начать работу только заного проинициализировав его. Оч полезно когда работаю
*	с CP2102, если я ее выдергиваю из компа, мне нужно прикрыть лавочку, чтобы избежать
*	всяких там левых сообщений.
*******************************************************************************************/
void CSerialPort::ClosePort()
{
	// если поток существует, то убиваем
	if (m_bThreadAlive)
	{
		// проверка на останов потока
		if(m_bThreadSuspended)
			ContinueMonitoring();
	
		DWORD dw;
		GetExitCodeThread(m_hThread, &dw);
		if(dw == STILL_ACTIVE) 
		{//поток еще жив
			TerminateThread(m_hThread,100);	//принудительно его остановлю
			m_bThreadAlive=false;
		}
		MyTRACE(_T("Serial Thread shutdowned.\n"));
	}
	
	// если буффер уже выделен, удаляем его
	if (m_szWriteBuffer != NULL)
	{
		delete [] m_szWriteBuffer;
		m_szWriteBuffer = NULL;
	}

	// если порт уже открыт, то закрываем его
	if (m_hComm != NULL || m_hComm!=INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hComm);
		m_hComm = NULL;
		//Без этой задержки последовательность закрыть COMx -> настроить  COMx не
		//работет, т.к. Windows не может закрыть и тут жет открыть тот же самы порт
		Sleep(100);
	}
	MyTRACE(_TEXT("Serial Communication ended.\n"));
}


/*****************************     SetParentSupervisor    ***********************************
*		задание внешнего обработчика
********************************************************************************************/
bool CSerialPort::SetParentSupervisor(void (*parent_supervisor)(UINT Msg, WPARAM wParam, LPARAM lParam))
{
	if(parent_supervisor==NULL) return false;
	ParentSupervisor=parent_supervisor;
	return true;
}


/*************************    StartMonitoring    *******************************
*		Запуск потока мониторинга за последовательным портом
*******************************************************************************/
BOOL CSerialPort::StartMonitoring()
{
	// поверка на существование потока
	if(m_bThreadAlive == TRUE)
		return TRUE;

	// создаем новый поток
	m_hThread = CreateThread(NULL, 0, CommThread, (PVOID) this, 0, NULL);
	if (m_hThread == NULL)
	{
		ProcessErrorMessage(_T("StartMonitoring()"));
		return FALSE;
	}
	TracePrint(_TEXT("Serial Thread started\n"));
	return TRUE;	
}

/******************************    ContinueMonitoring    ************************
*		Вывод потока из состояния приостанова
********************************************************************************/
BOOL CSerialPort::ContinueMonitoring()
{
	// поверка на существование потока
	if(m_bThreadAlive == FALSE)
		return FALSE;

	// восстановление потока
	if(ResumeThread(m_hThread)==-1)
	{//не смогли восстановить выполнение потока
		ProcessErrorMessage(_T("ContinueMonitoring()"));
		return FALSE;
	}
	m_bThreadSuspended = FALSE;
	//++++ TRACE("Serial Thread resumed\n");
	return TRUE;	
}

/********************    StopMonitoring    *****************************
*		Ввод потока в состояние приостанова.
***********************************************************************/
BOOL CSerialPort::StopMonitoring()
{
	// поверка на существование потока
	if(m_bThreadAlive == FALSE)
		return FALSE;

	// приостановка потока
	if(SuspendThread(m_hThread)==-1)
	{//не смогли преостановить поток
		ProcessErrorMessage(_T("StopMonitoring()"));
		return FALSE;
	}
	m_bThreadSuspended = TRUE;
	MyTRACE(_TEXT("Serial Thread suspended\n"));
	return TRUE;	
}

/***********************    ProcessErrorMessage    ****************************
*		Определение настоящей причины ошибки
******************************************************************************/
void CSerialPort::ProcessErrorMessage(TCHAR* ErrorText)
{
	TCHAR Temp[200];
	
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

	_stprintf_s(Temp, 200, _TEXT("WARNING:  %s Failed with the following error: \n%s\nPort: %d\n"), ErrorText, lpMsgBuf, m_nPortNr); 
	MessageBox(NULL, Temp, _TEXT("Application Error"), MB_ICONSTOP);

	LocalFree(lpMsgBuf);
}

/****************************    WriteChar    ************************************
*		Низкоуровневая запись сообщения в порт. Производится уже в потоке порта.
*	Запись производится из буфера отправки.
*********************************************************************************/
bool CSerialPort::WritePortLoLevel(CSerialPort* port)
{
	assert(port);						//есть сам порт
	assert(port->m_szWriteBuffer);		//и буфер откуда отправлять

	BOOL bResult = TRUE;
	DWORD BytesSent = 0;

	// Инициализация переменных
	port->m_ov.Offset = 0;				// всегда 0, если последовательный порт
	port->m_ov.OffsetHigh = 0;			// всегда 0, если последовательный порт

	// Очистка буфферов приема, передачи и останов приема, передачи
	//++++ PurgeComm(port->m_hComm, PURGE_FLAGS);

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
				{//это нормальная ситуация, байты еще не успели отправиться
					// сбрасываем число отправленных байтов
					BytesSent = 0;
					bResult = GetOverlappedResult(port->m_hComm,	// Хэндл последовательного порта 
								  &port->m_ov,		// асинхронная структура
								  &BytesSent,		// адрес возврата числа отправленных байт
								  TRUE); 			// флаг ожидания (TRUE - ожидание окончания операции)
					if (!bResult)  
					{//ошибка
						port->ProcessErrorMessage(_TEXT("GetOverlappedResults() in WriteFile()"));
						return false;
					}
					break;
				}
			default:
				{// все остальные ошибки
					port->ProcessErrorMessage(_TEXT("WriteFile()"));
					return false;
				}
		}
	}


	// проверка на равенство числа отправленных и отправляемых байт
	if (BytesSent != (port->dwBytesToWrite))
	{
		MyTRACE(_TEXT("WARNING: WriteFile() error.. Bytes Sent: %d; Message Length: %d\n"), BytesSent, port->dwBytesToWrite);
	}

	/*DWORD CommEvent;
	GetCommMask(port->m_hComm, &CommEvent);
	if (CommEvent & EV_TXEMPTY) 
	{//все байты переданы
		port->NotifyParent(WM_COMM_TXEMPTY_DETECTED, (WPARAM) 0, (LPARAM) port->m_nPortNr);	
	}*/
//++++	port->NotifyParent(WM_COMM_TXEMPTY_DETECTED, (WPARAM) 0, (LPARAM) port->m_nPortNr);	

	return true;
}

/***************************    ReceiveChar    ***************************************
*		Прием данных и побайтовая отправка его на уровень выше. Я передалал стандартную
*	функцию, принимающую по одному байту, т.к. это сильно тормозило CP2102.
**************************************************************************************/
bool CSerialPort::ReceivePortLoLevel(CSerialPort* port)
{
	BOOL  bResult = TRUE;
	DWORD BytesRead = 0;
	unsigned char RXBuff [100];		//буфер под хранения принятых данных

	for (;;) 
	{//считывать буду в бесконечном цикле, т.к. если я за раз не смогу выбрать
	//сразу весь буфер, то будет вторая, третья и т.д. попытки
		
		COMSTAT comstat;	//сюда будет записана информация о порте
		DWORD dwError=0;	//а сюда информация об ошибках
		ClearCommError(port->m_hComm, &dwError, &comstat);
		//в dwError приходят ошибки на приеме, но я их обрабатывать не буду

		if (comstat.cbInQue == 0)
		{//очередь приема пуста
			// break out when all bytes have been read
//				::SendMessage((port->m_pOwner)->m_hWnd, WM_COMM_CLEAR, (WPARAM) 0, (LPARAM) port->m_nPortNr);
			break;
		}
						
		//Узнаю, сколько байт мне нужно считать
		unsigned int lBytesToRead=comstat.cbInQue;
		//Защищусь от переполнения
		if(lBytesToRead>sizeof(RXBuff)) lBytesToRead=sizeof(RXBuff);

		bResult = ReadFile(port->m_hComm,		// Handle to COMM port 
						   &RXBuff,				// RX Buffer Pointer
						   lBytesToRead,		
						   &BytesRead,			// Stores number of bytes read
						   &port->m_ov);		// pointer to the m_ov structure

		if (!bResult)  
		{ 
			switch (dwError = GetLastError()) 
			{ 
				case ERROR_IO_PENDING: 	
					{//это нормальная ситуация
						bResult = GetOverlappedResult(port->m_hComm,	// Handle to COMM port 
									  &port->m_ov,		// Overlapped structure
									  &BytesRead,		// Stores number of bytes read
									  TRUE); 			// Wait flag

						if (!bResult)  
						{//ошибки
							port->ProcessErrorMessage(_T("GetOverlappedResults() in ReadFile()"));
							return false;
						}
						break;
					}
				case ERROR_BAD_COMMAND:	//22
					{//The device does not recognize the command
						//Такое случается, когда я выдергиваю CP2102
						//До конца еще не разобрался, что нужно делать
						return false;
					}
				default:
					{
						// Another error has occured.  Process this error.
						port->ProcessErrorMessage(_T("ReadFile()"));
						return false;
					}
			}
		}

		//А теперь побайтно отправлю на уровень выше
		unsigned char * ptr=&RXBuff[0];
		for(unsigned int i=0; i<BytesRead; i++)
		{
			//Оповещу того, кому это надо
			port->NotifyParent(WM_COMM_RXCHAR, (WPARAM) *ptr, (LPARAM) port->m_nPortNr);
			ptr++;
		}
	}//конец бесконечного цикла
	return true;
}
/*void CSerialPort::ReceiveChar(CSerialPort* port, COMSTAT comstat)
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
							port->ProcessErrorMessage(_T("ReadFile()"));
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
				port->ProcessErrorMessage(_T("GetOverlappedResults() in ReadFile()"));
			}	
		}  // close if (!bRead)
				
		LeaveCriticalSection(&port->m_csCommunicationSync);


		//Оповещу того, кому это надо
		port->NotifyParent(WM_COMM_RXCHAR, (WPARAM) RXBuff, (LPARAM) port->m_nPortNr);
		

	} // end forever loop
	
//	BOOL  bRead = TRUE; 
//	BOOL  bResult = TRUE;
//	DWORD dwError = 0;
//	DWORD BytesRead = 0;
//	unsigned char RXBuff = 0;
//
//	//В CP2102 ClearCommError выполняется очень долго, поэтому 
//	//вынесу ее за пределы вечного цикла, чтобы она выполнялась пореже
//	bResult = ClearCommError(port->m_hComm, &dwError, &comstat);
//
//	for (;;) 
//	{ 
////		bResult = ClearCommError(port->m_hComm, &dwError, &comstat);
//		if (comstat.cbInQue == 0)
//		{
//			// break out when all bytes have been read
////				::SendMessage((port->m_pOwner)->m_hWnd, WM_COMM_CLEAR, (WPARAM) 0, (LPARAM) port->m_nPortNr);
//
//			break;
//		}
//						
//		if (bRead)
//		{
//			bResult = ReadFile(port->m_hComm,		// Handle to COMM port 
//							   &RXBuff,				// RX Buffer Pointer
//							   1,					// Read one byte
//							   &BytesRead,			// Stores number of bytes read
//							   &port->m_ov);		// pointer to the m_ov structure
//			// deal with the error code 
//
//			if (!bResult)  
//			{ 
//				dwError = GetLastError();
//				if(dwError==ERROR_IO_PENDING)
//				{ 
//					// asynchronous i/o is still in progress 
//					// Proceed on to GetOverlappedResults();
////					break;
//					//Такая хрень часто случается с CP2102
//		//			bRead = TRUE;
//					//Оповещу того, кому это надо
//		//			port->NotifyParent(WM_COMM_RXCHAR, (WPARAM) RXBuff, (LPARAM) port->m_nPortNr);
//				}
//				else if(dwError==ERROR_BAD_COMMAND)	//22
//				{//The device does not recognize the command
//					//Такое случается, когда я выдергиваю CP2102
//					//До конца еще не разобрался, что нужно делать
////					break;
//				}
//				else
//				{
//					// Another error has occured.  Process this error.
//					port->ProcessErrorMessage(_TEXT("ReadFile()"));
//					break;
//				} 
//				ClearCommError(port->m_hComm, &dwError, &comstat);
//			}
//			else
//			{
//				bRead = TRUE;
//				
//			}
//		}  // close if (bRead)
//
//		if (!bRead)
//		{
//			bRead = TRUE;
//			bResult = GetOverlappedResult(port->m_hComm,	// Handle to COMM port 
//										  &port->m_ov,		// Overlapped structure
//										  &BytesRead,		// Stores number of bytes read
//										  TRUE); 			// Wait flag
//
//			// deal with the error code 
//			if (!bResult)  
//			{
//				port->ProcessErrorMessage(_TEXT("GetOverlappedResults() in ReadFile()"));
//			}	
//		}  // close if (!bRead)
//
//		//Оповещу того, кому это надо
//		port->NotifyParent(WM_COMM_RXCHAR, (WPARAM) RXBuff, (LPARAM) port->m_nPortNr);
//	} // end forever loop
}*/

//
// Write a string to the port
//
bool CSerialPort::WriteToPort(const BYTE* pBufToSend, DWORD NumBytes)
{		
	/*if(m_hComm == NULL || m_hComm ==INVALID_HANDLE_VALUE)
		return false;

	if(pBufToSend == NULL || NumBytes <= 0)
		return false;
	if(m_szWriteBuffer==NULL)
	{//порт проинициализирован некорректно и не было выделено места под буфер
		return false;
	}

	if(memcpy_s(m_szWriteBuffer, m_nWriteBufferSize, pBufToSend, NumBytes)!=0)
		return false;
	
	dwBytesToWrite = NumBytes;

	if(!SetEvent(m_hWriteEvent))
		return false;

	return true;*/

	DWORD dwWritten;
	bool success= false;
	OVERLAPPED o= {0};
	o.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!WriteFile(m_hComm, (LPCVOID)pBufToSend, NumBytes, &dwWritten, &o))
	{
		if (GetLastError() == ERROR_IO_PENDING)
			if (WaitForSingleObject(o.hEvent, INFINITE) == WAIT_OBJECT_0)
				if (GetOverlappedResult(m_hComm, &o, &dwWritten, FALSE))
					success = true;
	}
	else
		success = true;

	if (dwWritten != NumBytes)
		success = false;
	CloseHandle(o.hEvent);

	if(success)
	{//все байты переданы
		//это синтетическая заглушка, т.к. TX_EMPTY почему-то не работает
		NotifyParent(WM_COMM_TXEMPTY_DETECTED, (WPARAM) 0, (LPARAM) m_nPortNr);	
	}
	return success;
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

/*********************   NotifyParent    *******************************************
*		Модуль устроен так, что он может работать с родителем как посредством сообщений,
*	так и вызывая при каждом событии какую-либо внешнюю функцию. Эта функцию производит 
*	унификацию таких уведомлений.
***********************************************************************************/
void CSerialPort::NotifyParent(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if(ParentSupervisor!=NULL)
	{//нужно вызвать внешнуюю функцию
		ParentSupervisor(Msg, wParam, lParam);
	}
	else if(m_ParentHWND!=NULL)
	{//нужно отправить сообщение окну-родителю
		PostMessage(m_ParentHWND,Msg,wParam,lParam);
	}
	else
	{
		MessageBox(NULL,_TEXT("Событие Serial Port некуда отправлять."), _TEXT("Serial Port"),MB_ICONSTOP);
	}
}

//
//  Главная функция потока CommThread.
//  pParam - указатель на CSerialPort
DWORD WINAPI CommThread(PVOID pParam)
{
	assert(pParam);

	// Приведение указателя на тип void переданный в поток (AfxBeginThread(..., !this!))
	// назад к указателю на класс CSerialPort
	CSerialPort *lpPort = (CSerialPort*)pParam;
	
	// установка флага состояния потока (информация для родителя)  
	// TRUE - для индикации, что поток запущен.
	lpPort->m_bThreadAlive = TRUE;	
		
	// Прочие переменные
	// маска события
	DWORD Event		= 0;
	DWORD CommEvent = 0;
	// флаг резудьтата, предполагаем что все будет ОК!
	BOOL  bResult	= TRUE;
			
	// Очищаем буффер перед запуском
	if (lpPort->m_hComm)		// проверяем открыт ли порт
		PurgeComm(lpPort->m_hComm, PURGE_FLAGS);

	// начинаем вечный цикл.  Цикл будет существовать пока работает поток.
	for (;;) 
	{ 	// WaitCommEvent ожидает появление события из установленных для данного порта.
		// Вызываем WaitCommEvent(). Этот вызов возвратится немедленно,
		// потому что наш порт создан как асинхронный порт (FILE_FLAG_OVERLAPPED
		// и используется асинхронная структура m_ov). Функция установит элемент m_ov.hEvent,
		// который является частью массива событий m_hEventArray, в несигнальное состояние,  
		// если нет ни одного байта для чтения или в сигнальное состояние,  
		// если есть что прочитать из порта. Если этот хэндл события установлен в несигнальное состояние,  
		// то по приходу символа (байта) в порт он будет установлен в сигнальное состояние.
		Event=0;	//на всякий случай сброшу
		lpPort->m_ov.Internal = 0;
		lpPort->m_ov.InternalHigh = 0;
		lpPort->m_ov.Offset = 0;
		lpPort->m_ov.OffsetHigh = 0;

		bResult = WaitCommEvent(lpPort->m_hComm, &Event, &lpPort->m_ov);

		// Если асинхронная операция не может быть завершена немедленно, то вернется FALSE
		if (!bResult)  
		{ 
			// Если WaitCommEvent() возвратит FALSE, то обработаем последнюю ошибку
			// для определения причины..
			DWORD dwError;
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
			case ERROR_INVALID_PARAMETER:	//номер 87
				{
					// В Windows NT, этот код возвращается по нескольким причинам.
					// И это тоже нормальный ответ
					// Также ничего не делаем и продолжаем ожидание события в GetOverlappedResult!
					break;
				}
			case ERROR_ACCESS_DENIED:		//номер 5
			case ERROR_OPERATION_ABORTED:	//номер 995
			case ERROR_BAD_COMMAND:			//номер 22
				{
					//Закрыт доcтуп к порту. Это возникает, если он был, а теперь нету, 
					//например, как будто внезщапно отключили CP2102
					lpPort->NotifyParent(WM_COMM_UNCOWN_ERROR, (WPARAM) 0, (LPARAM) lpPort->m_nPortNr);
					//Производители CP210x рекомендуют при этом закрыть хэндл, я пока этого делать не буду
					break;
				}
			default:
				{
					// Все другие коды ошибок обозначают появление серьезной ошибки.
					// Обработаем эту ошибку.
					lpPort->NotifyParent(WM_COMM_ACCESS_DENIED, (WPARAM) 0, (LPARAM) lpPort->m_nPortNr);
					lpPort->ProcessErrorMessage(_TEXT("WaitCommEvent()"));
					//завершу поток
					goto EndOfCommThread;
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
	
			//bResult = ClearCommError(lpPort->m_hComm, &dwError, &comstat);

			// проверка количества байт в очереди
			//if (comstat.cbInQue == 0)
			//	continue;
		}	// конец если bResult

		// Главная функция ожидания.  Эта функция блокирует поток пока не появиться
		// одно из девяти событий.

		DWORD lObjectNum = WaitForMultipleObjects(3, lpPort->m_hEventArray, FALSE, INFINITE);

		switch (lObjectNum)
		{
		case WAIT_OBJECT_0:
			{//ПРЕКРАЩЕНИЕ ПОТОКА
			 	lpPort->m_bThreadAlive = FALSE;
				// Убиваем этот поток.
				return 100;
			}
		case WAIT_OBJECT_0+1:	// события по приходу байта в порт
			{//ЧТЕНИЕ ПОРТА
				//сброшу событие на порту
				if(!ResetEvent(lpPort->m_hEventArray[1]))
				{//ошибка
					lpPort->NotifyParent(WM_COMM_UNCOWN_ERROR, (WPARAM) 0, (LPARAM) lpPort->m_nPortNr);
					lpPort->ClosePort();
					return false;
				}

				/*CommEvent=0;		//на всякий случай сборшу
				if(!GetCommMask(lpPort->m_hComm, &CommEvent))
				{
					MyTRACE(_TEXT("Ошибка в функции GetCommMask()"));
				}

				DWORD dwError;
				COMSTAT comstat;
				if(!ClearCommError(lpPort->m_hComm, &dwError, &comstat))
				{//сброшу ее
					lpPort->ProcessErrorMessage(_T("ClearCommError() in ReceiveChar()"));
				}*/

				// событие на считывание байта из порта
				if ((Event & EV_RXCHAR) || (CommEvent & EV_RXCHAR))
				{
					if(!lpPort->ReceivePortLoLevel(lpPort))
					{//какие-то непонятные ошибки при приеме
						lpPort->NotifyParent(WM_COMM_UNCOWN_ERROR, (WPARAM) 0, (LPARAM) lpPort->m_nPortNr);
						lpPort->ClosePort();
						return false;
					}
				}

				if (CommEvent & EV_BREAK)
					lpPort->NotifyParent(WM_COMM_BREAK_DETECTED, (WPARAM) 0, (LPARAM) lpPort->m_nPortNr);

				if (CommEvent & EV_CTS)
					lpPort->NotifyParent(WM_COMM_CTS_DETECTED, (WPARAM) 0, (LPARAM) lpPort->m_nPortNr);
			
				if (CommEvent & EV_DSR)
					lpPort->NotifyParent(WM_COMM_DSR_DETECTED, (WPARAM) 0, (LPARAM) lpPort->m_nPortNr);
			
				if (CommEvent & EV_ERR)
				{//в порту произошла ошибка
					/*if(!ClearCommError(lpPort->m_hComm, &dwError, &comstat))
					{//сброшу ее
						lpPort->ProcessErrorMessage(_T("ClearCommError() in ReceiveChar()"));
					}*/
					//уведомлю родителя
					lpPort->NotifyParent(WM_COMM_ERR_DETECTED, (WPARAM) 0, (LPARAM) lpPort->m_nPortNr);
				}
			
				if (CommEvent & EV_EVENT1)
					lpPort->NotifyParent(WM_COMM_EVENT1_DETECTED, (WPARAM) 0, (LPARAM) lpPort->m_nPortNr);
			
				if (CommEvent & EV_EVENT2)
					lpPort->NotifyParent(WM_COMM_EVENT2_DETECTED, (WPARAM) 0, (LPARAM) lpPort->m_nPortNr);
			
				if (CommEvent & EV_PERR)
					lpPort->NotifyParent(WM_COMM_PERR_DETECTED, (WPARAM) 0, (LPARAM) lpPort->m_nPortNr);
			
				if (CommEvent & EV_RING)
					lpPort->NotifyParent(WM_COMM_RING_DETECTED, (WPARAM) 0, (LPARAM) lpPort->m_nPortNr);
				
				if (CommEvent & EV_RLSD)
					lpPort->NotifyParent(WM_COMM_RLSD_DETECTED, (WPARAM) 0, (LPARAM) lpPort->m_nPortNr);
			
				if (CommEvent & EV_RX80FULL)
					lpPort->NotifyParent(WM_COMM_RX80FULL_DETECTED, (WPARAM) 0, (LPARAM) lpPort->m_nPortNr);
			
				if (CommEvent & EV_RXFLAG)
					lpPort->NotifyParent(WM_COMM_RXFLAG_DETECTED, (WPARAM) 0, (LPARAM) lpPort->m_nPortNr);
				
				if ((CommEvent & EV_TXEMPTY) || (Event & EV_TXEMPTY))
				{//отправлено всё сообщение
					char t=5;
					//не работает на CP2102 lpPort->NotifyParent(WM_COMM_TXEMPTY_DETECTED, (WPARAM) 0, (LPARAM) lpPort->m_nPortNr);	
				}

				break;
			}  
		case WAIT_OBJECT_0+2: // событие записи
			{//ЗАПИСЬ В ПОРТ
				//сброшу событие по записи в порт
				if(!ResetEvent(lpPort->m_hWriteEvent))
				{//ошибка
					lpPort->NotifyParent(WM_COMM_UNCOWN_ERROR, (WPARAM) 0, (LPARAM) lpPort->m_nPortNr);
					lpPort->ClosePort();
					return false;
				}
				//запишу данные в порт
				if(!lpPort->WritePortLoLevel(lpPort))
				{//ошибка
					MyTRACE(_TEXT("Ошибка при низкоуровневой записи в порт"));
					lpPort->NotifyParent(WM_COMM_UNCOWN_ERROR, (WPARAM) 0, (LPARAM) lpPort->m_nPortNr);
					lpPort->ClosePort();
					return false;
				}

				//заплатка, т.к. на CP2102 не работает EV_TXEMPTY
				lpPort->NotifyParent(WM_COMM_TXEMPTY_DETECTED, (WPARAM) 0, (LPARAM) lpPort->m_nPortNr);	


				//Обнаружен серьёзный недочет, по которому проверить пустоту отправляющего
				//буфера можно проверить только приняв очередной символо, переписав обработ-
				//ку этого флага сюда, я смог добиться распознавания этого флага почаще,
				//но до сих пор не понятно, правильно это или нет!
				/*for(;;)
				{
					GetCommMask(lpPort->m_hComm, &CommEvent);
					if (CommEvent & EV_TXEMPTY) 
					{//все байты переданы
						lpPort->NotifyParent(WM_COMM_TXEMPTY_DETECTED, (WPARAM) 0, (LPARAM) lpPort->m_nPortNr);	
						break;
					}
				}*/
				/*if (Event & EV_TXEMPTY)
					char t=5;*/
				break;
			}

		} // конец switch
	} // закрываем вечный цикл
EndOfCommThread:
	TracePrint("Serial Thread ended");
	return 0;
}
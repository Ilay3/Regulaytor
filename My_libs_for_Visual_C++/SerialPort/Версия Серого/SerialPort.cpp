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
��� ����� ���������� ���������� �� �������� ��������������, ����� ������������
������ �������, ���������� �� �������� ��� ����������� �� �� �����. 
�������� strcut ������ strcut_s. 
*/
#define _CRT_SECURE_NO_DEPRECATE 


CSerialPort::CSerialPort()
{
	//�������������� ����� ������������ ����� (�����)
	m_hComm				= NULL;

	// �������������� ����������� ��������� ����� (�����������)
	m_ov.Offset			= 0;
	m_ov.OffsetHigh		= 0;

	// �������������� ������ �������
	m_ov.hEvent			= NULL;
	m_hWriteEvent		= NULL;
	m_hShutdownEvent	= NULL;

	// �������������� ������ ��������
	m_szWriteBuffer		= NULL;

	// ����� ����� ������������� ������
	m_bThreadAlive		= FALSE;

	// ����� ����� �������� ������
	m_bThreadSuspended	= FALSE;
}


//
// ����������� �������� ������
//
CSerialPort::~CSerialPort()
{
	// ���� ����� ����������, �� ������� - �������� ������� m_hShutdownEvent
	if (m_bThreadAlive)
	{
		// �������� �� ������� ������
		if(m_bThreadSuspended)
			RestartMonitoring();
	
		// �������� ������� m_hShutdownEvent, ���� ����� �� �������
		do
		{
			SetEvent(m_hShutdownEvent);
		} while (m_bThreadAlive == TRUE);

		// ������� ��������� ������� ������
		m_Thread = NULL;
		TRACE("Serial Thread shutdowned.\n");
	}

	// ���������� ��� �������
	if (m_ov.hEvent != NULL)
		ResetEvent(m_ov.hEvent);
	if (m_hWriteEvent != NULL)
		ResetEvent(m_hWriteEvent);
	if (m_hShutdownEvent != NULL)
		ResetEvent(m_hShutdownEvent);

	// ������� ������
	if (m_szWriteBuffer != NULL)
	{
		delete [] m_szWriteBuffer;
		m_szWriteBuffer=NULL;
	}

	// ��������� �������� ����� ����
	if (m_hComm != NULL)
	{
		CloseHandle(m_hComm);
		m_hComm = NULL;
		TRACE("Serial Port closed.\n");
	}

	// ������� ����������� ������
	DeleteCriticalSection(&m_csCommunicationSync);

	TRACE("Serial Communication ended.\n");
}

//
// ������������� �����. ����� ����� ������ ���������� � ��������� 0 - 255.
//
BOOL CSerialPort::InitPort(CWnd* pPortOwner,	// �������� �����
						   UINT  portnr,				// ����� ����� (1..255)
						   UINT  baud,					// �������� �����
						   char  parity,				// �������� 
						   UINT  databits,				// ����� ��� � ���� ������ 
						   UINT  stopbits,				// ����� �������� ��� 
						   DWORD dwCommEvents,			// ������� �� ������ EV_RXCHAR, EV_CTS � �.�.
						   UINT  writebuffersize)		// ������ ������� ��������

{
	// �������� ������ ����� 
	assert(portnr > 0 && portnr < 256);
	// �������� �� ������������� ��������
	assert(pPortOwner != NULL);

	// ���� ����� ����������: �������
	if (m_bThreadAlive)
	{
		// �������� �� ������� ������
		if(m_bThreadSuspended)
			RestartMonitoring();
	
		// �������� ������� m_hShutdownEvent, ���� ����� �� �������
		do
		{
			SetEvent(m_hShutdownEvent);
		} while (m_bThreadAlive == TRUE);
		// ������� ��������� ������� ������
		m_Thread = NULL;
		TRACE("Serial Thread shutdowned while init\n");
	}

	// ������� �������, �������������� �������
	if (m_ov.hEvent != NULL)
		ResetEvent(m_ov.hEvent);
	m_ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (m_hWriteEvent != NULL)
		ResetEvent(m_hWriteEvent);
	m_hWriteEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	
	if (m_hShutdownEvent != NULL)
		ResetEvent(m_hShutdownEvent);
	m_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// ������������� �������� �������
	m_hEventArray[0] = m_hShutdownEvent;	// ��������� ���������
	m_hEventArray[1] = m_ov.hEvent;
	m_hEventArray[2] = m_hWriteEvent;

	// ������������� ����������� ������
	InitializeCriticalSection(&m_csCommunicationSync);
	
	// ��������� ��������
	m_pOwner = pPortOwner;

	// ���� ������ ��� �������, ������� ���
	if (m_szWriteBuffer != NULL)
	{
		delete [] m_szWriteBuffer;
		m_szWriteBuffer = NULL;
	}
	// �������� ������ ��� ������ � ����
	m_szWriteBuffer = new unsigned char[writebuffersize];

	// ��������� ����� �����
	m_nPortNr = portnr;

	// ��������� ������� ������� ��� ������
	m_nWriteBufferSize = writebuffersize;

	// ��������� ����� ������� �� ������
	m_dwCommEvents = dwCommEvents;

//	BOOL bResult = FALSE;
	
	// ��������� ���������� �������
	char *szPort = new char[50];
	char *szBaud = new char[50];

	// ������ ��������!
	EnterCriticalSection(&m_csCommunicationSync);

	// ���� ���� ��� ������, �� ��������� ���
	if (m_hComm != NULL)
	{
		CloseHandle(m_hComm);
		m_hComm = NULL;
		TRACE("Serial Port closed while init\n");
	}

	// ���������� �������� ���������� ����� (������������ ��������� ������� MODE � DOS)
	// ��� �����
	sprintf_s(szPort, 50, "COM%d", portnr);
	// ������ ����������
	sprintf_s(szBaud, 50, "baud=%d parity=%c data=%d stop=%d", baud, parity, databits, stopbits);

	// ��������� ���� � ��������� ��� �����
	m_hComm = CreateFile(szPort,						// ������ ���������� ����� (COMX)
					     GENERIC_READ | GENERIC_WRITE,	// ������/������
					     0,								// ���������� ���������������� ���� ������ ���� ������ � ���������� 0 � ��������, ��� ������ � ������� � ��������� ��� �������� ���������� ���� ����� �� ����� ������ - �������������� ������
					     NULL,							// ��� ���������� ������������
					     OPEN_EXISTING,					// ���������� ���������������� ���� ������ ���� ������ � ������ OPEN_EXISTING (������� ������������ ����)
					     FILE_FLAG_OVERLAPPED,			// ����������� ����/�����
					     NULL);							// template ������ ���� NULL ��� ��������� �����

	// �������� ������ ����� 
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		// ���� �� ������
		// ������� ��������� �������
		delete [] szPort;
		delete [] szBaud;
		szPort = NULL;
		szBaud = NULL;

		// ������� ������ ��������
		delete [] m_szWriteBuffer;
		m_szWriteBuffer = NULL;

		// ����������� ����������� ������
		LeaveCriticalSection(&m_csCommunicationSync);
		TRACE("Serial Port invalid while init\n");
		return FALSE;
	}

	// ��������� ����-����� �����
	// ReadIntervalTimeout
	// ������������ ����� �������� ������� ���������� �����, ����
	// ������ ���������� ��� ��������� ����� �� ����� ������ ReadFile.
	// ReadFile ���������� ����� �������� ������, ���� �������� ��������� ���������
	m_CommTimeouts.ReadIntervalTimeout			= 1000;
	// �� ������� ����
	m_CommTimeouts.ReadTotalTimeoutMultiplier	= 1000;
	m_CommTimeouts.ReadTotalTimeoutConstant		= 1000;
	m_CommTimeouts.WriteTotalTimeoutMultiplier	= 1000;
	m_CommTimeouts.WriteTotalTimeoutConstant	= 1000;

	// ��������� �����
	// ��������� ����-�����
	if (SetCommTimeouts(m_hComm, &m_CommTimeouts))
	{						
		// ��������� ����� ����������� ������� �� ������ �����
		if (SetCommMask(m_hComm, dwCommEvents))
		{
			// ���������� ��������� DCB �������� ����������� �����
			if (GetCommState(m_hComm, &m_dcb))
			{
				// ��������� ����������� ��������
				// ���������� ����� RTS:
				// RTS_CONTROL_ENABLE - ��
				m_dcb.fRtsControl	= RTS_CONTROL_ENABLE;
				// RTS_CONTROL_TOGGLE - ������������� ��� RTS � 1, ���� ������� ����� ��� ��������.
				// ����� �������� ����� ������, ��� RTS ��������� � 0. 
				// ������ ��� Windows 2000/XP/2003
				//m_dcb.fRtsControl	= RTS_CONTROL_TOGGLE;		

				// ��� ������ ����!!!
				m_dcb.fNull			= FALSE;	

				// ���������� ��������� DCB ����������� �� ������ �������� ����� 
				if (BuildCommDCB(szBaud, &m_dcb))
				{
					// ���������������� ����� � ������������ � ����������� � DCB
					if (SetCommState(m_hComm, &m_dcb))
						; // ���������� ��������... ����������
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

	// ������� �����
	PurgeComm(m_hComm, PURGE_FLAGS);

	// ������������ ����������� ������
	LeaveCriticalSection(&m_csCommunicationSync);

	TRACE("Initialisation for communicationport %d completed.\nUse Startmonitor to communicate.\n", portnr);


	return TRUE;
}

//
//  ������� ������������ ������� ������ CommThread.
//
UINT CSerialPort::CommThread(LPVOID pParam)
{
	// ���������� ��������� �� ��� void ���������� � ����� (AfxBeginThread(..., !this!))
	// ����� � ��������� �� ����� CSerialPort
	CSerialPort *port = (CSerialPort*)pParam;
	
	// ��������� ����� ��������� ������ (���������� ��� ��������)  
	// TRUE - ��� ���������, ��� ����� �������.
	port->m_bThreadAlive = TRUE;	
		
	// ������ ����������
	// ���� ��������
//	DWORD BytesTransfered = 0; 
	// ����� �������
	DWORD Event		= 0;
	DWORD CommEvent = 0;
	// ����� ������
	DWORD dwError	= 0;
	// ���������, ���������� ���������� �� ����� 
	COMSTAT comstat;
	// ���� ����������, ������������ ��� ��� ����� ��!
	BOOL  bResult	= TRUE;
	// ��������� ��� 2005vc++, ������� �� �������������������� comstat
	// ��������� ��������� COMSTAT
	ClearCommError(port->m_hComm, &dwError, &comstat);

		
	// ������� ������ ����� ��������
	if (port->m_hComm)		// ��������� ������ �� ����
		PurgeComm(port->m_hComm, PURGE_FLAGS);

	// �������� ������ ����.  ���� ����� ������������ ���� �������� �����.
	for (;;) 
	{ 

		// WaitCommEvent ������� ��������� ������� �� ������������� ��� ������� �����.
		// �������� WaitCommEvent(). ���� ����� ������������ ����������,
		// ������ ��� ��� ���� ������ ��� ����������� ���� (FILE_FLAG_OVERLAPPED
		// � ������������ ����������� ��������� m_ov). ������� ��������� ������� m_ov.hEvent,
		// ������� �������� ������ ������� ������� m_hEventArray, � ������������ ���������,  
		// ���� ��� �� ������ ����� ��� ������ ��� � ���������� ���������,  
		// ���� ���� ��� ��������� �� �����. ���� ���� ����� ������� ���������� � ������������ ���������,  
		// �� �� ������� ������� (�����) � ���� �� ����� ���������� � ���������� ���������.

		bResult = WaitCommEvent(port->m_hComm, &Event, &port->m_ov);

		// ���� ����������� �������� �� ����� ���� ��������� ����������, �� �������� FALSE
		if (!bResult)  
		{ 
			// ���� WaitCommEvent() ��������� FALSE, �� ���������� ��������� ������
			// ��� ����������� �������..
			switch (dwError = GetLastError()) 
			{ 
			case ERROR_IO_PENDING: 	
				{ 
					// ��� ���������� ��� �������� - �������� �������� � ������� ������.
					// ������� ������������� m_ov.hEvent � ������������ ��������� �� �������� �� WaitCommEvent.
					// ����� ������� ������������� m_ov.hEvent � ���������� ��������� 
					// ��� ��������� ������� ��� ������������� ������.
					// ������� ������ �� ������ � ���������� �������� ������� � GetOverlappedResult!
					break;
				}
			case 87:
				{
					// � Windows NT, ���� ��� ������������ �� ���������� ��������.
					// � ��� ���� ���������� �����
					// ����� ������ �� ������ � ���������� �������� ������� � GetOverlappedResult!
					break;
				}
			default:
				{
					// ��� ������ ���� ������ ���������� ��������� ��������� ������.
					// ���������� ��� ������.
					port->ProcessErrorMessage("WaitCommEvent()");
					break;
				}
			}
		}
		else
		{
			// ���� WaitCommEvent() ��������� TRUE, �������� ��� �����������,
			// ��� � ������� ������ ���� ����� ��� ������. 
			// ���� �� ��������� ������ ������ ����� �� ��� �� ������� (��� �� ������ ��� ���������)
			// � ��� �������� ��������, ��� ������ ��������� ���� ������ ��������, ����
			// ��� WaitForMultipleObjects() ��������� ��������. WaitForMultipleObjects()
			// ������� ����� ������� m_ov.hEvent � ������������ ��������� ��� ������ ������������.
			//
			// ���� �� ����� ����� ������� ����� ������� � ������� ReadFile() ��������� ��� ����� 
			// ����� m_ov.hEvent ����� ���������� � ���������� ���������.
			// ����� ���������� ����� ReadFile(), ����� ������� ��� ����� �� ������� � ���������
			// ������������ � ���� �������� ������� �� WaitCommEvent().
			// 
			// � ����� ������� �� �������tcm � ��������, ����� m_ov.hEvent ����������,
			// �� ��� �� ������ ����� ��� ������.  ���� �� ���������� � �������� ReadFile(),
			// �� ��� ������������ ���������� �������� ��� ���������� ����������� �����, �
			// GetOverlappedResults() �� ������������ ���� �� ������ ��������� ����.
			//
			// ����� ��������� ��� ������� GetOverlappedResults() �� �������. 
			// ������� ����������� ������ (������� 0) � WriteFile() ������� (������� 1)
			// �� ����� �������� ���� ����� ������������ �������� GetOverlappedResults().
			//
			// �������� ����� �������� �������� ������� ������� ClearCommError().
			// ���� ����� ������� ����� �������, � ���� ��� �� ������ ����� ��� ������
			// �� ������ �������� � ������� WaitCommEvent() �����, � ����� ����������.
			// ���� ������� ����� ��� ������, �� ������ �� ������ � ����������.
		
			bResult = ClearCommError(port->m_hComm, &dwError, &comstat);

			// �������� ���������� ���� � �������
			if (comstat.cbInQue == 0)
				continue;
		}	// ����� ���� bResult

		// ������� ������� ��������.  ��� ������� ��������� ����� ���� �� ���������
		// ���� �� ������ �������.

		Event = WaitForMultipleObjects(3, port->m_hEventArray, FALSE, INFINITE);

		switch (Event)
		{
		case 0:
			{
				// ������� ����������� ������. ����� ����� ������� � ������� ������� �������,
				// ������� ����� ��������� ��������� � ����� �������� ������.

			 	port->m_bThreadAlive = FALSE;
				
				// ������� ���� �����.  break �� ���������, �� ������� ��� ������������.
				AfxEndThread(100, TRUE);
				break;
			}
		case 1:	// ������� �� ������� ����� � ����
			{
				// ���������� ����� �������
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

				// ������� �� ���������� ����� �� �����
				if (CommEvent & EV_RXCHAR)
					ReceiveChar(port, comstat);
				break;
			}  
		case 2: // ������� ������
			{
				// ������� ������ ����� � ����.
				WriteChar(port);
				break;
			}

		} // ����� switch
				
	} // ��������� ������ ����

	return 0;
}

//
// ������ ������ ����������� �� ���������������� ������
//
BOOL CSerialPort::StartMonitoring()
{
	// ������� �� ������������� ������
	if(m_bThreadAlive == TRUE)
		return FALSE;

	// ������� ����� �����
	m_Thread = AfxBeginThread(CommThread, this);
	if (m_Thread == NULL)
		return FALSE;
	TRACE("Serial Thread started\n");
	return TRUE;	
}

//
// ����� ������ �� ��������� �����������
//
BOOL CSerialPort::RestartMonitoring()
{
	// ������� �� ������������� ������
	if(m_bThreadAlive == FALSE)
		return FALSE;

	TRACE("Serial Thread resumed\n");
	// ����� ������
	m_Thread->ResumeThread();
	m_bThreadSuspended = FALSE;
	return TRUE;	
}

//
// ���� ������ � ��������� �����������
//
BOOL CSerialPort::StopMonitoring()
{
	// ������� �� ������������� ������
	if(m_bThreadAlive == FALSE)
		return FALSE;

	TRACE("Serial Thread suspended\n");
	// ���� ������
	m_Thread->SuspendThread();
	m_bThreadSuspended = TRUE;
	return TRUE;	
}



//
// ����������� ��������� ������� ������
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
// ������ �����.
//
void CSerialPort::WriteChar(CSerialPort* port)
{
	BOOL bWrite = TRUE;
	BOOL bResult = TRUE;

	DWORD BytesSent = 0;

	ResetEvent(port->m_hWriteEvent);

	// ���� � ����������� ������
	EnterCriticalSection(&port->m_csCommunicationSync);

	if (bWrite)
	{
		// ������������� ����������
		port->m_ov.Offset = 0;		// ������ 0, ���� ���������������� ����
		port->m_ov.OffsetHigh = 0;	// ������ 0, ���� ���������������� ����

		// ������� �������� ������, �������� � ������� ������, ��������
		PurgeComm(port->m_hComm, PURGE_FLAGS);

		bResult = WriteFile(port->m_hComm,							// ����� ����������������� �����
							port->m_szWriteBuffer,					// ��������� �� ������, ���������� � �������
							port->dwBytesToWrite,					// ����� ������������� ��������� 
							&BytesSent,								// ����� �������� ����� ������������ ����
							&port->m_ov);							// ����������� ���������
		
		
		// �������� �� ���� ����� ������
		if (!bResult)  
		{
			// �������� ��� ��������� ������
			DWORD dwError = GetLastError();
			switch (dwError)
			{
				// ������������� ����������� ��������
				case ERROR_IO_PENDING:
					{
						// ��������� � GetOverlappedResults()
						// ���������� ����� ������������ ������
						BytesSent = 0;
						// ��������� ���� ��������
						bWrite = FALSE;
						break;
					}
				// ��� ���������
				default:
					{
						// ��� ��������� ������
						port->ProcessErrorMessage("WriteFile()");
					}
			}
		} 
		else
		{
			LeaveCriticalSection(&port->m_csCommunicationSync);
		}
	} // ����� if(bWrite)

	if (!bWrite)
	{
		bWrite = TRUE;
		bResult = GetOverlappedResult(port->m_hComm,	// ����� ����������������� ����� 
									  &port->m_ov,		// ����������� ���������
									  &BytesSent,		// ����� �������� ����� ������������ ����
									  TRUE); 			// ���� �������� (TRUE - �������� ��������� ��������)
		LeaveCriticalSection(&port->m_csCommunicationSync);

		// ��������� ����� ������
		if (!bResult)  
		{
			port->ProcessErrorMessage("GetOverlappedResults() in WriteFile()");
		}	
	} // ����� if (!bWrite)

	// �������� �� ��������� ����� ������������ � ������������ ����
	if (BytesSent != (port->dwBytesToWrite))
	{
		TRACE("WARNING: WriteFile() error.. Bytes Sent: %d; Message Length: %d\n", BytesSent, port->dwBytesToWrite/*strlen((char*)port->m_szWriteBuffer)*/);
	}
}

//
// ����� �����. �������� ��������. 
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




//������ ��������� ���� RTS. 
//����� ����������� ��� ������� ����������� �������� ������ �� RS-485
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

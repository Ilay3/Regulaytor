#include "stdafx.h"
#include "SerialPort.h"
#include <MyTrace_visual_c++.h>				//��� trace ��� MFC
#include <assert.h>							//��� assert ��� MFC

CSerialPort::CSerialPort()
{
	//������ ����� �����
	m_nPortNr=0;

	//�������������� ����� ������������ ����� �����
	m_hComm				= NULL;

	// �������������� ����������� ��������� ����� (�����������)
	m_ov.Offset			= 0;
	m_ov.OffsetHigh		= 0;

	// �������������� ������ �������
	m_ov.hEvent			= CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hWriteEvent		= CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hShutdownEvent	= CreateEvent(NULL, TRUE, FALSE, NULL);

	// �������������� ������ ��������
	m_szWriteBuffer		= NULL;

	// ����� ����� ������������� ������
	m_bThreadAlive		= FALSE;

	// ����� ����� �������� ������
	m_bThreadSuspended	= FALSE;

	// ������������� ����������� ������
	//InitializeCriticalSection(&m_csCommunicationSync);

	ParentSupervisor=NULL;	//|
	m_ParentHWND=NULL;		//| � �������� ��� ������ �� ��������
}


//
// ����������� �������� ������
//
CSerialPort::~CSerialPort()
{
	//�������� ����
	ClosePort();

	// ���������� ��� �������
	CloseHandle(m_ov.hEvent);
	CloseHandle(m_hWriteEvent);
	CloseHandle(m_hShutdownEvent);

	// ������� ����������� ������
	//DeleteCriticalSection(&m_csCommunicationSync);
}

//
// ������������� �����. ����� ����� ������ ���������� � ��������� 0 - 255.
//
BOOL CSerialPort::InitPort(HWND  ParentHWND,			// �������� �����
						   UINT  portnr,				// ����� ����� (1..255)
						   UINT  baud,					// �������� �����
						   char  parity,				// �������� 
						   UINT  databits,				// ����� ��� � ���� ������ 
						   UINT  stopbits,				// ����� �������� ��� 
						   DWORD dwCommEvents,			// ������� �� ������ EV_RXCHAR, EV_CTS � �.�.
						   UINT  writebuffersize)		// ������ ������� ��������

{
	//���� ��� �� ������ �������������, �� ����� ������
	ClosePort();
	
	// �������� ������ ����� 
	if(portnr == 0 || portnr > 255) return false;
	//�������� ������� ������ ��� ��������
	if(writebuffersize==0)			return false;

	// ������ ��� �������
	ResetEvent(m_ov.hEvent);
	ResetEvent(m_hWriteEvent);
	ResetEvent(m_hShutdownEvent);

	// ������������� �������� �������
	m_hEventArray[0] = m_hShutdownEvent;
	m_hEventArray[1] = m_ov.hEvent;
	m_hEventArray[2] = m_hWriteEvent;

	// ��������� ��������
	m_ParentHWND = ParentHWND;

	// �������� ������ ��� ������ � ����
	m_szWriteBuffer = new BYTE[writebuffersize];

	// ��������� ����� �����
	m_nPortNr = portnr;

	// ��������� ������� ������� ��� ������
	m_nWriteBufferSize = writebuffersize;

	// ��������� ����� ������� �� ������
	m_dwCommEvents = dwCommEvents;

	// ��������� ���������� �������
	TCHAR szPort[30];
	TCHAR szBaud[50];
	// ���������� �������� ���������� ����� (������������ ��������� ������� MODE � DOS)
	// ��� �����
	_stprintf_s(szPort, sizeof(szPort)/sizeof(TCHAR), _TEXT("\\\\.\\COM%d\0"), portnr);
	// ������ ����������
	_stprintf_s(szBaud, sizeof(szBaud)/sizeof(TCHAR), _TEXT("baud=%d parity=%c data=%d stop=%d\0"), baud, parity, databits, stopbits);

	// ��������� ���� � ��������� ��� �����
	m_hComm = CreateFile(szPort,										// ������ ���������� ����� (COMX)
					     GENERIC_READ | GENERIC_WRITE,					// ������/������
					     0,												// ���������� ���������������� ���� ������ ���� ������ � ���������� 0 � ��������, ��� ������ � ������� � ��������� ��� �������� ���������� ���� ����� �� ����� ������ - �������������� ������
					     NULL,											// ��� ���������� ������������
					     OPEN_EXISTING,									// ���������� ���������������� ���� ������ ���� ������ � ������ OPEN_EXISTING (������� ������������ ����)
					     FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,	// ����������� ����/�����
					     NULL);											// template ������ ���� NULL ��� ��������� �����

	// �������� ������ ����� 
	if (m_hComm == INVALID_HANDLE_VALUE)
	{// ���� �� ������
		// ������� ������ ��������
		delete [] m_szWriteBuffer;
		m_szWriteBuffer = NULL;

		#ifdef _DEBUG
		DWORD lErrCode=GetLastError();			//��� ���������� ����� ����� ������� ����
		TCHAR lErrString[255];
		_stprintf_s(lErrString,sizeof(lErrString)/sizeof(TCHAR),_TEXT("Serial Port %s invalid while init. GetLastError()==%u\n"),szPort,lErrCode);
		TracePrint(lErrString);
		#endif
		return FALSE;
	}

	// ��������� ����-����� �����
	// ReadIntervalTimeout
	// ������������ ����� �������� ������� ���������� �����, ����
	// ������ ���������� ��� ��������� ����� �� ����� ������ ReadFile.
	// ReadFile ���������� ����� �������� ������, ���� �������� ��������� ���������
	m_CommTimeouts.ReadIntervalTimeout			= 0;
	// �� ������� ����
	m_CommTimeouts.ReadTotalTimeoutMultiplier	= 0;
	m_CommTimeouts.ReadTotalTimeoutConstant		= 0;
	m_CommTimeouts.WriteTotalTimeoutMultiplier	= 0;
	m_CommTimeouts.WriteTotalTimeoutConstant	= 0;

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
						Sleep(60); //��� ������������� � MSDN, ����� ��������� �������� � ����
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

	// ������� �����
	PurgeComm(m_hComm, PURGE_FLAGS);

	//����� ���������� ������, �������������� ����
	if(!StartMonitoring()) return FALSE;

	MyTRACE(_TEXT("Initialisation for communication port %d completed.\n"), portnr);

	return TRUE;
}

/**********************    ClosePort    ****************************************************
*		��� ������� ����������� ������ � ������: ���������� ������, ������� ��� ����-
*	������� ���������� ������ � �.�. ����� �������, ����� ������ ���� ������� � ������
*	����� ������ ������ ������ ������ ������������������ ���. �� ������� ����� �������
*	� CP2102, ���� � �� ���������� �� �����, ��� ����� �������� �������, ����� ��������
*	������ ��� ����� ���������.
*******************************************************************************************/
void CSerialPort::ClosePort()
{
	// ���� ����� ����������, �� �������
	if (m_bThreadAlive)
	{
		// �������� �� ������� ������
		if(m_bThreadSuspended)
			ContinueMonitoring();
	
		DWORD dw;
		GetExitCodeThread(m_hThread, &dw);
		if(dw == STILL_ACTIVE) 
		{//����� ��� ���
			TerminateThread(m_hThread,100);	//������������� ��� ���������
			m_bThreadAlive=false;
		}
		MyTRACE(_T("Serial Thread shutdowned.\n"));
	}
	
	// ���� ������ ��� �������, ������� ���
	if (m_szWriteBuffer != NULL)
	{
		delete [] m_szWriteBuffer;
		m_szWriteBuffer = NULL;
	}

	// ���� ���� ��� ������, �� ��������� ���
	if (m_hComm != NULL || m_hComm!=INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hComm);
		m_hComm = NULL;
		//��� ���� �������� ������������������ ������� COMx -> ���������  COMx ��
		//�������, �.�. Windows �� ����� ������� � ��� ��� ������� ��� �� ���� ����
		Sleep(100);
	}
	MyTRACE(_TEXT("Serial Communication ended.\n"));
}


/*****************************     SetParentSupervisor    ***********************************
*		������� �������� �����������
********************************************************************************************/
bool CSerialPort::SetParentSupervisor(void (*parent_supervisor)(UINT Msg, WPARAM wParam, LPARAM lParam))
{
	if(parent_supervisor==NULL) return false;
	ParentSupervisor=parent_supervisor;
	return true;
}


/*************************    StartMonitoring    *******************************
*		������ ������ ����������� �� ���������������� ������
*******************************************************************************/
BOOL CSerialPort::StartMonitoring()
{
	// ������� �� ������������� ������
	if(m_bThreadAlive == TRUE)
		return TRUE;

	// ������� ����� �����
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
*		����� ������ �� ��������� �����������
********************************************************************************/
BOOL CSerialPort::ContinueMonitoring()
{
	// ������� �� ������������� ������
	if(m_bThreadAlive == FALSE)
		return FALSE;

	// �������������� ������
	if(ResumeThread(m_hThread)==-1)
	{//�� ������ ������������ ���������� ������
		ProcessErrorMessage(_T("ContinueMonitoring()"));
		return FALSE;
	}
	m_bThreadSuspended = FALSE;
	//++++ TRACE("Serial Thread resumed\n");
	return TRUE;	
}

/********************    StopMonitoring    *****************************
*		���� ������ � ��������� �����������.
***********************************************************************/
BOOL CSerialPort::StopMonitoring()
{
	// ������� �� ������������� ������
	if(m_bThreadAlive == FALSE)
		return FALSE;

	// ������������ ������
	if(SuspendThread(m_hThread)==-1)
	{//�� ������ ������������� �����
		ProcessErrorMessage(_T("StopMonitoring()"));
		return FALSE;
	}
	m_bThreadSuspended = TRUE;
	MyTRACE(_TEXT("Serial Thread suspended\n"));
	return TRUE;	
}

/***********************    ProcessErrorMessage    ****************************
*		����������� ��������� ������� ������
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
*		�������������� ������ ��������� � ����. ������������ ��� � ������ �����.
*	������ ������������ �� ������ ��������.
*********************************************************************************/
bool CSerialPort::WritePortLoLevel(CSerialPort* port)
{
	assert(port);						//���� ��� ����
	assert(port->m_szWriteBuffer);		//� ����� ������ ����������

	BOOL bResult = TRUE;
	DWORD BytesSent = 0;

	// ������������� ����������
	port->m_ov.Offset = 0;				// ������ 0, ���� ���������������� ����
	port->m_ov.OffsetHigh = 0;			// ������ 0, ���� ���������������� ����

	// ������� �������� ������, �������� � ������� ������, ��������
	//++++ PurgeComm(port->m_hComm, PURGE_FLAGS);

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
				{//��� ���������� ��������, ����� ��� �� ������ �����������
					// ���������� ����� ������������ ������
					BytesSent = 0;
					bResult = GetOverlappedResult(port->m_hComm,	// ����� ����������������� ����� 
								  &port->m_ov,		// ����������� ���������
								  &BytesSent,		// ����� �������� ����� ������������ ����
								  TRUE); 			// ���� �������� (TRUE - �������� ��������� ��������)
					if (!bResult)  
					{//������
						port->ProcessErrorMessage(_TEXT("GetOverlappedResults() in WriteFile()"));
						return false;
					}
					break;
				}
			default:
				{// ��� ��������� ������
					port->ProcessErrorMessage(_TEXT("WriteFile()"));
					return false;
				}
		}
	}


	// �������� �� ��������� ����� ������������ � ������������ ����
	if (BytesSent != (port->dwBytesToWrite))
	{
		MyTRACE(_TEXT("WARNING: WriteFile() error.. Bytes Sent: %d; Message Length: %d\n"), BytesSent, port->dwBytesToWrite);
	}

	/*DWORD CommEvent;
	GetCommMask(port->m_hComm, &CommEvent);
	if (CommEvent & EV_TXEMPTY) 
	{//��� ����� ��������
		port->NotifyParent(WM_COMM_TXEMPTY_DETECTED, (WPARAM) 0, (LPARAM) port->m_nPortNr);	
	}*/
//++++	port->NotifyParent(WM_COMM_TXEMPTY_DETECTED, (WPARAM) 0, (LPARAM) port->m_nPortNr);	

	return true;
}

/***************************    ReceiveChar    ***************************************
*		����� ������ � ���������� �������� ��� �� ������� ����. � ��������� �����������
*	�������, ����������� �� ������ �����, �.�. ��� ������ ��������� CP2102.
**************************************************************************************/
bool CSerialPort::ReceivePortLoLevel(CSerialPort* port)
{
	BOOL  bResult = TRUE;
	DWORD BytesRead = 0;
	unsigned char RXBuff [100];		//����� ��� �������� �������� ������

	for (;;) 
	{//��������� ���� � ����������� �����, �.�. ���� � �� ��� �� ����� �������
	//����� ���� �����, �� ����� ������, ������ � �.�. �������
		
		COMSTAT comstat;	//���� ����� �������� ���������� � �����
		DWORD dwError=0;	//� ���� ���������� �� �������
		ClearCommError(port->m_hComm, &dwError, &comstat);
		//� dwError �������� ������ �� ������, �� � �� ������������ �� ����

		if (comstat.cbInQue == 0)
		{//������� ������ �����
			// break out when all bytes have been read
//				::SendMessage((port->m_pOwner)->m_hWnd, WM_COMM_CLEAR, (WPARAM) 0, (LPARAM) port->m_nPortNr);
			break;
		}
						
		//�����, ������� ���� ��� ����� �������
		unsigned int lBytesToRead=comstat.cbInQue;
		//�������� �� ������������
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
					{//��� ���������� ��������
						bResult = GetOverlappedResult(port->m_hComm,	// Handle to COMM port 
									  &port->m_ov,		// Overlapped structure
									  &BytesRead,		// Stores number of bytes read
									  TRUE); 			// Wait flag

						if (!bResult)  
						{//������
							port->ProcessErrorMessage(_T("GetOverlappedResults() in ReadFile()"));
							return false;
						}
						break;
					}
				case ERROR_BAD_COMMAND:	//22
					{//The device does not recognize the command
						//����� ���������, ����� � ���������� CP2102
						//�� ����� ��� �� ����������, ��� ����� ������
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

		//� ������ �������� �������� �� ������� ����
		unsigned char * ptr=&RXBuff[0];
		for(unsigned int i=0; i<BytesRead; i++)
		{
			//������� ����, ���� ��� ����
			port->NotifyParent(WM_COMM_RXCHAR, (WPARAM) *ptr, (LPARAM) port->m_nPortNr);
			ptr++;
		}
	}//����� ������������ �����
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


		//������� ����, ���� ��� ����
		port->NotifyParent(WM_COMM_RXCHAR, (WPARAM) RXBuff, (LPARAM) port->m_nPortNr);
		

	} // end forever loop
	
//	BOOL  bRead = TRUE; 
//	BOOL  bResult = TRUE;
//	DWORD dwError = 0;
//	DWORD BytesRead = 0;
//	unsigned char RXBuff = 0;
//
//	//� CP2102 ClearCommError ����������� ����� �����, ������� 
//	//������ �� �� ������� ������� �����, ����� ��� ����������� ������
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
//					//����� ����� ����� ��������� � CP2102
//		//			bRead = TRUE;
//					//������� ����, ���� ��� ����
//		//			port->NotifyParent(WM_COMM_RXCHAR, (WPARAM) RXBuff, (LPARAM) port->m_nPortNr);
//				}
//				else if(dwError==ERROR_BAD_COMMAND)	//22
//				{//The device does not recognize the command
//					//����� ���������, ����� � ���������� CP2102
//					//�� ����� ��� �� ����������, ��� ����� ������
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
//		//������� ����, ���� ��� ����
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
	{//���� ������������������ ����������� � �� ���� �������� ����� ��� �����
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
	{//��� ����� ��������
		//��� ������������� ��������, �.�. TX_EMPTY ������-�� �� ��������
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

/*********************   NotifyParent    *******************************************
*		������ ������� ���, ��� �� ����� �������� � ��������� ��� ����������� ���������,
*	��� � ������� ��� ������ ������� �����-���� ������� �������. ��� ������� ���������� 
*	���������� ����� �����������.
***********************************************************************************/
void CSerialPort::NotifyParent(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if(ParentSupervisor!=NULL)
	{//����� ������� �������� �������
		ParentSupervisor(Msg, wParam, lParam);
	}
	else if(m_ParentHWND!=NULL)
	{//����� ��������� ��������� ����-��������
		PostMessage(m_ParentHWND,Msg,wParam,lParam);
	}
	else
	{
		MessageBox(NULL,_TEXT("������� Serial Port ������ ����������."), _TEXT("Serial Port"),MB_ICONSTOP);
	}
}

//
//  ������� ������� ������ CommThread.
//  pParam - ��������� �� CSerialPort
DWORD WINAPI CommThread(PVOID pParam)
{
	assert(pParam);

	// ���������� ��������� �� ��� void ���������� � ����� (AfxBeginThread(..., !this!))
	// ����� � ��������� �� ����� CSerialPort
	CSerialPort *lpPort = (CSerialPort*)pParam;
	
	// ��������� ����� ��������� ������ (���������� ��� ��������)  
	// TRUE - ��� ���������, ��� ����� �������.
	lpPort->m_bThreadAlive = TRUE;	
		
	// ������ ����������
	// ����� �������
	DWORD Event		= 0;
	DWORD CommEvent = 0;
	// ���� ����������, ������������ ��� ��� ����� ��!
	BOOL  bResult	= TRUE;
			
	// ������� ������ ����� ��������
	if (lpPort->m_hComm)		// ��������� ������ �� ����
		PurgeComm(lpPort->m_hComm, PURGE_FLAGS);

	// �������� ������ ����.  ���� ����� ������������ ���� �������� �����.
	for (;;) 
	{ 	// WaitCommEvent ������� ��������� ������� �� ������������� ��� ������� �����.
		// �������� WaitCommEvent(). ���� ����� ����������� ����������,
		// ������ ��� ��� ���� ������ ��� ����������� ���� (FILE_FLAG_OVERLAPPED
		// � ������������ ����������� ��������� m_ov). ������� ��������� ������� m_ov.hEvent,
		// ������� �������� ������ ������� ������� m_hEventArray, � ������������ ���������,  
		// ���� ��� �� ������ ����� ��� ������ ��� � ���������� ���������,  
		// ���� ���� ��� ��������� �� �����. ���� ���� ����� ������� ���������� � ������������ ���������,  
		// �� �� ������� ������� (�����) � ���� �� ����� ���������� � ���������� ���������.
		Event=0;	//�� ������ ������ ������
		lpPort->m_ov.Internal = 0;
		lpPort->m_ov.InternalHigh = 0;
		lpPort->m_ov.Offset = 0;
		lpPort->m_ov.OffsetHigh = 0;

		bResult = WaitCommEvent(lpPort->m_hComm, &Event, &lpPort->m_ov);

		// ���� ����������� �������� �� ����� ���� ��������� ����������, �� �������� FALSE
		if (!bResult)  
		{ 
			// ���� WaitCommEvent() ��������� FALSE, �� ���������� ��������� ������
			// ��� ����������� �������..
			DWORD dwError;
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
			case ERROR_INVALID_PARAMETER:	//����� 87
				{
					// � Windows NT, ���� ��� ������������ �� ���������� ��������.
					// � ��� ���� ���������� �����
					// ����� ������ �� ������ � ���������� �������� ������� � GetOverlappedResult!
					break;
				}
			case ERROR_ACCESS_DENIED:		//����� 5
			case ERROR_OPERATION_ABORTED:	//����� 995
			case ERROR_BAD_COMMAND:			//����� 22
				{
					//������ ��c��� � �����. ��� ���������, ���� �� ���, � ������ ����, 
					//��������, ��� ����� ��������� ��������� CP2102
					lpPort->NotifyParent(WM_COMM_UNCOWN_ERROR, (WPARAM) 0, (LPARAM) lpPort->m_nPortNr);
					//������������� CP210x ����������� ��� ���� ������� �����, � ���� ����� ������ �� ����
					break;
				}
			default:
				{
					// ��� ������ ���� ������ ���������� ��������� ��������� ������.
					// ���������� ��� ������.
					lpPort->NotifyParent(WM_COMM_ACCESS_DENIED, (WPARAM) 0, (LPARAM) lpPort->m_nPortNr);
					lpPort->ProcessErrorMessage(_TEXT("WaitCommEvent()"));
					//������� �����
					goto EndOfCommThread;
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
	
			//bResult = ClearCommError(lpPort->m_hComm, &dwError, &comstat);

			// �������� ���������� ���� � �������
			//if (comstat.cbInQue == 0)
			//	continue;
		}	// ����� ���� bResult

		// ������� ������� ��������.  ��� ������� ��������� ����� ���� �� ���������
		// ���� �� ������ �������.

		DWORD lObjectNum = WaitForMultipleObjects(3, lpPort->m_hEventArray, FALSE, INFINITE);

		switch (lObjectNum)
		{
		case WAIT_OBJECT_0:
			{//����������� ������
			 	lpPort->m_bThreadAlive = FALSE;
				// ������� ���� �����.
				return 100;
			}
		case WAIT_OBJECT_0+1:	// ������� �� ������� ����� � ����
			{//������ �����
				//������ ������� �� �����
				if(!ResetEvent(lpPort->m_hEventArray[1]))
				{//������
					lpPort->NotifyParent(WM_COMM_UNCOWN_ERROR, (WPARAM) 0, (LPARAM) lpPort->m_nPortNr);
					lpPort->ClosePort();
					return false;
				}

				/*CommEvent=0;		//�� ������ ������ ������
				if(!GetCommMask(lpPort->m_hComm, &CommEvent))
				{
					MyTRACE(_TEXT("������ � ������� GetCommMask()"));
				}

				DWORD dwError;
				COMSTAT comstat;
				if(!ClearCommError(lpPort->m_hComm, &dwError, &comstat))
				{//������ ��
					lpPort->ProcessErrorMessage(_T("ClearCommError() in ReceiveChar()"));
				}*/

				// ������� �� ���������� ����� �� �����
				if ((Event & EV_RXCHAR) || (CommEvent & EV_RXCHAR))
				{
					if(!lpPort->ReceivePortLoLevel(lpPort))
					{//�����-�� ���������� ������ ��� ������
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
				{//� ����� ��������� ������
					/*if(!ClearCommError(lpPort->m_hComm, &dwError, &comstat))
					{//������ ��
						lpPort->ProcessErrorMessage(_T("ClearCommError() in ReceiveChar()"));
					}*/
					//�������� ��������
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
				{//���������� �� ���������
					char t=5;
					//�� �������� �� CP2102 lpPort->NotifyParent(WM_COMM_TXEMPTY_DETECTED, (WPARAM) 0, (LPARAM) lpPort->m_nPortNr);	
				}

				break;
			}  
		case WAIT_OBJECT_0+2: // ������� ������
			{//������ � ����
				//������ ������� �� ������ � ����
				if(!ResetEvent(lpPort->m_hWriteEvent))
				{//������
					lpPort->NotifyParent(WM_COMM_UNCOWN_ERROR, (WPARAM) 0, (LPARAM) lpPort->m_nPortNr);
					lpPort->ClosePort();
					return false;
				}
				//������ ������ � ����
				if(!lpPort->WritePortLoLevel(lpPort))
				{//������
					MyTRACE(_TEXT("������ ��� �������������� ������ � ����"));
					lpPort->NotifyParent(WM_COMM_UNCOWN_ERROR, (WPARAM) 0, (LPARAM) lpPort->m_nPortNr);
					lpPort->ClosePort();
					return false;
				}

				//��������, �.�. �� CP2102 �� �������� EV_TXEMPTY
				lpPort->NotifyParent(WM_COMM_TXEMPTY_DETECTED, (WPARAM) 0, (LPARAM) lpPort->m_nPortNr);	


				//��������� ��������� �������, �� �������� ��������� ������� �������������
				//������ ����� ��������� ������ ������ ��������� �������, ��������� �������-
				//�� ����� ����� ����, � ���� �������� ������������� ����� ����� ������,
				//�� �� ��� ��� �� �������, ��������� ��� ��� ���!
				/*for(;;)
				{
					GetCommMask(lpPort->m_hComm, &CommEvent);
					if (CommEvent & EV_TXEMPTY) 
					{//��� ����� ��������
						lpPort->NotifyParent(WM_COMM_TXEMPTY_DETECTED, (WPARAM) 0, (LPARAM) lpPort->m_nPortNr);	
						break;
					}
				}*/
				/*if (Event & EV_TXEMPTY)
					char t=5;*/
				break;
			}

		} // ����� switch
	} // ��������� ������ ����
EndOfCommThread:
	TracePrint("Serial Thread ended");
	return 0;
}
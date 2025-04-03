#include "stdafx.h"
#include "LogFile.h"
#include <AtlConv.h>		//для перобразований unicode<->ANSI



LOG_FILE_Class::LOG_FILE_Class()
{
	this->m_sFileName=_T("LogFile");
	m_pError=NULL;
}

/**************************************************************
*		ИНИЦИАЛИЗАЦИЯ
***************************************************************/
bool LOG_FILE_Class::InitLogFile(const TCHAR* file_name, ERROR_Class *pError, CString name_of_program, bool UseUnicode)
{
	// file_name - имя файла журнала сообщений
	// pError - указатель на структуру, обслуживающий ошибки
	// name_of_program - необязательное имя программы, нужно для заголовка

	m_sFileName=file_name;
	m_pError=pError;
	m_UseUnicodeFormat=UseUnicode;
	//откроем файл
	if(!Open(name_of_program))
	{
		return false;
	}
	return true;
}


/***********************	Show(void)	*****************************************
*	Просто открывает этот файл в блокнотике
*********************************************************************************/
bool LOG_FILE_Class::Show(void)
{
	CString str;
	str.Format(_TEXT("Notepad.exe %s"),m_sFileName);
	
	STARTUPINFO si={sizeof(si)};
	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );
	if( !CreateProcess( NULL, (LPTSTR)str.GetBuffer(), NULL, NULL, FALSE,
		0, NULL, NULL, &si, &pi))
	{
		m_pError->SetERROR(_TEXT("Невозможно запустить блокнотик для показа журнала сообщений."), ERP_ERROR | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
		return false;
	}
	//Мне не нужны описатели дочернего процесса и потока
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );

	return true;
}

bool LOG_FILE_Class::Open(CString name_of_program)
{
	/************************************************************
	*	Если пользователь поменяет текущую директорию программы, то 
	*	мы уже потом не сможем открыть файл блокнотиком только по
	*	имени файла, поэтому сохранять будем полный путь. Если работаем
	*	в ANSI, а на пути к файлу стоят русские буквы, то файл
	*	НЕ ОТКРОЕТСЯ, пока не будет сменена кодировка!
	*************************************************************/
	if(m_sFileName[2]!=':')
	{//программе по аргументу было передано только имя
		//запишем полный путь
		TCHAR dir[255];
		GetCurrentDirectory(244,dir);
		m_sFileName=dir+('\\'+m_sFileName);
	}
	if(LogFile.is_open()) 
		LogFile.close();//на всякий случай
	
	LogFile.open(m_sFileName,ios::trunc | ios::out | ios::binary);
	if(!LogFile)
	{
		DWORD lErr=GetLastError();
		CString str; str.Format(_TEXT("Не создался файл \"%s\", возможно диск защищен от записи или переполнен. GetLastError()=%u"),m_sFileName,lErr);
		if(m_pError!=NULL)
		{
			m_pError->SetERROR(str,ERP_ERROR | ER_SHOW_MESSAGE);		
		}
		else
		{//если нет класса ошибок, то хоть покажем сообщение
			AfxMessageBox(str);
		}
		return false;
	}
	else
	{
		CString str=_TEXT("Журнал регистрации программы...");
		if (name_of_program!=_TEXT(""))
		{//попробуем для большей информативность проставить имя файла
			str.TrimRight('.');
			str.Append(_TEXT("  \""));
			str.Append(name_of_program);
			str.Append(_TEXT("\"."));
		}
		if(m_UseUnicodeFormat)
		{//нужно писать в unicode
			str.Append(_T("\r\n\r\n"));	//|
			unsigned short t=0xFEFF;	//|
			LogFile.write((char*)&t,2);	//| примочка, говорящая, что файл unicode
			LogFile.write((char*)str.GetBuffer(),str.GetLength()*sizeof(TCHAR));
		}
		else
		{//нужно писать в ansi
			USES_CONVERSION;
			LogFile.write(T2A(str.GetBuffer()),str.GetLength());
		}
	}
	return true;
}

/*************************	Write(*)	***********************************
*		Эти функции делают очередную запись в файл
***************************************************************************/
bool LOG_FILE_Class::Write(ERROR_Class* perror)
{
	if(!LogFile.is_open()) return false;		//файл вообще не открыт
	CString str;
		
	//Сначала запишем время
	SYSTEMTIME time;
	GetLocalTime(&time);
	str.Format(_TEXT("\r\n%ih:%imin : %is : %ims      %s"),time.wHour,time.wMinute,time.wSecond,time.wMilliseconds,perror->GetLastError());

	if(m_UseUnicodeFormat)
	{//нужно писать в unicode
		LogFile.write((char*)str.GetBuffer(),str.GetLength()*sizeof(TCHAR)); //запишем ошибку
	}
	else
	{//в формате ANSI
		USES_CONVERSION;
		LogFile.write(T2A(str.GetBuffer()),str.GetLength()); //запишем ошибку
	}
/*	if(perror->m_iPriority & ~ER_WRITE_TO_LOG)
	{//ненулевой приоритет
		str.Format("    Приоритет:   %u ",perror->m_iPriority & ~ER_WRITE_TO_LOG);
		LogFile.write(str,str.GetLength());  //и ее важность
	}*/
	LogFile.flush();		//скину из буфера Windows в физический файл на диске

	return true;
}

/*************************	Write	***************************************
*		Эти функции делают очередную запись в файл
***************************************************************************/
bool LOG_FILE_Class::Write(CString error, unsigned char priority)
{
	if(!LogFile.is_open()) return false;

/*	CString str=error;
	LogFile.write("     ",5);
	USES_CONVERSION;
	LogFile.write(T2A(str),str.GetLength()); //запишем ошибку
	if(priority & ~ER_WRITE_TO_LOG)
	{//ненулевой приоритет
		str.Format(_TEXT("    Приоритет:   %u "),priority & ~ER_WRITE_TO_LOG);
		LogFile.write(str,str.GetLength());  //и ее важность
	}
	LogFile<<endl;*/
	return true;
}
#include "stdafx.h"
#include "ERRORS.h"

/*********************  КОНСТРУКТОР  ****************************************************
***************************************************************************************/
ERROR_Class::ERROR_Class()
{
	ErrorServ=NULL;			//сбросим указатель на внешнюю вызываемую функцию
	m_bNew=false;
	m_iPriority=0;
	m_sLastError="No error";
	m_pParent=NULL;
}

bool ERROR_Class::InitERROR(void (*error_serv)(ERROR_Class *error),CWnd* pParent)
{
	this->ErrorServ=error_serv;		//запомним указатель на обрабатывающую функцию
	m_pParent=pParent;				//запомним указатель на родителя, чтобы в случае чего
									//отправлять ему сообщения
	return true;
}

/****************** SetERROR(CString error, unsigned char priority) ************************
*		Добавление записи в журнал сообщений.
*	error - текст сообщения
*	priority - приоритет сообщения (влияет на иконку и цвет)
*	Title - указатель на строку заголовка сообщения. Заголовок в LogFile не пишется
********************************************************************************************/
void ERROR_Class::SetERROR(CString error, unsigned char priority, TCHAR* Title)	
{
	m_iPriority=priority;	//приоритет ошибки
	m_sLastError=error;		//текст ошибки
	if(Title!=NULL)
		m_sLastTitle=Title;	//заголовок сообщения
	else 
		m_sLastTitle=_T("");//нет заголовка

	m_bNew=true;			//флаг, указывает, что это новая ошибка
	if(ErrorServ!=NULL)
	{//еще нужно вызвать внешнюю фукцию
		ErrorServ(this);		//вызовем внешнюю фукцию, которая и покажет пользователю ошибку
	}

	if(m_iPriority & ER_SHOW_MESSAGE)
	{//Стоит флаг отображения сообщения
		UINT lFlags=0;
		priority&=0x0F;				//обрежем область флагов	
		TCHAR lTitle[100];			//заголовок окна сообщения

		if(Title!=NULL)
			_tcscpy_s(lTitle,_countof(lTitle),Title);	//заголовок уже указан

		if(priority>=ERP_FATALERROR)
		{
			lFlags|=MB_TOPMOST | MB_ICONERROR;
			if(Title==NULL)
				_tcscpy_s(lTitle,_T("Фатальная ошибка\0"));
		}
		if(priority>=ERP_ERROR)
		{//флаг отображения всегда наверху
			lFlags|=MB_ICONERROR;
			if(Title==NULL)
				_tcscpy_s(lTitle,_T("Ошибка\0"));
		}
		else if(priority>=ERP_WARNING)
		{
			lFlags|=MB_ICONWARNING;
			if(Title==NULL)
				_tcscpy_s(lTitle,_T("Предупреждение\0"));
		}
		else if(priority>=ERP_WATCHFUL)
		{
			lFlags|=MB_ICONWARNING;
			if(Title==NULL)
				_tcscpy_s(lTitle,_T("Обратите внимание\0"));
		}
		else if(priority>=ERP_NORMAL)
		{
			lFlags|=MB_ICONINFORMATION;
			if(Title==NULL)
				_tcscpy_s(lTitle,_T("Сообщение\0"));
		}
		else if(priority>=ERP_GOOD)
		{
			lFlags|=MB_ICONINFORMATION;
			if(Title==NULL)
				_tcscpy_s(lTitle,_T("Операция выполнена\0"));
		}
		else if(priority>=ERP_VERYGOOD)
		{
			lFlags|=MB_ICONINFORMATION;
			if(Title==NULL)
				_tcscpy_s(lTitle,_T("Примите поздравления\0"));
		}
		//Покажу сообщение
		if(m_pParent!=NULL)
			MessageBox(m_pParent->m_hWnd,error,lTitle,lFlags);
		else
			MessageBox(NULL,error,lTitle,lFlags);
	}

	if(m_pParent!=NULL && IsWindow(m_pParent->m_hWnd))
	{//нужно отправить сообщение обрабатывающему окну
		m_pParent->SendMessage(MSG_ER_ERROR,0, reinterpret_cast<LPARAM>(this));
	}
}

/****************** GetLastError() **********************************************************
*		выдает текст последней происшедшей ошибки, при этом сбрасывая флаг
*		необработанности этой ошибки
********************************************************************************************/
CString ERROR_Class::GetLastError()			
{
	m_bNew=false;
	return m_sLastError;
}
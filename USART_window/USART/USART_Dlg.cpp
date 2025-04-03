// USART_Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "USART_Dlg.h"
#include "Resource.h"					//без этого мы не понимаем ID-шники
#include "FlagsDlg.h"					//окошко с настройками
#include "PROTOCOL_Struct.h"			//файл с протоколом

/*****************************************************************************
*		Здесь объявим импортируемые переменные
*********************************************************************************/
//extern CUSART_Dlg USART_Dlg;

IMPLEMENT_DYNAMIC(CUSART_Dlg, CDialog)

CUSART_Dlg::CUSART_Dlg(CWnd* pParent, UINT ID)
	: CDialog(ID, pParent)
{

}

CUSART_Dlg::~CUSART_Dlg()
{
}

void CUSART_Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OUTMESSAGE, m_OutMessage);
	DDX_Control(pDX, IDC_RECIEVE, m_Recieve);
	DDX_Control(pDX, IDC_INMESSAGE, m_ReceiveMessage);
	DDX_Control(pDX, IDC_ERROR_INMESSAGE, m_InMessageError); 
	DDX_Control(pDX, IDC_ERROR_OUTMESSAGE,m_OutMessageError);
	DDX_Control(pDX, IDC_SEND, m_sended);
	DDX_Control(pDX, IDC_COMBO1, m_ListPacket);
}


BEGIN_MESSAGE_MAP(CUSART_Dlg, CDialog)
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_BUTTON5, OnFile)
	ON_BN_CLICKED(IDC_VERIFY, OnVerify)
	ON_CBN_SELENDOK(IDC_COMBO1, OnVibor)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_OUTMESSAGE, OnEnChangeOutMessage)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_OPTIONS, OnBnClickedOptions)
	ON_MESSAGE(WM_COMM_RXCHAR, OnCommunication)
	ON_MESSAGE(WM_COMM_TXEMPTY_DETECTED, OnTXEmpty)
	ON_MESSAGE(WM_COMM_ACCESS_DENIED, PortFailure)
END_MESSAGE_MAP()


/***********************   TrancieverTimer   ******************************************
*		Если стоит опция непрерыной передачи (в процессе отладки, или в элементарных 
*	приложениях, то при срабатывании таймера мы переносимся сюда, а отсюда нам нужно просто
*	отправить следующее сообщение
***************************************************************************************/
void CALLBACK TrancieverTimer(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2) 
{ 
	CUSART_Dlg* dlg=reinterpret_cast<CUSART_Dlg*>(dwUser);
	CString str;
	#ifndef DELAY_COMPILE_ALL
	//если нет режима с предварительной компоновкой, то построим посылку сейчас
	if(!dlg->pPROTOCOL->CompileALL())
	{
		dlg->ShowERROR_in_out_message(_T("Функция CompileALL отработала с ошибкой"),9);
		return;
	}
	#endif
	
	if(!dlg->TranseiveMessage(&dlg->pPROTOCOL->NextMessage))
	{//физическая отправка сообщения не удалась
		if(dlg->pPROTOCOL->pProtocolOptions->NotifyProgram!=NULL)
		{//есть указатель на функцию уведомлений
			dlg->pPROTOCOL->pProtocolOptions->NotifyProgram(NP_TRANSFER_ERROR);
		}
		return;	
	}
	return;
}

BOOL CUSART_Dlg::Create(UINT ID, CWnd* pParentWnd)
{
	BOOL result=CDialog::Create(ID, pParentWnd);
	this->IsStarted=false;						//передача пока не начиналась
	this->LastGoodReceived=0;					//пакеты мы пока еще не отпарвляли и не принимали
	pTranceivingMessage=NULL;					//никакое сообщение не отправляется
	
	//организуем оповещение о глобальном разрешении передачи (по умолчанию занятое)
	HGlobalStart=CreateEvent(NULL,FALSE,FALSE,NULL);
	ASSERT(HGlobalStart);	//на всякий случай
	
	//организуем глобальное оповещение о временной занятости передатчика (на время отправки сообщения)
	HTranceiverBusi=CreateEvent(NULL,FALSE,TRUE,NULL);
	ASSERT(HTranceiverBusi);

	return result;
}

/*****************************	OnInitDialog()	*******************************************
*		Инициализация диалога
******************************************************************************************/
BOOL CUSART_Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();



	return TRUE;  // return TRUE unless you set the focus to a control
}

bool CUSART_Dlg::MyInit(ERROR_Class* m_ERROR, POTOCOL_OPTIONS_Struct* pProtocolOptions, PROTOCOL_Struct *protocol)
{
	//проверка аргументов
	if(m_ERROR==NULL || pProtocolOptions==NULL || protocol==NULL)	return false;

	this->pPROTOCOL=protocol;							//указатель на протокол
	this->pPROTOCOL->pProtocolOptions=pProtocolOptions;
	this->m_pError=m_ERROR;
	
	//ИНИЦИАЛИЗАЦИЯ ПОСЛЕДОВАТЕЛЬНОГО ПОРТА
	if(pProtocolOptions->var_PORT_NUMBER!=PN_NOTINITIALIZED)
	{//пользователь хочет выставить последовательный порт
		ReSetCOMport(pProtocolOptions->var_PORT_NUMBER);
	}
	
	//ПРОИНИЦИАЛИЗИРУЕМ КЛАСС ПРОТОКОЛА
	if(!pPROTOCOL->InitPROTOCOL(this, m_pError))
	{//были ошибки, дальнейшее продолжение невозможно
		return false;
	}

	//Зададимся первоначальным типом отправляемого сообщения
	//++++ m_ListPacket.SetCurSel(0);
	//++++ this->OnVibor();

	#ifndef	DELAY_COMPILE_ALL
	//поменяем надпись над отправляемым сообщением
	GetDlgItem(IDC_OUT_MESSAGE_CAPTION)->SetWindowText(_T("Последнее отправленное сообщение:"));
	#endif

	//Затемним эдитик отпраляемого сообщения, чтобы вручную его можно было
	//менять только после выбора режима ручного смена посылки
	m_OutMessage.SetReadOnly(true);

	return true;	//вроде все нормально
}

/*********************   OnStart()   ************************************************
*		Нажали на кнопку СТАРТ/СТОП, просто нужно проверить, начать или закончить
*	приемопередачу
*************************************************************************************/
void CUSART_Dlg::OnStart() 
{
	CString str;
	((CButton*)GetDlgItem(IDC_START))->GetWindowText(str);
	if(str==_T("СТАРТ!"))
	{
		StartTransfer();
	}
	else if(str==_T("СТОП!"))
	{
		StopTransfer();
	}
}

/*******************************   StartTransfer(void)       *****************************
*		Эта функция вызывается, когда после нажатия на кнопку старт оказалось, что
*	нужно начать передачу.
******************************************************************************************/
bool CUSART_Dlg::StartTransfer(void)
{
	#ifndef DELAY_COMPILE_ALL
	//если нет режима с предварительной компоновкой, то построим посылку сейчас
	if(!pPROTOCOL->CompileALL())
	{
		ShowERROR_in_out_message(_T("Функция CompileALL отработала с ошибкой"),9);
		return false;
	}
	#endif

	if(pPROTOCOL->pProtocolOptions->o_emulation) 
	{//эмуляция приемопередачи, пакеты гоняются вне порта по таймеру
		if((char*)pPROTOCOL->NextMessage.text=="")//если нечего отправлять
		{
			m_pError->SetERROR(_T("Буфер пуст, эмуляция отменена"), ERP_WARNING | ER_WRITE_TO_LOG);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_START))->SetWindowText(_T("СТОП!"));
			m_pError->SetERROR(_T("Пошла эмуляция"),ERP_NORMAL);	//?????
			my_timer=SetTimer(0x02,pPROTOCOL->pProtocolOptions->o_EmulSpeed,NULL);
		}
	}
	else
	{//это передача через физический порт RS232

		m_Ports.ClearTXBuff();	//на всякий случай очистим передатчик

		//отправим по крайней мере первое сообщение
		if(!TranseiveMessage(&pPROTOCOL->NextMessage))
		{
			return false;
		}
		if(pPROTOCOL->pProtocolOptions->o_nepr)
		{//это не последнее сообщение, которое от нас требуют отправить
		 //нужно вести передачу непрерывно
			RealTimer.SetTimerCallback(pPROTOCOL->pProtocolOptions->o_Period,5,pPROTOCOL->pProtocolOptions->o_nepr,TrancieverTimer,reinterpret_cast<DWORD>(this));	//Таймер до следующего пакета
		}
	}

	//не забыть поменять надпись на кнопке
	((CButton*)GetDlgItem(IDC_START))->SetWindowText(_T("СТОП!"));
	return true;
}

/*******************************    StopTransfer(void)    *******************************
*		Эта функция вызывается, когда нужно прекратить передачу, или ничего не делать, если
*	она не идет
*****************************************************************************************/
bool CUSART_Dlg::StopTransfer(void)
{
	if(pPROTOCOL->pProtocolOptions->o_emulation)
	{//эмуляция приемопередачи, пакеты гоняются вне порта по таймеру
		KillTimer(0x02);	//остановим таймер отправки символов
	}
	else
	{//это передача через физический порт RS232
		m_Ports.SetRTS(false);			//Уставливаем RTS в 0, для приема
		RealTimer.CancelTimer();		//остановим таймер передачи
	}
	//не забыть поменять надпись на кнопке
	((CButton*)GetDlgItem(IDC_START))->SetWindowText(_T("СТАРТ!"));
	return true;
}

/************************    OnVibor()   ********************************
*		Эта функция вызывается когда пользователь вручную выбирает следующее 
*	сообщение, которое нужно отправить
**************************************************************************/
void CUSART_Dlg::OnVibor()
{
	int i=m_ListPacket.GetCurSel();
	
	pPROTOCOL->Mode=i+1;//чтобы не получился нулевой режим

	//проверим не является ли выбранный режим ручной сменой посылки
	if(pPROTOCOL->Mode==pPROTOCOL->pProtocolOptions->CompileMessage_x.size()+1)
	{//этот режим находится в строке после всех стандартных сообщений
		//сделаем эдитик отправляемого сообщения доступным
		m_OutMessage.SetReadOnly(false);
	}
	else
	{//это обычное сообщение
		//сделаем эдитик отправляемого сообщения недоступным
		m_OutMessage.SetReadOnly(true);
	}


	/*Расшифровка номеров дана в классе протокола*/
	#ifdef DELAY_COMPILE_ALL
	//сразу же скомпонуем новое сообщение
	if(!pPROTOCOL->CompileALL())
	{//оповестим об ошибке
		pUSART_Dlg->ShowERROR(pPROTOCOL->m_pError.GetLastError(),pPROTOCOL->m_pError.m_iPriority,true);
	}
	#endif
}

/*****************	OnVerify() *****************************************
*	Теперь по нажатию на эту кнопку можно распознать сообщение, которое
*	находится в эдитике принимаемого сообщения. Эта опция полезна для 
*	отладки, когда нужно повторно обработать файл принятых сообщений, то можно
*	выборочно проверить пакеты в нем
**********************************************************************/
void CUSART_Dlg::OnVerify() 
{//нажали на кнопку идентификации

	CString str,error;
	unsigned char *temp_buf;
	temp_buf=new unsigned char [pPROTOCOL->pProtocolOptions->var_MAX_LEN];
	
	m_ReceiveMessage.GetWindowText(str);
	int len=GiveHexFromStr(str,temp_buf,&error);
	if(len==0)
	{//что-то в окне либо плохая информация, либо ничего нету
		m_pError->SetERROR(error, 3);
		delete []temp_buf;	//подчистим созданный массив
		return;
	}
	
	//проверим корректность контрольной суммы, правда суммой будем считать только последний байт
	unsigned char sum=pPROTOCOL->pProtocolOptions->CURRENT_SUMM(temp_buf,1,len-1);
	CString sum_right;
	if(sum==temp_buf[len-1])
	{
		sum_right=_T("Контрольная сумма корректна");
	}
	else
	{
		sum_right=_T("Контрольная сумма неверна");
	}
	//даже если контрольная сумма неверна, то все равно распознаем сообщение, в этом и преимущество этого алгоритма

	if(!pPROTOCOL->pProtocolOptions->OnVerify(&error,temp_buf))
	{//были ошибки при выполнении
		//m_pError->SetERROR(pPROTOCOL->m_pError.GetLastError(),pPROTOCOL->m_pError.m_iPriority);
	}
	else
	{//все норамально, покажем информацию о распознанном сообщении
		ShowERROR_in_input_message(error+_T("\n")+sum_right,0);
	}
	delete []temp_buf;	//подчистим созданный массив

}

/************************	ShowERROR_in_out_message	**********************
*	 показывает ошибку в отправляемом сообщении
******************************************************************************/
void CUSART_Dlg::ShowERROR_in_out_message(CString error , char importence)
{
	//error - текст ошибки
	//importence - ее важность
	m_OutMessageError.SetWindowText(error);
	
	switch (importence)
	{
		case 0:m_OutMessageError.SetTextColor(RGB(0, 0, 0)); break;
		case 1:m_OutMessageError.SetTextColor(RGB(55, 18, 0)); break;
		case 2:m_OutMessageError.SetTextColor(RGB(55, 18, 0)); break;
		case 3:m_OutMessageError.SetTextColor(RGB(128, 128, 128)); break;
		case 4:m_OutMessageError.SetTextColor(RGB(0, 200, 0)); break;
		case 5:m_OutMessageError.SetTextColor(RGB(0, 200, 0)); break;
		case 6:m_OutMessageError.SetTextColor(RGB(0, 200, 0)); break;
		case 7:m_OutMessageError.SetTextColor(RGB(0, 0, 255)); break;
		case 8:m_OutMessageError.SetTextColor(RGB(0, 0, 255)); break;
		case 9:m_OutMessageError.SetTextColor(RGB(255, 0, 0)); break;
		default:AfxMessageBox(_T("Чет не заладилось с важностью ошибки в отправляемом сообщении..."));
	}
}

/************************	ShowERROR_in_out_message	**********************
*	 показывает ошибку в принимаемом сообщении
******************************************************************************/
void CUSART_Dlg::ShowERROR_in_input_message(CString error , char importence)
{
	//error - текст ошибки
	//importence - ее важность
	m_InMessageError.SetWindowText(error);
	switch (importence)
	{
		case 0:m_InMessageError.SetTextColor(RGB(0, 0, 0)); break;
		case 1:m_InMessageError.SetTextColor(RGB(55, 18, 0)); break;
		case 2:m_InMessageError.SetTextColor(RGB(55, 18, 0)); break;
		case 3:m_InMessageError.SetTextColor(RGB(128, 128, 128)); break;
		case 4:m_InMessageError.SetTextColor(RGB(0, 200, 0)); break;
		case 5:m_InMessageError.SetTextColor(RGB(0, 200, 0)); break;
		case 6:m_InMessageError.SetTextColor(RGB(0, 200, 0)); break;
		case 7:m_InMessageError.SetTextColor(RGB(0, 0, 255)); break;
		case 8:m_InMessageError.SetTextColor(RGB(0, 0, 255)); break;
		case 9:m_InMessageError.SetTextColor(RGB(255, 0, 0)); break;
		default:AfxMessageBox(_T("Чет не заладилось с важностью ошибки в принимаемом сообщении..."));
	}
            
}

HBRUSH CUSART_Dlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	/*
	Поменяем цвета шрифтов для большей наглядности
	*/

	/*
	Раскопируем код, для большей гибкости
	*/
	if(*pWnd==*GetDlgItem(IDC_ERROR_INMESSAGE))//ошибки в принимаемом сообщении
	{
		
	}
	return hbr;
}

/*******************    OnEnChangeOutMessage()   **********************
*		Эта функция вызывается, когда пользователь поменял посылку
*	в окне отавлемого сообщения. Это ему позволятся делать только в
*	режиме ручной смены посылки
***********************************************************************/
void CUSART_Dlg::OnEnChangeOutMessage()
{
	static bool first=true;	//чтобы постоянно не показыватьпредупреждающее сообщение

	if(pPROTOCOL->Mode==9 && first)
	{//установлен режим ручного редактирования посылки
		this->ShowERROR_in_out_message(_T("Изменение этой строки вручную опасно! Время передачи и \r\nприема осталось неизмененным."),9);
		first =false;
	}
}

/*****************    ShowOutputMessage(void)   *****************************
*	Показывает на экране отправляемое сообщение. Изюминка в том, что выводятся
*	шестнадцатеричные значения и дополнительная информация
****************************************************************************/
void CUSART_Dlg::ShowOutputMessage(void)
{//в этой функции сообщение, скомпонованное в классе протокола оказывается на экране
	
	CString str;
	str.Format(_T("Успешная компановка сообщения, буфер заполнен,\r\n РАЗМЕР ПОСЫЛКИ: %d байт \r\n ЧЕК СУММА: H'%x'"),pPROTOCOL->NextMessage.length,pPROTOCOL->NextMessage.summa);
	ShowERROR_in_out_message(str,0);
	/*
	Здесь один раз было исключение, поэтому я решил вставить проверку
	*/
	if(m_OutMessage.m_hWnd!=NULL)
	{
		if(pPROTOCOL->pProtocolOptions->o_HexDisplayOutMessage)//нужно ли показывать шестнадцатеричные значения?
		{
			m_OutMessage.SetWindowText(GiveStrFromHex(pPROTOCOL->NextMessage.text,pPROTOCOL->NextMessage.length));
		}
		else
		{
			//покажем на экране то, что сможем, т.к. в теле м/б нули
			USES_CONVERSION;	//т.к. мне возможно придется отображать в UNICODE
			m_OutMessage.SetWindowText(A2T((char*)pPROTOCOL->NextMessage.text));
		}
	}
	else
	{
		m_pError->SetERROR(_T("Непонятно почему, но попытались вывести принятое сообщение в несуществующее окно"),ERP_FATALERROR | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
	}
}

/*****************************    OnFile()   ******************************
*		Хотим показать пользователю блокнотик с принятыми (или отправленными)
*	по сети сообщениями
**************************************************************************/
void CUSART_Dlg::OnFile()
{
	if(!(pPROTOCOL->pProtocolOptions->o_LogInMessage || pPROTOCOL->pProtocolOptions->o_LogOutMessage))
	{//просто в установках не стоит запрос на запись отправляемых или принимаемых 
	//сообщений и просто выходим
		MessageBox(_T("Опция записи сообщений в файл отключена, отображённая информация м.б. недостоверной"),_T("Уведомление"),MB_ICONWARNING);
	}
	CString str;
	TCHAR buff[255];
	GetCurrentDirectory(255,buff);
	str.Format(_T("Notepad.exe %s\\")+pPROTOCOL->pProtocolOptions->o_inMesFile,buff);
	
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );
	if( !CreateProcess( NULL, (LPTSTR)str.GetBuffer(), NULL, NULL, FALSE,
		0, NULL, NULL, &si, &pi))
	{
		m_pError->SetERROR(_T("Невозможно запустить блокнотик для показа файла листинга сообщений."), ERP_ERROR | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
		return;
	}
	// Close process and thread handles.
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	
}
void CUSART_Dlg::OnTimer(UINT nIDEvent)
{
	CString str;
	if(nIDEvent==1)//сработал таймер порта
	{
		
		
	}
	else
		if(nIDEvent==2)//сработал мой таймер отладки
		{
			static unsigned short char_count=0;	//счетчик отправленных символов
			char simbol;
			if((char*)pPROTOCOL->NextMessage.text!="")
			{
				simbol=pPROTOCOL->NextMessage.text[(int)char_count];
				::SendMessage(this->m_hWnd, WM_COMM_RXCHAR, (WPARAM) simbol, (LPARAM) 99);
				char_count++;//наращиваем количество отправленных символов
			}
			else
			{
				 ShowERROR_in_out_message(_T("БУФЕР ОТПРАВКИ ПУСТ"),1);
			}
			if(pPROTOCOL->NextMessage.length==char_count)//передали все символы 
			{
				/*В режиме эмуляции нет возможности
				передавать непрерывно*/
				pPROTOCOL->AddOutMessageCount();
				KillTimer(2); //шоу заканчивается
				char_count=0;
				//если нужно, запишем в файл
				pPROTOCOL->WriteMesFile(&pPROTOCOL->NextMessage,UART_MSG_TRANSEIVED);
				((CButton*)GetDlgItem(IDC_START))->SetWindowText(_T("СТАРТ!"));
			}
		}
	//CDialog::OnTimer(nIDEvent);
}

void CUSART_Dlg::OnBnClickedOptions()
{
	//окошко с дополнительными настройками алгоритма связи
	HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(pUSARTDLL->hModule);
	CFlagsDlg m_flags(this);
	

	//Здесь вызывается окно настроек программы
	if(m_flags.m_hWnd==NULL)
	{//покажем только, если его еще нет на экране
		int res=(int)m_flags.DoModal();
		//res==1 - нажато ОК
		//res==2 - нажато CANCEL
		/*
		Если ОК - сохраняем новые настройки,
			CANCEL -оставляем  старые
		*/
		if(res==1)
		{
			pPROTOCOL->pProtocolOptions->o_emulation=m_flags.o_emulation1;
			pPROTOCOL->pProtocolOptions->o_verify=m_flags.o_verify1;
			pPROTOCOL->pProtocolOptions->o_nepr=m_flags.o_nepr1;
			//может открыть другой файл?
			if(pPROTOCOL->pProtocolOptions->o_outMesFile!=m_flags.o_outMesFile1)
			{
				//заменим файл листинга принятых сообщений		
				pPROTOCOL->pProtocolOptions->o_outMesFile=m_flags.o_outMesFile1;
				pPROTOCOL->OutMesFileOpen();
				//прочтем очередное сообщение
				pPROTOCOL->CompileALL();
			}
			if(pPROTOCOL->pProtocolOptions->o_OFF!=m_flags.o_OFF1)
			{
				pPROTOCOL->pProtocolOptions->o_OFF=m_flags.o_OFF1;
				if(pPROTOCOL->pProtocolOptions->o_OFF==false)//переключились на общую передачу
				{
					pPROTOCOL->CompileALL();//покажем посылку, скомпилированную программно
					//ЭЛЕМЕНТЫ УПРАВЛЕНИЯ, КОТОРЫЕ НУЖНО ПОКАЗАТЬ
					//ShowControl(SW_SHOW);
					//переведем указатель
					if(!pPROTOCOL->CompileALL())
					{
						//m_pError->SetERROR(pPROTOCOL->m_pError.GetLastError(),pPROTOCOL->m_pError.m_iPriority);
						return;
					}
				}
				else//переключились на передачу из файла
				{
					//ЭЛЕМЕНТЫ УПРАВЛЕНИЯ, КОТОРЫЕ НУЖНО СПРЯТАТЬ
					///ShowControl(SW_HIDE);
					
					//Откроем файл отправки
					pPROTOCOL->OutMesFileOpen();
					//прочтем очередное сообщение
					pPROTOCOL->CompileALL();
				}
			}

			if(pPROTOCOL->pProtocolOptions->o_inMesFile!=m_flags.o_inMesFile1)
			{
				//заменим файл листинга принятых сообщений		
				pPROTOCOL->pProtocolOptions->o_inMesFile=m_flags.o_inMesFile1;
				pPROTOCOL->InMesFileOpen();
			}

			if(pPROTOCOL->pProtocolOptions->o_HexDisplayOutMessage!=m_flags.o_HexDisplayOutMessage1)
			{
				pPROTOCOL->pProtocolOptions->o_HexDisplayOutMessage=m_flags.o_HexDisplayOutMessage1;
				ShowOutputMessage();
			}

			if(pPROTOCOL->pProtocolOptions->o_HexDisplayInMessage!=m_flags.o_HexDisplayInMessage1)
			{
				pPROTOCOL->pProtocolOptions->o_HexDisplayInMessage=m_flags.o_HexDisplayInMessage1;
			}

			if(pPROTOCOL->pProtocolOptions->o_LogOutMessage!=m_flags.o_LogOutMessage1)
			{
				pPROTOCOL->pProtocolOptions->o_LogOutMessage=m_flags.o_LogOutMessage1;
			}

			if(pPROTOCOL->pProtocolOptions->o_LogInMessage!=m_flags.o_LogInMessage1)
			{
				pPROTOCOL->pProtocolOptions->o_LogInMessage=m_flags.o_LogInMessage1;
			}
			
			if(pPROTOCOL->pProtocolOptions->o_LogTimeMessages!=m_flags.o_LogTimeMessages1)
			{
				pPROTOCOL->pProtocolOptions->o_LogTimeMessages=m_flags.o_LogTimeMessages1;
			}

			if (pPROTOCOL->pProtocolOptions->o_LogInByteStuffing != m_flags.o_LogInByteStuffing1)
			{
				pPROTOCOL->pProtocolOptions->o_LogInByteStuffing = m_flags.o_LogInByteStuffing1;
			}
			
			if(pPROTOCOL->pProtocolOptions->o_Period!=m_flags.o_Period1)
			{
				pPROTOCOL->pProtocolOptions->o_Period=m_flags.o_Period1;
			}

			if(pPROTOCOL->pProtocolOptions->o_EmulSpeed!=m_flags.o_EmulSpeed1)
			{
				pPROTOCOL->pProtocolOptions->o_EmulSpeed=m_flags.o_EmulSpeed1;
			}
		}
	}
	else
	{
		m_flags.SetForegroundWindow();
	}
	AfxSetResourceHandle(hInstOld); // restore the old resource chain
}

/**********************	OnCommunication (WPARAM ch, LPARAM port) *******************
*	Вызывается, когда отправлен очередной символ
***************************************************************************/
LONG CUSART_Dlg::OnCommunication (WPARAM ch, LPARAM port)
{
	//Получили очередной символ
	if(!pPROTOCOL->ReceiveChar(static_cast <unsigned char>(ch)))
	{
		//пока не знаю
	}
	return 0;
}

/**********************	OnTXEmpty(WPARAM, LPARAM port) ****************************
*	Вызывается, когда отправлен последний символ
***************************************************************************/
LONG CUSART_Dlg::OnTXEmpty(WPARAM, LPARAM port)
{	
	if(pTranceivingMessage==NULL)
	{//это похоже глюк, но это случается
		return 0;
	}

	#ifdef DELAY_COMPILE_ALL
	dlg.Dlg4.pPROTOCOL->CompileALL();//подготовим следующее сообщение
	#endif
	
	//может передать еще одно сообщение?
	if(!pPROTOCOL->pProtocolOptions->o_nepr)//если флаг непрерывной передачи
	{//высветим правильную надпись
		this->GetDlgItem(IDC_START)->SetWindowText(_T("СТАРТ!"));
	}

	//оповестим внешнюю программу об удачно отправленном сообщении
	if(pPROTOCOL->pProtocolOptions->NotifyProgram!=NULL)
	{
		pPROTOCOL->pProtocolOptions->NotifyProgram(NP_MESSAGE_TRANSFERED);
	}
	/*Если закомментировать следующие строки, то получим задержку после приема
	, например для снятия RTS в RS485*/
	/*
	HANDLE hTimer = NULL;	   //for timer
	LARGE_INTEGER liDueTime;   //x * 100 nanoseconds; 1sec=-10 000 000
	//грубая перенастройка времени передачи в аргумент для таймера 
	long f=(dlg.Dlg4.pPROTOCOL->NextMessage.peredacha-15)*(-10000);
	liDueTime.QuadPart=f;
	hTimer = CreateWaitableTimer(NULL, TRUE, "WaitableTimer");
	SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0);
	WaitForSingleObject(hTimer, INFINITE);
	*/
	if(pPROTOCOL->pProtocolOptions->var_RS485EN)
	{//включена поддрежка RS485
		m_Ports.SetRTS(false);	//Устанвливаем RTS для приема
	}

	//Увеличиваем счетчик отправленных посылок
	pPROTOCOL->AddOutMessageCount();
	//если нужно, запишем в файл
	pPROTOCOL->WriteMesFile(pTranceivingMessage,UART_MSG_TRANSEIVED);
	pTranceivingMessage=NULL;	//теперь никакое сообщение не отправляется
	
	return 0;
}

/************************   PortFailure(UINT, LONG port)   **************
*		Запрещен доступ к порту. Эта функция писалась для поддержки 
*	CP2102. Когда микросхема подключена - порт успешно был открыт, но 
*	когда ее выдерают, в модуле SerialPort возникает ошибка ERROR_ACCESS_DENIED
*	и посылается об этом сообщение
*************************************************************************/
LONG CUSART_Dlg::PortFailure(UINT, LONG port)
{
	//остановим работу порта
	m_Ports.StopMonitoring();
	//не будем делать из этого ошибку, т.к. может быть просто отключили CP2102
	CString str; str.Format(_T("Невозможно получить доступ к порту %i"),port);
	m_pError->SetERROR(str, ERP_WARNING | ER_WRITE_TO_LOG);
	//выставим флаг, что ни один из портов не открыт
	pPROTOCOL->pProtocolOptions->var_PORT_NUMBER=PN_NOTINITIALIZED;
	return 0;
}

/****************	ReSetCOMport(unsigned char num)	*********************
*	функция, настраивающая СОМ порт с определенным номером.
*	Если пользователю вдруг вздумалось поменять не только номер порта, а
*	еще и дополнительные настройки, например скорость, то сначала он
*	должен провести все изменения в pProtocolOptions, а затем вызвать эту
*	функцию.
*		Также поддерживаются вызовы PN_FIRSTAVAILABLE и PN_NOTINITIALIZED.
*	Используется рекурсия!
************************************************************************/
bool CUSART_Dlg::ReSetCOMport(INT8U num)
{
	if(num==PN_FIRST_AVAILABLE)
	{// инициализируем первый попавшийся порт, т.к. нужный не задан
		INT8U i;
		for(i=1; i<10; i++)
		{//пройдусь по ниболее частым номерам портов
			if(ReSetCOMport(i))
			{//попали на незанятый порт
				break;
			}
		}
		if(i==10)
		{
			m_pError->SetERROR(_T("При инициализации не оказалось ни одного свободного порта"),ERP_WARNING | ER_WRITE_TO_LOG);
			return false;
		}
	}
	else if(num==PN_NOTINITIALIZED)
	{//Последовательный порт не нужно инициализировать при старте
		return false;		//запрещено
	}
	else 
	{//Номер последовательного порта четко задан
		if (m_Ports.InitPort(	this->m_hWnd,										//окно, которому будут передаваться все сообщения
								num,												//порт, который нужно настроить
								pPROTOCOL->pProtocolOptions->var_TRANSFER_SPEED,	//скорость передачи
								pPROTOCOL->pProtocolOptions->var_PARITY,			//контроль четности
								pPROTOCOL->pProtocolOptions->var_BITS_PER_PACKET,	//битность
								pPROTOCOL->pProtocolOptions->var_STOP_BITS))		//стоповых битов
									/*Константы передаются при инициализации CreateMAIN*/
		{
			//ПОРТ НАЙДЕН УСПЕШНО
			pPROTOCOL->pProtocolOptions->var_PORT_NUMBER=num;
		}
		else
		{
			//НЕ НАШЛИ ПОРТОВ
			CString str; str.Format(_T("Запрашиваемый СОМ порт номер %i не найден"), num);
			m_pError->SetERROR(str,ERP_ERROR | ER_WRITE_TO_LOG);
			//теперь программа не настроена ни на один из портов
			pPROTOCOL->pProtocolOptions->var_PORT_NUMBER=PN_NOTINITIALIZED;
			return false;
		}
	}
	
	return true;
}

/*********************    GetIsStarted(void)   **************************************
*		Эта функция возвращае значение того, идет ли в данный момент передача вообще
*	Например при непрерывной передаче, пакеты отпарвляются раз в секунду, но между ними
*	считается, что передача идет, т.к. есть ожидающие отправки пакеты
*************************************************************************************/
bool CUSART_Dlg::GetIsStarted(void)
{
	return this->IsStarted;
}
/********************     GetLastGoodReceived(void)    ******************************
*		Эта функция возвращает, какое количество отправленных пакетов назад был принят
*	последний успешный пакет.
*************************************************************************************/
int	 CUSART_Dlg::GetLastGoodReceived(void)
{
	return this->LastGoodReceived;
}

/********************	GlobalStartTranceiving(void)   ****************************
*		Эта функция выставляет флаг о глобальном начале передачи, а попросту
*	запускает событие с хэндлом
***********************************************************************************/
bool CUSART_Dlg::GlobalStartTranceiving(void)
{
	if(!SetEvent(HGlobalStart))
	{
		CString str=_T("Невозможно выставить глобальное разрешешние передачи");
		if(m_pError!=NULL)
		{
			m_pError->SetERROR(str,ERP_ERROR | ER_SHOW_MESSAGE | ER_WRITE_TO_LOG);
		}
		else
		{
			AfxMessageBox(str);
		}
		return false;
	}
	return true;
}

/********************   GlobalStopTranceiving(void)   ******************************
*		Прямое противопоставление функции GlobalStartTranceiving
************************************************************************************/
bool CUSART_Dlg::GlobalStopTranceiving(void)
{
	if(!ResetEvent(HGlobalStart))
	{
		CString str=_T("Невозможно выставить глобальное запрещение передачи");
		if(m_pError!=NULL)
		{
			m_pError->SetERROR(str,ERP_ERROR | ER_SHOW_MESSAGE | ER_WRITE_TO_LOG);
		}
		else
		{
			AfxMessageBox(str);
		}
		return false;
	}
	return true;
}

/********************    SetTranceiverBusi(void)   ********************************
*		Функция выставляет событие занятости передатчика
**********************************************************************************/
bool CUSART_Dlg::SetTranceiverBusi(void)
{
	if(!ResetEvent(HTranceiverBusi))
	{
		CString str=_T("Невозможно выставить событие временной занятости передатчика");
		if(m_pError!=NULL)
		{
			m_pError->SetERROR(str,ERP_ERROR | ER_SHOW_MESSAGE | ER_WRITE_TO_LOG);
		}
		else
		{
			AfxMessageBox(str);
		}
		return false;
	}
	return true;
}

/********************    ClearTranceiverBusi(void)   ******************************
*		Функция выставляет событие освобождения передатчика
**********************************************************************************/
bool CUSART_Dlg::ClearTranceiverBusi(void)
{
	if(!SetEvent(HTranceiverBusi))
	{
		CString str=_T("Невозможно сбросить событие освобождения передатчика");
		if(m_pError!=NULL)
		{
			m_pError->SetERROR(str,ERP_ERROR | ER_SHOW_MESSAGE | ER_WRITE_TO_LOG);
		}
		else
		{
			AfxMessageBox(str);
		}
		return false;
	}
	return true;
}

/****************************   TranseiveMessage(CMessage* pNextMessage)   *********************
*		Функция, которая прерывает все прыдудущии отправляемые сообщения (если есть)
*	и отправляет свое.
***********************************************************************************************/
bool CUSART_Dlg::TranseiveMessage(CMessage* pMessage)
{
	if(pMessage==NULL) return false;

	if(pPROTOCOL->pProtocolOptions->var_PORT_NUMBER==PN_NOTINITIALIZED)
	{
		m_pError->SetERROR(_T("Невозможно отправить пакет. Последовательный порт не инициализирован."), ERP_WARNING | ER_WRITE_TO_LOG);
		if(pPROTOCOL->pProtocolOptions->NotifyProgram!=NULL)
		{//есть указатель на функцию уведомлений
			pPROTOCOL->pProtocolOptions->NotifyProgram(NP_TRANSFER_ERROR);
		}
		return false;
	}

	//осуществим связь
	m_Ports.ClearTXBuff();//очистим буфер передатчика
	if(pPROTOCOL->pProtocolOptions->var_RS485EN)
	{//включена поддрежка RS485
		m_Ports.SetRTS(true);		//Устанвливаем RTS для передачи
	}

	if(pMessage->text==NULL || pMessage->length==0)
	{
		m_pError->SetERROR(_T("Запрос на посылку сообщения без тела"), ERP_ERROR | ER_WRITE_TO_LOG);
		return false;
	}

	//************Записываем в порт для передачи*************		
	
	//Отсылаем пакет
	if(!m_Ports.WriteToPort((INT8U *)pMessage->text, pMessage->length))
	{
		m_pError->SetERROR(_T("Сообщение не отправлено"), ERP_ERROR | ER_WRITE_TO_LOG);
		return false;
	}
	
	if(pTranceivingMessage!=NULL)
	{//значит еще не успели отправить предыдущее сообщение
		pTranceivingMessage->name=pTranceivingMessage->name+_T("  Прервано из-за другого сообщения");
		pPROTOCOL->WriteMesFile(pTranceivingMessage, UART_MSG_NOTTRANCEIVED);
	}

	pTranceivingMessage=pMessage;	//сохраним указатель на отправляемое сообщение
	
	return true; //всё прошло нормально
}

/***************************************************************
*		НАДОЕЛО ВЫХОДИТЬ ПО НАЖАТИЮ НА ЕНТЕР
****************************************************************/
void CUSART_Dlg::OnOK()
{
	//++++ CDialog::OnOK();
}

/************************    SetMode(INT16U mode)   *********************
*	функция выставляет номер отправляемого сообщения
*************************************************************************/
bool CUSART_Dlg::SetMode(INT16U mode)
{
	//Проверим правильность режима
	if(mode==0)
	{
		m_pError->SetERROR(_T("Режим передатчика не может быть нулевым."), ERP_ERROR | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
		return false;
	}

	if(mode>pPROTOCOL->pProtocolOptions->CompileMessage_x.size()+1)
	{
		m_pError->SetERROR(_T("Выбранное сообщение не существует"), ERP_ERROR | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
		return false;
	}

	//как бы нажмем на нужный элемент в выпадающем списке
	m_ListPacket.SetCurSel(mode-1);	//-1, т.к. в списке элементы начинаются с нуля
	OnVibor();						//чтобы изменения вступили в силу
	return true;
}

/******************    GetMode()   ***************************************
*		функция возвращает номер отправляемого сообщения.
*	Просто возьмем это значение из класса протокола
*************************************************************************/
INT16U CUSART_Dlg::GetMode()
{
	return pPROTOCOL->Mode;
}

/*********************    ClosePort    ******************************************
*		Функция грамотно останавливает работу с портом.
********************************************************************************/
void CUSART_Dlg::ClosePort(void)
{
	//передаю на нижний уровень
	m_Ports.ClosePort();
	//нет открытого последовательного порта
	pPROTOCOL->pProtocolOptions->var_PORT_NUMBER=PN_NOTINITIALIZED;
}

/****************************	DestroyUSART(CUSART_Dlg* pDlg)	*****************
*		Эта функция определена в классе CUSART_Dlg как friеnd и поэтому имеет доступ
*	к закрытому деструктору
*********************************************************************************/
void DestroyUSART(CUSART_Dlg* pDlg)
{
	//pDlg->SendMessage(WM_CLOSE);
	//pDlg->~CUSART_Dlg();
	delete pDlg;
}


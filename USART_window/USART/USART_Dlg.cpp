// USART_Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "USART_Dlg.h"
#include "Resource.h"					//��� ����� �� �� �������� ID-�����
#include "FlagsDlg.h"					//������ � �����������
#include "PROTOCOL_Struct.h"			//���� � ����������

/*****************************************************************************
*		����� ������� ������������� ����������
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
*		���� ����� ����� ���������� �������� (� �������� �������, ��� � ������������ 
*	�����������, �� ��� ������������ ������� �� ����������� ����, � ������ ��� ����� ������
*	��������� ��������� ���������
***************************************************************************************/
void CALLBACK TrancieverTimer(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2) 
{ 
	CUSART_Dlg* dlg=reinterpret_cast<CUSART_Dlg*>(dwUser);
	CString str;
	#ifndef DELAY_COMPILE_ALL
	//���� ��� ������ � ��������������� �����������, �� �������� ������� ������
	if(!dlg->pPROTOCOL->CompileALL())
	{
		dlg->ShowERROR_in_out_message(_T("������� CompileALL ���������� � �������"),9);
		return;
	}
	#endif
	
	if(!dlg->TranseiveMessage(&dlg->pPROTOCOL->NextMessage))
	{//���������� �������� ��������� �� �������
		if(dlg->pPROTOCOL->pProtocolOptions->NotifyProgram!=NULL)
		{//���� ��������� �� ������� �����������
			dlg->pPROTOCOL->pProtocolOptions->NotifyProgram(NP_TRANSFER_ERROR);
		}
		return;	
	}
	return;
}

BOOL CUSART_Dlg::Create(UINT ID, CWnd* pParentWnd)
{
	BOOL result=CDialog::Create(ID, pParentWnd);
	this->IsStarted=false;						//�������� ���� �� ����������
	this->LastGoodReceived=0;					//������ �� ���� ��� �� ���������� � �� ���������
	pTranceivingMessage=NULL;					//������� ��������� �� ������������
	
	//���������� ���������� � ���������� ���������� �������� (�� ��������� �������)
	HGlobalStart=CreateEvent(NULL,FALSE,FALSE,NULL);
	ASSERT(HGlobalStart);	//�� ������ ������
	
	//���������� ���������� ���������� � ��������� ��������� ����������� (�� ����� �������� ���������)
	HTranceiverBusi=CreateEvent(NULL,FALSE,TRUE,NULL);
	ASSERT(HTranceiverBusi);

	return result;
}

/*****************************	OnInitDialog()	*******************************************
*		������������� �������
******************************************************************************************/
BOOL CUSART_Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();



	return TRUE;  // return TRUE unless you set the focus to a control
}

bool CUSART_Dlg::MyInit(ERROR_Class* m_ERROR, POTOCOL_OPTIONS_Struct* pProtocolOptions, PROTOCOL_Struct *protocol)
{
	//�������� ����������
	if(m_ERROR==NULL || pProtocolOptions==NULL || protocol==NULL)	return false;

	this->pPROTOCOL=protocol;							//��������� �� ��������
	this->pPROTOCOL->pProtocolOptions=pProtocolOptions;
	this->m_pError=m_ERROR;
	
	//������������� ����������������� �����
	if(pProtocolOptions->var_PORT_NUMBER!=PN_NOTINITIALIZED)
	{//������������ ����� ��������� ���������������� ����
		ReSetCOMport(pProtocolOptions->var_PORT_NUMBER);
	}
	
	//����������������� ����� ���������
	if(!pPROTOCOL->InitPROTOCOL(this, m_pError))
	{//���� ������, ���������� ����������� ����������
		return false;
	}

	//��������� �������������� ����� ������������� ���������
	//++++ m_ListPacket.SetCurSel(0);
	//++++ this->OnVibor();

	#ifndef	DELAY_COMPILE_ALL
	//�������� ������� ��� ������������ ����������
	GetDlgItem(IDC_OUT_MESSAGE_CAPTION)->SetWindowText(_T("��������� ������������ ���������:"));
	#endif

	//�������� ������ ������������ ���������, ����� ������� ��� ����� ����
	//������ ������ ����� ������ ������ ������� ����� �������
	m_OutMessage.SetReadOnly(true);

	return true;	//����� ��� ���������
}

/*********************   OnStart()   ************************************************
*		������ �� ������ �����/����, ������ ����� ���������, ������ ��� ���������
*	��������������
*************************************************************************************/
void CUSART_Dlg::OnStart() 
{
	CString str;
	((CButton*)GetDlgItem(IDC_START))->GetWindowText(str);
	if(str==_T("�����!"))
	{
		StartTransfer();
	}
	else if(str==_T("����!"))
	{
		StopTransfer();
	}
}

/*******************************   StartTransfer(void)       *****************************
*		��� ������� ����������, ����� ����� ������� �� ������ ����� ���������, ���
*	����� ������ ��������.
******************************************************************************************/
bool CUSART_Dlg::StartTransfer(void)
{
	#ifndef DELAY_COMPILE_ALL
	//���� ��� ������ � ��������������� �����������, �� �������� ������� ������
	if(!pPROTOCOL->CompileALL())
	{
		ShowERROR_in_out_message(_T("������� CompileALL ���������� � �������"),9);
		return false;
	}
	#endif

	if(pPROTOCOL->pProtocolOptions->o_emulation) 
	{//�������� ��������������, ������ �������� ��� ����� �� �������
		if((char*)pPROTOCOL->NextMessage.text=="")//���� ������ ����������
		{
			m_pError->SetERROR(_T("����� ����, �������� ��������"), ERP_WARNING | ER_WRITE_TO_LOG);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_START))->SetWindowText(_T("����!"));
			m_pError->SetERROR(_T("����� ��������"),ERP_NORMAL);	//?????
			my_timer=SetTimer(0x02,pPROTOCOL->pProtocolOptions->o_EmulSpeed,NULL);
		}
	}
	else
	{//��� �������� ����� ���������� ���� RS232

		m_Ports.ClearTXBuff();	//�� ������ ������ ������� ����������

		//�������� �� ������� ���� ������ ���������
		if(!TranseiveMessage(&pPROTOCOL->NextMessage))
		{
			return false;
		}
		if(pPROTOCOL->pProtocolOptions->o_nepr)
		{//��� �� ��������� ���������, ������� �� ��� ������� ���������
		 //����� ����� �������� ����������
			RealTimer.SetTimerCallback(pPROTOCOL->pProtocolOptions->o_Period,5,pPROTOCOL->pProtocolOptions->o_nepr,TrancieverTimer,reinterpret_cast<DWORD>(this));	//������ �� ���������� ������
		}
	}

	//�� ������ �������� ������� �� ������
	((CButton*)GetDlgItem(IDC_START))->SetWindowText(_T("����!"));
	return true;
}

/*******************************    StopTransfer(void)    *******************************
*		��� ������� ����������, ����� ����� ���������� ��������, ��� ������ �� ������, ����
*	��� �� ����
*****************************************************************************************/
bool CUSART_Dlg::StopTransfer(void)
{
	if(pPROTOCOL->pProtocolOptions->o_emulation)
	{//�������� ��������������, ������ �������� ��� ����� �� �������
		KillTimer(0x02);	//��������� ������ �������� ��������
	}
	else
	{//��� �������� ����� ���������� ���� RS232
		m_Ports.SetRTS(false);			//����������� RTS � 0, ��� ������
		RealTimer.CancelTimer();		//��������� ������ ��������
	}
	//�� ������ �������� ������� �� ������
	((CButton*)GetDlgItem(IDC_START))->SetWindowText(_T("�����!"));
	return true;
}

/************************    OnVibor()   ********************************
*		��� ������� ���������� ����� ������������ ������� �������� ��������� 
*	���������, ������� ����� ���������
**************************************************************************/
void CUSART_Dlg::OnVibor()
{
	int i=m_ListPacket.GetCurSel();
	
	pPROTOCOL->Mode=i+1;//����� �� ��������� ������� �����

	//�������� �� �������� �� ��������� ����� ������ ������ �������
	if(pPROTOCOL->Mode==pPROTOCOL->pProtocolOptions->CompileMessage_x.size()+1)
	{//���� ����� ��������� � ������ ����� ���� ����������� ���������
		//������� ������ ������������� ��������� ���������
		m_OutMessage.SetReadOnly(false);
	}
	else
	{//��� ������� ���������
		//������� ������ ������������� ��������� �����������
		m_OutMessage.SetReadOnly(true);
	}


	/*����������� ������� ���� � ������ ���������*/
	#ifdef DELAY_COMPILE_ALL
	//����� �� ���������� ����� ���������
	if(!pPROTOCOL->CompileALL())
	{//��������� �� ������
		pUSART_Dlg->ShowERROR(pPROTOCOL->m_pError.GetLastError(),pPROTOCOL->m_pError.m_iPriority,true);
	}
	#endif
}

/*****************	OnVerify() *****************************************
*	������ �� ������� �� ��� ������ ����� ���������� ���������, �������
*	��������� � ������� ������������ ���������. ��� ����� ������� ��� 
*	�������, ����� ����� �������� ���������� ���� �������� ���������, �� �����
*	��������� ��������� ������ � ���
**********************************************************************/
void CUSART_Dlg::OnVerify() 
{//������ �� ������ �������������

	CString str,error;
	unsigned char *temp_buf;
	temp_buf=new unsigned char [pPROTOCOL->pProtocolOptions->var_MAX_LEN];
	
	m_ReceiveMessage.GetWindowText(str);
	int len=GiveHexFromStr(str,temp_buf,&error);
	if(len==0)
	{//���-�� � ���� ���� ������ ����������, ���� ������ ����
		m_pError->SetERROR(error, 3);
		delete []temp_buf;	//��������� ��������� ������
		return;
	}
	
	//�������� ������������ ����������� �����, ������ ������ ����� ������� ������ ��������� ����
	unsigned char sum=pPROTOCOL->pProtocolOptions->CURRENT_SUMM(temp_buf,1,len-1);
	CString sum_right;
	if(sum==temp_buf[len-1])
	{
		sum_right=_T("����������� ����� ���������");
	}
	else
	{
		sum_right=_T("����������� ����� �������");
	}
	//���� ���� ����������� ����� �������, �� ��� ����� ���������� ���������, � ���� � ������������ ����� ���������

	if(!pPROTOCOL->pProtocolOptions->OnVerify(&error,temp_buf))
	{//���� ������ ��� ����������
		//m_pError->SetERROR(pPROTOCOL->m_pError.GetLastError(),pPROTOCOL->m_pError.m_iPriority);
	}
	else
	{//��� ����������, ������� ���������� � ������������ ���������
		ShowERROR_in_input_message(error+_T("\n")+sum_right,0);
	}
	delete []temp_buf;	//��������� ��������� ������

}

/************************	ShowERROR_in_out_message	**********************
*	 ���������� ������ � ������������ ���������
******************************************************************************/
void CUSART_Dlg::ShowERROR_in_out_message(CString error , char importence)
{
	//error - ����� ������
	//importence - �� ��������
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
		default:AfxMessageBox(_T("��� �� ���������� � ��������� ������ � ������������ ���������..."));
	}
}

/************************	ShowERROR_in_out_message	**********************
*	 ���������� ������ � ����������� ���������
******************************************************************************/
void CUSART_Dlg::ShowERROR_in_input_message(CString error , char importence)
{
	//error - ����� ������
	//importence - �� ��������
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
		default:AfxMessageBox(_T("��� �� ���������� � ��������� ������ � ����������� ���������..."));
	}
            
}

HBRUSH CUSART_Dlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	/*
	�������� ����� ������� ��� ������� �����������
	*/

	/*
	����������� ���, ��� ������� ��������
	*/
	if(*pWnd==*GetDlgItem(IDC_ERROR_INMESSAGE))//������ � ����������� ���������
	{
		
	}
	return hbr;
}

/*******************    OnEnChangeOutMessage()   **********************
*		��� ������� ����������, ����� ������������ ������� �������
*	� ���� ���������� ���������. ��� ��� ���������� ������ ������ �
*	������ ������ ����� �������
***********************************************************************/
void CUSART_Dlg::OnEnChangeOutMessage()
{
	static bool first=true;	//����� ��������� �� ������������������������� ���������

	if(pPROTOCOL->Mode==9 && first)
	{//���������� ����� ������� �������������� �������
		this->ShowERROR_in_out_message(_T("��������� ���� ������ ������� ������! ����� �������� � \r\n������ �������� ������������."),9);
		first =false;
	}
}

/*****************    ShowOutputMessage(void)   *****************************
*	���������� �� ������ ������������ ���������. �������� � ���, ��� ���������
*	����������������� �������� � �������������� ����������
****************************************************************************/
void CUSART_Dlg::ShowOutputMessage(void)
{//� ���� ������� ���������, �������������� � ������ ��������� ����������� �� ������
	
	CString str;
	str.Format(_T("�������� ���������� ���������, ����� ��������,\r\n ������ �������: %d ���� \r\n ��� �����: H'%x'"),pPROTOCOL->NextMessage.length,pPROTOCOL->NextMessage.summa);
	ShowERROR_in_out_message(str,0);
	/*
	����� ���� ��� ���� ����������, ������� � ����� �������� ��������
	*/
	if(m_OutMessage.m_hWnd!=NULL)
	{
		if(pPROTOCOL->pProtocolOptions->o_HexDisplayOutMessage)//����� �� ���������� ����������������� ��������?
		{
			m_OutMessage.SetWindowText(GiveStrFromHex(pPROTOCOL->NextMessage.text,pPROTOCOL->NextMessage.length));
		}
		else
		{
			//������� �� ������ ��, ��� ������, �.�. � ���� �/� ����
			USES_CONVERSION;	//�.�. ��� �������� �������� ���������� � UNICODE
			m_OutMessage.SetWindowText(A2T((char*)pPROTOCOL->NextMessage.text));
		}
	}
	else
	{
		m_pError->SetERROR(_T("��������� ������, �� ���������� ������� �������� ��������� � �������������� ����"),ERP_FATALERROR | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
	}
}

/*****************************    OnFile()   ******************************
*		����� �������� ������������ ��������� � ��������� (��� �������������)
*	�� ���� �����������
**************************************************************************/
void CUSART_Dlg::OnFile()
{
	if(!(pPROTOCOL->pProtocolOptions->o_LogInMessage || pPROTOCOL->pProtocolOptions->o_LogOutMessage))
	{//������ � ���������� �� ����� ������ �� ������ ������������ ��� ����������� 
	//��������� � ������ �������
		MessageBox(_T("����� ������ ��������� � ���� ���������, ����������� ���������� �.�. �������������"),_T("�����������"),MB_ICONWARNING);
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
		m_pError->SetERROR(_T("���������� ��������� ��������� ��� ������ ����� �������� ���������."), ERP_ERROR | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
		return;
	}
	// Close process and thread handles.
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	
}
void CUSART_Dlg::OnTimer(UINT nIDEvent)
{
	CString str;
	if(nIDEvent==1)//�������� ������ �����
	{
		
		
	}
	else
		if(nIDEvent==2)//�������� ��� ������ �������
		{
			static unsigned short char_count=0;	//������� ������������ ��������
			char simbol;
			if((char*)pPROTOCOL->NextMessage.text!="")
			{
				simbol=pPROTOCOL->NextMessage.text[(int)char_count];
				::SendMessage(this->m_hWnd, WM_COMM_RXCHAR, (WPARAM) simbol, (LPARAM) 99);
				char_count++;//���������� ���������� ������������ ��������
			}
			else
			{
				 ShowERROR_in_out_message(_T("����� �������� ����"),1);
			}
			if(pPROTOCOL->NextMessage.length==char_count)//�������� ��� ������� 
			{
				/*� ������ �������� ��� �����������
				���������� ����������*/
				pPROTOCOL->AddOutMessageCount();
				KillTimer(2); //��� �������������
				char_count=0;
				//���� �����, ������� � ����
				pPROTOCOL->WriteMesFile(&pPROTOCOL->NextMessage,UART_MSG_TRANSEIVED);
				((CButton*)GetDlgItem(IDC_START))->SetWindowText(_T("�����!"));
			}
		}
	//CDialog::OnTimer(nIDEvent);
}

void CUSART_Dlg::OnBnClickedOptions()
{
	//������ � ��������������� ����������� ��������� �����
	HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(pUSARTDLL->hModule);
	CFlagsDlg m_flags(this);
	

	//����� ���������� ���� �������� ���������
	if(m_flags.m_hWnd==NULL)
	{//������� ������, ���� ��� ��� ��� �� ������
		int res=(int)m_flags.DoModal();
		//res==1 - ������ ��
		//res==2 - ������ CANCEL
		/*
		���� �� - ��������� ����� ���������,
			CANCEL -���������  ������
		*/
		if(res==1)
		{
			pPROTOCOL->pProtocolOptions->o_emulation=m_flags.o_emulation1;
			pPROTOCOL->pProtocolOptions->o_verify=m_flags.o_verify1;
			pPROTOCOL->pProtocolOptions->o_nepr=m_flags.o_nepr1;
			//����� ������� ������ ����?
			if(pPROTOCOL->pProtocolOptions->o_outMesFile!=m_flags.o_outMesFile1)
			{
				//������� ���� �������� �������� ���������		
				pPROTOCOL->pProtocolOptions->o_outMesFile=m_flags.o_outMesFile1;
				pPROTOCOL->OutMesFileOpen();
				//������� ��������� ���������
				pPROTOCOL->CompileALL();
			}
			if(pPROTOCOL->pProtocolOptions->o_OFF!=m_flags.o_OFF1)
			{
				pPROTOCOL->pProtocolOptions->o_OFF=m_flags.o_OFF1;
				if(pPROTOCOL->pProtocolOptions->o_OFF==false)//������������� �� ����� ��������
				{
					pPROTOCOL->CompileALL();//������� �������, ���������������� ����������
					//�������� ����������, ������� ����� ��������
					//ShowControl(SW_SHOW);
					//��������� ���������
					if(!pPROTOCOL->CompileALL())
					{
						//m_pError->SetERROR(pPROTOCOL->m_pError.GetLastError(),pPROTOCOL->m_pError.m_iPriority);
						return;
					}
				}
				else//������������� �� �������� �� �����
				{
					//�������� ����������, ������� ����� ��������
					///ShowControl(SW_HIDE);
					
					//������� ���� ��������
					pPROTOCOL->OutMesFileOpen();
					//������� ��������� ���������
					pPROTOCOL->CompileALL();
				}
			}

			if(pPROTOCOL->pProtocolOptions->o_inMesFile!=m_flags.o_inMesFile1)
			{
				//������� ���� �������� �������� ���������		
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
*	����������, ����� ��������� ��������� ������
***************************************************************************/
LONG CUSART_Dlg::OnCommunication (WPARAM ch, LPARAM port)
{
	//�������� ��������� ������
	if(!pPROTOCOL->ReceiveChar(static_cast <unsigned char>(ch)))
	{
		//���� �� ����
	}
	return 0;
}

/**********************	OnTXEmpty(WPARAM, LPARAM port) ****************************
*	����������, ����� ��������� ��������� ������
***************************************************************************/
LONG CUSART_Dlg::OnTXEmpty(WPARAM, LPARAM port)
{	
	if(pTranceivingMessage==NULL)
	{//��� ������ ����, �� ��� ���������
		return 0;
	}

	#ifdef DELAY_COMPILE_ALL
	dlg.Dlg4.pPROTOCOL->CompileALL();//���������� ��������� ���������
	#endif
	
	//����� �������� ��� ���� ���������?
	if(!pPROTOCOL->pProtocolOptions->o_nepr)//���� ���� ����������� ��������
	{//�������� ���������� �������
		this->GetDlgItem(IDC_START)->SetWindowText(_T("�����!"));
	}

	//��������� ������� ��������� �� ������ ������������ ���������
	if(pPROTOCOL->pProtocolOptions->NotifyProgram!=NULL)
	{
		pPROTOCOL->pProtocolOptions->NotifyProgram(NP_MESSAGE_TRANSFERED);
	}
	/*���� ���������������� ��������� ������, �� ������� �������� ����� ������
	, �������� ��� ������ RTS � RS485*/
	/*
	HANDLE hTimer = NULL;	   //for timer
	LARGE_INTEGER liDueTime;   //x * 100 nanoseconds; 1sec=-10 000 000
	//������ ������������� ������� �������� � �������� ��� ������� 
	long f=(dlg.Dlg4.pPROTOCOL->NextMessage.peredacha-15)*(-10000);
	liDueTime.QuadPart=f;
	hTimer = CreateWaitableTimer(NULL, TRUE, "WaitableTimer");
	SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0);
	WaitForSingleObject(hTimer, INFINITE);
	*/
	if(pPROTOCOL->pProtocolOptions->var_RS485EN)
	{//�������� ��������� RS485
		m_Ports.SetRTS(false);	//������������ RTS ��� ������
	}

	//����������� ������� ������������ �������
	pPROTOCOL->AddOutMessageCount();
	//���� �����, ������� � ����
	pPROTOCOL->WriteMesFile(pTranceivingMessage,UART_MSG_TRANSEIVED);
	pTranceivingMessage=NULL;	//������ ������� ��������� �� ������������
	
	return 0;
}

/************************   PortFailure(UINT, LONG port)   **************
*		�������� ������ � �����. ��� ������� �������� ��� ��������� 
*	CP2102. ����� ���������� ���������� - ���� ������� ��� ������, �� 
*	����� �� ��������, � ������ SerialPort ��������� ������ ERROR_ACCESS_DENIED
*	� ���������� �� ���� ���������
*************************************************************************/
LONG CUSART_Dlg::PortFailure(UINT, LONG port)
{
	//��������� ������ �����
	m_Ports.StopMonitoring();
	//�� ����� ������ �� ����� ������, �.�. ����� ���� ������ ��������� CP2102
	CString str; str.Format(_T("���������� �������� ������ � ����� %i"),port);
	m_pError->SetERROR(str, ERP_WARNING | ER_WRITE_TO_LOG);
	//�������� ����, ��� �� ���� �� ������ �� ������
	pPROTOCOL->pProtocolOptions->var_PORT_NUMBER=PN_NOTINITIALIZED;
	return 0;
}

/****************	ReSetCOMport(unsigned char num)	*********************
*	�������, ������������� ��� ���� � ������������ �������.
*	���� ������������ ����� ���������� �������� �� ������ ����� �����, �
*	��� � �������������� ���������, �������� ��������, �� ������� ��
*	������ �������� ��� ��������� � pProtocolOptions, � ����� ������� ���
*	�������.
*		����� �������������� ������ PN_FIRSTAVAILABLE � PN_NOTINITIALIZED.
*	������������ ��������!
************************************************************************/
bool CUSART_Dlg::ReSetCOMport(INT8U num)
{
	if(num==PN_FIRST_AVAILABLE)
	{// �������������� ������ ���������� ����, �.�. ������ �� �����
		INT8U i;
		for(i=1; i<10; i++)
		{//�������� �� ������� ������ ������� ������
			if(ReSetCOMport(i))
			{//������ �� ��������� ����
				break;
			}
		}
		if(i==10)
		{
			m_pError->SetERROR(_T("��� ������������� �� ��������� �� ������ ���������� �����"),ERP_WARNING | ER_WRITE_TO_LOG);
			return false;
		}
	}
	else if(num==PN_NOTINITIALIZED)
	{//���������������� ���� �� ����� ���������������� ��� ������
		return false;		//���������
	}
	else 
	{//����� ����������������� ����� ����� �����
		if (m_Ports.InitPort(	this->m_hWnd,										//����, �������� ����� ������������ ��� ���������
								num,												//����, ������� ����� ���������
								pPROTOCOL->pProtocolOptions->var_TRANSFER_SPEED,	//�������� ��������
								pPROTOCOL->pProtocolOptions->var_PARITY,			//�������� ��������
								pPROTOCOL->pProtocolOptions->var_BITS_PER_PACKET,	//��������
								pPROTOCOL->pProtocolOptions->var_STOP_BITS))		//�������� �����
									/*��������� ���������� ��� ������������� CreateMAIN*/
		{
			//���� ������ �������
			pPROTOCOL->pProtocolOptions->var_PORT_NUMBER=num;
		}
		else
		{
			//�� ����� ������
			CString str; str.Format(_T("������������� ��� ���� ����� %i �� ������"), num);
			m_pError->SetERROR(str,ERP_ERROR | ER_WRITE_TO_LOG);
			//������ ��������� �� ��������� �� �� ���� �� ������
			pPROTOCOL->pProtocolOptions->var_PORT_NUMBER=PN_NOTINITIALIZED;
			return false;
		}
	}
	
	return true;
}

/*********************    GetIsStarted(void)   **************************************
*		��� ������� ��������� �������� ����, ���� �� � ������ ������ �������� ������
*	�������� ��� ����������� ��������, ������ ������������ ��� � �������, �� ����� ����
*	���������, ��� �������� ����, �.�. ���� ��������� �������� ������
*************************************************************************************/
bool CUSART_Dlg::GetIsStarted(void)
{
	return this->IsStarted;
}
/********************     GetLastGoodReceived(void)    ******************************
*		��� ������� ����������, ����� ���������� ������������ ������� ����� ��� ������
*	��������� �������� �����.
*************************************************************************************/
int	 CUSART_Dlg::GetLastGoodReceived(void)
{
	return this->LastGoodReceived;
}

/********************	GlobalStartTranceiving(void)   ****************************
*		��� ������� ���������� ���� � ���������� ������ ��������, � ��������
*	��������� ������� � �������
***********************************************************************************/
bool CUSART_Dlg::GlobalStartTranceiving(void)
{
	if(!SetEvent(HGlobalStart))
	{
		CString str=_T("���������� ��������� ���������� ����������� ��������");
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
*		������ ������������������ ������� GlobalStartTranceiving
************************************************************************************/
bool CUSART_Dlg::GlobalStopTranceiving(void)
{
	if(!ResetEvent(HGlobalStart))
	{
		CString str=_T("���������� ��������� ���������� ���������� ��������");
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
*		������� ���������� ������� ��������� �����������
**********************************************************************************/
bool CUSART_Dlg::SetTranceiverBusi(void)
{
	if(!ResetEvent(HTranceiverBusi))
	{
		CString str=_T("���������� ��������� ������� ��������� ��������� �����������");
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
*		������� ���������� ������� ������������ �����������
**********************************************************************************/
bool CUSART_Dlg::ClearTranceiverBusi(void)
{
	if(!SetEvent(HTranceiverBusi))
	{
		CString str=_T("���������� �������� ������� ������������ �����������");
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
*		�������, ������� ��������� ��� ���������� ������������ ��������� (���� ����)
*	� ���������� ����.
***********************************************************************************************/
bool CUSART_Dlg::TranseiveMessage(CMessage* pMessage)
{
	if(pMessage==NULL) return false;

	if(pPROTOCOL->pProtocolOptions->var_PORT_NUMBER==PN_NOTINITIALIZED)
	{
		m_pError->SetERROR(_T("���������� ��������� �����. ���������������� ���� �� ���������������."), ERP_WARNING | ER_WRITE_TO_LOG);
		if(pPROTOCOL->pProtocolOptions->NotifyProgram!=NULL)
		{//���� ��������� �� ������� �����������
			pPROTOCOL->pProtocolOptions->NotifyProgram(NP_TRANSFER_ERROR);
		}
		return false;
	}

	//���������� �����
	m_Ports.ClearTXBuff();//������� ����� �����������
	if(pPROTOCOL->pProtocolOptions->var_RS485EN)
	{//�������� ��������� RS485
		m_Ports.SetRTS(true);		//������������ RTS ��� ��������
	}

	if(pMessage->text==NULL || pMessage->length==0)
	{
		m_pError->SetERROR(_T("������ �� ������� ��������� ��� ����"), ERP_ERROR | ER_WRITE_TO_LOG);
		return false;
	}

	//************���������� � ���� ��� ��������*************		
	
	//�������� �����
	if(!m_Ports.WriteToPort((INT8U *)pMessage->text, pMessage->length))
	{
		m_pError->SetERROR(_T("��������� �� ����������"), ERP_ERROR | ER_WRITE_TO_LOG);
		return false;
	}
	
	if(pTranceivingMessage!=NULL)
	{//������ ��� �� ������ ��������� ���������� ���������
		pTranceivingMessage->name=pTranceivingMessage->name+_T("  �������� ��-�� ������� ���������");
		pPROTOCOL->WriteMesFile(pTranceivingMessage, UART_MSG_NOTTRANCEIVED);
	}

	pTranceivingMessage=pMessage;	//�������� ��������� �� ������������ ���������
	
	return true; //�� ������ ���������
}

/***************************************************************
*		������� �������� �� ������� �� �����
****************************************************************/
void CUSART_Dlg::OnOK()
{
	//++++ CDialog::OnOK();
}

/************************    SetMode(INT16U mode)   *********************
*	������� ���������� ����� ������������� ���������
*************************************************************************/
bool CUSART_Dlg::SetMode(INT16U mode)
{
	//�������� ������������ ������
	if(mode==0)
	{
		m_pError->SetERROR(_T("����� ����������� �� ����� ���� �������."), ERP_ERROR | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
		return false;
	}

	if(mode>pPROTOCOL->pProtocolOptions->CompileMessage_x.size()+1)
	{
		m_pError->SetERROR(_T("��������� ��������� �� ����������"), ERP_ERROR | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
		return false;
	}

	//��� �� ������ �� ������ ������� � ���������� ������
	m_ListPacket.SetCurSel(mode-1);	//-1, �.�. � ������ �������� ���������� � ����
	OnVibor();						//����� ��������� �������� � ����
	return true;
}

/******************    GetMode()   ***************************************
*		������� ���������� ����� ������������� ���������.
*	������ ������� ��� �������� �� ������ ���������
*************************************************************************/
INT16U CUSART_Dlg::GetMode()
{
	return pPROTOCOL->Mode;
}

/*********************    ClosePort    ******************************************
*		������� �������� ������������� ������ � ������.
********************************************************************************/
void CUSART_Dlg::ClosePort(void)
{
	//������� �� ������ �������
	m_Ports.ClosePort();
	//��� ��������� ����������������� �����
	pPROTOCOL->pProtocolOptions->var_PORT_NUMBER=PN_NOTINITIALIZED;
}

/****************************	DestroyUSART(CUSART_Dlg* pDlg)	*****************
*		��� ������� ���������� � ������ CUSART_Dlg ��� fri�nd � ������� ����� ������
*	� ��������� �����������
*********************************************************************************/
void DestroyUSART(CUSART_Dlg* pDlg)
{
	//pDlg->SendMessage(WM_CLOSE);
	//pDlg->~CUSART_Dlg();
	delete pDlg;
}


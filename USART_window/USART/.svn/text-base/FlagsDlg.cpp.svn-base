/***************************************************************************************************
****************************************************************************************************
**		В этом файле описываются функции окна настройки связи (не путать с самим окном связи)
**
****************************************************************************************************
*****************************************************************************************************/

/********************** ЛИСТ РЕГИСТРАЦИИ ИЗМЕНЕНИЙ *************************************************
*		11.09.2006 - при отсутствии файла ПС, соответствующая галочка не показывается на форме
****************************************************************************************************/

#include "stdafx.h"
#include "USART_Dlg.h"
#include "FlagsDlg.h"
#include "PROTOCOL_Struct.h"//файл с протоколом


IMPLEMENT_DYNAMIC(CFlagsDlg, CDialog)
CFlagsDlg::CFlagsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFlagsDlg::IDD, pParent)
	, o_verify1(false)
	, o_inMesFile1(_T(""))
	, o_nepr1(false)
	, o_OFF1(false)
	, o_outMesFile1(_T(""))
	, o_HexDisplayOutMessage1(false)
	, o_HexDisplayInMessage1(false)
	, o_LogOutMessage1(0)
	, o_DoHistoryFile1(false)
{
	pUSART_Dlg=(CUSART_Dlg*)pParent;
	
	
}

CFlagsDlg::~CFlagsDlg()
{
}

void CFlagsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFlagsDlg, CDialog)
	ON_BN_CLICKED(IDC_EMUL, OnEmulatioN)
	ON_BN_CLICKED(IDC_VERIFY, OnVerify)
	ON_EN_CHANGE(IDC_EDIT1, OnInMesFile)
	ON_BN_CLICKED(IDC_NEPR, OnNepr)
	ON_BN_CLICKED(IDC_OFF, OnOFF)
	ON_EN_CHANGE(IDC_EDIT2, OnEnChangeOutfile)
	ON_BN_CLICKED(IDC_HEXOUT, OnBnClickedHexout)
	ON_BN_CLICKED(IDC_HEXIN, OnBnClickedHexin)
	ON_BN_CLICKED(IDC_LOGOUT, OnBnClickedLogout)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_CONFIGBUTTON, OnConfigbutton)
	ON_EN_CHANGE(IDC_PERIOD1, OnEnChangePeriod)
	ON_BN_CLICKED(IDC_LOGIN, &CFlagsDlg::OnBnClickedLogin)
	ON_BN_CLICKED(IDC_LOGTIME, &CFlagsDlg::OnBnClickedLogtime)
	ON_EN_CHANGE(IDC_EMUL_SPEED, &CFlagsDlg::OnEnChangeEmulSpeed)
END_MESSAGE_MAP()


// CFlagsDlg message handlers
BOOL CFlagsDlg::OnInitDialog()
{
	TCHAR s_com[50];
	if(pUSART_Dlg->m_Ports.m_hComm!=INVALID_HANDLE_VALUE)
	{
		TCHAR buff;
		if(pUSART_Dlg->m_Ports.GetDCB().Parity==0) buff='N';
		if(pUSART_Dlg->m_Ports.GetDCB().Parity==1) buff='O';
		if(pUSART_Dlg->m_Ports.GetDCB().Parity==2) buff='E';
		if(pUSART_Dlg->m_Ports.GetDCB().Parity==3) buff='M';
		if(pUSART_Dlg->m_Ports.GetDCB().Parity==4) buff='S';

		wsprintf(s_com,_T("COM%d %d %c %d %d"),
				pUSART_Dlg->m_Ports.m_nPortNr, 
				pUSART_Dlg->m_Ports.GetDCB().BaudRate, 
				buff,
				pUSART_Dlg->m_Ports.GetDCB().ByteSize,
				pUSART_Dlg->m_Ports.GetDCB().StopBits
				);
		GetDlgItem(IDC_INFO_COM)->SetWindowText(s_com);
	}
	else
	{
		GetDlgItem(IDC_INFO_COM)->SetWindowText(_T("NO COM PORTS WERE FOUNDED!"));
	}
	//выставим элементы в соответствии настройкам
	o_emulation1=pUSART_Dlg->pPROTOCOL->pProtocolOptions->o_emulation;
	((CButton*)GetDlgItem(IDC_EMUL))->SetCheck(o_emulation1);
	this->OnEmulatioN();	//для правильного отображения элементов на окне

	o_verify1=pUSART_Dlg->pPROTOCOL->pProtocolOptions->o_verify;
	((CButton*)GetDlgItem(IDC_VERIFY))->SetCheck(o_verify1);
	
	o_nepr1=pUSART_Dlg->pPROTOCOL->pProtocolOptions->o_nepr;
	((CButton*)GetDlgItem(IDC_NEPR))->SetCheck(o_nepr1);
	if(!o_nepr1)
	{//погасим возможность задания периода передачи
		GetDlgItem(IDC_PERIOD_LABEL)->EnableWindow(false);
		GetDlgItem(IDC_PERIOD1)->EnableWindow(false);
	}

	o_OFF1=pUSART_Dlg->pPROTOCOL->pProtocolOptions->o_OFF;
	((CButton*)GetDlgItem(IDC_OFF))->SetCheck(o_OFF1);
	if(!o_OFF1)
	{
		//погасим ввод имени файла
		((CEdit*)GetDlgItem(IDC_EDIT2))->EnableWindow(false);
	}

	o_inMesFile1=pUSART_Dlg->pPROTOCOL->pProtocolOptions->o_inMesFile;
	SetDlgItemText(IDC_EDIT1,o_inMesFile1);

	o_outMesFile1=pUSART_Dlg->pPROTOCOL->pProtocolOptions->o_outMesFile;
	SetDlgItemText(IDC_EDIT2,o_outMesFile1);

	o_HexDisplayOutMessage1=pUSART_Dlg->pPROTOCOL->pProtocolOptions->o_HexDisplayOutMessage;
	((CButton*)GetDlgItem(IDC_HEXOUT))->SetCheck(o_HexDisplayOutMessage1);

	o_HexDisplayInMessage1=pUSART_Dlg->pPROTOCOL->pProtocolOptions->o_HexDisplayInMessage;
	((CButton*)GetDlgItem(IDC_HEXIN))->SetCheck(o_HexDisplayInMessage1);

	o_LogOutMessage1=pUSART_Dlg->pPROTOCOL->pProtocolOptions->o_LogOutMessage;
	((CButton*)GetDlgItem(IDC_LOGOUT))->SetCheck(o_LogOutMessage1);

	o_LogTimeMessages1=pUSART_Dlg->pPROTOCOL->pProtocolOptions->o_LogOutMessage;
	((CButton*)GetDlgItem(IDC_LOGTIME))->SetCheck(o_LogTimeMessages1);

	o_LogInMessage1=pUSART_Dlg->pPROTOCOL->pProtocolOptions->o_LogInMessage;
	((CButton*)GetDlgItem(IDC_LOGIN))->SetCheck(o_LogInMessage1);

	o_EmulSpeed1=pUSART_Dlg->pPROTOCOL->pProtocolOptions->o_EmulSpeed;

	CString str;
	str.Format(_T("%u"),o_EmulSpeed1);
	((CEdit*)GetDlgItem(IDC_EMUL_SPEED))->SetWindowText(str);

	o_Period1=pUSART_Dlg->pPROTOCOL->pProtocolOptions->o_Period;
	
	str.Format(_T("%u"),o_Period1);
	((CEdit*)GetDlgItem(IDC_PERIOD1))->SetWindowText(str);

	return CDialog::OnInitDialog();
}

void CFlagsDlg::OnEmulatioN()
{
	if(((CButton*)GetDlgItem(IDC_EMUL))->GetCheck()==BST_CHECKED)
	{
		o_emulation1=true;
	}
	else
	{
		o_emulation1=false;
	}

	if(!o_emulation1)
	{
		((CStatic *) GetDlgItem(IDC_EMUL_SPEED_LABEL))->EnableWindow(false);
		((CEdit*)GetDlgItem(IDC_EMUL_SPEED))->EnableWindow(false);
	}
	else
	{
		((CStatic *) GetDlgItem(IDC_EMUL_SPEED_LABEL))->EnableWindow(true);
		((CEdit*)GetDlgItem(IDC_EMUL_SPEED))->EnableWindow(true);
	}
}

void CFlagsDlg::OnVerify()
{
	if(((CButton*)GetDlgItem(IDC_VERIFY))->GetCheck()==BST_CHECKED)
	{
		o_verify1=true;
	}
	else
	{
		o_verify1=false;
	}
}

void CFlagsDlg::OnInMesFile()
{
	CString str;
	GetDlgItemText(IDC_EDIT1,str);
	o_inMesFile1=str;
}

void CFlagsDlg::OnNepr()
{
	if(((CButton*)GetDlgItem(IDC_NEPR))->GetCheck()==BST_CHECKED)
	{
		o_nepr1=true;
	}
	else
	{
		o_nepr1=false;
	}

	if(!o_nepr1)
	{
		GetDlgItem(IDC_PERIOD_LABEL)->EnableWindow(false);
		GetDlgItem(IDC_PERIOD1)->EnableWindow(false);
	}
	else
	{
		GetDlgItem(IDC_PERIOD_LABEL)->EnableWindow(true);
		GetDlgItem(IDC_PERIOD1)->EnableWindow(true);
	}
}

void CFlagsDlg::OnOFF()
{
	if(((CButton*)GetDlgItem(IDC_OFF))->GetCheck()==BST_CHECKED)
	{
		o_OFF1=true;
	}
	else
	{
		o_OFF1=false;
	}

	if(o_OFF1)
	{
		//подсветим ввод имени файла
		((CEdit*)GetDlgItem(IDC_EDIT2))->EnableWindow(true);
	}
	else
	{
		//погасим ввод имени файла
		((CEdit*)GetDlgItem(IDC_EDIT2))->EnableWindow(false);
	}
}

void CFlagsDlg::OnEnChangeOutfile()
{
	CString str;
	GetDlgItemText(IDC_EDIT2,str);
	o_outMesFile1=str;
}

void CFlagsDlg::OnBnClickedHexout()
{
	if(((CButton*)GetDlgItem(IDC_HEXOUT))->GetCheck()==BST_CHECKED)
		o_HexDisplayOutMessage1=true;
	else
		o_HexDisplayOutMessage1=false;
}

void CFlagsDlg::OnBnClickedHexin()
{
	if(((CButton*)GetDlgItem(IDC_HEXIN))->GetCheck()==BST_CHECKED)
		o_HexDisplayInMessage1=true;
	else
		o_HexDisplayInMessage1=false;
}

void CFlagsDlg::OnBnClickedLogout()
{
	if(((CButton*)GetDlgItem(IDC_LOGOUT))->GetCheck()==BST_CHECKED)
		o_LogOutMessage1=true;
	else
		o_LogOutMessage1=false;
}

void CFlagsDlg::OnBnClickedLogin()
{
	if(((CButton*)GetDlgItem(IDC_LOGIN))->GetCheck()==BST_CHECKED)
		o_LogInMessage1=true;
	else
		o_LogInMessage1=false;
}

void CFlagsDlg::OnBnClickedLogtime()
{
	if(((CButton*)GetDlgItem(IDC_LOGTIME))->GetCheck()==BST_CHECKED)
		o_LogTimeMessages1=true;
	else
		o_LogTimeMessages1=false;
}

/******************   OnConfigbutton() *******************************************
*		Нажали на кнопку "Настройка", чтобы изменить параметры порта
**********************************************************************************/
void CFlagsDlg::OnConfigbutton() 
{
	CConfigDlg* dlg = new CConfigDlg(this, pUSART_Dlg->m_Ports.GetDCB(), pUSART_Dlg->pPROTOCOL->pProtocolOptions->var_PORT_NUMBER);
	
	if (dlg->DoModal() == IDOK)
	{
		pUSART_Dlg->pPROTOCOL->pProtocolOptions->var_TRANSFER_SPEED=_ttoi(dlg->m_strBaudRate);	//скорость передачи
		pUSART_Dlg->pPROTOCOL->pProtocolOptions->var_PARITY=dlg->m_Parity;						//контроль четности
		pUSART_Dlg->pPROTOCOL->pProtocolOptions->var_BITS_PER_PACKET=_ttoi(dlg->m_strDataBits);	//битность
		pUSART_Dlg->pPROTOCOL->pProtocolOptions->var_STOP_BITS=_ttoi(dlg->m_strStopBits);
			
		if(pUSART_Dlg->ReSetCOMport(dlg->m_PortNum)!=true)
		{
			pUSART_Dlg->m_pError->SetERROR(_T("При смене настроек порта произошла ошибка"),ERP_ERROR | ER_SHOW_MESSAGE | ER_WRITE_TO_LOG);
		}
		else
		{
			pUSART_Dlg->m_pError->SetERROR(_T("Настройки порта изменены успешно"),ERP_GOOD | ER_WRITE_TO_LOG);
		}
	}
	else 
	{
		dlg->DestroyWindow();
		delete dlg;
		return;
	}

	TCHAR s_com[50];
	wsprintf(s_com,_T("COM%d %d %c %d %d"),
		pUSART_Dlg->pPROTOCOL->pProtocolOptions->var_PORT_NUMBER, 
		_ttoi(dlg->m_strBaudRate), 
		dlg->m_Parity,
		_ttoi(dlg->m_strDataBits),
		_ttoi(dlg->m_strStopBits)
		);
	GetDlgItem(IDC_INFO_COM)->SetWindowText(s_com);

	dlg->DestroyWindow();
	delete dlg;
}

/*********************    OnEnChangePeriod   ************************************
*		Меняем период отправки сообщений при непрерывной отправки
********************************************************************************/
void CFlagsDlg::OnEnChangePeriod()
{
	UINT temp=GetDlgItemInt(IDC_PERIOD1,NULL,FALSE);
	if(temp>10 && temp<10000)
	{//учту только допустимые значения
		o_Period1=temp;
	}
}

/**************************    OnEnChangeEmulSpeed   ****************************
*		Меняем период между символами при эмуляции
********************************************************************************/
void CFlagsDlg::OnEnChangeEmulSpeed()
{
	UINT temp=GetDlgItemInt(IDC_EMUL_SPEED,NULL,FALSE);
	if(temp>10 && temp<10000)
	{//учту только допустимые значения
		o_EmulSpeed1=temp;
	}
}

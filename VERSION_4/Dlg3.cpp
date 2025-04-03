// Dlg3.cpp : implementation file
//

#include "stdafx.h"
#include "Dlg3.h"
#include "VERSION_4Dlg.h"


// CDlg3 dialog

IMPLEMENT_DYNAMIC(CDlg3, CDialog)

CDlg3::~CDlg3()
{
}

void CDlg3::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, edit1);
	DDX_Control(pDX, IDC_EDIT2, edit2);
	DDX_Control(pDX, IDC_EDIT3, edit3);
	DDX_Control(pDX, IDC_EDIT4, edit4);
	DDX_Control(pDX, IDC_EDIT5, edit5);
	DDX_Control(pDX, IDC_EDIT6, edit6);
	DDX_Control(pDX, IDC_EDIT7, edit7);
	DDX_Control(pDX, IDC_EDIT8, edit8);
	
}


BEGIN_MESSAGE_MAP(CDlg3, CDialog)

	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// ������������� ��� ������
BOOL CDlg3::Create(UINT ID, CVERSION_4Dlg* pParentWnd)
{
	MAIN_WINDOW=pParentWnd;	//�������� ��������� �� ��������

	return CDialog::Create(ID, pParentWnd);
}

BOOL CDlg3::OnInitDialog()
{
	CDialog::OnInitDialog();
	/********	�������� �������� �������	*******************************/
	static CFont m_Font;
	m_Font.DeleteObject();  //�� ������ ������
	m_Font.CreateFont		/*�������� ����� ��������*/
		(
			-10,                   // nHeight
			0,                         // nWidth
			0,                         // nEscapement
			0,                         // nOrientation
			FW_NORMAL,                 // nWeight
			FALSE,                     // bItalic
			FALSE,                     // bUnderline
			0,                         // cStrikeOut
			DEFAULT_CHARSET,           // nCharSet
			OUT_DEFAULT_PRECIS,        // nOutPrecision
			CLIP_DEFAULT_PRECIS,       // nClipPrecision
			DEFAULT_QUALITY,           // nQuality
			DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
			_T("Arial"));                 // lpszFacename

	/*GetDlgItem(IDC_EDIT5)->SetFont(&m_Font,true);
	GetDlgItem(IDC_EDIT6)->SetFont(&m_Font,true);
	GetDlgItem(IDC_EDIT7)->SetFont(&m_Font,true);
	GetDlgItem(IDC_EDIT8)->SetFont(&m_Font,true);*/
	
	static CFont m_Font2;
	m_Font.DeleteObject();  //�� ������ ������
	m_Font.CreateFont		/*�������� ����� ��������*/
		(
			-14,                   // nHeight
			0,                         // nWidth
			0,                         // nEscapement
			0,                         // nOrientation
			FW_BOLD,                 // nWeight
			FALSE,                     // bItalic
			FALSE,                     // bUnderline
			0,                         // cStrikeOut
			DEFAULT_CHARSET,           // nCharSet
			OUT_DEFAULT_PRECIS,        // nOutPrecision
			CLIP_DEFAULT_PRECIS,       // nClipPrecision
			DEFAULT_QUALITY,           // nQuality
			DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
			_T("Times New Roman"));                 // lpszFacename

	GetDlgItem(IDC_EDIT1)->SetFont(&m_Font,true);
	GetDlgItem(IDC_EDIT2)->SetFont(&m_Font,true);
	GetDlgItem(IDC_EDIT3)->SetFont(&m_Font,true);
	GetDlgItem(IDC_EDIT4)->SetFont(&m_Font,true);

	/*��������� ��������� �������*/
	GetDlgItem(IDC_EDIT5)->SetWindowText(_T("  ������������ �� �������� ����� "));
	GetDlgItem(IDC_EDIT6)->SetWindowText(_T("  ������ ��������� �� �������� ����� "));
	GetDlgItem(IDC_EDIT7)->SetWindowText(_T("  ������ �� ���������� �������� "));
	GetDlgItem(IDC_EDIT8)->SetWindowText(_T("  ����������� �� ������� "));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CDlg3::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	/*
	CTLCOLOR_BTN    	������
	CTLCOLOR_DLG    	������
	CTLCOLOR_EDIT   	������� ��������������
	CTLCOLOR_LISTBOX 	������  
	CTLCOLOR_MSGBOX   	���� ���������
	CTLCOLOR_SCROLLBAR  ��������
	CTLCOLOR_STATIC   	����������� �����
	*/
	#define RedColor RGB(251,91,68)	//����������� ���� ������� �������������� ����������� �������
	
	if (CTLCOLOR_EDIT == nCtlColor)
	{
		/*��� ������� �������� ������*/
		UINT id = pWnd->GetDlgCtrlID();
		if (id == IDC_EDIT1)
		{/*������������ �� �������� �����*/
			//����� ��������������� �� ������, ����� ���� ������������, �� � ����� ���� ������
//			if(MAIN_WINDOW->GLAVNIY_CLASS.var_Oil_Prot==0x40 ||MAIN_WINDOW->GLAVNIY_CLASS.var_Oil_Prot==0xC0 || MAIN_WINDOW->GLAVNIY_CLASS.var_Oil_Prot==0x80)
			if(MAIN_WINDOW->GLAVNIY_CLASS.var_Oil_Prot & 0xC0)
			{//���� ������������
				pDC->SetTextColor			(RGB(000,000,000));	//���� ������
				pDC->SetBkColor				(RGB(255,255,000));	//���� �� ������� 
				SetDCBrushColor(pDC->m_hDC,	 RGB(255,255,000));	//���� �������
			}
			else if(MAIN_WINDOW->GLAVNIY_CLASS.var_Oil_Prot & 0x10)
			{//��������� ����� ����� ������� ��������� �����
				pDC->SetTextColor			(RGB(000,000,000));	//���� ������
				pDC->SetBkColor				(RGB(200,200,000));	//���� �� ������� 
				SetDCBrushColor(pDC->m_hDC,	 RGB(200,200,000));	//���� �������
			}
			else
			{//��� ������������
				pDC->SetTextColor			(RGB(000,000,000));	//���� ������
				pDC->SetBkColor				(RGB(070,198,240));	//���� �� ������� 
				SetDCBrushColor(pDC->m_hDC,	 RGB(070,198,240));	//���� �������
			}
			return (HBRUSH)::GetStockObject(DC_BRUSH);		//����������� ��� ���������
		
		}
		if ( id == IDC_EDIT2)
		{/*������ �� �������� �����*/
			
			if(MAIN_WINDOW->GLAVNIY_CLASS.var_Oil_Prot==0xC0 || MAIN_WINDOW->GLAVNIY_CLASS.var_Oil_Prot==0x80)
			{//���� ������
				pDC->SetTextColor			(RGB(000,000,000));	//���� ������
				pDC->SetBkColor				(RedColor);	//���� �� ������� 
				SetDCBrushColor(pDC->m_hDC,	 RedColor);	//���� �������
			}
			else if(MAIN_WINDOW->GLAVNIY_CLASS.var_Oil_Prot & 0x20)
			{//��������� ����� ������� �������� �����
				pDC->SetTextColor			(RGB(000,000,000));	//���� ������
				pDC->SetBkColor				(RGB(200,200,000));	//���� �� ������� 
				SetDCBrushColor(pDC->m_hDC,	 RGB(200,200,000));	//���� �������
			}
			else
			{//��� ������
				pDC->SetTextColor			(RGB(000,000,000));	//���� ������
				pDC->SetBkColor				(RGB(070,198,240));	//���� �� ������� 
				SetDCBrushColor(pDC->m_hDC,	 RGB(070,198,240));	//���� �������
			}
			return (HBRUSH)::GetStockObject(DC_BRUSH);		//����������� ��� ���������
		}
		if ( id == IDC_EDIT3)
		{/*������ �� ���������� ��������*/
			static bool flag=false;		//��������� ����

			if(MAIN_WINDOW->GLAVNIY_CLASS.var_Raznos)
			{//���� ������ �� ���������� ��������
				pDC->SetTextColor			(RGB(000,000,000));	//���� ������
				pDC->SetBkColor				(RedColor);	//���� �� ������� 
				SetDCBrushColor(pDC->m_hDC,	 RedColor);	//���� �������
			}
			else
			{//��� ������ �� ���������� ��������
				pDC->SetTextColor			(RGB(000,000,000));	//���� ������
				pDC->SetBkColor				(RGB(070,198,240));	//���� �� ������� 
				SetDCBrushColor(pDC->m_hDC,	 RGB(070,198,240));	//���� �������
			}
			return (HBRUSH)::GetStockObject(DC_BRUSH);		//����������� ��� ���������
		}
		
		if ( id == IDC_EDIT4)
		{/*����������� �� �������*/
			if(MAIN_WINDOW->GLAVNIY_CLASS.var_Nadduv_Limit==0x00)
			{//��� ������ �� �������
				pDC->SetTextColor			(RGB(000,000,000));	//���� ������
				pDC->SetBkColor				(RGB(070,198,240));	//���� �� ������� 
				SetDCBrushColor(pDC->m_hDC,	 RGB(070,198,240));	//���� �������
			}
			else if(MAIN_WINDOW->GLAVNIY_CLASS.var_Nadduv_Limit==0xFE)
			{//���� ������
				pDC->SetTextColor			(RGB(000,000,000));	//���� ������
				pDC->SetBkColor				(RedColor);	//���� �� ������� 
				SetDCBrushColor(pDC->m_hDC,	 RedColor);	//���� �������
			}
			else if(MAIN_WINDOW->GLAVNIY_CLASS.var_Nadduv_Limit==0xEE)
			{//����� �� ����� ������ �������� �������
				pDC->SetTextColor			(RGB(000,000,000));	//���� ������
				pDC->SetBkColor				(RGB(200,200,000));	//���� �� ������� 
				SetDCBrushColor(pDC->m_hDC,	 RGB(200,200,000));	//���� �������
			}
			return (HBRUSH)::GetStockObject(DC_BRUSH);		//����������� ��� ���������
		}

		/*��� ������� �������� ������*/
		if (id == IDC_EDIT5)
		{/*������������ �� �������� �����*/
			if(MAIN_WINDOW->GLAVNIY_CLASS.var_Oil_Prot & 0x40)
			{//���� ������ �� ���������� ��������
				pDC->SetTextColor			(RGB(000,000,000));	//���� ������
				pDC->SetBkColor				(RedColor);	//���� �� ������� 
				SetDCBrushColor(pDC->m_hDC,	 RedColor);	//���� �������
			}
			else if(MAIN_WINDOW->GLAVNIY_CLASS.var_Oil_Prot & 0x10)
			{//��������� ����� ������� ��������� �����
				pDC->SetTextColor			(RGB(000,000,000));	//���� ������
				pDC->SetBkColor				(RedColor);	//���� �� ������� 
				SetDCBrushColor(pDC->m_hDC,	 RedColor);	//���� �������
			}
			else
			{//��� ������
				pDC->SetTextColor			(RGB(000,000,000));	//���� ������
				pDC->SetBkColor				(RGB(146,228,174));	//���� �� ������� 
				SetDCBrushColor(pDC->m_hDC,	 RGB(146,228,174));	//���� �������
			}
			return (HBRUSH)::GetStockObject(DC_BRUSH);		//����������� ��� ���������
		}
		if ( id == IDC_EDIT6)
		{/*������ �� �������� �����*/
//			if(MAIN_WINDOW->GLAVNIY_CLASS.var_Oil_Prot==0xC0 || MAIN_WINDOW->GLAVNIY_CLASS.var_Oil_Prot==0x80)
			if(MAIN_WINDOW->GLAVNIY_CLASS.var_Oil_Prot & 0xC0)
			{//���� ������ �� ���������� ��������
				pDC->SetTextColor			(RGB(000,000,000));	//���� ������
				pDC->SetBkColor				(RedColor);	//���� �� ������� 
				SetDCBrushColor(pDC->m_hDC,	 RedColor);	//���� �������
			}
			else if(MAIN_WINDOW->GLAVNIY_CLASS.var_Oil_Prot & 0x20)
			{//��������� ����� ������� �������� �����
				pDC->SetTextColor			(RGB(000,000,000));	//���� ������
				pDC->SetBkColor				(RedColor);	//���� �� ������� 
				SetDCBrushColor(pDC->m_hDC,	 RedColor);	//���� �������
			}
			else
			{//��� ������
				pDC->SetTextColor			(RGB(000,000,000));	//���� ������
				pDC->SetBkColor				(RGB(146,228,174));	//���� �� ������� 
				SetDCBrushColor(pDC->m_hDC,	 RGB(146,228,174));	//���� �������
			}
			return (HBRUSH)::GetStockObject(DC_BRUSH);		//����������� ��� ���������
		}
		if ( id == IDC_EDIT7)
		{/*������ �� ���������� ��������*/
			if(MAIN_WINDOW->GLAVNIY_CLASS.var_Raznos)
			{//���� ������ �� ���������� ��������
				pDC->SetTextColor			(RGB(000,000,000));	//���� ������
				pDC->SetBkColor				(RedColor);	//���� �� ������� 
				SetDCBrushColor(pDC->m_hDC,	 RedColor);	//���� �������
			}
			else
			{//��� ������
				pDC->SetTextColor			(RGB(000,000,000));	//���� ������
				pDC->SetBkColor				(RGB(146,228,174));	//���� �� ������� 
				SetDCBrushColor(pDC->m_hDC,	 RGB(146,228,174));	//���� �������
			}
			return (HBRUSH)::GetStockObject(DC_BRUSH);		//����������� ��� ���������
		}
		if ( id == IDC_EDIT8)
		{/*����������� �� �������*/
			if(MAIN_WINDOW->GLAVNIY_CLASS.var_Nadduv_Limit==0xFE || MAIN_WINDOW->GLAVNIY_CLASS.var_Nadduv_Limit==0xEE)
			{//���� ������ �� ���������� ��������, ��� �������� ������
				pDC->SetTextColor			(RGB(000,000,000));	//���� ������
				pDC->SetBkColor				(RedColor);	//���� �� ������� 
				SetDCBrushColor(pDC->m_hDC,	 RedColor);	//���� �������
			}
			else
			{//��� ������� ������� � ���� ������
				pDC->SetTextColor			(RGB(000,000,000));	//���� ������
				pDC->SetBkColor				(RGB(146,228,174));	//���� �� ������� 
				SetDCBrushColor(pDC->m_hDC,	 RGB(146,228,174));	//���� �������
			}
			return (HBRUSH)::GetStockObject(DC_BRUSH);		//����������� ��� ���������
		}
	}

	if (CTLCOLOR_STATIC == nCtlColor)
	{
		UINT id = pWnd->GetDlgCtrlID();
		
	}

	if (CTLCOLOR_DLG == nCtlColor)
	{
		SetDCBrushColor(pDC->m_hDC,RGB(5,196,211));	//���� ������� �����
		return (HBRUSH)::GetStockObject(DC_BRUSH);		//����������� ��� ���������
	}

 //   pDC->SetBkColor(RGB(127,127,127));


	return hbr;
}

/***************************	PreTranslateMessage(MSG *pMsg)	***********************************
*	����� � ��������� ��� ������� ������, ����� �������� ������� �������
***************************************************************************************************/
BOOL CDlg3::PreTranslateMessage(MSG *pMsg)
{
	int k=0;
	switch(pMsg->message)
	{
	case WM_KEYDOWN:
		switch(pMsg->wParam)
		{
			//��������� ��������� ����� �� �����, ����, � ���� ��������� � �����,
			//������� �������� ��� �������� ����
			case VK_ESCAPE:
			case VK_RETURN:
			case '1':
			case VK_NUMPAD1:
			case '2':
			case VK_NUMPAD2:
			case '3':
			case VK_NUMPAD3:
			case '4':
			case VK_NUMPAD4:
			case '5':
			case VK_NUMPAD5:
			case '6':
			case VK_NUMPAD6:
			case '7':
			case VK_NUMPAD7:
			case '8':
			case VK_NUMPAD8:
			case '9':
			case VK_NUMPAD9:
			case '0':
			case VK_NUMPAD0:
			case 'W':
			case 'S':
			case 'A':
			case 'D':
		
			return MAIN_WINDOW->PreTranslateMessage(pMsg);
			break;
		}
		break;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}


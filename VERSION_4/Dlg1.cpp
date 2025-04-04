// Dlg1.cpp : implementation file
//

#include "stdafx.h"
#include "Dlg1.h"
#include "VERSION_4Dlg.h"

IMPLEMENT_DYNAMIC(CDlg1, CDialog)
CDlg1::~CDlg1()
{
	
}

void CDlg1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_RPM,		m_UST_Freq_Rot);
	DDX_Control(pDX, IDC_STOP,		m_UST_Stop);
	DDX_Control(pDX, IDC_RUN,		m_UST_Run);
	DDX_Control(pDX, IDC_HOLOST,	m_UST_Holost);
	DDX_Control(pDX, IDC_NAGR,		m_UST_Nagr);
	DDX_Control(pDX, IDC_POSITION,	m_UST_Position);
	DDX_Control(pDX, IDC_REYKA_EDIT,	m_UST_Reyka);
	DDX_Control(pDX, IDC_CHECK_REYKA,		m_UST_Reyka_Set);
	DDX_Control(pDX, IDC_CHECK_BUX,		m_UST_Bux);
}


BEGIN_MESSAGE_MAP(CDlg1, CDialog)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_RPM, OnEnChangeUST_Freq_Rot)
	ON_BN_CLICKED(IDC_RADIO1, OnBnClickedPosition_1)
	ON_BN_CLICKED(IDC_RADIO2, OnBnClickedPosition_2)
	ON_BN_CLICKED(IDC_RADIO3, OnBnClickedPosition_3)
	ON_BN_CLICKED(IDC_RADIO4, OnBnClickedPosition_4)
	ON_BN_CLICKED(IDC_RADIO5, OnBnClickedPosition_5)
	ON_BN_CLICKED(IDC_RADIO6, OnBnClickedPosition_6)
	ON_BN_CLICKED(IDC_RADIO7, OnBnClickedPosition_7)
	ON_BN_CLICKED(IDC_RADIO8, OnBnClickedPosition_8)
	ON_BN_CLICKED(IDC_RADIO9, OnBnClickedPosition_9)
	ON_BN_CLICKED(IDC_RADIO10, OnBnClickedPosition_10)
	ON_BN_CLICKED(IDC_RADIO11, OnBnClickedPosition_11)
	ON_BN_CLICKED(IDC_RADIO12, OnBnClickedPosition_12)
	ON_BN_CLICKED(IDC_RADIO13, OnBnClickedPosition_13)
	ON_BN_CLICKED(IDC_RADIO14, OnBnClickedPosition_14)
	ON_BN_CLICKED(IDC_RADIO15, OnBnClickedPosition_15)

	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_BN_CLICKED(IDC_RUN,  OnRun)

	ON_BN_CLICKED(IDC_NAGR,  OnNagr)
	ON_BN_CLICKED(IDC_HOLOST,OnHolost)

	ON_BN_CLICKED(IDC_CHECK_REYKA, OnReykaSet)

	ON_BN_CLICKED(IDC_CHECK_BUX, OnBnClickedBux)

	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

// ������������� ��� ������
BOOL CDlg1::Create(UINT ID, CVERSION_4Dlg* pParentWnd)
{
	MAIN_WINDOW=pParentWnd;	//�������� ��������� �� ��������

	return CDialog::Create(ID, pParentWnd);
}

BOOL CDlg1::OnInitDialog()
{
	CDialog::OnInitDialog();
	/********	�������� �������� �������	*******************************/
	static CFont m_Font;
	m_Font.DeleteObject();  //�� ������ ������
	m_Font.CreateFont		/*�������� ����� ��������*/
		(
			-20,                   // nHeight
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
			_T("Times New Roman"));                 // lpszFacename

	GetDlgItem(IDC_STATIC1)->SetFont(&m_Font,true);
	GetDlgItem(IDC_STATIC2)->SetFont(&m_Font,true);

	static CFont m_Font2;
	m_Font.DeleteObject();  //�� ������ ������
	m_Font.CreateFont		/*�������� ����� ��������*/
		(
			-22,                   // nHeight
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

	m_UST_Position.SetFont(&m_Font,true);
	m_UST_Freq_Rot.SetFont(&m_Font,true);
	m_UST_Reyka.SetFont(&m_Font,true);

	m_bReyka_Edit = false;

	/*
	� ������� ��������� ������� �� ��� ��� ������, �.�. ������� ����� ������������� 
	����������� ������ ������� �� ��������, ������� ����� ����������� ��������� ��-
	����� ��� ������ �������
	*/
	//��������� ����� 1 �� ��������� ��������� ������� � ���������� ���������� ��������
	m_UST_Position.SetWindowText(_T("1"));
	//� ������ ������ �� ���� �������
	((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(BST_CHECKED);


	((CButton *)GetDlgItem(IDC_RADIO1))->ModifyStyle(0,BS_OWNERDRAW);
	((CButton *)GetDlgItem(IDC_RADIO2))->ModifyStyle(0,BS_OWNERDRAW);
	((CButton *)GetDlgItem(IDC_RADIO3))->ModifyStyle(0,BS_OWNERDRAW);
	((CButton *)GetDlgItem(IDC_RADIO4))->ModifyStyle(0,BS_OWNERDRAW);
	((CButton *)GetDlgItem(IDC_RADIO5))->ModifyStyle(0,BS_OWNERDRAW);
	((CButton *)GetDlgItem(IDC_RADIO6))->ModifyStyle(0,BS_OWNERDRAW);
	((CButton *)GetDlgItem(IDC_RADIO7))->ModifyStyle(0,BS_OWNERDRAW);
	((CButton *)GetDlgItem(IDC_RADIO8))->ModifyStyle(0,BS_OWNERDRAW);
	((CButton *)GetDlgItem(IDC_RADIO9))->ModifyStyle(0,BS_OWNERDRAW);
	((CButton *)GetDlgItem(IDC_RADIO10))->ModifyStyle(0,BS_OWNERDRAW);
	((CButton *)GetDlgItem(IDC_RADIO11))->ModifyStyle(0,BS_OWNERDRAW);
	((CButton *)GetDlgItem(IDC_RADIO12))->ModifyStyle(0,BS_OWNERDRAW);
	((CButton *)GetDlgItem(IDC_RADIO13))->ModifyStyle(0,BS_OWNERDRAW);
	((CButton *)GetDlgItem(IDC_RADIO14))->ModifyStyle(0,BS_OWNERDRAW);
	((CButton *)GetDlgItem(IDC_RADIO15))->ModifyStyle(0,BS_OWNERDRAW);
	
	r1.Attach(IDC_RADIO1, this, BGColorEn, RGB(0, 0, 0), RGB(255, 0, 255), 2);
	r2.Attach(IDC_RADIO2, this, BGColorDis, RGB(0, 0, 0), RGB(255, 0, 255), 2);
	r3.Attach(IDC_RADIO3, this, BGColorDis, RGB(0, 0, 0), RGB(255, 0, 255), 2);
	r4.Attach(IDC_RADIO4, this, BGColorDis, RGB(0, 0, 0), RGB(255, 0, 255), 2);
	r5.Attach(IDC_RADIO5, this, BGColorDis, RGB(0, 0, 0), RGB(255, 0, 255), 2);
	r6.Attach(IDC_RADIO6, this, BGColorDis, RGB(0, 0, 0), RGB(255, 0, 255), 2);
	r7.Attach(IDC_RADIO7, this, BGColorDis, RGB(0, 0, 0), RGB(255, 0, 255), 2);
	r8.Attach(IDC_RADIO8, this, BGColorDis, RGB(0, 0, 0), RGB(255, 0, 255), 2);
	r9.Attach(IDC_RADIO9, this, BGColorDis, RGB(0, 0, 0), RGB(255, 0, 255), 2);
	r10.Attach(IDC_RADIO10, this, BGColorDis, RGB(0, 0, 0), RGB(255, 0, 255), 2);
	r11.Attach(IDC_RADIO11, this, BGColorDis, RGB(0, 0, 0), RGB(255, 0, 255), 2);
	r12.Attach(IDC_RADIO12, this, BGColorDis, RGB(0, 0, 0), RGB(255, 0, 255), 2);
	r13.Attach(IDC_RADIO13, this, BGColorDis, RGB(0, 0, 0), RGB(255, 0, 255), 2);
	r14.Attach(IDC_RADIO14, this, BGColorDis, RGB(0, 0, 0), RGB(255, 0, 255), 2);
	r15.Attach(IDC_RADIO15, this, BGColorDis, RGB(0, 0, 0), RGB(255, 0, 255), 2);
	prev_regime=&r1;	//�� ��������� ������

	//�������� ������ �����
	m_UST_Run.ModifyStyle(0,BS_OWNERDRAW);
	m_UST_Run.SetBGColor(BGColorDis);
	m_UST_Run.SetFGColor(RGB(0, 0, 0));
	m_UST_Run.SetDisabledColor(RGB(255, 0, 255));
	m_UST_Run.SetBevel(2);

	//�������� ������ �����
	m_UST_Stop.ModifyStyle(0,BS_OWNERDRAW);
	m_UST_Stop.SetBGColor(BGColorEn);
	m_UST_Stop.SetFGColor(RGB(0, 0, 0));
	m_UST_Stop.SetDisabledColor(RGB(255, 0, 255));
	m_UST_Stop.SetBevel(2);

	//�������� ������ ��������� ����
	m_UST_Holost.ModifyStyle(0,BS_OWNERDRAW);
	m_UST_Holost.SetBGColor(BGColorEn);
	m_UST_Holost.SetFGColor(RGB(0, 0, 0));
	m_UST_Holost.SetDisabledColor(RGB(255, 0, 255));
	m_UST_Holost.SetBevel(2);

	//�������� ������ ��������� �������� ������
	m_UST_Nagr.ModifyStyle(0,BS_OWNERDRAW);
	m_UST_Nagr.SetBGColor(BGColorDis);
	m_UST_Nagr.SetFGColor(RGB(0, 0, 0));
	m_UST_Nagr.SetDisabledColor(RGB(255, 0, 255));
	m_UST_Nagr.SetBevel(2);

	//�������� ������ ������� �����
	m_UST_Reyka_Set.ModifyStyle(0,BS_OWNERDRAW);
	m_UST_Reyka_Set.SetBGColor(BGColorDis);
	m_UST_Reyka_Set.SetFGColor(RGB(0, 0, 0));
	m_UST_Reyka_Set.SetDisabledColor(RGB(255, 0, 255));
	m_UST_Reyka_Set.SetBevel(2);

	// ������� ��������� �����
	m_UST_Reyka.SetWindowText(_T("0.00"));
	m_UST_Reyka.SendMessage(EM_SETREADONLY, (WPARAM)TRUE, 0);
	m_UST_Reyka_Set.SetBGColor(BGColorDis);

	//�������� ������ ����������
	// ����������
	m_UST_Bux.ModifyStyle(0,BS_OWNERDRAW);
	m_UST_Bux.SetBGColor(BGColorDis);
	m_UST_Bux.SetFGColor(RGB(0, 0, 0));
	m_UST_Bux.SetDisabledColor(RGB(255, 0, 255));
	m_UST_Bux.SetBevel(2);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlg1::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnClose();
}

HBRUSH CDlg1::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
	if (CTLCOLOR_EDIT == nCtlColor)
	{
		UINT id = pWnd->GetDlgCtrlID();
		
		pDC->SetTextColor			(RGB(0,000,000));	//���� ������
		pDC->SetBkColor				(RGB(070,198,240));	//���� �� ������� 
		SetDCBrushColor(pDC->m_hDC,	 RGB(070,198,240));	//���� �������
		return (HBRUSH)::GetStockObject(DC_BRUSH);		//����������� ��� ���������
	}

	if (CTLCOLOR_STATIC == nCtlColor)
	{
		UINT id = pWnd->GetDlgCtrlID();

		pDC->SetTextColor			(RGB(000,000,000));	//���� ������
		pDC->SetBkColor				(RGB(070,198,240));	//���� �� ������� 
		SetDCBrushColor(pDC->m_hDC,	 RGB(070,198,240));	//���� �������
		return (HBRUSH)::GetStockObject(DC_BRUSH);		//����������� ��� ���������
		
	}

	if (CTLCOLOR_DLG == nCtlColor)
	{
		SetDCBrushColor(pDC->m_hDC,RGB(15,196,211));	//���� ������� �����
		return (HBRUSH)::GetStockObject(DC_BRUSH);		//����������� ��� ���������
	}

	return hbr;
}

//********************************************************************

void CDlg1::OnEnChangeUST_Freq_Rot()
{
	if(!MAIN_WINDOW->GLAVNIY_CLASS.Add_UST_Freq_Rot(true))
	{//���� ������, ��������� � ���
	//	MAIN_WINDOW->ShowERROR(MAIN_WINDOW->GLAVNIY_CLASS.m_Error,true);
	}
}

//********************************************************************

void CDlg1::OnReykaSet()
{
	float temp;
	CString s;


	if (m_bReyka_Edit == false) {
		m_bReyka_Edit = true;

		m_UST_Reyka.GetWindowText(s);
		m_reyka_temp = (float)_wtof(s.GetBuffer());
		s.ReleaseBuffer();

		m_UST_Reyka.SendMessage(EM_SETREADONLY, (WPARAM)FALSE, 0);
		m_UST_Reyka.SetFocus();
		m_UST_Reyka.SetSel(0, -1);
		m_UST_Reyka_Set.SetBGColor(BGColorEn);

	} else {
		m_UST_Reyka.GetWindowText(s);
		temp = (float)_wtof(s.GetBuffer());
		s.ReleaseBuffer();
		if (temp > 30.0f)
			temp = 30.0f;

		MAIN_WINDOW->GLAVNIY_CLASS.var_UST_Reyka = temp;
		
		s.Format(_T("%.2f"), temp);
		m_UST_Reyka.SetWindowText(s);

		m_bReyka_Edit = false;
		m_UST_Reyka.SendMessage(EM_SETREADONLY, (WPARAM)TRUE, 0);
		m_UST_Reyka_Set.SetBGColor(BGColorDis);
	}

//	if(!MAIN_WINDOW->GLAVNIY_CLASS.Add_UST_ONLOAD(false))
	{//���� ������, ��������� � ���
	//	MAIN_WINDOW->ShowERROR(MAIN_WINDOW->GLAVNIY_CLASS.m_Error,true);
	}
}

void CDlg1::ReykaCancel()
{
	CString s;


	s.Format(_T("%.2f"), m_reyka_temp);
	m_UST_Reyka.SetWindowText(s);

	m_bReyka_Edit = false;
	m_UST_Reyka.SendMessage(EM_SETREADONLY, (WPARAM)TRUE, 0);
	m_UST_Reyka_Set.SetBGColor(BGColorDis);
}



//******************************************************************************************
void CDlg1::OnBnClickedPosition_1()
{
	//��������� ����� ��������� ������� � ���������� ���������� ��������
	m_UST_Position.SetWindowText(_T("1"));

	if(!MAIN_WINDOW->GLAVNIY_CLASS.Add_UST_Freq_Rot(false,(FP32)MAIN_WINDOW->Dlg5.Positions_data.Pos[0]))
	{//���� ������, ��������� � ���
	//	MAIN_WINDOW->ShowERROR(MAIN_WINDOW->GLAVNIY_CLASS.m_Error,true);
	}
	if(prev_regime!=NULL)
	{
		prev_regime->SetBGColor(BGColorDis);//���������� ������������ ���������� �����
		prev_regime->Invalidate();			//���������� ���������� ������
		prev_regime=&r1;					//�������� ���������� ��������� ��� ���������� ������
		r1.SetBGColor(BGColorEn);			//�������� ���� ������
		r1.Invalidate();					//���������� ��� ������
	}

}
void CDlg1::OnBnClickedPosition_2()
{
	//��������� ����� ��������� ������� � ���������� ���������� ��������
	m_UST_Position.SetWindowText(_T("2"));

	if(!MAIN_WINDOW->GLAVNIY_CLASS.Add_UST_Freq_Rot(false,(FP32)MAIN_WINDOW->Dlg5.Positions_data.Pos[1]))
	{//���� ������, ��������� � ���
	//	MAIN_WINDOW->ShowERROR(MAIN_WINDOW->GLAVNIY_CLASS.m_Error,true);
	}
	if(prev_regime!=NULL)
	{
		prev_regime->SetBGColor(BGColorDis);//���������� ������������ ���������� �����
		prev_regime->Invalidate();			//���������� ���������� ������
		prev_regime=&r2;					//�������� ���������� ��������� ��� ���������� ������
		r2.SetBGColor(BGColorEn);			//�������� ���� ������
		r2.Invalidate();					//���������� ��� ������
	}
}
void CDlg1::OnBnClickedPosition_3()
{
	//��������� ����� ��������� ������� � ���������� ���������� ��������
	m_UST_Position.SetWindowText(_T("3"));

	if(!MAIN_WINDOW->GLAVNIY_CLASS.Add_UST_Freq_Rot(false,(FP32)MAIN_WINDOW->Dlg5.Positions_data.Pos[2]))
	{//���� ������, ��������� � ���
	//	MAIN_WINDOW->ShowERROR(MAIN_WINDOW->GLAVNIY_CLASS.m_Error,true);
	}
	if(prev_regime!=NULL)
	{
		prev_regime->SetBGColor(BGColorDis);//���������� ������������ ���������� �����
		prev_regime->Invalidate();			//���������� ���������� ������
		prev_regime=&r3;					//�������� ���������� ��������� ��� ���������� ������
		r3.SetBGColor(BGColorEn);			//�������� ���� ������
		r3.Invalidate();					//���������� ��� ������
	}
}
void CDlg1::OnBnClickedPosition_4()
{
	//��������� ����� ��������� ������� � ���������� ���������� ��������
	m_UST_Position.SetWindowText(_T("4"));

	if(!MAIN_WINDOW->GLAVNIY_CLASS.Add_UST_Freq_Rot(false,(FP32)MAIN_WINDOW->Dlg5.Positions_data.Pos[3]))
	{//���� ������, ��������� � ���
	//	MAIN_WINDOW->ShowERROR(MAIN_WINDOW->GLAVNIY_CLASS.m_Error,true);
	}
	if(prev_regime!=NULL)
	{
		prev_regime->SetBGColor(BGColorDis);//���������� ������������ ���������� �����
		prev_regime->Invalidate();			//���������� ���������� ������
		prev_regime=&r4;					//�������� ���������� ��������� ��� ���������� ������
		r4.SetBGColor(BGColorEn);			//�������� ���� ������
		r4.Invalidate();					//���������� ��� ������
	}
}
void CDlg1::OnBnClickedPosition_5()
{
	//��������� ����� ��������� ������� � ���������� ���������� ��������
	m_UST_Position.SetWindowText(_T("5"));

	if(!MAIN_WINDOW->GLAVNIY_CLASS.Add_UST_Freq_Rot(false,(FP32)MAIN_WINDOW->Dlg5.Positions_data.Pos[4]))
	{//���� ������, ��������� � ���
	//	MAIN_WINDOW->ShowERROR(MAIN_WINDOW->GLAVNIY_CLASS.m_Error,true);
	}
	if(prev_regime!=NULL)
	{
		prev_regime->SetBGColor(BGColorDis);//���������� ������������ ���������� �����
		prev_regime->Invalidate();			//���������� ���������� ������
		prev_regime=&r5;					//�������� ���������� ��������� ��� ���������� ������
		r5.SetBGColor(BGColorEn);			//�������� ���� ������
		r5.Invalidate();					//���������� ��� ������
	}
}
void CDlg1::OnBnClickedPosition_6()
{
	//��������� ����� ��������� ������� � ���������� ���������� ��������
	m_UST_Position.SetWindowText(_T("6"));

	if(!MAIN_WINDOW->GLAVNIY_CLASS.Add_UST_Freq_Rot(false,(FP32)MAIN_WINDOW->Dlg5.Positions_data.Pos[5]))
	{//���� ������, ��������� � ���
	//	MAIN_WINDOW->ShowERROR(MAIN_WINDOW->GLAVNIY_CLASS.m_Error,true);
	}
	if(prev_regime!=NULL)
	{
		prev_regime->SetBGColor(BGColorDis);//���������� ������������ ���������� �����
		prev_regime->Invalidate();			//���������� ���������� ������
		prev_regime=&r6;					//�������� ���������� ��������� ��� ���������� ������
		r6.SetBGColor(BGColorEn);			//�������� ���� ������
		r6.Invalidate();					//���������� ��� ������
	}
}
void CDlg1::OnBnClickedPosition_7()
{
	//��������� ����� ��������� ������� � ���������� ���������� ��������
	m_UST_Position.SetWindowText(_T("7"));

	if(!MAIN_WINDOW->GLAVNIY_CLASS.Add_UST_Freq_Rot(false,(FP32)MAIN_WINDOW->Dlg5.Positions_data.Pos[6]))
	{//���� ������, ��������� � ���
	//	MAIN_WINDOW->ShowERROR(MAIN_WINDOW->GLAVNIY_CLASS.m_Error,true);
	}
	if(prev_regime!=NULL)
	{
		prev_regime->SetBGColor(BGColorDis);//���������� ������������ ���������� �����
		prev_regime->Invalidate();			//���������� ���������� ������
		prev_regime=&r7;					//�������� ���������� ��������� ��� ���������� ������
		r7.SetBGColor(BGColorEn);			//�������� ���� ������
		r7.Invalidate();					//���������� ��� ������
	}
}
void CDlg1::OnBnClickedPosition_8()
{
	//��������� ����� ��������� ������� � ���������� ���������� ��������
	m_UST_Position.SetWindowText(_T("8"));

	if(!MAIN_WINDOW->GLAVNIY_CLASS.Add_UST_Freq_Rot(false,(FP32)MAIN_WINDOW->Dlg5.Positions_data.Pos[7]))
	{//���� ������, ��������� � ���
	//	MAIN_WINDOW->ShowERROR(MAIN_WINDOW->GLAVNIY_CLASS.m_Error,true);
	}
	if(prev_regime!=NULL)
	{
		prev_regime->SetBGColor(BGColorDis);//���������� ������������ ���������� �����
		prev_regime->Invalidate();			//���������� ���������� ������
		prev_regime=&r8;					//�������� ���������� ��������� ��� ���������� ������
		r8.SetBGColor(BGColorEn);			//�������� ���� ������
		r8.Invalidate();					//���������� ��� ������
	}
}
void CDlg1::OnBnClickedPosition_9()
{
	//��������� ����� ��������� ������� � ���������� ���������� ��������
	m_UST_Position.SetWindowText(_T("9"));

	if(!MAIN_WINDOW->GLAVNIY_CLASS.Add_UST_Freq_Rot(false,(FP32)MAIN_WINDOW->Dlg5.Positions_data.Pos[8]))
	{//���� ������, ��������� � ���
	//	MAIN_WINDOW->ShowERROR(MAIN_WINDOW->GLAVNIY_CLASS.m_Error,true);
	}
	if(prev_regime!=NULL)
	{
		prev_regime->SetBGColor(BGColorDis);//���������� ������������ ���������� �����
		prev_regime->Invalidate();			//���������� ���������� ������
		prev_regime=&r9;					//�������� ���������� ��������� ��� ���������� ������
		r9.SetBGColor(BGColorEn);			//�������� ���� ������
		r9.Invalidate();					//���������� ��� ������
	}
}
void CDlg1::OnBnClickedPosition_10()
{
	//��������� ����� ��������� ������� � ���������� ���������� ��������
	m_UST_Position.SetWindowText(_T("10"));

	if(!MAIN_WINDOW->GLAVNIY_CLASS.Add_UST_Freq_Rot(false,(FP32)MAIN_WINDOW->Dlg5.Positions_data.Pos[9]))
	{//���� ������, ��������� � ���
	//	MAIN_WINDOW->ShowERROR(MAIN_WINDOW->GLAVNIY_CLASS.m_Error,true);
	}
	if(prev_regime!=NULL)
	{
		prev_regime->SetBGColor(BGColorDis);//���������� ������������ ���������� �����
		prev_regime->Invalidate();			//���������� ���������� ������
		prev_regime=&r10;					//�������� ���������� ��������� ��� ���������� ������
		r10.SetBGColor(BGColorEn);			//�������� ���� ������
		r10.Invalidate();					//���������� ��� ������
	}
}
void CDlg1::OnBnClickedPosition_11()
{
	//��������� ����� ��������� ������� � ���������� ���������� ��������
	m_UST_Position.SetWindowText(_T("11"));

	if(!MAIN_WINDOW->GLAVNIY_CLASS.Add_UST_Freq_Rot(false,(FP32)MAIN_WINDOW->Dlg5.Positions_data.Pos[10]))
	{//���� ������, ��������� � ���
	//	MAIN_WINDOW->ShowERROR(MAIN_WINDOW->GLAVNIY_CLASS.m_Error,true);
	}
	if(prev_regime!=NULL)
	{
		prev_regime->SetBGColor(BGColorDis);//���������� ������������ ���������� �����
		prev_regime->Invalidate();			//���������� ���������� ������
		prev_regime=&r11;					//�������� ���������� ��������� ��� ���������� ������
		r11.SetBGColor(BGColorEn);			//�������� ���� ������
		r11.Invalidate();					//���������� ��� ������
	}
}
void CDlg1::OnBnClickedPosition_12()
{
	//��������� ����� ��������� ������� � ���������� ���������� ��������
	m_UST_Position.SetWindowText(_T("12"));

	if(!MAIN_WINDOW->GLAVNIY_CLASS.Add_UST_Freq_Rot(false,(FP32)MAIN_WINDOW->Dlg5.Positions_data.Pos[11]))
	{//���� ������, ��������� � ���
	//	MAIN_WINDOW->ShowERROR(MAIN_WINDOW->GLAVNIY_CLASS.m_Error,true);
	}
	if(prev_regime!=NULL)
	{
		prev_regime->SetBGColor(BGColorDis);//���������� ������������ ���������� �����
		prev_regime->Invalidate();			//���������� ���������� ������
		prev_regime=&r12;					//�������� ���������� ��������� ��� ���������� ������
		r12.SetBGColor(BGColorEn);			//�������� ���� ������
		r12.Invalidate();					//���������� ��� ������
	}
}
void CDlg1::OnBnClickedPosition_13()
{
	//��������� ����� ��������� ������� � ���������� ���������� ��������
	m_UST_Position.SetWindowText(_T("13"));

	if(!MAIN_WINDOW->GLAVNIY_CLASS.Add_UST_Freq_Rot(false,(FP32)MAIN_WINDOW->Dlg5.Positions_data.Pos[12]))
	{//���� ������, ��������� � ���
	//	MAIN_WINDOW->ShowERROR(MAIN_WINDOW->GLAVNIY_CLASS.m_Error,true);
	}
	if(prev_regime!=NULL)
	{
		prev_regime->SetBGColor(BGColorDis);//���������� ������������ ���������� �����
		prev_regime->Invalidate();			//���������� ���������� ������
		prev_regime=&r13;					//�������� ���������� ��������� ��� ���������� ������
		r13.SetBGColor(BGColorEn);			//�������� ���� ������
		r13.Invalidate();					//���������� ��� ������
	}
}
void CDlg1::OnBnClickedPosition_14()
{
	//��������� ����� ��������� ������� � ���������� ���������� ��������
	m_UST_Position.SetWindowText(_T("14"));

	if(!MAIN_WINDOW->GLAVNIY_CLASS.Add_UST_Freq_Rot(false,(FP32)MAIN_WINDOW->Dlg5.Positions_data.Pos[13]))
	{//���� ������, ��������� � ���
	//	MAIN_WINDOW->ShowERROR(MAIN_WINDOW->GLAVNIY_CLASS.m_Error,true);
	}
	if(prev_regime!=NULL)
	{
		prev_regime->SetBGColor(BGColorDis);//���������� ������������ ���������� �����
		prev_regime->Invalidate();			//���������� ���������� ������
		prev_regime=&r14;					//�������� ���������� ��������� ��� ���������� ������
		r14.SetBGColor(BGColorEn);			//�������� ���� ������
		r14.Invalidate();					//���������� ��� ������
	}
}
void CDlg1::OnBnClickedPosition_15()
{
	//��������� ����� ��������� ������� � ���������� ���������� ��������
	m_UST_Position.SetWindowText(_T("15"));

	if(!MAIN_WINDOW->GLAVNIY_CLASS.Add_UST_Freq_Rot(false,(FP32)MAIN_WINDOW->Dlg5.Positions_data.Pos[14]))
	{//���� ������, ��������� � ���
	//	MAIN_WINDOW->ShowERROR(MAIN_WINDOW->GLAVNIY_CLASS.m_Error,true);
	}
	if(prev_regime!=NULL)
	{
		prev_regime->SetBGColor(BGColorDis);//���������� ������������ ���������� �����
		prev_regime->Invalidate();			//���������� ���������� ������
		prev_regime=&r15;					//�������� ���������� ��������� ��� ���������� ������
		r15.SetBGColor(BGColorEn);			//�������� ���� ������
	}
}

//******************************************************************************************
void CDlg1::OnBnClickedBux()
{
	if (MAIN_WINDOW->GLAVNIY_CLASS.var_UST_Bux == false) {
		MAIN_WINDOW->GLAVNIY_CLASS.var_UST_Bux = true;

		m_UST_Bux.SetBGColor(BGColorEn);
	} else {
		MAIN_WINDOW->GLAVNIY_CLASS.var_UST_Bux = false;		
		m_UST_Bux.SetBGColor(BGColorDis);
	}
}

//******************************************************************************************
void CDlg1::OnStop()
{
	if(!MAIN_WINDOW->GLAVNIY_CLASS.Add_UST_Run_Stop(false))
	{//���� ������, ��������� � ���
	//	MAIN_WINDOW->ShowERROR(MAIN_WINDOW->GLAVNIY_CLASS.m_Error,true);
	}
}
void CDlg1::OnRun()
{
	if(!MAIN_WINDOW->GLAVNIY_CLASS.Add_UST_Run_Stop(true))
	{//���� ������, ��������� � ���
	//	MAIN_WINDOW->ShowERROR(MAIN_WINDOW->GLAVNIY_CLASS.m_Error,true);
	}
}

//*********************************************************************************************
//******************************************************************************************
void CDlg1::OnNagr()
{
	if(!MAIN_WINDOW->GLAVNIY_CLASS.Add_UST_ONLOAD(true))
	{//���� ������, ��������� � ���
	//	MAIN_WINDOW->ShowERROR(MAIN_WINDOW->GLAVNIY_CLASS.m_Error,true);
	}
	
}
void CDlg1::OnHolost()
{
	if(!MAIN_WINDOW->GLAVNIY_CLASS.Add_UST_ONLOAD(false))
	{//���� ������, ��������� � ���
	//	MAIN_WINDOW->ShowERROR(MAIN_WINDOW->GLAVNIY_CLASS.m_Error,true);
	}
}

//*********************************************************************************************


void CDlg1::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
}

/***************************	PreTranslateMessage(MSG *pMsg)	***********************************
*	����� � ��������� ��� ������� ������, ����� �������� ������� �������
***************************************************************************************************/
BOOL CDlg1::PreTranslateMessage(MSG *pMsg)
{
	int k=0;



	if (pMsg->hwnd == GetDlgItem(IDC_REYKA_EDIT)->m_hWnd)
	{
		switch(pMsg->message)
		{
			case WM_KEYDOWN:
				switch(pMsg->wParam)
				{
					//��������� ��������� ����� �� �����, ����, � ���� ��������� � �����,
					//������� �������� ��� �������� ����
					case VK_ESCAPE:
						ReykaCancel();
						return TRUE;
					case VK_RETURN:
						OnReykaSet();
						return TRUE;
				}
			break;
		}
		return CDialog::PreTranslateMessage(pMsg);	
	}
		

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
			case '.':
			case ',':
		
			return MAIN_WINDOW->PreTranslateMessage(pMsg);
			break;
		}
		break;
	}
	
	//return CDialog::PreTranslateMessage(pMsg);
	return FALSE;
}


#include "stdafx.h"
#include "GRAPH_Dlg.h"
#include "time.h"
//#include "afx.h"		//��� ������� GetStatus

IMPLEMENT_DYNAMIC(CGRAPH_Dlg, CDialog)

BEGIN_MESSAGE_MAP(CGRAPH_Dlg, CDialog)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CLOSE()

	ON_COMMAND(ID_FUNCTION_1,ShowFunction_1)
	ON_COMMAND(ID_FUNCTION_2,ShowFunction_2)
	ON_COMMAND(ID_FUNCTION_3,ShowFunction_3)
	ON_COMMAND(ID_FUNCTION_4,ShowFunction_4)
	ON_COMMAND(ID_FUNCTION_5,ShowFunction_5)
	ON_COMMAND(ID_FUNCTION_6,ShowFunction_6)
	ON_COMMAND(ID_FUNCTION_7,ShowFunction_7)
	ON_COMMAND(ID_FUNCTION_8,ShowFunction_8)

	ON_COMMAND(ID_PAUSE,Pause)
	ON_COMMAND(ID_STOP,Stop)
	ON_COMMAND(ID_RESTART,Restart)

	ON_COMMAND(ID_MOVE,ShowDlgScroll)
	ON_COMMAND(ID_FUNC_OPT ,ShowDlgFuncOptions)
	ON_COMMAND(ID_GRAPH_OPT, ShowDlgGraphOpt)
	ON_COMMAND(ID_CODOMAIN,ShowDlgCodomain)

	ON_COMMAND(ID_SAVE,Save)
	ON_COMMAND(ID_SAVE_FOR_MATHCAD,SaveForMathcad)
	ON_COMMAND(ID_LOAD,Load)
	ON_COMMAND(ID_SAVE_BMP,SaveBMP)

	ON_WM_MENUSELECT()
	ON_WM_SIZE()
	ON_WM_SIZING()

	ON_MESSAGE(MSG_GR_FUNC_NAME_SEL,FuncNameSelect)
	ON_COMMAND(ID_PRINTGRAPH, &CGRAPH_Dlg::OnPrintGraph)
END_MESSAGE_MAP()


/**********************************************************
*		�����������
************************************************************/
CGRAPH_Dlg::CGRAPH_Dlg(CWnd* pParent, UINT ID)
	: CDialog(ID, pParent)
{
	m_pGraphicOptions=NULL;
	m_pError=NULL;
//	m_pGraph=NULL;	//������� ��������� �� ������� ������
}

/**********************************************************
*		����������
************************************************************/
CGRAPH_Dlg::~CGRAPH_Dlg()
{
	//��������� ��� ������
/*	if (m_pGraph)
	{
		delete m_pGraph;
	}*/
}


BOOL CGRAPH_Dlg::Create(UINT ID, CWnd* pParentWnd,ERROR_Class * pError, CGraphicOptions * pGraphicOptions)
{
	ASSERT(pGraphicOptions);
	m_pGraphicOptions=pGraphicOptions;

	//���������� ��������� �� ����� ����������� ������
	ASSERT(pError);
	m_pError=pError;

	return CDialog::Create(ID, pParentWnd);
}

void CGRAPH_Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CGRAPH_Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	
	menu.LoadMenu(IDR_MENU_FOR_GRAPH);
	SetMenu(&menu);

	//��������� ������ ���� ��������
	HICON	m_hIcon;
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON);
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	//���������� ��������� ������� � ������� ����, ��� ���� ���������������� � ��������
	UINT tmp;

	tmp=menu.GetMenuState(ID_FUNCTION_1,MF_BYCOMMAND)==MF_CHECKED?MF_UNCHECKED:MF_CHECKED;
	menu.CheckMenuItem(ID_FUNCTION_1, MF_CHECKED | MF_BYCOMMAND);

	tmp=menu.GetMenuState(ID_FUNCTION_2,MF_BYCOMMAND)==MF_CHECKED?MF_UNCHECKED:MF_CHECKED;
	menu.CheckMenuItem(ID_FUNCTION_2, MF_CHECKED | MF_BYCOMMAND);

	tmp=menu.GetMenuState(ID_FUNCTION_3,MF_BYCOMMAND)==MF_CHECKED?MF_UNCHECKED:MF_CHECKED;
	menu.CheckMenuItem(ID_FUNCTION_3, MF_CHECKED | MF_BYCOMMAND);

	tmp=menu.GetMenuState(ID_FUNCTION_4,MF_BYCOMMAND)==MF_CHECKED?MF_UNCHECKED:MF_CHECKED;
	menu.CheckMenuItem(ID_FUNCTION_4, MF_CHECKED | MF_BYCOMMAND);

	tmp=menu.GetMenuState(ID_FUNCTION_5,MF_BYCOMMAND)==MF_CHECKED?MF_UNCHECKED:MF_CHECKED;
	menu.CheckMenuItem(ID_FUNCTION_5, MF_CHECKED | MF_BYCOMMAND);

	tmp=menu.GetMenuState(ID_FUNCTION_6,MF_BYCOMMAND)==MF_CHECKED?MF_UNCHECKED:MF_CHECKED;
	menu.CheckMenuItem(ID_FUNCTION_6, MF_CHECKED | MF_BYCOMMAND);

	tmp=menu.GetMenuState(ID_FUNCTION_7,MF_BYCOMMAND)==MF_CHECKED?MF_UNCHECKED:MF_CHECKED;
	menu.CheckMenuItem(ID_FUNCTION_7, MF_CHECKED | MF_BYCOMMAND);

	tmp=menu.GetMenuState(ID_FUNCTION_8,MF_BYCOMMAND)==MF_CHECKED?MF_UNCHECKED:MF_CHECKED;
	menu.CheckMenuItem(ID_FUNCTION_8, MF_CHECKED | MF_BYCOMMAND);

	if(!m_Graph.Create(this,m_pGraphicOptions,m_pError,G_DEFAULTSCHEME))
	{
		m_pError->SetERROR(_TEXT("��� ������������� ������� �������� ������"), ERP_FATALERROR | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
		return FALSE;
	}
	
	//�������� ������ �� ���� ������ (!��� ������������ �������� ��� ����� ������ ����� �������� �������
	this->SetWindowPos(0,0,0,m_pGraphicOptions->var_GraphWidth+5,m_pGraphicOptions->var_GraphHeight+40,SWP_NOMOVE | SWP_NOZORDER);
	

	//�������� ������ ���� ������� ������� �������
	DlgCodomain.CreateMAIN(IDD_CODOMAIN_DLG,this);
	//++++ ShowDlgCodomain(); //� ������� ��� (���������� ��� ������ �� ������ ���������� ������� ����� ���� ������������)

	//�������� ������ ��������� �����������
	DlgScroll.CreateMAIN(IDD_SCROLL_BACK,this);
	//++++ ShowDlgScroll(); //� ������� ���
	
	//�������� ���� ��������� �������
	DlgGraphOpt.Create(IDD_GRAPH_OPT,this);
	//++++ ShowDlgGraphOpt();

	//�������� ������ � ������� ���������� �������
	DlgFuncOptions.CreateMAIN(IDD_FUNC_OPT,this);
	//++++ ShowDlgFuncOptions();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CGRAPH_Dlg::OnClose()
{

	if(DlgScroll.IsWindowVisible())
	{//���� ��� ���� ���� �������� ���� ���������, �� ������� ���
		::SendMessage(DlgScroll.m_hWnd,WM_CLOSE,0,0);
	}

	CDialog::OnClose();
}

//******************************************************************************
// ���������� ��� ��� �������
void CGRAPH_Dlg::ShowFunction_1()
{//���������� / �� ���������� ������������� ������ ������ �����,
//����� ����� ���� ����������� � ����
	ASSERT(m_Graph.VecFunctions.size()>0);
	//������ ��������� �������
	UINT state=menu.GetMenuState(ID_FUNCTION_1,MF_BYCOMMAND);

	if (state & MF_CHECKED)
	{//�� ����� ���� ������� ����������
		menu.CheckMenuItem(ID_FUNCTION_1, MF_UNCHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[0].Visible=false;//� ������ �������
	}
    else
	{//�� ����� ���� ������� �� ����������
		menu.CheckMenuItem(ID_FUNCTION_1, MF_CHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[0].Visible=true;//� ����� �������
	}
}
// ���������� ��� ��� �������
void CGRAPH_Dlg::ShowFunction_2()
{//���������� / �� ���������� ������������� ������ ������ �����,
//����� ����� ���� ����������� � ����
	ASSERT(m_Graph.VecFunctions.size()>1);
	//������ ��������� �������
	UINT state=menu.GetMenuState(ID_FUNCTION_2,MF_BYCOMMAND);

	if (state & MF_CHECKED)
	{//�� ����� ���� ������� ����������
		menu.CheckMenuItem(ID_FUNCTION_2, MF_UNCHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[1].Visible=false;//� ������ �������
	}
    else
	{//�� ����� ���� ������� �� ����������
		menu.CheckMenuItem(ID_FUNCTION_2, MF_CHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[1].Visible=true;//� ����� �������
	}
}
// ���������� ��� ��� �������
void CGRAPH_Dlg::ShowFunction_3()
{//���������� / �� ���������� ������������� ������ ������ �����,
//����� ����� ���� ����������� � ����
	ASSERT(m_Graph.VecFunctions.size()>2);
	//������ ��������� �������
	UINT state=menu.GetMenuState(ID_FUNCTION_3,MF_BYCOMMAND);

	if (state & MF_CHECKED)
	{//�� ����� ���� ������� ����������
		menu.CheckMenuItem(ID_FUNCTION_3, MF_UNCHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[2].Visible=false;//� ������ �������
	}
    else
	{//�� ����� ���� ������� �� ����������
		menu.CheckMenuItem(ID_FUNCTION_3, MF_CHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[2].Visible=true;//� ����� �������
	}
}
// ���������� ��� ��� �������
void CGRAPH_Dlg::ShowFunction_4()
{//���������� / �� ���������� ������������� ������ ������ �����,
//����� ����� ���� ����������� � ����
	ASSERT(m_Graph.VecFunctions.size()>3);
	//������ ��������� �������
	UINT state=menu.GetMenuState(ID_FUNCTION_4,MF_BYCOMMAND);

	if (state & MF_CHECKED)
	{//�� ����� ���� ������� ����������
		menu.CheckMenuItem(ID_FUNCTION_4, MF_UNCHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[3].Visible=false;//� ������ �������
	}
    else
	{//�� ����� ���� ������� �� ����������
		menu.CheckMenuItem(ID_FUNCTION_4, MF_CHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[3].Visible=true;//� ����� �������
	}
}
// ���������� ��� ��� �������
void CGRAPH_Dlg::ShowFunction_5()
{//���������� / �� ���������� ������������� ������ ������ �����,
//����� ����� ���� ����������� � ����
	ASSERT(m_Graph.VecFunctions.size()>4);
	//������ ��������� �������
	UINT state=menu.GetMenuState(ID_FUNCTION_5,MF_BYCOMMAND);

	if (state & MF_CHECKED)
	{//�� ����� ���� ������� ����������
		menu.CheckMenuItem(ID_FUNCTION_5, MF_UNCHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[4].Visible=false;//� ������ �������
	}
    else
	{//�� ����� ���� ������� �� ����������
		menu.CheckMenuItem(ID_FUNCTION_5, MF_CHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[4].Visible=true;//� ����� �������
	}
}
// ���������� ��� ��� �������
void CGRAPH_Dlg::ShowFunction_6()
{//���������� / �� ���������� ������������� ������ ������ �����,
//����� ����� ���� ����������� � ����
	ASSERT(m_Graph.VecFunctions.size()>5);
	//������ ��������� �������
	UINT state=menu.GetMenuState(ID_FUNCTION_6,MF_BYCOMMAND);

	if (state & MF_CHECKED)
	{//�� ����� ���� ������� ����������
		menu.CheckMenuItem(ID_FUNCTION_6, MF_UNCHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[5].Visible=false;//� ������ �������
	}
    else
	{//�� ����� ���� ������� �� ����������
		menu.CheckMenuItem(ID_FUNCTION_6, MF_CHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[5].Visible=true;//� ����� �������
	}

}
// ���������� ��� ��� �������
void CGRAPH_Dlg::ShowFunction_7()
{//���������� / �� ���������� ������������� ������ ������ �����,
//����� ����� ���� ����������� � ����
	ASSERT(m_Graph.VecFunctions.size()>6);
	//������ ��������� �������
	UINT state=menu.GetMenuState(ID_FUNCTION_7,MF_BYCOMMAND);

	if (state & MF_CHECKED)
	{//�� ����� ���� ������� ����������
		menu.CheckMenuItem(ID_FUNCTION_7, MF_UNCHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[6].Visible=false;//� ������ �������
	}
    else
	{//�� ����� ���� ������� �� ����������
		menu.CheckMenuItem(ID_FUNCTION_7, MF_CHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[6].Visible=true;//� ����� �������
	}

}
// ���������� ��� ��� �������
void CGRAPH_Dlg::ShowFunction_8()
{//���������� / �� ���������� ������������� ������ ������ �����,
//����� ����� ���� ����������� � ����
	ASSERT(m_Graph.VecFunctions.size()>7);
	//������ ��������� �������
	UINT state=menu.GetMenuState(ID_FUNCTION_8,MF_BYCOMMAND);

	if (state & MF_CHECKED)
	{//�� ����� ���� ������� ����������
		menu.CheckMenuItem(ID_FUNCTION_8, MF_UNCHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[7].Visible=false;//� ������ �������
	}
    else
	{//�� ����� ���� ������� �� ����������
		menu.CheckMenuItem(ID_FUNCTION_8, MF_CHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[7].Visible=true;//� ����� �������
	}

}
//************************************************************************************

//�������� ���� ��������� ���� �������
void CGRAPH_Dlg::ShowDlgFuncOptions()
{
	DlgFuncOptions.ShowWindow(SW_NORMAL);
}
	
//� ������� ������ �� �����
void CGRAPH_Dlg::Pause (void)
{
	CString str;
	menu.GetMenuString(ID_PAUSE,str,MF_BYCOMMAND);
	if(str==_T("�����"))
	{//���� ������� �����
		if(m_Graph.SetRegime(GRG_PAUSE))
		{
			menu.ModifyMenu(ID_PAUSE, MF_BYCOMMAND, ID_PAUSE, _T("����������"));
		}
	}
	else
	{//���� ����� ��������������
		if(DlgScroll.IsWindowVisible())
		{
			AfxMessageBox(_T("�������� ���� ��������� � ��������� �������"));
			//this->GRAPH_WINDOW->DlgScroll.CloseWindow();
			return;
		}
		if(m_Graph.SetRegime(GRG_PLAY))
		{
			menu.ModifyMenu(ID_PAUSE, MF_BYCOMMAND, ID_PAUSE, _T("�����"));
		}
	}
}
/*************************** Stop ****************************************
*					� ������� ������ �� ����
***************************************************************************/
void CGRAPH_Dlg::Stop (void)
{
	//��� ������� ��������������� ������� �����
	menu.ModifyMenu(ID_PAUSE, MF_BYCOMMAND, ID_PAUSE, _T("�����"));
	//������� �����������, �� ������ �� ����� �������������� ���
	menu.EnableMenuItem(ID_PAUSE,MF_BYCOMMAND | MF_GRAYED);
	menu.EnableMenuItem(ID_STOP,MF_BYCOMMAND | MF_GRAYED);
	
	m_Graph.SetRegime(GRG_STOP);
}

/*************************** Start ****************************************
*					� ������� ������ �� �����
***************************************************************************/
void CGRAPH_Dlg::Restart (void)
{
	//����� ����� ���������
	menu.EnableMenuItem(ID_PAUSE,MF_BYCOMMAND | MF_ENABLED);
	menu.EnableMenuItem(ID_STOP,MF_BYCOMMAND | MF_ENABLED);

	//������������ ������
	m_Graph.SetRegime(GRG_RESTART);

	//�������� �������� ����������� ���� �� ���-�� �����������
	SetWindowText(_T("  ������"));
}

//*****************************************************************************************

//�������� ���� � ���������� �������
void CGRAPH_Dlg::ShowDlgScroll(void)
{
	DlgScroll.ShowWindow(SW_NORMAL);
}

//�������� ���� ��������� �������
void CGRAPH_Dlg::ShowDlgGraphOpt(void)
{
	DlgGraphOpt.ShowWindow(SW_NORMAL);
}

//�������� ���� ������� ��������
void CGRAPH_Dlg::ShowDlgCodomain(void)
{
	DlgCodomain.ShowWindow(SW_NORMAL);
	DlgCodomain.SetFocus();
}
//******************************************************************************************

//��������� ������
void CGRAPH_Dlg::Save (void)
{
	// �������� ����������� ������ ������ ����� Open
	CFileDialog DlgOpen(FALSE,(LPCTSTR)_T("ser"),_T("first.ser"),OFN_HIDEREADONLY,(LPCTSTR)_T("����� ������� (*.ser) |*.ser||"));
	while(1)
	{//�� ������, ���� ������������ � ������ ������� �� ������ �������
		// ����������� ����������� ������ ������ ����� Open
		if(DlgOpen.DoModal()==IDOK)
		{
			CString str;
			str=DlgOpen.GetFileName();
			CFileStatus lStatus;
			if(CFile::GetStatus(str,lStatus))
			{//���� � ����� ������ ��� ����������
				if(AfxMessageBox(_T("���� � ����� ������ ��� ����������. ������������?"), MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2)==IDNO)
				{//������������ �� ������� �������������� ���� ����, ����� � ������
					continue;
				}
			}
			if(m_Graph.SaveGraph(str.GetBuffer()))
			{//���������� ������ �������
				m_pError->SetERROR(_T("������ ������� �������� � ���� \"")+str+_T("\""),ERP_GOOD | ER_WRITE_TO_LOG);
			}
		}
		break;
	}
}

/*******************************    SaveForMathcad(void)   ***********************
*	������� ��������� ������ �� �������_�������� ������� � ���� ������� ��� mathcad
**********************************************************************************/
void CGRAPH_Dlg::SaveForMathcad(void)
{
	// �������� ����������� ������ ������ ����� Open
	CFileDialog DlgOpen(FALSE,(LPCTSTR)_T("dat"),_T("for_mathcad.dat"),OFN_HIDEREADONLY,(LPCTSTR)_T("����� ������ ��� mathcad (*.dat) |*.dat||"));
	// ����������� ����������� ������ ������ ����� Open
	if(DlgOpen.DoModal()==IDOK)
	{
		CString str;
		str=DlgOpen.GetFileName();
		
		if(m_Graph.SaveGraphMathcad(str.GetBuffer(), -1))
		{//���������� ������ �������
			m_pError->SetERROR(_T("������ ������� �������� ��� Mathcad � ���� \"")+str+_T("\""),ERP_GOOD | ER_WRITE_TO_LOG);
		}
	}
}

/*******************************    Load (void)   ******************************
*		��������� ������ �� �����
**********************************************************************************/
void CGRAPH_Dlg::Load (void)
{
	// �������� ����������� ������ ������ ����� Open
	CFileDialog DlgOpen(TRUE,(LPCTSTR)_T("ser"),NULL,0,(LPCTSTR)_T("����� ������� (*.ser) |*.ser| ��� ����� (*.*) |*.*||"),this);
	// ����������� ����������� ������ ������ ����� Open
	if(DlgOpen.DoModal()==IDOK)
	{
		CString open_file_name, open_file_dir;
		open_file_name=DlgOpen.GetFileName();
		open_file_dir=DlgOpen.GetPathName();
		
		if(!m_Graph.VecFunctions.empty() && AfxMessageBox(_T("�������� ���������� � ������������� �������?"), MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2)==IDYES)
		{//�� ������� ��� ���� ������� � ������������ �� �����, ����� ��� ��������
			m_Graph.LoadGraphToExists(open_file_name.GetBuffer());
		}
		else
		{
			m_Graph.LoadGraph(open_file_name.GetBuffer());
			//�������� �������� ����������� ����, ����� ��� ���������� ��� ��������� �����
			if(open_file_dir.GetLength()>80)
			{//�������� ������� ������� ��� �����
				open_file_dir=_T("...")+open_file_dir.Right(80);
			}
			SetWindowText(_T("   ")+open_file_dir);
		}
	}
}

/***************************** SaveBMP(void) ***************************************************
*		��������� ��������� ������� ���� (��� � �������� ������) � ���� *.bmp 
*************************************************************************************************/
void CGRAPH_Dlg :: SaveBMP(void)
{
	//� ��� ���
	CString BMP_file_name;
	

	// �������� ����������� ������ ������ ����� Open
	CFileDialog DlgOpen(FALSE,(LPCTSTR)_T("bmp"),_T("graph_1.bmp"),OFN_HIDEREADONLY,(LPCTSTR)_T("������� (*.bmp) |*.bmp||"));
	// ����������� ����������� ������ ������ ����� Open

	if(DlgOpen.DoModal()==IDOK)
	{
		BMP_file_name=DlgOpen.GetFileName();
		//���� ������������ �� ��������� ����������
		if(BMP_file_name.Find('.')==BMP_file_name.GetLength())
			BMP_file_name=BMP_file_name+_T(".bmp");

		if(m_Graph.SaveGraphBMP(BMP_file_name.GetBuffer()))
		{//���������� ������ �������
			m_pError->SetERROR(_T("�������� ������� ������� ��������� � ���� \"")+BMP_file_name+_T("\""),ERP_GOOD | ER_WRITE_TO_LOG);
		}
	}
}



/********************************* OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu) **********
*		��� ������� ����������, ����� ������������ ����� �� ����� ���� � ���� ����
*****************************************************************************************************/
void CGRAPH_Dlg::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
{
	CDialog::OnMenuSelect(nItemID, nFlags, hSysMenu);
	
	//��� ����� ���������� ������� � ���������� ����, ����� ��� ��������� �������� ��������
	//Visible ������ �� �������
	if(nItemID==0)
	{//��� ��� ��� ���� "���"
		/*��� ���������� ������ �� ������� ��������, ��� ����� ���������, ������ �� �������,
		, � ���� �� - ��������� �������� ��� �������
		*/
		for(unsigned char i=0; i<8; i++)
		{
			UINT ID;
			switch (i)
			{
			case 0: ID=ID_FUNCTION_1; break;
			case 1: ID=ID_FUNCTION_2; break;
			case 2: ID=ID_FUNCTION_3; break;
			case 3: ID=ID_FUNCTION_4; break;
			case 4: ID=ID_FUNCTION_5; break;
			case 5: ID=ID_FUNCTION_6; break;
			case 6: ID=ID_FUNCTION_7; break;
			case 7: ID=ID_FUNCTION_8; break;
			}
			if(i>=m_Graph.VecFunctions.size())
			{//����� ������� ������ �� ����������
				menu.EnableMenuItem(ID, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
			}
			else
			{//������� ����, ����� ����������, ������ �� ���
				if(m_Graph.VecFunctions[i].Visible)
				{	menu.CheckMenuItem(ID, MF_CHECKED | MF_BYCOMMAND); }
				else
				{	menu.CheckMenuItem(ID, MF_UNCHECKED | MF_BYCOMMAND);}
			}
		}
	}
}

/*********************    OnSize(UINT nType, int cx, int cy)   ********************
*		��� ������� ����������, ����� �������� ������� ����. ��� ����� �������� �������
*	�������
**********************************************************************************/
void CGRAPH_Dlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	//����� ���������� � ������� �������
	m_Graph.SetGraphSize(cy, cx);
}

/**********************   OnSizing(UINT fwSide, LPRECT pRect)   ********************
*		��� ��������� ����������, ����� �� ��� ������ �������� �������� ������
*	� ������ ���� ����������� ������ ���������� �������
*************************************************************************************/
void CGRAPH_Dlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	//������� �������, ������� �� ���������
	if((pRect->right-pRect->left)<G_MINGRAPHWIDTH)
	{
		pRect->right=pRect->left+G_MINGRAPHWIDTH;
	}
	INT8U lCorrect=50;	//������������� �� ���� � ��������� ����
	if((pRect->bottom-pRect->top-lCorrect)<G_MINGRAPHHEIGHT)
	{
		pRect->bottom=pRect->top+lCorrect+G_MINGRAPHHEIGHT;
	}
	CDialog::OnSizing(fwSide, pRect);
}

/****************    FuncNameSelect    *********************************************
*		������ ������� ��������� � ���, ��� ������������ ������ ������� ����� �� 
*	����� �� ���� �������
***********************************************************************************/
LRESULT CGRAPH_Dlg::FuncNameSelect(WPARAM FuncNum, LPARAM l)
{
	//�������� ���������� �� ��������� �������
	DlgFuncOptions.m_PrimaryFunctionNum=(INT32S)FuncNum;
	ShowDlgFuncOptions();

	return 0;
}

/****************************	DestroyGRAPH(CGRAPH_Dlg* pDlg)	*****************
*		��� ������� ���������� � ������ CGRAPH_Dlg ��� fri�nd � ������� ����� ������
*	� ��������� �����������
*********************************************************************************/
void DestroyGRAPH(CGRAPH_Dlg* pDlg)
{
	//pDlg->SendMessage(WM_CLOSE);
	//pDlg->~CUSART_Dlg();
	delete pDlg;
}

/*******************************     OnPrintGraph    *******************************
*		� ������� ������ �� �������������� �������.
***********************************************************************************/
void CGRAPH_Dlg::OnPrintGraph()
{
	//������ ����������� ������� �������
	m_Graph.PrintGraph();
}

#include "stdafx.h"
#include "CommandWindow.h"

BEGIN_MESSAGE_MAP(CCommandWindow, CComboBox)
ON_CONTROL_REFLECT(CBN_SELENDOK, &CCommandWindow::OnCbnSelendok)
ON_CONTROL_REFLECT(CBN_EDITCHANGE, &CCommandWindow::OnCbnEditchange)
END_MESSAGE_MAP()

/*******************************************************************************
*		�������������		�������������		�������������
*	pError		- ��������� �� ����� ����������� ������
*	ProgramDir	- ���������� � ������ ���������
*	pFunc		- ��������� �� ������� ������� ��������� ������
********************************************************************************/
bool CCommandWindow::InitCommandWindow(ERROR_Class *pError,  CString ProgramDir)
{
	ASSERT(pError);

	m_ProgramDir=ProgramDir;
	m_pError=pError;

	//������� ������� ������� �������� �����������
	AddString(_T("mac [File_Name/ch]"));		//��������� ������
	AddString(_T("FP32 [var_name]"));			//������� ���������� � ��������� ������
	return true;
}

/*******************************************************************************
*				��������� ���������
*******************************************************************************/
BOOL CCommandWindow::PreTranslateMessage(MSG *pMsg)
{
	int k=0;
	switch(pMsg->message)
	{
	case WM_KEYDOWN:	//���� ������ ������
		switch(pMsg->wParam)
		{
		case VK_SPACE:
			if(GetAsyncKeyState(VK_CONTROL) & 0x8000)
			{
				AfxMessageBox(_T("adsg"));
			}
			break;
		case VK_UP:
			//��������� ���������� �������
			SetWindowText(PreviousCommand);	
			//����� ������ ����� � ����� 
			SetEditSel(PreviousCommand.GetLength(),PreviousCommand.GetLength());
			return true;
		case VK_RIGHT:
			{//��� ������� �� ��� ������ �� ��������� � ���� ��� �����
			//������� ������ �� ����������� �����

				int ltop=GetCurSel();	//������ � ���������� ���� ���������� ������
				if(ltop==CB_ERR)
				{//���� ���������� ��������� ��� - �������� ������ ������
					ltop=GetTopIndex();
				}
				if(ltop==CB_ERR) break;
				CString str;
				GetLBText(ltop,str);
				SetWindowText(str);
				//����� ������ ����� � ����� 
				unsigned char lpos=str.Find('[');	//���������� ������� ������� �������� ����������
				if(lpos==-1)lpos=str.GetLength();
				SetEditSel(lpos,str.GetLength());
				return true;
			}
		case VK_DOWN:
			//� ����������� ���������� ���� ���������� ������ ������ ��� 
			//������� �� ��� ������ ������������� ��������� �������
			//� ��� �� �����������,	��� ��� �� ����������
			if(!GetDroppedState())
			{
				ShowDropDown(true);	//������� ���������� ������
				return true;	
			}
			break;
		case VK_RETURN:
			{//��������� ������� �������
				//������� �����, ������� �� �������
				CString lAllString;			//��� �������� ������� � ������� � ����������� �� �������
				GetWindowText(lAllString);

				ShowDropDown(false);		//������� �����

				//���������� ��������
				bool lAllRight=PreTranslateCommand(lAllString);
				
				if(lAllRight)
				{//��� ������ ���������
					SetWindowText(_T(""));			//������ ���� ����� ��������� ���������� �������
					PreviousCommand=lAllString;		//�������� ��� �������
				}
				return true;						//�� ���� ���������� Enter � ������������ ����
			}
		}
	}
	return CComboBox::PreTranslateMessage(pMsg);
}

/*************************************************
* ���� ��� �� ��������� ������ ����� ������� ������
*	����������, �� ��� ����. ��������� ������ ���������
***************************************************/
bool CCommandWindow::GetBOOLArgument(CString *pstr, bool *ok)
{
	//���� ������� ���������� �������,
	//�� � *lok ������������ ����, � ��������� ������
	//�������� �� ����������

	pstr->TrimLeft(' ');	//������� ������� � ������
	if(pstr->GetLength()==0)
	{//��� ����� ������ ���
		*ok=false;			//��������� � �������� �����������
		return false;
	}
	CString larg;
	unsigned char lpos=pstr->Find(' ');
	if(lpos==-1)
	{//��� ������ - �������� ��� ��������
		lpos=pstr->GetLength()-1;
	}
	larg=pstr->Left(lpos); //������� �������� � ������
	pstr->Delete(0,lpos);	//������ �� ������ ��������� ��������

	if(larg==_T("true") || larg==_T("ok") || larg==_T("on") || larg==_T("yes"))
	{
		return true;
	}
	else if(larg==_T("false") || larg==_T("off") || larg==_T("no"))
	{
		return false;
	}
	else
	{//������������ ��������
		*ok=false;	//��������� � �������� �����������
		return false;
	}
}

/********************************************************
*	���� �� ��������� ������ ����� ������� �����, ��
*	���������� ��� �������
*******************************************************/
float CCommandWindow::GetFLOATArgument(CString *pstr, bool *ok)
{
	//���� ������� ���������� �������,
	//�� � *lok ������������ ����, � ��������� ������
	//�������� �� ����������

	pstr->TrimLeft(' ');	//������� ������� � ������
	if(pstr->GetLength()==0)
	{//��� ����� ������ ���
		*ok=false;
		return 0;
	}
	CString larg;
	unsigned char lpos=pstr->Find(' ');
	if(lpos==-1)
	{//��� ������ - �������� ��� ��������
		lpos=pstr->GetLength()-1;
	}
	larg=pstr->Left(lpos);	//������� �������� � ������
	pstr->Delete(0,lpos);	//������ �� ������ ��������� ��������

	//��������, ����� ��� � �� ��������� �����, � ����������
	if(FP32_map.find(larg)!=FP32_map.end())
	{//��, ����� ���������� ����
		return FP32_map[larg];	//������ �� ��������
	}

	return (float)_wtof(larg.GetBuffer());
}

/***************************************************************
*	����� �� ���������, �������� �� ��� ����� �����, ����� �������
*	��� �������. ��� ��������� ������ �� ����� �����
*****************************************************************/
INT32S CCommandWindow::GetINTArgument(CString *pstr, bool *ok)
{
	//���� ������� ���������� �������,
	//�� � *lok ������������ ����, � ��������� ������
	//�������� �� ����������

	FP32 temp=GetFLOATArgument(pstr, ok);	//������� ������� ��� �����
	if(*ok)
	{
		if(temp-((INT32S)temp)!=0)
		{//��� �� ����� �����
			*ok=false;
			CString str; str.Format(_T("��������� ������: �������� \"%f\" �� ����� �����"), temp);
			m_pError->SetERROR(str, ERP_WARNING | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
			return 0;
		}
	}
	return (INT32S)temp;
}

/***************************************************************
*	�� ��, ��� � GetINTArgument, �� ����������� �� ������������� 
*	�����
*****************************************************************/
INT32U CCommandWindow::GetUINTArgument(CString *pstr, bool *ok)
{
	FP32 temp=GetFLOATArgument(pstr, ok);	//������� ������� ��� �����
	if(*ok)
	{
		if(temp>=0 && (temp-((INT32S)temp)!=0))
		{//��� �� ����� �����
			*ok=false;
			CString str; str.Format(_T("��������� ������: �������� \"%f\" �� ������������� ����� �����"), temp);
			m_pError->SetERROR(str, ERP_WARNING | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
			return 0;
		}
	}
	return (INT32U)temp;
}

/*******************************************************************
*	�������� �� ������ ��������� ��������
********************************************************************/
CString CCommandWindow::GetSTRINGArgument(CString *pstr, bool *ok)
{
	//���� ������� ���������� �������,
	//�� � *lok ������������ ����, � ��������� ������
	//�������� �� ����������

	pstr->TrimLeft(' ');	//������� ������� � ������
	if(pstr->GetLength()==0)
	{//��� ����� ������ ���
		*ok=false;
		return _T("");
	}

	CString larg;	//���� ����� ������� �������� ���������
	INT8U	lpos;	//����� ������� - ����� ���������� ���������

	//��������� �������� ����� ����� �������, ����� ��� ����� ��������� � �������
	if((*pstr)[0]=='"')
	{//��, �������� � ��������
		pstr->TrimLeft('"');	//������ ��� ��������� �������
		lpos=pstr->Find('"');
	}
	else
	{//�������� ������������� ��������
		lpos=pstr->Find(' ');
		if(lpos==-1)
		{//��� ������ - �������� ��� ��������
			lpos=pstr->GetLength()-1;
		}
	}
	
	larg=pstr->Left(lpos); //������� �������� � ������
	pstr->Delete(0,lpos);	//������ �� ������ ��������� ��������
	if((*pstr)[0]=='"')
	{//� ����� ������ �������� ��� �������
		pstr->TrimLeft('"');	//������ �������
	}

	return larg;
}

bool CCommandWindow::ReadMacros(CString filename)
{
	ifstream lfile;
	if(filename[1]!=':')
	{//������� ������ ���, ��� ����������� ����
		//������� ����������
		filename=m_ProgramDir+_T("\\")+filename;
	}

	lfile.open(filename, ios_base::binary);
	if(!lfile.is_open())
	{
		AfxMessageBox(_T("�� ������� ������� ���� ������� ")+filename);
		return false;
	}

	while(!lfile.eof())
	{//��������������� ������� ��� ������� �� �����
		char lline[250];
		lfile.getline(lline,250);
		USES_CONVERSION;
		CString str=A2T(lline);
		str.TrimRight(0x0d);	//������� ������ �������� ������
		if(str==_T(""))
		{//������ ������ ������
			continue;
		}
		//���������� ��������
		if(!PreTranslateCommand(str))
		{
			return false;
		}
	}
	return true;	//��� ������ ���������
}

/*********     Create_FP32_var(CString var_name, FP32 var_value)   **************
*		������� ������� ���������� � ��������� ������ �� �������.
*	var_name	- ��� ����������
*	var_value	- ��������� ��������
********************************************************************************/
bool CCommandWindow::Create_FP32_var(CString var_name, FP32 var_value)
{
	if(FP32_map.find(var_name)==FP32_map.end())
	{//����� ���������� ��� ���
		FP32_map[var_name]=var_value;
	}
	else
	{
		CString str; str.Format(_T("������� �������������� ���������� %s"),var_name);
		m_pError->SetERROR(str,ERP_WARNING | ER_WRITE_TO_LOG);
		return false;
	}
	return true;
}

/**************************    Set_FP32_var    **********************************
*		����������� ���������� � ��������� ������ �� ������� �����-�� ��������
********************************************************************************/
bool CCommandWindow::Set_FP32_var(CString var_name, FP32 var_value)
{
	if(FP32_map.find(var_name)!=FP32_map.end())
	{//���������� ����������
		FP32_map[var_name]=var_value;
	}
	else
	{
		CString str; str.Format(_T("������� ��������� �������� �������������� ���������� %s"),var_name);
		m_pError->SetERROR(str,ERP_WARNING | ER_WRITE_TO_LOG);
		return false;
	}
	return true;
}

/*************************    OnCbnSelendok()     ***********************************
*		���������� ����� ������ � ������ ������-�� ��������. �� ��� ����� ������
*	������.
************************************************************************************/
void CCommandWindow::OnCbnSelendok()
{
	
}

/************************    OnCbnEditchange()    ***********************************
*		������������ ���� ���-�� �����.
************************************************************************************/
void CCommandWindow::OnCbnEditchange()
{
	CString str;
	GetWindowText(str);
	if(!GetDroppedState())
	{
		ShowDropDown(true);
		SetWindowText(str);
		SetEditSel(1,1);
	}
}

/***********************    PreTranslateCommand   ************************************
*		���������� ���������� ��������� ����������� �������� (����� ���� �������, 
*	������� ����� ���������� �����).
*	� ���� ���� ���������� ��������
*************************************************************************************/
bool CCommandWindow::PreTranslateCommand(CString AllString)
{
	//���������� ��������� �������
	CWnd* ptr=GetParent();

	CString lCommand;	//���� ��������� ���, ��� �������� ����� �������
	CString lArgString;	//� ���� ��������� ���� �������

	//������� �����������
	INT16U a=AllString.Find(_T("//"));
	AllString=AllString.Left(a);	//������� ���, ��� �� ������������
	AllString.TrimLeft(' ');							//������� �������
	if(AllString==_T(""))
	{//��� ������ ���� ������������
		return true;	//�� ��� �� ��� ���������
	}

	//������� ����� �������
	char lnum=AllString.Find(' ');
	if(lnum!=-1)
	{//����� ����� ������� ���� ���-�� ���
		lCommand=AllString.Left(lnum);
		lArgString=AllString.Right(AllString.GetLength()-lnum);
	}
	else
	{//��, ��� ����� ����� ������� ����������� ��������
		lCommand=AllString;
		lArgString=_T("");
	}

	bool lAllRight=false;

	//��������������� ��������� ��������� ������ 
	if(lCommand==_T("mac"))
	{//����� ��������� ������
		bool lok=true;
		//�������� �����
		CString lFileName=GetSTRINGArgument(&lArgString, &lok);
		if(lok)
		{
			if(lFileName==_T("ch"))
			{//������������ ����� ������� ����
				// �������� ����������� ������ ������ ����� Open
				CFileDialog DlgOpen(FALSE,(LPCTSTR)_T("txt"),NULL,NULL,(LPCTSTR)_T("������ (*.txt) |*.txt||"));

				// ����������� ����������� ������ ������ ����� Open
				if(DlgOpen.DoModal()==IDCANCEL)
				{//������������ �� ������� �������� ������
					return true;
				}

				lFileName=DlgOpen.GetPathName();
			}
			else
			{//��� ����� ������
				lFileName.Append(_T(".txt"));	//���������� ������������ �������������
			}
			if(ReadMacros(lFileName))
			{
				CString str; str.Format(_T("������ %s ������� ��������"), lFileName);
				m_pError->SetERROR(str,ERP_GOOD);
			}
			lAllRight=true; //� ����� ������� ����������
		}
	}
	else if(lCommand==_T("FP32"))
	{//������� ���������� � ��������� ������
	//���� ��� �� ������
		bool lok=true;
		CString lVarName=CCommandWindow::GetSTRINGArgument(&lArgString, &lok);
		if(lok)
		{
			ReCreate_FP32_var(lVarName, 0);
			lAllRight=true; //� ����� ������� ����������
		}
	}
	else if(lCommand==_T("SetVarValue"))
	{//������ ���������� ��������
		bool lok=true;
		CString lVarName=GetSTRINGArgument(&lArgString, &lok);
		//!!!���������� ��� ������ ���� !!!!
		FP32 lVarValue=GetFLOATArgument(&lArgString, &lok);
		if(lok)
		{
			Set_FP32_var(lVarName,lVarValue);
			lAllRight=true;				//� ����� ������� ����������
		}
	}
	else
	{//��� ������� ����������, �������� ������� � ������������� ����
		lAllRight=ptr->SendMessage(MSG_NEW_COMMAND,(WPARAM)lCommand.GetBuffer(),(LPARAM)lArgString.GetBuffer())==0?false:true;
	}

	if(!lAllRight)
	{
		m_pError->SetERROR(_T("����������� ������ ������� \"")+AllString+_T("\""), ERP_WATCHFUL | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
	}
	return lAllRight;
}
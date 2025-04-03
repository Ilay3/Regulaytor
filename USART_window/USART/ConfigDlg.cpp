#pragma once
#include "stdafx.h"
#include "ConfigDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigDlg dialog


CConfigDlg::CConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigDlg)
	m_strBaudRate = _T("");
	m_strDataBits = _T("");
	m_Parity = 'N';
	m_strStopBits = _T("");
	m_CommBreakDetected = FALSE;
	m_CommCTSDetected = FALSE;
	m_CommDSRDetected = FALSE;
	m_CommERRDetected = FALSE;
	m_CommRingDetected = FALSE;
	m_CommRLSDDetected = FALSE;
	m_CommRxchar = TRUE;
	m_CommRxcharFlag = FALSE;
	m_CommTXEmpty = FALSE;
	//}}AFX_DATA_INIT
}

CConfigDlg::CConfigDlg(CWnd* pParent, DCB dcb,  INT8U PortNum)
	: CDialog(CConfigDlg::IDD, pParent)
{
	m_dcb = dcb;		//��������� ������� ��������� �����
	m_PortNum=PortNum;	//��������� ����� �����
}



void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_BAUDRATECOMBO, m_strBaudRate);
	DDX_CBString(pDX, IDC_DATABITSCOMBO, m_strDataBits);
/*�� �����	DDX_CBString(pDX, IDC_PARITYCOMBO, m_strParity);*/
	DDX_CBString(pDX, IDC_STOPBITSCOMBO, m_strStopBits);
	DDX_Control(pDX, IDC_COM_NUM, m_ComNumList);
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialog)
	//{{AFX_MSG_MAP(CConfigDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/******************    OnInitDialog()   ***********************************
*		��� �������� ������� ��� ����� ��������� �������� ������������� 
*	�����������
***************************************************************************/
BOOL CConfigDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString strTemp;

	strTemp.Format(_T("%d"), m_dcb.BaudRate);
	if(((CComboBox*)GetDlgItem(IDC_BAUDRATECOMBO))->SelectString(0, strTemp)==CB_ERR)
	{//������� �������� � ������ �� ����������
		//��� ������ �� ������, ������ ����� ������� ������ � ����� ���������
		INT16U tmp=((CComboBox*)GetDlgItem(IDC_BAUDRATECOMBO))->AddString(strTemp);	
		//� ������ ������� ��
		((CComboBox*)GetDlgItem(IDC_BAUDRATECOMBO))->SetCurSel(tmp);
	}

	((CComboBox*)GetDlgItem(IDC_PARITYCOMBO))->SetCurSel(m_dcb.Parity);

	((CComboBox*)GetDlgItem(IDC_STOPBITSCOMBO))->SetCurSel(m_dcb.StopBits);

	strTemp.Format(_T("%d"), m_dcb.ByteSize);
	((CComboBox*)GetDlgItem(IDC_DATABITSCOMBO))->SelectString(0, strTemp);
	

	//����� �����
	CString TextCOM;
	COMMCONFIG cc;
	DWORD dwCCSize = sizeof(cc);
	bool ComInstalled[256];
	int TotalComNumbar = 0;

	//������ �� ������ ������ ������� ���������� ������
	int numDevs = m_ComNumList.GetCount();
	for (int i = 0; i < numDevs; i++)
	{
		m_ComNumList.DeleteString(0);
	}

	//������ ����� �����, ������� ��� �����������, ����� ���������� ���
	int lExistingNum=m_PortNum;

	for (int i = 1; i < 10; i++)
	{
		TextCOM.Format(_T("Com%i"),i);
		if ( GetDefaultCommConfig(TextCOM, &cc, &dwCCSize) )
		{
			ComInstalled[i-1] = true;
			TotalComNumbar++;
			//������� ��������� �������
			CString str; str.Format(_T("%i"),i);	//������ �����
			m_ComNumList.AddString(str);
			if(i==lExistingNum)
			{//���� ���� ������������ ������, ������� ���
				m_ComNumList.SetCurSel(TotalComNumbar-1);
			}
		}
		else
		{
			ComInstalled[i-1] = false;
		}
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
																  

/**************************    OnOK()   ******************************************
*		����� ��������� ���������. ����� ��� ����� ��������� ����� �����������������
*	�����, �.�. ������� �� ���� ������ �����������.
********************************************************************************/
void CConfigDlg::OnOK()
{
	TCHAR str[5];
	m_ComNumList.GetLBText(m_ComNumList.GetCurSel(),str);
	m_PortNum=_ttoi(str);
	
	CDialog::OnOK();
}

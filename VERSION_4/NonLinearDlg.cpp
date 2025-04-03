// NonLinearDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VERSION_4.h"
#include "NonLinearDlg.h"
#include "ModeInputDlg.h"	


// CNonLinearDlg dialog

IMPLEMENT_DYNAMIC(CNonLinearDlg, CDialog)

//CNonLinearDlg::CNonLinearDlg(CWnd* pParent /*=NULL*/)
//	: CDialog(CNonLinearDlg::IDD, pParent)
//{
//}

CNonLinearDlg::~CNonLinearDlg()
{
}

void CNonLinearDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNonLinearDlg, CDialog)
	ON_BN_CLICKED(IDC_INSERT_NL_BUTTON, OnBnClickedInsertNLBtn)
	ON_BN_CLICKED(IDC_RESULT_NL_BUTTON, OnBnClickedResultNLBtn)
	ON_BN_CLICKED(IDC_SAVE_NL_BUTTON, OnBnClickedSaveNLBtn)
	ON_BN_CLICKED(IDC_PRINT_NL_BUTTON, OnBnClickedPrintNLBtn)
END_MESSAGE_MAP()


// CNonLinearDlg message handlers
BOOL CNonLinearDlg::OnInitDialog()
{
	CDialog::OnInitDialog();


	m_EnterDlg.pGLAVNIY_CLASS = pGLAVNIY_CLASS;
	m_EnterDlg.Create(IDD_INSERT_NL_DLG, this);
	m_EnterDlg.CenterWindow();

	m_ReportDlg.pGLAVNIY_CLASS = pGLAVNIY_CLASS;
	m_ReportDlg.Create(IDD_EGU_REPORT_DLG, this);
	m_ReportDlg.CenterWindow();

	TCHAR dir[_MAX_PATH];
	GetCurrentDirectory(_MAX_PATH, dir);
	m_sDir.Format(_T("%s\\Результаты ДП"), dir);
	CreateDirectory(m_sDir, NULL);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CNonLinearDlg::OnBnClickedInsertNLBtn()
{
	CModeInputDlg dlg;
	dlg.DoModal();

	m_EnterDlg.mode = dlg.mode;
	m_EnterDlg.ShowWindow(SW_SHOW);
}

void CNonLinearDlg::OnBnClickedResultNLBtn()
{
	m_ReportDlg.Update();
	m_ReportDlg.ShowWindow(SW_SHOW);
}

void CNonLinearDlg::OnBnClickedSaveNLBtn()
{
	Save();
}

void CNonLinearDlg::OnBnClickedPrintNLBtn()
{
	Print();
}



void CNonLinearDlg::Save()
{
	CString str; 

	CStdioFile m_stdFile;

	CString m_sFile(m_sDir + _T("\\") + pGLAVNIY_CLASS->strNumberSensor + _T(".txt")); 

	static TCHAR BASED_CODE szFilter[] = _T("Txt Files (*.txt)|*.txt|All Files (*.*)|*.*||");

	CFileDialog dlg(FALSE, _T("*.txt"), m_sFile, OFN_HIDEREADONLY, szFilter);						
	if (dlg.DoModal() == IDOK)
	{
		m_sFile = dlg.GetPathName();

		if (m_stdFile.Open(m_sFile, CFile::modeCreate | CFile::modeWrite, NULL)) {
			m_stdFile.WriteString(pGLAVNIY_CLASS->time_date);
			m_stdFile.WriteString(_T("\n"));
			m_stdFile.WriteString(_T("Номер ДП: "));
			m_stdFile.WriteString(pGLAVNIY_CLASS->strNumberSensor);
			m_stdFile.WriteString(_T("\n"));

			m_stdFile.WriteString(_T("\n"));
			str.Format(_T("Максимальное + отклонение: %.2f мм\n"), pGLAVNIY_CLASS->fhmaxp);
			m_stdFile.WriteString(str);
			str.Format(_T("Максимальное - отклонение: %.2f мм\n"), pGLAVNIY_CLASS->fhmaxm);
			m_stdFile.WriteString(str);
			str.Format(_T("Размах: %.2f мм\n"), pGLAVNIY_CLASS->fhmax);
			m_stdFile.WriteString(str);
			str.Format(_T("Нелинейность: %.2f %%\n"), pGLAVNIY_CLASS->fgamma);
			m_stdFile.WriteString(str);
			m_stdFile.WriteString(_T("\n"));

			str.Format(_T("Начальное значение X: %.2f мм\n"), pGLAVNIY_CLASS->fXStart);
			m_stdFile.WriteString(str);
			m_stdFile.WriteString(_T("\n"));

			str.Format(_T("Ход, мм\tКод ДП, мм\tОтклонение, мм\tОтклонение, %%\n"), pGLAVNIY_CLASS->fXStart);
			m_stdFile.WriteString(str);
			for (int i=0; i<pGLAVNIY_CLASS->m_VectNonLinear.size(); i++) {
				str.Format(_T("%d\t%.2f\t\t%.2f\t\t%.2f\n"), i+(int)pGLAVNIY_CLASS->fXStart,
					pGLAVNIY_CLASS->m_VectNonLinear[i].fY,
					pGLAVNIY_CLASS->m_VectNonLinear[i].fh,
					pGLAVNIY_CLASS->m_VectNonLinear[i].fgammai);
				m_stdFile.WriteString(str);
			}

			m_stdFile.Close();
		}
	}
}



void CNonLinearDlg::Print()
{
  // print dialog options
  CPrintDialog dlgPrint(FALSE,
      PD_ALLPAGES | PD_USEDEVMODECOPIES | PD_NOPAGENUMS | PD_NOSELECTION | PD_DISABLEPRINTTOFILE,
      this);

  //  show the print dialog.
  if(dlgPrint.DoModal() == IDOK)
  {
    //  get handle to printer object.
    HDC hdcPrinter = dlgPrint.GetPrinterDC();
    if (hdcPrinter!=NULL)
    {
      // get printer dc
      CDC dcPrinter;
      dcPrinter.Attach(hdcPrinter);

      //  fill the docinfo structure.
      DOCINFO diPrinter;
      ::ZeroMemory(&diPrinter, sizeof(DOCINFO));
      diPrinter.cbSize = sizeof(DOCINFO);
      diPrinter.lpszOutput = NULL;
      diPrinter.lpszDocName = _T("EGU100");//m_szName.GetBuffer(1);

      if (dcPrinter.StartDoc(&diPrinter) >= 0)
      {
        // start page
        VERIFY(dcPrinter.StartPage() >= 0);

        //  get the printer width.
        int nPrinterWidth = 0;
        nPrinterWidth = dcPrinter.GetDeviceCaps(HORZRES);
//        nPrinterWidth = dcPrinter.GetDeviceCaps(LOGPIXELSX);

        //  get the printer height.
        int nPrinterHeight = 0;
        nPrinterHeight = dcPrinter.GetDeviceCaps(VERTRES);
//        nPrinterHeight = dcPrinter.GetDeviceCaps(LOGPIXELSY);

        //  claculate a size for the text.
        int nFontHeight = (int)((float)nPrinterHeight / 80.0f);

        //  get the area to draw in.
	      CRect rcClient(0, 0, nPrinterWidth, nPrinterHeight);

	      dcPrinter.SaveDC();

        //  draw all the support graphics.
//		  	m_pGraph->Render(&dcPrinter, &rcClient, TRUE);
/*
        CRect rcPlotArea = DrawFramework(&dcPrinter, rcClient, nFontHeight, true);
        //  draw the plots.
        DrawData(&dcPrinter, rcPlotArea, true);	*/
//		  dcPrinter.TextOut(100, 100, _T("ЖОПА"));	
	      dcPrinter.RestoreDC(-1);

        VERIFY(dcPrinter.EndPage() >= 0);
        VERIFY(dcPrinter.EndDoc() >= 0);

      }
      else //  there was a printer error of some sort....
      {
        VERIFY(dcPrinter.AbortDoc( ) >= 0 );
        AfxMessageBox(L"Cannot Start Print Job.");
      }
      //  clean up
      dcPrinter.Detach();
      VERIFY( DeleteDC(hdcPrinter));
    }
  }
}

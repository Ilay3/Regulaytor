/******************************************************************************************************
*******************************************************************************************************
**		Это класс окна настроек приемо-передачи
*******************************************************************************************************
********************************************************************************************************/

/********************** ЛИСТ РЕГИСТРАЦИИ ИЗМЕНЕНИЙ ****************************************************
*		11.09.2006 - Теперь при отключееном файле ПС не создается соответствующей переменной настройки
*******************************************************************************************************/

#pragma once
#include "afxwin.h"
#include <SerialPort\SerialPort.h>				//класс работы с последовательным портом
#include "ConfigDlg.h"

class CUSART_Dlg;

class CFlagsDlg : public CDialog
{
	DECLARE_DYNAMIC(CFlagsDlg)

public:
	CFlagsDlg(CWnd* pParent);   // standard constructor
	virtual ~CFlagsDlg();
	int b_flag;
	CUSART_Dlg* pUSART_Dlg;
// Dialog Data
	enum { IDD = IDD_FLAGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnConfigbutton();

	
public:
	virtual BOOL OnInitDialog();
	//ВРЕМЕННЫЕ ПЕРЕМЕННЫЕ НАСТРОЙКИ
	bool			o_emulation1;
	bool			o_nepr1;				//передавать ли непрерывно
	CString			o_inMesFile1;			//путь к файлу листига полученных сообщений
	bool			o_verify1;				//распознавать ли все сообщения
	bool			o_OFF1;					//передавать ли сообщения из файла
	CString			o_outMesFile1;			//имя файла из которого отправляются сообщения
	bool			o_HexDisplayOutMessage1;//показывать ли шестнадцатеричные значения в поле отправки
	bool			o_HexDisplayInMessage1;	//показывать ли шестнадцатеричные числа при приеме
	bool			o_LogOutMessage1;		//нужно ли фиксировать отправляемые сообщения в файле
	bool			o_LogInMessage1;		//нужно ли фиксировать принимаемые сообщения в файле
	bool			o_LogTimeMessages1;		//нужно ли фиксировать время отправки и приема сообщений
	INT32U			o_Period1;				//период передачи сообщений при непрерывной передачи
	INT16U			o_EmulSpeed1;			//период отправки символов при эмуляции
	
	bool		o_DoHistoryFile1;			//фиксировать ли ПС в отдельном файле
	//КОНЕЦ ВРЕМЕННЫХ ПЕРЕМЕННЫХ НАСТРОЙКИ

	//Реакции при нажатии или изменении настроек
	afx_msg void OnEmulatioN();
	afx_msg void OnVerify();
	afx_msg void OnInMesFile();
	afx_msg void OnNepr();
	afx_msg void OnOFF();
	afx_msg void OnEnChangeOutfile();
	afx_msg void OnBnClickedHexout();
	afx_msg void OnBnClickedHexin();
	afx_msg void OnBnClickedLogout();
	afx_msg void OnEnChangePeriod();

	// надпись о состоянии порта
	CStatic m_SCOM;
	
	
	
public:
	afx_msg void OnBnClickedLogin();
public:
	afx_msg void OnBnClickedLogtime();
	afx_msg void OnEnChangeEmulSpeed();
};

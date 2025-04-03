// Var.cpp : implementation file
//

#include "stdafx.h"
#include "Dlg5.h"
#include "VERSION_4Dlg.h"
//#include <stdlib.h>



// CDlg5 dialog

IMPLEMENT_DYNAMIC(CDlg5, CDialog)

CDlg5::~CDlg5()
{
}

void CDlg5::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMNUM, m_ComNumList);
}


BEGIN_MESSAGE_MAP(CDlg5, CDialog)
	ON_BN_CLICKED(IDC_SAVE, OnBnClickedSAVE)

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
	
	ON_BN_CLICKED(IDC_RADIO_TYPE_7, OnBnClickedType7)
	ON_BN_CLICKED(IDC_RADIO_TYPE_8, OnBnClickedType8)

	ON_BN_CLICKED(IDC_ENABLE_GRAPH, &CDlg5::OnBnClickedEnableGraph)

	ON_BN_CLICKED(IDC_BUTTON_SET_SPEED_1, OnBnClickedSetSpeed_1)
	ON_BN_CLICKED(IDC_BUTTON_SET_SPEED_2, OnBnClickedSetSpeed_2)

	ON_BN_CLICKED(IDC_OK1,     OnOK)
	ON_BN_CLICKED(IDC_CANCEL1, OnCANCEL)

	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()

END_MESSAGE_MAP()



// переопределим для защиты
BOOL CDlg5::Create(UINT ID, CVERSION_4Dlg* pParentWnd)
{
	MAIN_WINDOW=pParentWnd;	//сохраним указатель на родителя

	return CDialog::Create(ID, pParentWnd);
}

BOOL CDlg5::OnInitDialog()
{
	CDialog::OnInitDialog();
	//проставим это значение во избежание глюков
	m_iPred_pos= IDC_EDIT1;

	//отключим поля задания ЧВ
	((CEdit*)GetDlgItem(IDC_EDIT1))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT2))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT3))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT4))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT5))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT6))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT7))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT8))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT9))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT100))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT110))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT120))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT130))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT140))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT150))->EnableWindow(false);

    MAIN_WINDOW->ProtocolOptions.o_MsgType = 0;         // тип сообщения - 1 (L_DAT = 7)
  //MAIN_WINDOW->ProtocolOptions.o_MsgType = 1;         // тип сообщения - 2 (L_DAT = 8)

	//зададимся начальными значениями уставок ЧВ по режимам
	if(!Storing(false))
	{
		MAIN_WINDOW->m_Error.SetERROR(_T("Не удалось загрузить настройки при старте, прошла загрузка по умолчанию"),ERP_WATCHFUL | ER_WRITE_TO_LOG);
		// установки по умолчанию
		Positions_data.Pos[0] =350;
		Positions_data.Pos[1] =395;
		Positions_data.Pos[2] =445;
		Positions_data.Pos[3] =490;
		Positions_data.Pos[4] =535;
		Positions_data.Pos[5] =580;
		Positions_data.Pos[6] =630;
		Positions_data.Pos[7] =675;
		Positions_data.Pos[8] =720;
		Positions_data.Pos[9] =770;
		Positions_data.Pos[10]=815;
		Positions_data.Pos[11]=860;
		Positions_data.Pos[12]=910;
		Positions_data.Pos[13]=955;
		Positions_data.Pos[14]=1000;
		// Оставим без изменения настройку на первый попавшийся PortNumer
		MAIN_WINDOW->GLAVNIY_CLASS.period_of_transfer=150;	//период опроса в мс
	    MAIN_WINDOW->ProtocolOptions.o_MsgType = 0;         // тип сообщения - 1 (L_DAT = 7)
	  //MAIN_WINDOW->ProtocolOptions.o_MsgType = 1;         // тип сообщения - 2 (L_DAT = 8)
	}
	else
	{
		/*Т.к. инициализация этого диалога ДОЛЖНА происходить ДО инициаллизации модуля приемопередатчика
		, то еще не поздно записать номер последовательного порта*/
	}
	
	if (MAIN_WINDOW->ProtocolOptions.o_MsgType == 0)
	{
		((CButton *)GetDlgItem(IDC_RADIO_TYPE_7))->SetCheck(BST_CHECKED);
		((CButton *)GetDlgItem(IDC_RADIO_TYPE_8))->SetCheck(BST_UNCHECKED);
	} else {
		((CButton *)GetDlgItem(IDC_RADIO_TYPE_7))->SetCheck(BST_UNCHECKED);
		((CButton *)GetDlgItem(IDC_RADIO_TYPE_8))->SetCheck(BST_CHECKED);
	}

	
	
	/********	СОЗДАДИМ КРАСИВЫЕ НАДПИСИ	*******************************/
	static CFont m_Font;
	m_Font.DeleteObject();  //на всякий случай
	m_Font.CreateFont		//Выставим шрифт побольше
		(
			-20,                       // nHeight
			0,                         // nWidth
			0,                         // nEscapement
			0,                         // nOrientation
			FW_BOLD,                   // nWeight
			FALSE,                     // bItalic
			FALSE,                     // bUnderline
			0,                         // cStrikeOut
			DEFAULT_CHARSET,           // nCharSet
			OUT_DEFAULT_PRECIS,        // nOutPrecision
			CLIP_DEFAULT_PRECIS,       // nClipPrecision
			DEFAULT_QUALITY,           // nQuality
			DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
			_T("Times New Roman"));    // lpszFacename

	GetDlgItem(IDC_STATIC1)->SetFont(&m_Font,true);

	//фокус на кнопку
	GetDlgItem(IDC_SAVE)->SetFocus();

	//вывод в центр
	CenterWindow();

	return true;
}


void CDlg5::OnShowWindow(BOOL bShow, UINT nStatus)
{
	//проставим значение уставок ЧВ на форме
	TCHAR t[10];

	_itot_s(Positions_data.Pos[0],t,_countof(t),10);
	GetDlgItem(IDC_EDIT1)->SetWindowText(t);
	_itot_s(Positions_data.Pos[1],t,_countof(t),10);
	GetDlgItem(IDC_EDIT2)->SetWindowText(t);
	_itot_s(Positions_data.Pos[2],t,_countof(t),10);
	GetDlgItem(IDC_EDIT3)->SetWindowText(t);
	_itot_s(Positions_data.Pos[3],t,_countof(t),10);
	GetDlgItem(IDC_EDIT4)->SetWindowText(t);
	_itot_s(Positions_data.Pos[4],t,_countof(t),10);
	GetDlgItem(IDC_EDIT5)->SetWindowText(t);
	_itot_s(Positions_data.Pos[5],t,_countof(t),10);
	GetDlgItem(IDC_EDIT6)->SetWindowText(t);
	_itot_s(Positions_data.Pos[6],t,_countof(t),10);
	GetDlgItem(IDC_EDIT7)->SetWindowText(t);
	_itot_s(Positions_data.Pos[7],t,_countof(t),10);
	GetDlgItem(IDC_EDIT8)->SetWindowText(t);
	_itot_s(Positions_data.Pos[8],t,_countof(t),10);
	GetDlgItem(IDC_EDIT9)->SetWindowText(t);
	_itot_s(Positions_data.Pos[9],t,_countof(t),10);
	GetDlgItem(IDC_EDIT100)->SetWindowText(t);
	_itot_s(Positions_data.Pos[10],t,_countof(t),10);
	GetDlgItem(IDC_EDIT110)->SetWindowText(t);
	_itot_s(Positions_data.Pos[11],t,_countof(t),10);
	GetDlgItem(IDC_EDIT120)->SetWindowText(t);
	_itot_s(Positions_data.Pos[12],t,_countof(t),10);
	GetDlgItem(IDC_EDIT130)->SetWindowText(t);
	_itot_s(Positions_data.Pos[13],t,_countof(t),10);
	GetDlgItem(IDC_EDIT140)->SetWindowText(t);
	_itot_s(Positions_data.Pos[14],t,_countof(t),10);
	GetDlgItem(IDC_EDIT150)->SetWindowText(t);

	/**********	Покажем надпись номера текущего СОМ порта	************************/
	CString TextCOM;
	COMMCONFIG cc;
	DWORD dwCCSize = sizeof(cc);
	bool ComInstalled[256];
	int TotalComNumbar = 0;

	//очистим выпадающий список от старых значений
	int numDevs = m_ComNumList.GetCount();
	for (int i = 0; i < numDevs; i++)
		m_ComNumList.DeleteString(0);

	//узнаем номер порта, который уже испльзуется, чтобы подсветить его
	int lExistingNum=MAIN_WINDOW->ProtocolOptions.var_PORT_NUMBER;

	for (int i = 1; i < 10; i++)
	{
		TextCOM.Format(_T("Com%i"),i);
		if ( GetDefaultCommConfig(TextCOM, &cc, &dwCCSize) )
		{
			ComInstalled[i-1] = true;
			TotalComNumbar++;
			//добавим очередную строчку
			CString str; str.Format(_T("  COM%i"),i);	//!Менять нельзя, т.к. до номер д.б. 5 знаков!
			m_ComNumList.AddString(str);
			if(i==lExistingNum)
			{//этот порт используется сейчас, выберем его
				m_ComNumList.SetCurSel(TotalComNumbar-1);
			}
		}
		else
		{
			ComInstalled[i-1] = false;
		}
	}
	/*Если это окно показывается при старте, var_PORT_NUMBER может оказаться еще непроинициализированным и
	содержать 0*/
/*	if(MAIN_WINDOW->ProtocolOptions.var_PORT_NUMBER==PN_NOTINITIALIZED)
	{//порт невозможно было открыть при старте
		GetDlgItem(IDC_COMPORTVEIW)->SetWindowText(_T("Не настроен"));
		MAIN_WINDOW->m_Error.SetERROR(_T("Из сохраненных настроек прочитано значение порта, который в данный момент не может быть открыт"), ERP_WARNING | ER_WRITE_TO_LOG);
	}
	else if(MAIN_WINDOW->ProtocolOptions.var_PORT_NUMBER==PN_FIRST_AVAILABLE)
	{//этого не может быть, т.к. это число затирается при инициализации модуля приемопередатчика
		GetDlgItem(IDC_COMPORTVEIW)->SetWindowText(_T("ОШИБКА"));
		MAIN_WINDOW->m_Error.SetERROR(_T("Во время работы программы оказалось, что работаем с значением порта PN_FIRST_AVALABLE"), ERP_FATALERROR | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
	}
	else
	{//нормальный порт
		_itot_s(MAIN_WINDOW->ProtocolOptions.var_PORT_NUMBER,t,_countof(t),10);
		GetDlgItem(IDC_COMPORTVEIW)->SetWindowText(t);
	}*/

	/********** Покажем период опроса регулятора в мс ********************************/
	_itot_s(MAIN_WINDOW->GLAVNIY_CLASS.period_of_transfer,t,_countof(t),10);
	GetDlgItem(IDC_PERIOD_POLLING)->SetWindowText(t);
}

//******************************************************************************************
/*
Наша задача - просто сделать нужный элемент доступным для изменения
*/
void CDlg5::OnBnClickedPosition_1() {
	EditPositionSpeed(IDC_EDIT1);
}
void CDlg5::OnBnClickedPosition_2() {
	EditPositionSpeed(IDC_EDIT2);
}
void CDlg5::OnBnClickedPosition_3() {
	EditPositionSpeed(IDC_EDIT3);
}
void CDlg5::OnBnClickedPosition_4() {
	EditPositionSpeed(IDC_EDIT4);
}
void CDlg5::OnBnClickedPosition_5() {
	EditPositionSpeed(IDC_EDIT5);
}
void CDlg5::OnBnClickedPosition_6() {
	EditPositionSpeed(IDC_EDIT6);
}
void CDlg5::OnBnClickedPosition_7() {
	EditPositionSpeed(IDC_EDIT7);
}
void CDlg5::OnBnClickedPosition_8() {
	EditPositionSpeed(IDC_EDIT8);
}
void CDlg5::OnBnClickedPosition_9() {
	EditPositionSpeed(IDC_EDIT9);
}
void CDlg5::OnBnClickedPosition_10() {
	EditPositionSpeed(IDC_EDIT100);
}
void CDlg5::OnBnClickedPosition_11() {
	EditPositionSpeed(IDC_EDIT110);
}
void CDlg5::OnBnClickedPosition_12() {
	EditPositionSpeed(IDC_EDIT120);
}
void CDlg5::OnBnClickedPosition_13() {
	EditPositionSpeed(IDC_EDIT130);
}
void CDlg5::OnBnClickedPosition_14() {
	EditPositionSpeed(IDC_EDIT140);
}
void CDlg5::OnBnClickedPosition_15() {
	EditPositionSpeed(IDC_EDIT150);
}

//************************************************************************************************************
//редактирование одной выбраной уставки
void CDlg5::EditPositionSpeed(UINT idc_pos)
{
	((CEdit*)GetDlgItem(idc_pos))->EnableWindow(true);			//активируем текущий эдитик
	((CEdit*)GetDlgItem(idc_pos))->SetReadOnly(false);			//выставим текущий эдитик для редактирования

	if (m_iPred_pos != idc_pos)									//если нажали не повторно
	{
		((CEdit*)GetDlgItem(m_iPred_pos))->SetReadOnly(true);	//затемним предыдущий
		((CEdit*)GetDlgItem(m_iPred_pos))->EnableWindow(false);	//отключим предыдущий
	}

	m_iPred_pos = idc_pos;										//сохраним номер последнего редактируемого эдита
}


//************************************************************************************************************

	//Разделение сделано, чтобы во время отладки не было ограничений
#ifdef LAST_BUILD
	#define MAX_VALUE	1600	//|максимальное и минимальное для введения значение
	#define MIN_VALUE	100		//|
#else
	#define MAX_VALUE	2500	//|максимальное и минимальное для введения значение
	#define MIN_VALUE	0		//|
#endif
//проверяет правильность значений уставое и, если правильно, записывает их в Positions_data
bool CDlg5::CheckAllRight()
{
	/*
	Осуществим проверку правильности введенных данных, и только в случае их 
	корректности позволим закрыть окно
	*/
	CString str;
	Positions_data_struct temp_Positions_data;

	GetDlgItem(IDC_EDIT1)->GetWindowText(str);
	temp_Positions_data.Pos[0]=_ttoi(str.GetBuffer());
	GetDlgItem(IDC_EDIT2)->GetWindowText(str);
	temp_Positions_data.Pos[1]=_ttoi(str.GetBuffer());
	GetDlgItem(IDC_EDIT3)->GetWindowText(str);
	temp_Positions_data.Pos[2]=_ttoi(str.GetBuffer());
	GetDlgItem(IDC_EDIT4)->GetWindowText(str);
	temp_Positions_data.Pos[3]=_ttoi(str.GetBuffer());
	GetDlgItem(IDC_EDIT5)->GetWindowText(str);
	temp_Positions_data.Pos[4]=_ttoi(str.GetBuffer());
	GetDlgItem(IDC_EDIT6)->GetWindowText(str);
	temp_Positions_data.Pos[5]=_ttoi(str.GetBuffer());
	GetDlgItem(IDC_EDIT7)->GetWindowText(str);
	temp_Positions_data.Pos[6]=_ttoi(str.GetBuffer());
	GetDlgItem(IDC_EDIT8)->GetWindowText(str);
	temp_Positions_data.Pos[7]=_ttoi(str.GetBuffer());
	GetDlgItem(IDC_EDIT9)->GetWindowText(str);
	temp_Positions_data.Pos[8]=_ttoi(str.GetBuffer());
	GetDlgItem(IDC_EDIT100)->GetWindowText(str);
	temp_Positions_data.Pos[9]=_ttoi(str.GetBuffer());
	GetDlgItem(IDC_EDIT110)->GetWindowText(str);
	temp_Positions_data.Pos[10]=_ttoi(str.GetBuffer());
	GetDlgItem(IDC_EDIT120)->GetWindowText(str);
	temp_Positions_data.Pos[11]=_ttoi(str.GetBuffer());
	GetDlgItem(IDC_EDIT130)->GetWindowText(str);
	temp_Positions_data.Pos[12]=_ttoi(str.GetBuffer());
	GetDlgItem(IDC_EDIT140)->GetWindowText(str);
	temp_Positions_data.Pos[13]=_ttoi(str.GetBuffer());
	GetDlgItem(IDC_EDIT150)->GetWindowText(str);
	temp_Positions_data.Pos[14]=_ttoi(str.GetBuffer());

	for(unsigned char i=0;i<15;i++)
	{
		if(temp_Positions_data.Pos[i]>=MIN_VALUE && temp_Positions_data.Pos[i]<=MAX_VALUE)
		{
		}
		else
		{
			CString str; str.Format(_T("%u"),i+1);
			AfxMessageBox(CString(_T("Недопустимое значение на позиции "))+str+CString(_T("\n Исправьте данные!")));
			return false;
		}
	}
	

	//все нормально, можно копировать
	for(unsigned char i=0;i<15;i++)
	{
		Positions_data.Pos[i]=temp_Positions_data.Pos[i];
	}
	return true;
}
//************************************************************************************************************


void CDlg5::OnBnClickedSAVE()
{//кликнули на кнопку сохранить
	/*Перезапишем уставки по режимам*/
	if(CheckAllRight())
	{//если уставки корректны
		Storing(true);
	}
}

//сохраняет настройки в файл, или загружает их из него
bool CDlg5::Storing(bool save_or_load)
{
	//save_or_load==true	- сохранить настройки
	//save_or_load==false	- загрузить настройки из файла

	if(save_or_load)
	{//сохранить настройки
		CFile file(_T("SAVE.sav"), CFile::modeWrite | CFile::modeCreate);
		CArchive ar(&file, CArchive::store);
		//запишем все режимы
		ar << Positions_data.Pos[0];
		ar << Positions_data.Pos[1];
		ar << Positions_data.Pos[2];
		ar << Positions_data.Pos[3];
		ar << Positions_data.Pos[4];
		ar << Positions_data.Pos[5];
		ar << Positions_data.Pos[6];
		ar << Positions_data.Pos[7];
		ar << Positions_data.Pos[8];
		ar << Positions_data.Pos[9];
		ar << Positions_data.Pos[10];
		ar << Positions_data.Pos[11];
		ar << Positions_data.Pos[12];
		ar << Positions_data.Pos[13];
		ar << Positions_data.Pos[14];
		//и номер последовательного порта
		int lnum=m_ComNumList.GetCurSel();
		CString str; m_ComNumList.GetLBText(lnum,str); CString str2=str.Right(str.GetLength()-5);
		unsigned char lCOM_Num=_ttoi(str2);
//		unsigned char v=GetDlgItemInt(IDC_COMPORTVEIW);
		ar << lCOM_Num;
		//сохраним период опроса
		ar<<MAIN_WINDOW->GLAVNIY_CLASS.period_of_transfer;
		//сохраним тип сообщения
		ar<<MAIN_WINDOW->ProtocolOptions.o_MsgType;

		ar.Close();
		file.Close();
		MAIN_WINDOW->m_Error.SetERROR(_T("Настройки успешно сохранены"),ERP_NORMAL | ER_WRITE_TO_LOG);
	}
	else
	{//загрузить настройки
		CFile file;
		if(!file.Open(_T("SAVE.sav"), CFile::modeRead))
		{
			return false;
		}
		CArchive ar(&file, CArchive::load);
		//прочитаем уставки по режимам
		ar >> Positions_data.Pos[0];
		ar >> Positions_data.Pos[1];
		ar >> Positions_data.Pos[2];
		ar >> Positions_data.Pos[3];
		ar >> Positions_data.Pos[4];
		ar >> Positions_data.Pos[5];
		ar >> Positions_data.Pos[6];
		ar >> Positions_data.Pos[7];
		ar >> Positions_data.Pos[8];
		ar >> Positions_data.Pos[9];
		ar >> Positions_data.Pos[10];
		ar >> Positions_data.Pos[11];
		ar >> Positions_data.Pos[12];
		ar >> Positions_data.Pos[13];
		ar >> Positions_data.Pos[14];
		//и номер последовательного порта
		char t;
		ar>>t;	//гребаный компилятор
		MAIN_WINDOW->ProtocolOptions.var_PORT_NUMBER=t;
		//char t[3];
		//itoa(PortNumer,t,10);
		//((CEdit*)GetDlgItem(IDC_COMPORTVEIW))->SetWindowText(t);
		//загрузим период опроса
		ar>>MAIN_WINDOW->GLAVNIY_CLASS.period_of_transfer;
		//загрузим тип сообщения (два варианта сообщения по значению в поле L_DAT)
		//ar>>MAIN_WINDOW->ProtocolOptions.o_MsgType;

		ar.Close();
		file.Close();
		MAIN_WINDOW->m_Error.SetERROR(_T("Начальные настройки были успешно загружены"),ERP_NORMAL | ER_WRITE_TO_LOG);
	}
	return true;
}

/**********************    OnBnClickedEnableGraph    ************************************
*		Хотим включить/выключить построение графика
****************************************************************************************/
void CDlg5::OnBnClickedEnableGraph()
{
	if(MAIN_WINDOW->GLAVNIY_CLASS.GetGraphRegime()==GR_DISABLED)
		MAIN_WINDOW->GLAVNIY_CLASS.SetGraphRegime(GR_ENABLED);
	else
		MAIN_WINDOW->GLAVNIY_CLASS.SetGraphRegime(GR_DISABLED);
}


void CDlg5::OnBnClickedType7()
{
	MAIN_WINDOW->ProtocolOptions.o_MsgType = 0;
}

void CDlg5::OnBnClickedType8()
{
	MAIN_WINDOW->ProtocolOptions.o_MsgType = 1;
}


void CDlg5::OnBnClickedSetSpeed_1()
{
	int new_speed_pos[15];

	//уставки для Т3-02, Т3-06, Т3-10
	//
	new_speed_pos[ 0] =  350;		// 0,1
	new_speed_pos[ 1] =  395;		// 2
	new_speed_pos[ 2] =  445;
	new_speed_pos[ 3] =  490;
	new_speed_pos[ 4] =  535;
	new_speed_pos[ 5] =  580;
	new_speed_pos[ 6] =  630;
	new_speed_pos[ 7] =  675;
	new_speed_pos[ 8] =  720;
	new_speed_pos[ 9] =  770;
	new_speed_pos[10] =  815;
	new_speed_pos[11] =  860;
	new_speed_pos[12] =  910;
	new_speed_pos[13] =  955;
	new_speed_pos[14] = 1000;		// 15

	EditAllPositionSpeed(new_speed_pos);
}

void CDlg5::OnBnClickedSetSpeed_2()
{
	int new_speed_pos[15];

	//уставки для Т3-12
	//
	new_speed_pos[ 0] =  520;		// 0,1
	new_speed_pos[ 1] =  590;		// 2
	new_speed_pos[ 2] =  660;
	new_speed_pos[ 3] =  730;
	new_speed_pos[ 4] =  800;
	new_speed_pos[ 5] =  870;
	new_speed_pos[ 6] =  940;
	new_speed_pos[ 7] = 1010;
	new_speed_pos[ 8] = 1080;
	new_speed_pos[ 9] = 1150;
	new_speed_pos[10] = 1220;
	new_speed_pos[11] = 1290;
	new_speed_pos[12] = 1360;
	new_speed_pos[13] = 1430;
	new_speed_pos[14] = 1500;		// 15

	EditAllPositionSpeed(new_speed_pos);
}


//редактирование всех уставок
void CDlg5::EditAllPositionSpeed(int new_speed_pos[15])
{
	//проставим значение уставок ЧВ на форме
	TCHAR t[10];

	_itot_s(new_speed_pos[ 0], t, _countof(t), 10);			GetDlgItem(IDC_EDIT1)->SetWindowText(t);		// 0,1
	_itot_s(new_speed_pos[ 1], t, _countof(t), 10);			GetDlgItem(IDC_EDIT2)->SetWindowText(t);		// 2
	_itot_s(new_speed_pos[ 2], t, _countof(t), 10);			GetDlgItem(IDC_EDIT3)->SetWindowText(t);
	_itot_s(new_speed_pos[ 3], t, _countof(t), 10);			GetDlgItem(IDC_EDIT4)->SetWindowText(t);
	_itot_s(new_speed_pos[ 4], t, _countof(t), 10);			GetDlgItem(IDC_EDIT5)->SetWindowText(t);
	_itot_s(new_speed_pos[ 5], t, _countof(t), 10);			GetDlgItem(IDC_EDIT6)->SetWindowText(t);
	_itot_s(new_speed_pos[ 6], t, _countof(t), 10);			GetDlgItem(IDC_EDIT7)->SetWindowText(t);
	_itot_s(new_speed_pos[ 7], t, _countof(t), 10);			GetDlgItem(IDC_EDIT8)->SetWindowText(t);
	_itot_s(new_speed_pos[ 8], t, _countof(t), 10);			GetDlgItem(IDC_EDIT9)->SetWindowText(t);
	_itot_s(new_speed_pos[ 9], t, _countof(t), 10);			GetDlgItem(IDC_EDIT100)->SetWindowText(t);
	_itot_s(new_speed_pos[10], t, _countof(t), 10);			GetDlgItem(IDC_EDIT110)->SetWindowText(t);
	_itot_s(new_speed_pos[11], t, _countof(t), 10);			GetDlgItem(IDC_EDIT120)->SetWindowText(t);
	_itot_s(new_speed_pos[12], t, _countof(t), 10);			GetDlgItem(IDC_EDIT130)->SetWindowText(t);
	_itot_s(new_speed_pos[13], t, _countof(t), 10);			GetDlgItem(IDC_EDIT140)->SetWindowText(t);
	_itot_s(new_speed_pos[14], t, _countof(t), 10);			GetDlgItem(IDC_EDIT150)->SetWindowText(t);		// 15


	//отключим поля ввода
	((CEdit*)GetDlgItem(IDC_EDIT1))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT2))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT3))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT4))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT5))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT6))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT7))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT8))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT9))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT100))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT110))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT120))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT130))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT140))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT150))->EnableWindow(false);
}


void CDlg5::OnOK()
{//Нет нужны, поэтому проведем их обработку
	CString str;

	/***********************************************************
	*    Проверим на корректность номер последоватьльного порта
	************************************************************/
	int lnum=m_ComNumList.GetCurSel();
	m_ComNumList.GetLBText(lnum,str); CString str2=str.Right(str.GetLength()-5);
	int lCOM_Num=_ttoi(str2);
	if(MAIN_WINDOW->pUSART_Dlg->ReSetCOMport(lCOM_Num))
	{
		MAIN_WINDOW->m_Error.SetERROR(_T("Последовательный порт изменен, новое значение:")+ str, ERP_GOOD | ER_WRITE_TO_LOG);
		MAIN_WINDOW->pUSART_Dlg->StartTransfer();	//начнем передачу
	}
	else
	{
		MAIN_WINDOW->m_Error.SetERROR(_T("Ошибка при переопределении СОМ порта, попробуйте использовать другой номер"), ERP_ERROR | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
		m_ComNumList.SetFocus();	//Сделаем неверное окно активным
		return;
	}

	/*
		GetDlgItem(IDC_COMPORTVEIW)->GetWindowText(str);
		INT16U portNUM=_ttoi(str);
		if(portNUM!=MAIN_WINDOW->ProtocolOptions.var_PORT_NUMBER)
		{//изменили номер СОМ порта
		//проверим правильность задания СОМ порта
		if(portNUM<1 || portNUM>20)
		{//нет, такие порты нам не подходят
			AfxMessageBox(CString(_T("СОМ порт должен находиться в диапазоне от 1 до 20, исправьте данные!")));
			GetDlgItem(IDC_COMPORTVEIW)->SetFocus();	//Сделаем неверное окно активным
			return;
		}
		if(MAIN_WINDOW->pUSART_Dlg->ReSetCOMport(_ttoi(str)))
		{
			MAIN_WINDOW->m_Error.SetERROR(_T("СОМ порт переназначен успешно"), ERP_GOOD | ER_WRITE_TO_LOG);
			MAIN_WINDOW->pUSART_Dlg->StartTransfer();	//начнем передачу
		}
		else
		{
			MAIN_WINDOW->m_Error.SetERROR(_T("Ошибка при переопределении СОМ порта, попробуйте использовать другой номер"), ERP_ERROR | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
			GetDlgItem(IDC_COMPORTVEIW)->SetFocus();	//Сделаем неверное окно активным
			return;
		}

		//Запишем изменения в журнал регистрации
		MAIN_WINDOW->m_Error.SetERROR(_T("Последовательный порт изменен, новое значение: ")+ str, ERP_NORMAL | ER_WRITE_TO_LOG | ER_HIDE_FROM_USER);
		}
	*/

	/***********************************************************
	*    Проверим на корректность период опроса регулятора
	************************************************************/
	GetDlgItem(IDC_PERIOD_POLLING)->GetWindowText(str);
	INT16U lPeriod=_ttoi(str);
	if(lPeriod!=MAIN_WINDOW->GLAVNIY_CLASS.period_of_transfer)
	{//изменили период опроса
		
		if(lPeriod<10 || lPeriod>1000)
		{//недопустимые значения периода
			MAIN_WINDOW->m_Error.SetERROR(_T("Период опроса регулятора должен быть в пределах от 10 мс до 1 с, пожалуйста измените значение."), ERP_WATCHFUL | ER_SHOW_MESSAGE);
			GetDlgItem(IDC_PERIOD_POLLING)->SetFocus();	//Сделаем неверное окно активным
			return;
		}
		//Перезапустим таймер
		MAIN_WINDOW->GLAVNIY_CLASS.period_of_transfer=lPeriod;
		MAIN_WINDOW->SetTimer(MAIN_WINDOW->GLAVNIY_CLASS.PERIOD_OF_TRANSFER_ID,MAIN_WINDOW->GLAVNIY_CLASS.period_of_transfer,NULL);

		//Запишем изменения в журнал регистрации
		MAIN_WINDOW->m_Error.SetERROR(_T("Период опроса регулятора изменен, новое значение: ")+ str+ _T(" мс."), ERP_NORMAL | ER_WRITE_TO_LOG | ER_HIDE_FROM_USER);
	}

	/***********************************************************
	*    Проверим на корректность уставки по позициям
	************************************************************/
	if(CheckAllRight())
	{//если уставки корректны
		this->OnCancel();	//всё, можно закрывать окно
		MAIN_WINDOW->Dlg1.prev_regime->SendMessage(BM_CLICK,0,0);
		//MAIN_WINDOW->Dlg1.prev_regime->SetCheck(BST_CHECKED);
	}
}


void CDlg5::OnCANCEL()
{//нам не нужны проделанные изменения
	this->OnCancel();
}


void CDlg5::OnClose()
{

	CDialog::OnClose();
}


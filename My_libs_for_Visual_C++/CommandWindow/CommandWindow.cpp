#include "stdafx.h"
#include "CommandWindow.h"

BEGIN_MESSAGE_MAP(CCommandWindow, CComboBox)
ON_CONTROL_REFLECT(CBN_SELENDOK, &CCommandWindow::OnCbnSelendok)
ON_CONTROL_REFLECT(CBN_EDITCHANGE, &CCommandWindow::OnCbnEditchange)
END_MESSAGE_MAP()

/*******************************************************************************
*		ИНИЦИАЛИЗАЦИЯ		ИНИЦИАЛИЗАЦИЯ		ИНИЦИАЛИЗАЦИЯ
*	pError		- указатель на класс регистрации ошибок
*	ProgramDir	- директория с файлом программы
*	pFunc		- указатель на внешнюю функцию обработки команд
********************************************************************************/
bool CCommandWindow::InitCommandWindow(ERROR_Class *pError,  CString ProgramDir)
{
	ASSERT(pError);

	m_ProgramDir=ProgramDir;
	m_pError=pError;

	//ДОБАВЛЮ КОМАНДЫ КОТОРЫЕ ЯВЛЯЮТСЯ ВНУТРЕННИМИ
	AddString(_T("mac [File_Name/ch]"));		//выполняет макрос
	AddString(_T("FP32 [var_name]"));			//создает переменную с плавающей точкой
	return true;
}

/*******************************************************************************
*				ОБРАБОТКА СООБЩНЕИЙ
*******************************************************************************/
BOOL CCommandWindow::PreTranslateMessage(MSG *pMsg)
{
	int k=0;
	switch(pMsg->message)
	{
	case WM_KEYDOWN:	//была нажата кнопка
		switch(pMsg->wParam)
		{
		case VK_SPACE:
			if(GetAsyncKeyState(VK_CONTROL) & 0x8000)
			{
				AfxMessageBox(_T("adsg"));
			}
			break;
		case VK_UP:
			//подставим предыдущую команду
			SetWindowText(PreviousCommand);	
			//чтобы курсор мигал в конце 
			SetEditSel(PreviousCommand.GetLength(),PreviousCommand.GetLength());
			return true;
		case VK_RIGHT:
			{//при нажатии на эту кнопку мы подставим в поле для ввода
			//верхнюю ячейку из выпадающего листа

				int ltop=GetCurSel();	//первым в приоритете идет выделенная ячейка
				if(ltop==CB_ERR)
				{//если выделенных элементов нет - займемся первым сверху
					ltop=GetTopIndex();
				}
				if(ltop==CB_ERR) break;
				CString str;
				GetLBText(ltop,str);
				SetWindowText(str);
				//чтобы курсор мигал в конце 
				unsigned char lpos=str.Find('[');	//попытаемся выделит область описания аргументов
				if(lpos==-1)lpos=str.GetLength();
				SetEditSel(lpos,str.GetLength());
				return true;
			}
		case VK_DOWN:
			//в стандартном исполнении если выпадающий список закрыт при 
			//нажатии на эту кнопку подставляется очередная команда
			//И ТУТ ЖЕ ВЫПОЛНЯЕТСЯ,	что нас не устраивает
			if(!GetDroppedState())
			{
				ShowDropDown(true);	//покажем выпадающий список
				return true;	
			}
			break;
		case VK_RETURN:
			{//ЗАКОНЧИЛИ ВВОДИТЬ КОМАНДУ
				//получим текст, который мы набрали
				CString lAllString;			//это значение оставим в целости и сохранности на будущее
				GetWindowText(lAllString);

				ShowDropDown(false);		//закроем пасть

				//обработаем комманду
				bool lAllRight=PreTranslateCommand(lAllString);
				
				if(lAllRight)
				{//все прошло нормально
					SetWindowText(_T(""));			//затрем окно послу успешного выполнения команды
					PreviousCommand=lAllString;		//сохраним эту команду
				}
				return true;						//не буду передавать Enter в родительское окно
			}
		}
	}
	return CComboBox::PreTranslateMessage(pMsg);
}

/*************************************************
* Если нам из командной строки нужно извлечь булеву
*	переменную, то это сюда. Командная строка урезается
***************************************************/
bool CCommandWindow::GetBOOLArgument(CString *pstr, bool *ok)
{
	//Если функция отработала неверно,
	//то в *lok записывается ложь, в противном случае
	//значение не изменяется

	pstr->TrimLeft(' ');	//обрежем пробелы в начале
	if(pstr->GetLength()==0)
	{//там точно ничего нет
		*ok=false;			//оповестим о неверной расшифровке
		return false;
	}
	CString larg;
	unsigned char lpos=pstr->Find(' ');
	if(lpos==-1)
	{//вся строка - аргумент без пробелов
		lpos=pstr->GetLength()-1;
	}
	larg=pstr->Left(lpos); //выделим аргумент в строке
	pstr->Delete(0,lpos);	//удалим из строки считанный оргумент

	if(larg==_T("true") || larg==_T("ok") || larg==_T("on") || larg==_T("yes"))
	{
		return true;
	}
	else if(larg==_T("false") || larg==_T("off") || larg==_T("no"))
	{
		return false;
	}
	else
	{//неопознанный аргумент
		*ok=false;	//оповестим о неверной расшифровке
		return false;
	}
}

/********************************************************
*	Если из командной строки нужно извлечь число, то
*	вызывается эта функция
*******************************************************/
float CCommandWindow::GetFLOATArgument(CString *pstr, bool *ok)
{
	//Если функция отработала неверно,
	//то в *lok записывается ложь, в противном случае
	//значение не изменяется

	pstr->TrimLeft(' ');	//обрежем пробелы в начале
	if(pstr->GetLength()==0)
	{//там точно ничего нет
		*ok=false;
		return 0;
	}
	CString larg;
	unsigned char lpos=pstr->Find(' ');
	if(lpos==-1)
	{//вся строка - аргумент без пробелов
		lpos=pstr->GetLength()-1;
	}
	larg=pstr->Left(lpos);	//выделим аргумент в строке
	pstr->Delete(0,lpos);	//удалим из строки считанный оргумент

	//проверим, может это и не константа вовсе, а переменная
	if(FP32_map.find(larg)!=FP32_map.end())
	{//да, такая переменная есть
		return FP32_map[larg];	//вернем ее значение
	}

	return (float)_wtof(larg.GetBuffer());
}

/***************************************************************
*	Чтобы не проверять, является ли это число целым, можно вызвать
*	эту функцию. Она реагирует только на целые числа
*****************************************************************/
INT32S CCommandWindow::GetINTArgument(CString *pstr, bool *ok)
{
	//Если функция отработала неверно,
	//то в *lok записывается ложь, в противном случае
	//значение не изменяется

	FP32 temp=GetFLOATArgument(pstr, ok);	//сначала считаем что можно
	if(*ok)
	{
		if(temp-((INT32S)temp)!=0)
		{//это не целое число
			*ok=false;
			CString str; str.Format(_T("Командная строка: аргумент \"%f\" не целое число"), temp);
			m_pError->SetERROR(str, ERP_WARNING | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
			return 0;
		}
	}
	return (INT32S)temp;
}

/***************************************************************
*	То же, что и GetINTArgument, но проверяется на положительное 
*	целое
*****************************************************************/
INT32U CCommandWindow::GetUINTArgument(CString *pstr, bool *ok)
{
	FP32 temp=GetFLOATArgument(pstr, ok);	//сначала считаем что можно
	if(*ok)
	{
		if(temp>=0 && (temp-((INT32S)temp)!=0))
		{//это не целое число
			*ok=false;
			CString str; str.Format(_T("Командная строка: аргумент \"%f\" не положительное целое число"), temp);
			m_pError->SetERROR(str, ERP_WARNING | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
			return 0;
		}
	}
	return (INT32U)temp;
}

/*******************************************************************
*	Считыает из строки строковый аргумент
********************************************************************/
CString CCommandWindow::GetSTRINGArgument(CString *pstr, bool *ok)
{
	//Если функция отработала неверно,
	//то в *lok записывается ложь, в противном случае
	//значение не изменяется

	pstr->TrimLeft(' ');	//обрежем пробелы в начале
	if(pstr->GetLength()==0)
	{//там точно ничего нет
		*ok=false;
		return _T("");
	}

	CString larg;	//сюда будет помещен итоговый результат
	INT8U	lpos;	//номре символа - конца строкового аргумента

	//Строковый аргумент может иметь пробелы, тогда его нужно заключать в кавычкм
	if((*pstr)[0]=='"')
	{//да, аргумент в кавычках
		pstr->TrimLeft('"');	//удалим уже найденную кавычку
		lpos=pstr->Find('"');
	}
	else
	{//аргумент заканчивается пробелом
		lpos=pstr->Find(' ');
		if(lpos==-1)
		{//вся строка - аргумент без пробелов
			lpos=pstr->GetLength()-1;
		}
	}
	
	larg=pstr->Left(lpos); //выделим аргумент в строке
	pstr->Delete(0,lpos);	//удалим из строки считанный оргумент
	if((*pstr)[0]=='"')
	{//в конце строки осталась еще кавычка
		pstr->TrimLeft('"');	//удалим кавычку
	}

	return larg;
}

bool CCommandWindow::ReadMacros(CString filename)
{
	ifstream lfile;
	if(filename[1]!=':')
	{//указано только имя, без глобального пути
		//добавим директорию
		filename=m_ProgramDir+_T("\\")+filename;
	}

	lfile.open(filename, ios_base::binary);
	if(!lfile.is_open())
	{
		AfxMessageBox(_T("Не удалось открыть файл макроса ")+filename);
		return false;
	}

	while(!lfile.eof())
	{//последовательно считаем все команды из файла
		char lline[250];
		lfile.getline(lline,250);
		USES_CONVERSION;
		CString str=A2T(lline);
		str.TrimRight(0x0d);	//обрежем символ переноса строки
		if(str==_T(""))
		{//просто пустая строка
			continue;
		}
		//обработаем комманду
		if(!PreTranslateCommand(str))
		{
			return false;
		}
	}
	return true;	//все прошло нормлаьно
}

/*********     Create_FP32_var(CString var_name, FP32 var_value)   **************
*		Функция создает переменную с плавающей точкой из макроса.
*	var_name	- имя перменнной
*	var_value	- начальное значение
********************************************************************************/
bool CCommandWindow::Create_FP32_var(CString var_name, FP32 var_value)
{
	if(FP32_map.find(var_name)==FP32_map.end())
	{//такой переменной еще нет
		FP32_map[var_name]=var_value;
	}
	else
	{
		CString str; str.Format(_T("Попытка переопределить переменную %s"),var_name);
		m_pError->SetERROR(str,ERP_WARNING | ER_WRITE_TO_LOG);
		return false;
	}
	return true;
}

/**************************    Set_FP32_var    **********************************
*		Присваивает переменной с плавающей точкой из макроса какое-то значение
********************************************************************************/
bool CCommandWindow::Set_FP32_var(CString var_name, FP32 var_value)
{
	if(FP32_map.find(var_name)!=FP32_map.end())
	{//переменная существует
		FP32_map[var_name]=var_value;
	}
	else
	{
		CString str; str.Format(_T("Попытка присвоить значение несуществующей переменной %s"),var_name);
		m_pError->SetERROR(str,ERP_WARNING | ER_WRITE_TO_LOG);
		return false;
	}
	return true;
}

/*************************    OnCbnSelendok()     ***********************************
*		Вызывается после выбора в списке какого-то элемента. Но мне здесь делать
*	нечего.
************************************************************************************/
void CCommandWindow::OnCbnSelendok()
{
	
}

/************************    OnCbnEditchange()    ***********************************
*		Пользователь ввел что-то новое.
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
*		Производит внутреннюю обработку поступившей комманды (вдруг есть команды, 
*	которые нужно обработать здесь).
*	и если нодо отправляет родителю
*************************************************************************************/
bool CCommandWindow::PreTranslateCommand(CString AllString)
{
	//собственно выполение команды
	CWnd* ptr=GetParent();

	CString lCommand;	//сюда отделится все, что касается имени команды
	CString lArgString;	//а сюда аргументы этой функции

	//ВЫРЕЖЕМ КОММЕНТАРИИ
	INT16U a=AllString.Find(_T("//"));
	AllString=AllString.Left(a);	//оставим все, что до комментариев
	AllString.TrimLeft(' ');							//вырежем пробелы
	if(AllString==_T(""))
	{//вся строка была комментарием
		return true;	//но все же это правильно
	}

	//ОСТАВИМ СЛОВО КОМАНДЫ
	char lnum=AllString.Find(' ');
	if(lnum!=-1)
	{//после слова команды есть что-то еще
		lCommand=AllString.Left(lnum);
		lArgString=AllString.Right(AllString.GetLength()-lnum);
	}
	else
	{//всё, что ввели можно считать полноценной командой
		lCommand=AllString;
		lArgString=_T("");
	}

	bool lAllRight=false;

	//ПРЕДВАРИТЕЛЬНАЯ ОБРАБОТКА ИЗВЕСТНЫХ КОМАНД 
	if(lCommand==_T("mac"))
	{//НУЖНО ВЫПОЛНИТЬ МАКРОС
		bool lok=true;
		//название файла
		CString lFileName=GetSTRINGArgument(&lArgString, &lok);
		if(lok)
		{
			if(lFileName==_T("ch"))
			{//пользователь хочет выбрать файл
				// создание стандартной панели выбора файла Open
				CFileDialog DlgOpen(FALSE,(LPCTSTR)_T("txt"),NULL,NULL,(LPCTSTR)_T("Макрос (*.txt) |*.txt||"));

				// отображение стандартной панели выбора файла Open
				if(DlgOpen.DoModal()==IDCANCEL)
				{//пользователь не захотел работать дальше
					return true;
				}

				lFileName=DlgOpen.GetPathName();
			}
			else
			{//имя файла задано
				lFileName.Append(_T(".txt"));	//расширение дописывается автоматически
			}
			if(ReadMacros(lFileName))
			{
				CString str; str.Format(_T("Макрос %s успешно выполнен"), lFileName);
				m_pError->SetERROR(str,ERP_GOOD);
			}
			lAllRight=true; //в целом команда правильная
		}
	}
	else if(lCommand==_T("FP32"))
	{//создать переменную с плавающей точкой
	//пока все на соплях
		bool lok=true;
		CString lVarName=CCommandWindow::GetSTRINGArgument(&lArgString, &lok);
		if(lok)
		{
			ReCreate_FP32_var(lVarName, 0);
			lAllRight=true; //в целом команда правильная
		}
	}
	else if(lCommand==_T("SetVarValue"))
	{//задает переменной значение
		bool lok=true;
		CString lVarName=GetSTRINGArgument(&lArgString, &lok);
		//!!!Переделать под разные типы !!!!
		FP32 lVarValue=GetFLOATArgument(&lArgString, &lok);
		if(lok)
		{
			Set_FP32_var(lVarName,lVarValue);
			lAllRight=true;				//в целом команда правильная
		}
	}
	else
	{//НАМ КОМАНДА НЕИЗВЕСТНА, ПОПЫТАЕМ СЧАСТЬЮ У РОДИТЕЛЬСКОГО ОКНА
		lAllRight=ptr->SendMessage(MSG_NEW_COMMAND,(WPARAM)lCommand.GetBuffer(),(LPARAM)lArgString.GetBuffer())==0?false:true;
	}

	if(!lAllRight)
	{
		m_pError->SetERROR(_T("Неизвестный формат команды \"")+AllString+_T("\""), ERP_WATCHFUL | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
	}
	return lAllRight;
}
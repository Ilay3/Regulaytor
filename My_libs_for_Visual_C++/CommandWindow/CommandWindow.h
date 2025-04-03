/*******************************************************************************
********************************************************************************
**		Это класс коммандного окна, он обрабатывает введенные пользователем 
**	команды (с подсказками) и отсылает сообщение MSG_NEW_COMMAND родительскому
**	окну, чтобы оно завершило выполнение команды.
********************************************************************************
*******************************************************************************/

/******************************************************************************
*						ЛИСТ РЕГИСТРАЦИИ ИЗМЕНЕНИЙ
*	17.12.2007 - большая переделка, еще больше приблизились к библиотечности
*	19.12.2007 - при неправильной обработке команды создано предупреждение
*	20.11.2007 - введена обработка внутренних команд, дочерним окнам передается
*					команда и ее аргумент отдельно.
*	23.09.2008 - поддержка UNICODE
******************************************************************************/
#pragma once
#include <types_visual_c++.h>	//файл с моими типами
#include <ERRORS\ERRORS.h>		//класс уведомления об ошибках и сообщениях
#include <fstream>				//для работы с файлами
#include <map>					//для работы со словарями
#include <AtlConv.h>			//для работы с UNICODE

using namespace std;

#define MSG_NEW_COMMAND (WM_USER+30)	//это сообщение отправляется, когда поступула очередная ошибка
//ДЕЙСТВИЯ ПО ДОБАВЛЕНИЮ ОБРАБОТЧИКА
//	1. Добавить ON_MESSAGE(MSG_NEW_COMMAND,InterpretMessage)
//	2. Добавить LRESULT ДИАЛОГ::InterpretMessage(WPARAM pCommand, LPARAM pArgString)
//	3. pCommand указатель на строку с коммандой типа TCHAR
//	4. Работать


//СОБСТВЕННО КЛАСС КОМАНДНОГО ОКНА
class CCommandWindow: public CComboBox
{
	BOOL PreTranslateMessage(MSG *pMsg);
	
	ERROR_Class *m_pError;			//все существенные ошибки в классе пишутся сюда

	
	//хранит последнюю удачно завершившуюся команду
	CString PreviousCommand;
public:

	//инициализация коммандного окна
	bool InitCommandWindow(ERROR_Class *pError, CString ProgramDir);

	//я решил, что неплохо бы запомнить директорию, с которой стартовала программа
	CString m_ProgramDir;	

	//это контейнер, которые содержит все переменные с плавающей точкой в макросе
	map<CString,FP32,less<CString> > FP32_map;

	//создает переменную с плавающей точкой
	bool Create_FP32_var(CString var_name, FP32 var_value);
	
	//Если такая переменная уже существует, то не выдает ошибки
	bool ReCreate_FP32_var(CString var_name, FP32 var_value)
	{
		FP32_map[var_name]=var_value;
		return true;
	}
	
	//присваивает перменной с плавающей точкой какое-то значение
	bool Set_FP32_var(CString var_name, FP32 var_value);

	static bool GetBOOLArgument(CString *pstr, bool *ok=NULL);
	float GetFLOATArgument(CString *pstr, bool *ok=NULL);
	INT32S GetINTArgument(CString *pstr, bool *ok=NULL);
	INT32U GetUINTArgument(CString *pstr, bool *ok=NULL);
	static CString GetSTRINGArgument(CString *pstr, bool *ok=NULL);
	bool ReadMacros(CString filename);
public:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelendok();
public:
	afx_msg void OnCbnEditchange();

	//Производит внутреннюю обработку поступившей комманды
	//и если нодо отправляет родителю
	bool PreTranslateCommand(CString AllString);
};
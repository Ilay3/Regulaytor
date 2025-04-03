/******************************************************************************************
*********************************************************************************************
**		Этот модуль позволяет редактировать отображение каждой функции в отдельности
**	Функция задана в рамках структуры CFunctionClass
*********************************************************************************************
*********************************************************************************************/

/*******************************	ЛИСТ РЕГИСТРАЦИИ ИЗМЕНЕНИЙ	****************************
*	30.11.2006 - создан
*	5.1.2007 - добавлена возможность выбора цвета и единицы измерения функциии, исправлен глюк,при
*				котором после повторного включения окна функцию было трудно выбрать правильно
*	27.02.2007 - теперь нет переполнения при злоумышленном залипании клавиши на вводе пропорциональ-
*					ного коэффициента.
*********************************************************************************************/


#pragma once
#include "afxwin.h"
#include "FunctionClass\FunctionClass.h"

#include "DlgFuncName.h"	//диалог с вводом имени функции
#include "resource.h"


//продекларируем здесь класс главного окна графика, чтобы не возиться с подключением графика
class CGRAPH_Dlg;

class CDlgFuncOptions : public CDialog
{
	DECLARE_DYNAMIC(CDlgFuncOptions)

public:
	CDlgFuncOptions(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgFuncOptions();

	enum { IDD = IDD_FUNC_OPT };

	// переопределим для защиты
	void CreateMAIN(UINT str, CGRAPH_Dlg * parent);
	
	// указатель на родителя (главное окно графика)
	CGRAPH_Dlg* GRAPH_WINDOW;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	//этот указатель будет указывать на обрабатываемую функци
	CFunctionClass* m_pCurrentFunc;
public:
	//Такая ситуация - мне нужно показать окно, но я точно знаю, какая функция выбрана
	//При этом перед показам окна я должен запистать сюда номер трубуемой функции
	// -1 - я не хочу пользоваться этой переменной
	INT32S		m_PrimaryFunctionNum;
private:

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	
	//список, по которому мы выбираем какую функцию редактировать
	CComboBox m_CurrentFunc;
	afx_msg void OnCbnSelendokCurrentFunc();

	//список, с типами элементов отображения функции
	CComboBox m_ChartType;


	virtual BOOL OnInitDialog();
public:
	afx_msg void OnCbnSelendokFuncType();
	// источник коэффициента пропорциональности
	CEdit m_ProporCoef;
	afx_msg void OnEnChangeProporCoef();
	afx_msg void OnBnClickedColor();
	afx_msg void OnBnClickedName();
	afx_msg void OnEnChangeUnit();
	// единица измерения
	CEdit m_Unit;
public:
	// Источник свойства Always Down
	CButton m_AlwaysDown;

	//изменили свойство Always Down
	afx_msg void OnBnClickedAlwaysDown();

	//изменили толщину линии функции
	afx_msg void OnDeltaPosLineWidth(NMHDR *pNMHDR, LRESULT *pResult);
};

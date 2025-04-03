/**************************************************************************
***************************************************************************
**		��� ������� ������ ��� ���������� ���� � �� �� ������� ����������� ������,
**	������� � ����� ������� �� ��� � ��������� �����.
***************************************************************************
**************************************************************************/

/*************************************************************************
*					���� ����������� ���������
*	12 ������ 2008 - ������� ������� PreSubclassWindow
*************************************************************************/
#pragma once
#include "Types_Visual_c++.h"		//���� � ����� ������
#include "ColorEdit\ColorEdit.h"	//����� ������� ��������

// CErrorEdit

#pragma once
class CErrorEdit : public CColorEdit
{
	DECLARE_DYNAMIC(CErrorEdit)

public:
	CErrorEdit();
	virtual ~CErrorEdit();

	//���������� � ���� ����� ���������
	bool SetError(CString error, INT8U priority);
	//������� ���� ���������
	void ClearErrorWindow();
protected:
	DECLARE_MESSAGE_MAP()
	//������ �� ������������ ������� ����� ������
	const static UINT_PTR	NEW_ERR_SIGNAL_ID=0x199;
	//������������ �������
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void PreSubclassWindow();
public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
};



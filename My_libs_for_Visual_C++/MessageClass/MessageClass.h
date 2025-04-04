/**************************************************************************************
**************************************************************************************
**		� ���� ������������ ����� ���������� ��������� ��������� ������������� �� ����.
**	� ��������, ��� �������� �� ���� ����������, �.�. �������� ��� ��, ��� ����� ������-
**	����, ���� ��������� �������� ���� (�������� ��� ���������)
**************************************************************************************
**************************************************************************************/

/**********************	���� ����������� ���������	**********************************
*	14.04.2007 - ��������� ���� �� ����� ���������
*	3.08.2008 - �������� �������� �����������, �������� ����
**************************************************************************************/
#pragma once
#include "Types_Visual_c++.h"	//��� ���� ������
;
struct CMessage
{
public:
	CMessage(){text=NULL; length=0;}
	CMessage(INT8U* pBuf){text=pBuf; length=0;}
	INT8U		  * text;		//��������� �� ����� � ������������ �������
    unsigned int	peredacha;	//�����, ���������� �� ��������
	unsigned int	priem;		//�����, ���������� ��� ������
	unsigned int	nomer;		//����� �������
	bool			type;		//��� ������� - ���� ����� ������ 7 ��� 8
	INT8U			summa;		//����������� ����� (������ ��������� ������������ ���� ������ �������
	INT16U			length;		//����� ��������� (����� ���������� ���� ��������)
	CString			name;		//��� ������, ������� ����� ������������� � ������ ���������
};
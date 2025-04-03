#include "stdafx.h"
#include "ByteStuffing.h"

bool RecompileMessageWithBytestuffing(CMessage * pMes, INT8U ByteForFind, INT8U ByteForReplace)
{
	// pMes - ��������� �� ��������� ������� ����� ����������
	// ByteForFind - ����, ������� ������� ����
	// ByteForReplace - ����, ������� ������� ��������� �� ByteForFind
	// ���� ��� ��������� - ������� ���������� true, ����� false

	if(pMes==NULL || pMes->text==NULL)
	{//������� ���������
		return false;
	}
	if(pMes->length<=0)
	{//������� �����
		return false;
	}
	for(INT32S i=1; i<pMes->length-1; i++)
	{//�������� ��� ������� ����� ������� � ����������
		unsigned char tmp= pMes->text[i];
		if(tmp==ByteForFind)
		{//��� ����� ��������� "��������� ����"
			for(INT32S j=pMes->length;j>i;j--)
			{//��������� ����� ������� �������������
				pMes->text[j+1]=pMes->text[j];
			}
			pMes->text[i+1]=ByteForReplace;	//������� ����� ������
			pMes->length++;					//�������� �����

		}
	}

	return true;	//��� ������ ���������
}

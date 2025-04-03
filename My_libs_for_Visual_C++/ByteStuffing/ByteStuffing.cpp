#include "stdafx.h"
#include "ByteStuffing.h"

bool RecompileMessageWithBytestuffing(CMessage * pMes, INT8U ByteForFind, INT8U ByteForReplace)
{
	// pMes - указатель на сообщение которое нужно подправить
	// ByteForFind - байт, который функция ищет
	// ByteForReplace - байт, который функция вставляет за ByteForFind
	// Если все нормально - функция возвращает true, иначе false

	if(pMes==NULL || pMes->text==NULL)
	{//нулевые указатели
		return false;
	}
	if(pMes->length<=0)
	{//нулевая длина
		return false;
	}
	for(INT32S i=1; i<pMes->length-1; i++)
	{//проверим все символы кроме первого и последнего
		unsigned char tmp= pMes->text[i];
		if(tmp==ByteForFind)
		{//это самый настоящий "нехороший байт"
			for(INT32S j=pMes->length;j>i;j--)
			{//освободим место символу байтстаффинга
				pMes->text[j+1]=pMes->text[j];
			}
			pMes->text[i+1]=ByteForReplace;	//вставим новый символ
			pMes->length++;					//нарастим длину

		}
	}

	return true;	//все прошло нормально
}

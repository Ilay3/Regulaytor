#include "stdafx.h"
#include "LobanovData.h"	

/**************************	   SV_to_float(unsigned char * pData)    ********************
*	Конвертация из данных СВ в обычный float. pData - указатель на последовательность из 
*	трех байт в формате Сергей Владимировича.
******************************************************************************************/
float SV_to_float(unsigned char * pData)
{
	float k;

	//Если число < 0, то нужно
	//проводить дополнительные махинации с битами
	bool lNeedToSign=false;
	if(pData[0] & 0x80)
	{//это отрицательное число
		lNeedToSign=true;
		//1. Инвертирую все 3 байта
		pData[0]=~pData[0];
		pData[1]=~pData[1];
		pData[2]=~pData[2];
		//2. Прибавляю 1 и учитываю перенос
		if(pData[2]==0xFF)
		{
			pData[2]=0x00;
			if(pData[1]==0xFF)
			{
				pData[1]=0x00;
				pData[2]++;
			}
			else pData[1]++;
		}
		else pData[2]++;
	}

	k = (float)(pData[1]*256. + pData[2]);

	while(pData[0] > 80)
	{
		k*=2.;
		pData[0]--;
	}

	while(pData[0] < 80)
	{
		k/=2.;
		pData[0]++;
	}

	//если нужно - поменяю знак
	if(lNeedToSign) k=-k;

	return k;
}

/*************************   float_to_SV(float Data)   ************************************
*	Конвертация из обычного float в формат СВ. Результатом является указатель на массив из
*	трех байт
*	!!!!! Работает только с положительными числами !!!!!
********************************************************************************************/
unsigned char * float_to_SV(float Data)
{
	static unsigned char buf[3];	//чтобы не было геморроя и утечек памяти
									//однако теперь полученные значения внешняя программа
									//должна будет побыстрее скопировать

	//Если Data<0, также
	//нужно принудительно менять знаковый бин в конце вычислений
	bool lNeedToSign=false;
	if(Data<0)
	{
		Data=-Data;
		lNeedToSign=true;
	}

	if(Data == 0)
	{
		buf[0]=0;
		buf[1]=0;
		buf[2]=0;
		return buf;
	}

	buf[0]=80;
	buf[1]=0;
	buf[2]=0;

	while(Data > 65535.)
	{
		buf[0]++;
		Data/=2.;
	}

	while(Data < 32768.)
	{
		buf[0]--;
		Data*=2.;
	}

	if(Data >= 32768.)
	{
		Data-= 32768.;
		buf[1]+=128;
	}
	if(Data >= 16384.)
	{
		Data-= 16384.;
		buf[1]+=64;
	}
	if(Data >= 8192.)
	{
		Data-= 8192.;
		buf[1]+=32;
	}
	if(Data >= 4096.)
	{
		Data-= 4096.;
		buf[1]+=16;
	}
	if(Data >= 2048.)
	{
		Data-= 2048.;
		buf[1]+=8;
	}
	if(Data >= 1024.)
	{
		Data-= 1024.;
		buf[1]+=4;
	}
	if(Data >= 512.)
	{
		Data-= 512.;
		buf[1]+=2;
	}
	if(Data >= 256.)
	{
		Data-= 256.;
		buf[1]+=1;
	}
	if(Data >= 128.)
	{
		Data-= 128.;
		buf[2]+=128;
	}
	if(Data >= 64.)
	{
		Data-= 64.;
		buf[2]+=64;
	}
	if(Data >= 32.)
	{
		Data-= 32.;
		buf[2]+=32;
	}
	if(Data >= 16.)
	{
		Data-= 16.;
		buf[2]+=16;
	}
	if(Data >= 8.)
	{
		Data-= 8.;
		buf[2]+=8;
	}
	if(Data >= 4.)
	{
		Data-= 4.;
		buf[2]+=4;
	}
	if(Data >= 2.)
	{
		Data-= 2.;
		buf[2]+=2;
	}
	if(Data >= 1.)
	{
		Data-= 1.;
		buf[2]+=1;
	}

	if(lNeedToSign)
	{//нужно менять знак
		//1. Инвертирую все 3 байта
		buf[0]=~buf[0];
		buf[1]=~buf[1];
		buf[2]=~buf[2];
		//2. Прибавляю 1 и учитываю перенос
		if(buf[2]==0xFF)
		{
			buf[2]=0x00;
			if(buf[1]==0xFF)
			{
				buf[1]=0x00;
				buf[2]++;
			}
			else buf[1]++;
		}
		else buf[2]++;
	}

	return buf;
}

/***********************************************************************
*	ЦЕЛОЧИСЛЕННАЯ АРИФМЕТИКА
*************************************************************************/

//двухбайтное целое старшим байтом вперед
INT16U SV2BUINT_to_int_ST(unsigned char * pData)
{
	unsigned int temp=0;
	char *ptemp=(char*)&temp;
	ptemp[0]=pData[1];	//младший байт нормального числа
	ptemp[1]=pData[0];	

	return temp;
}

void int_to_SV2BUINT_ST(INT16U Data, INT8U* pBuf)
{
	//Data - указатель на нормальное число
	//pBuf - указатель на буфер, куда нужно записать байты в обратном порядке
	*pBuf=Data>>8;
	pBuf++;
	*pBuf=(INT8U)Data;
}


/***********************************************************************
*	Сергей Владимирович передает все старшим байтом вперед
***********************************************************************/
unsigned int SV3BUINT_to_int_ST(unsigned char * pData)
{
	unsigned int temp=0;
	char *ptemp=(char*)&temp;
	ptemp[0]=pData[2];	//младший байт нормального числа
	ptemp[1]=pData[1];	
	ptemp[2]=pData[0];	

	return temp;
}

unsigned char * int_to_SV3BUINT_ST(unsigned int Data)
{
	//старший байт бесследно пропадает!!!!
	static unsigned char buf[3];
	char *ptemp=(char*) &Data;
	buf[0]=ptemp[2];
	buf[1]=ptemp[1];
	buf[2]=ptemp[0];

	return buf;
}

/**************************************************************************
*	Младшим байтом вперед
***************************************************************************/
unsigned int SV3BUINT_to_int_ML(unsigned char * pData)
{
	unsigned int temp=0;
	char *ptemp=(char*)&temp;
	ptemp[0]=pData[0];	//младший байт нормального числа
	ptemp[1]=pData[1];	
	ptemp[2]=pData[2];	

	return temp;
}

INT8U * int_to_SV3BUINT_ML(INT32U Data)
{
	//старший байт бесследно пропадает!!!!
	static INT8U buf[3];
	char *ptemp=(char*) &Data;
	buf[0]=ptemp[0];
	buf[1]=ptemp[1];
	buf[2]=ptemp[2];

	return buf;
}
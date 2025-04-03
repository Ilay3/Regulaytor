#include "stdafx.h"
#include "FunctionClass.h"
#include <AtlConv.h>			//для перобразований unicode<->ANSI

/******************************************************************************************
*	     КОНСТРУКТОР		КОНСТРУКТОР		КОНСТРУКТОР		КОНСТРУКТОР		КОНСТРУКТОР
//Не знаю почему, но если конструктор расположить в срр файле, то при выполеннии 
//release проекта visual c++ выдает исключение
*******************************************************************************************/
CFunctionClass::CFunctionClass()
{
		ProporCoef=1;				//нет коэффициента пропорциональности
		ChartType=G_LINECHART;		//график - непрерывная линия
		szFunctionName=_T("No named");	//пока нету имени
		szYLegend=_T("no");			//нет единицы измерения
		szXLegend=_T("Время, с");	//подпись по оси Х
		Visible=false;				//не будем ее пока показывать
		AlwaysDown=false;			//отключим свойство отображения внизу экрана
		Color=RGB(255,0,0);			//цвет графика
		ColorEx=RGB(0,255,0);		//цвет для всяких дополнительных нужд
		LineWidth=1;				//толщина линии один пиксель
		LineType=0;					//пока не реализовано
}

/********************************************************************************************
*		ДЕСТРУКТОР		ДЕСТРУКТОР		ДЕСТРУКТОР		ДЕСТРУКТОР
********************************************************************************************/
CFunctionClass::~CFunctionClass()
{
}

/******************************** AddGraphWhithTime (float var, float time) *************************
*		Эта функция аналогична предыдущей, однако время заводится пользователем в качестве аргумента,
*	а не выключается автоматически.
******************************************************************************************************/
bool CFunctionClass::AddGraphWhithTime (FP32 var, FP32 time)
{	//!!!!! time - в СЕКУНДАХ!!!!!
	//if(m_StopALL)
	//{//построение остановлено
	//	return false;
	//}
	pPlotXYItems.push_back(CDPoint(time,var));//добавляем новую точку
	return true;
}

/**********************	GetMaxValue()   GetMinValue()	****************************************
*	Эти две функции возвращают максимальное значение и минимальное значение (соответственно) 
*	из всех точек, которые содержит данная функция, что полезно, для автоматического определения
*	границ просмотра на графике
*		FromTime и ToTime - временные границы, в которых ищется значение
*	Если функции пустые, или не попадают в заданное поле, то выдается -FLT_MAX или FLT_MAX
*		UseProporCoef - true - при вычислениях учитывается коэффицинет пропорциональности функции
*						false - не учитывается
************************************************************************************************/
FP32 CFunctionClass::GetMaxValue(FP32 FromTime, FP32 ToTime, bool UseProporCoef)
{
	FP32 MAX=-FLT_MAX;	//не знаю, по-моему меньше уже некуда, хотя скользкий вопрос...
	for(size_t i=0; i<pPlotXYItems.size();i++)
	{//элементарнейший алгоритм перебора
		FP32 lValue=pPlotXYItems[i].y;
		if(UseProporCoef) lValue*=ProporCoef;		//если надо, учту коэффициент пропорциональности
		if(lValue>MAX && pPlotXYItems[i].x>=FromTime && pPlotXYItems[i].x<=ToTime)
		{
			MAX=lValue;
		}
	}
	//вернем то, что насчитали
	return MAX;
}
FP32 CFunctionClass::GetMinValue(FP32 FromTime, FP32 ToTime, bool UseProporCoef)
{
	FP32 MIN=FLT_MAX;	//не знаю, по-моему больше уже некуда, хотя скользкий вопрос...
	for(size_t i=0; i<pPlotXYItems.size();i++)
	{//элементарнейший алгоритм перебора
		FP32 lValue=pPlotXYItems[i].y;
		if(UseProporCoef) lValue*=ProporCoef;		//если надо, учту коэффициент пропорциональности
		if(lValue<MIN && pPlotXYItems[i].x>=FromTime && pPlotXYItems[i].x<=ToTime)
		{
			MIN=lValue;
		}
	}
	//вернем то, что насчитали
	return MIN;
}

/**********************	GetMaxTime()   GetMinTime()	****************************************
*	Эти две функции возвращают максимальное значение и минимальное значение (соответственно) ВРЕМЕНИ 
*	при котором присутствуют точки, что полезно, для автоматического определения
*	границ просмотра на графике
************************************************************************************************/
FP32 CFunctionClass::GetMaxTime()
{
	float MAX=-FLT_MAX;	//не знаю, по-моему меньше уже некуда, хотя скользкий вопрос...
	for(size_t i=0; i<pPlotXYItems.size();i++)
	{//элементарнейший алгоритм перебора
		if(pPlotXYItems[i].x>MAX)
		{
			MAX=pPlotXYItems[i].x;
		}
	}
	//вернем то, что насчитали
	return MAX;
}

FP32 CFunctionClass::GetMinTime()
{
	float MIN=FLT_MAX;	//не знаю, по-моему больше уже некуда, хотя скользкий вопрос...
	for(size_t i=0; i<pPlotXYItems.size();i++)
	{//элементарнейший алгоритм перебора
		if(pPlotXYItems[i].x<MIN)
		{
			MIN=pPlotXYItems[i].x;
		}
	}
	//вернем то, что насчитали
	return MIN;
}


/***************************	CutTimeOneFunc	**********************************************
*	эта функция вырезает из графика все значения входящие, или наоборот не входящие в диапазон
************************************************************************************************/
bool CFunctionClass::CutTimeOneFunc(FP32 TimeFrom, FP32 TimeTo, bool reverse)
{	//reverse == true - оставить выделенное, а остальное вырезать
	//reverse == false - все наоборот
	if(TimeTo<TimeFrom)
	{//время до куда почему-то меньше времени откуда
		return false;
	}
	size_t i;
	unsigned long number=0;
	for(i=0;i<pPlotXYItems.size();i++)
	{//последовательно пересмотрим все элементы массива
		if(reverse)
		{//оставить выделенное
			if(pPlotXYItems[i].x<TimeFrom || pPlotXYItems[i].x>TimeTo)
			{//Элемент не попал в нужный диапазон, удалим его
				pPlotXYItems.erase(pPlotXYItems.begin()+number);
				i--;	//т.к. один элемент удалили
			}
			else
			{//нарастим указатель пропущенных элементов
				number++;
			}
		}
		else
		{//выделенное вырезать
			if(pPlotXYItems[i].x>=TimeFrom && pPlotXYItems[i].x<=TimeTo)
			{//Элемент не попал в нужный диапазон, удалим его
				pPlotXYItems.erase(pPlotXYItems.begin()+number);
				i--;	//т.к. один элемент удалили
			}
			else
			{//нарастим указатель пропущенных элементов
				number++;
			}
		}
		
	}
	return true;	//все в порядке
}

/******************************    WriteFunction    ************************************
*		Производит запись функции в поток по последней действующей версии.
*		!!! Формат файла графика описан в файле FileFormats.txt
*		Текущая версия 1.1
****************************************************************************************/
bool CFunctionClass::WriteFunction(ofstream& os)
{
	CString str;
	INT16U len;
	unsigned long data_len;

	os <<'$'; //разделитель полей

	/*Длина имени функции*/
	len=szFunctionName.GetLength();
	if(len>255)	return false;							//длина не может быть больше 255 байт
	os.write((char*)&len,sizeof(INT8U));
	os <<'$';  //разделитель полей
	
	/*Само имя функции*/
	USES_CONVERSION;
	os.write(T2A(szFunctionName.GetBuffer()), len);
	os <<'$'; //разделитель полей
	
	/*Цвет функции*/
	os.write((char *)&Color,sizeof(COLORREF));
	os <<'$'; //разделитель полей
	
	/*Тип прорисовки функции*/
	os.write((char *)&ChartType,sizeof(INT8U));
	os <<'$'; //разделитель полей

	/*Длина единицы измерения функции*/
	len=this->szYLegend.GetLength();
	if(len>255)	return false;							//длина не может быть больше 255 байт
	os.write((char *)&len,sizeof(INT8U));
	os <<'$'; //разделитель полей

	/*Единица измерения функции*/
	os.write( T2A(szYLegend.GetBuffer()), len);
	os <<'$'; //разделитель полей

	/*Ширина линии графика*/
	os.write((char*)&this->LineWidth, sizeof(INT8U));
	os <<'$'; //разделитель полей

	/*Тип линии графика*/
	os.write((char*)&this->LineType, sizeof(INT8U));
	os <<'$'; //разделитель полей

	/*Свойство AlwaysDown*/
	os.write((char*)&this->AlwaysDown, sizeof(INT8U));
	os <<'$'; //разделитель полей

	/*Дополнительный цвет графика ColorEx*/
	os.write( (char *)&this->ColorEx, sizeof(COLORREF));
	os <<'$'; //разделитель полей

	/*Пропорциональный коэффициент*/
	os.write( (char *)&this->ProporCoef, sizeof(FP32));
	os <<'$'; //разделитель полей

	/*Количество элементов (точек) в функции*/
	data_len=(INT64U)this->pPlotXYItems.size();
	//Будем писать длину не в количестве элементов CDPoint, а в байтах
	//это сложнее, но практичнее
	data_len=data_len*2*sizeof(float);
	os.write((char *)&data_len,sizeof(INT64U));
	os <<'$'; //разделитель полей
	
	/*Сами элементы функции (точки)*/
	for(unsigned long i=0;i<this->pPlotXYItems.size();i++)
	{
		os.write((char *)&(this->pPlotXYItems[i].x),sizeof(float));
		os.write((char *)&(this->pPlotXYItems[i].y),sizeof(float));
	}
	os <<'$'; //разделитель полей

	return true;		//все прошло нормально
}


/***************************    ReadFunction_1_0   *************************************
*		Считывает фукнцию из потока. Версия 1.0
****************************************************************************************/
bool CFunctionClass::ReadFunction_1_0(ifstream& is)
{
	char separate=0;
	INT8U len=0;
	char name[255];
	unsigned long data_len;
	
	//Выставим значения по умолчанию, чтобы функции выглядели одинаково
	this->ProporCoef=1; //рисуентся непрерывной линией

	is.read(&separate,1); //прочтем разделитель
	if(separate!='$')
	{
		//наверное функция просто не сохранена, не стоит паниковать
		is.close();
		return false;
	}
	separate=0;	//если будет конец файла, то при чтении seperate не изменяется 

	is.read((char*)&len,sizeof(unsigned char));//прочтем длину имени функции
	
	/*Прочтем длину имени функции*/
	is.read(&separate,1); //прочтем разделитель
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("Обнаружен неверный разделитель после поля длины имени функции, операция прервана"));
		is.close();
		return false;
	}
	separate=0;	//если будет конец файла, то при чтении seperate не изменяется

	/*Прочтем имя функции*/
	is.read(name, len);
	name[len]=0; //сделаем строку оканчивающейся нулем
	this->szFunctionName=name;

	is.read(&separate,1); //прочтем разделитель
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("Обнаружен неверный разделитель после поля имени функции, операция прервана"));
		is.close();
		return false;
	}
	separate=0;	//если будет конец файла, то при чтении seperate не изменяется

	/*прочтем цвет загружаемой функции*/
	is.read((char *)&this->Color, sizeof(COLORREF));

	is.read(&separate,1); //прочтем разделитель
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("Обнаружен неверный разделитель после поля цвета функции, операция прервана"));
		is.close();
		return false;
	}
	separate=0;	//если будет конец файла, то при чтении seperate не изменяется

	/*прочтем тип функции*/
	is.read((char*)&this->ChartType, sizeof(unsigned char));

	is.read(&separate,1); //прочтем разделитель
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("Обнаружен неверный разделитель после поля типа функции, операция прервана"));
		is.close();
		return false;
	}
	separate=0;	//если будет конец файла, то при чтении seperate не изменяется

	/*Прочтем длину единиц измерения функции*/
	is.read((char*)&len,sizeof(unsigned char));//прочтем длину имени функции
	
	is.read(&separate,1); //прочтем разделитель
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("Обнаружен неверный разделитель после поля длины единицы измерения функции, операция прервана"));
		is.close();
		return false;
	}
	separate=0;	//если будет конец файла, то при чтении seperate не изменяется

	/*Прочтем единицу измерения функции*/
	is.read(name,len);
	name[len]=0;
	this->szYLegend=name;

	is.read(&separate,1); //прочтем разделитель
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("Обнаружен неверный разделитель после поля единицы измерения функции, операция прервана"));
		is.close();
		return false;
	}
	separate=0;	//если будет конец файла, то при чтении seperate не изменяется

	/*Прочтем длину В БАЙТАХ поля с данными*/
	is.read((char *) &data_len, sizeof(long));

	is.read(&separate,1); //прочтем разделитель
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("Обнаружен неверный разделитель после поля длины данных, операция прервана"));
		is.close();
		return false;
	}
	separate=0;	//если будет конец файла, то при чтении seperate не изменяется

	data_len=data_len/(2*sizeof(float));

	for (unsigned long i=0; i<data_len;i++)
	{
		//CDPoint point;
		float x,y;
		is.read((char*)&x,sizeof(float));
		is.read((char*)&y,sizeof(float));
		this->pPlotXYItems.push_back(CDPoint(x,y));
	}

	is.read(&separate,1); //прочтем разделитель
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("Обнаружен неверный разделитель после данных, операция прервана"));
		is.close();
		return false;
	}
	separate=0;	//если будет конец файла, то при чтении seperate не изменяется

	this->Visible=true;	//только если мы дошли досюда - функция прочиталась корректно и ее можно показывать

	return true;
}

/***************************    ReadFunction_1_1   *************************************
*		Считывает фукнцию из потока. Версия 1.1
****************************************************************************************/
bool CFunctionClass::ReadFunction_1_1(ifstream& is)
{
	char separate=0;
	INT8U len=0;
	char name[255];
	unsigned long data_len;
	
	is.read(&separate,1); //прочтем разделитель
	if(separate!='$')
	{
		//наверное функция просто не сохранена, не стоит паниковать
		is.close();
		return false;
	}
	separate=0;	//если будет конец файла, то при чтении seperate не изменяется 

	is.read((char*)&len,sizeof(unsigned char));//прочтем длину имени функции
	
	/*Прочтем длину имени функции*/
	is.read(&separate,1); //прочтем разделитель
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("Обнаружен неверный разделитель после поля длины имени функции, операция прервана"));
		is.close();
		return false;
	}
	separate=0;	//если будет конец файла, то при чтении seperate не изменяется

	/*Прочтем имя функции*/
	is.read(name, len);
	name[len]=0; //сделаем строку оканчивающейся нулем
	this->szFunctionName=name;

	is.read(&separate,1); //прочтем разделитель
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("Обнаружен неверный разделитель после поля имени функции, операция прервана"));
		is.close();
		return false;
	}
	separate=0;	//если будет конец файла, то при чтении seperate не изменяется

	/*прочтем цвет загружаемой функции*/
	is.read((char *)&this->Color, sizeof(COLORREF));

	is.read(&separate,1); //прочтем разделитель
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("Обнаружен неверный разделитель после поля цвета функции, операция прервана"));
		is.close();
		return false;
	}
	separate=0;	//если будет конец файла, то при чтении seperate не изменяется

	/*прочтем тип функции*/
	is.read((char*)&this->ChartType, sizeof(unsigned char));

	is.read(&separate,1); //прочтем разделитель
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("Обнаружен неверный разделитель после поля типа функции, операция прервана"));
		is.close();
		return false;
	}
	separate=0;	//если будет конец файла, то при чтении seperate не изменяется

	/*Прочтем длину единиц измерения функции*/
	is.read((char*)&len,sizeof(unsigned char));//прочтем длину имени функции
	
	is.read(&separate,1); //прочтем разделитель
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("Обнаружен неверный разделитель после поля длины единицы измерения функции, операция прервана"));
		is.close();
		return false;
	}
	separate=0;	//если будет конец файла, то при чтении seperate не изменяется

	/*Прочтем единицу измерения функции*/
	is.read(name,len);
	name[len]=0;
	this->szYLegend=name;

	is.read(&separate,1); //прочтем разделитель
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("Обнаружен неверный разделитель после поля единицы измерения функции, операция прервана"));
		is.close();
		return false;
	}
	separate=0;	//если будет конец файла, то при чтении seperate не изменяется

	/*Ширина линии графика*/
	is.read((char*)&this->LineWidth, sizeof(INT8U));
	is.read(&separate,1); //прочтем разделитель
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("Обнаружен неверный разделитель после поля ширины линии графика, операция прервана"));
		is.close();
		return false;
	}
	separate=0;	//если будет конец файла, то при чтении seperate не изменяется

	/*Тип линии графика*/
	is.read((char*)&this->LineType, sizeof(INT8U));
	is.read(&separate,1); //прочтем разделитель
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("Обнаружен неверный разделитель после поля типа линии графика, операция прервана"));
		is.close();
		return false;
	}
	separate=0;	//если будет конец файла, то при чтении seperate не изменяется

	/*Свойство AlwaysDown*/
	is.read((char*)&this->AlwaysDown, sizeof(INT8U));
	is.read(&separate,1); //прочтем разделитель
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("Обнаружен неверный разделитель после поля свойства \"Always Down\" линии графика, операция прервана"));
		is.close();
		return false;
	}
	separate=0;	//если будет конец файла, то при чтении seperate не изменяется

	/*Дополнительный цвет графика ColorEx*/
	is.read( (char *)&this->ColorEx, sizeof(COLORREF));
	is.read(&separate,1); //прочтем разделитель
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("Обнаружен неверный разделитель после поля свойства \"ColorEx\" графика, операция прервана"));
		is.close();
		return false;
	}
	separate=0;	//если будет конец файла, то при чтении seperate не изменяется

	/*Пропорциональный коэффициент*/
	is.read( (char *)&this->ProporCoef, sizeof(FP32));
	is.read(&separate,1); //прочтем разделитель
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("Обнаружен неверный разделитель после поля пропорционального коэффициента функции, операция прервана"));
		is.close();
		return false;
	}
	separate=0;	//если будет конец файла, то при чтении seperate не изменяется

	/*Прочтем длину В БАЙТАХ поля с данными*/
	is.read((char *) &data_len, sizeof(long));

	is.read(&separate,1); //прочтем разделитель
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("Обнаружен неверный разделитель после поля длины данных, операция прервана"));
		is.close();
		return false;
	}
	separate=0;	//если будет конец файла, то при чтении seperate не изменяется

	data_len=data_len/(2*sizeof(float));

	for (unsigned long i=0; i<data_len;i++)
	{
		//CDPoint point;
		float x,y;
		is.read((char*)&x,sizeof(float));
		is.read((char*)&y,sizeof(float));
		this->pPlotXYItems.push_back(CDPoint(x,y));
	}

	is.read(&separate,1); //прочтем разделитель
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("Обнаружен неверный разделитель после данных, операция прервана"));
		is.close();
		return false;
	}
	separate=0;	//если будет конец файла, то при чтении seperate не изменяется

	this->Visible=true;	//только если мы дошли досюда - функция прочиталась корректно и ее можно показывать

	return true;
}

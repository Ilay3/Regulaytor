#include "stdafx.h"
#include "GlavniyClass.h"
#include "VERSION_4Dlg.h"

//*****************************************************************************************
bool GLAVNIY_CLASS_Struct::InitGLAVNIY_CLASS(CVERSION_4Dlg * main_window, ERROR_Class *pError)
{//должна вызываться после присвоения источников
	ASSERT(main_window);	//
	ASSERT(pError);			//на всякий случай
	/*
	Все ошибки внутри сообщения (например неверное значение коэффициентов) обрабатываются внутри
	функций, однако если возникли ошибки программного характера (например с указателями), то функция должна вернуть
	значение false и код ошибки в структуре ERRORS.
	*/
	this->MAIN_WINDOW=main_window;	//скопируем указатель на класс главного окна
	m_pError=pError;				//скопируем указатель на класс ошибки

	bool tmp=true;

	//********************************************
	tmp&=this->Add_UST_Freq_Rot	(false,350);		//!!! см. сноску в соответствующей функции
	tmp&=this->Add_UST_ONLOAD	(false);
	tmp&=this->Add_UST_Run_Stop	(false);

	//*******************************************
	tmp&=this-> Add_Freq_Rot		(false, 0); 
	tmp&=this-> Add_Reyka			(false, 0); 
	tmp&=this-> Add_Oil_Temp		(false, 0); 
	tmp&=this-> Add_Freq_Turbine	(false, 0); 
	tmp&=this-> Add_Nadduv_Pressure	(false, 0); 
	tmp&=this-> Add_Oil_Prot		(0x00); 
	tmp&=this-> Add_Oil_Pressure	(false, 0); 
	tmp&=this-> Add_Nadduv_Limit	(0x00);
	tmp&=this-> Add_Run_Stop		(false);
	tmp&=this-> Add_Raznos			(false, false);

	//период отправики сообщений (мс)
	//!!!! итоговое значение уже загрузилось при загрузке Dlg5 period_of_transfer=1000;	//
	m_ReceiveControl=RECEIVE_CONTROL_THRESHOLD;		//пока не было неотвеченных запросов


	fXStart = 0.0f;
	fdX = 1.0f;
	fA = fB = 0.0f;
	fhmax  = 0.0f;
	fhmaxp = 0.0f;
	fhmaxm = 0.0f;
	fgamma = 0.0f;
	strNumberSensor = _T("0");
	time_date = _T("");

	fdXe = 1.0f;
	fYw = 0.0f; // линейность участка ЭГУ
	fK = 0.0f;  // значение шага, мм/шаг
	fH = 0.0f;  // "0" шкалы нагрузка, мм
	fL = 0.0f;  // расстояние от торца втулки до торца ползуна
	Fix_w = 0;;	 // индекс выбранного окна


	var_EXT_Protocol = false;		// По умолчанию расширенный протокол отключен

	var_UST_Bux = false;


	return (bool)tmp;
}


GLAVNIY_CLASS_Struct::GLAVNIY_CLASS_Struct()
{
	m_GraphRegime=GR_NOTINITIALIZED;
}

GLAVNIY_CLASS_Struct::~GLAVNIY_CLASS_Struct()
{
}

bool GLAVNIY_CLASS_Struct::Add_UST_Freq_Rot ( bool dir, FP32 temp)
{
	// dir = true - поменяли значение в окне программы
	// dir = false - где-то еще и в окне нужно обновить
	CString str;
	if(dir==true)
	{//было изменено значение в окне программы, и поэтому его обновлять не надо
		/*Значение в окне менять НЕЖЕЛАТЕЛЬНО, т.к. существует прямая связь с кнопками режимов передачи,
		которая здесь не обрабатывается*/
		MAIN_WINDOW->Dlg1.m_UST_Freq_Rot.GetWindowText(str);//считаем с экрана
		temp=(FP32)_wtof(str.GetBuffer());//строка в вещественное
		if(temp>=0.f && temp<10000.f)
		{//корректное значение
			this->var_UST_Freq_Rot=temp;
		}
		else
		{//была ошибка, но не тотальная
			MAIN_WINDOW->m_Error.SetERROR(_T("Некорректное значение поля скорости Режим 1, число не обработано"), ERP_WARNING | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
			return true;
		}
	}
	else
	{//нужно еще и обновить в окне
		this->var_UST_Freq_Rot=temp;
		//Не буду париться с форматами, а стану работать с целым числом оборотов
		str.Format(_T("%u"),(INT16U)temp);	str.TrimRight('.');
		MAIN_WINDOW->Dlg1.m_UST_Freq_Rot.SetWindowText(str);//покажем на экране
	}
	return true;
}

/**************************   Add_UST_ONLOAD   ********************************************
*		Двигатель работатет на холостом ходу/под нагрузкой.
*	Проблема в том, что по этому ебанутому протоколу, я не получаю ответ с подтвержением
*	уставки.
******************************************************************************************/
bool GLAVNIY_CLASS_Struct::Add_UST_ONLOAD(bool OnNagr)
{
	//OnNagr = true - дизель работает под нагрузкой
	//OnNagr = false - дизель на холостом ходу
	if(OnNagr)
	{//переводим на работу под нагрузкой
		MAIN_WINDOW->Dlg1.m_UST_Nagr.SetBGColor(BGColorEn);
		MAIN_WINDOW->Dlg1.m_UST_Holost.SetBGColor(BGColorDis);
	}
	else
	{//переводим на работу на холостом ходу
		MAIN_WINDOW->Dlg1.m_UST_Nagr.SetBGColor(BGColorDis);
		MAIN_WINDOW->Dlg1.m_UST_Holost.SetBGColor(BGColorEn);
	}
	var_UST_OnLoad=OnNagr;
	return true;
}

/*************************    Add_UST_Run_Stop    *****************************************
*		Запустить/остановить двигатель
*	Run ==	true - запустить
*			false - остановить
******************************************************************************************/
bool GLAVNIY_CLASS_Struct::Add_UST_Run_Stop(bool Run)
{
	if(Run)
	{//нужно запустить
		MAIN_WINDOW->Dlg1.m_UST_Stop.SetBGColor(BGColorDis);
		MAIN_WINDOW->Dlg1.m_UST_Run.SetBGColor(BGColorUST_Set);
	}
	else
	{//нужно остановить
		/*********
		А теперь еще нужно перевести обратно в первую позицию
		*********/
		MAIN_WINDOW->Dlg1.OnBnClickedPosition_1();
		MAIN_WINDOW->Dlg1.m_UST_Stop.SetBGColor(BGColorUST_Set);
		MAIN_WINDOW->Dlg1.m_UST_Run.SetBGColor(BGColorDis);
	}
		
	var_UST_Run_Stop=Run;
	return true;
}

//*****************************************************************************************
bool GLAVNIY_CLASS_Struct::Add_Freq_Rot ( bool dir, FP32 temp)
{
	// dir = true - поменяли значение в окне программы
	// dir = false - где-то еще и в окне нужно обновить
	CString str;
	if(dir==true)
	{//было изменено значение в окне программы, и поэтому его обновлять не надо
		MAIN_WINDOW->Dlg2.m_Edit_1.GetWindowText(str);//считаем с экрана
		temp=(FP32)_wtof(str.GetBuffer());//строка в вещественное
		if(temp>=0. && temp<10000.)
		{//корректное значение
			this->var_Freq_Rot=temp;
		}
		else
		{//была ошибка, но не тотальная
			MAIN_WINDOW->m_Error.SetERROR(_T("Некорректное значение поля скорости Режим 1, число не обработано"), ERP_WARNING | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
			return true;
		}
	}
	else
	{//нужно еще и обновить в окне
		this->var_Freq_Rot=temp;
		str.Format(_T("%u"),(INT16U)temp);
		MAIN_WINDOW->Dlg2.m_Edit_1.SetWindowText(str);//покажем на экране

		if(GetGraphRegime()==GR_ENABLED && MAIN_WINDOW->pGRAPH_Dlg->m_Graph.GetRegime()!=GRG_STOP)
		{//включено построение графика, нумера функций в InitGraph
			MAIN_WINDOW->pGRAPH_Dlg->m_Graph.VecFunctions[0].AddGraphWhithTime(temp,MAIN_WINDOW->pGRAPH_Dlg->m_Graph.GetOffsetTime());
		}
	}
	return true;
}

bool GLAVNIY_CLASS_Struct::Add_Reyka ( bool dir, FP32 temp)
{
	// dir = true - поменяли значение в окне программы
	// dir = false - где-то еще и в окне нужно обновить
	CString str;
	if(dir==true)
	{//было изменено значение в окне программы, и поэтому его обновлять не надо
		MAIN_WINDOW->Dlg2.m_Edit_2.GetWindowText(str);//считаем с экрана
		temp=(float)_wtof(str.GetBuffer());//строка в вещественное
		if(temp>=0. && temp<10000.)
		{//корректное значение
			this->var_Reyka=temp;
		}
		else
		{//была ошибка, но не тотальная
			MAIN_WINDOW->m_Error.SetERROR(_T("Некорректное значение поля скорости Режим 1, число не обработано"), ERP_WARNING | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
			return true;
		}
	}
	else
	{//нужно еще и обновить в окне
		this->var_Reyka=temp;
		str.Format(_T("%.2f"),temp);
		MAIN_WINDOW->Dlg2.m_Edit_2.SetWindowText(str);//покажем на экране
		if(GetGraphRegime()==GR_ENABLED && MAIN_WINDOW->pGRAPH_Dlg->m_Graph.GetRegime()!=GRG_STOP)
		{//включено построение графика, нумера функций в InitGraph
			MAIN_WINDOW->pGRAPH_Dlg->m_Graph.VecFunctions[1].AddGraphWhithTime(temp,MAIN_WINDOW->pGRAPH_Dlg->m_Graph.GetOffsetTime());
		}
	}
	return true;
}

bool GLAVNIY_CLASS_Struct::Add_Oil_Temp ( bool dir, FP32 temp)
{
	// dir = true - поменяли значение в окне программы
	// dir = false - где-то еще и в окне нужно обновить
	CString str;
	if(dir==true)
	{//было изменено значение в окне программы, и поэтому его обновлять не надо
		MAIN_WINDOW->Dlg2.m_Edit_3.GetWindowText(str);//считаем с экрана
		temp=(float)_wtof(str.GetBuffer());//строка в вещественное
		if(temp>=0. && temp<10000.)
		{//корректное значение
			this->var_Oil_Temp=temp;
		}
		else
		{//была ошибка, но не тотальная
			MAIN_WINDOW->m_Error.SetERROR(_T("Некорректное значение поля скорости Режим 1, число не обработано"), ERP_WARNING | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
			return true;
		}
	}
	else
	{//нужно еще и обновить в окне
		this->var_Oil_Temp=temp;
		str.Format(_T("%.2f"),temp);
		MAIN_WINDOW->Dlg2.m_Edit_3.SetWindowText(str);//покажем на экране
		if(GetGraphRegime()==GR_ENABLED && MAIN_WINDOW->pGRAPH_Dlg->m_Graph.GetRegime()!=GRG_STOP)
		{//включено построение графика, нумера функций в InitGraph
			MAIN_WINDOW->pGRAPH_Dlg->m_Graph.VecFunctions[2].AddGraphWhithTime(temp,MAIN_WINDOW->pGRAPH_Dlg->m_Graph.GetOffsetTime());
		}
	}
	return true;
}

bool GLAVNIY_CLASS_Struct::Add_Freq_Turbine ( bool dir, FP32 temp)
{
	// dir = true - поменяли значение в окне программы
	// dir = false - где-то еще и в окне нужно обновить
	CString str;
	if(dir==true)
	{//было изменено значение в окне программы, и поэтому его обновлять не надо
		MAIN_WINDOW->Dlg2.m_Edit_4.GetWindowText(str);//считаем с экрана
		temp=(float)_wtof(str.GetBuffer());//строка в вещественное
		if(temp>=0. && temp<10000.)
		{//корректное значение
			this->var_Freq_Turbine=temp;
		}
		else
		{//была ошибка, но не тотальная
			MAIN_WINDOW->m_Error.SetERROR(_T("Некорректное значение поля скорости Режим 1, число не обработано"), ERP_WARNING | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
			return true;
		}
	}
	else
	{//нужно еще и обновить в окне
		this->var_Freq_Turbine=temp;
		str.Format(_T("%d"),(int)temp);
		MAIN_WINDOW->Dlg2.m_Edit_4.SetWindowText(str);//покажем на экране
		if(GetGraphRegime()==GR_ENABLED && MAIN_WINDOW->pGRAPH_Dlg->m_Graph.GetRegime()!=GRG_STOP)
		{//включено построение графика, нумера функций в InitGraph
			MAIN_WINDOW->pGRAPH_Dlg->m_Graph.VecFunctions[3].AddGraphWhithTime(temp,MAIN_WINDOW->pGRAPH_Dlg->m_Graph.GetOffsetTime());
		}
	}
	return true;
}

bool GLAVNIY_CLASS_Struct::Add_Nadduv_Pressure ( bool dir, FP32 temp)
{
	// dir = true - поменяли значение в окне программы
	// dir = false - где-то еще и в окне нужно обновить
	CString str;
	if(dir==true)
	{//было изменено значение в окне программы, и поэтому его обновлять не надо
		MAIN_WINDOW->Dlg2.m_Edit_5.GetWindowText(str);//считаем с экрана
		temp=(float)_wtof(str.GetBuffer());//строка в вещественное
		if(temp>=0. && temp<10000.)
		{//корректное значение
			this->var_Nadduv_Pressure=temp;
		}
		else
		{//была ошибка, но не тотальная
			MAIN_WINDOW->m_Error.SetERROR(_T("Некорректное значение поля скорости Режим 1, число не обработано"), ERP_WARNING | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
			return true;
		}
	}
	else
	{//нужно еще и обновить в окне
		this->var_Nadduv_Pressure=temp;
		str.Format(_T("%.2f"),temp);
		MAIN_WINDOW->Dlg2.m_Edit_5.SetWindowText(str);//покажем на экране
		if(GetGraphRegime()==GR_ENABLED && MAIN_WINDOW->pGRAPH_Dlg->m_Graph.GetRegime()!=GRG_STOP)
		{//включено построение графика, нумера функций в InitGraph
			MAIN_WINDOW->pGRAPH_Dlg->m_Graph.VecFunctions[4].AddGraphWhithTime(temp,MAIN_WINDOW->pGRAPH_Dlg->m_Graph.GetOffsetTime());
		}
	}
	return true;
}

/**********************************    Add_Oil_Prot    ***********************************
*		Защита по давления масла
******************************************************************************************/
bool GLAVNIY_CLASS_Struct::Add_Oil_Prot (INT8U temp)
{
	CString str;

	//Теперь сделаем так, введем числовую переменную
	static INT8U regime=0xFF;
	//у которой будет несколько состояний
	//		0xFF - состояние при запуске
	//		0x00 - нет никаких сообщений
	//		0x01 - сработала сигнализация
	//		0x02 - сработала защита
	//		0x03 - отказал датчик давления наддува
	//		0x04 - отказал датчик положения
	static INT8U sygnal1=0xFF;		// Сигнализация в окне 1
	static INT8U sygnal2=0xFF;		// Сигнализация в окне 2
	//		0xFF - состояние при запуске
	//		0x00 - нет никаких сообщений
	//		0x01 - сработала сигнализация/защита
	//		0x02 - отказал датчик
	//		0x10 - сообщение "нет никаких сообщений" не изменилось 
	//		0x11 - сообщение "сработала сигнализация/защита" не изменилось 
	//		0x12 - сообщение "отказал датчик" не изменилось 
	
	temp = temp & 0xF0;	  // Очищаем ненужные биты, чтобы не было ошибочных реакций (нам нужны биты 4-7)

	if (temp == 0x00) {
		if (sygnal1 != 0x10)
			sygnal1 = 0x00;
		if (sygnal2 != 0x10)
			sygnal2 = 0x00;
	} else {
		if ((temp & 0x40) == 0x40) {
			if (sygnal1 != 0x11)
				sygnal1 = 0x01;
		}
		if ((temp & 0x10) == 0x10) {
			if (sygnal1 != 0x12)
				sygnal1 = 0x02;
		}
		if ((temp & 0x80) == 0x80) {
			if (sygnal2 != 0x11)
				sygnal2 = 0x01;
		}
		if ((temp & 0x20) == 0x20) {
			if (sygnal2 != 0x12)
				sygnal2 = 0x02;
		}
	}

	switch (sygnal1) {
		case 0x00:
			MAIN_WINDOW->Dlg3.edit1.SetWindowText(_T(""));	//скроем строку
			MAIN_WINDOW->Dlg3.edit5.SetWindowText(_T("Сигнализация по давлению масла"));
			MAIN_WINDOW->Dlg3.edit5.Invalidate();
			sygnal1 = 0x10;
			break;

		case 0x01:
			//выставим признак сигнализации
			str.Format(_T("%.2f"),var_Oil_Pressure);
			MAIN_WINDOW->Dlg3.edit1.SetWindowText(_T("Сработала при давлении ")+str+_T(" кг/см²"));
			MAIN_WINDOW->Dlg3.edit5.SetWindowText(_T("Сигнализация по давлению масла"));
			MAIN_WINDOW->Dlg3.edit5.Invalidate();
			sygnal1 = 0x11;
			break;

		case 0x02:
			MAIN_WINDOW->Dlg3.edit1.SetWindowText(_T("Отказ датчика положения рейки"));
			MAIN_WINDOW->Dlg3.edit5.SetWindowText(_T(""));	//скроем старое сообщение в левом столбце
			MAIN_WINDOW->Dlg3.edit5.Invalidate();
			sygnal1 = 0x12;
			break;

		default:
			break;
	}

	switch (sygnal2) {
		case 0x00:
			MAIN_WINDOW->Dlg3.edit2.SetWindowText(_T(""));	//скроем строку
			MAIN_WINDOW->Dlg3.edit6.SetWindowText(_T("Защита двигателя по давлению масла"));
			MAIN_WINDOW->Dlg3.edit6.Invalidate();
			sygnal2 = 0x10;
			break;

		case 0x01:
			str.Format(_T("%.2f"),var_Oil_Pressure);
			MAIN_WINDOW->Dlg3.edit2.SetWindowText(_T("Останов при давлении ")+str+_T(" кг/см²"));
			MAIN_WINDOW->Dlg3.edit6.SetWindowText(_T("Защита двигателя по давлению масла"));
			MAIN_WINDOW->Dlg3.edit6.Invalidate();
			sygnal2 = 0x11;
			break;

		case 0x02:
			MAIN_WINDOW->Dlg3.edit2.SetWindowText(_T("Отказ датчика давления масла"));
			MAIN_WINDOW->Dlg3.edit6.SetWindowText(_T(""));	//скроем старое сообщение в левом столбце
			MAIN_WINDOW->Dlg3.edit6.Invalidate();
			sygnal2 = 0x12;
			break;

		default:
			break;
	}
 /*






	if (temp==0x00)
	{//нет ни защиты, ни сигнализации - ничего не покажем
		if (regime!=0x00)
		{//это случилось впервые
			//восстановим информационные надписи в левом столбце, а то они могли скрыться
			MAIN_WINDOW->Dlg3.edit5.SetWindowText(_T("Сигнализация по давлению масла"));
			MAIN_WINDOW->Dlg3.edit6.SetWindowText(_T("Защита двигателя по давлению масла"));

			//AfxMessageBox("Сняли Сигнализацию");
			MAIN_WINDOW->Dlg3.edit1.SetWindowText(_T(""));
			MAIN_WINDOW->Dlg3.edit5.Invalidate();
		
			//AfxMessageBox("Сняли защиту");
			MAIN_WINDOW->Dlg3.edit2.SetWindowText(_T(""));
			MAIN_WINDOW->Dlg3.edit6.Invalidate();

			regime=0x00;	//чтобы в следующий раз сюда опять не зайти
		}
	} else {
		// IDC_EDIT1
		// IDC_EDIT5
		if (temp & 0x50) {
			if (temp==0x40) {//сработала сигнализация
				if(regime!=0x01)
				{//это случилось впервые
					//восстановим информационные надписи в левом столбце, а то они могли скрыться
					MAIN_WINDOW->Dlg3.edit5.SetWindowText(_T("Сигнализация по давлению масла"));
					MAIN_WINDOW->Dlg3.edit6.SetWindowText(_T("Защита двигателя по давлению масла"));

					//выставим признак сигнализации
					str.Format(_T("%.2f"),var_Oil_Pressure);
					MAIN_WINDOW->Dlg3.edit1.SetWindowText(_T("Сработала при давлении ")+str+_T(" кг/см²"));
					MAIN_WINDOW->Dlg3.edit5.Invalidate();
					if(regime==0x02)
					{
						//если надо, уберем признак защиты 
						//а то защита может сняться и без 
						//исчезновения сигнализации
						MAIN_WINDOW->Dlg3.edit2.SetWindowText(_T(""));
						MAIN_WINDOW->Dlg3.edit6.Invalidate();
					}
					regime=0x01;
				}
			} else {//отказ датчика положения рейки
				if(regime!=0x03)
				{//это случилось впервые
					//Для начала нужно скрыть старые надписи о сигнализации и защите, потому что теперь 
					//после отказа датчика они не имеют никакого значения
					MAIN_WINDOW->Dlg3.edit5.SetWindowText(_T(""));	//скроем старое сообщение в левом столбце
					MAIN_WINDOW->Dlg3.edit5.Invalidate();
					MAIN_WINDOW->Dlg3.edit1.SetWindowText(_T("Отказ датчика положения рейки"));
					
					regime=0x03;
				}
			}
		} else {//нужно скрыть показанную прежде информацию
			MAIN_WINDOW->Dlg3.edit5.SetWindowText(_T("Сигнализация по давлению масла"));
			MAIN_WINDOW->Dlg3.edit1.SetWindowText(_T(""));	//скроем строку снизу
		}

		// IDC_EDIT2
		// IDC_EDIT6
		if (temp & 0xE0) {
			if (temp & 0xC0) {//сработала защита
				if(regime!=0x02)
				{//это случилось впервые
					//восстановим информационные надписи в левом столбце, а то они могли скрыться
					MAIN_WINDOW->Dlg3.edit5.SetWindowText(_T("Сигнализация по давлению масла"));
					MAIN_WINDOW->Dlg3.edit6.SetWindowText(_T("Защита двигателя по давлению масла"));

					//AfxMessageBox("Поставили защиту");
					str.Format(_T("%.2f"),var_Oil_Pressure);
					MAIN_WINDOW->Dlg3.edit2.SetWindowText(_T("Останов при давлении ")+str+_T(" кг/см²"));
					MAIN_WINDOW->Dlg3.edit6.Invalidate();

					if(regime!=0x01)
					{//если пришёл флаг защиты, а сигнализация просто не 
						//успела сработать, то всё равно высветим сигнализацию
						str.Format(_T("%.2f"),var_Oil_Pressure);
						MAIN_WINDOW->Dlg3.edit1.SetWindowText(_T("Сработала при давлении ")+str+_T(" кг/см²"));
						MAIN_WINDOW->Dlg3.edit5.Invalidate();
					}
					regime=0x02;
				}
			} else {//отказ датчика давления масла
				if(regime!=0x04)
				{//это случилось впервые
					//Для начала нужно скрыть старые надписи о сигнализации и защите, потому что теперь 
					//после отказа датчика они не имеют никакого значения
					MAIN_WINDOW->Dlg3.edit6.SetWindowText(_T(""));	//скроем старое сообщение в левом столбце
					MAIN_WINDOW->Dlg3.edit6.Invalidate();
					MAIN_WINDOW->Dlg3.edit2.SetWindowText(_T("Отказ датчика давления масла"));
					regime=0x04;
				}
			}
		} else {//нужно скрыть показанную прежде информацию
			MAIN_WINDOW->Dlg3.edit6.SetWindowText(_T("Защита двигателя по давлению масла"));
			MAIN_WINDOW->Dlg3.edit2.SetWindowText(_T(""));	//скроем строку снизу
		}
	}*/
/*




	if(temp==0x00)
	{//нет ни защиты, ни сигнализации - ничего не покажем
		if(regime!=0x00)
		{//это случилось впервые
			//восстановим информационные надписи в левом столбце, а то они могли скрыться
			MAIN_WINDOW->Dlg3.edit5.SetWindowText(_T("Сигнализация по давлению масла"));
			MAIN_WINDOW->Dlg3.edit6.SetWindowText(_T("Защита двигателя по давлению масла"));

			//AfxMessageBox("Сняли Сигнализацию");
			MAIN_WINDOW->Dlg3.edit1.SetWindowText(_T(""));
			MAIN_WINDOW->Dlg3.edit5.Invalidate();
		
			//AfxMessageBox("Сняли защиту");
			MAIN_WINDOW->Dlg3.edit2.SetWindowText(_T(""));
			MAIN_WINDOW->Dlg3.edit6.Invalidate();

			regime=0x00;	//чтобы в следующий раз сюда опять не зайти
		}
	}
	else if(temp==0x40)
	{//сработала сигнализация
		if(regime!=0x01)
		{//это случилось впервые
			//восстановим информационные надписи в левом столбце, а то они могли скрыться
			MAIN_WINDOW->Dlg3.edit5.SetWindowText(_T("Сигнализация по давлению масла"));
			MAIN_WINDOW->Dlg3.edit6.SetWindowText(_T("Защита двигателя по давлению масла"));

			//AfxMessageBox("Поставили сигнализацию");
			//выставим признак сигнализации
			str.Format(_T("%.2f"),var_Oil_Pressure);
			MAIN_WINDOW->Dlg3.edit1.SetWindowText(_T("Сработала при давлении ")+str+_T(" кг/см²"));
			MAIN_WINDOW->Dlg3.edit5.Invalidate();
			if(regime==0x02)
			{
				//если надо, уберем признак защиты 
				//а то защита может сняться и без 
				//исчезновения сигнализации
				MAIN_WINDOW->Dlg3.edit2.SetWindowText(_T(""));
				MAIN_WINDOW->Dlg3.edit6.Invalidate();
			}
			regime=0x01;
		}
	}
	else if(temp & 0x40 || temp & 0x80)
	{//сработала защита
		if(regime!=0x02)
		{//это случилось впервые
			//восстановим информационные надписи в левом столбце, а то они могли скрыться
			MAIN_WINDOW->Dlg3.edit5.SetWindowText(_T("Сигнализация по давлению масла"));
			MAIN_WINDOW->Dlg3.edit6.SetWindowText(_T("Защита двигателя по давлению масла"));

			//AfxMessageBox("Поставили защиту");
			str.Format(_T("%.2f"),var_Oil_Pressure);
			MAIN_WINDOW->Dlg3.edit2.SetWindowText(_T("Останов при давлении ")+str+_T(" кг/см²"));
			MAIN_WINDOW->Dlg3.edit6.Invalidate();

			if(regime!=0x01)
			{//если пришёл флаг защиты, а сигнализация просто не 
				//успела сработать, то всё равно высветим сигнализацию
				str.Format(_T("%.2f"),var_Oil_Pressure);
				MAIN_WINDOW->Dlg3.edit1.SetWindowText(_T("Сработала при давлении ")+str+_T(" кг/см²"));
				MAIN_WINDOW->Dlg3.edit5.Invalidate();
			}
			regime=0x02;
		}
	}
	else if(temp & 0x38)
	{//отказ какого-то датчика
		if(temp & 0x20)
		{//отказ датчика давления масла
			if(regime!=0x04)
			{//это случилось впервые
				//Для начала нужно скрыть старые надписи о сигнализации и защите, потому что теперь 
				//после отказа датчика они не имеют никакого значения
				MAIN_WINDOW->Dlg3.edit6.SetWindowText(_T(""));	//скроем старое сообщение в левом столбце
				MAIN_WINDOW->Dlg3.edit6.Invalidate();
				MAIN_WINDOW->Dlg3.edit2.SetWindowText(_T("Отказ датчика давления масла"));
				regime=0x04;
			}
		}
		else
		{//нужно скрыть показанную прежде информацию
			MAIN_WINDOW->Dlg3.edit2.SetWindowText(_T(""));	//скроем строку снизу
			MAIN_WINDOW->Dlg3.edit6.SetWindowText(_T("Защита двигателя по давлению масла"));
		}
		if(temp & 0x10)
		{//отказ датчика положения рейки
			if(regime!=0x03)
			{//это случилось впервые
				//Для начала нужно скрыть старые надписи о сигнализации и защите, потому что теперь 
				//после отказа датчика они не имеют никакого значения
				MAIN_WINDOW->Dlg3.edit5.SetWindowText(_T(""));	//скроем старое сообщение в левом столбце
				MAIN_WINDOW->Dlg3.edit5.Invalidate();
				MAIN_WINDOW->Dlg3.edit1.SetWindowText(_T("Отказ датчика положения рейки"));
				
				regime=0x03;
			}
		}
		else
		{//нужно скрыть показанную прежде информацию
			MAIN_WINDOW->Dlg3.edit5.SetWindowText(_T("Сигнализация по давлению масла"));
			MAIN_WINDOW->Dlg3.edit1.SetWindowText(_T(""));	//скроем строку снизу
		}
		if(temp & 0x08)
		{//отказ датчика давления наддува
			//Здесь я ничего не буду делать, т.к. этот бит дублирует 
			//соответствующее значение в байте 7 данных
			//и все действия я уже произвёл при его обработке
		}
	}
	else
	{
		str.Format(_T("Пришел неверный символ (%u) в поле приёма состояния защиты по маслу, датчиков положения и давления масла"),temp);
		MAIN_WINDOW->m_Error.SetERROR(str,ERP_ERROR | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
	}
 */
	this->var_Oil_Prot=temp;

	return true;
}

bool GLAVNIY_CLASS_Struct::Add_Oil_Pressure (bool dir, FP32 temp)
{
	// dir = true - поменяли значение в окне программы
	// dir = false - где-то еще и в окне нужно обновить
	CString str;
	if(dir==true)
	{//было изменено значение в окне программы, и поэтому его обновлять не надо
		MAIN_WINDOW->Dlg2.m_Edit_7.GetWindowText(str);//считаем с экрана
		temp=(float)_wtof(str.GetBuffer());//строка в вещественное
		if(temp>=0. && temp<10000.)
		{//корректное значение
			this->var_Oil_Pressure=temp;
		}
		else
		{//была ошибка, но не тотальная
			MAIN_WINDOW->m_Error.SetERROR(_T("Некорректное значение поля скорости Режим 1, число не обработано"), ERP_WARNING | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
			return true;
		}
	}
	else
	{//нужно еще и обновить в окне
		this->var_Oil_Pressure=temp;
		str.Format(_T("%.2f"),temp);
		MAIN_WINDOW->Dlg2.m_Edit_7.SetWindowText(str);//покажем на экране
		if(GetGraphRegime()==GR_ENABLED && MAIN_WINDOW->pGRAPH_Dlg->m_Graph.GetRegime()!=GRG_STOP)
		{//включено построение графика, нумера функций в InitGraph
			MAIN_WINDOW->pGRAPH_Dlg->m_Graph.VecFunctions[5].AddGraphWhithTime(temp,MAIN_WINDOW->pGRAPH_Dlg->m_Graph.GetOffsetTime());
		}
	}
	return true;
}

/****************************   Add_Nadduv_Limit    *******************************
*		Ограничение по наддуву
***********************************************************************************/
bool GLAVNIY_CLASS_Struct::Add_Nadduv_Limit(INT8U temp)
{
	CString str;
	static unsigned char regime=0xFF;	//будем отслеживать последовательность событий
				//0xFF - состояние при первом зупуске
				//0x00 - нет никаких событий
				//0x01 - защита по наддуву
				//0x02 - вышел из строя датчик

	if(temp==0x00)
	{//нет защиты по наддуву
		if(regime!=0x00)
		{//это случилось впервые
			//название строки, а то оно могло затереться при отказе датчика
			MAIN_WINDOW->Dlg3.edit8.SetWindowText(_T("Ограничение по наддуву"));
			MAIN_WINDOW->Dlg3.edit4.SetWindowText(_T(""));	//скроем старую надпись в левом столбце
			regime=0x00;
		}
	}
	else if(temp==0xFE)
	{//есть защита по наддуву
		if(regime!=0x01)
		{//это случилось впервые
			//название строки, а то оно могло затереться при отказе датчика
			MAIN_WINDOW->Dlg3.edit8.SetWindowText(_T("Ограничение по наддуву"));

			str.Format(_T("%.2f"),var_Nadduv_Pressure);
			MAIN_WINDOW->Dlg3.edit4.SetWindowText(_T("Наступило при давлении ")+str+_T(" кг/кв.см."));
			regime=0x01;
		}
	}
	else if(temp==0xEE)
	{//вышел из строя датчик давления наддува
		if(regime!=0x02)
		{//это случилось впервые
			MAIN_WINDOW->Dlg3.edit4.SetWindowText(_T("Отказ датчика давления наддува"));
			MAIN_WINDOW->Dlg3.edit8.SetWindowText(_T(""));	//скроем старую надпись в левом столбце
			regime=0x02;
		}
	}
	else
	{
		str.Format(_T("Неверный символ (%u) в поле ограничения по наддуву"),temp);
		m_pError->SetERROR(str,ERP_WARNING | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
		return false;	//не всё так гладко
	}
	this->var_Nadduv_Limit=temp;	//перепишем в переменную класса
	return true;
}

bool GLAVNIY_CLASS_Struct::Add_Run_Stop(bool Run)
{
	var_Run_Stop=Run;
	//поменяет цвет кнопки на форме с желтого (ожидание)
	//на красный (пошла работо)
	if(Run==true)
	{
		if(var_UST_Run_Stop)
		{//сделаем изменения в цвете только в случае, если здесь была уставка
			MAIN_WINDOW->Dlg1.m_UST_Run.SetBGColor(BGColorEn);
			MAIN_WINDOW->Dlg1.m_UST_Stop.SetBGColor(BGColorDis);
		}
	}
	else
	{
		if(var_UST_Run_Stop==false)
		{//сделаем изменения в цвете только в случае, если здесь была уставка
			MAIN_WINDOW->Dlg1.m_UST_Run.SetBGColor(BGColorDis);
			MAIN_WINDOW->Dlg1.m_UST_Stop.SetBGColor(BGColorEn);
		}
	}
	return true;
}

bool GLAVNIY_CLASS_Struct::Add_Raznos(bool dir, bool temp)
{
	// dir = true - поменяли значение в окне программы
	// dir = false - где-то еще и в окне нужно обновить
	if(dir==true)
	{
//		this->var_Raznos=MAIN_WINDOW->Dlg2.m_Button_3.GetCheck();
	}
	else
	{
		this->var_Raznos=temp;
		static bool flag=false;		//временный флаг
		if(var_Raznos)
		{//есть защита по предельным оборотам
			if(flag==false)
			{//это случилось впервые
				CString str;
				str.Format(_T("%d"),(int)var_Freq_Rot);
				MAIN_WINDOW->Dlg3.edit3.SetWindowText(_T("Останов при оборотах ")+str+_T(" об/мин"));
				MAIN_WINDOW->Dlg3.edit7.Invalidate();
				flag=true;
			}
		}
		else
		{//нет защиты по предельным оборотам
			if(flag==true)
			{//это случилось впервые
				MAIN_WINDOW->Dlg3.edit3.SetWindowText(_T(""));
				MAIN_WINDOW->Dlg3.edit7.Invalidate();
				flag=false;
			}
		}
	}
	return true;
}

/********************************************************************************
*		Функции управления графиком
********************************************************************************/
bool GLAVNIY_CLASS_Struct::SetGraphRegime(INT8U regime)
{
	if(m_GraphRegime==regime) return true;		//я уже нахожусь в нужном режиме

	//что нужно сделать со старыми настройками
	switch(m_GraphRegime)
	{
	case GR_NOTINITIALIZED:
		break;
	case GR_DISABLED:
		break;
	case GR_ENABLED:
		break;
	default:				//неизвестный режим
		return false;
	}

	//что нужно сделать, чтобы перейти в новый режим
	switch(regime)
	{
	case GR_NOTINITIALIZED:			//в этот режим нельзя переходить
		return false;
	case GR_DISABLED:
		((CButton*)MAIN_WINDOW->Dlg5.GetDlgItem(IDC_ENABLE_GRAPH))->SetCheck(BST_UNCHECKED);
		break;
	case GR_ENABLED:
		((CButton*)MAIN_WINDOW->Dlg5.GetDlgItem(IDC_ENABLE_GRAPH))->SetCheck(BST_CHECKED);
		break;
	default:						//неизвестный режим
		return false;
	}
	m_GraphRegime=regime;
	return true;	//всё прошло нормально
}
INT8U GLAVNIY_CLASS_Struct::GetGraphRegime()
{
	return m_GraphRegime;
}


void  GLAVNIY_CLASS_Struct::CalcLinear(void)
{
	int ix=0;
	int size;
	float dFullX;
	float dfh;


	size = m_VectNonLinear.size();
	if (size == 0)
		return;
	
	dFullX = (m_VectNonLinear[size-1].fX - m_VectNonLinear[0].fX);

	if (dFullX == 0.0f)
	{
		for (ix=0; ix<size; ix++) {
			m_VectNonLinear[ix].fh = 0.0f;
			m_VectNonLinear[ix].fgammai = 0.0f;
		}
		return;
	}

	fA = (m_VectNonLinear[size-1].fY - m_VectNonLinear[0].fY) / dFullX;
//	fB = (fY[0] * (fX[15]) - fY[15] * (fX[0])) / (fX[15] - fX[0]);

	fB = (m_VectNonLinear[0].fY * m_VectNonLinear[size-1].fX -
		  m_VectNonLinear[size-1].fY * m_VectNonLinear[0].fX) / dFullX;

	fhmax  = 0.0f;
	fhmaxp = 0.0f;
	fhmaxm = 0.0f;

	for (ix=0; ix<size; ix++) {
		dfh = (fA * ((float)ix) + fB) - m_VectNonLinear[ix].fY;
		if (dfh > 0.0f) {
			if (fhmaxp < dfh)
				fhmaxp = dfh;
		}
		if (dfh < 0.0f) {
			if (fhmaxm < fabsf(dfh))
				fhmaxm = fabsf(dfh);
		}
		m_VectNonLinear[ix].fh = dfh;
		m_VectNonLinear[ix].fgammai = dfh * 100.0f / dFullX;
	}
	fhmax  = fhmaxp + fhmaxm;
	fgamma = fhmax * 100.0f / dFullX; 

/*	CTime time = CTime::GetCurrentTime();

	time_date.Format(_T("%.2d.%.2d.%d %2d:%2d"), time.GetDay(), time.GetMonth(),
		time.GetYear(), time.GetHour(), time.GetMinute());	*/ 
}

void  GLAVNIY_CLASS_Struct::CalcEGU(void)
{
	float dFullX;
	int   size;			// 
	int   size_w;		// окна
	int   ix;		// индекс измерений внутри окна
	int   ix_w;		// индекс окон
	float fa, fb;	// коэф.прямой окна
	float da = 0.0f;
	float ci;
	float cyw;

	int wconst=21;
	fYw = 0.0f;


	size = m_VectEGU.size();
	if (size < wconst)
		return;
	size_w = size - wconst;		// кол-во окон


	for (ix_w = 0; ix_w < size_w; ix_w++) {
		da = 0.0f;	 // свой максимум для каждого участка

 		dFullX = (m_VectEGU[wconst + ix_w - 1].fX - m_VectEGU[ix_w].fX);

		if (dFullX == 0.0f)
		{
			Fix_w = -1;		// ошибка
			return;
		}
		////////////////////////////////////////
		fa = (m_VectEGU[wconst + ix_w - 1].fY - m_VectEGU[ix_w].fY) / dFullX;

/*		fb = (m_VectEGU[ix_w].fY * m_VectEGU[wconst + ix_w - 1].fX -
			  m_VectEGU[wconst + ix_w - 1].fY * m_VectEGU[ix_w].fX) / dFullX;  */
		fb = m_VectEGU[ix_w].fY;
	
		for (ix=0; ix<21; ix++) {
			ci = fa*((float)ix) + fb;
			da = __max(da, fabs(ci - m_VectEGU[ix + ix_w].fY));
		}


		if (m_VectEGU[ix + ix_w].fY == m_VectEGU[ix_w].fY) {
			Fix_w = -1;		// ошибка
			return;
		}

		cyw = da / (m_VectEGU[ix + ix_w].fY - m_VectEGU[ix_w].fY);

		if (fYw == 0.0f) fYw = cyw;
		else if (fYw > cyw) {
			Fix_w = ix_w;
			fYw   = cyw;
		}
	}

	fL = fK * ((float)Fix_w) - fH;
}

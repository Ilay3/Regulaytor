/************************************************************************************************
*************************************************************************************************
**		Этот файл содержит кодфункций класса приемо-передатчика. Они делают все, что нужно для связи,
**	расшифровки, компоновки сообщений. Об их своевременной отправке, контроле ошибок и т.д. Однако все данные
**	как для передачи, так и после расшифровке записываются в главный класс.
***************************************************************************************************
***************************************************************************************************/

/**************************** ЛИСТ РЕГИСТРАЦИИ ИЗМЕНЕНИЙ *******************************************************
*	11.09.2006 - добавлены изменения в пакет компьютер-МПСУТП, исчезла идиотская надпись "Сумма не подсчитана" 
*			в протоколе с Байтстаффингом.
****************************************************************************************************************/

#include "stdafx.h"
#include "protocol_struct.h"
#include "USART_Dlg.h"

/*IMPLEMENT_DYNAMIC(PROTOCOL_Struct)*/

/**********************	InitPROTOCOL(CUSART_Dlg * pParent, ERROR_Class * error)*********************
*		Инициализация. pParent - указатель на окно связи
*						error - указатель на переменную отображения ошибки
*******************************************************************************************************/
bool PROTOCOL_Struct::InitPROTOCOL(CUSART_Dlg * pParent, ERROR_Class * error)
{
	ASSERT(pParent);
	ASSERT(error);

	this->pUSART_Dlg=pParent;		//присвоим указатель на главное окно по связи
	this->m_pError=error;			//присвоим указатель на ошибку

	if(pProtocolOptions->var_MAX_LEN!=0)
	{//создадим массив-буфер для принимаемого сообщения 
		ReceiveMessage= new unsigned char [pProtocolOptions->var_MAX_LEN];
	}
	else
	{
		m_pError->SetERROR(_T("При инициализации приемного буфера не указан его размер"), ERP_ERROR | ER_WRITE_TO_LOG);
	}

	if(pProtocolOptions->o_OFF)
	{
		//Открываем файл готовых к отправке сообщений
		OutMesFileOpen();
	}

	pProtocolOptions->o_MsgType = 0;				// Тип сообщения - без учета CS в поле L_DAT (L_DAT=7)

	this->ClearInMessageCount();
	this->ClearOutMessageCount();

	//А вот здесь мы попатаемся откомпилировать сообщения, 
	//с целью выявить профпригодные, и если сообщение 
	//"живое", то показать его название в листинге доступных сообщений
	//Таким образом есть довольно существенное ограничение:
	//типы посылок должны идти последовательно друг за другом без пропусков
	//и давать положительный результат при инициализации уже на этапе инициали-
	//зации программы
	unsigned short i;
	for(i=0; i<pProtocolOptions->CompileMessage_x.size(); i++)
	{
		CMessage temp;
		temp.type = 1;	//1-7/0-8
		if(!pProtocolOptions->CompileMessage_x[i](temp))
		{
			//что-то там не так
			return false;
		}
		pUSART_Dlg->m_ListPacket.AddString(temp.name);
	}
	if(i==0)
	{//блин нет даже первой посылки
		m_pError->SetERROR(_T("Невозможно откомпилировать ни одного сообщения, проверьте, правильно ли присвоены указатели на функции"), ERP_WARNING | ER_WRITE_TO_LOG);
	}

	//Следующей в списке идет надпись ручной компоновки сообщения
	pUSART_Dlg->m_ListPacket.AddString(_T("Ручное составление посылки"));

	return true;		//автоматически возвернуться ошибки, если таковые были
}

/****************************************************************************
*									КОНСТРУКТОР	
*****************************************************************************/
PROTOCOL_Struct::PROTOCOL_Struct(void)
{
	InMesFileOpened=false;	//Файл принятых по сети сообщений открыт не был
}

bool PROTOCOL_Struct::CompileALL(void)
{
	/*
	Если передатчику нужно передать следующую посылку, ему просто нужно вызвать эту функцию,
	и при удачном выполнении в MAIN_MESSAGE будет то , что ему нужно отправить в следующий раз

	*/
	/*Здесь по кусочкам собираются различные пакеты,
	в принципе их можно пересчитать все, но здесь больше 
	возможностей для другого хода мыслей*/
	/*Например, если нужно поочередно передавать два сообщения, в соответствующей ветке нужно
	просто сделать поочередное переключений указателей
	*/
	/*
	Однако можно передавать сообщение и поодиночке, для этого нужно просто каждый раз компилировать новое 
	сообщение (именно это), или вообще ничего не делать (если оно скомпилировалось где-то еще)
	*/
	if(pProtocolOptions->o_OFF)
	{
		/*
		Однако, если надо передавать из файла, то ни о чем другом речи не идет!!!
		*/
		if(!CompileMessage_10(NextMessage))//может она еще не успела откомпилироваться
		{//нет, это что-то посерьезнее
			m_pError->SetERROR(_TEXT("Сбой при компиляции посылки из файла"),ERP_ERROR | ER_WRITE_TO_LOG);
			return false;//это уже серьезно
		}
		//обработки байт-стаффинга естесственно нету
	}
	else if (Mode==pProtocolOptions->CompileMessage_x.size()+1)
	{//СООБЩЕНИЕ БЫЛО СКОМПОНОВАНО ВРУЧНУЮ
		if(!CompileMessage_9(NextMessage))//может она еще не успела откомпилироваться
		{//нет, это что-то посерьезнее
			m_pError->SetERROR(_TEXT("Сбой при компиляции ручной посылки"),ERP_ERROR | ER_WRITE_TO_LOG);
			return false;//это уже серьезно
		}
		//обработки байт-стаффинга естесственно нету
	}
	else 
	{//САМОЕ ОБЫЧНОЕ ВНЕШНЕЕ СООБЩЕНИЕ
		if(Mode==0)
		{//Не выбран ни один тип сообщения
			m_pError->SetERROR(_T("Для начала передачи выберите тип отправляемого сообщения"), ERP_WATCHFUL | ER_SHOW_MESSAGE);
			return true;	//нет, это не ошибка
		}

		NextMessage.type = 1;
		if(!pProtocolOptions->CompileMessage_x[Mode-1](NextMessage))//может она еще не успела откомпилироваться
		{//нет, это что-то посерьезнее
			CString str; str.Format(_T("Сбой при инциализации сообщения №%i"),Mode);
			m_pError->SetERROR(str,ERP_ERROR | ER_WRITE_TO_LOG);
			return false;//это уже серьезно
		}
		if(pProtocolOptions->var_Regime==BYTE_STUFFING && this->Mode!=9)
		{//учтем байтстаффинг (Ручное сообщение обслуживать байтстаффингом не будем!!!)
			if(!RecompileMessageWithBytestuffing(&NextMessage, pProtocolOptions->var_FIRST_BYTE, pProtocolOptions->var_STUF_BYTE))
			{
				CString str; str.Format(_T("Сбой при байтстаффинге в сообщении №%i"),Mode);
				m_pError->SetERROR(str,ERP_ERROR | ER_WRITE_TO_LOG);
				return false;
			}
		}
	}


	//Покажем скомпованной сообщение
	pUSART_Dlg->ShowOutputMessage();
	return true;
}


/**************************************************************************************
*	Следующие две функции настраивают передачу ручного сообщения и 
*	передачу из файла соответственно
**************************************************************************************/
bool PROTOCOL_Struct::CompileMessage_9(CMessage &nextmessage) //собирает NextMessage_8 из главного класса
{
	/*
	Это не простая посылка, она компануеся вручную из эдитика на окне настройки
	связи
	*/
	//оставляем старые времена передачи и приема!!!
	
	//здесь будет содержаться готовая к отправке посылка
	static unsigned char mas[100];	
	unsigned short lMesLen;

	if(pProtocolOptions->o_HexDisplayOutMessage)
	{//там всё отображалось в шестнадцатеричных кодах
		CString str;
		pUSART_Dlg->m_OutMessage.GetWindowText(str);
		CString error;										//чтобы поддерживать ошибки
		lMesLen=GiveHexFromStr(str,mas,&error);	//переконвертируем из шестнадцатеричного значения
		if(error!="")
		{//функция отработала с ошибками
			m_pError->SetERROR(error,4);
			return false;
		}
	}
	else
	{//просто скопирем текст
		pUSART_Dlg->m_OutMessage.GetWindowText((LPTSTR)mas,100);
		lMesLen=(unsigned short)strlen((char*)mas);
	}

	nextmessage.peredacha=NextMessage.peredacha;	//|
	nextmessage.priem=NextMessage.priem;			//|А время оставляем прежним
	nextmessage.text=mas;			//текст перекопируем
	nextmessage.length=lMesLen;		//длина посылки
	return true;
}

bool PROTOCOL_Struct::CompileMessage_10(CMessage &nextmessage) //собирает NextMessage_8 из главного класса
{

	//Просто подготавливает систему к отправке нового сообщения
	//из внешнего файла, не задаваясь вопросом о корректности 
	//этого сообщения
	//но проставляя поля времени передачи и приема

	static CString message_from_file;
	CString error=_TEXT("Нет ошибок");
	CString str,str1;
	if(!(OutMessageFile.is_open()))
	{
		error=CString(_TEXT("Файл отправки "))+pProtocolOptions->o_outMesFile + CString(_TEXT(" не открыт"));
		goto ERROR2;//чтобы ошибок больше не было
	}
	
	char t[150];
	//неправильно! спросить Серого
	do //пропускаем строки с комментариями
	{
		OutMessageFile.getline(t,150,0x0A);
		//Пока работает
		//0х0А - перенос строки, /n - не работает
		str=t;
		str.TrimLeft(' ');//удаляем пробелы вначале строки
		
		if(str==_T("RETURN"))
		{
			//сюда запихнута процедура перехода 
			//в начало файла
			OutMessageFile.clear();
			OutMessageFile.seekg(0,ios::beg);
			continue;
		}
		else
			if(OutMessageFile.eof())
			{
				error=_T("Достигнут конец файла!");
				goto ERROR2;//чтобы ошибок больше не было
			}
		//пропускаем пустые строки
		//str.TrimLeft(' ');
		if(str=="") continue;//пустая строка
		if(str.Left(2)!="//")break;//не комментарий, а деловая информация
	}
	while(true);
	
	//если str - не комментарий, не конец, а сообщение 
	unsigned col=str.Find('|');
	str1=str.Left(col);
	str=str.Right(str.GetLength()-col-1);//обрежем использованное
	
	unsigned time;
	time=_ttoi(str1.GetString());//временная переменная
	if(!time)//номер сообщения
	{
		//хотя номер в принципе пока не используется
		error=_T("Посылка без номера.");
		goto ERROR2;//чтобы ошибок больше не было
	}
	else nextmessage.nomer=time;
	//выделим текст посылки
	//и удалим пробелы
	str.TrimLeft(' ');
	col=str.Find('|');
	str1=str.Left(col);
	str=str.Right(str.GetLength()-col-1);//обрежем использованное
	str1.TrimRight(' ');
	message_from_file=str1;
	
	col=str.Find('|');
	str1=str.Left(col);
	str=str.Right(str.GetLength()-col-1);//обрежем использованное
	time=_ttoi(str1);
	if(!time)//время передачи
	{
		error=_T("Время передачи - не число");
		goto ERROR2;//чтобы ошибок больше не было
	}
	else
		if(time<15 || time>800)
		{
			error=_T("Нереальное время передачи");
			goto ERROR2;//чтобы ошибок больше не было
		}
	nextmessage.peredacha=time;
	
	time=_ttoi(str);
	if(!time)//время приема
	{
		error=_T("Время приема - не число");
		goto ERROR2;//чтобы ошибок больше не было
	}
	else
		if(time<15 || time>800)
		{
			error=_T("Нереальное время приема");
			goto ERROR2;//чтобы ошибок больше не было
		}
	nextmessage.priem=time;
ERROR2:		
	if(error!=_T("Нет ошибок"))
	{
		//просигннализируем об ошибке
		message_from_file=_T("");//во избежании коллизий
		m_pError->SetERROR(error,4);
		return false;	//и просто выйдем
	}
	
	nextmessage.text=(unsigned char *)message_from_file.GetBuffer();
	nextmessage.length=message_from_file.GetLength(); //пока так, хотя нужна серьезная доработка
	nextmessage.summa=0; //а хрен его знает где и как она там считается

	pUSART_Dlg->ShowOutputMessage();
	pUSART_Dlg->ShowERROR_in_out_message(error,5);
	return true;
}

// открываем файл листинга полученных сообщений
bool PROTOCOL_Struct::InMesFileOpen(void)
{
	if(ReceiveMessageFile.is_open()) ReceiveMessageFile.close();//на всякий случай
	ReceiveMessageFile.open(pProtocolOptions->o_inMesFile,ios::trunc | ios::out);
	CString str;
	if(!ReceiveMessageFile)
	{
		m_pError->SetERROR(_T("Не создался файл \r\n ")+pProtocolOptions->o_inMesFile, ERP_WARNING | ER_WRITE_TO_LOG);		
	}
	else
	{
		CStringA str="Файл листинга полученных по сети сообщений...";
		ReceiveMessageFile.write(str,str.GetLength());
		ReceiveMessageFile<<endl<<endl;
		m_pError->SetERROR(_T("Файл регистрации входящих сообщений \"")+pProtocolOptions->o_inMesFile+_T("\" успешно открыт"), ERP_NORMAL | ER_WRITE_TO_LOG | ER_HIDE_FROM_USER);
	}
	return true;
}

bool PROTOCOL_Struct::OutMesFileOpen(void)
{
	if(OutMessageFile.is_open())OutMessageFile.close();
	OutMessageFile.open(pProtocolOptions->o_outMesFile);
	if(!OutMessageFile)
	{
		m_pError->SetERROR(_T("Не могу открыть файл отправки ")+pProtocolOptions->o_outMesFile,ERP_WARNING | ER_WRITE_TO_LOG);
	}
	else
	{
		m_pError->SetERROR(_T("Файл ")+pProtocolOptions->o_outMesFile+_T(" успешно открыт"), ERP_NORMAL | ER_WRITE_TO_LOG | ER_HIDE_FROM_USER);
		//сбросим указатель в файле
		BeginOutMesFile();
	}
	return true;
}
void PROTOCOL_Struct::BeginOutMesFile(void)
{// сбрасывает указатель в файле, из которого читаются сообщения
	OutMessageFile.clear();
	OutMessageFile.seekg(0,ios::beg);
}

/********************** ClearInMessageCount(void) ************************
*	стирает количество принятых пакетов
*************************************************************************/
bool PROTOCOL_Struct::ClearInMessageCount(void)
{
	this->InMessageCount=0;
	pUSART_Dlg->m_Recieve.SetWindowText(_T("0\0"));
	return true;
}

/********************** AddInMessageCount(void) ************************
*	стирает количество принятых пакетов
*************************************************************************/
bool PROTOCOL_Struct::AddInMessageCount(void)
{//просто обновим и тут, и там
	InMessageCount++;
	CString str; str.Format(_T("%u"),InMessageCount);
	pUSART_Dlg->m_Recieve.SetWindowText(str);
	return true;
}


/********************** ClearOutMessageCount(void) ************************
*	стирает количество отправленных пакетов
*************************************************************************/
bool PROTOCOL_Struct::ClearOutMessageCount(void)
{
	this->OutMessageCount=0;
	pUSART_Dlg->m_sended.SetWindowText(_T("0\0"));
	return true;
}

/********************** AddOutMessageCount(void) ************************
*	Довляет количество отправленных пакетов
*************************************************************************/
bool PROTOCOL_Struct::AddOutMessageCount(void)
{
	OutMessageCount++;
	CString str; str.Format(_T("%u"),OutMessageCount);
	pUSART_Dlg->m_sended.SetWindowText(str);
	return true;
}

/************************* ReceiveChar(unsigned char ch) ************************
*	дописывает полученный  символ, проверяет чек-сумму, максимальную длину,
*	окончание старого и начало нового пакета. Короче элементарные вещи.
*	Если эта функция приняла пакет без ошибок - можно гарантировать, что пакет
*	соответствует формату протокола. А вот корректные ли внутри него данные
*	решает функция pProtocolOptions->OnVerify()
*	ch - очередной принятый символ
********************************************************************************/
bool PROTOCOL_Struct::ReceiveChar(unsigned char ch)
{
	if(pProtocolOptions->OnVerify==NULL)	return false;
	if(ReceiveMessage==NULL)				return false;

	/*Общие для всех алгоритмов переменные*/
	static unsigned char	pred_char		=	0;						//предыдущий символ
	static unsigned short	char_count		=	0;						//количество принятых символов
	static CString error;
	static CMessage lsReseivingMessage(ReceiveMessage);
	
	char_count++;//приняли еще один символ
	
	/*if(pUSART_Dlg->m_hWnd==NULL || pUSART_Dlg->m_ReceiveMessage.m_hWnd==NULL)
	{
		this->m_pError->SetERROR(_T("Окно связи разрушено, но пытается использоваться в функции ReceiveChar"),ERP_FATALERROR | ER_WRITE_TO_LOG);
		return false;
	}*/

	if(pProtocolOptions->var_Regime==PACKET_LEN)
	{
		/*
		Протокол с фиксированным полем длины, внутри сообщения могут быть любые символы,
		четко отслеживается конец пакета (по длине).
		*/

		static bool verifi_on=0;			//если 1, то пакет распознан и записывается
											//в нужное место
		
		static bool FirstByteReady=false;	//Допустим, сообщение еще не распознано (не пошло поле
											//данных, а в шапке появился опять первый символ.
											//Этот флаг устранит возможность неверного начала нового
											//сообщения

		static unsigned short paket_length=0;	//длина оставшейся после распознавания части пакета
		static unsigned short input_count=0;	//количество принятых байт
		
		unsigned char RC_register=ch;			//просто так

		//СЛЕДУЮЩИЙ НАБОР ФЛАГОВ ПРОСТО ОТРАЖАЕТ СОСТОЯНИЕ ПРИЕМА, НО НЕ ВКЛИНИВАЕТСЯ В НЕГО
		static bool Chepuha=true;		//после последнего удачно принятого сообщения еще не принят ни один стартовый символ
		static bool LenCorrect=false;	//принятое поле длины имело корректное значение	
		static bool TheEnd=false;		//только что приняли конец сообщения, его можно отобразить на экране
		static bool SumCorrect=false;

		if(input_count>pProtocolOptions->var_MAX_LEN)//переполнение буфера, а по сети гонится лажа
		{//начнем сначала
			input_count=0;
		}

		if(TheEnd)
		{//в прошлый раз закончили принимать сообщение, а с этим символом забудем о предыдущем пакете
			TheEnd=false;
			Chepuha=true;
		}

		//ДОБАВИМ ОЧЕРЕДНОЙ ПРИНЯТЫЙ СИМВОЛ
		ReceiveMessage[input_count]=RC_register;	//записываем в буфер приема
		input_count++;								//нарастим количество принятых символов

		//Пока пакет не опознан, в нем НЕ допускается $ (кроме, ессно, начала)
		if(RC_register==pProtocolOptions->var_FIRST_BYTE && FirstByteReady==false)
		{
			input_count=0;			//начали новое сообщение - сбросили счетчик
			ReceiveMessage[0]=RC_register;	//запишем этот символ уже на свое место
			input_count=1;			//начали новое сообщение - сбросили счетчик
			FirstByteReady=true;	//однако первый байт уже приняли

			Chepuha=false;		//это уже осмысленный прием
			LenCorrect=false;	//пока еще ничего не знаем о длине
			SumCorrect=false;	//контрольную сумму еще не считали
		}
		else if(Chepuha==false)
		{//пакет опознан
			if(input_count==pProtocolOptions->var_LEN_STATE+1)//четвертый принятый символ
			{//пакет уже распознан, а по протоколу, в этом месте стоит длина данных в пакете
				if(RC_register>pProtocolOptions->var_MAX_LEN-pProtocolOptions->var_LEN_ADD)
				{//поле длины посылки слишком большое
					paket_length=pProtocolOptions->var_MAX_LEN;
					FirstByteReady=false;	//начнем ожидание новой посылки
				}
				else
				{
					//выставим, какое кол-во символов нам следует принять
					paket_length=RC_register+pProtocolOptions->var_LEN_ADD;

					LenCorrect=true;	//поле длины было корректно
				}
			}
			else if(input_count==paket_length)
			{//сообщение принято целиком, пора проверить контрольную сумму
				if(LenCorrect)
				{//если длина была корректна, то можно попробовать и посчитатьконтрольную сумму
					unsigned char sim=ch;
					unsigned char sim2=pProtocolOptions->CURRENT_SUMM(ReceiveMessage,1,input_count-1);
					if(sim!=sim2)
					{//ошибка в контрольной сумме
					}
					else
					{//контрольная сумма верна
						SumCorrect=true;
					}
				}
				FirstByteReady=false;	//можно начать принимать новое сообщение

				TheEnd=true;		//выставим флаг конца сообщения
			}
		}

			
		//ОТОБРАЗИМ ТО, ЧТО ПРИНЯЛИ НА ЭКРАНЕ
		if(pProtocolOptions->o_HexDisplayInMessage)
		{
			////////////////////////////////////////////////////////////////////////////////////////////
			//	Здесь определяется, что нам нужно - быстродействие или информативность
			////////////////////////////////////////////////////////////////////////////////////////
			if(!pProtocolOptions->OptimazeInMessageView)
			{//будем показывать каждый принятый символ
				static CString hex_view=_T("");	//Сюда будет наращиваться шестнадцатеричный вид принимаемой посылки
				if(input_count==1)
				{//это первый символ посылки, очистим буфер
					hex_view="";
				}
							
				//покажем информацию о принимаемом сообщении
				CString AboutMessage;
				if(Chepuha)
				{
					AboutMessage=_T("Гонится спам...");
				}
				else if(FirstByteReady || TheEnd)	
				{//сообщение либо уже принято, либо нормально начато
					AboutMessage=_T("Начато новое сообщение");
				}

				if(input_count>pProtocolOptions->var_LEN_STATE)
				{
					if(LenCorrect)
					{
						AboutMessage.Append(_T("; Поле длины корректно"));
					}
					else
					{	
						if(!Chepuha)
						{//Если гонится спам, то нечего и показывать предупреждающее сообщение
							AboutMessage.Append(_T("; Неверное поле длины"));
						}
					}
					if(TheEnd==true)
					{//приняли все сообщение (ну или переполнили буфер чепухой)
						if(SumCorrect)
						{
							if(pProtocolOptions->OnVerify!=NULL)
							{
								pProtocolOptions->OnVerify(&AboutMessage,ReceiveMessage);
							}
						}
						else
						{
							AboutMessage.Append(_T("; Контрольная сумма некорректна"));
						}
						//нарастим число принятых пакетов
						AddInMessageCount();
						//заполню поля принятого сообщения
						lsReseivingMessage.length=input_count;
						lsReseivingMessage.name=AboutMessage;
						//запишем в файл
						WriteMesFile(&lsReseivingMessage,UART_MSG_RECEIVED);
					}
				}
				//покажем информацию о принятом сообщении
				pUSART_Dlg->m_InMessageError.SetWindowText(AboutMessage);

				//получим шестнадцатеричный вид очередного символа
				hex_view.Append(GiveStrFromHex(&ch,1));	
				
				//покажем на экране само принятое сообщение
				pUSART_Dlg->m_ReceiveMessage.SetWindowText(hex_view);

			}
			else
			{//похоже для нас важно быстродействие, отобразим только полностью принятые сообщения
				if(TheEnd==true)
				{//только что закончили принимать сообщения
					//переведем в удобочитаемый вид
					CString str=GiveStrFromHex(ReceiveMessage,input_count);

					CString AboutMessage;
					if(!Chepuha && LenCorrect && SumCorrect)
					{//если все было нормально, то можно еще и проверифицировать
						if(pProtocolOptions->OnVerify!=NULL)
						{
							pProtocolOptions->OnVerify(&AboutMessage,ReceiveMessage);
						}
					}
					else if(!LenCorrect) AboutMessage=_T("Некорректное поле длины");
					else if(!SumCorrect) AboutMessage=_T("Неверная контрольная сумма");

					pUSART_Dlg->m_ReceiveMessage.SetWindowText(str);//покажем сообщение 
					//покажем информацию о принимаемом сообщении
					pUSART_Dlg->m_InMessageError.SetWindowText(AboutMessage);
					//нарастим число принятых пакетов
					AddInMessageCount();
					//заполню поля принятого сообщения
					lsReseivingMessage.length=input_count;
					lsReseivingMessage.name=AboutMessage;
					//запишем в файл
					WriteMesFile(&lsReseivingMessage,UART_MSG_RECEIVED);
				}
			}
		}
		else
		{//покажем сообщение так, как оно есть
			USES_CONVERSION;
			pUSART_Dlg->m_ReceiveMessage.SetWindowText(A2T((char*)ReceiveMessage));
		}

		if(TheEnd && pProtocolOptions->NotifyProgram!=NULL)
		{//оповестим главную программу об удачном или ошибочном приеме
			if(!Chepuha && LenCorrect && SumCorrect)
			{//ошибок нет
				pProtocolOptions->NotifyProgram(NP_MESSAGE_RESEIVED);
			}
			else
			{//есть ошибки, но они на уровне протокола, а не железа
				pProtocolOptions->NotifyProgram(NP_RECEIVE_PROTOCOL_ERROR);
			}
		}
	}
	else if (pProtocolOptions->var_Regime==BYTE_STUFFING)
	{//РЕЖИМ ПРИЕМО-ПЕРЕДАЧИ С БАЙТСТАФФИНГОМ

		/*
		Включен протокол с элементарным байт-стаффингом. Отличается отсутствием даже предположений на длину пакета,
		а значит конец пакета отслеживается только по началу следующего. Следовательно, всегда существует задержка
		в один пакет (что ПЛОХО)
		*/

		static CString	error		=	_T("Первое сообщение");		//прослеживает ошибки в течении приема
		static bool		staf_flag	=	0;							//будет отслеживать байт стаффинг


		if(pred_char==pProtocolOptions->var_FIRST_BYTE)
		{
			if(ch!=pProtocolOptions->var_STUF_BYTE)
			{//новое сообщение
				if(char_count>=3)
				{//при этом это не первое сообщение, и мне нужно сначала закончить предыдущее
					//здесь мы проверяем корректность чек-суммы предыдущего пакета, т.к. в этом дурном протоколе нету
					INT8U sim=pUSART_Dlg->pPROTOCOL->ReceiveMessage[char_count-3]; //-3, т.к. теоретически сумма была передана задолго назад
				
					INT8U sim2=pProtocolOptions->CURRENT_SUMM(pUSART_Dlg->pPROTOCOL->ReceiveMessage,1,char_count-3);
					bool lError;	//чтобы в конце знать, была ли ошибка
					if(sim!=sim2)
					{
						error=_T("Ошибка чек-суммы");
						lError=true;
					}
					else
					{
						error=_T("Нет ошибок");
						lError=false;
					}

					pUSART_Dlg->pPROTOCOL->AddInMessageCount();//нарастим число принятых пакетов
				
					//Если не было ошибок в пакете, то распознаем
					if(!lError)
					{//Сообщение корректно и его можно распознать
						if(!pProtocolOptions->OnVerify(&error, pUSART_Dlg->pPROTOCOL->ReceiveMessage))
						{
							return false;//там произошла глобальная ошибка
						}
					}
					else
					{
						//error="Пакет принят успешно"; //начнем новую жизнь
					}

					//заполню поля принятого сообщения
					lsReseivingMessage.length=char_count-2;
					lsReseivingMessage.name=error;
					//запишем в файл
					WriteMesFile(&lsReseivingMessage,UART_MSG_RECEIVED);
					
					char_count=2; //т.к. 2 символа уже принято
					
					//перезапишу упущенный первый симовол
					pUSART_Dlg->pPROTOCOL->ReceiveMessage[0]=pProtocolOptions->var_FIRST_BYTE;

					if(pProtocolOptions->NotifyProgram!=NULL)
					{//оповестим главную программу об удачном или ошибочном приеме
						if(!lError)
						{//ошибок нет
							pProtocolOptions->NotifyProgram(NP_MESSAGE_RESEIVED);
						}
						else
						{//есть ошибки, но они на уровне протокола, а не железа
							pProtocolOptions->NotifyProgram(NP_RECEIVE_PROTOCOL_ERROR);
						}
					}
				}
			}
			else
			{//это байт стаффинг
				char_count--;	//этого символа как-будто не было
				//Дальше мы принудительно не запишем этот символ
			}
		}

		if(pred_char!=pProtocolOptions->var_FIRST_BYTE || ch!=pProtocolOptions->var_STUF_BYTE)
		{//если принятый символ - не стартовый и не байтстаффинг
			//то просто допишу очередной символ в буфер
			pUSART_Dlg->pPROTOCOL->ReceiveMessage[char_count]=0;//чтобы не было глюков при отображении на экране
			pUSART_Dlg->pPROTOCOL->ReceiveMessage[char_count-1]=ch;
		}

		if(pUSART_Dlg->m_ReceiveMessage.m_hWnd!=NULL)
		{//чтобы не попытаться вывести в несуществующее окно
			//Если тщательно обмозговать это место, то можно качественно ускорить выполнение
			CString str=GiveStrFromHex(pUSART_Dlg->pPROTOCOL->ReceiveMessage,char_count);
			pUSART_Dlg->m_ReceiveMessage.SetWindowText(str);//покажем сообщение 
		}

		if(char_count>=pProtocolOptions->var_MAX_LEN-1)//переполнение буфера, а по сети гонится лажа
		{//начнем сначала
			lsReseivingMessage.length=char_count;
			error=_T("Переполнение приемного буфера");
			lsReseivingMessage.name=error;
			//запишем в файл
			WriteMesFile(&lsReseivingMessage,UART_MSG_RECEIVED);
			char_count=0;
		}

		pUSART_Dlg->m_InMessageError.SetWindowText(error);//покажем ошибку
	}
	else if(pProtocolOptions->var_Regime==EXTERN_FUNC)
	{//ПЕРЕДАЧА ПРИНЯТОГО СИМВОЛА ВНЕШНЕЙ ФУНКЦИИ
		if(pProtocolOptions->ExternReceiveChar==NULL)
		{
			m_pError->SetERROR(_T("Указатель ExternReceiveChar при обращении оказался равным нулю"), ERP_ERROR | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
			return false;
		}
		if(!pProtocolOptions->ExternReceiveChar(ch))
		{
			m_pError->SetERROR(_T("Зазбоила функция ExternReceiveChar"), ERP_ERROR | ER_WRITE_TO_LOG);
			return false;
		}
		return true;
	}
	else 
	{//НЕЙЗВЕТНЫЙ РЕЖИМ
		m_pError->SetERROR(_T("Неопознанный режим обработки приянятого символа"), ERP_ERROR | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
		return false;
	}
	
	pred_char=ch; //чтобы потом был предыдущий символ
	return true;
}

PROTOCOL_Struct::~PROTOCOL_Struct(void)
{
	if(ReceiveMessage!=NULL)
	{//удалим массив-буфер приемника
		delete [] ReceiveMessage;
	}
}

bool PROTOCOL_Struct::WriteMesFile(CMessage *pMessage, INT8U code)
		//Я ЗНАЮ, ЧТО ЭТО ГЛЮК, НО РАБОТАЕТ ТОЛЬКО ТАК, ИНАЧЕН НЕ ОПРЕДЕЛЯЕТСЯ КАК
		//ВНЕШНИЙ СИМВОЛ ДЛЛ
{//Записывает в файл принятые и отправленные сообщения
	/*
	in_out==true - записывается отправленное сообщение
	in_out==false - принятое сообщение
	*/
	if(!(pProtocolOptions->o_LogInMessage || pProtocolOptions->o_LogOutMessage))
	{//просто в установках не стоит запрос на запись отправляемых или принимаемых 
	//сообщений и просто выходим
		return true;	
	}
	if(!InMesFileOpened)
	{//файл куда пишутся сообщения еще не был открыт
		InMesFileOpened=true;	//выставим флаг, что он открыт
		//Создаем файл листинга полученных сообщений
		InMesFileOpen();
	}
	if(ReceiveMessageFile.is_open())
	{
		CStringA str;					//т.к. в файл пока пишется не UNICODE!!!
		if(code==UART_MSG_TRANSEIVED)
		{//это отправленное сообщение
			if(pProtocolOptions->o_LogOutMessage) //есть разрешение на запись
			{
				if(pProtocolOptions->o_LogTimeMessages)
				{//нужно зафиксировать время отправки
					//Сначала запишем время
					SYSTEMTIME time;
					GetLocalTime(&time);
					
					str.Format("\n%i:%02i %02is %ims  ",time.wHour,time.wMinute,time.wSecond,time.wMilliseconds);
					ReceiveMessageFile.write(str,str.GetLength());
				}
				str="Отправлен пакет: \n";
				ReceiveMessageFile.write(str,str.GetLength());
				str.Format("%i)\t",OutMessageCount);
				ReceiveMessageFile.write(str,str.GetLength());
				ReceiveMessageFile.write("   ",3);
				//сам пакет

				if(pProtocolOptions->o_HexDisplayOutMessage)//может нужно писать шестнадцатеричные числа?
				{//запишем шестнадцатеричные значения
					str=GiveStrFromHex(pMessage->text,pMessage->length);
				}
				else
				{//строковая запись
					str=pMessage->text;
				}

				ReceiveMessageFile.write(str,str.GetLength());
				ReceiveMessageFile.write("     ",5);
				//возникшие ошибки
				USES_CONVERSION;
				ReceiveMessageFile.write(T2A(pMessage->name.GetBuffer()),pMessage->name.GetLength());

				ReceiveMessageFile<<endl;
				ReceiveMessageFile.flush();		//сброшу данные на жёсткий диск
			}
		}
		else if(code==UART_MSG_RECEIVED)
		{//это принятое сообщение
			if(pProtocolOptions->o_LogTimeMessages)
			{//нужно зафиксировать время получения
				//Сначала запишем время
				SYSTEMTIME time;
				GetLocalTime(&time);
				
				str.Format("\n%i:%02i %02is %ims  ",time.wHour,time.wMinute,time.wSecond,time.wMilliseconds);
				ReceiveMessageFile.write(str,str.GetLength());
			}
			str="Принят пакет: \n";
			ReceiveMessageFile.write(str,str.GetLength());
			str.Format("%i)  \t",InMessageCount);
			ReceiveMessageFile.write(str,5);
			ReceiveMessageFile.write("   ",3);
			//сам пакет

			if(pProtocolOptions->o_HexDisplayInMessage)//может нужно писать шестнадцатеричные числа?
			{//запишем шестнадцатеричные значения
				str=GiveStrFromHex(pMessage->text,pMessage->length);
			}
			else
			{//строковая запись
				str=pMessage->text;
			}

			ReceiveMessageFile.write(str,str.GetLength());
			ReceiveMessageFile.write("     ",5);
			//возникшие ошибки
			USES_CONVERSION;
			ReceiveMessageFile.write(T2A(pMessage->name.GetBuffer()),pMessage->name.GetLength());

			ReceiveMessageFile<<endl;
			ReceiveMessageFile.flush();		//сброшу данные на жёсткий диск
		}
		else if(code==UART_MSG_NOTTRANCEIVED)
		{//неудачная попытка отправить сообщение
			if(pProtocolOptions->o_LogTimeMessages)
			{//нужно зафиксировать время
				//Сначала запишем время
				SYSTEMTIME time;
				GetLocalTime(&time);
				
				str.Format("\n%i:%02i %02is %ims  ",time.wHour,time.wMinute,time.wSecond,time.wMilliseconds);
				ReceiveMessageFile.write(str,str.GetLength());
			}
			str="Не получилось отправить сообщение: \n";
			ReceiveMessageFile.write(str,str.GetLength());
			str.Format("%i)  \t",InMessageCount);
			ReceiveMessageFile.write(str,5);
			ReceiveMessageFile.write("   ",3);
			//сам пакет

			if(pProtocolOptions->o_HexDisplayInMessage)//может нужно писать шестнадцатеричные числа?
			{//запишем шестнадцатеричные значения
				str=GiveStrFromHex(pMessage->text,pMessage->length);
			}
			else
			{//строковая запись
				str=pMessage->text;
			}

			ReceiveMessageFile.write(str,str.GetLength());
			ReceiveMessageFile.write("     ",5);
			//возникшие ошибки
			USES_CONVERSION;
			ReceiveMessageFile.write(T2A(pMessage->name.GetBuffer()),pMessage->name.GetLength());

			ReceiveMessageFile<<endl;
			ReceiveMessageFile.flush();		//сброшу данные на жёсткий диск
		}
		else
		{//непонятный код сообщения
			return false;
		}
		
	}
	return true;	//все прошло нормально
}	


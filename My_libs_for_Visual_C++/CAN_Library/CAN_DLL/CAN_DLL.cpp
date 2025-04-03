// CAN_DLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "CAN_DLL.h"


/***********************************************************************
*			ЭКСПОРТИРУЕМЫЕ ПЕРЕМЕННЫЕ
***********************************************************************/

/**********************************************************************
*			ИМПОРТИРУЕМЫЕ ПЕРЕМЕННЫЕ
***********************************************************************/
extern HWND	g_hPseudoWindow;	//хэндл моего псевдо-окна из dllmain.cpp

/**********************************************************************
*			ДЕКЛАРАЦИЯ ВНУТРЕННИХ КЛАССОВ
**********************************************************************/
// Структура, определяющая скорость CAN
// Подробности о настройке скорости нужно искать в даташите на PIC18F4585 контроллер
struct CAN_DLL_API CCAN_Speed
{
	//конструкторы
	CCAN_Speed(){};
	CCAN_Speed(BYTE brp, BYTE propseg, BYTE phseg1, BYTE phseg2, BYTE sjw);
	BYTE BRP;		//от 0 до 63 - отвечает за время тика
	BYTE PROPSEG;	//|
	BYTE PHSEG1;	//| от 0 до 7 - в сумме дают количество тиков в бите
	BYTE PHSEG2;	//|
	BYTE SJW;		// от 0 до 3 - не учавствует в подсчете скорости, а так..., 
					// я рекомендую SJW==1
};

/**********************************************************************
*			ВНУТРЕННИЕ ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ
**********************************************************************/
//для портов
CSerialPort		g_COM_Port;
//хэндл окна, которому я буду посылать сообщение при приеме CAN посылки
HWND g_MesReceiveWindow;

//ОПИСАТЕЛИ СОБЫТИЙ
HANDLE g_hg_ReceiveErrorsCountMes;	//получено сообщение с количеством ошибок при приеме
HANDLE g_hg_TranceiveErrorsCountMes;	//			>>					>>		  при передаче
HANDLE g_hBusOffMessage;			//	>>		>>		 о состоянии "Bus-Off"
HANDLE g_hCAN_SpeedMessage;			//	>>		>>		 со скоростью CAN
HANDLE g_hFilterSetOK;				//	>>		>>		 с подтверждением изменения фильтра
HANDLE g_hMaskSetOK;				//	>>		>>		 с подтверждением изменения маски
HANDLE g_hSpeedSetOK;				//	>>		>>		 с подтверждением изменения скорости
HANDLE g_hRegimeSetOK;				//	>>		>>		 с подтверждением изменения режима настройки скорости
HANDLE g_hCAN_MesSended;			//	>>		>>		 что CAN сообщение отправлено
HANDLE g_hFilterEnDisOK;			//	>>		>>		 с подтверждением включения/выключения фильтра
HANDLE g_hFilterRequestOK;			//	>>		>>		 со значением фильтра
HANDLE g_hMaskRequestOK;			//	>>		>>		 со значением маски
HANDLE g_hRegimeRequestOK;			//	>>		>>		 со значением режима работы преобразователя

//Здесь находятся переменные, через которые я буду возвращать результат приема сообщения по RS-232
//Можно было бы обойтись много меньшим количеством перменных, но я решил не путать карты, а сделать
//для каждого сообщнеия своё место возвращения результата.
volatile INT8U		g_ReceiveErrorsCount;
volatile INT8U		g_TranceiveErrorsCount;
volatile bool		g_Bus_Off;
volatile CCAN_Speed g_CAN_SpeedResult;
volatile INT8U		g_FilterSetResult;
volatile INT8U		g_MaskSetResult;
volatile INT8U		g_SpeedSetResult;
volatile INT8U		g_RegimeSetResult;
volatile INT8U		g_SendMesCAN_Result;
volatile INT8U		g_FilterEnDisResult;
volatile INT32U		g_FilterValue;
volatile bool		g_IsExtended;
volatile bool		g_IsEnable;
volatile INT32U		g_MaskValue;
volatile INT8U		g_RegimeGetResult;

const INT8U COMPUTER_ID			= 0x51;				// Идентификатор компьютера в сети RS-232
const INT8U	CONTROLLER_ID		= 0x50;				//		>>		>>		контроллера
const WORD	CONTROLLER_VID		= 0x10C4;			//| 
const WORD	CONTROLLER_PID		= 0xEA60;			//| идентификаторы CP210x в сети USB
const TCHAR	CONTROLLER_SERIAL[]	= _TEXT("1234\0");	//  строка серийного номера преобразователя в сети USB
const INT32U F_OSC = 40000000;						//частота кварца микроконтроллера

bool g_bConnected=false;			//Подключен ли преобразователь
									// true - да
									// false - нет
bool g_bCOM_Forsibly;				//Был ли открыт преобразователь через СОМ порт
									// true - да
									// false - нет

DWORD	g_dTimeOut=1000;			//Время в мс, в течение которого я буду ждать 
									//ответа от преобразователя

/**********************************************************************
*			ДЕКЛАРАЦИЯ ВНУТРЕННИХ ФУНКЦИЙ
***********************************************************************/
//дополнительная инициализация
void InitCAN_DLL(void);
//Эта фукнция будет принимать сообщения от Serial Port и 
//определять, что делать дальше
void CAN_Supervisor(UINT Msg, WPARAM wParam, LPARAM lParam);
//Распознает полученные по RS-232 сообщения
bool CAN_RS232_Verify(BYTE *pReceiveMessage, WORD MesageLength);
//принимает символ по RS-232
bool ReceiveChar(INT8U ch);
//обработчик сообщения WM_DEVICECHANGE
void OnDeviceChange(WPARAM wParam, LPARAM lParam);
//проверяет подключен ли преобразователь к компьютеру
void UpdateConnected();
//инициализирует COM порт на преобразователь
CAN_RESULT InitializeCOM();
//проверяет на корректность CAN сообщение
bool VerifyCAN_Message(CCAN_Message* pCAN_Message);

//Ухищрения при настройке скорости
CAN_RESULT	CAN_SpeedRegFind(DWORD BaudRate,CCAN_Speed *pCAN_Speed=NULL, float* pError=NULL, DWORD* pActualSpeed=NULL);
bool		CAN_CheckSpeedRight (const CCAN_Speed *pCAN_Speed);
DWORD		CAN_CalculateSpeed	(CCAN_Speed *pCAN_Speed);
CAN_RESULT	CAN_GetSpeed(CCAN_Speed *pCAN_Speed);
CAN_RESULT	CAN_SetSpeed(const CCAN_Speed *pCAN_Speed);

/***********************************************************************
*				МЕТОДЫ СТРУКТУРЫ CCAN_Speed
***********************************************************************/
//конструктор
CCAN_Speed::CCAN_Speed(BYTE brp, BYTE propseg, BYTE phseg1, BYTE phseg2, BYTE sjw)
{
	BRP=brp; PROPSEG=propseg; PHSEG1=phseg1; PHSEG2=phseg2; SJW=sjw;
}

/************************************************************************
*					ВНУТРЕННИЕ ФУНКЦИИ
************************************************************************/
/**********************    InitCAN_DLL      *****************************
*		После запуска пользовательской программы вызывается функция 
*	dllmain() в файле dllmain.cpp, однако, т.к. у меня куча локольных
*	переменных, экспортировать которые не хочется, то я оттуда просто
*	вызову эту функцию.
************************************************************************/
void InitCAN_DLL(void)
{
	g_MesReceiveWindow=NULL;

	//Создам события, которые будут сигнализировать о приходе того 
	//или иного сообщения по сети RS-232
	g_hg_ReceiveErrorsCountMes	= CreateEvent(NULL, TRUE, TRUE, NULL);	//получено сообщение с количеством ошибок при приеме
	g_hg_TranceiveErrorsCountMes	= CreateEvent(NULL, TRUE, TRUE, NULL);	//			>>					>>		  при передаче
	g_hBusOffMessage			= CreateEvent(NULL, TRUE, TRUE, NULL);	//	>>		>>		 о состоянии "Bus-Off"
	g_hCAN_SpeedMessage			= CreateEvent(NULL, TRUE, TRUE, NULL);	//	>>		>>		 со скоростью CAN
	g_hFilterSetOK				= CreateEvent(NULL, TRUE, TRUE, NULL);	//	>>		>>		 с подтверждением изменения фильтра
	g_hMaskSetOK				= CreateEvent(NULL, TRUE, TRUE, NULL);	//	>>		>>		 с подтверждением изменения маски
	g_hSpeedSetOK				= CreateEvent(NULL, TRUE, TRUE, NULL);	//	>>		>>		 с подтверждением изменения скорости
	g_hRegimeSetOK				= CreateEvent(NULL, TRUE, TRUE, NULL);	//	>>		>>		 с подтверждением изменения режима настройки скорости
	g_hCAN_MesSended			= CreateEvent(NULL, TRUE, TRUE, NULL);	//	>>		>>		 что CAN сообщение отправлено
	g_hFilterEnDisOK			= CreateEvent(NULL, TRUE, TRUE, NULL);	//	>>		>>		 с подтверждением включения/выключения фильтра
	g_hFilterRequestOK			= CreateEvent(NULL, TRUE, TRUE, NULL);	//	>>		>>		 со значением фильтра
	g_hMaskRequestOK			= CreateEvent(NULL, TRUE, TRUE, NULL);	//	>>		>>		 со значением маски
	g_hRegimeRequestOK			= CreateEvent(NULL, TRUE, TRUE, NULL);	//	>>		>>		 со значением режима работы преобразователя

	g_bCOM_Forsibly=false;		//принудительно через последовательный порт не подключались

	//узнаю, подключен ли преобразователь
	UpdateConnected();
	//и, если подключен, то настрою порт
	InitializeCOM();
}
/**********************    OnDeviceChange     ***************************
*		Это обработчик сообщения WM_DEVICECHANGE, которое пресылается 
*	моему фиктивному окну в файле dllmain.cpp когда меняется список
*	подключенных к системе устройств. По ней я определяю, когда воткнули или
*	выткнули преобразователь.
*************************************************************************/
void OnDeviceChange(WPARAM wParam, LPARAM lParam)
{
	if(g_bCOM_Forsibly)	return;		//идет работа принудительно через COM порт

	switch(wParam)
	{
	case DBT_DEVICEARRIVAL:			//устройство воткнули и оно готово к работе
		//если преобразователь уже
		//подключен, то зачем проверять еще один
		if(CAN_IsMonitorConnected()==true) return;	
		break;
	case DBT_DEVICEREMOVECOMPLETE:	//устройство вытащили из компьютера
		//если преобразователя нет и в помине, то при отключении
		//какой-то хуйни не буду поднимать панику
		if(CAN_IsMonitorConnected()==false) return;
		break;
	default:						//там есть еще несколько довольно интересных значений
		return;    
	}
	//если я прошёл блок switch, то значит я могу надеятся, что подключили 
	//или отключили преобразователь
	
	bool lOldConnected=CAN_IsMonitorConnected();
	UpdateConnected();		//заного просмотрю список подключенных устройств
	if(lOldConnected==false && CAN_IsMonitorConnected()==true)
	{//преобразователь только-что подключили
		if(InitializeCOM()==CR_ALLOK)
		{//подключились нормально
			PostMessage(g_MesReceiveWindow,MSG_NEW_RECEIVE,(WPARAM)NULL,CTF_STATE_CHANGED);
		}
	}
	else if(lOldConnected==true  && CAN_IsMonitorConnected()==false)
	{//преобразователь только-что выключили
		PostMessage(g_MesReceiveWindow,MSG_NEW_RECEIVE,(WPARAM)NULL,CTF_STATE_CHANGED);
	}
}

/**************************    UpdateConnected    ***********************************
*		Проверяет подключен ли преобразователь к компьютеру. Просто преверяются
*	все подключенные к компу CP210x устройства, и если среди них наш преобразователь
*	- я обновляю глобальную переменную g_bConnected. Для поддрежания многозадачности -
*	я исключил все паразитные ложные переключения g_bConnected, которая в каждый момент
*	времени содержит действительное значение.
*************************************************************************************/
void UpdateConnected()
{
	DWORD lCP210xCount;
	CP210x_STATUS lRes=CP210x_GetNumDevices(&lCP210xCount);
	if(lRes!=CP210x_SUCCESS)
	{//что-то с не так с функцией
		g_bConnected=false;	//буду считать, что ничего не подключено
		return;
	}
	for(DWORD i=0; i<lCP210xCount;i++)
	{//цикл по всем подключенным CP210x
		CP210x_DEVICE_STRING lDevString;
		lRes=CP210x_GetProductString(i,lDevString,CP210x_RETURN_FULL_PATH);
		if(lRes==CP210x_SUCCESS)
		{//функция отработала нормально
			CHAR lBuf[5];			//|
			lBuf[0]=lDevString[26];	//|
			lBuf[1]=lDevString[27];	//| скопирую только ту часть огромедной строки, в которой содержится 
			lBuf[2]=lDevString[28];	//| серийный номер
			lBuf[3]=lDevString[29];	//|
			lBuf[4]=0;				//|
			//сравнения все в UNICODE
			WCHAR lSerStringW[256];
			wsprintf(lSerStringW,L"%S\0",lBuf);
			if(lstrcmp(lSerStringW,CONTROLLER_SERIAL)==0)
			{//строки равны, это преобразователь!
				g_bConnected=true;	//|
				return;				//| преобразователь подключен!
			}
		}
	}
	g_bConnected=false;			//|
	return;						//| так и не нашли требуемое устройство
}


/************************    InitializeCOM    *********************************
*		Если преобразователь подключен к компьютеру, то мне нужно настроить 
*	последовательный порт на работу с ним. Этим и занимается функция.
******************************************************************************/
CAN_RESULT InitializeCOM()
{
	//узнаю, подключен ли преобразователь к компьютеру
	if(!CAN_IsMonitorConnected())
	{//Преобразователь не подключен
		return CR_NOTCONNECTED;
	}

	//узнаю номер последовательного порта преобразователя
	int lPortNum=GetPortNumXP2000Vista(CONTROLLER_VID, CONTROLLER_PID, (TCHAR*)CONTROLLER_SERIAL);
	if(lPortNum==-1)
	{//о преобразователе нет записи в реестре, хотя он подключен: странно-странно?
		g_bConnected=false;
		return CR_OTHERERROR;
	}

	//инициализация этого порта
	if(g_COM_Port.InitPort(NULL,				//окно, которому будут передаваться все сообщения
						lPortNum,				//порт, который нужно настроить
						115200,					//скорость передачи
						'N',					//контроль четности
						8,						//битность
						1))						//стоповых битов
	{//ПОРТ НАЙДЕН УСПЕШНО
		//пердам указатель на функцию-супервизор, которая будет обслуживать
		//события на порту
		if(!g_COM_Port.SetParentSupervisor(CAN_Supervisor))
		{//ошибки
			g_bConnected=false;
			return CR_OTHERERROR;
		}
	}
	else
	{//НЕ НАШЛИ ПОРТОВ
		g_bConnected=false;
		return CR_OTHERERROR;
	}

	return CR_ALLOK;
}

/**************************    VerifyCAN_Message    *******************************
*		Проверяет на корректность CAN сообщение. Пока проверяется только длина
*	данных, т.к. неверное значение в этом поле может накрыть медным тазом протокол
*	передачи.
***********************************************************************************/
bool VerifyCAN_Message(CCAN_Message* pCAN_Message)
{
	if(pCAN_Message==NULL) return false;
	if(pCAN_Message->DataLen>8) return false;
	return true;
}

/**********************     CAN_COM_Open    ****************************************
*		На платке преобразователя есть также обычный последовательный выход, если 
*	глючный CP2102 не пашет (что обычно бывает при отладке), то эта функция принудительно 
*	инициализирует устройство через последовательный порт
***********************************************************************************/
CAN_DLL_API CAN_RESULT	CAN_COM_Open(unsigned char port_num)
{
	//инициализация этого порта
	if(g_COM_Port.InitPort(NULL,					//окно, которому будут передаваться все сообщения
						port_num,				//порт, который нужно настроить
						115200,					//скорость передачи
						'N',					//контроль четности
						8,						//битность
						1))						//стоповых битов
	{//ПОРТ НАЙДЕН УСПЕШНО
		//пердам указатель на функцию-супервизор, которая будет обслуживать
		//события на порту
		if(!g_COM_Port.SetParentSupervisor(CAN_Supervisor))
		{//ошибки
			g_bCOM_Forsibly=false;
			return CR_OTHERERROR;
		}
	}
	else
	{//НЕ НАШЛИ ПОРТОВ
		g_bCOM_Forsibly=false;
		return CR_OTHERERROR;
	}
	g_bCOM_Forsibly=true;		//я подключился через последовательный порт
	return CR_ALLOK;
}
/**************************    CAN_IsCOM_Opened    *********************************
*		Если COM порт был открыт принудительно - то возвращается истина, в противном
*	случае - ложь.В большинстве случаев эта функци не нужна, она может понадобиться, 
*	когда придется распознавать - подключен ли преобразователь к COM порту.
***********************************************************************************/
CAN_DLL_API bool CAN_IsCOM_Opened(void)
{
	return g_bCOM_Forsibly;
}

/**************************    CAN_IsMonitorConnected   ****************************
*		Данная функция определяет - подключен ли преобразователь к компьютеру
*	или нет.
*		true - преобразователь подключен и CP2102 инициализирована, либо была 
*				принудительная инициализация через функцию CAN_COM_Open
*		false - не подключен
***********************************************************************************/
CAN_DLL_API bool CAN_IsMonitorConnected()
{
	if(g_bCOM_Forsibly) return true;	//ведется работа через COM порт, поэтому естесственно
										//преобразователь подключен
	//подключен ли он через USB
	return g_bConnected;
}

/*************************    CAN_SetMesReceiveWindow    ***************************
*		Устанавливает указатель на окно, которое будет принимать сообщения по CAN
*	hWnd - хэндл окна
*	Если все нормально, то возвращает CR_ALLOK. Если что-то не то с хэндлом, то
*	CR_BADARGUMENTS
************************************************************************************/
CAN_RESULT	CAN_SetMesReceiveWindow(HWND hWnd)
{
	if(hWnd==NULL || hWnd==INVALID_HANDLE_VALUE )return CR_BADARGUMENTS;
	g_MesReceiveWindow=hWnd;
	return CR_ALLOK;
}

/*************************    CAN_SetTimeOut    ***********************************
*		Устанавливает тайм-аут ожидания ответа преобразователя.
*	time_ms - время тайм-аута в миллисекундах. Должно быть не меньше 10 мс.
*	Если все нормально, то возвращает CR_ALLOK. Если что-то не то с time_ms, то
*	CR_BADARGUMENTS
***********************************************************************************/
CAN_DLL_API CAN_RESULT CAN_SetTimeOut(DWORD time_ms)
{
	if(time_ms<10) return CR_BADARGUMENTS;
	g_dTimeOut=time_ms;							//изменю время
	return CR_ALLOK;
}

/************************    CAN_GetTimeOut    *************************************
*		Возвращает установленный тайм-аут ожидания ответа преобразователя.
***********************************************************************************/
CAN_DLL_API DWORD	CAN_GetTimeOut(void)
{
	return g_dTimeOut;
}

/*************************     CAN_CheckSpeedRight     *************************************
*	Функция проверяет правильность значений скорости CAN согласно даташиту PIC18F4585
*********************************************************************************************/
bool CAN_CheckSpeedRight (const CCAN_Speed *pCAN_Speed)
{
	if(pCAN_Speed==NULL)	return false;

	if(pCAN_Speed->BRP    <0 || pCAN_Speed->BRP    >63){return false;}
	if(pCAN_Speed->PROPSEG<0 || pCAN_Speed->PROPSEG>7) {return false;}
	if(pCAN_Speed->PHSEG1 <0 || pCAN_Speed->PHSEG1 >7) {return false;}
	if(pCAN_Speed->PHSEG2 <0 || pCAN_Speed->PHSEG2 >7) {return false;}
	if(pCAN_Speed->SJW    <0 || pCAN_Speed->SJW    >3) {return false;}
	if(pCAN_Speed->PHSEG2 > (pCAN_Speed->PHSEG1+pCAN_Speed->PROPSEG)){return false;}
	if(pCAN_Speed->PHSEG2 < pCAN_Speed->SJW)		   {return false;}
	return true;
}

/*********************     CAN_CalculateSpeed     ***************************************
*	Преобразует ненаглядные значения регистров микроконтроллера в скорость в бодах.
*	Чтобы понять механизм работы см. даташит PIC18F4585
*		0 - функция отработала с ошибками
****************************************************************************************/
DWORD	CAN_CalculateSpeed	(CCAN_Speed *pCAN_Speed)
{
	if(!CAN_CheckSpeedRight (pCAN_Speed)) return 0;

	DWORD temp=F_OSC/(2*(pCAN_Speed->BRP+1));					 //1 делить на время одного кванта (тика)
	DWORD lBaud=temp/(1+pCAN_Speed->PROPSEG+pCAN_Speed->PHSEG1+pCAN_Speed->PHSEG2+3);	//общее время бита

	return lBaud;
}

/*********************    CAN_SpeedRegFind   ******************************************
*		Подсчет конкретных регистров для заданной скорости весьма трудная задача.
*	В этой функции я попытаюсь автоматизировать этот процесс.
*		BaudRate - требуемая скорость в бодах (например 500 000 Бит/с)
*		pCAN_Speed - указатель на структуру скорости, которая будет принимать результат (м.б. NULL)
*		pError  - указатель на переменную, которая будет принимать ошибку (м.б. NULL)
*		pActualSpeed - указатель на переменную, которая будет принимать действительную скорость (м.б. NULL)
**************************************************************************************/
CAN_RESULT	CAN_SpeedRegFind(DWORD BaudRate,CCAN_Speed *pCAN_Speed, float* pError, DWORD* pActualSpeed)
{
	INT32S lMaxMatch=100000000;
	CCAN_Speed lSpeed(0,0,0,0,1);
	
	for(lSpeed.BRP=0; lSpeed.BRP<63; lSpeed.BRP++)
		for(lSpeed.PROPSEG=0; lSpeed.PROPSEG<7; lSpeed.PROPSEG++)
			for(lSpeed.PHSEG1=0; lSpeed.PHSEG1<7; lSpeed.PHSEG1++)
				for(lSpeed.PHSEG2=0; lSpeed.PHSEG2<7; lSpeed.PHSEG2++)
				{
					//если скорость невозможна, то не буду ее рассматривать
					if(!CAN_CheckSpeedRight(&lSpeed)) continue;
					INT32S lCurSpeed=CAN_CalculateSpeed	(&lSpeed);
					if(abs(lCurSpeed-(INT32S)BaudRate)<abs(lMaxMatch-(INT32S)BaudRate))
					{//этот набор подходит лучше
						lMaxMatch=lCurSpeed;
						if(pCAN_Speed!=NULL)
						{
							pCAN_Speed->BRP=lSpeed.BRP;
							pCAN_Speed->PROPSEG=lSpeed.PROPSEG;
							pCAN_Speed->PHSEG1=lSpeed.PHSEG1;
							pCAN_Speed->PHSEG2=lSpeed.PHSEG2;
						}
					}

				}
	//подсчитаю ошибку в процентах
	if(pError!=NULL) *pError=(FP32)abs(lMaxMatch-(INT32S)BaudRate)/(FP32)BaudRate*100.f;
	//запишу наиболее подходящую скорость
	if(pActualSpeed!=NULL) *pActualSpeed=lMaxMatch;
	//ну и проверенный, но не влияющий на скорость SJW
	if(pCAN_Speed!=NULL) pCAN_Speed->SJW=1;

	return CR_ALLOK;
}

/**************************     CAN_CalcActualSpeed    **************************************
*		Это урезанная версия CAN_SpeedRegFind, доступная извне DLL
********************************************************************************************/
CAN_DLL_API CAN_RESULT	CAN_CalcActualSpeed(DWORD BaudRate, float* pError, DWORD* pActualSpeed)
{
	return CAN_SpeedRegFind(BaudRate,NULL, pError, pActualSpeed);
}

/**************************    CAN_SendMessage     **********************************
*		Отправляет по сети CAN сообщение.
*	pCAN_Message - указатель на отправляемое сообщение
*		Возвращаемые значения:
*	CR_ALLOK		- сообщение успешно отправлено
*	CR_NOTCONNECTED - преобразователь не подключен
*	CR_BADARGUMENTS - pCAN_Message равен NULL или сообщение некорректно
*	CR_DEVICEBUSY	- преобразователь не смог отправить сообщение, видно он не подк-
*						лючен к CAN сети или настроен не на ту скорость
*	CR_NOANSWER		- преобразователь не отвечает 
*	CR_OTHERERROR	- другие непонятные ошибки
************************************************************************************/
CAN_DLL_API CAN_RESULT CAN_SendMessage(CCAN_Message* pCAN_Message)
{
	if(!CAN_IsMonitorConnected())		return CR_NOTCONNECTED;	//проверка на подключение
	if(!VerifyCAN_Message(pCAN_Message))return CR_BADARGUMENTS;	//проверка аргуметнов

	/*******************************************************************
	*	Отправка по CAN сообщения с компьютера
	********************************************************************/
	static INT8U message[35];
	INT8U i=0; 

	//ПОШЛА ШАПКА СООБЩЕНИЯ
	message[i++]='$';						//первый символ посылки
	message[i++]=COMPUTER_ID;				//идентификатор контроллера
	message[i++]=0x25;						//Message Type
	message[i++]=pCAN_Message->DataLen+6;	//Data Length
	
	//ПОШЛИ ДАННЫЕ
	*((INT32U*)&message[i++])=pCAN_Message->ID; i++;i++;i++;	//идентификатор
	message[i++]=pCAN_Message->DataLen;							//длина поля данных
	message[i++]=pCAN_Message->Flags;							//дополнительные флаги
	for(INT8U k=0; k<pCAN_Message->DataLen; k++)
	{//копирование байтов данных (копировать будем все 8 байт)
		message[i++]=pCAN_Message->Data[k];
	}
	
	//ПОШЛА КОНТРОЛЬНАЯ СУММА
	INT8U sum=CRC_8_polinom_8541(message,1,i);					//первый байт в сумму не входит
	message[i]=sum;
	message[i+1]=0x00;											//на всякий случай
	
	//отправлю сообщение
	if(!g_COM_Port.WriteToPort(message,i+1))
	{//что-то там не срослось
		return CR_OTHERERROR;
	}

	//настроюсь на ожидание ответа от контроллера
	if(!ResetEvent(g_hCAN_MesSended))
	{
		return CR_OTHERERROR;
	}

	//буду ждать ответа
	DWORD tmp=WaitForSingleObject(g_hCAN_MesSended, CAN_GetTimeOut()/*INFINITE*/);
	if(tmp==WAIT_OBJECT_0)
	{//контроллер ответил, все в порядке
		//здесь писать нечего
		if(g_SendMesCAN_Result!=0)
			return CR_DEVICEBUSY;
	}
	else if(tmp==WAIT_TIMEOUT)
	{//контроллер не ответил
		SetEvent(g_hCAN_MesSended);
		return CR_NOANSWER;
	}
	else
	{//не пашет Windows
		return CR_OTHERERROR;
	}

	return CR_ALLOK;
}

/************************    CAN_Getg_ReceiveErrorsCount    ************************
*		Возвращает количество ошибок при приеме (по спецификации CAN 2.0B).
*		pCount - указатель на переменную, которая будет принимать результат.
*		Возвращаемые значения:
*	Если все прошло нормально			- CR_ALLOK
*	pCount равно NULL					- CR_BADARGUMENTS
*	Если преобразователь не подключет	- CR_NOTCONNECTED
*	Если преобразователь не отвечает	- CR_NOANSWER
*	Другие непонятные ошибки			- CR_OTHERERROR
************************************************************************************/
CAN_DLL_API CAN_RESULT CAN_GetReceiveErrorsCount(unsigned char* pCount)
{
	if(pCount==NULL)				return CR_BADARGUMENTS;	//некуда писать результат
	if(!CAN_IsMonitorConnected())	return CR_NOTCONNECTED;	//проверка на подключение

	static INT8U message[15];
	INT8U i=0; 

	//ПОШЛА ШАПКА СООБЩЕНИЯ
	message[i++]='$';					//первый символ посылки
	message[i++]=COMPUTER_ID;			//идентификатор контроллера
	message[i++]=0x10;					//Message Type
	message[i++]=0x00;					//Data Length
	//ПОШЛИ ДАННЫЕ
	
	//ПОШЛА КОНТРОЛЬНАЯ СУММА
	INT8U sum=CRC_8_polinom_8541(message,1,i);				//первый байт в сумму не входит
	message[i]=sum;
	message[i+1]=0x00;										//на всякий случай
	
	//отправлю сообщение
	if(!g_COM_Port.WriteToPort(message,i+1))
	{//что-то там не срослось
		return CR_OTHERERROR;
	}

	//настроюсь на ожидание ответа от контроллера
	if(!ResetEvent(g_hg_ReceiveErrorsCountMes))
	{
		return CR_OTHERERROR;
	}

	//буду ждать ответа
	DWORD tmp=WaitForSingleObject(g_hg_ReceiveErrorsCountMes, CAN_GetTimeOut());
	if(tmp==WAIT_OBJECT_0)
	{//контроллер ответил, все в порядке
		*pCount=g_ReceiveErrorsCount;
	}
	else if(tmp==WAIT_TIMEOUT)
	{//контроллер не ответил
		SetEvent(g_hg_ReceiveErrorsCountMes);
		return CR_NOANSWER;
	}
	else
	{//не пашет Windows
		return CR_OTHERERROR;
	}

	return CR_ALLOK;
}

/************************    CAN_Getg_ReceiveErrorsCount    ************************
*		Возвращает количество ошибок при передаче (по спецификации CAN 2.0B).
*		pCount - указатель на переменную, которая будет принимать результат.
*		Возвращаемые значения:
*	Если все прошло нормально			- CR_ALLOK
*	pCount равно NULL					- CR_BADARGUMENTS
*	Если преобразователь не подключет	- CR_NOTCONNECTED
*	Если преобразователь не отвечает	- CR_NOANSWER
*	Другие непонятные ошибки			- CR_OTHERERROR
************************************************************************************/
CAN_DLL_API CAN_RESULT CAN_GetTranceiveErrorsCount(unsigned char* pCount)
{
	if(pCount==NULL)				return CR_BADARGUMENTS;	//некуда писать результат
	if(!CAN_IsMonitorConnected())	return CR_NOTCONNECTED;	//проверка на подключение

	static INT8U message[15];
	INT8U i=0; 

	//ПОШЛА ШАПКА СООБЩЕНИЯ
	message[i++]='$';					//первый символ посылки
	message[i++]=COMPUTER_ID;			//идентификатор контроллера
	message[i++]=0x11;					//Message Type
	message[i++]=0x00;					//Data Length
	//ПОШЛИ ДАННЫЕ
	
	//ПОШЛА КОНТРОЛЬНАЯ СУММА
	INT8U sum=CRC_8_polinom_8541(message,1,i);		//первый байт в сумму не входит
	message[i]=sum;
	message[i+1]=0x00;										//на всякий случай
	
	//отправлю сообщение
	if(!g_COM_Port.WriteToPort(message,i+1))
	{//что-то там не срослось
		return CR_OTHERERROR;
	}

	//настроюсь на ожидание ответа от контроллера
	if(!ResetEvent(g_hg_TranceiveErrorsCountMes))
	{
		return CR_OTHERERROR;
	}

	//буду ждать ответа
	DWORD tmp=WaitForSingleObject(g_hg_TranceiveErrorsCountMes, CAN_GetTimeOut());
	if(tmp==WAIT_OBJECT_0)
	{//контроллер ответил, все в порядке
		*pCount=g_TranceiveErrorsCount;
	}
	else if(tmp==WAIT_TIMEOUT)
	{//контроллер не ответил
		SetEvent(g_hg_TranceiveErrorsCountMes);
		return CR_NOANSWER;
	}
	else
	{//не пашет Windows
		return CR_OTHERERROR;
	}

	return CR_ALLOK;
}

/***********************    CAN_GetBusOff     ***************************************
*	Проверяет преобразователь на состояние "Bus-Off" (по спецификации CAN)
*		pBusOff - указатель на переменную, которая будет принимать результат
*	Возвращаемые значения:
*		CR_ALLOK		- функция успешно отработала
*		CR_NOTCONNECTED - преобразователь не подключен к компьютеру
*		CR_NOANSWER		- преобразователь не отвечает на запросы
*		CR_OTHERERROR	- другая ошибка
*		*pBusOff ==	true  - преобразователь находится в состоянии Bus-Off
*					false - в активном состоянии
************************************************************************************/
CAN_DLL_API CAN_RESULT CAN_GetBusOff(bool *pBusOff)
{
	if(pBusOff==NULL)				return CR_BADARGUMENTS;	//некуда писать результат
	if(!CAN_IsMonitorConnected())	return CR_NOTCONNECTED;	//проверка на подключение

	static INT8U message[15];
	INT8U i=0; 

	//ПОШЛА ШАПКА СООБЩЕНИЯ
	message[i++]='$';					//первый символ посылки
	message[i++]=COMPUTER_ID;			//идентификатор контроллера
	message[i++]=0x12;					//Message Type
	message[i++]=0x00;					//Data Length
	//ПОШЛИ ДАННЫЕ
	
	//ПОШЛА КОНТРОЛЬНАЯ СУММА
	INT8U sum=CRC_8_polinom_8541(message,1,i);				//первый байт в сумму не входит
	message[i]=sum;
	message[i+1]=0x00;										//на всякий случай
	
	//отправлю сообщение
	if(!g_COM_Port.WriteToPort(message,i+1))
	{//что-то там не срослось
		return CR_OTHERERROR;
	}

	//настроюсь на ожидание ответа от контроллера
	if(!ResetEvent(g_hBusOffMessage))
	{
		return CR_OTHERERROR;
	}

	//буду ждать ответа
	DWORD tmp=WaitForSingleObject(g_hBusOffMessage, CAN_GetTimeOut());
	if(tmp==WAIT_OBJECT_0)
	{//контроллер ответил, все в порядке
		*pBusOff=g_Bus_Off;
	}
	else if(tmp==WAIT_TIMEOUT)
	{//контроллер не ответил
		SetEvent(g_hBusOffMessage);
		return CR_NOANSWER;
	}
	else
	{//не пашет Windows
		return CR_OTHERERROR;
	}

	return CR_ALLOK;//скомпоновали нормально
}

/**************************    CAN_GetSpeed    **************************************
*		Запрос скорости, на которую настроен преобразователь.
************************************************************************************/
CAN_RESULT CAN_GetSpeed(CCAN_Speed *pCAN_Speed)
{
	if(pCAN_Speed==NULL)			return CR_BADARGUMENTS;	//некуда писать результат
	if(!CAN_IsMonitorConnected())	return CR_NOTCONNECTED;	//проверка на подключение

	static INT8U message[15];
	INT8U i=0; 

	//ПОШЛА ШАПКА СООБЩЕНИЯ
	message[i++]='$';					//первый символ посылки
	message[i++]=COMPUTER_ID;			//идентификатор контроллера
	message[i++]=0x13;					//Message Type
	message[i++]=0x00;					//Data Length
	//ПОШЛИ ДАННЫЕ
	
	//ПОШЛА КОНТРОЛЬНАЯ СУММА
	INT8U sum=CRC_8_polinom_8541(message,1,i);				//первый байт в сумму не входит
	message[i]=sum;
	message[i+1]=0x00;										//на всякий случай
	
	//отправлю сообщение
	if(!g_COM_Port.WriteToPort(message,i+1))
	{//что-то там не срослось
		return CR_OTHERERROR;
	}

	//настроюсь на ожидание ответа от контроллера
	if(!ResetEvent(g_hCAN_SpeedMessage))
	{
		return CR_OTHERERROR;
	}

	//буду ждать ответа
	DWORD tmp=WaitForSingleObject(g_hCAN_SpeedMessage, CAN_GetTimeOut());
	if(tmp==WAIT_OBJECT_0)
	{//контроллер ответил, все в порядке
		pCAN_Speed->BRP		= g_CAN_SpeedResult.BRP;
		pCAN_Speed->PROPSEG	= g_CAN_SpeedResult.PROPSEG;
		pCAN_Speed->PHSEG1	= g_CAN_SpeedResult.PHSEG1;
		pCAN_Speed->PHSEG2	= g_CAN_SpeedResult.PHSEG2;
		pCAN_Speed->SJW		= g_CAN_SpeedResult.SJW;
	}
	else if(tmp==WAIT_TIMEOUT)
	{//контроллер не ответил
		SetEvent(g_hCAN_SpeedMessage);
		return CR_NOANSWER;
	}
	else
	{//не пашет Windows
		return CR_OTHERERROR;
	}

	return CR_ALLOK;//скомпоновали нормально
}

/***********************   CAN_GetSpeed    ****************************************
*		Экспортируемая во внешний мир копия CAN_GetSpeed(CCAN_Speed *pCAN_Speed)
***********************************************************************************/
CAN_DLL_API CAN_RESULT	CAN_GetSpeed(DWORD* pBaudRate)
{
	if(pBaudRate==NULL) return CR_BADARGUMENTS;

	//свяжусь с преобразователем и узнаю заначения регистров
	CCAN_Speed lCAN_Speed;
	CAN_RESULT lRes=CAN_GetSpeed(&lCAN_Speed);
	if(lRes!=CR_ALLOK)	return lRes;

	*pBaudRate=CAN_CalculateSpeed(&lCAN_Speed);
	return CR_ALLOK;
}

/***********************    CAN_SetSpeed     ****************************************
*		Выставляет новую скорость CAN на преобразователе.
************************************************************************************/
CAN_RESULT CAN_SetSpeed(const CCAN_Speed *pCAN_Speed)
{
	if(!CAN_IsMonitorConnected()) return CR_NOTCONNECTED;		//проверка на подключение
	if(!CAN_CheckSpeedRight(pCAN_Speed)) return CR_BADARGUMENTS;//скорость некорректна
	
	static INT8U message[25];
	INT8U i=0; 

	//ПОШЛА ШАПКА СООБЩЕНИЯ
	message[i++]='$';					//первый символ посылки
	message[i++]=COMPUTER_ID;			//идентификатор контроллера
	message[i++]=0x23;					//Message Type
	message[i++]=0x05;					//Data Length
	//ПОШЛИ ДАННЫЕ
	message[i++]=pCAN_Speed->BRP;
	message[i++]=pCAN_Speed->PROPSEG;
	message[i++]=pCAN_Speed->PHSEG1;
	message[i++]=pCAN_Speed->PHSEG2;
	message[i++]=pCAN_Speed->SJW;

	//ПОШЛА КОНТРОЛЬНАЯ СУММА
	INT8U sum=CRC_8_polinom_8541(message,1,i);				//первый байт в сумму не входит
	message[i]=sum;
	message[i+1]=0x00;										//на всякий случай
	
	//отправлю сообщение
	if(!g_COM_Port.WriteToPort(message,i+1))
	{//что-то там не срослось
		return CR_OTHERERROR;
	}

	//настроюсь на ожидание ответа от контроллера
	if(!ResetEvent(g_hSpeedSetOK))
	{
		return CR_OTHERERROR;
	}

	//буду ждать ответа
	DWORD tmp=WaitForSingleObject(g_hSpeedSetOK, CAN_GetTimeOut());
	if(tmp==WAIT_OBJECT_0)
	{//контроллер ответил, все в порядке
		if(g_SpeedSetResult==0)
		{//скорость CAN изменена успешно
			//даже не знаю, что сюда написать
		}
		else
		{//преобразователь не смог изменить свою скорость CAN
			return CR_DEVICEBUSY;
		}
	}
	else if(tmp==WAIT_TIMEOUT)
	{//контроллер не ответил
		SetEvent(g_hSpeedSetOK);
		return CR_NOANSWER;
	}
	else
	{//не пашет Windows
		return CR_OTHERERROR;
	}

	return CR_ALLOK;//скомпоновали нормально
}

/**********************    CAN_SetSpeed   *******************************************
*		Это доступный извне аналог функции CAN_SetSpeed(const CCAN_Speed *pCAN_Speed)
************************************************************************************/
CAN_DLL_API CAN_RESULT	CAN_SetSpeed(DWORD BaudRate)
{
	CCAN_Speed lCAN_Speed;
	//подсчитаю регистры
	CAN_RESULT lRes=CAN_SpeedRegFind(BaudRate,&lCAN_Speed);
	if(lRes!=CR_ALLOK)	return lRes;

	//собственно смена скорости CAN в преобразователе
	lRes=CAN_SetSpeed(&lCAN_Speed);
	return lRes;
}

/**********************    CAN_SetRegime    ************************************
*		Устанавливает режим работы преобразователя. (см. общее описание)
*		Regime - требуемый режим работы. Одно из трёх значений 
*					RG_NORMAL, RG_SPEEDFIND, RG_AUTOSEND
*	Возвращаемые значения:
*		Если все прошло нормально			- CR_ALLOK
*		неизвестный Regime					- CR_BADARGUMENTS
*		Если преобразователь не подключет	- CR_NOTCONNECTED
*		Если преобразователь не отвечает	- CR_NOANSWER
*		Другие непонятные ошибки			- CR_OTHERERROR
**********************************************************************************/
CAN_DLL_API CAN_RESULT CAN_SetRegime(BYTE Regime)
{
	if(Regime!=RG_NORMAL && Regime!=RG_SPEEDFIND && Regime!=RG_AUTOSEND)
									return CR_BADARGUMENTS;	//недопустимый режим
	if(!CAN_IsMonitorConnected())	return CR_NOTCONNECTED;	//проверка на подключение

	static INT8U message[15];
	INT8U i=0; 

	//ПОШЛА ШАПКА СООБЩЕНИЯ
	message[i++]='$';					//первый символ посылки
	message[i++]=COMPUTER_ID;			//идентификатор контроллера
	message[i++]=0x22;					//Message Type
	message[i++]=0x01;					//Data Length
	//ПОШЛИ ДАННЫЕ
	message[i++]=Regime;
	//ПОШЛА КОНТРОЛЬНАЯ СУММА
	INT8U sum=CRC_8_polinom_8541(message,1,i);				//первый байт в сумму не входит
	message[i]=sum;
	message[i+1]=0x00;										//на всякий случай
	
	//отправлю сообщение
	if(!g_COM_Port.WriteToPort(message,i+1))
	{//что-то там не срослось
		return CR_OTHERERROR;
	}

	//настроюсь на ожидание ответа от контроллера
	if(!ResetEvent(g_hRegimeSetOK))
	{
		return CR_OTHERERROR;
	}

	//буду ждать ответа
	DWORD tmp=WaitForSingleObject(g_hRegimeSetOK, CAN_GetTimeOut());
	if(tmp==WAIT_OBJECT_0)
	{//контроллер ответил, все в порядке
		if(g_RegimeSetResult==0)
		{//режим настройки скорости был изменен успешно
			//не знаю, что сюда писать
		}
		else
		{//преобразователь не смог выполнить операцию
			return CR_DEVICEBUSY;
		}
	}
	else if(tmp==WAIT_TIMEOUT)
	{//контроллер не ответил
		SetEvent(g_hRegimeSetOK);
		return CR_NOANSWER;
	}
	else
	{//не пашет Windows
		return CR_OTHERERROR;
	}

	return CR_ALLOK;//скомпоновали нормально
}

/************************     CAN_GetRegime     ***********************************
*		Возвращает режим работы преобразователя.
*		pRegime	- указатель на переменную, которая будет принимать текущий режим.
*	Возвращаемые значения:
*		Если все прошло нормально			- CR_ALLOK
*		pRegime==NULL						- CR_BADARGUMENTS
*		Если преобразователь не подключет	- CR_NOTCONNECTED
*		Если преобразователь не отвечает	- CR_NOANSWER
*		Другие непонятные ошибки			- CR_OTHERERROR
***********************************************************************************/
CAN_DLL_API CAN_RESULT	CAN_GetRegime(BYTE* pRegime)
{
	if(pRegime==NULL)				return false;			//некуда писать результат
	if(!CAN_IsMonitorConnected())	return CR_NOTCONNECTED;	//проверка на подключение

	static INT8U message[15];
	INT8U i=0; 

	//ПОШЛА ШАПКА СООБЩЕНИЯ
	message[i++]='$';					//первый символ посылки
	message[i++]=COMPUTER_ID;			//идентификатор контроллера
	message[i++]=0x16;					//Message Type
	message[i++]=0x00;					//Data Length
	//ПОШЛИ ДАННЫЕ
	//ПОШЛА КОНТРОЛЬНАЯ СУММА
	INT8U sum=CRC_8_polinom_8541(message,1,i);				//первый байт в сумму не входит
	message[i]=sum;
	message[i+1]=0x00;										//на всякий случай
	
	//отправлю сообщение
	if(!g_COM_Port.WriteToPort(message,i+1))
	{//что-то там не срослось
		return CR_OTHERERROR;
	}

	//настроюсь на ожидание ответа от контроллера
	if(!ResetEvent(g_hRegimeRequestOK))
	{
		return CR_OTHERERROR;
	}

	//буду ждать ответа
	DWORD tmp=WaitForSingleObject(g_hRegimeRequestOK, CAN_GetTimeOut());
	if(tmp==WAIT_OBJECT_0)
	{//контроллер ответил, все в порядке
		*pRegime=g_RegimeGetResult;							//результат запроса
	}
	else if(tmp==WAIT_TIMEOUT)
	{//контроллер не ответил
		SetEvent(g_hRegimeRequestOK);
		return CR_NOANSWER;
	}
	else
	{//не пашет Windows
		return CR_OTHERERROR;
	}

	return CR_ALLOK;//скомпоновали нормально
}

/*****************************     CAN_SetFilter     *******************************
*		Изменяет значение фильтра входящих сообщений на преобразователе.
*		FilterNum			- номер фильтра от 0 до 15
*		FilterValue			- Значение фильтра, которое нужно установить
*								от 0 до (2^11)-1 если фильтр стандартный
*								от 0 до (2^29)-1 если фильтр расширенный
*							Если значение вылезает за допустимые рамки, то 
*							оно просто обрезается.
*		IsFilterExtended	-	true - фильтр для расширенных сообщений
*								false - для стандартных
*	Возвращаемые значения:
*		Если все прошло нормально			- CR_ALLOK
*		Недопустимый номер фильтра			- CR_BADARGUMENTS
*		Если преобразователь не подключет	- CR_NOTCONNECTED
*		Если преобразователь не отвечает	- CR_NOANSWER
*		Другие непонятные ошибки			- CR_OTHERERROR
************************************************************************************/
CAN_DLL_API CAN_RESULT CAN_SetFilter(BYTE FilterNum, DWORD FilterValue, bool IsFilterExtended)
{
	if(FilterNum>15)				return CR_BADARGUMENTS;	//недопустимый номер фильтра
	if(!CAN_IsMonitorConnected())	return CR_NOTCONNECTED;	//проверка на подключение

	static INT8U message[15];
	INT8U i=0; 

	//ПОШЛА ШАПКА СООБЩЕНИЯ
	message[i++]='$';					//первый символ посылки
	message[i++]=COMPUTER_ID;			//идентификатор контроллера
	message[i++]=0x20;					//Message Type
	if(IsFilterExtended)
		message[i++]=0x05;				//длина с расширенным идентификатором
	else
		message[i++]=0x03;				//длина с обычным идентификатором
	//ПОШЛИ ДАННЫЕ
	message[i++]=FilterNum;				//номер фильтра
	INT8U* mas=(INT8U*)&FilterValue;	//указатель
	message[i++]=mas[0];	//|
	message[i++]=mas[1];	//|	покрайней мере для стандартных сообщений фильр задали
	if(IsFilterExtended)	
	{//фильтр для расширенных сообщений
		message[i++]=mas[2];
		message[i++]=mas[3];
	}
	//ПОШЛА КОНТРОЛЬНАЯ СУММА
	INT8U sum=CRC_8_polinom_8541(message,1,i);				//первый байт в сумму не входит
	message[i]=sum;
	message[i+1]=0x00;										//на всякий случай

	//отправлю сообщение
	if(!g_COM_Port.WriteToPort(message,i+1))
	{//что-то там не срослось
		return CR_OTHERERROR;
	}

	//настроюсь на ожидание ответа от контроллера
	if(!ResetEvent(g_hFilterSetOK))
	{
		return CR_OTHERERROR;
	}

	//буду ждать ответа
	DWORD tmp=WaitForSingleObject(g_hFilterSetOK, CAN_GetTimeOut());
	if(tmp==WAIT_OBJECT_0)
	{//контроллер ответил, все в порядке
		if(g_FilterSetResult==0)
		{//фильтр был изменен успешно
			//не знаю, что сюда писать
		}
		else
		{//преобразователь не смог выполнить операцию
			return CR_DEVICEBUSY;
		}
	}
	else if(tmp==WAIT_TIMEOUT)
	{//контроллер не ответил
		SetEvent(g_hFilterSetOK);
		return CR_NOANSWER;
	}
	else
	{//не пашет Windows
		return CR_OTHERERROR;
	}

	return CR_ALLOK;//скомпоновали нормально
}

/*****************************    CAN_GetFilter     **************************************
*		Возвращает значение, режим и включенность/выключенность фильтра.
*	FilterNum - номер фильтра от 0 до 15
*	pFilterValue		- сюда будет записано значение фильтра
*	pIsFilterExtended	- сюда будет записано является ли фильтр расширенным
*							true	- расширенный идентификатор
*							false	- стандартный идентификатор
*	pIsFilterEnable		- сюда будет записановключен ли фильтр
*							true	- да
*							false	- нет
*	pFilterValue,pIsFilterExtended,pIsFilterEnabled м.б. нулевыми, в этом случае в 
*	соответствующие позиции результат не пишется.
*	Возвращаемые значения:
*		Если все прошло нормально			- CR_ALLOK
*		Недопустимый номер фильтра			- CR_BADARGUMENTS
*		Если преобразователь не подключет	- CR_NOTCONNECTED
*		Если преобразователь не отвечает	- CR_NOANSWER
*		Другие непонятные ошибки			- CR_OTHERERROR
*****************************************************************************************/
CAN_DLL_API CAN_RESULT	CAN_GetFilter(BYTE FilterNum, DWORD *pFilterValue, bool* pIsFilterExtended, bool* pIsFilterEnabled)
{
	if(!CAN_IsMonitorConnected()) return CR_NOTCONNECTED;	//проверка на подключение
	if(FilterNum>15)	return CR_BADARGUMENTS;				//нет такого фильтра

	static INT8U message[10];
	INT8U i=0; 

	//ПОШЛА ШАПКА СООБЩЕНИЯ
	message[i++]='$';					//первый символ посылки
	message[i++]=COMPUTER_ID;			//идентификатор контроллера
	message[i++]=0x14;					//Message Type
	message[i++]=0x01;					//длина поля данных
	//ПОШЛИ ДАННЫЕ
	message[i++]=FilterNum;				//номер фильтра
	//ПОШЛА КОНТРОЛЬНАЯ СУММА
	INT8U sum=CRC_8_polinom_8541(message,1,i);				//первый байт в сумму не входит
	message[i]=sum;
	message[i+1]=0x00;										//на всякий случай

	//отправлю сообщение
	if(!g_COM_Port.WriteToPort(message,i+1))
	{//что-то там не срослось
		return CR_OTHERERROR;
	}

	//настроюсь на ожидание ответа от контроллера
	if(!ResetEvent(g_hFilterRequestOK))
	{
		return CR_OTHERERROR;
	}

	//буду ждать ответа
	DWORD tmp=WaitForSingleObject(g_hFilterRequestOK, CAN_GetTimeOut());
	if(tmp==WAIT_OBJECT_0)
	{//контроллер ответил, все в порядке
		//результаты запроса
		if(pFilterValue!=NULL)		*pFilterValue		= g_FilterValue;
		if(pIsFilterExtended!=NULL) *pIsFilterExtended	= g_IsExtended;
		if(pIsFilterEnabled!=NULL)	*pIsFilterEnabled	= g_IsEnable;
	}
	else if(tmp==WAIT_TIMEOUT)
	{//контроллер не ответил
		SetEvent(g_hFilterRequestOK);
		return CR_NOANSWER;
	}
	else
	{//не пашет Windows
		return CR_OTHERERROR;
	}

	return CR_ALLOK;//скомпоновали нормально
}


/*****************************    CAN_EnableFilter    ************************************
*		Включает/отключает работу фильтра идентификаторов сети CAN на преобразователе
*	FilterNum	- номер фильтра от 0 до 15
*	IsEnable	- true  - фильтр нужно включить
*				  false - фильтр нужно выключить
*	Возвращаемые значения:
*		Если все прошло нормально			- CR_ALLOK
*		Недопустимый номер фильтра			- CR_BADARGUMENTS
*		Если преобразователь не подключет	- CR_NOTCONNECTED
*		Если преобразователь не отвечает	- CR_NOANSWER
*		Другие непонятные ошибки			- CR_OTHERERROR
*****************************************************************************************/
CAN_DLL_API CAN_RESULT	CAN_EnableFilter(BYTE FilterNum, bool IsEnable)
{
	if(FilterNum>15)				return CR_BADARGUMENTS;	//нет такого фильтра
	if(!CAN_IsMonitorConnected())	return CR_NOTCONNECTED;	//проверка на подключение

	static INT8U message[10];
	INT8U i=0; 

	//ПОШЛА ШАПКА СООБЩЕНИЯ
	message[i++]='$';					//первый символ посылки
	message[i++]=COMPUTER_ID;			//идентификатор контроллера
	message[i++]=0x26;					//Message Type
	message[i++]=0x02;					//длина поля данных
	//ПОШЛИ ДАННЫЕ
	message[i++]=FilterNum;				//номер фильтра
	message[i++]=IsEnable?1:0;			//включен/выключен
	
	//ПОШЛА КОНТРОЛЬНАЯ СУММА
	INT8U sum=CRC_8_polinom_8541(message,1,i);				//первый байт в сумму не входит
	message[i]=sum;
	message[i+1]=0x00;										//на всякий случай

	//отправлю сообщение
	if(!g_COM_Port.WriteToPort(message,i+1))
	{//что-то там не срослось
		return CR_OTHERERROR;
	}

	//настроюсь на ожидание ответа от контроллера
	if(!ResetEvent(g_hFilterEnDisOK))
	{
		return CR_OTHERERROR;
	}

	//буду ждать ответа
	DWORD tmp=WaitForSingleObject(g_hFilterEnDisOK, CAN_GetTimeOut());
	if(tmp==WAIT_OBJECT_0)
	{//контроллер ответил, все в порядке
		if(g_FilterEnDisResult==0)
		{//фильтр был изменен успешно
			//не знаю, что сюда писать
		}
		else
		{//преобразователь не смог выполнить операцию
			return CR_DEVICEBUSY;
		}
	}
	else if(tmp==WAIT_TIMEOUT)
	{//контроллер не ответил
		SetEvent(g_hFilterEnDisOK);
		return CR_NOANSWER;
	}
	else
	{//не пашет Windows
		return CR_OTHERERROR;
	}

	return CR_ALLOK;//скомпоновали нормально
}

/********************    CAN_SetMask    **********************************************
*		Изменяет значение маски для фильтров входящих сообщений на преобразователе.
*		MaskValue - новое значение маски от 0 до (2^29)-1
*					Если значение превышает допустимые пределы, то оно просто обрезается.
*	Возвращаемые значения:
*		Если все прошло нормально			- CR_ALLOK
*		Если преобразователь не подключет	- CR_NOTCONNECTED
*		Если преобразователь не отвечает	- CR_NOANSWER
*		Другие непонятные ошибки			- CR_OTHERERROR
*************************************************************************************/
CAN_DLL_API CAN_RESULT CAN_SetMask(DWORD MaskValue)
{
	if(!CAN_IsMonitorConnected()) return CR_NOTCONNECTED;	//проверка на подключение

	static INT8U message[15];
	INT8U i=0; 

	//ПОШЛА ШАПКА СООБЩЕНИЯ
	message[i++]='$';					//первый символ посылки
	message[i++]=COMPUTER_ID;			//идентификатор контроллера
	message[i++]=0x21;					//Message Type
	message[i++]=0x04;					//длина поля данных
	//ПОШЛИ ДАННЫЕ
	INT8U* mas=(INT8U*)&MaskValue;		//указатель
	message[i++]=mas[0];				
	message[i++]=mas[1];				
	message[i++]=mas[2];
	message[i++]=mas[3];
	
	//ПОШЛА КОНТРОЛЬНАЯ СУММА
	INT8U sum=CRC_8_polinom_8541(message,1,i);				//первый байт в сумму не входит
	message[i]=sum;
	message[i+1]=0x00;										//на всякий случай

	//отправлю сообщение
	if(!g_COM_Port.WriteToPort(message,i+1))
	{//что-то там не срослось
		return CR_OTHERERROR;
	}

	//настроюсь на ожидание ответа от контроллера
	if(!ResetEvent(g_hMaskSetOK))
	{
		return CR_OTHERERROR;
	}

	//буду ждать ответа
	DWORD tmp=WaitForSingleObject(g_hMaskSetOK, CAN_GetTimeOut());
	if(tmp==WAIT_OBJECT_0)
	{//контроллер ответил, все в порядке
		if(g_MaskSetResult==0)
		{//фильтр был изменен успешно
			//не знаю, что сюда писать
		}
		else
		{//преобразователь не смог выполнить операцию
			return CR_DEVICEBUSY;
		}
	}
	else if(tmp==WAIT_TIMEOUT)
	{//контроллер не ответил
		SetEvent(g_hMaskSetOK);
		return CR_NOANSWER;
	}
	else
	{//не пашет Windows
		return CR_OTHERERROR;
	}

	return CR_ALLOK;
}

/***********************     CAN_GetMask     ****************************************
*		Возвращает значение маски сети CAN на микроконтроллере.
*		pMaskValue - указатель на переменную, которая будет принимать значение маски
*	Возвращаемые значения:
*		Если все прошло нормально			- CR_ALLOK
*		pMaskValue == NULL					- CR_BADARGUMENTS
*		Если преобразователь не подключет	- CR_NOTCONNECTED
*		Если преобразователь не отвечает	- CR_NOANSWER
*		Другие непонятные ошибки			- CR_OTHERERROR
************************************************************************************/
CAN_DLL_API CAN_RESULT	CAN_GetMask(DWORD* pMaskValue)
{
	if(pMaskValue==NULL)			return CR_BADARGUMENTS;	//некуда писать результат
	if(!CAN_IsMonitorConnected())	return CR_NOTCONNECTED;	//проверка на подключение

	static INT8U message[10];
	INT8U i=0; 

	//ПОШЛА ШАПКА СООБЩЕНИЯ
	message[i++]='$';					//первый символ посылки
	message[i++]=COMPUTER_ID;			//идентификатор контроллера
	message[i++]=0x15;					//Message Type
	message[i++]=0x00;					//длина поля данных
	//ПОШЛИ ДАННЫЕ
	//ПОШЛА КОНТРОЛЬНАЯ СУММА
	INT8U sum=CRC_8_polinom_8541(message,1,i);				//первый байт в сумму не входит
	message[i]=sum;
	message[i+1]=0x00;										//на всякий случай

	//отправлю сообщение
	if(!g_COM_Port.WriteToPort(message,i+1))
	{//что-то там не срослось
		return CR_OTHERERROR;
	}

	//настроюсь на ожидание ответа от контроллера
	if(!ResetEvent(g_hMaskRequestOK))
	{
		return CR_OTHERERROR;
	}

	//буду ждать ответа
	DWORD tmp=WaitForSingleObject(g_hMaskRequestOK, CAN_GetTimeOut());
	if(tmp==WAIT_OBJECT_0)
	{//контроллер ответил, все в порядке
		*pMaskValue=g_MaskValue;						//результаты запроса
	}
	else if(tmp==WAIT_TIMEOUT)
	{//контроллер не ответил
		SetEvent(g_hMaskRequestOK);
		return CR_NOANSWER;
	}
	else
	{//не пашет Windows
		return CR_OTHERERROR;
	}

	return CR_ALLOK;//скомпоновали нормально
}

/***********************    CAN_Supervisor     ***************************************
*		Эта фукнция будет принимать сообщения от Serial Port и 
*	определять, что делать дальше. !Функция принадлежит к потоку Serial Port и 
*	может выставлять события!.
*************************************************************************************/
void CAN_Supervisor(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
	case WM_COMM_RXCHAR:
		{//ПРИНЯТ СИМВОЛ
			ReceiveChar((INT8U)wParam);
			break;
		}
	case WM_COMM_ACCESS_DENIED:
		{//ДОСТУП К ПОРТУ ЗАКРЫТ
			//похоже преобразователь выдернули из USB разъема
			g_COM_Port.StopMonitoring();	//перестану общаться с портом
			g_bConnected=false;				//буду считать устройство отключенным
			break;
		}
	case WM_COMM_TXEMPTY_DETECTED:
		{//СООБЩЕНИЕ ОТПРАВЛЕНО
			break;
		}
	}
}

/*************************   ReceiveChar   ************************************
*	дописывает полученный  символ, проверяет чек-сумму, максимальную длину,
*	окончание старого и начало нового пакета. Короче элементарные вещи.
*	Если эта функция приняла пакет без ошибок - можно гарантировать, что пакет
*	соответствует формату протокола. А вот корректные ли внутри него данные
*	решает функция CAN_RS232_Verify()
*	ch - очередной принятый символ
********************************************************************************/
bool ReceiveChar(INT8U ch)
{
	const INT16U	lMAX_LEN=50;						//максимальная длина сообщения
	static INT8U	ReceiveMessage[lMAX_LEN];			//буфер принимаемого сообщения
	const  INT8U	lLEN_ADD		= 0x05;
	const  INT8U	lFIRST_BYTE		= 0x24;
	const  INT8U	lLEN_STATE		= 0x03;
	static INT8U	pred_char		= 0;				//предыдущий символ
	static INT16U	char_count		= 0;				//количество принятых символов
	
	char_count++;										//приняли еще один символ
	
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

	static INT16U paket_length=0;		//длина оставшейся после распознавания части пакета
	static INT16U input_count=0;		//количество принятых байт
	
	INT8U RC_register=ch;				//просто так

	//СЛЕДУЮЩИЙ НАБОР ФЛАГОВ ПРОСТО ОТРАЖАЕТ СОСТОЯНИЕ ПРИЕМА, НО НЕ ВКЛИНИВАЕТСЯ В НЕГО
	static bool Chepuha=true;		//после последнего удачно принятого сообщения еще не принят ни один стартовый символ
	static bool LenCorrect=false;	//принятое поле длины имело корректное значение	
	static bool TheEnd=false;		//только что приняли конец сообщения, его можно отобразить на экране
	static bool SumCorrect=false;

	if(input_count>lMAX_LEN)		//переполнение буфера, а по сети гонится лажа
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
	if(RC_register==lFIRST_BYTE && FirstByteReady==false)
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
		if(input_count==lLEN_STATE+1)//четвертый принятый символ
		{//пакет уже распознан, а по протоколу, в этом месте стоит длина данных в пакете
			if(RC_register>lMAX_LEN-lLEN_ADD)
			{//поле длины посылки слишком большое
				paket_length=lMAX_LEN;
				FirstByteReady=false;	//начнем ожидание новой посылки
				MyTRACE("CAN_DLL Поле длины в принимающемся по RS232 сообщении слишком большое");
			}
			else
			{
				//выставим, какое кол-во символов нам следует принять
				paket_length=RC_register+lLEN_ADD;

				LenCorrect=true;	//поле длины было корректно
			}
		}
		else if(input_count==paket_length)
		{//сообщение принято целиком, пора проверить контрольную сумму
			if(LenCorrect)
			{//если длина была корректна, то можно попробовать и посчитатьконтрольную сумму
				unsigned char sim=ch;
				unsigned char sim2=CRC_8_polinom_8541(ReceiveMessage,1,input_count-1);
				if(sim!=sim2)
				{//ошибка в контрольной сумме
					MyTRACE("CAN_DLL Ошибка контрольной суммы в принимающемся по RS232 сообщении");
				}
				else
				{//контрольная сумма верна
					SumCorrect=true;
					CAN_RS232_Verify(ReceiveMessage, input_count);
				}
			}
			FirstByteReady=false;	//можно начать принимать новое сообщение

			TheEnd=true;			//выставим флаг конца сообщения
		}
	}
	else
	{
 		MyTRACE("CAN_DLL После завершенной по RS-232 посылки пошла чепуха");
	}
	
	pred_char=ch; //чтобы потом был предыдущий символ
	return true;
}

/***********************    CAN_RS232_Verify    ***************************************
*		Распознает полученные по RS-232 сообщения. И в соответствии с этим выставляет
*	соответствующие события.
**************************************************************************************/
bool CAN_RS232_Verify(BYTE *pReceiveMessage, WORD MesageLength)
{
	if(pReceiveMessage==NULL || MesageLength==0)
	{//некорректное сообщение
		return false;
	}

	const INT8U MT_STATE=0x02;							//поле типа сообщения
	const INT8U lMesType=pReceiveMessage[MT_STATE];		//тип принятого сообщения
	const INT8U ID_STATE=0x01;							//поле идентификатора отправителя
	const INT8U lMesID=pReceiveMessage[ID_STATE];		//идентификатор отправителя
	const INT8U DAT_STATE=0x04;							//начало поля данных
	const INT8U LEN_STATE=0x03;							//положение длины поля данных
	const INT8U lDatLen=pReceiveMessage[LEN_STATE];		//длина поля данных

	if(lMesID==COMPUTER_ID)
	{/*ЭТО СООБЩЕНИЕ Я ОТПРАВИЛ САМ СЕБЕ*/
	}
	else if (lMesID==CONTROLLER_ID)
	{/*ЭТО СООБЩЕНИЕ ОТПРАВИЛ КОНТРОЛЛЕР*/
		if(lMesType==0x01)
		{//ПРИНЯТО ОЧЕРЕДНОЕ CAN СООБЩЕНИЕ
			const INT8U lcMasLength=100;							//|
			static INT8U lsMasCounter=0;							//|
			static CCAN_Message ReceivedCAN_MessageMas[lcMasLength];//| см. пояснение ниже
			//скопирую идентификатор
			ReceivedCAN_MessageMas[lsMasCounter].ID=*((DWORD *)&pReceiveMessage[DAT_STATE]);	
			//количество байт данных
			ReceivedCAN_MessageMas[lsMasCounter].DataLen=pReceiveMessage[DAT_STATE+4];
			//скопирую флаги
			ReceivedCAN_MessageMas[lsMasCounter].Flags=pReceiveMessage[DAT_STATE+5];
			//скопирую поле данных
			INT8U i=0;
			for(; i<ReceivedCAN_MessageMas[lsMasCounter].DataLen; i++)
			{
				ReceivedCAN_MessageMas[lsMasCounter].Data[i]=pReceiveMessage[DAT_STATE+6+i];
			}
			//обнулю незанятые ячейки
			for(; i<8;i++) ReceivedCAN_MessageMas[lsMasCounter].Data[i]=0;

			if(g_MesReceiveWindow!=INVALID_HANDLE_VALUE)
			{//вызову внешний обработчик
				//Когда я делаю PostMessage, и в это время приходит
				//еще одно сообщение, то оно затирает статическую структуру
				//ReceivedCAN_Message. SendMessage делать также нельзя, т.к.
				//тогда поток COM порта залезет в поток окна и начнется каша!
				//Поэтому я сделал целый статический массив, который будет
				//заполняться по кругу. Я надеюсь, внешняя программа будет успевать
				//выбирить и расшифровывать сообщения, пока я не пошёл по кругу.
				PostMessage(g_MesReceiveWindow,MSG_NEW_RECEIVE,(WPARAM)&ReceivedCAN_MessageMas[lsMasCounter],0);
				if(lsMasCounter>=lcMasLength)
					lsMasCounter=0;
				else
					lsMasCounter++;
			}
			else
			{//нечем обрабатывать принятое сообщение
				return false;
			}
		}
		else if(lMesType==0x10)
		{//КОЛИЧЕСТВО ОШИБОК ПРИ ПРИЕМЕ
			if(WaitForSingleObject(g_hg_ReceiveErrorsCountMes,0)!=WAIT_TIMEOUT)
				MessageBox(NULL,_TEXT("Преобразователь прислал сообщение с количеством ошибок по приему без запроса"),_TEXT("ОШИБКА ПРЕОБРАЗОВАТЕЛЯ"), MB_ICONWARNING);
			g_ReceiveErrorsCount=pReceiveMessage[DAT_STATE];
			if(!SetEvent(g_hg_ReceiveErrorsCountMes)) return false;
		}
		else if(lMesType==0x11)
		{//КОЛИЧЕСТВО ОШИБОК ПРИ ПЕРЕДАЧЕ
			if(WaitForSingleObject(g_hg_TranceiveErrorsCountMes,0)!=WAIT_TIMEOUT)
				MessageBox(NULL,_TEXT("Преобразователь прислал сообщение с количеством ошибок по передаче без запроса"),_TEXT("ОШИБКА ПРЕОБРАЗОВАТЕЛЯ"), MB_ICONWARNING);
			g_TranceiveErrorsCount=pReceiveMessage[4];
			if(!SetEvent(g_hg_TranceiveErrorsCountMes)) return false;
		}
		else if(lMesType==0x12)
		{//СОСТОЯНИЕ BUS-OFF
			if(WaitForSingleObject(g_hBusOffMessage,0)!=WAIT_TIMEOUT)
				MessageBox(NULL,_TEXT("Преобразователь прислал сообщение с состоянием Bus-Off без запроса"),_TEXT("ОШИБКА ПРЕОБРАЗОВАТЕЛЯ"), MB_ICONWARNING);
			if(pReceiveMessage[4])
				g_Bus_Off=true;			//"Отключент от шины"
			else
				g_Bus_Off=false;			//в активном режиме
			if(!SetEvent(g_hBusOffMessage)) return false;

		}
		else if(lMesType==0x13)
		{//ИНФОРМАЦИЯ О СКОРОСТИ CAN
			if(WaitForSingleObject(g_hCAN_SpeedMessage,0)!=WAIT_TIMEOUT)
				MessageBox(NULL,_TEXT("Преобразователь прислал сообщение со скоростью CAN без запроса"),_TEXT("ОШИБКА ПРЕОБРАЗОВАТЕЛЯ"), MB_ICONWARNING);
			g_CAN_SpeedResult.BRP		= pReceiveMessage[DAT_STATE];
			g_CAN_SpeedResult.PROPSEG	= pReceiveMessage[DAT_STATE+1];
			g_CAN_SpeedResult.PHSEG1	= pReceiveMessage[DAT_STATE+2];
			g_CAN_SpeedResult.PHSEG2	= pReceiveMessage[DAT_STATE+3];
			g_CAN_SpeedResult.SJW		= pReceiveMessage[DAT_STATE+4];
			
			if(!SetEvent(g_hCAN_SpeedMessage)) return false;
		}
		else if(lMesType==0x14)
		{//КОНТРОЛЛЕР ПРИСЛАЛ ЗНАЧЕНИЕ ФИЛЬТРА
			if(WaitForSingleObject(g_hFilterRequestOK,0)!=WAIT_TIMEOUT)
				MessageBox(NULL,_TEXT("Преобразователь прислал сообщение со значением фильтра без запроса"),_TEXT("ОШИБКА ПРЕОБРАЗОВАТЕЛЯ"), MB_ICONWARNING);
			if(lDatLen==4)
			{//в фильтре 2 байта, значит идентификатор стандартный
				g_IsExtended=false;
				g_FilterValue=*((INT16U*)&pReceiveMessage[DAT_STATE+2]);
			}
			else if(lDatLen==6)
			{//в фильтре 4 байта, значит идентификатор расширенный
				g_IsExtended=true;
				g_FilterValue=*((INT32U*)&pReceiveMessage[DAT_STATE+2]);
			}
			else
				MessageBox(NULL,_TEXT("Неверный формат сообщения со значением фильтра"),_TEXT("ОШИБКА ПРОТОКОЛА"), MB_ICONWARNING);
			g_IsEnable=pReceiveMessage[DAT_STATE+1]==0?false:true;	//включен ли фильтр?
			if(!SetEvent(g_hFilterRequestOK)) return false;
		}
		else if(lMesType==0x15)
		{//КОНТРОЛЛЕР ПРИСЛАЛ ЗНАЧЕНИЕ МАСКИ
			if(WaitForSingleObject(g_hMaskRequestOK,0)!=WAIT_TIMEOUT)
				MessageBox(NULL,_TEXT("Преобразователь прислал сообщение со значением маски без запроса"),_TEXT("ОШИБКА ПРЕОБРАЗОВАТЕЛЯ"), MB_ICONWARNING);
			g_MaskValue=*((INT32U*)&pReceiveMessage[DAT_STATE]);
			if(!SetEvent(g_hMaskRequestOK)) return false;

		}
		else if(lMesType==0x16)
		{//КОНТРОЛЛЕР ПРИСЛАЛ СВОЙ РЕЖИМ РАБОТЫ
			if(WaitForSingleObject(g_hRegimeRequestOK,0)!=WAIT_TIMEOUT)
				MessageBox(NULL,_TEXT("Преобразователь прислал сообщение с режимом работы без запроса"),_TEXT("ОШИБКА ПРЕОБРАЗОВАТЕЛЯ"), MB_ICONWARNING);
			g_RegimeGetResult=pReceiveMessage[DAT_STATE];
			if(!SetEvent(g_hRegimeRequestOK)) return false;
		}
		else if(lMesType==0x20)
		{//ПОДТВЕРЖДЕНИЕ ИЗМЕНЕНИЯ ФИЛЬТРА
			if(WaitForSingleObject(g_hFilterSetOK,0)!=WAIT_TIMEOUT)
				MessageBox(NULL,_TEXT("Преобразователь прислал сообщение с подтвержением установки фильтра CAN без запроса"),_TEXT("ОШИБКА ПРЕОБРАЗОВАТЕЛЯ"), MB_ICONWARNING);
			g_FilterSetResult=pReceiveMessage[DAT_STATE];
			if(!SetEvent(g_hFilterSetOK)) return false;
		}
		else if(lMesType==0x21)
		{//ПОДТВЕРЖДЕНИЕ ИЗМЕНЕНИЯ МАСКИ
			if(WaitForSingleObject(g_hMaskSetOK,0)!=WAIT_TIMEOUT)
				MessageBox(NULL,_TEXT("Преобразователь прислал сообщение с подтвержением установки маски CAN без запроса"),_TEXT("ОШИБКА ПРЕОБРАЗОВАТЕЛЯ"), MB_ICONWARNING);
			g_MaskSetResult=pReceiveMessage[DAT_STATE];
			if(!SetEvent(g_hMaskSetOK)) return false;
		}
		else if(lMesType==0x22)
		{//ПОДТВЕРЖДЕНИЕ ИЗМЕНЕНИЯ РЕЖИМА РАБОТЫ ПРЕОБРАЗОВАТЕЛЯ
			if(WaitForSingleObject(g_hRegimeSetOK,0)!=WAIT_TIMEOUT)
				MessageBox(NULL,_TEXT("Преобразователь прислал сообщение с подтвержением изменения режима настройки скорости CAN без запроса"),_TEXT("ОШИБКА ПРЕОБРАЗОВАТЕЛЯ"), MB_ICONWARNING);
			g_RegimeSetResult=pReceiveMessage[DAT_STATE];
			if(!SetEvent(g_hRegimeSetOK)) return false;
		}
		else if(lMesType==0x23)
		{//ПОДТВЕРЖДЕНИЕ ИЗМЕНЕНИЯ СКОРОСТИ CAN
			if(WaitForSingleObject(g_hSpeedSetOK,0)!=WAIT_TIMEOUT)
				MessageBox(NULL,_TEXT("Преобразователь прислал сообщение с подтвержением изменения скорости CAN без запроса"),_TEXT("ОШИБКА ПРЕОБРАЗОВАТЕЛЯ"), MB_ICONWARNING);
			g_SpeedSetResult=pReceiveMessage[DAT_STATE];
			if(!SetEvent(g_hSpeedSetOK)) return false;
		}
		else if(lMesType==0x25)
		{//CAN СООБЩЕНИЕ ОТПРАВЛЕНО
			if(WaitForSingleObject(g_hCAN_MesSended,0)!=WAIT_TIMEOUT)
			{
				//++++ MessageBox(NULL,_TEXT("Преобразователь ответил на отправку CAN сообщения без запроса"),_TEXT("ОШИБКА ПРЕОБРАЗОВАТЕЛЯ"), MB_ICONWARNING);
				//Не буду показывать MessageBox, т.к. такое возможно напримен, если при только 2 узлах в сети контроллер встал на брейкпоинте
				//а программа на ПК будет доставть постоянными сообщениями, что не хорошо, поэтому отправлю сообщение, а там уж пуска
				//разбираются
				PostMessage(g_MesReceiveWindow,MSG_NEW_RECEIVE,(WPARAM)NULL,CTF_UNCOWN_CONFIRM);
			}
			g_SendMesCAN_Result=pReceiveMessage[DAT_STATE];
			if(!SetEvent(g_hCAN_MesSended)) return false;
		}
		else if(lMesType==0x26)
		{//ПОДТВЕРЖДЕНИЕ ВКЛЮЧЕНИЯ/ВЫКЛЮЧЕНИЯ ФИЛЬТРА
			if(WaitForSingleObject(g_hFilterEnDisOK,0)!=WAIT_TIMEOUT)
				MessageBox(NULL,_TEXT("Преобразователь прислал сообщение с подтвержением включения//выключения фильтра без запроса"),_TEXT("ОШИБКА ПРЕОБРАЗОВАТЕЛЯ"), MB_ICONWARNING);
			g_FilterEnDisResult=pReceiveMessage[DAT_STATE];
			if(!SetEvent(g_hFilterEnDisOK)) return false;
		}
		else
		{//НЕИЗВЕСТНЫЙ ПАКЕТ
			return false;
		}
		
	}//конец сообщений от контроллера
	else
	{//НЕИЗВЕСТНЫЙ ОТПРАВИТЕЛЬ
		return false;
	}

	return true;
}
#ifdef CAN_DLL_EXPORTS
#define CAN_DLL_API __declspec(dllexport)
#else
#define CAN_DLL_API __declspec(dllimport)
#endif

/******************************************************************************
*						ЛИСТ РЕГИСТРАЦИИ ИЗМЕНЕНИЙ
*	06.04.2009 - Теперь если с преобразователем возникает какое-то событие, 
*		то окну-родителю отправляется сообщение с кодом этого события.
******************************************************************************/

// Структура, определяющая сообщение CAN
struct CAN_DLL_API CCAN_Message 
{
	DWORD ID;				// идентификатор CAN сообщения
 
//флаги как для передачи, так и для приема
#define CF_XTD_FRAME		0x20 // 0b00100000 расширенный идентификатор
#define CF_RTR_FRAME		0x40 // 0b01000000 удаленный запрос (нет поля данных)

//флаги, появляющиеся только при приеме
#define CF_RX_DBL_BUFFERED  0x80 // 0b10000000 двойная буферизация
#define CF_RX_OVERFLOW      0x08 // 0b00001000 переполнение приемного буфера
#define CF_RX_INVALID_MSG   0x10 // 0b00010000 неверное сообщение
	BYTE Flags;				// дополнительные флаги
	BYTE DataLen;			// Количество байт данных
	BYTE Data[8];			// Данные
};

//Результаты работы функций
typedef unsigned char CAN_RESULT;
#define CR_ALLOK			0x00	//операция завершена успешно
#define CR_BADARGUMENTS		0x01	//в функцию переданы недопустимые аргументы
#define CR_NOANSWER			0x02	//преобразователь не отвечает
#define CR_OTHERERROR		0x03	//другая ошибка
#define CR_NOTCONNECTED		0x04	//преобразователь не подключен к компьютеру
#define CR_DEVICEBUSY		0x05	//преобразователь ответил, что не смог выполнить
									//запрошенную операцию

//режимы работы преобразователя
#define RG_NOTINITIALIZED	0x00	//ОШИБКА В ПРЕОБРАЗОВАТЕЛЕ
#define RG_NORMAL			0x01	//нормальный режим работы
#define RG_SPEEDFIND		0x02	//автопоиск скорости CAN
#define RG_AUTOSEND			0x03	//автоотправка пакетов (без участия PC)

/*******************************************************************************
*		При получении очередного CAN сообщения, либо по какому другому событию,
*	программа посылает окну, хэндл которого ранее был задан функцией CAN_SetMesReceiveWindow, сообщение.
*******************************************************************************/
#define MSG_NEW_RECEIVE		(WM_USER+26)	//это сообщение отправляется, когда поступула очередная ошибка
//Если WPARAM==NULL, то LPARAM оперделяет событие
#define CTF_STATE_CHANGED	0x00			//изменилось состояние подключен/отключен преобразователя CAN-USB
#define CTF_UNCOWN_CONFIRM	0x01			//преобразователь отправил подтверждение отправки незапрашиваемого сообщения


//ОПИСАНИЕ ФУНКЦИЙ НАХОДИТСЯ В ФАЙЛЕ ABOUT_CAN_DLL.TXT
CAN_DLL_API CAN_RESULT	CAN_SetMesReceiveWindow(HWND hWnd);
CAN_DLL_API bool		CAN_IsMonitorConnected();
CAN_DLL_API CAN_RESULT	CAN_SetTimeOut(DWORD time_ms);
CAN_DLL_API DWORD		CAN_GetTimeOut(void);

CAN_DLL_API CAN_RESULT	CAN_GetReceiveErrorsCount(unsigned char* pCount);
CAN_DLL_API CAN_RESULT	CAN_GetTranceiveErrorsCount(unsigned char* pCount);
CAN_DLL_API CAN_RESULT	CAN_GetBusOff(bool *pBusOff);

CAN_DLL_API CAN_RESULT	CAN_CalcActualSpeed(DWORD BaudRate, float* pError=NULL, DWORD* pActualSpeed=NULL);
CAN_DLL_API CAN_RESULT	CAN_GetSpeed(DWORD* pBaudRate);
CAN_DLL_API CAN_RESULT	CAN_SetSpeed(DWORD BaudRate); 

CAN_DLL_API CAN_RESULT	CAN_SetRegime(BYTE Regime);
CAN_DLL_API CAN_RESULT	CAN_GetRegime(BYTE* pRegime);
CAN_DLL_API CAN_RESULT	CAN_SendMessage(CCAN_Message* pCAN_Message);

CAN_DLL_API CAN_RESULT	CAN_SetFilter(BYTE FilterNum, DWORD FilterValue, bool IsFilterExtended=false);
CAN_DLL_API CAN_RESULT	CAN_EnableFilter(BYTE FilterNum, bool IsEnable);
CAN_DLL_API CAN_RESULT	CAN_GetFilter(BYTE FilterNum, DWORD *pFilterValue, bool* pIsFilterExtended, bool* pIsFilterEnabled);

CAN_DLL_API CAN_RESULT	CAN_SetMask(DWORD MaskValue);
CAN_DLL_API CAN_RESULT	CAN_GetMask(DWORD* pMaskValue);

CAN_DLL_API CAN_RESULT	CAN_COM_Open(unsigned char port_num);
CAN_DLL_API bool		CAN_IsCOM_Opened(void);

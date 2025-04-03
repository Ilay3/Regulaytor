// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

//эта функция содержится в CAN_DLL.cpp и является обработчиком сообщения WM_DEVICECHANGE
extern void OnDeviceChange(WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_CREATE: break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_DEVICECHANGE:	//в системе изменились устройства
			//это может быть наш преобразователь, поэтому 
			//вызову внешний обработчик
			OnDeviceChange(wParam, lParam);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

HWND	m_hPseudoWindow;						//хэндл моего псевдо-окна
TCHAR*	szClassName = _TEXT("CAN_DLL\0");		//имя класса, под который создается окно
extern void InitCAN_DLL(void);					//иницилаизация файла CAN_DLL.cpp
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:	//создался процесс, который подключил DLL
		{
			WNDCLASSEX	wndClass;

			//начинается жуткий процесс регистрации

			wndClass.cbSize = sizeof(WNDCLASSEX);
			//обязательное поле размера структуры. Если его 
			//не указать, может очень нефигово аукнуться 

			wndClass.style			= CS_HREDRAW | CS_VREDRAW;
			//стиль окна. Описывает возможности окна. В данном 
			//случае окно будет перересовываться при изменении 
			//размеров (ширина,высота). Как уже наверное понятно, 
			//стили объеденяются логической операцией ИЛИ (|).

			wndClass.lpfnWndProc	= (WNDPROC)WndProc;
			//А вот это указатель на так называемую оконную 
			//функцию. Помните, что имя функции в С есть ее 
			//адрес? Так вот в этой функции мы и будет 
			//обрабатывать сообщения.

			wndClass.cbClsExtra		= 0;
			wndClass.cbWndExtra		= 0;
			//Два редко используемых поля, которые указывают 
			//на количество дополнительных байт связанных с 
			//данным классом окна и окном.

			wndClass.hInstance		= hModule;
			//Это тот параметр описатель процесса, о котором 
			//мы говорили в самом начале.

			wndClass.hIcon			= NULL;//LoadIcon(hModule, (LPCTSTR)IDI_SIMPLE);
			wndClass.hIconSm		= NULL;//LoadIcon(wndClass.hInstance, (LPCTSTR)IDI_SMALL);
			wndClass.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);
			//Описатели курсора и значка (помните пример?)

			wndClass.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
			//Описатель кисти для фона

			wndClass.lpszMenuName	= NULL;
			//Сей параметр есть указатель на строку с последним
			//нулевым символом,  которая содержит название 
			//ресурса отвечающего за меню. 

			wndClass.lpszClassName	= (LPCWSTR)szClassName;
			//имя только что созданного класса окна. 
			//У нас будет TemplateDirectX

			//Регистрирую описанный класс
			RegisterClassEx(&wndClass);

			TCHAR* lWndTitle=_TEXT("Это окно не должно быть видимым\0");
			m_hPseudoWindow = CreateWindow(szClassName, 
				//все то же название класса окна, 
				//вернее указатель на строку с нулевым 
				//указателем, которая содержит название 
				//класса окна

				lWndTitle, //указатель на строку, 
				//заканчивающуюся нулевым символом, 
				//содержащую заголовок окна

				WS_OVERLAPPEDWINDOW,//стили окна, которые 
				//могут объеденятся оператором ИЛИ (|). 
				//В данном случае установлен стиль - 
				//ПОВЕРХ ВСЕХ ОКОН

				CW_USEDEFAULT, //X-координата
				0, //Y-координата
				CW_USEDEFAULT, //Ширина
				0, //Высота
				NULL, //Указатель на родительское окно
				NULL, //Описатель меню
				hModule, //описатель экземпляра приложения, 
				//который будет ассоциироваться с данным окном
				NULL//Дополнительные данные, которые 
				//могут использоваться при обработке 
				//сообщения WM_CREATE
				);
			if(m_hPseudoWindow==NULL)
			{//не смогли создать свое псевдо-окно
				return FALSE;
			}
			InitCAN_DLL();	//проинициализирую файл CAN_CLL.cpp
			break;
		}
	case DLL_THREAD_ATTACH:		//процесс создал новый поток (кроме первого)
	case DLL_THREAD_DETACH:		//процесс завершается
	case DLL_PROCESS_DETACH:	//процесс завершил очередной поток (кроме первого)
		break;
	}
	return TRUE;
}


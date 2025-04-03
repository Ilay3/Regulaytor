// USART.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>
#include "USART_Dlg.h"		//заголовочный файл с основным классом настройки связи
#include "Resource.h"
#include "PROTOCOL_Struct.h"//файл с протоколом


/*****************************************************************************
*		Здесь объявим переменные, которые будут находиться в этой библиотеке
*********************************************************************************/
CUSART_Dlg		*USART_Dlg;			//собственно главное окно
PROTOCOL_Struct PROTOCOL;			//протокол обмена

#ifdef _MANAGED
#error Please read instructions in USART.cpp to compile with /clr
// If you want to add /clr to your project you must do the following:
//	1. Remove the above include for afxdllx.h
//	2. Add a .cpp file to your project that does not have /clr thrown and has
//	   Precompiled headers disabled, with the following text:
//			#include <afxwin.h>
//			#include <afxdllx.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


static AFX_EXTENSION_MODULE USARTDLL = { NULL, NULL };

#ifdef _MANAGED
#pragma managed(push, off)
#endif



extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("USART.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(USARTDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		// create a new CDynLinkLibrary for this app
		#pragma warning(push)
		#pragma warning(disable:6014) //silence prefast warning because CDynLinkLibrary is cleaned up elsewhere
		new CDynLinkLibrary(USARTDLL);
		#pragma warning(pop)

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("USART.DLL Terminating!\n");

		//удалим созданное окно по связи
		DestroyUSART(USART_Dlg);

		// Terminate the library before destructors are called
		AfxTermExtensionModule(USARTDLL);
	}
	return 1;   // ok
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

/*************************	InitUSART_Window	****************************************
*		Эта функция делает все необходимое, чтобы в клиентской программе появилась связь
****************************************************************************************/
extern "C" __declspec (dllexport) CUSART_Dlg* /* CALLBACK*/ InitUSART_Window(CWnd* pParent, ERROR_Class* m_ERROR, POTOCOL_OPTIONS_Struct* pProtocolOptions)
{
	//	Реализуем небольшую защиту от повторного включения
	static bool counter=true;	
	if(!counter)
	{
		AfxMessageBox(_TEXT("Повтораня попытка создания окна со связью USART"));
		return NULL;
	}
	counter=false;

	//	Создадим наше окно
	/*
	Если в этом месте AfxGetResourceHandle выдает ошибку значит:
	- главный проект unicode, а этот multy byte character set (или наоборот)
	- главный проект use MFC static library, а этот shared DLL
	Это просто нужно согласовать!
	*/
	HINSTANCE hInstOld = AfxGetResourceHandle();
//	DWORD r=GetLastError();
	AfxSetResourceHandle(USARTDLL.hModule);
	USART_Dlg=new CUSART_Dlg();
	USART_Dlg->Create(IDD_USART_DLG,pParent);
	if(!USART_Dlg->MyInit(m_ERROR,pProtocolOptions,&PROTOCOL))
	{//видно там что-то плохое
		return NULL;
	}
	USART_Dlg->pUSARTDLL=&USARTDLL;
	AfxSetResourceHandle(hInstOld); // restore the old resource chain
	
	return USART_Dlg;
	
}

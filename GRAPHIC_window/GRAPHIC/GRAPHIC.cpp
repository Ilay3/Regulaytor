// GRAPHIC.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>
#include "GRAPH_Dlg.h"	//���� � ������� ���� �������

/*****************************************************************************
*		����� ������� ����������, ������� ����� ���������� � ���� ����������
*********************************************************************************/
CGRAPH_Dlg		*GRAPH_Dlg;			//���������� ������� ����

#ifdef _MANAGED
#error Please read instructions in GRAPHIC.cpp to compile with /clr
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


static AFX_EXTENSION_MODULE GRAPHICDLL = { NULL, NULL };

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
		TRACE0("GRAPHIC.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(GRAPHICDLL, hInstance))
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
		new CDynLinkLibrary(GRAPHICDLL);
		#pragma warning(pop)

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("GRAPHIC.DLL Terminating!\n");

		//������ ��������� ���� �� �����
		DestroyGRAPH(GRAPH_Dlg);
		// Terminate the library before destructors are called
		AfxTermExtensionModule(GRAPHICDLL);
	}
	return 1;   // ok
}

/*************************	InitGraphic_Window	****************************************
*		��� ������� ������ ��� �����������, ����� � ���������� ��������� ��������� �����
****************************************************************************************/
extern "C" __declspec(dllexport) CGRAPH_Dlg* /* CALLBACK*/ InitGraphic_Window(CWnd* pParent, ERROR_Class* m_ERROR, CGraphicOptions * pGraphicOptions)
{
	/*************	��������� �� ���� ��������� ��������� ANSI	********************************/
	//	!!!!!   ������ ��� ����� ����������� ���������, �.�. ��� ������ � ������� ��� ����� 
	//			���������
	char *buf=setlocale( LC_CTYPE, "rus" );
	if(buf==NULL)
	{//���� ������� ����, �� ���������� �����
		AfxMessageBox(_T("�� ������� �������� ���������!!!"));
	}

	//	��������� ��������� ������ �� ���������� ���������
	static bool lFirst=true;	
	if(!lFirst)
	{
		AfxMessageBox(_T("��������� ������� �������� ���� � ���������"));
		return false;
	}
	lFirst=false;
	//	�������� ���� ����
	HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(GRAPHICDLL.hModule);
	GRAPH_Dlg=new CGRAPH_Dlg();
	GRAPH_Dlg->Create(IDD_GRAPH_DLG,pParent,m_ERROR, pGraphicOptions);
	GRAPH_Dlg->pGRAPHDLL=&GRAPHICDLL;
	AfxSetResourceHandle(hInstOld); // restore the old resource chain
	
	return GRAPH_Dlg;
	
}


#ifdef _MANAGED
#pragma managed(pop)
#endif


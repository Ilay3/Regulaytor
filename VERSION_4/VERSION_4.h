// VERSION_4.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CVERSION_4App:
// See VERSION_4.cpp for the implementation of this class
//

class CVERSION_4App : public CWinApp
{
public:
	CVERSION_4App();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
private:
	ULONG_PTR  m_gdiplusToken;
};

extern CVERSION_4App theApp;
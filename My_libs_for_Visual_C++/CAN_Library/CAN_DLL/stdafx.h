// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>



#include <Tchar.h>							//некоторые макросы для UNICODE
#include <Dbt.h>							//константы для подключинеия USB
#include <math.h>							//математика

#include <CP210x\CP210x.h>					//работа с CP210x
#include <SerialPort\SerialPort.h>			//класс работы с последовательным портом
#include <types_visual_c++.h>				//файл с моими типами
#include <SummCollection\SummCollection.h>	//сборник контрольных сумм
#include <MyTrace_visual_c++.h>				//для trace без MFC


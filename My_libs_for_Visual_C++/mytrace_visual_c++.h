//���� TRACE �������� ��� MFC
#pragma once
#ifdef _DEBUG
#define MyTRACE   TracePrint
#else
#define MyTRACE
#endif
#include <stdio.h>		//��� printf

inline void TracePrint(const char   *szFormat,...)
{
    va_list args;
    va_start(args, szFormat);
    char str[1024]; // �������� ������, ����� ������
    int len = vsprintf_s(str,1024,szFormat,args);
    strncat_s(str,1024,"\n",1024);
    OutputDebugStringA(str);
}


inline void TracePrint(const WCHAR  *szFormat,...)
{
    va_list args;
    va_start(args, szFormat);
    WCHAR str[1024]; // �������� ������, ����� ������
    int len = vswprintf_s(str,1024,szFormat,args);
    wcsncat_s(str,1024,L"\n",1024);
    OutputDebugStringW(str);
}
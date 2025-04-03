// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

//��� ������� ���������� � CAN_DLL.cpp � �������� ������������ ��������� WM_DEVICECHANGE
extern void OnDeviceChange(WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_CREATE: break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_DEVICECHANGE:	//� ������� ���������� ����������
			//��� ����� ���� ��� ���������������, ������� 
			//������ ������� ����������
			OnDeviceChange(wParam, lParam);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

HWND	m_hPseudoWindow;						//����� ����� ������-����
TCHAR*	szClassName = _TEXT("CAN_DLL\0");		//��� ������, ��� ������� ��������� ����
extern void InitCAN_DLL(void);					//������������� ����� CAN_DLL.cpp
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:	//�������� �������, ������� ��������� DLL
		{
			WNDCLASSEX	wndClass;

			//���������� ������ ������� �����������

			wndClass.cbSize = sizeof(WNDCLASSEX);
			//������������ ���� ������� ���������. ���� ��� 
			//�� �������, ����� ����� �������� ��������� 

			wndClass.style			= CS_HREDRAW | CS_VREDRAW;
			//����� ����. ��������� ����������� ����. � ������ 
			//������ ���� ����� ���������������� ��� ��������� 
			//�������� (������,������). ��� ��� �������� �������, 
			//����� ������������ ���������� ��������� ��� (|).

			wndClass.lpfnWndProc	= (WNDPROC)WndProc;
			//� ��� ��� ��������� �� ��� ���������� ������� 
			//�������. �������, ��� ��� ������� � � ���� �� 
			//�����? ��� ��� � ���� ������� �� � ����� 
			//������������ ���������.

			wndClass.cbClsExtra		= 0;
			wndClass.cbWndExtra		= 0;
			//��� ����� ������������ ����, ������� ��������� 
			//�� ���������� �������������� ���� ��������� � 
			//������ ������� ���� � �����.

			wndClass.hInstance		= hModule;
			//��� ��� �������� ��������� ��������, � ������� 
			//�� �������� � ����� ������.

			wndClass.hIcon			= NULL;//LoadIcon(hModule, (LPCTSTR)IDI_SIMPLE);
			wndClass.hIconSm		= NULL;//LoadIcon(wndClass.hInstance, (LPCTSTR)IDI_SMALL);
			wndClass.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);
			//��������� ������� � ������ (������� ������?)

			wndClass.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
			//��������� ����� ��� ����

			wndClass.lpszMenuName	= NULL;
			//��� �������� ���� ��������� �� ������ � ���������
			//������� ��������,  ������� �������� �������� 
			//������� ����������� �� ����. 

			wndClass.lpszClassName	= (LPCWSTR)szClassName;
			//��� ������ ��� ���������� ������ ����. 
			//� ��� ����� TemplateDirectX

			//����������� ��������� �����
			RegisterClassEx(&wndClass);

			TCHAR* lWndTitle=_TEXT("��� ���� �� ������ ���� �������\0");
			m_hPseudoWindow = CreateWindow(szClassName, 
				//��� �� �� �������� ������ ����, 
				//������ ��������� �� ������ � ������� 
				//����������, ������� �������� �������� 
				//������ ����

				lWndTitle, //��������� �� ������, 
				//��������������� ������� ��������, 
				//���������� ��������� ����

				WS_OVERLAPPEDWINDOW,//����� ����, ������� 
				//����� ����������� ���������� ��� (|). 
				//� ������ ������ ���������� ����� - 
				//������ ���� ����

				CW_USEDEFAULT, //X-����������
				0, //Y-����������
				CW_USEDEFAULT, //������
				0, //������
				NULL, //��������� �� ������������ ����
				NULL, //��������� ����
				hModule, //��������� ���������� ����������, 
				//������� ����� ��������������� � ������ �����
				NULL//�������������� ������, ������� 
				//����� �������������� ��� ��������� 
				//��������� WM_CREATE
				);
			if(m_hPseudoWindow==NULL)
			{//�� ������ ������� ���� ������-����
				return FALSE;
			}
			InitCAN_DLL();	//���������������� ���� CAN_CLL.cpp
			break;
		}
	case DLL_THREAD_ATTACH:		//������� ������ ����� ����� (����� �������)
	case DLL_THREAD_DETACH:		//������� �����������
	case DLL_PROCESS_DETACH:	//������� �������� ��������� ����� (����� �������)
		break;
	}
	return TRUE;
}


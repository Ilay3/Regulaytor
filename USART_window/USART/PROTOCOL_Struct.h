/*
*************************************************************************************************
*	���� ����� �������� �� ����� ��������� �� ���������� � �������� ����������. � ��� ������������� 
*	������� �������	����������� �� ��������, ������� ����������� ����� � �.�. ����� ����� ������� �
*	��������, �����, ������������� �������. ��� ���������� ������ ����� ������ ����� �����:
*		- ������� �����, � ������� ���������� ���������� ������� (�� ������� � ����� �������������
*		  ������� � ���� ����� ���������������� �������� �����
*		- ������ � ���������� � ����������� (������������� ��� ����� �� ��� ����������� � �����������
*		  �������)
*		- ������ ����...
**************************************************************************************************
*/

/******************** ���� ����������� ��������� *************************************************
*	8.08.2007 - �.�. ������ ���� ��������� DLL, �� ��� ��������� ������� � ���� �����:
*					USARTDlg.h
*****************************************************************************************************/

#pragma once
#include <ERRORS\ERRORS.h>						//������ ����������� ���������
#include <ProtocolOptions\ProtocolOptions.h>	//�������� ����� � �����������
#include <Hex_and_String\Hex_and_String.h>		//��������� ����������� �������� �������
#include <MessageClass\MessageClass.h>			//����� ������������� ���������
#include <types_visual_c++.h>					//���� � ����� ������
#include <ByteStuffing\ByteStuffing.h>			//������� ��� ������������������ � ��������������
#include <fstream>								//��� ������ � �������
using namespace std;


//��������� �� ������� ���� �����
class CUSART_Dlg;

class AFX_EXT_CLASS PROTOCOL_Struct
{
	/*DECLARE_DYNAMIC(PROTOCOL_Struct)*/
public:
	CUSART_Dlg *pUSART_Dlg;

	INT16U Mode; //��������� ����� �� �������� ������������� �������� ��������
	
	INT64U InMessageCount; //���������� ���������� �������
	
	INT64U OutMessageCount;//���������� ������������ �������
	
	//������ ��� ��������� � ����� ������������
	CMessage NextMessage;	//������� � �������� ��������� ���� 1 (���� �������)

	//� 1 �� 8 ���������� � protokol_options.h
	//� ��� �� ��������� �� �������, �.�. �� ��������� �� ���������� �� ��������� � ���������
	bool CompileMessage_9(CMessage &nextmessage); //������ ������������� �������
	bool CompileMessage_10(CMessage &nextmessage);//�������� �� �����

	unsigned char *ReceiveMessage;	//��������� ���������� ���������, ����� ���� ��� ����� ������� ��������� - 
									//������������ � ��������� � ����

	ERROR_Class *m_pError;			//��������� �� �������� ������
	
	//��������� ���������� �������� �������
	bool AddInMessageCount(void);
	//������� ���������� �������� �������
	bool ClearInMessageCount(void);

	//��������� ���������� ������������ �������
	bool AddOutMessageCount(void);
	//������� ���������� ������������ �������
	bool ClearOutMessageCount(void);

	//�������� ����� ��������� �� ������ �������� ���������
	bool CompileALL(void);

	//���������� ����������  ������, ��������� ���-�����, ������������ �����,
	//��������� ������� � ������ ������ ������. ������ ������������ ����.
	bool ReceiveChar(unsigned char ch);
	
	fstream OutMessageFile;			// �� ����� ����� ���������� ���������
	ofstream ReceiveMessageFile;	//���� �������� ���������� ���������
	
	
	//���������� � ���� ��������/������������/��������� ���������
	#define UART_MSG_TRANSEIVED		0x01	//��������� ����������
	#define UART_MSG_RECEIVED		0x02	//��������� �������
	#define UART_MSG_NOTTRANCEIVED	0x03	//��������� ������� ��������� ���������
	bool WriteMesFile(CMessage *pMessage, INT8U code);

	//���� ����, ��� ���� �������� �� ���� ��������� ��� ������,
	//���� ���, �� ��������� ������������� ��� ���������
	bool InMesFileOpened;	// true - ���� ��� ������
							// false - ���� ������ �� ���
	// ��������� ���� �������� ���������� ���������
	bool InMesFileOpen(void);
	// ��������� ���� �������� ���������
	bool OutMesFileOpen(void);
	// ���������� ��������� � �����, �� �������� �������� ���������
	void BeginOutMesFile(void);

	/**********************************************************************
	*		� ���� ��������� ��������� ��� ��������� ���������, ��� � �������
	*	������ � �������, ��� � ���������� �����. � ���� � ������, ��� ������
	*	�� ���� �� ����� �����, �.�. �������� ��������� ����� - ���� �� �����
	*	������ (�� � ���������� ������������ � SerialPort.h, �� ������ ����� 
	*	��� ��� ��������� � ����� �����
	***********************************************************************/
	POTOCOL_OPTIONS_Struct * pProtocolOptions;

	PROTOCOL_Struct(void);//�����������
	~PROTOCOL_Struct(void);//����������
	bool InitPROTOCOL(CUSART_Dlg * pUSART_Dlg, ERROR_Class * error);//��������� ������������� ���������� ����������
};


/************************************************************************************************
*************************************************************************************************
**		���� ���� �������� ���������� ������ ������-�����������. ��� ������ ���, ��� ����� ��� �����,
**	�����������, ���������� ���������. �� �� ������������� ��������, �������� ������ � �.�. ������ ��� ������
**	��� ��� ��������, ��� � ����� ����������� ������������ � ������� �����.
***************************************************************************************************
***************************************************************************************************/

/**************************** ���� ����������� ��������� *******************************************************
*	11.09.2006 - ��������� ��������� � ����� ���������-������, ������� ��������� ������� "����� �� ����������" 
*				 � ��������� � ��������������.
****************************************************************************************************************
*	11.11.2024 - �������� ������� ������ ����� ReceiveChar ��� ������ � ��������������.
* 
****************************************************************************************************************/

#include "stdafx.h"
#include "protocol_struct.h"
#include "USART_Dlg.h"

#define  MSULINK_RX_STATE_SD               0               /* �������� ���������� �����������          */
#define  MSULINK_RX_STATE_L_DAT            1               /* �������� ����� ������ (COM+DATA+CHKSUM)  */
#define  MSULINK_RX_STATE_DATA             2               /* �������� ������ (������� COM)            */
#define  MSULINK_RX_STATE_BS               3               /* �������� �������������                   */
#define  MSULINK_RX_STATE_CHKSUM           4               /* �������� ����������� �����               */

/*IMPLEMENT_DYNAMIC(PROTOCOL_Struct)*/

/**********************	InitPROTOCOL(CUSART_Dlg * pParent, ERROR_Class * error)*********************
*		�������������. pParent - ��������� �� ���� �����
*						error - ��������� �� ���������� ����������� ������
*******************************************************************************************************/
bool PROTOCOL_Struct::InitPROTOCOL(CUSART_Dlg * pParent, ERROR_Class * error)
{
	ASSERT(pParent);
	ASSERT(error);

	this->pUSART_Dlg=pParent;		//�������� ��������� �� ������� ���� �� �����
	this->m_pError=error;			//�������� ��������� �� ������

	if(pProtocolOptions->var_MAX_LEN!=0)
	{//�������� ������-����� ��� ������������ ��������� 
		ReceiveMessage= new unsigned char [pProtocolOptions->var_MAX_LEN];
	}
	else
	{
		m_pError->SetERROR(_T("��� ������������� ��������� ������ �� ������ ��� ������"), ERP_ERROR | ER_WRITE_TO_LOG);
	}

	if(pProtocolOptions->o_OFF)
	{
		//��������� ���� ������� � �������� ���������
		OutMesFileOpen();
	}

    pProtocolOptions->o_MsgType = 0;				// ��� ��������� - ��� ����� CS � ���� L_DAT (L_DAT=7)
  //pProtocolOptions->o_MsgType = 1;				// ��� ��������� - ��� ����� CS � ���� L_DAT (L_DAT=8)

	this->ClearInMessageCount();
	this->ClearOutMessageCount();

	//� ��� ����� �� ���������� ��������������� ���������, 
	//� ����� ������� �������������, � ���� ��������� 
	//"�����", �� �������� ��� �������� � �������� ��������� ���������
	//����� ������� ���� �������� ������������ �����������:
	//���� ������� ������ ���� ��������������� ���� �� ������ ��� ���������
	//� ������ ������������� ��������� ��� ������������� ��� �� ����� ��������-
	//����� ���������
	unsigned short i;
	for(i=0; i<pProtocolOptions->CompileMessage_x.size(); i++)
	{
		CMessage temp;
		temp.type = 1;	//1-7/0-8
		if(!pProtocolOptions->CompileMessage_x[i](temp))
		{
			//���-�� ��� �� ���
			return false;
		}
		pUSART_Dlg->m_ListPacket.AddString(temp.name);
	}
	if(i==0)
	{//���� ��� ���� ������ �������
		m_pError->SetERROR(_T("���������� ��������������� �� ������ ���������, ���������, ��������� �� ��������� ��������� �� �������"), ERP_WARNING | ER_WRITE_TO_LOG);
	}

	//��������� � ������ ���� ������� ������ ���������� ���������
	pUSART_Dlg->m_ListPacket.AddString(_T("������ ����������� �������"));

	return true;		//������������� ������������ ������, ���� ������� ����
}

/****************************************************************************
*									�����������	
*****************************************************************************/
PROTOCOL_Struct::PROTOCOL_Struct(void)
{
	InMesFileOpened=false;	//���� �������� �� ���� ��������� ������ �� ���
}

/****************************************************************************
*									����������
*****************************************************************************/
PROTOCOL_Struct::~PROTOCOL_Struct(void)
{
	if (ReceiveMessage != NULL)
	{//������ ������-����� ���������
		delete[] ReceiveMessage;
	}
}


bool PROTOCOL_Struct::CompileALL(void)
{
	/*
	���� ����������� ����� �������� ��������� �������, ��� ������ ����� ������� ��� �������,
	� ��� ������� ���������� � MAIN_MESSAGE ����� �� , ��� ��� ����� ��������� � ��������� ���

	*/
	/*����� �� �������� ���������� ��������� ������,
	� �������� �� ����� ����������� ���, �� ����� ������ 
	������������ ��� ������� ���� ������*/
	/*��������, ���� ����� ���������� ���������� ��� ���������, � ��������������� ����� �����
	������ ������� ����������� ������������ ����������
	*/
	/*
	������ ����� ���������� ��������� � ����������, ��� ����� ����� ������ ������ ��� ������������� ����� 
	��������� (������ ���), ��� ������ ������ �� ������ (���� ��� ���������������� ���-�� ���)
	*/
	if(pProtocolOptions->o_OFF)
	{
		/*
		������, ���� ���� ���������� �� �����, �� �� � ��� ������ ���� �� ����!!!
		*/
		if(!CompileMessage_10(NextMessage))//����� ��� ��� �� ������ �����������������
		{//���, ��� ���-�� �����������
			m_pError->SetERROR(_TEXT("���� ��� ���������� ������� �� �����"),ERP_ERROR | ER_WRITE_TO_LOG);
			return false;//��� ��� ��������
		}
		//��������� ����-��������� ������������ ����
	}
	else if (Mode==pProtocolOptions->CompileMessage_x.size()+1)
	{//��������� ���� ������������ �������
		if(!CompileMessage_9(NextMessage))//����� ��� ��� �� ������ �����������������
		{//���, ��� ���-�� �����������
			m_pError->SetERROR(_TEXT("���� ��� ���������� ������ �������"),ERP_ERROR | ER_WRITE_TO_LOG);
			return false;//��� ��� ��������
		}
		//��������� ����-��������� ������������ ����
	}
	else 
	{//����� ������� ������� ���������
		if(Mode==0)
		{//�� ������ �� ���� ��� ���������
			m_pError->SetERROR(_T("��� ������ �������� �������� ��� ������������� ���������"), ERP_WATCHFUL | ER_SHOW_MESSAGE);
			return true;	//���, ��� �� ������
		}

		NextMessage.type = 1;
		if(!pProtocolOptions->CompileMessage_x[Mode-1](NextMessage))//����� ��� ��� �� ������ �����������������
		{//���, ��� ���-�� �����������
			CString str; str.Format(_T("���� ��� ������������ ��������� �%i"),Mode);
			m_pError->SetERROR(str,ERP_ERROR | ER_WRITE_TO_LOG);
			return false;//��� ��� ��������
		}
		if(pProtocolOptions->var_Regime==BYTE_STUFFING && this->Mode!=9)
		{//����� ������������ (������ ��������� ����������� �������������� �� �����!!!)
			if(!RecompileMessageWithBytestuffing(&NextMessage, pProtocolOptions->var_FIRST_BYTE, pProtocolOptions->var_STUF_BYTE))
			{
				CString str; str.Format(_T("���� ��� ������������� � ��������� �%i"),Mode);
				m_pError->SetERROR(str,ERP_ERROR | ER_WRITE_TO_LOG);
				return false;
			}
		}
	}


	//������� ������������ ���������
	pUSART_Dlg->ShowOutputMessage();
	return true;
}


/**************************************************************************************
*	��������� ��� ������� ����������� �������� ������� ��������� � 
*	�������� �� ����� ��������������
**************************************************************************************/
bool PROTOCOL_Struct::CompileMessage_9(CMessage &nextmessage) //�������� NextMessage_8 �� �������� ������
{
	/*
	��� �� ������� �������, ��� ���������� ������� �� ������� �� ���� ���������
	�����
	*/
	//��������� ������ ������� �������� � ������!!!
	
	//����� ����� ����������� ������� � �������� �������
	static unsigned char mas[100];	
	unsigned short lMesLen;

	if(pProtocolOptions->o_HexDisplayOutMessage)
	{//��� �� ������������ � ����������������� �����
		CString str;
		pUSART_Dlg->m_OutMessage.GetWindowText(str);
		CString error;										//����� ������������ ������
		lMesLen=GiveHexFromStr(str,mas,&error);	//���������������� �� ������������������ ��������
		if(error!="")
		{//������� ���������� � ��������
			m_pError->SetERROR(error,4);
			return false;
		}
	}
	else
	{//������ �������� �����
		pUSART_Dlg->m_OutMessage.GetWindowText((LPTSTR)mas,100);
		lMesLen=(unsigned short)strlen((char*)mas);
	}

	nextmessage.peredacha=NextMessage.peredacha;	//|
	nextmessage.priem=NextMessage.priem;			//|� ����� ��������� �������
	nextmessage.text=mas;			//����� ������������
	nextmessage.length=lMesLen;		//����� �������
	return true;
}

bool PROTOCOL_Struct::CompileMessage_10(CMessage &nextmessage) //�������� NextMessage_8 �� �������� ������
{

	//������ �������������� ������� � �������� ������ ���������
	//�� �������� �����, �� ��������� �������� � ������������ 
	//����� ���������
	//�� ���������� ���� ������� �������� � ������

	static CString message_from_file;
	CString error=_TEXT("��� ������");
	CString str,str1;
	if(!(OutMessageFile.is_open()))
	{
		error=CString(_TEXT("���� �������� "))+pProtocolOptions->o_outMesFile + CString(_TEXT(" �� ������"));
		goto ERROR2;//����� ������ ������ �� ����
	}
	
	char t[150];
	//�����������! �������� ������
	do //���������� ������ � �������������
	{
		OutMessageFile.getline(t,150,0x0A);
		//���� ��������
		//0�0� - ������� ������, /n - �� ��������
		str=t;
		str.TrimLeft(' ');//������� ������� ������� ������
		
		if(str==_T("RETURN"))
		{
			//���� ��������� ��������� �������� 
			//� ������ �����
			OutMessageFile.clear();
			OutMessageFile.seekg(0,ios::beg);
			continue;
		}
		else
			if(OutMessageFile.eof())
			{
				error=_T("��������� ����� �����!");
				goto ERROR2;//����� ������ ������ �� ����
			}
		//���������� ������ ������
		//str.TrimLeft(' ');
		if(str=="") continue;//������ ������
		if(str.Left(2)!="//")break;//�� �����������, � ������� ����������
	}
	while(true);
	
	//���� str - �� �����������, �� �����, � ��������� 
	unsigned col=str.Find('|');
	str1=str.Left(col);
	str=str.Right(str.GetLength()-col-1);//������� ��������������
	
	unsigned time;
	time=_ttoi(str1.GetString());//��������� ����������
	if(!time)//����� ���������
	{
		//���� ����� � �������� ���� �� ������������
		error=_T("������� ��� ������.");
		goto ERROR2;//����� ������ ������ �� ����
	}
	else nextmessage.nomer=time;
	//������� ����� �������
	//� ������ �������
	str.TrimLeft(' ');
	col=str.Find('|');
	str1=str.Left(col);
	str=str.Right(str.GetLength()-col-1);//������� ��������������
	str1.TrimRight(' ');
	message_from_file=str1;
	
	col=str.Find('|');
	str1=str.Left(col);
	str=str.Right(str.GetLength()-col-1);//������� ��������������
	time=_ttoi(str1);
	if(!time)//����� ��������
	{
		error=_T("����� �������� - �� �����");
		goto ERROR2;//����� ������ ������ �� ����
	}
	else
		if(time<15 || time>800)
		{
			error=_T("���������� ����� ��������");
			goto ERROR2;//����� ������ ������ �� ����
		}
	nextmessage.peredacha=time;
	
	time=_ttoi(str);
	if(!time)//����� ������
	{
		error=_T("����� ������ - �� �����");
		goto ERROR2;//����� ������ ������ �� ����
	}
	else
		if(time<15 || time>800)
		{
			error=_T("���������� ����� ������");
			goto ERROR2;//����� ������ ������ �� ����
		}
	nextmessage.priem=time;
ERROR2:		
	if(error!=_T("��� ������"))
	{
		//����������������� �� ������
		message_from_file=_T("");//�� ��������� ��������
		m_pError->SetERROR(error,4);
		return false;	//� ������ ������
	}
	
	nextmessage.text=(unsigned char *)message_from_file.GetBuffer();
	nextmessage.length=message_from_file.GetLength(); //���� ���, ���� ����� ��������� ���������
	nextmessage.summa=0; //� ���� ��� ����� ��� � ��� ��� ��� ���������

	pUSART_Dlg->ShowOutputMessage();
	pUSART_Dlg->ShowERROR_in_out_message(error,5);
	return true;
}

// ��������� ���� �������� ���������� ���������
bool PROTOCOL_Struct::InMesFileOpen(void)
{
	if(ReceiveMessageFile.is_open()) ReceiveMessageFile.close();//�� ������ ������
	ReceiveMessageFile.open(pProtocolOptions->o_inMesFile,ios::trunc | ios::out);
	CString str;
	if(!ReceiveMessageFile)
	{
		m_pError->SetERROR(_T("�� �������� ���� \r\n ")+pProtocolOptions->o_inMesFile, ERP_WARNING | ER_WRITE_TO_LOG);		
	}
	else
	{
		CStringA str="���� �������� ���������� �� ���� ���������...";
		ReceiveMessageFile.write(str,str.GetLength());
		ReceiveMessageFile<<endl<<endl;
		m_pError->SetERROR(_T("���� ����������� �������� ��������� \"")+pProtocolOptions->o_inMesFile+_T("\" ������� ������"), ERP_NORMAL | ER_WRITE_TO_LOG | ER_HIDE_FROM_USER);
	}
	return true;
}

bool PROTOCOL_Struct::OutMesFileOpen(void)
{
	if(OutMessageFile.is_open())OutMessageFile.close();
	OutMessageFile.open(pProtocolOptions->o_outMesFile);
	if(!OutMessageFile)
	{
		m_pError->SetERROR(_T("�� ���� ������� ���� �������� ")+pProtocolOptions->o_outMesFile,ERP_WARNING | ER_WRITE_TO_LOG);
	}
	else
	{
		m_pError->SetERROR(_T("���� ")+pProtocolOptions->o_outMesFile+_T(" ������� ������"), ERP_NORMAL | ER_WRITE_TO_LOG | ER_HIDE_FROM_USER);
		//������� ��������� � �����
		BeginOutMesFile();
	}
	return true;
}

void PROTOCOL_Struct::BeginOutMesFile(void)
{// ���������� ��������� � �����, �� �������� �������� ���������
	OutMessageFile.clear();
	OutMessageFile.seekg(0,ios::beg);
}


/********************** ClearInMessageCount(void) ************************
*	������� ���������� �������� �������
*************************************************************************/
bool PROTOCOL_Struct::ClearInMessageCount(void)
{
	this->InMessageCount=0;
	pUSART_Dlg->m_Recieve.SetWindowText(_T("0\0"));
	return true;
}


/********************** AddInMessageCount(void) ************************
*	������� ���������� �������� �������
*************************************************************************/
bool PROTOCOL_Struct::AddInMessageCount(void)
{//������ ������� � ���, � ���
	InMessageCount++;
	CString str; str.Format(_T("%u"),InMessageCount);
	pUSART_Dlg->m_Recieve.SetWindowText(str);
	return true;
}


/********************** ClearOutMessageCount(void) ************************
*	������� ���������� ������������ �������
*************************************************************************/
bool PROTOCOL_Struct::ClearOutMessageCount(void)
{
	this->OutMessageCount=0;
	pUSART_Dlg->m_sended.SetWindowText(_T("0\0"));
	return true;
}


/********************** AddOutMessageCount(void) ************************
*	��������� ���������� ������������ �������
*************************************************************************/
bool PROTOCOL_Struct::AddOutMessageCount(void)
{
	OutMessageCount++;
	CString str; str.Format(_T("%u"),OutMessageCount);
	pUSART_Dlg->m_sended.SetWindowText(str);
	return true;
}




/************************* ReceiveChar(unsigned char ch) ************************
*	���������� ����������  ������, ��������� ���-�����, ������������ �����,
*	��������� ������� � ������ ������ ������. ������ ������������ ����.
*	���� ��� ������� ������� ����� ��� ������ - ����� �������������, ��� �����
*	������������� ������� ���������. � ��� ���������� �� ������ ���� ������
*	������ ������� pProtocolOptions->OnVerify()
*	ch - ��������� �������� ������
********************************************************************************/
bool PROTOCOL_Struct::ReceiveChar(unsigned char ch)
{
	// ��������� ������ ������ ����  �� �� ���������� ����30� (������ L_DAT = 8)
	//************************************************************************************************************************
	#ifdef SIMULATION_RX
	{
			#define SIMULATION_RX  TRUE;

			static int MSG_NUM = 0;

			UINT8 len_sim = 0x08;				//����� ������ (������ 20 = DATA(19) + CS(1))

			unsigned char* CH_ptr = NULL;		//��������� �� ������ � ��������� �������


			static unsigned char CH_0[8 + 2]{ NULL };
			{
				CH_0[0] = 0xFF;		// Byte Start		#1
				CH_0[1] = 0x08;		// Length   		#2
				CH_0[2] = 0x10;		//   D-1			#3
				CH_0[3] = 0x01;		//   D-2			#4
				CH_0[4] = 0x5E;		//   D-3			#5
				CH_0[5] = 0x00;		//   D-4			#6
				CH_0[6] = 0x00;		//   D-5			#7
				CH_0[7] = 0x00;		//   D-6			#8
				CH_0[8] = 0x00;		//   D-7			#9
				CH_0[9] = 0x89;		//   D-8  CS		#10
			}

			static unsigned char CH_1[8 + 2]{ NULL };
			{
				CH_1[0] = 0xFF;		// Byte Start		#1
				CH_1[1] = 0x08;		// Length   		#2
				CH_1[2] = 0x10;		//   D-1			#3
				CH_1[3] = 0x01;		//   D-2			#4
				CH_1[4] = 0x5E;		//   D-3			#5
				CH_1[5] = 0xFF;		//   D-4			#6
				CH_1[6] = 0x00;		//   D-5			#7
				CH_1[7] = 0x01;		//   D-6			#8
				CH_1[8] = 0x02;		//   D-7			#9
				CH_1[9] = 0x87;		//   D-8  CS		#10
			}

			static unsigned char CH_2[8 + 2]{ NULL };
			{
				CH_2[0] = 0xFF;		// Byte Start		#1
				CH_2[1] = 0x08;		// Length   		#2
				CH_2[2] = 0x10;		//   D-1			#3
				CH_2[3] = 0x01;		//   D-2			#4
				CH_2[4] = 0x5E;		//   D-3			#5
				CH_2[5] = 0xFF;		//   D-4			#6
				CH_2[6] = 0xFF;		//   D-5			#7
				CH_2[7] = 0x01;		//   D-6			#8
				CH_2[8] = 0x04;		//   D-7			#9
				CH_2[9] = 0x86;		//   D-8  CS		#10
			}

			static unsigned char CH_3[8 + 2]{ NULL };
			{
				CH_3[0] = 0xFF;		// Byte Start		#1
				CH_3[1] = 0x08;		// Length   		#2
				CH_3[2] = 0x10;		//   D-1			#3
				CH_3[3] = 0x01;		//   D-2			#4
				CH_3[4] = 0x5E;		//   D-3			#5
				CH_3[5] = 0xFF;		//   D-4			#6
				CH_3[6] = 0xFE;		//   D-5			#7
				CH_3[7] = 0x05;		//   D-6			#8
				CH_3[8] = 0x06;		//   D-7			#9
				CH_3[9] = 0x81;		//   D-8  CS		#10
			}

			static unsigned char CH_4[8 + 2]{ NULL };
			{
				CH_4[0] = 0xFF;		// Byte Start		#1
				CH_4[1] = 0x08;		// Length   		#2
				CH_4[2] = 0x10;		//   D-1			#3
				CH_4[3] = 0x01;		//   D-2			#4
				CH_4[4] = 0x5E;		//   D-3			#5
				CH_4[5] = 0xFF;		//   D-4			#6
				CH_4[6] = 0x07;		//   D-5			#7
				CH_4[7] = 0xFF;		//   D-6			#8
				CH_4[8] = 0x14;		//   D-7			#9
				CH_4[9] = 0x70;		//   D-8  CS		#10
			}

			static unsigned char CH_5[8 + 2]{ NULL };
			{
				CH_5[0] = 0xFF;		// Byte Start		#1
				CH_5[1] = 0x08;		// Length   		#2
				CH_5[2] = 0x10;		//   D-1			#3
				CH_5[3] = 0x01;		//   D-2			#4
				CH_5[4] = 0x5E;		//   D-3			#5
				CH_5[5] = 0xFF;		//   D-4			#6
				CH_5[6] = 0xFF;		//   D-5			#7
				CH_5[7] = 0xFF;		//   D-6			#8
				CH_5[8] = 0xFF;		//   D-7			#9
				CH_5[9] = 0x8D;		//   D-8  CS		#10
			}

			static unsigned char CH_6[8 + 2]{ NULL };
			{
				CH_6[0] = 0xFF;		// Byte Start		#1
				CH_6[1] = 0x08;		// Length   		#2
				CH_6[2] = 0x10;		//   D-1			#3
				CH_6[3] = 0x01;		//   D-2			#4
				CH_6[4] = 0xD6;		//   D-3			#5
				CH_6[5] = 0x00;		//   D-4			#6
				CH_6[6] = 0xFF;		//   D-5			#7
				CH_6[7] = 0xFF;		//   D-6			#8
				CH_6[8] = 0xFF;		//   D-7			#9
				CH_6[9] = 0x14;		//   D-8  CS		#10
			}

			static unsigned char CH_7[8 + 2]{ NULL };
			{
				CH_7[0] = 0xFF;		// Byte Start		#1
				CH_7[1] = 0x08;		// Length   		#2
				CH_7[2] = 0x10;		//   D-1			#3
				CH_7[3] = 0xEE;		//   D-2			#4
				CH_7[4] = 0xFF;		//   D-3			#5
				CH_7[5] = 0xFF;		//   D-4			#6
				CH_7[6] = 0xFF;		//   D-5			#7
				CH_7[7] = 0xFF;		//   D-6			#8
				CH_7[8] = 0xFF;		//   D-7			#9
				CH_7[9] = 0xFF;		//   D-8  CS		#10
			}

			static unsigned char CH_8[8 + 2]{ NULL };
			{
				CH_8[0] = 0xFF;		// Byte Start		#1
				CH_8[1] = 0x08;		// Length   		#2
				CH_8[2] = 0x10;		//   D-1			#3
				CH_8[3] = 0x01;		//   D-2			#4
				CH_8[4] = 0x5E;		//   D-3			#5
				CH_8[5] = 0x00;		//   D-4			#6
				CH_8[6] = 0x00;		//   D-5			#7
				CH_8[7] = 0x00;		//   D-6			#8
				CH_8[8] = 0x8A;		//   D-7			#9
				CH_8[9] = 0xFF;		//   D-8  CS		#10
			}

			static unsigned char CH_9[8 + 2]{ NULL };
			{
				CH_9[0] = 0x08;		// Byte Start		#1
				CH_9[1] = 0x10;		// Length   		#2
				CH_9[2] = 0x01;		//   D-1			#3
				CH_9[3] = 0x5E;		//   D-2			#4
				CH_9[4] = 0x00;		//   D-3			#5
				CH_9[5] = 0x00;		//   D-4			#6
				CH_9[6] = 0x00;		//   D-5			#7
				CH_9[7] = 0x00;		//   D-6			#8
				CH_9[8] = 0x89;		//   D-7			#9
				CH_9[9] = 0xFF;		//   D-8  CS		#10
			}


			//������� ������ �� ������ � �������
			switch (MSG_NUM)
			{
				case 0:		CH_ptr = &CH_0[0];		break;
				case 1:		CH_ptr = &CH_1[0];		break;
				case 2:		CH_ptr = &CH_2[0];		break;
				case 3:		CH_ptr = &CH_3[0];		break;
				case 4:		CH_ptr = &CH_4[0];		break;
				case 5:		CH_ptr = &CH_5[0];		break;
				case 6:		CH_ptr = &CH_6[0];		break;
				case 7:		CH_ptr = &CH_7[0];		break;
				case 8:		CH_ptr = &CH_8[0];		break;
				case 9:		CH_ptr = &CH_9[0];		break;
			}

			/*
				//��������� CS ��� ���������� ������ (�������)
				UINT8 CS = NULL;
				{
					for (int i = 0; i < len_sim; ++i)
						CS += CH_ptr[i + 1];
					CS = (~CS) + 1;
				}
				CH_ptr[len_sim + 1] = CS;
			*/

			static int  n = 0;				// # byte

			static bool l_flag_stuff = false;

			switch (n)		//��������� n-�� ���� ������������ ������
			{
				case 0:	   ch = CH_ptr[0];    break;	// SD    - Byte Start
				case 1:    ch = CH_ptr[1];    break;	// L_DAT - Length

				default:	// DATA - BS - CS
				{
					if ((n < (len_sim + 1)) || (((n == (len_sim + 1))) && (l_flag_stuff == true)))	// DATA - BS
					{
						ch = CH_ptr[n];					// DATA

						if (l_flag_stuff == true)		// BS
						{								//
							ch = 0xFE;					//
							l_flag_stuff = false;		//
							n--;						//
						}								//
														//
						if (ch == 0xFF)					//
						{								//
							l_flag_stuff = true;		//		!!!!! ���� ������ �������� ������������� � ���������,   �� ����������������
						}								//

						break;
					}

					else if (n == (len_sim + 1))		// CS
					{									//
						ch = CH_ptr[len_sim + 1];		//		!!!!! ���� ������ ����������� �����,   �� ����������������
						l_flag_stuff = false;			//
						break;							//
					}									//
				}
			}

			n++;



			if (n > (len_sim + 1))	//��� ������ ���������� �����
			{
				n = 0;
				switch (MSG_NUM)	//������ ��������� ����� �� ��������
				{
					case 0:		MSG_NUM = 1;	break;
					case 1:		MSG_NUM = 2;	break;
					case 2:		MSG_NUM = 3;	break;
					case 3:		MSG_NUM = 4;	break;
					case 4:		MSG_NUM = 5;	break;
					case 5:		MSG_NUM = 6;	break;
					case 6:		MSG_NUM = 7;	break;
					case 7:		MSG_NUM = 8;	break;
					case 8:		MSG_NUM = 9;	break;
					case 9:		MSG_NUM = 0;	break;

					default:	MSG_NUM = 0;	break;
				}
			}
	}
	#endif
	//************************************************************************************************************************


	//********************************************************************************************************************************
	#ifndef MSG_RX_OLD_TYPE
	// ����� ���������� (� ������������ ������ ����-���������)
	{
		/*
		*  ������� �������� � ������������ ����-����������.
		*  ������ ������ ������������� �� ���������� �����. ��������� ���������� �����
		*  � ������ �������������� ����-����������. ���������� ������������ ������
		*  �������������� � �������� ��� ������������ (�����, ������, ����������� �����).
		*/

		static unsigned char    MSULink_RxState = MSULINK_RX_STATE_SD;				/* ������� ��������� (�������������)			*/

		static bool b_Show_BS = pProtocolOptions->o_LogInByteStuffing; 				/* ���� ������ ����-��������� ��� ��� ������	*/

		static bool b_MsgType = pProtocolOptions->o_MsgType;						/* ������ ����� ������:							*/
																					/*  0 - ��� ����� CS (L_DAT = 7);				*/
																					/*	1 - � ������  CS (L_DAT = 8).				*/

		static CMessage  lsReseivingMessage(ReceiveMessage);						/* ��������� - �������� ���������				*/

		static unsigned char    receiveMessage_bs[50];								/* ����� ��� �������� ������    (� BS)			*/
		static unsigned short   char_count_bs = 0;									/* ���������� �������� �������� (� BS)			*/

		static unsigned short	char_count = 0;										/* ���������� �������� ��������					*/

		static unsigned char	pre_last_char = 0;									/* ������, ���������� �������������				*/
		static unsigned char	    last_char = 0;									/* ������, ���������� ��������� 				*/

		static unsigned char    MSULink_RxRemainLen = NULL;							/* ������� ���������� ����� ������				*/



		/**/   bool     result = true;												/* ���� �� ������   (true - �� ������)			*/
		static CString  error = L"";												/* ������������ ������ � ������� ������			*/

		enum MSULink_RxState_Ending_												/*       �������  ��������  ��������:			*/
		{
			RX_STATUS_BAD_DEF_to_SD,												/* ������ - ���� � �������� ��������� 			*/
			RX_STATUS_BAD_SD_to_SD,													/* ������ - ������������ ������ ������			*/
			RX_STATUS_BAD_L_DAT_1_to_L_DAT,											/* ������ - ������������ ����� ������			*/
			RX_STATUS_BAD_L_DAT_2_to_SD,											/* ������ - ������������ ����� ������			*/
			RX_STATUS_BAD_BS_1_to_L_DAT,											/* ������ - ����������� ����-���������			*/
			RX_STATUS_BAD_BS_2_to_SD,												/* ������ - ����������� ����-���������			*/
			RX_STATUS_BAD_BS_3_to_DATA,												/* ������ - ����������� ����-���������			*/
			RX_STATUS_BAD_CHKSUM_1_to_L_DAT,										/* ������ - ������������ ����������� �����		*/
			RX_STATUS_BAD_CHKSUM_2_to_SD,											/* ������ - ������������ ����������� �����		*/
			RX_STATUS_GOOD,															/* ������� � ����� ����. ��� ������ (�� �����)	*/
			RX_STATUS_SUCCESS														/* �������� ���������� ���������� ���������		*/
		};

		static unsigned char    MSULink_RxState_Ending = RX_STATUS_SUCCESS;			/* ������ �������� �������� � ����� ����.		*/



		// ��������� �������� ����������
		{
			if (pProtocolOptions->var_Regime != BYTE_STUFFING)
			{
				m_pError->SetERROR(_T("������������ ����� ��������� ��������� �������"), ERP_ERROR | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
				return false;
			}

			if (pProtocolOptions->OnVerify == NULL)		return false;
			if (ReceiveMessage             == NULL)		return false;
		}

		// ������� ������ ������
		{
			pre_last_char = last_char;												/* ������, ���������� �������������				*/
			last_char     = ch;														/* ������, ���������� ��������� 				*/
		}

		// ������ � �����
		{
			char_count++;															/* ������� ��� ���� ������						*/
			pUSART_Dlg->pPROTOCOL->ReceiveMessage[char_count - 1] = ch;				/* ������ ������ ��������� ������ � �����,		*/
			pUSART_Dlg->pPROTOCOL->ReceiveMessage[char_count - 0] = 0;				/* � ����� �� ���� ������ ��� �����������		*/

			char_count_bs++;														/* �� �� ����� ��� ������ � ������� �����.	BS	*/
			receiveMessage_bs[char_count_bs - 1] = ch;  							/*												*/
			receiveMessage_bs[char_count_bs - 0] = 0;								/*												*/
		}

		//����� ������ �� �����
		{
			// ����������� �������� ��������� ������ � ����
			if (pUSART_Dlg->m_ReceiveMessage.m_hWnd != NULL)
			{//����� �� ���������� ������� � �������������� ����
				CString str = GiveStrFromHex(receiveMessage_bs, char_count_bs);
				pUSART_Dlg->m_ReceiveMessage.SetWindowText(str);					/* ������� ���������� ������					*/
			}
		}



		// �������  ���������
		switch (MSULink_RxState)
		{
			case MSULINK_RX_STATE_SD:												/* �������� ���������� �����������				*/
				{
					b_MsgType = pProtocolOptions->o_MsgType;						/* ��������� ������ ����� ������				*/
					b_Show_BS = pProtocolOptions->o_LogInByteStuffing;				/* ��������� ���� ������ ����-���������			*/

					if (ch == pProtocolOptions->var_FIRST_BYTE) 					/* ���� ������ ��������� �����������			*/
					{//GOOD
						error = _T("...");											/* ��������� ��������� � ����������				*/
						MSULink_RxState = MSULINK_RX_STATE_L_DAT;					/* ����. ��������� �������� ����� ������		*/
						MSULink_RxState_Ending = RX_STATUS_GOOD;					/* ������ ���������� �������� ���������			*/
					}
					else 															/* ���� ������ �� ��������� �����������			*/
					{//BAD
						error = _T("������. ������������ ������ ������");			/* ��������� ��������� � ����������				*/
						MSULink_RxState = MSULINK_RX_STATE_SD;						/* ����. ��������� �������� ������ ������		*/
						MSULink_RxState_Ending = RX_STATUS_BAD_SD_to_SD;			/* ������ ���������� �������� ���������			*/
					}
				}
				break;
			case MSULINK_RX_STATE_L_DAT:											/* �������� ����� ������ (COM+DATA+CS)			*/
				{
					if (ch == pProtocolOptions->var_FIRST_BYTE) 					/* ���� ������ ��������� �����������			*/
					{//BAD
						error = _T("������. ������������ ���� ����� ������");		/* ��������� ��������� � ����������				*/
						MSULink_RxState = MSULINK_RX_STATE_L_DAT;					/* ����. ��������� �������� ����� ������		*/
						MSULink_RxState_Ending = RX_STATUS_BAD_L_DAT_1_to_L_DAT;	/* ������ ���������� �������� ���������			*/
					}
					else if (   (ch <  (b_MsgType ? 2 : 1))							/* ���� ����� ������ < 1(2) (COM+(CS))  		*/
							 || (ch >  0x14)										/* ��� ����� ������ ������ ����. ��������		*/
							 || (ch >  pProtocolOptions->var_MAX_LEN) 				/* ��� ����� ������ ������ ������       		*/
							 || (ch == pProtocolOptions->var_STUF_BYTE))			/* ��� ����� ������ ����� ����-��������� 		*/
					{//BAD
						error = _T("������. ������������ ���� ����� ������");		/* ��������� ��������� � ����������				*/
						MSULink_RxState = MSULINK_RX_STATE_SD;						/* ����. ��������� �������� ������ ������		*/
						MSULink_RxState_Ending = RX_STATUS_BAD_L_DAT_2_to_SD;		/* ������ ���������� �������� ���������			*/
					}
					else
					{//GOOD
						MSULink_RxRemainLen = ch - ((b_MsgType ? 1 : 0));			/* ������� �����. ����� ������ (�������������)	*/
						MSULink_RxState = MSULINK_RX_STATE_DATA;					/* ����. ��������� �������� ������  			*/
						MSULink_RxState_Ending = RX_STATUS_GOOD;					/* ������ ���������� �������� ���������			*/
					}
				}
				break;
			case MSULINK_RX_STATE_DATA:												/* �������� ������								*/
				{
					MSULink_RxRemainLen--;											/* ���������� �������� ���������� ����� ������	*/

					if (MSULink_RxRemainLen != 0)									/* ���� ������� ����� ��� �� ��������			*/
					{//GOOD
						MSULink_RxState = MSULINK_RX_STATE_DATA;					/* ����. ��������� �������� ������  			*/
						MSULink_RxState_Ending = RX_STATUS_GOOD;					/* ������ ���������� �������� ���������			*/
					}
					else if (MSULink_RxRemainLen == 0) 								/* ���� ������� ����� ��������					*/
					{//GOOD
						MSULink_RxState = MSULINK_RX_STATE_CHKSUM;					/* ����. ��������� �������� �����. �����		*/
						MSULink_RxState_Ending = RX_STATUS_GOOD;					/* ������ ���������� �������� ���������			*/
					}

					if (ch == pProtocolOptions->var_FIRST_BYTE) 					/* ���� ������ ���� = ��������� �����������		*/
					{//GOOD
						MSULink_RxState = MSULINK_RX_STATE_BS;						/* ����. ��������� �������� ����-���������		*/
						MSULink_RxState_Ending = RX_STATUS_GOOD;					/* ������ ���������� �������� ���������			*/
					}
				}
				break;
			case MSULINK_RX_STATE_BS:												/* �������� ����-���������						*/
				{
					if (ch == pProtocolOptions->var_STUF_BYTE) 						/* ���� ������ ����-��������					*/
					{
						if (MSULink_RxRemainLen == 0)								/* ���� ������� ����� ��������					*/
						{//GOOD
							MSULink_RxState = MSULINK_RX_STATE_CHKSUM;				/* ����. ��������� �������� �����.�����			*/
							MSULink_RxState_Ending = RX_STATUS_GOOD;				/* ������ ���������� �������� ���������			*/
						}
						else
						{//GOOD
							MSULink_RxState = MSULINK_RX_STATE_DATA;				/* ����. ��������� �������� ������				*/
							MSULink_RxState_Ending = RX_STATUS_GOOD;				/* ������ ���������� �������� ���������			*/
						}
						char_count--;												/* �� ������� ���� ��������� ���� �����.		*/
					}
					else 															/* ���� ������ �� ����-��������					*/
					{
						error = _T("������. ����� �� �������� (��� ����-���������)");	/* ��������� ��������� � ����������			*/
						result = false;													/* ����, ��� ��������� ������				*/

						if (ch == pProtocolOptions->var_FIRST_BYTE)					/* ���� ������ ��������� �����������			*/
						{//BAD
							MSULink_RxState = MSULINK_RX_STATE_L_DAT;				/* ����. ��������� �������� ����� ������		*/
							MSULink_RxState_Ending = RX_STATUS_BAD_BS_1_to_L_DAT;	/* ������ ���������� �������� ���������			*/
						}
						else														/* ���� ������ �� ��������� �����������			*/
						{
							if (   (ch < (b_MsgType ? 2 : 1))						/* ���� ����� ������ < 1(2) (COM+(CS))  		*/
								|| (ch == pProtocolOptions->var_STUF_BYTE)			/* ��� ����� ������ ����� ����-��������� 		*/
								|| (ch > pProtocolOptions->var_MAX_LEN))			/* ��� ����� ������ ������ ������       		*/
							{//BAD
								MSULink_RxState = MSULINK_RX_STATE_SD;				/* ����. ��������� �������� ������ ������		*/
								MSULink_RxState_Ending = RX_STATUS_BAD_BS_2_to_SD;	/* ������ ���������� �������� ���������			*/
							}
							else
							{//BAD
								MSULink_RxState = MSULINK_RX_STATE_DATA;				/* ����. ��������� �������� ������  		*/
								MSULink_RxState_Ending = RX_STATUS_BAD_BS_3_to_DATA;	/* ������ ���������� �������� ���������		*/
							}
						}
					}
				}
				break;
			case MSULINK_RX_STATE_CHKSUM:											/* �������� ����������� �����					*/
				{
					unsigned char sum  = ch;
					unsigned char sum2 = pProtocolOptions->CURRENT_SUMM(ReceiveMessage, 1, char_count - 1);

					if (sum == sum2)												/* �������� ����������� �����					*/
					{//SUCCESS
						error = _T("��� ������");									/* ��������� ��������� � ����������				*/
						MSULink_RxState = MSULINK_RX_STATE_SD;						/* ����. ��������� �������� ������ ������		*/
						MSULink_RxState_Ending = RX_STATUS_SUCCESS;					/* ������ ���������� �������� ���������			*/

						if (b_Show_BS) { error += _T(" -BS"); }						/* ���� �����. ����-�������� ������� ��������	*/
					}
					else
					{
						if (ch == pProtocolOptions->var_FIRST_BYTE) 					/* ���� ������ ��������� �����������		*/
						{//BAD
							error = _T("������. �������� ����������� �����");			/* ��������� ��������� � ����������			*/
							MSULink_RxState = MSULINK_RX_STATE_L_DAT;					/* ����. ��������� �������� ������ ������	*/
							MSULink_RxState_Ending = RX_STATUS_BAD_CHKSUM_1_to_L_DAT;	/* ������ ���������� �������� ���������		*/
						}
						else 															/* ���� ������ �� ��������� �����������		*/
						{//BAD
							error = _T("������. �������� ����������� �����");			/* ��������� ��������� � ����������			*/
							MSULink_RxState = MSULINK_RX_STATE_SD;						/* ����. ��������� �������� ������ ������	*/
							MSULink_RxState_Ending = RX_STATUS_BAD_CHKSUM_2_to_SD;		/* ������ ���������� �������� ���������		*/
						}
					}
				}
				break;
			default:																/* ���� � �������� ���������					*/
				{//BAD
					error = _T("������ � �������� ���������");						/* ��������� ��������� � ����������				*/
					MSULink_RxState = MSULINK_RX_STATE_SD;							/* ����. ��������� �������� ������ ������		*/
					MSULink_RxState_Ending = RX_STATUS_BAD_DEF_to_SD;				/* ������ ���������� �������� ���������			*/
				}
				break;
		}	// END - ������� ���������



		// ��������� ������
		if (MSULink_RxState_Ending != RX_STATUS_GOOD)
		{
			if (MSULink_RxState_Ending == RX_STATUS_SUCCESS) {
				result = true;
			}
			else {
				result = false;
			}


			pUSART_Dlg->pPROTOCOL->AddInMessageCount();								/* �������� ����� �������� �������				*/


			if (MSULink_RxState_Ending == RX_STATUS_SUCCESS)
			{
				//����� ������ ������� ������ � �����.
				//����� ��������� (�� ����� � ����������� �����) � ��� ����� ���������� �  OnVerify().
				// P.S.:  ����� �� ������ ��������� ����-�������� (��. VERSION_4_1 -> my_function.cpp -> OnVerify())
				result = pProtocolOptions->OnVerify(&error, pUSART_Dlg->pPROTOCOL->ReceiveMessage);

				// P.P.S: OnVerify() ����� �� ������ ���������� true  (�� ����� ��� � ��������� �� ���???)
			}


			if (b_Show_BS)
			{
				for (int i = 0; i < char_count_bs + 1; i++) {
					pUSART_Dlg->pPROTOCOL->ReceiveMessage[i] = receiveMessage_bs[i];
				}

				char_count = char_count_bs;
			}


			// ����� � ����
			{
				if (   (MSULink_RxState_Ending == RX_STATUS_SUCCESS)
					|| (MSULink_RxState_Ending == RX_STATUS_BAD_DEF_to_SD)
					|| (MSULink_RxState_Ending == RX_STATUS_BAD_SD_to_SD)
					|| (MSULink_RxState_Ending == RX_STATUS_BAD_L_DAT_2_to_SD)
					|| (MSULink_RxState_Ending == RX_STATUS_BAD_BS_2_to_SD)
					|| (MSULink_RxState_Ending == RX_STATUS_BAD_CHKSUM_2_to_SD))
				{
					//char_count += 0;												/* NOP											*/
				}

				if (   (MSULink_RxState_Ending == RX_STATUS_BAD_L_DAT_1_to_L_DAT)
					|| (MSULink_RxState_Ending == RX_STATUS_BAD_BS_1_to_L_DAT)
					|| (MSULink_RxState_Ending == RX_STATUS_BAD_CHKSUM_1_to_L_DAT))
				{
					char_count += -1;												/* �� ����� ��������� � ���� 1 ��������� ����	*/
				}

				if (MSULink_RxState_Ending == RX_STATUS_BAD_BS_3_to_DATA)
				{
					char_count += -2;												/* �� ����� ��������� � ���� 2 ��������� �����	*/
				}


				lsReseivingMessage.length = char_count;								/* ������� ���� ��������� ��������� (�����)		*/
				lsReseivingMessage.name   = error;									/* ������ � ������� ������						*/

				WriteMesFile(&lsReseivingMessage, UART_MSG_RECEIVED);				/* ������� � ����								*/
			}


			// ������� ������ ������ ������
			{
				if (   (MSULink_RxState_Ending == RX_STATUS_BAD_L_DAT_1_to_L_DAT)
					|| (MSULink_RxState_Ending == RX_STATUS_BAD_BS_1_to_L_DAT)
					|| (MSULink_RxState_Ending == RX_STATUS_BAD_CHKSUM_1_to_L_DAT))
				{
					char_count = 1;													/* ������������� ������� �������� ��������		*/
					pUSART_Dlg->pPROTOCOL->ReceiveMessage[0] = last_char;			/* �������� � ������ ������ 1 ��������� ����	*/

					char_count_bs = 1;												/* �� �� ����� ��� ������ � ������� �����.	BS	*/
					receiveMessage_bs[0] = last_char;  								/*												*/
				}
				else if (MSULink_RxState_Ending == RX_STATUS_BAD_BS_3_to_DATA)
				{
					char_count = 2;													/* ������������� ������� �������� ��������		*/
					pUSART_Dlg->pPROTOCOL->ReceiveMessage[0] = pre_last_char;		/* �������� � ������ ������ 2 ��������� �����	*/
					pUSART_Dlg->pPROTOCOL->ReceiveMessage[1] = last_char;			/*												*/

					char_count_bs = 2;												/* �� �� ����� ��� ������ � ������� �����.	BS	*/
					receiveMessage_bs[0] = pre_last_char; 							/*												*/
					receiveMessage_bs[1] = last_char;								/*												*/
				}
				else
				{
					char_count    = 0;												/* ������������� ������� �������� ��������		*/
					char_count_bs = 0;					 							/*												*/
				}
			}


			if (pProtocolOptions->NotifyProgram != NULL)
			{//��������� ������� ��������� �� ������� ��� ��������� ������
				if (result == true)
				{//������ ���
					pProtocolOptions->NotifyProgram(NP_MESSAGE_RESEIVED);
				}
				else
				{//���� ������, �� ��� �� ������ ���������, � �� ������
					pProtocolOptions->NotifyProgram(NP_RECEIVE_PROTOCOL_ERROR);
				}
			}
		}


		pUSART_Dlg->m_InMessageError.SetWindowText(error);							/* ������� ������ /���������					*/

		return  result;
	}
	#else
	//********************************************************************************************************************************
	// ��� ���� � �������
	{
		if (pProtocolOptions->OnVerify == NULL)	return false;
		if (ReceiveMessage == NULL)				return false;

		/*����� ��� ���� ���������� ����������*/
		static unsigned char	pred_char = 0;						//���������� ������
		static unsigned short	char_count = 0;						//���������� �������� ��������
		static CString error;
		static CMessage lsReseivingMessage(ReceiveMessage);

		char_count++;//������� ��� ���� ������

		/*if(pUSART_Dlg->m_hWnd==NULL || pUSART_Dlg->m_ReceiveMessage.m_hWnd==NULL)
		{
			this->m_pError->SetERROR(_T("���� ����� ���������, �� �������� �������������� � ������� ReceiveChar"),ERP_FATALERROR | ER_WRITE_TO_LOG);
			return false;
		}*/

		if (pProtocolOptions->var_Regime == PACKET_LEN)
		{//����� � ������������� ����� �����
			/*
			�������� � ������������� ����� �����, ������ ��������� ����� ���� ����� �������,
			����� ������������� ����� ������ (�� �����).
			*/

			static bool verifi_on = 0;			//���� 1, �� ����� ��������� � ������������
												//� ������ �����

			static bool FirstByteReady = false;	//��������, ��������� ��� �� ���������� (�� ����� ����
												//������, � � ����� �������� ����� ������ ������.
												//���� ���� �������� ����������� ��������� ������ ������
												//���������

			static unsigned short paket_length = 0;	//����� ���������� ����� ������������� ����� ������
			static unsigned short input_count = 0;	//���������� �������� ����

			unsigned char RC_register = ch;			//������ ���

			//��������� ����� ������ ������ �������� ��������� ������, �� �� ������������ � ����
			static bool Chepuha = true;		//����� ���������� ������ ��������� ��������� ��� �� ������ �� ���� ��������� ������
			static bool LenCorrect = false;	//�������� ���� ����� ����� ���������� ��������	
			static bool TheEnd = false;		//������ ��� ������� ����� ���������, ��� ����� ���������� �� ������
			static bool SumCorrect = false;

			if (input_count > pProtocolOptions->var_MAX_LEN)//������������ ������, � �� ���� ������� ����
			{//������ �������
				input_count = 0;
			}

			if (TheEnd)
			{//� ������� ��� ��������� ��������� ���������, � � ���� �������� ������� � ���������� ������
				TheEnd = false;
				Chepuha = true;
			}

			//������� ��������� �������� ������
			ReceiveMessage[input_count] = RC_register;	//���������� � ����� ������
			input_count++;								//�������� ���������� �������� ��������

			//���� ����� �� �������, � ��� �� ����������� $ (�����, �����, ������)
			if (RC_register == pProtocolOptions->var_FIRST_BYTE && FirstByteReady == false)
			{
				input_count = 0;			//������ ����� ��������� - �������� �������
				ReceiveMessage[0] = RC_register;	//������� ���� ������ ��� �� ���� �����
				input_count = 1;			//������ ����� ��������� - �������� �������
				FirstByteReady = true;	//������ ������ ���� ��� �������

				Chepuha = false;		//��� ��� ����������� �����
				LenCorrect = false;	//���� ��� ������ �� ����� � �����
				SumCorrect = false;	//����������� ����� ��� �� �������
			}
			else if (Chepuha == false)
			{//����� �������
				if (input_count == pProtocolOptions->var_LEN_STATE + 1)//��������� �������� ������
				{//����� ��� ���������, � �� ���������, � ���� ����� ����� ����� ������ � ������
					if (RC_register > pProtocolOptions->var_MAX_LEN - pProtocolOptions->var_LEN_ADD)
					{//���� ����� ������� ������� �������
						paket_length = pProtocolOptions->var_MAX_LEN;
						FirstByteReady = false;	//������ �������� ����� �������
					}
					else
					{
						//��������, ����� ���-�� �������� ��� ������� �������
						paket_length = RC_register + pProtocolOptions->var_LEN_ADD;

						LenCorrect = true;	//���� ����� ���� ���������
					}
				}
				else if (input_count == paket_length)
				{//��������� ������� �������, ���� ��������� ����������� �����
					if (LenCorrect)
					{//���� ����� ���� ���������, �� ����� ����������� � �������������������� �����
						unsigned char sim = ch;
						unsigned char sim2 = pProtocolOptions->CURRENT_SUMM(ReceiveMessage, 1, input_count - 1);
						if (sim != sim2)
						{//������ � ����������� �����
						}
						else
						{//����������� ����� �����
							SumCorrect = true;
						}
					}
					FirstByteReady = false;	//����� ������ ��������� ����� ���������

					TheEnd = true;		//�������� ���� ����� ���������
				}
			}


			//��������� ��, ��� ������� �� ������
			if (pProtocolOptions->o_HexDisplayInMessage)
			{
				////////////////////////////////////////////////////////////////////////////////////////////
				//	����� ������������, ��� ��� ����� - �������������� ��� ���������������
				////////////////////////////////////////////////////////////////////////////////////////
				if (!pProtocolOptions->OptimazeInMessageView)
				{//����� ���������� ������ �������� ������
					static CString hex_view = _T("");	//���� ����� ������������ ����������������� ��� ����������� �������
					if (input_count == 1)
					{//��� ������ ������ �������, ������� �����
						hex_view = "";
					}

					//������� ���������� � ����������� ���������
					CString AboutMessage;
					if (Chepuha)
					{
						AboutMessage = _T("������� ����...");
					}
					else if (FirstByteReady || TheEnd)
					{//��������� ���� ��� �������, ���� ��������� ������
						AboutMessage = _T("������ ����� ���������");
					}

					if (input_count > pProtocolOptions->var_LEN_STATE)
					{
						if (LenCorrect)
						{
							AboutMessage.Append(_T("; ���� ����� ���������"));
						}
						else
						{
							if (!Chepuha)
							{//���� ������� ����, �� ������ � ���������� ��������������� ���������
								AboutMessage.Append(_T("; �������� ���� �����"));
							}
						}
						if (TheEnd == true)
						{//������� ��� ��������� (�� ��� ����������� ����� �������)
							if (SumCorrect)
							{
								if (pProtocolOptions->OnVerify != NULL)
								{
									pProtocolOptions->OnVerify(&AboutMessage, ReceiveMessage);
								}
							}
							else
							{
								AboutMessage.Append(_T("; ����������� ����� �����������"));
							}
							//�������� ����� �������� �������
							AddInMessageCount();
							//������� ���� ��������� ���������
							lsReseivingMessage.length = input_count;
							lsReseivingMessage.name = AboutMessage;
							//������� � ����
							WriteMesFile(&lsReseivingMessage, UART_MSG_RECEIVED);
						}
					}
					//������� ���������� � �������� ���������
					pUSART_Dlg->m_InMessageError.SetWindowText(AboutMessage);

					//������� ����������������� ��� ���������� �������
					hex_view.Append(GiveStrFromHex(&ch, 1));

					//������� �� ������ ���� �������� ���������
					pUSART_Dlg->m_ReceiveMessage.SetWindowText(hex_view);

				}
				else
				{//������ ��� ��� ����� ��������������, ��������� ������ ��������� �������� ���������
					if (TheEnd == true)
					{//������ ��� ��������� ��������� ���������
						//��������� � ������������� ���
						CString str = GiveStrFromHex(ReceiveMessage, input_count);

						CString AboutMessage;
						if (!Chepuha && LenCorrect && SumCorrect)
						{//���� ��� ���� ���������, �� ����� ��� � �����������������
							if (pProtocolOptions->OnVerify != NULL)
							{
								pProtocolOptions->OnVerify(&AboutMessage, ReceiveMessage);
							}
						}
						else if (!LenCorrect) AboutMessage = _T("������������ ���� �����");
						else if (!SumCorrect) AboutMessage = _T("�������� ����������� �����");

						pUSART_Dlg->m_ReceiveMessage.SetWindowText(str);//������� ��������� 
						//������� ���������� � ����������� ���������
						pUSART_Dlg->m_InMessageError.SetWindowText(AboutMessage);
						//�������� ����� �������� �������
						AddInMessageCount();
						//������� ���� ��������� ���������
						lsReseivingMessage.length = input_count;
						lsReseivingMessage.name = AboutMessage;
						//������� � ����
						WriteMesFile(&lsReseivingMessage, UART_MSG_RECEIVED);
					}
				}
			}
			else
			{//������� ��������� ���, ��� ��� ����
				USES_CONVERSION;
				pUSART_Dlg->m_ReceiveMessage.SetWindowText(A2T((char*)ReceiveMessage));
			}

			if (TheEnd && pProtocolOptions->NotifyProgram != NULL)
			{//��������� ������� ��������� �� ������� ��� ��������� ������
				if (!Chepuha && LenCorrect && SumCorrect)
				{//������ ���
					pProtocolOptions->NotifyProgram(NP_MESSAGE_RESEIVED);
				}
				else
				{//���� ������, �� ��� �� ������ ���������, � �� ������
					pProtocolOptions->NotifyProgram(NP_RECEIVE_PROTOCOL_ERROR);
				}
			}
		}
		else if (pProtocolOptions->var_Regime == BYTE_STUFFING)
		{//����� ������-�������� � ��������������

			/*
			������� �������� � ������������ ����-����������. ���������� ����������� ���� ������������� �� ����� ������,
			� ������ ����� ������ ������������� ������ �� ������ ����������. �������������, ������ ���������� ��������
			� ���� ����� (��� �����)
			*/

			static CString	error = _T("������ ���������");		//������������ ������ � ������� ������
			static bool		staf_flag = 0;							//����� ����������� ���� ��������


			if (pred_char == pProtocolOptions->var_FIRST_BYTE)
			{
				if (ch != pProtocolOptions->var_STUF_BYTE)
				{//����� ���������
					if (char_count >= 3)
					{//��� ���� ��� �� ������ ���������, � ��� ����� ������� ��������� ����������
						//����� �� ��������� ������������ ���-����� ����������� ������, �.�. � ���� ������ ��������� ����
						INT8U sim = pUSART_Dlg->pPROTOCOL->ReceiveMessage[char_count - 3]; //-3, �.�. ������������ ����� ���� �������� ������� �����

						INT8U sim2 = pProtocolOptions->CURRENT_SUMM(pUSART_Dlg->pPROTOCOL->ReceiveMessage, 1, char_count - 3);
						bool lError;	//����� � ����� �����, ���� �� ������
						if (sim != sim2)
						{
							error = _T("������ ���-�����");
							lError = true;
						}
						else
						{
							error = _T("��� ������");
							lError = false;
						}

						pUSART_Dlg->pPROTOCOL->AddInMessageCount();//�������� ����� �������� �������

						//���� �� ���� ������ � ������, �� ����������
						if (!lError)
						{//��������� ��������� � ��� ����� ����������
							if (!pProtocolOptions->OnVerify(&error, pUSART_Dlg->pPROTOCOL->ReceiveMessage))
							{
								return false;//��� ��������� ���������� ������
							}
						}
						else
						{
							//error="����� ������ �������"; //������ ����� �����
						}

						//������� ���� ��������� ���������
						lsReseivingMessage.length = char_count - 2;
						lsReseivingMessage.name = error;
						//������� � ����
						WriteMesFile(&lsReseivingMessage, UART_MSG_RECEIVED);

						char_count = 2; //�.�. 2 ������� ��� �������

						//���������� ��������� ������ �������
						pUSART_Dlg->pPROTOCOL->ReceiveMessage[0] = pProtocolOptions->var_FIRST_BYTE;

						if (pProtocolOptions->NotifyProgram != NULL)
						{//��������� ������� ��������� �� ������� ��� ��������� ������
							if (!lError)
							{//������ ���
								pProtocolOptions->NotifyProgram(NP_MESSAGE_RESEIVED);
							}
							else
							{//���� ������, �� ��� �� ������ ���������, � �� ������
								pProtocolOptions->NotifyProgram(NP_RECEIVE_PROTOCOL_ERROR);
							}
						}
					}
				}
				else
				{//��� ���� ��������
					char_count--;	//����� ������� ���-����� �� ����
					//������ �� ������������� �� ������� ���� ������
				}
			}

			if (pred_char != pProtocolOptions->var_FIRST_BYTE || ch != pProtocolOptions->var_STUF_BYTE)
			{//���� �������� ������ - �� ��������� � �� ������������
				//�� ������ ������ ��������� ������ � �����
				pUSART_Dlg->pPROTOCOL->ReceiveMessage[char_count] = 0;//����� �� ���� ������ ��� ����������� �� ������
				pUSART_Dlg->pPROTOCOL->ReceiveMessage[char_count - 1] = ch;
			}

			if (pUSART_Dlg->m_ReceiveMessage.m_hWnd != NULL)
			{//����� �� ���������� ������� � �������������� ����
				//���� ��������� ����������� ��� �����, �� ����� ����������� �������� ����������
				CString str = GiveStrFromHex(pUSART_Dlg->pPROTOCOL->ReceiveMessage, char_count);
				pUSART_Dlg->m_ReceiveMessage.SetWindowText(str);//������� ��������� 
			}

			if (char_count >= pProtocolOptions->var_MAX_LEN - 1)//������������ ������, � �� ���� ������� ����
			{//������ �������
				lsReseivingMessage.length = char_count;
				error = _T("������������ ��������� ������");
				lsReseivingMessage.name = error;
				//������� � ����
				WriteMesFile(&lsReseivingMessage, UART_MSG_RECEIVED);
				char_count = 0;
			}

			pUSART_Dlg->m_InMessageError.SetWindowText(error);//������� ������
		}
		else if (pProtocolOptions->var_Regime == EXTERN_FUNC)
		{//�������� ��������� ������� ������� �������
			if (pProtocolOptions->ExternReceiveChar == NULL)
			{
				m_pError->SetERROR(_T("��������� ExternReceiveChar ��� ��������� �������� ������ ����"), ERP_ERROR | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
				return false;
			}
			if (!pProtocolOptions->ExternReceiveChar(ch))
			{
				m_pError->SetERROR(_T("�������� ������� ExternReceiveChar"), ERP_ERROR | ER_WRITE_TO_LOG);
				return false;
			}
			return true;
		}
		else
		{//����������� �����
			m_pError->SetERROR(_T("������������ ����� ��������� ��������� �������"), ERP_ERROR | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
			return false;
		}

		pred_char = ch; //����� ����� ��� ���������� ������
		return true;
	}
	#endif
	//********************************************************************************************************************************
}




bool PROTOCOL_Struct::WriteMesFile(CMessage *pMessage, INT8U code)
		//� ����, ��� ��� ����, �� �������� ������ ���, ������ �� ������������ ���
		//������� ������ ���
{//���������� � ���� �������� � ������������ ���������
	/*
	in_out==true - ������������ ������������ ���������
	in_out==false - �������� ���������
	*/
	if(!(pProtocolOptions->o_LogInMessage || pProtocolOptions->o_LogOutMessage))
	{//������ � ���������� �� ����� ������ �� ������ ������������ ��� ����������� 
	//��������� � ������ �������
		return true;	
	}
	if(!InMesFileOpened)
	{//���� ���� ������� ��������� ��� �� ��� ������
		InMesFileOpened=true;	//�������� ����, ��� �� ������
		//������� ���� �������� ���������� ���������
		InMesFileOpen();
	}
	if(ReceiveMessageFile.is_open())
	{
		CStringA str;					//�.�. � ���� ���� ������� �� UNICODE!!!
		if(code==UART_MSG_TRANSEIVED)
		{//��� ������������ ���������
			if(pProtocolOptions->o_LogOutMessage) //���� ���������� �� ������
			{
				if(pProtocolOptions->o_LogTimeMessages)
				{//����� ������������� ����� ��������
					//������� ������� �����
					SYSTEMTIME time;
					GetLocalTime(&time);
					
					str.Format("\n%i:%02i %02is %ims  ",time.wHour,time.wMinute,time.wSecond,time.wMilliseconds);
					ReceiveMessageFile.write(str,str.GetLength());
				}
				str="��������� �����: \n";
				ReceiveMessageFile.write(str,str.GetLength());
				str.Format("%i)\t",OutMessageCount);
				ReceiveMessageFile.write(str,str.GetLength());
				ReceiveMessageFile.write("   ",3);
				//��� �����

				if(pProtocolOptions->o_HexDisplayOutMessage)//����� ����� ������ ����������������� �����?
				{//������� ����������������� ��������
					str=GiveStrFromHex(pMessage->text,pMessage->length);
				}
				else
				{//��������� ������
					str=pMessage->text;
				}

				ReceiveMessageFile.write(str,str.GetLength());
				ReceiveMessageFile.write("     ",5);
				//��������� ������
				USES_CONVERSION;
				ReceiveMessageFile.write(T2A(pMessage->name.GetBuffer()),pMessage->name.GetLength());

				ReceiveMessageFile<<endl;
				ReceiveMessageFile.flush();		//������ ������ �� ������ ����
			}
		}
		else if(code==UART_MSG_RECEIVED)
		{//��� �������� ���������
			if(pProtocolOptions->o_LogTimeMessages)
			{//����� ������������� ����� ���������
				//������� ������� �����
				SYSTEMTIME time;
				GetLocalTime(&time);
				
				str.Format("\n%i:%02i %02is %ims  ",time.wHour,time.wMinute,time.wSecond,time.wMilliseconds);
				ReceiveMessageFile.write(str,str.GetLength());
			}
			str="������ �����: \n";
			ReceiveMessageFile.write(str,str.GetLength());
			str.Format("%i)  \t",InMessageCount);
			ReceiveMessageFile.write(str,5);
			ReceiveMessageFile.write("   ",3);
			//��� �����

			if(pProtocolOptions->o_HexDisplayInMessage)//����� ����� ������ ����������������� �����?
			{//������� ����������������� ��������
				str=GiveStrFromHex(pMessage->text,pMessage->length);
			}
			else
			{//��������� ������
				str=pMessage->text;
			}

			ReceiveMessageFile.write(str,str.GetLength());
			ReceiveMessageFile.write("     ",5);
			//��������� ������
			USES_CONVERSION;
			ReceiveMessageFile.write(T2A(pMessage->name.GetBuffer()),pMessage->name.GetLength());

			ReceiveMessageFile<<endl;
			ReceiveMessageFile.flush();		//������ ������ �� ������ ����
		}
		else if(code==UART_MSG_NOTTRANCEIVED)
		{//��������� ������� ��������� ���������
			if(pProtocolOptions->o_LogTimeMessages)
			{//����� ������������� �����
				//������� ������� �����
				SYSTEMTIME time;
				GetLocalTime(&time);
				
				str.Format("\n%i:%02i %02is %ims  ",time.wHour,time.wMinute,time.wSecond,time.wMilliseconds);
				ReceiveMessageFile.write(str,str.GetLength());
			}
			str="�� ���������� ��������� ���������: \n";
			ReceiveMessageFile.write(str,str.GetLength());
			str.Format("%i)  \t",InMessageCount);
			ReceiveMessageFile.write(str,5);
			ReceiveMessageFile.write("   ",3);
			//��� �����

			if(pProtocolOptions->o_HexDisplayInMessage)//����� ����� ������ ����������������� �����?
			{//������� ����������������� ��������
				str=GiveStrFromHex(pMessage->text,pMessage->length);
			}
			else
			{//��������� ������
				str=pMessage->text;
			}

			ReceiveMessageFile.write(str,str.GetLength());
			ReceiveMessageFile.write("     ",5);
			//��������� ������
			USES_CONVERSION;
			ReceiveMessageFile.write(T2A(pMessage->name.GetBuffer()),pMessage->name.GetLength());

			ReceiveMessageFile<<endl;
			ReceiveMessageFile.flush();		//������ ������ �� ������ ����
		}
		else
		{//���������� ��� ���������
			return false;
		}
		
	}
	return true;	//��� ������ ���������
}	


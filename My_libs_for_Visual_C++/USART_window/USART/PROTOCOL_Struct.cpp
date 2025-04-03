/************************************************************************************************
*************************************************************************************************
**		���� ���� �������� ���������� ������ ������-�����������. ��� ������ ���, ��� ����� ��� �����,
**	�����������, ���������� ���������. �� �� ������������� ��������, �������� ������ � �.�. ������ ��� ������
**	��� ��� ��������, ��� � ����� ����������� ������������ � ������� �����.
***************************************************************************************************
***************************************************************************************************/

/**************************** ���� ����������� ��������� *******************************************************
*	11.09.2006 - ��������� ��������� � ����� ���������-������, ������� ��������� ������� "����� �� ����������" 
*			� ��������� � ��������������.
****************************************************************************************************************/

#include "stdafx.h"
#include "protocol_struct.h"
#include "USART_Dlg.h"

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
*	������� ���������� ������������ �������
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
	if(pProtocolOptions->OnVerify==NULL)	return false;
	if(ReceiveMessage==NULL)				return false;

	/*����� ��� ���� ���������� ����������*/
	static unsigned char	pred_char		=	0;						//���������� ������
	static unsigned short	char_count		=	0;						//���������� �������� ��������
	static CString error;
	static CMessage lsReseivingMessage(ReceiveMessage);
	
	char_count++;//������� ��� ���� ������
	
	/*if(pUSART_Dlg->m_hWnd==NULL || pUSART_Dlg->m_ReceiveMessage.m_hWnd==NULL)
	{
		this->m_pError->SetERROR(_T("���� ����� ���������, �� �������� �������������� � ������� ReceiveChar"),ERP_FATALERROR | ER_WRITE_TO_LOG);
		return false;
	}*/

	if(pProtocolOptions->var_Regime==PACKET_LEN)
	{
		/*
		�������� � ������������� ����� �����, ������ ��������� ����� ���� ����� �������,
		����� ������������� ����� ������ (�� �����).
		*/

		static bool verifi_on=0;			//���� 1, �� ����� ��������� � ������������
											//� ������ �����
		
		static bool FirstByteReady=false;	//��������, ��������� ��� �� ���������� (�� ����� ����
											//������, � � ����� �������� ����� ������ ������.
											//���� ���� �������� ����������� ��������� ������ ������
											//���������

		static unsigned short paket_length=0;	//����� ���������� ����� ������������� ����� ������
		static unsigned short input_count=0;	//���������� �������� ����
		
		unsigned char RC_register=ch;			//������ ���

		//��������� ����� ������ ������ �������� ��������� ������, �� �� ������������ � ����
		static bool Chepuha=true;		//����� ���������� ������ ��������� ��������� ��� �� ������ �� ���� ��������� ������
		static bool LenCorrect=false;	//�������� ���� ����� ����� ���������� ��������	
		static bool TheEnd=false;		//������ ��� ������� ����� ���������, ��� ����� ���������� �� ������
		static bool SumCorrect=false;

		if(input_count>pProtocolOptions->var_MAX_LEN)//������������ ������, � �� ���� ������� ����
		{//������ �������
			input_count=0;
		}

		if(TheEnd)
		{//� ������� ��� ��������� ��������� ���������, � � ���� �������� ������� � ���������� ������
			TheEnd=false;
			Chepuha=true;
		}

		//������� ��������� �������� ������
		ReceiveMessage[input_count]=RC_register;	//���������� � ����� ������
		input_count++;								//�������� ���������� �������� ��������

		//���� ����� �� �������, � ��� �� ����������� $ (�����, �����, ������)
		if(RC_register==pProtocolOptions->var_FIRST_BYTE && FirstByteReady==false)
		{
			input_count=0;			//������ ����� ��������� - �������� �������
			ReceiveMessage[0]=RC_register;	//������� ���� ������ ��� �� ���� �����
			input_count=1;			//������ ����� ��������� - �������� �������
			FirstByteReady=true;	//������ ������ ���� ��� �������

			Chepuha=false;		//��� ��� ����������� �����
			LenCorrect=false;	//���� ��� ������ �� ����� � �����
			SumCorrect=false;	//����������� ����� ��� �� �������
		}
		else if(Chepuha==false)
		{//����� �������
			if(input_count==pProtocolOptions->var_LEN_STATE+1)//��������� �������� ������
			{//����� ��� ���������, � �� ���������, � ���� ����� ����� ����� ������ � ������
				if(RC_register>pProtocolOptions->var_MAX_LEN-pProtocolOptions->var_LEN_ADD)
				{//���� ����� ������� ������� �������
					paket_length=pProtocolOptions->var_MAX_LEN;
					FirstByteReady=false;	//������ �������� ����� �������
				}
				else
				{
					//��������, ����� ���-�� �������� ��� ������� �������
					paket_length=RC_register+pProtocolOptions->var_LEN_ADD;

					LenCorrect=true;	//���� ����� ���� ���������
				}
			}
			else if(input_count==paket_length)
			{//��������� ������� �������, ���� ��������� ����������� �����
				if(LenCorrect)
				{//���� ����� ���� ���������, �� ����� ����������� � �������������������� �����
					unsigned char sim=ch;
					unsigned char sim2=pProtocolOptions->CURRENT_SUMM(ReceiveMessage,1,input_count-1);
					if(sim!=sim2)
					{//������ � ����������� �����
					}
					else
					{//����������� ����� �����
						SumCorrect=true;
					}
				}
				FirstByteReady=false;	//����� ������ ��������� ����� ���������

				TheEnd=true;		//�������� ���� ����� ���������
			}
		}

			
		//��������� ��, ��� ������� �� ������
		if(pProtocolOptions->o_HexDisplayInMessage)
		{
			////////////////////////////////////////////////////////////////////////////////////////////
			//	����� ������������, ��� ��� ����� - �������������� ��� ���������������
			////////////////////////////////////////////////////////////////////////////////////////
			if(!pProtocolOptions->OptimazeInMessageView)
			{//����� ���������� ������ �������� ������
				static CString hex_view=_T("");	//���� ����� ������������ ����������������� ��� ����������� �������
				if(input_count==1)
				{//��� ������ ������ �������, ������� �����
					hex_view="";
				}
							
				//������� ���������� � ����������� ���������
				CString AboutMessage;
				if(Chepuha)
				{
					AboutMessage=_T("������� ����...");
				}
				else if(FirstByteReady || TheEnd)	
				{//��������� ���� ��� �������, ���� ��������� ������
					AboutMessage=_T("������ ����� ���������");
				}

				if(input_count>pProtocolOptions->var_LEN_STATE)
				{
					if(LenCorrect)
					{
						AboutMessage.Append(_T("; ���� ����� ���������"));
					}
					else
					{	
						if(!Chepuha)
						{//���� ������� ����, �� ������ � ���������� ��������������� ���������
							AboutMessage.Append(_T("; �������� ���� �����"));
						}
					}
					if(TheEnd==true)
					{//������� ��� ��������� (�� ��� ����������� ����� �������)
						if(SumCorrect)
						{
							if(pProtocolOptions->OnVerify!=NULL)
							{
								pProtocolOptions->OnVerify(&AboutMessage,ReceiveMessage);
							}
						}
						else
						{
							AboutMessage.Append(_T("; ����������� ����� �����������"));
						}
						//�������� ����� �������� �������
						AddInMessageCount();
						//������� ���� ��������� ���������
						lsReseivingMessage.length=input_count;
						lsReseivingMessage.name=AboutMessage;
						//������� � ����
						WriteMesFile(&lsReseivingMessage,UART_MSG_RECEIVED);
					}
				}
				//������� ���������� � �������� ���������
				pUSART_Dlg->m_InMessageError.SetWindowText(AboutMessage);

				//������� ����������������� ��� ���������� �������
				hex_view.Append(GiveStrFromHex(&ch,1));	
				
				//������� �� ������ ���� �������� ���������
				pUSART_Dlg->m_ReceiveMessage.SetWindowText(hex_view);

			}
			else
			{//������ ��� ��� ����� ��������������, ��������� ������ ��������� �������� ���������
				if(TheEnd==true)
				{//������ ��� ��������� ��������� ���������
					//��������� � ������������� ���
					CString str=GiveStrFromHex(ReceiveMessage,input_count);

					CString AboutMessage;
					if(!Chepuha && LenCorrect && SumCorrect)
					{//���� ��� ���� ���������, �� ����� ��� � �����������������
						if(pProtocolOptions->OnVerify!=NULL)
						{
							pProtocolOptions->OnVerify(&AboutMessage,ReceiveMessage);
						}
					}
					else if(!LenCorrect) AboutMessage=_T("������������ ���� �����");
					else if(!SumCorrect) AboutMessage=_T("�������� ����������� �����");

					pUSART_Dlg->m_ReceiveMessage.SetWindowText(str);//������� ��������� 
					//������� ���������� � ����������� ���������
					pUSART_Dlg->m_InMessageError.SetWindowText(AboutMessage);
					//�������� ����� �������� �������
					AddInMessageCount();
					//������� ���� ��������� ���������
					lsReseivingMessage.length=input_count;
					lsReseivingMessage.name=AboutMessage;
					//������� � ����
					WriteMesFile(&lsReseivingMessage,UART_MSG_RECEIVED);
				}
			}
		}
		else
		{//������� ��������� ���, ��� ��� ����
			USES_CONVERSION;
			pUSART_Dlg->m_ReceiveMessage.SetWindowText(A2T((char*)ReceiveMessage));
		}

		if(TheEnd && pProtocolOptions->NotifyProgram!=NULL)
		{//��������� ������� ��������� �� ������� ��� ��������� ������
			if(!Chepuha && LenCorrect && SumCorrect)
			{//������ ���
				pProtocolOptions->NotifyProgram(NP_MESSAGE_RESEIVED);
			}
			else
			{//���� ������, �� ��� �� ������ ���������, � �� ������
				pProtocolOptions->NotifyProgram(NP_RECEIVE_PROTOCOL_ERROR);
			}
		}
	}
	else if (pProtocolOptions->var_Regime==BYTE_STUFFING)
	{//����� ������-�������� � ��������������

		/*
		������� �������� � ������������ ����-����������. ���������� ����������� ���� ������������� �� ����� ������,
		� ������ ����� ������ ������������� ������ �� ������ ����������. �������������, ������ ���������� ��������
		� ���� ����� (��� �����)
		*/

		static CString	error		=	_T("������ ���������");		//������������ ������ � ������� ������
		static bool		staf_flag	=	0;							//����� ����������� ���� ��������


		if(pred_char==pProtocolOptions->var_FIRST_BYTE)
		{
			if(ch!=pProtocolOptions->var_STUF_BYTE)
			{//����� ���������
				if(char_count>=3)
				{//��� ���� ��� �� ������ ���������, � ��� ����� ������� ��������� ����������
					//����� �� ��������� ������������ ���-����� ����������� ������, �.�. � ���� ������ ��������� ����
					INT8U sim=pUSART_Dlg->pPROTOCOL->ReceiveMessage[char_count-3]; //-3, �.�. ������������ ����� ���� �������� ������� �����
				
					INT8U sim2=pProtocolOptions->CURRENT_SUMM(pUSART_Dlg->pPROTOCOL->ReceiveMessage,1,char_count-3);
					bool lError;	//����� � ����� �����, ���� �� ������
					if(sim!=sim2)
					{
						error=_T("������ ���-�����");
						lError=true;
					}
					else
					{
						error=_T("��� ������");
						lError=false;
					}

					pUSART_Dlg->pPROTOCOL->AddInMessageCount();//�������� ����� �������� �������
				
					//���� �� ���� ������ � ������, �� ����������
					if(!lError)
					{//��������� ��������� � ��� ����� ����������
						if(!pProtocolOptions->OnVerify(&error, pUSART_Dlg->pPROTOCOL->ReceiveMessage))
						{
							return false;//��� ��������� ���������� ������
						}
					}
					else
					{
						//error="����� ������ �������"; //������ ����� �����
					}

					//������� ���� ��������� ���������
					lsReseivingMessage.length=char_count-2;
					lsReseivingMessage.name=error;
					//������� � ����
					WriteMesFile(&lsReseivingMessage,UART_MSG_RECEIVED);
					
					char_count=2; //�.�. 2 ������� ��� �������
					
					//���������� ��������� ������ �������
					pUSART_Dlg->pPROTOCOL->ReceiveMessage[0]=pProtocolOptions->var_FIRST_BYTE;

					if(pProtocolOptions->NotifyProgram!=NULL)
					{//��������� ������� ��������� �� ������� ��� ��������� ������
						if(!lError)
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

		if(pred_char!=pProtocolOptions->var_FIRST_BYTE || ch!=pProtocolOptions->var_STUF_BYTE)
		{//���� �������� ������ - �� ��������� � �� ������������
			//�� ������ ������ ��������� ������ � �����
			pUSART_Dlg->pPROTOCOL->ReceiveMessage[char_count]=0;//����� �� ���� ������ ��� ����������� �� ������
			pUSART_Dlg->pPROTOCOL->ReceiveMessage[char_count-1]=ch;
		}

		if(pUSART_Dlg->m_ReceiveMessage.m_hWnd!=NULL)
		{//����� �� ���������� ������� � �������������� ����
			//���� ��������� ����������� ��� �����, �� ����� ����������� �������� ����������
			CString str=GiveStrFromHex(pUSART_Dlg->pPROTOCOL->ReceiveMessage,char_count);
			pUSART_Dlg->m_ReceiveMessage.SetWindowText(str);//������� ��������� 
		}

		if(char_count>=pProtocolOptions->var_MAX_LEN-1)//������������ ������, � �� ���� ������� ����
		{//������ �������
			lsReseivingMessage.length=char_count;
			error=_T("������������ ��������� ������");
			lsReseivingMessage.name=error;
			//������� � ����
			WriteMesFile(&lsReseivingMessage,UART_MSG_RECEIVED);
			char_count=0;
		}

		pUSART_Dlg->m_InMessageError.SetWindowText(error);//������� ������
	}
	else if(pProtocolOptions->var_Regime==EXTERN_FUNC)
	{//�������� ��������� ������� ������� �������
		if(pProtocolOptions->ExternReceiveChar==NULL)
		{
			m_pError->SetERROR(_T("��������� ExternReceiveChar ��� ��������� �������� ������ ����"), ERP_ERROR | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
			return false;
		}
		if(!pProtocolOptions->ExternReceiveChar(ch))
		{
			m_pError->SetERROR(_T("�������� ������� ExternReceiveChar"), ERP_ERROR | ER_WRITE_TO_LOG);
			return false;
		}
		return true;
	}
	else 
	{//���������� �����
		m_pError->SetERROR(_T("������������ ����� ��������� ���������� �������"), ERP_ERROR | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
		return false;
	}
	
	pred_char=ch; //����� ����� ��� ���������� ������
	return true;
}

PROTOCOL_Struct::~PROTOCOL_Struct(void)
{
	if(ReceiveMessage!=NULL)
	{//������ ������-����� ���������
		delete [] ReceiveMessage;
	}
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


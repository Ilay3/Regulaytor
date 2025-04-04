#include "stdafx.h"
#include <Lobanov Data\LobanovData.h>		//���� ������ ��
#include <Hex_and_String\Hex_and_String.h>	//������� ����������������� ��������


#include "My_functions.h"
#include "VERSION_4Dlg.h"		//����� �������� ���� ���������


//� ��� ��������� �� ������� ���� ���������
CVERSION_4Dlg* pDlg;

/************************************************************************************************
*		�.�. ���� ������� ������ ����� ������ � �������� ������, �� �������� ���������� 
*	(������ ��� ����� �����) ���������, �� �������� � ����� ����������� ���������
************************************************************************************************/
void InitMyFunctions(CVERSION_4Dlg* pdlg)
{
	ASSERT(pdlg);
	pDlg=pdlg;
}

/*****************************	real_CompileMessage_?	*********************************************
**		��� ������� ������� �����, ������� � �������� � ���������� ��� � NextMessage?.text
**	, ������� �������� ���� �����, ����������� �����, �����, ����� �������� � ������.
***************************************************************************************************/
bool CompileRequestMessage(CMessage& NextMessage) //�������� NextMessage_1 �� �������� ������
{
	ASSERT(pDlg);
	
	/*********************************************************************************
	*		����� ��� �.�. ����-��
	*	� ����� � ���, ��� �������� � �������� �������, ���������� ������� ��������� ����
	*	��������� ���������� �������. ������ - � 7 � ���� ����� �����. ������ - � 8 � ����
	*	����� ������. ����� ������� ���� ����� �� ����� ������ ������� ����������, �.�. ��� ����
	*	��� 6 ����, ��� � ��������
	**********************************************************************************/
	static unsigned char message[255];
	unsigned char i=0; 

	//����� ����� ���������
	message[i++]=pDlg->ProtocolOptions.var_FIRST_BYTE;		//������ ������ �������
	
	//���� ����� ������
#ifndef MSG_TX_OLD_TYPE
	if (pDlg->ProtocolOptions.o_MsgType == 0)
		message[i++]=7;		//������ ������������� ������
	else
		message[i++]=8;		//���� ������������� ������
#else
	static bool pac=false;

	if (pac == false)
		message[i++]=7;		//������ ������������� ������
	else
		message[i++]=8;		//���� ������������� ������

	pac=!pac;
#endif

	message[i++]=pDlg->ProtocolOptions.var_MY_ID;	//������� ������ ����� ������ 
	//����� ������
	//������� ��������
	/*//++++*/ int_to_SV2BUINT_ST((INT16U)pDlg->GLAVNIY_CLASS.var_UST_Freq_Rot,&message[i]);
	/*������������ �� �������� ������ �����
	static INT16U temp=0;
	int_to_SV2BUINT_ST((INT16U)temp++,&message[i]);*/
	i++; i++;
	
	if(pDlg->GLAVNIY_CLASS.var_UST_OnLoad)
	{//��������
		if (pDlg->GLAVNIY_CLASS.var_UST_Bux)
			message[i++]=0x7C;
		else
			message[i++]=0xCC;
	}
	else
	{//�� ��������
		message[i++]=0x00;
	}
	
	if(pDlg->GLAVNIY_CLASS.var_UST_Run_Stop)
	{//����
		message[i++]=0x01;
	}
	else
	{//����
		message[i++]=0x00;
	}

	// ���������� � ��������� -  ���������� �����!!!
	//�����
	int_to_SV2BUINT_ST((INT16U)(pDlg->GLAVNIY_CLASS.var_UST_Reyka * 100.0f), &message[i]);
	i++; i++;

	//����� ����������� �����
	char sum=pDlg->ProtocolOptions.CURRENT_SUMM(message,1,i); //������ ���� � ����� �� ������
	if( ((CButton *)pDlg->Dlg4.GetDlgItem(IDC_SUM_ER))->GetCheck() == BST_CHECKED )
	{
		message[i]=0xbb;	//�� ���������, �� ���� ���������
	}
	else
	{
		message[i]=sum;
	}
	message[i+1]=0x00;					//�� ������ ������
	NextMessage.text=(unsigned char*) message;
	NextMessage.summa=sum;
	NextMessage.nomer=0;
	NextMessage.priem=30;
	NextMessage.length=i+1;
	NextMessage.name="����� �� ����������� ���������";

	return true;//������������ ���������
}

bool CompileAnswerMessage(CMessage& NextMessage)
{
	ASSERT(pDlg);
	
	/*******************************************************************
	*	��� ��, ��� ��� �������� ��������� � ����� �� ��� ������.
	*	� ���� ��� ��������� ��� ���������� �����.
	********************************************************************/
	static INT8U message[25];
	INT8U i=0; 

	//����� ����� ���������
	message[i++]=pDlg->ProtocolOptions.var_FIRST_BYTE;		//������ ������ �������
	message[i++]=0x14;										//����� ��� ������
	message[i++]=0x10;										//� ����� ����

	//����� ������
	//������� �������� ������� ����
	int_to_SV2BUINT_ST(rand()%1500, &message[i]);
	i+=2;
	//��������� �����, ���������� �� 100 (���)
	int_to_SV2BUINT_ST(rand()%500, &message[i]);
	i+=2;
	//����������� ����� (����)
	int_to_SV2BUINT_ST(rand()%300, &message[i]);
	i+=2;
	//����������� �� �������
	message[i++]=0x00;
	//������� �������
	int_to_SV2BUINT_ST(rand()%2500, &message[i]);
	i+=2;
	//�������� �������
	int_to_SV2BUINT_ST(rand()%500, &message[i]);
	i+=2;
	//������� ����
	message[i++]=0x00;
	//������ �� ���������� ��������
	message[i++]=0x00;
	//������ �� �����
	message[i++]=0x00;
	//�������� �����
	int_to_SV2BUINT_ST(rand()%500, &message[i]);
	i+=2;
	//������
	message[i++]=0x00;
	message[i++]=0x00;

	//����� ����������� �����
	INT8U sum=pDlg->ProtocolOptions.CURRENT_SUMM(message,1,i); //������ ���� � ����� �� ������
	message[i]=sum;
	message[i+1]=0x00;					//�� ������ ������
	NextMessage.text=(INT8U*) message;
	NextMessage.summa=sum;
	NextMessage.nomer=0;
	NextMessage.priem=30;
	NextMessage.length=i+1;
	NextMessage.name="����� ����������";

	return true;//������������ ���������
}

/************************ OnVerify(CString * err) *******************************
**		��������� �������� ��������� � ���������� ��� (����, �����, � ����� �����������)
**	������� ���������� ������ ������ �� �������� � � err �������� � ����������� �������
**	��� ������� ������������� � ����� ������� (������� � �������� � ������� ���������
**	��� ��������� �� ������), ��� ������� ���� � ����� �������.
*******************************************************************************************/
bool OnVerify(CString * err, unsigned char *ReceiveMessage)
{
	ASSERT(err);
	ASSERT(ReceiveMessage);

	*err="��� ������ � �������������";
	unsigned char tmp;

#ifndef MSG_TX_OLD_TYPE
	if (    ReceiveMessage[2]==pDlg->ProtocolOptions.var_MY_ID
		&& (   (ReceiveMessage[1]==0x13 && pDlg->ProtocolOptions.o_MsgType == 0)
		    || (ReceiveMessage[1]==0x14 && pDlg->ProtocolOptions.o_MsgType == 1)))
#else
	if (   ReceiveMessage[2]==pDlg->ProtocolOptions.var_MY_ID
		&& ReceiveMessage[1]==0x14)
#endif
	{
	  //*err = "����-�� ������� ����������... ";
		*err = "��������� ������� ����������... ";
		INT16U var1=SV2BUINT_to_int_ST(&ReceiveMessage[3]);
		INT16U var2=SV2BUINT_to_int_ST(&ReceiveMessage[5]);
		INT16U var3=SV2BUINT_to_int_ST(&ReceiveMessage[7]);
		INT16U var4=SV2BUINT_to_int_ST(&ReceiveMessage[10]);
		INT16U var5=SV2BUINT_to_int_ST(&ReceiveMessage[12]);
		INT16U var6=SV2BUINT_to_int_ST(&ReceiveMessage[17]);


		pDlg->GLAVNIY_CLASS.Add_Freq_Rot		(false,var1);
		pDlg->GLAVNIY_CLASS.Add_Reyka			(false,var2/100.f);
		pDlg->GLAVNIY_CLASS.Add_Oil_Temp		(false,var3);
		pDlg->GLAVNIY_CLASS.Add_Freq_Turbine	(false,var4*10.f);
		pDlg->GLAVNIY_CLASS.Add_Nadduv_Pressure	(false,var5/128.f);
		pDlg->GLAVNIY_CLASS.Add_Oil_Pressure	(false,var6/100.f);

		//����������� �� �������
		pDlg->GLAVNIY_CLASS.Add_Nadduv_Limit(ReceiveMessage[9]);
		
		tmp=ReceiveMessage[14];
		if(tmp==0x00)
		{//����
			pDlg->GLAVNIY_CLASS.Add_Run_Stop(false);
		}
		else if(tmp==0xfd)
		{//����
			pDlg->GLAVNIY_CLASS.Add_Run_Stop(true);
		}
		else
		{
			*err = *err + _T("������������ ������ � ���� �������� ����");
		}

		tmp=ReceiveMessage[15];
		if(tmp==0xFF)
		{//������
			pDlg->GLAVNIY_CLASS.Add_Raznos(false,true);
		}
		else if(tmp==0x00)
		{//��� �������
			pDlg->GLAVNIY_CLASS.Add_Raznos(false,false);
		}
		
		//������������ � ������ �� �����
		pDlg->GLAVNIY_CLASS.Add_Oil_Prot(ReceiveMessage[16]);

		pDlg->GLAVNIY_CLASS.m_ReceiveControl=0;	//������� ������� ������������ ��������
	}
	else if (ReceiveMessage[2]==pDlg->ProtocolOptions.var_MY_ID && (ReceiveMessage[1]==0x08 || ReceiveMessage[1]==0x07))
	{
		//������ ������� ������
		pDlg->LINK_FORMATERROR();
		*err = _T("����� ��������� �����������");
	}
	else
	{
		//������ ������� ������
		pDlg->LINK_FORMATERROR();
		*err = _T("������. ��������� ������� ��������� ���");
	}


	return true;	//�� ������ ���������
}

/******************************   NewReceiveChar(unsigned char ch)   ********************
*		������� �������� ������������ ����� � ������ ����������� ������ ������ ���������
****************************************************************************************/
//bool NewReceiveChar(unsigned char ch)
//{
//	/*����� ��� ���� ���������� ����������*/
//	static unsigned char	pred_char		=	0;						//���������� ������
//	static unsigned char	char_count		=	0;						//���������� �������� ��������
//	
//	
//	char_count++;//������� ��� ���� ������
//	
//	/*
//	������� �������� � ������������ ����-����������. ���������� ����������� ���� ������������� �� ����� ������,
//	� ������ ����� ������ ������������� ������ �� ������ ����������. �������������, ������ ���������� ��������
//	� ���� ����� (��� �����)
//	*/
//
//	static CString	error		=	"������ ���������";  //������������ ������ � ������� ������
//	static bool		staf_flag	=	0;						//����� ����������� ���� ��������
//
//
//	if(pred_char==pDlg->ProtocolOptions.var_FIRST_BYTE)
//	{
//		if(ch!=pDlg->ProtocolOptions.var_STUF_BYTE)
//		{//����� ���������
//			
//			//����� �� ��������� ������������ ���-����� ����������� ������, �.�. � ���� ������ ��������� ����
//			char sim=pDlg->pUSART_Dlg->pPROTOCOL->ReceiveMessage[char_count-3]; //-3, �.�. ������������ ����� ���� �������� ������� �����
//		
//			char sim2=(char)pDlg->ProtocolOptions.CURRENT_SUMM(pDlg->pUSART_Dlg->pPROTOCOL->ReceiveMessage,1,char_count-3);
//			if(sim!=sim2)
//			{
//				error="������ ���-�����";
//			}
//			else
//			{
//				error="��� ������";
//			}
//
//			pDlg->pUSART_Dlg->pPROTOCOL->AddInMessageCount();//�������� ����� �������� �������
//			//���� �� ���� ������ � ������, �� ����������
//
//			if(error=="��� ������")
//			{//��������� ��������� � ��� ����� ����������
//				if(!OnVerify(&error, pDlg->pUSART_Dlg->pPROTOCOL->ReceiveMessage))
//				{
//					return false;//��� ��������� ���������� ������
//				}
//			}
//			else
//			{
//				//error="����� ������ �������"; //������ ����� �����
//			}
//
//			//������� � ���� ���������� ���������
//			pDlg->pUSART_Dlg->pPROTOCOL->WriteInMesFile(pDlg->pUSART_Dlg->pPROTOCOL->ReceiveMessage,char_count-2,error,false);
//			
//			char_count=2; //�.�. 2 ������� ��� �������
//			
//			pDlg->pUSART_Dlg->pPROTOCOL->ReceiveMessage[0]=pDlg->ProtocolOptions.var_FIRST_BYTE;		//|��������� ���������
//		}
//		else
//		{//��� ���� ��������
//			char_count--;	//����� ������� ���-����� �� ����
//			//������ �� ������������� �� ������� ���� ������
//		}
//	}
//
//	if(!(pred_char==pDlg->ProtocolOptions.var_FIRST_BYTE && ch==pDlg->ProtocolOptions.var_STUF_BYTE))
//	{//���� �������� ������ - �� ������������
//		//�� ��� ������ ������ ���������
//		pDlg->pUSART_Dlg->pPROTOCOL->ReceiveMessage[char_count]=0;//����� �� ���� ������ ��� ����������� �� ������
//		pDlg->pUSART_Dlg->pPROTOCOL->ReceiveMessage[char_count-1]=ch;
//	}
//
//	if(pDlg->pUSART_Dlg->m_ReceiveMessage.m_hWnd!=NULL)
//	{//����� �� ���������� ������� � �������������� ����
//		//���� ��������� ����������� ��� �����, �� ����� ����������� �������� ����������
//		CString str=GiveStrFromHex(pDlg->pUSART_Dlg->pPROTOCOL->ReceiveMessage,char_count);
//		pDlg->pUSART_Dlg->m_ReceiveMessage.SetWindowText(str);//������� ��������� 
//	}
//
//	pDlg->pUSART_Dlg->m_InMessageError.SetWindowText(error);//������� ������
//	pred_char=ch; //����� ����� ��� ���������� ������
//	return true;
//}

/*******************   BadMessageReceived(void)  ************************************
*		��� ������� ����� ����������, ����� ��������� ��������� ������� ���������
*	� ��������, ������� ���������� �� ������. ��� ���� flags ����� �������� �� ��� 
*	�������.
	NP_MESSAGE_TRANSFERED		//�����-�� ��������� ����������
	NP_RECEIVE_PROTOCOL_ERROR	//������ ��� ������ �� ������ ��������� (�������� � ����������� �����)
	NP_MESSAGE_RESEIVED			//�����-�� ��������� ������� (���������� ������������ � OnVerify)
	NP_TRANSFER_ERROR			//������ ��� �������� �� ������ ��������
*************************************************************************************/
void NotifyProgram(INT32U flags)
{
	switch (flags)
	{
	case NP_MESSAGE_TRANSFERED: 
		break;
	case NP_RECEIVE_PROTOCOL_ERROR:
		pDlg->LINK_FORMATERROR();
		break;
	case NP_MESSAGE_RESEIVED:
		break;
	case NP_TRANSFER_ERROR:
		pDlg->LINK_PORTERRROR();
		break;
	}
}
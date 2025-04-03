#pragma once
#include <ERRORS\ERRORS.h>		//������ ����������� ������
#include <Types_Visual_c++.h>	//��� ���� ������
#include <vector>
using namespace std;


typedef struct _NONLINEAR_STRUCT {
	float fY;
	float fX;
	float fh;
	float fgammai;
} NONLINEAR_STRUCT;

typedef struct _EGU_STRUCT {
	float fY;
	float fX;
} EGU_STRUCT;


class CVERSION_4Dlg;//���������� ������ ������������� ����

class GLAVNIY_CLASS_Struct
{
	/*
	���������� ���������� �������� ������������� �����������, �.�. ��� ����� ������������ ������������ �� ������
	����������� Add...
	*/

public:
	//	����������� - ����������
	GLAVNIY_CLASS_Struct();
	~GLAVNIY_CLASS_Struct();

	//	������� �������������
	bool InitGLAVNIY_CLASS(CVERSION_4Dlg * main_window, ERROR_Class *pError);//������ ���������� ����� ���������� ����������

	//����� �������� ����
	CVERSION_4Dlg *MAIN_WINDOW;
	
	//��� ������������ ������ ������� ����
	ERROR_Class *m_pError;		
		
	/**  �������  **/
	FP32	var_UST_Freq_Rot;		//������� ������� ��������
	
	bool	var_UST_OnLoad;			//����� ������ 
									//false - �������� ���
									//true - ��� ���������
	
	bool	var_UST_Run_Stop;		//false - ������ ����������
									//true - ������ �������

	// ��������� 29.11.2010
	// ������� ��������� � ��������� ��������� �����. ����� �������� � ���� �����.
	// ����������� �������� ��������������, � ���� ������������ ��� ������ � ���� "����������� ��������"

	FP32	var_UST_Reyka;			// ������� ��������� �����
	bool	var_EXT_Protocol;		// ����������� ��������� � ����������� ���� ����� �����

	bool	var_UST_Bux;			// ����������

	/**  ��������� ��������� �������  **/
	FP32	var_Freq_Rot;			//�������� ������� ��������
	FP32	var_Reyka;				//��������� ����� [��]
	FP32	var_Oil_Temp;			//����������� ����� [�]
	
	unsigned char	var_Nadduv_Limit;//0x00 - ��� ����������� �� �������
									//0xFE - ����
									//0xEE - ����� �� ����� ������ �������� �������

	FP32	var_Freq_Turbine;		//������� �������
	FP32	var_Nadduv_Pressure;	//�������� �������

	bool	var_Run_Stop;			//true - ������ ��������
									//false - ������ ����������

	bool	var_Raznos;				//true - ���� ������
									//false - ��� �������

	INT8U	var_Oil_Prot;			//������ �� �����
									//0�00 - ��� ������
									//0�40 - ������������
									//0��0 - ������

	FP32 var_Oil_Pressure;		//�������� �����




	//������� ������ ������� ��������, ������������� � ��� �������� ��� ������
	// dir = true - �������� �������� � ���� ���������
	// dir = false - ���-�� ��� � � ���� ����� ��������
	//���������� 
	// true - ������� ���������� �������
	// false - ��� ��������� ���� ������, ��� �������� � ERRORS
	bool Add_UST_Freq_Rot(bool dir, FP32 temp=-1); 
	bool Add_UST_ONLOAD(bool OnNagr);
	bool Add_UST_Run_Stop(bool Run);

	bool Add_Freq_Rot		(bool dir, FP32 temp=-1); 
	bool Add_Reyka			(bool dir, FP32 temp=-1); 
	bool Add_Oil_Temp		(bool dir, FP32 temp=-1); 
	bool Add_Freq_Turbine	(bool dir, FP32 temp=-1); 
	bool Add_Nadduv_Pressure(bool dir, FP32 temp=-1); 
	bool Add_Oil_Prot		(INT8U temp=-1); 
	bool Add_Oil_Pressure	(bool dir, FP32 temp=-1); 
	bool Add_Nadduv_Limit	(INT8U temp=-1);
	bool Add_Run_Stop		(bool Run);
	bool Add_Raznos			(bool dir, bool temp=false);

	/******	 �������������� ���������   **************************/
	INT8U m_ReceiveControl;
	const static INT8U RECEIVE_CONTROL_THRESHOLD=3;	//���� ���������� ��� ���������� �����������
													//��������, �� ����� ��������� ����������
	UINT period_of_transfer;	//������ � ��, � ������� ��������� ����� 
								//������������ �� ����_��
	const static UINT_PTR PERIOD_OF_TRANSFER_ID=0x00;	//������������� ����� �������

private:
#define GR_NOTINITIALIZED	0x00	//������ �� ����������������
#define GR_ENABLED			0x01	//���������� ������� ��������
#define GR_DISABLED			0x02	//� ������ ������ �� �����������
	INT8U m_GraphRegime;			//����� ������ �������
public:
	bool SetGraphRegime(INT8U regime);
	INT8U GetGraphRegime();
	

	// ������ ������������
	CString strNumberSensor;

	typedef vector<NONLINEAR_STRUCT, allocator<NONLINEAR_STRUCT>> VECTOR_NONLINEAR;

	VECTOR_NONLINEAR m_VectNonLinear;

	float fXStart;
	float fdX;
	float fA, fB;
	float fhmax;
	float fhmaxp;
	float fhmaxm;
	float fgamma;

	CString time_date;
	void  CalcLinear(void);






	typedef vector<EGU_STRUCT, allocator<EGU_STRUCT>> VECTOR_EGU;
	VECTOR_EGU m_VectEGU;

	float fdXe;	// ��� �� � ��� ���
	float fYw; // ���������� ������� ���
	float fK;  // �������� ����, ��/���
	float fH;  // "0" ����� ��������, ��
	float fL;  // ���������� �� ����� ������ �� ����� �������
	int   Fix_w;	 // ������ ���������� ����

	void  CalcEGU(void);

};
	
#pragma once
#include "stdafx.h"
#include "RealTimer.h"

UINT RealTimer_CLASS::GetMinPeriod()
{//выдает минимаьно-возможное разрешение (точность)
	TIMECAPS tm;
	timeGetDevCaps(&tm,sizeof(tm));
	return tm.wPeriodMin;
}

bool RealTimer_CLASS::SetTimerCallback(DWORD period, UINT precision, bool Periodic, LPTIMECALLBACK pFunction, DWORD MyData) 
{ //выставляем таймер
	DWORD tmp=Periodic==true?TIME_PERIODIC:TIME_ONESHOT;
	ID = timeSetEvent(period, precision, pFunction,MyData, tmp);                
	if(! ID)
	{
		AfxMessageBox(_T("Не могу настроить точный таймер"));
	}

	return true;
}

RealTimer_CLASS::~RealTimer_CLASS()
{
	CancelTimer();
}

void RealTimer_CLASS::CancelTimer(void)
{//остановим таймер
	if(ID) 
	{                // is timer event pending?
		timeKillEvent(ID);  // cancel the event
		ID = 0;
	}
}
#include "stdafx.h"

#include "HighResolutionTimer.h"


CHighResolutionTimer::CHighResolutionTimer()
{
	m_nIDTimer = NULL;
	m_uDelay = 0;
	m_pfnCallback = 0;
}


CHighResolutionTimer::~CHighResolutionTimer()
{
	KillTimer();
}

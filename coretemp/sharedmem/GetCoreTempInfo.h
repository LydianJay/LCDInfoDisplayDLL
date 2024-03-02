#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>


#define UNKNOWN_EXCEPTION 0x20000000

typedef struct core_temp_shared_data
{
	unsigned int	uiLoad[256];
	unsigned int	uiTjMax[128];
	unsigned int	uiCoreCnt;
	unsigned int	uiCPUCnt;
	float			fTemp[256];
	float			fVID;
	float			fCPUSpeed;
	float			fFSBSpeed;
	float			fMultipier;	
	char			sCPUName[100];
	unsigned char	ucFahrenheit;
	unsigned char	ucDeltaToTjMax;
}CORE_TEMP_SHARED_DATA,*PCORE_TEMP_SHARED_DATA,**PPCORE_TEMP_SHARED_DATA;
extern bool (WINAPI* fnGetCoreTempInfoAlt)(CORE_TEMP_SHARED_DATA* pData);
bool initCoreTemp();

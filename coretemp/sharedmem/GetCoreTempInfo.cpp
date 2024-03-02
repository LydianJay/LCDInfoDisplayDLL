#include "GetCoreTempInfo.h"

bool (WINAPI* fnGetCoreTempInfoAlt)(CORE_TEMP_SHARED_DATA* pData) = nullptr;



bool initCoreTemp() {
	auto hndl = LoadLibrary(L"coretemp/sharedmem/x64/GetCoreTempInfo.dll");
	if (hndl == NULL)
		return false;

	fnGetCoreTempInfoAlt = (bool(WINAPI*)(CORE_TEMP_SHARED_DATA*))GetProcAddress(hndl, "fnGetCoreTempInfoAlt");
	if (fnGetCoreTempInfoAlt == nullptr)
		return false;


	return true;
}

#include <Windows.h>
#include "helper.h"

typedef struct _HELPER_PARAM {
	EXECUTION_STATE state;
	HANDLE waitobj;
} HELPER_PARAM, * PHELPER_PARAM;

HANDLE HelperExitEvent = NULL;
HANDLE hThread = NULL;
CRITICAL_SECTION helper_cs;

DWORD WINAPI HelperThread(LPVOID lp);

VOID InitHelper()
{
	InitializeCriticalSection(&helper_cs);
	HelperExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

BOOL GetHelperStatus()
{
	BOOL result;
	EnterCriticalSection(&helper_cs);

	result = hThread?TRUE:FALSE;

	LeaveCriticalSection(&helper_cs);
	return result;
}

VOID StartHelper(BOOL screen)
{
	EnterCriticalSection(&helper_cs);

	if (hThread || (!HelperExitEvent))
	{
		LeaveCriticalSection(&helper_cs);
		return;
	}

	PHELPER_PARAM param = (PHELPER_PARAM)GlobalAlloc(GMEM_FIXED, sizeof(HELPER_PARAM));
	if (param)
	{
		param->state = ES_SYSTEM_REQUIRED | ES_CONTINUOUS;
		param->waitobj = HelperExitEvent;
		if (screen)
			param->state |= ES_DISPLAY_REQUIRED;
		ResetEvent(HelperExitEvent);
		hThread = CreateThread(NULL, 0, HelperThread, param, 0, NULL);
	}

	LeaveCriticalSection(&helper_cs);
}

VOID StopHelper()
{
	EnterCriticalSection(&helper_cs);

	if (hThread)
	{
		SetEvent(HelperExitEvent);
		WaitForSingleObject(hThread, INFINITE);
		hThread = NULL;
	}

	LeaveCriticalSection(&helper_cs);
}

DWORD WINAPI HelperThread(LPVOID lp)
{
	PHELPER_PARAM param = (PHELPER_PARAM)lp;
	HANDLE obj = param->waitobj;
	SetThreadExecutionState(param->state);
	GlobalFree(lp);
	WaitForSingleObject(obj, INFINITE);
	return 0;
}

VOID DestroyHelper()
{
	if (GetHelperStatus())
		StopHelper();

	CloseHandle(HelperExitEvent);
	HelperExitEvent = NULL;
	DeleteCriticalSection(&helper_cs);
}
#pragma once
#include <Windows.h>

VOID InitHelper();
BOOL GetHelperStatus();
VOID StartHelper(BOOL screen);
VOID StopHelper();
VOID DestroyHelper();
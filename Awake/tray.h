#pragma once
#include <Windows.h>

#define WM_TRAY (WM_USER + 1)

BOOL InitTray(HINSTANCE hInstance, HWND hWnd);
VOID DestroyTray();
LRESULT CALLBACK OnTrayMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
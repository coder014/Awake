
#include <Windows.h>
#include "resource.h"
#include "tray.h"
#include "helper.h"

#define IDM_TRAY_AWAKE 0
#define IDM_TRAY_SCREEN 1
#define IDM_TRAY_QUIT 5

NOTIFYICONDATA nid;
HMENU hMenu;
BOOL AwakeChecked = 0, ScreenChecked = 0;

VOID OnTrayAwake();
VOID OnTrayScreen();

BOOL InitTray(HINSTANCE hInstance, HWND hWnd)
{
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = IDI_TRAY;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_TRAY;
	nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_AWAKE));
	lstrcpy(nid.szTip, L"Awake Tray");

	hMenu = CreatePopupMenu();
	AppendMenu(hMenu, MF_STRING, IDM_TRAY_AWAKE, TEXT("Keep awake"));
	AppendMenu(hMenu, MF_STRING, IDM_TRAY_SCREEN, TEXT("Keep screen on"));
	AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
	AppendMenu(hMenu, MF_STRING, IDM_TRAY_QUIT, TEXT("Exit"));
	OnTrayAwake();
	Shell_NotifyIcon(NIM_ADD, &nid);
	return TRUE;
}

VOID DestroyTray()
{
	Shell_NotifyIcon(NIM_DELETE, &nid);
}

LRESULT CALLBACK OnTrayMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_TRAY && lParam == WM_RBUTTONUP)
	{
		POINT p;
		GetCursorPos(&p);
		SetForegroundWindow(hWnd);

		TrackPopupMenu(hMenu, TPM_LEFTBUTTON, p.x, p.y, 0, hWnd, NULL);
		return 0;
	}
	else if (uMsg == WM_COMMAND && HIWORD(wParam) == 0)
	{
		switch (LOWORD(wParam))
		{
		case IDM_TRAY_AWAKE: OnTrayAwake(); break;
		case IDM_TRAY_SCREEN: OnTrayScreen(); break;
		case IDM_TRAY_QUIT: PostMessage(hWnd, WM_CLOSE, 0, 0); break;
		}
		return 0;
	}
	else return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

VOID OnTrayAwake()
{
	if (AwakeChecked) // Switching off
	{
		StopHelper();
		CheckMenuItem(hMenu, IDM_TRAY_AWAKE, MF_BYCOMMAND | MF_UNCHECKED);
		EnableMenuItem(hMenu, IDM_TRAY_SCREEN, MF_BYCOMMAND | MF_GRAYED);
	}
	else // Switching on
	{
		if (GetHelperStatus())
			StopHelper();
		StartHelper(ScreenChecked);
		CheckMenuItem(hMenu, IDM_TRAY_AWAKE, MF_BYCOMMAND | MF_CHECKED);
		EnableMenuItem(hMenu, IDM_TRAY_SCREEN, MF_BYCOMMAND | MF_ENABLED);
	}
	AwakeChecked ^= 1;
}

VOID OnTrayScreen()
{
	if (ScreenChecked) // Switching off
	{
		StopHelper();
		StartHelper(FALSE);
		CheckMenuItem(hMenu, IDM_TRAY_SCREEN, MF_BYCOMMAND | MF_UNCHECKED);
	}
	else // Switching on
	{
		StopHelper();
		StartHelper(TRUE);
		CheckMenuItem(hMenu, IDM_TRAY_SCREEN, MF_BYCOMMAND | MF_CHECKED);
	}
	ScreenChecked ^= 1;
}
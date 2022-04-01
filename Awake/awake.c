#include <Windows.h>
#include "tray.h"
#include "helper.h"
#include "resource.h"

HINSTANCE hInst;
const TCHAR *szTitle = TEXT("Awake Tray Window");
const TCHAR *szWindowClass = TEXT("AwakeWin");

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	MyRegisterClass(hInstance);
	if (!InitInstance(hInstance))
		return FALSE;

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return TRUE;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style          = 0;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_AWAKE));
	wcex.hCursor        = NULL;
	wcex.hbrBackground  = NULL;
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = szWindowClass;
	wcex.hIconSm        = NULL;

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance)
{
   hInst = hInstance;

   HWND hWnd = CreateWindowEx(WS_EX_TOOLWINDOW, szWindowClass, szTitle, WS_POPUP,
	  CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
   if (!hWnd) return FALSE;

   ShowWindow(hWnd, SW_HIDE);
   UpdateWindow(hWnd);

   InitHelper();
   InitTray(hInstance, hWnd);
   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	case WM_TRAY:
		return OnTrayMsg(hWnd, message, wParam, lParam);
	case WM_QUIT:
	case WM_CLOSE:
	case WM_DESTROY:
		DestroyTray();
		DestroyHelper();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
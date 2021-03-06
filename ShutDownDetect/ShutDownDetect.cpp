// ShutDownDetect.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ShutDownDetect.h"
#include <stdio.h>
void UpdateShutdownInfo(LPARAM lParam);
void AutomaticLaunch();

#define SHUTDOWN_OR_RESTART 0
#define STARTUP 1
const char* week_day_list[] = {
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday"
};


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SHUTDOWNDETECT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    UpdateShutdownInfo(STARTUP);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SHUTDOWNDETECT));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SHUTDOWNDETECT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_SHUTDOWNDETECT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, SW_HIDE/*nCmdShow*/);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
        break;
    case WM_QUERYENDSESSION:
        UpdateShutdownInfo(lParam);
        return TRUE;
        break;
    case WM_ENDSESSION:
        break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void UpdateShutdownInfo(LPARAM lParam)
{
    if (lParam != SHUTDOWN_OR_RESTART && lParam != STARTUP) return;
    char path[MAX_PATH] = {0};
    char path_backup[MAX_PATH] = {0};
    GetModuleFileNameA(NULL, path, MAX_PATH);
    *strrchr(path, '\\') = 0;
    strcat(path, "\\shutdown_time_record.csv");
    strcat(path_backup, path);
    strcat(path_backup, "_backup");
    FILE* fp = fopen(path, "a");
    if (!fp) return;
    SYSTEMTIME tm;
    GetLocalTime(&tm);
    char record[100] = {0};
    char action[100] = {0};
    if (lParam == SHUTDOWN_OR_RESTART) {
        strcpy(action, "shutdown or restart");
    } else if (lParam == STARTUP) {
        strcpy(action, "startup");
    }
    sprintf(record, "%d-%d-%d,%s,%d:%d:%d,%s\n", tm.wYear, tm.wMonth, tm.wDay, week_day_list[tm.wDayOfWeek], tm.wHour, tm.wMinute, tm.wSecond, action);
    fwrite(record, strlen(record), 1, fp);
    fclose(fp);
    CopyFileA(path, path_backup, FALSE);

}

void AutomaticLaunch()
{

    HKEY hKey;
    LPCSTR lpRun = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    long lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE, lpRun, 0, KEY_WRITE, &hKey); 
    if(lRet == ERROR_SUCCESS) {
        char path[MAX_PATH] = {0};
        char name[MAX_PATH] = {0};
        DWORD dwRet = GetModuleFileNameA(NULL, path, MAX_PATH);
        strncpy(name, strrchr(path, '\\') + 1, strrchr(path, '.') - 1 - strrchr(path, '\\'));
        lRet = RegSetValueExA(hKey, name, 0, REG_SZ, (BYTE *)path, dwRet);
        RegCloseKey(hKey); 
        if(lRet != ERROR_SUCCESS) 
        {
        } 
    }
}
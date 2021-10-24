// Lab6.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Lab6.h"
#include <cmath>

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;								// текущий экземпляр
TCHAR szTitle[MAX_LOADSTRING];					// Текст строки заголовка
TCHAR szWindowClass[MAX_LOADSTRING];			// имя класса главного окна

// Прямые объявления функций, включенных в этот модуль кода:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	// Инициализация глобальныя строки
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_LAB6, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB6));

	// Основной цикл сообщений:
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
//	ФУНКЦИЯ: MyRegisterClass()
//
//  НАЗНАЧЕНИЕ: Регистрирует класс окна.
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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB6));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_LAB6);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

#define START_BUTTON_ID 1001
#define STOP_BUTTON_ID 1002

int winWidth = 600;

HANDLE thread1, thread2;

struct ThreadArgs {
	int time;
	int x, y;
} args1, args2;

DWORD WINAPI MoveCircle(ThreadArgs *args){
	while (true) {
		args->time += 13;
		args->x = args->time % winWidth;
		args->y = sin((double)(args->time) / 100) * 70 + 130;
		Sleep(40);
	}
	return 0;
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   НАЗНАЧЕНИЕ: Сохраняет дескриптор экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
// В этой функции мы сохраняем дескриптор экземпляра в глобальной переменной и
// создаем и отображаем главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Храним дескриптор экземпляра в нашей глобальной переменной

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 1200, 400, NULL, NULL, hInstance, NULL);

   CreateWindow(L"BUTTON", L"Start", WS_CHILD | WS_VISIBLE,
	   20, 10, 150, 30, hWnd, (HMENU)START_BUTTON_ID, hInstance, NULL);
   CreateWindow(L"BUTTON", L"Stop", WS_CHILD | WS_VISIBLE,
	   200, 10, 150, 30, hWnd, (HMENU)STOP_BUTTON_ID, hInstance, NULL);

   args1.time = 0;
   args2.time = 100;

   thread1 = CreateThread(NULL, 0u, (LPTHREAD_START_ROUTINE)MoveCircle, &args1, 0, NULL);
   SuspendThread(thread1);

   thread2 = CreateThread(NULL, 0u, (LPTHREAD_START_ROUTINE)MoveCircle, &args2, 0, NULL);
   SuspendThread(thread2);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

bool active = false;

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  НАЗНАЧЕНИЕ: Обрабатывает сообщения для главного окна.
//
//  WM_COMMAND	- обработать меню приложения
//  WM_PAINT	- Раскрасить главное окно
//  WM_DESTROY	- отправить сообщение о выходе и вернуться
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
		switch (wmId)
		{
		case START_BUTTON_ID:
			if (!active) {
				ResumeThread(thread1);
				ResumeThread(thread2);

				SetTimer(hWnd, NULL, 20, NULL);

				active = true;
			}
			break;
		case STOP_BUTTON_ID:
			if (active) {
				SuspendThread(thread1);
				SuspendThread(thread2);
				active = false;
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_TIMER:
		InvalidateRect(hWnd, NULL, true);
		if (active)
			SetTimer(hWnd, NULL, 20, NULL);
		break;
	case WM_SIZE:
		winWidth = LOWORD(lParam);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		if (args1.y != 0)
		{
			HBRUSH brush = CreateSolidBrush(RGB(84, 84, 84));
			SelectObject(hdc, brush);

			Ellipse(hdc, args1.x, args1.y, args1.x + 100, args1.y + 100);
			Ellipse(hdc, args2.x, args2.y, args2.x + 60, args2.y + 60);
		}
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}















#include "stdafx.h"
#include "Win32Project1.h"
#include <windows.h>
#include <CommDlg.h>
#include <stdio.h>
#include <string>
#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;								// текущий экземпляр
TCHAR szTitle[MAX_LOADSTRING];					// Текст строки заголовка
TCHAR szWindowClass[MAX_LOADSTRING];			// имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: разместите код здесь.
	MSG msg;
	HACCEL hAccelTable;

	// Инициализация глобальных строк
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WIN32PROJECT1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT1));

	// Цикл основного сообщения:
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
//  ФУНКЦИЯ: MyRegisterClass()
//
//  НАЗНАЧЕНИЕ: регистрирует класс окна.
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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32PROJECT1));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WIN32PROJECT1);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}
	
//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   НАЗНАЧЕНИЕ: сохраняет обработку экземпляра и создает главное окно.
//
//   КОММЕНТАРИИ:
//
//        В данной функции дескриптор экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится на экран главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

HBITMAP bitmaps[5];
HBITMAP PrevBitmap;
int curBitmap=-1;
bool static flag=false,flag1=false,flag2=false,flag3=true;

void CrBitmap(HDC hdc,RECT rect)
{
	int i;
	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
	HANDLE oldBitmap = SelectObject(hdcMem, hBitmap);
	BitBlt(hdcMem, 0, 0, rect.right, rect.bottom, hdc, 0, 0, SRCCOPY);	
	PrevBitmap=bitmaps[0];	
	if(curBitmap<4)
	{
		curBitmap++;
		bitmaps[curBitmap] = hBitmap;	

	}
	else
	{
		for(i=0;i<4;i++)
		{
			bitmaps[i]=bitmaps[i+1];

		}
		
		bitmaps[4] = hBitmap;	
	}
	
	SelectObject(hdcMem, oldBitmap);
	DeleteObject(oldBitmap);
	DeleteDC(hdcMem);

}
HBITMAP hBitmap;

void LdBitmap(HDC hdc,HWND hWnd,RECT rect)
{

	HDC hdcMem = CreateCompatibleDC(hdc);	
	if(flag1)
	{		
		if(curBitmap>0)
		{
			curBitmap--;
		}
		else bitmaps[0]=PrevBitmap;
	}			
	hBitmap = bitmaps[curBitmap];			
	HGDIOBJ oldBitmap = SelectObject(hdcMem, hBitmap);
	BITMAP bitmap;
	GetObject(hBitmap,sizeof(bitmap),&bitmap);
	BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);
	SelectObject(hdcMem, oldBitmap);
	
	DeleteDC(hdcMem);
}






LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC static hdc; 
	POINT static StartPoint,EndPoint,PrevPoint;	
	RECT static rect,rect1;
	int static status=3;
	HMENU static MainMenu = CreateMenu();
	HMENU static SubMenuDraw = CreateMenu();
	HMENU static SubMenuAction = CreateMenu();
	HMENU static SubMenuFile= CreateMenu();		
	HDC static hdcMem;
	HBITMAP static hBitmap;
	HGDIOBJ static oldBitmap;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		
		// Разобрать выбор в меню:
		status=wParam;
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
	case WM_CREATE:			
		AppendMenu(MainMenu,MF_POPUP,(UINT_PTR)SubMenuFile,L"File");
		AppendMenu(MainMenu,MF_POPUP,(UINT_PTR)SubMenuDraw,L"Draw");
		AppendMenu(MainMenu,MF_POPUP,(UINT_PTR)SubMenuAction,L"Action");		
		AppendMenu(SubMenuDraw, MF_STRING, 3, L"Pen");
		AppendMenu(SubMenuDraw, MF_STRING, 4, L"Line");	
		AppendMenu(SubMenuDraw, MF_STRING, 5, L"Triangle");
		AppendMenu(SubMenuDraw, MF_STRING, 6, L"Rectangle");
		AppendMenu(SubMenuDraw, MF_STRING, 7, L"Ellipse");
		AppendMenu(SubMenuDraw, MF_STRING, 8, L"Poliline");		
		AppendMenu(SubMenuFile, MF_STRING, 10, L"SaveFile");
		AppendMenu(SubMenuFile, MF_STRING, 11, L"OpenFile");
		AppendMenu(SubMenuFile, MF_STRING, 11, L"Print");
		AppendMenu(SubMenuFile, MF_STRING, 12, L"Exit");
		AppendMenu(SubMenuAction, MF_STRING, 20, L"Zoom");
		AppendMenu(SubMenuAction, MF_STRING, 21, L"Move");
		SetMenu(hWnd, MainMenu);	
		break;


	case WM_LBUTTONDOWN:
		if(flag3) 
		{	
			GetClientRect(hWnd,&rect);
			CrBitmap(hdc,rect);
			flag3=false;
		}	
				
		flag=true;
		StartPoint.x=LOWORD(lParam);	     
		StartPoint.y=HIWORD(lParam);		
		ReleaseDC(hWnd, hdc);
		break;
	case WM_KEYDOWN:
		if ((GetAsyncKeyState(VK_CONTROL)) && (GetAsyncKeyState(0x5A))){
		flag1 = true;		
		InvalidateRect(hWnd,NULL,TRUE);
		UpdateWindow(hWnd);		
		}
		break;
	case WM_LBUTTONUP:
		hdc = GetDC(hWnd);
		CrBitmap(hdc,rect);		
		flag=false;
		ReleaseDC(hWnd,hdc);
		break;
	case WM_MOUSEMOVE:	
		hdc=GetDC(hWnd);			
		EndPoint.x=LOWORD(lParam);	     
		EndPoint.y=HIWORD(lParam);	       		
		GetClientRect(hWnd,&rect);
		if(flag )
		{
			if(status==3)
			{
				MoveToEx(hdc,PrevPoint.x,PrevPoint.y,NULL);
				LineTo(hdc,EndPoint.x,EndPoint.y);
			}
		
			if(status>3 && status<8)
			{				
				
				hdcMem = CreateCompatibleDC(hdc);				
				hBitmap = CreateCompatibleBitmap(hdc,rect.right,rect.bottom);							
				oldBitmap = SelectObject(hdcMem, hBitmap);
				FillRect(hdcMem,&rect,WHITE_BRUSH);
			//	DeleteObject(SelectObject(hdcMem, hBitmap));
				LdBitmap(hdcMem,hWnd,rect);
			}
			switch(status)
			{
				
				case 4:									
					MoveToEx(hdcMem,StartPoint.x,StartPoint.y,NULL);
					LineTo(hdcMem,EndPoint.x,EndPoint.y);
					break;
				
				case 5:			
					MoveToEx(hdcMem,StartPoint.x,StartPoint.y,NULL);
					LineTo(hdcMem,StartPoint.x-(StartPoint.x-EndPoint.x),StartPoint.y);
					LineTo(hdcMem,StartPoint.x-(StartPoint.x-EndPoint.x)/2,StartPoint.y-(StartPoint.y-EndPoint.y));
					LineTo(hdcMem,StartPoint.x,StartPoint.y);
					break;
				case 6:				
					MoveToEx(hdcMem,StartPoint.x,StartPoint.y,NULL);
					LineTo(hdcMem,StartPoint.x-(StartPoint.x-EndPoint.x),StartPoint.y);
					LineTo(hdcMem,StartPoint.x-(StartPoint.x-EndPoint.x),StartPoint.y-(StartPoint.y-EndPoint.y));
					LineTo(hdcMem,StartPoint.x,StartPoint.y-(StartPoint.y-EndPoint.x));
					LineTo(hdcMem,StartPoint.x,StartPoint.y);
					break;
				case 7:												
					Ellipse(hdcMem,StartPoint.x,StartPoint.y,StartPoint.x-(StartPoint.x-EndPoint.x),StartPoint.y-(StartPoint.y-EndPoint.y));
					break;
				default:	
					break;
			}	
			if(status>3 && status<8)
			{
				
				BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);					
				SelectObject(hdcMem, oldBitmap);
				DeleteObject(hBitmap);
				DeleteDC(hdcMem);	
			}

		}
		PrevPoint=EndPoint;
		GetFocus();
		ReleaseDC(hWnd,hdc);
		break;
    case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: добавьте любой код отрисовки...			 
		LdBitmap(hdc, hWnd,rect);
		flag1=false;
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

// Обработчик сообщений для окна "О программе".
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

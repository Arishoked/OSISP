#include "stdafx.h"
#include <windows.h>
#include <CommDlg.h>
#include <stdio.h>
#include <string>
#include "Win32Project1.h"

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
int  status=3;
int curBitmap=-1;
bool  Move=false,Cancel=false,Start=true;

void CrBitmap(HDC hdc,RECT rect)
{
	int i;
	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
	HANDLE oldBitmap = SelectObject(hdcMem, hBitmap);
	FillRect(hdcMem,&rect,WHITE_BRUSH);	
	if(status==20) BitBlt(hdcMem, 0, 0, rect.right-4, rect.bottom-4, hdc, 0, 0, SRCCOPY);
	else BitBlt(hdcMem, 0, 0, rect.right, rect.bottom, hdc, 0, 0, SRCCOPY);
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
	if(Cancel)
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
	HDC static hdc,hdc1; 
	POINT static StartPoint,EndPoint,PrevPoint,StartPolylinePoint,PrevPolylinePoint;	
	RECT static rect;
	BOOL static bText=false;
	BOOL static bPolyline=false;
	BOOL static bStartPolyline=false;
	int static Width=1;
	HMENU static MainMenu = CreateMenu();
	HMENU static SubMenuDraw = CreateMenu();
	HMENU static SubMenuAction = CreateMenu();
	HMENU static SubMenuFile = CreateMenu();		
	HMENU static SubMenuColor = CreateMenu();
	HMENU static SubMenuWidth = CreateMenu();
	HDC static hdcMem,CompabitibleDC;
	HBITMAP static hBitmap,CompabitibleBitmap;
	HGDIOBJ static oldBitmap,oldBitmap1,oldPen,oldPen1;
	HPEN static hPen=(PS_SOLID, 1, RGB(0,0,0));
	HBRUSH static hBrush;
	
	static COLORREF  crCustColor[16];
	static CHOOSECOLOR ccPen,ccBrush;
	switch (message)
	{
	case WM_CREATE:			
		ShowWindow(hWnd,SW_SHOWMAXIMIZED);
		hdc=GetDC(hWnd);
		AppendMenu(MainMenu,MF_POPUP,(UINT_PTR)SubMenuFile,L"File");
		AppendMenu(MainMenu,MF_POPUP,(UINT_PTR)SubMenuDraw,L"Draw");
		AppendMenu(MainMenu,MF_POPUP,(UINT_PTR)SubMenuAction,L"Action");		
		AppendMenu(MainMenu,MF_POPUP,(UINT_PTR)SubMenuColor,L"Color");
		AppendMenu(MainMenu,MF_POPUP,(UINT_PTR)SubMenuWidth,L"Width");
	
		AppendMenu(SubMenuDraw, MF_STRING, 3, L"Pen");
		AppendMenu(SubMenuDraw, MF_STRING, 4, L"Line");	
		AppendMenu(SubMenuDraw, MF_STRING, 5, L"Triangle");
		AppendMenu(SubMenuDraw, MF_STRING, 6, L"Rectangle");
		AppendMenu(SubMenuDraw, MF_STRING, 7, L"Ellipse");
		AppendMenu(SubMenuDraw, MF_STRING, 8, L"Polyline");	
		AppendMenu(SubMenuDraw, MF_STRING, 9, L"Polygon");			
		AppendMenu(SubMenuDraw, MF_STRING, 2, L"Eraser");	
		AppendMenu(SubMenuDraw, MF_STRING, 1, L"Text");	
		
		AppendMenu(SubMenuFile, MF_STRING, 13, L"New");
		AppendMenu(SubMenuFile, MF_STRING, 10, L"Save");
		AppendMenu(SubMenuFile, MF_STRING, 11, L"Open");
		AppendMenu(SubMenuFile, MF_STRING, 11, L"Print");
		AppendMenu(SubMenuFile, MF_STRING, 12, L"Exit");
		
		AppendMenu(SubMenuAction, MF_STRING, 20, L"Pan");		
		AppendMenu(SubMenuAction, MF_STRING, 21, L"Zoom");
		
		AppendMenu(SubMenuColor, MF_STRING, 30, L"PenColor");
		AppendMenu(SubMenuColor, MF_STRING, 32, L"TransparentFill");
		AppendMenu(SubMenuColor, MF_STRING, 31, L"FillColor");	
		
		AppendMenu(SubMenuWidth, MF_STRING, 41, L"1");
		AppendMenu(SubMenuWidth, MF_STRING, 42, L"2");
		AppendMenu(SubMenuWidth, MF_STRING, 43, L"3");
		AppendMenu(SubMenuWidth, MF_STRING, 44, L"4");
		AppendMenu(SubMenuWidth, MF_STRING, 45, L"5");
		SetMenu(hWnd, MainMenu);	
		break;	
	
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		
		// Разобрать выбор в меню:
		if(wParam>1 && wParam<10  || wParam==20) status=wParam;	
		if(wParam==1) bText=true;
		if(wParam==8 || wParam==9) bPolyline=true;
		else
		{
			bPolyline=false;
			if(bStartPolyline) 
			{
				bStartPolyline=false;
				CrBitmap(hdc,rect);							
				SelectObject(CompabitibleDC, oldBitmap1);
				DeleteObject(CompabitibleBitmap);
				DeleteDC(CompabitibleDC);

			}
		}
		if(wParam==30) 
		{
			ccPen.lStructSize = sizeof(CHOOSECOLOR);
			ccPen.hInstance = NULL;
			ccPen.hwndOwner = hWnd;
			ccPen.lpCustColors = crCustColor;
			ccPen.Flags = CC_RGBINIT|CC_FULLOPEN;
			ccPen.lCustData = 0L;
			ccPen.lpfnHook = NULL;
			ccPen.rgbResult = RGB(0x80, 0x80, 0x80);
			ccPen.lpTemplateName = NULL;	
			InvalidateRect(hWnd,NULL,TRUE);
			UpdateWindow(hWnd);
			if(ChooseColor(&ccPen))
			{
				DeleteObject(hPen);
				hPen=CreatePen(PS_SOLID, Width, ccPen.rgbResult);
				DeleteObject(SelectObject(hdc,hPen));
			}
		}
		if(wParam==31)
		{
			ccBrush.lStructSize = sizeof(CHOOSECOLOR);
			ccBrush.hInstance = NULL;
			ccBrush.hwndOwner = hWnd;
			ccBrush.lpCustColors = crCustColor;
			ccBrush.Flags = CC_RGBINIT|CC_FULLOPEN;
			ccBrush.lCustData = 0L;
			ccBrush.lpfnHook = NULL;
			ccBrush.rgbResult = RGB(0x80, 0x80, 0x80);
			ccBrush.lpTemplateName = NULL;	
			InvalidateRect(hWnd,NULL,TRUE);
			UpdateWindow(hWnd);
			if(ChooseColor(&ccBrush))
			{
				DeleteObject(hBrush);
				hBrush=CreateSolidBrush(ccBrush.rgbResult);

			}
		}
		if(wParam==32)
		{
			DeleteObject(hBrush);
			hBrush=(HBRUSH)GetStockObject(NULL_BRUSH);

		}
		if(wParam>40 && wParam<46) 
		{	
			Width=wParam-40;;
			DeleteObject(hPen);
			hPen=CreatePen(PS_SOLID, Width, ccPen.rgbResult);
			DeleteObject(SelectObject(hdc,hPen));
		}
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



	case WM_LBUTTONDOWN:		
		if(Start) 
		{	
			GetClientRect(hWnd,&rect);
			CrBitmap(hdc,rect);
			Start=false;
		}	
		StartPoint.x=LOWORD(lParam);	     
		StartPoint.y=HIWORD(lParam);		
		if(bStartPolyline)
		{
			MoveToEx(hdc,PrevPolylinePoint.x,PrevPolylinePoint.y,NULL);
			LineTo(hdc,StartPoint.x,StartPoint.y);
		}		
		if(!bStartPolyline)
		{
			CompabitibleDC = CreateCompatibleDC(hdc);				
			CompabitibleBitmap = CreateCompatibleBitmap(hdc,rect.right,rect.bottom);							
			oldBitmap1 = SelectObject(CompabitibleDC, CompabitibleBitmap);
			FillRect(CompabitibleDC,&rect,WHITE_BRUSH);
			LdBitmap(CompabitibleDC,hWnd,rect);		
		}
		Move=true;
		if(bPolyline)
		{
			bStartPolyline=true;
			bPolyline=false; 
			StartPolylinePoint.x=LOWORD(lParam);
			StartPolylinePoint.y=HIWORD(lParam);
			PrevPolylinePoint.x=LOWORD(lParam);
			PrevPolylinePoint.y=HIWORD(lParam);
		}	
		break;
	case WM_RBUTTONDOWN:
		if(bStartPolyline)
		{
			bStartPolyline=false;	
			bPolyline=true;
			if(status==9)
			{
				MoveToEx(hdc,PrevPolylinePoint.x,PrevPolylinePoint.y,NULL);	
				LineTo(hdc,StartPolylinePoint.x,StartPolylinePoint.y);				
			}
			CrBitmap(hdc,rect);							
			SelectObject(CompabitibleDC, oldBitmap1);
			DeleteObject(CompabitibleBitmap);
			DeleteDC(CompabitibleDC);
		}
		break;
	case WM_KEYDOWN:
		if ((GetAsyncKeyState(VK_CONTROL)) && (GetAsyncKeyState(0x5A)))
		{
			Cancel = true;		
			InvalidateRect(hWnd,NULL,TRUE);
			UpdateWindow(hWnd);		
		}
		break;
	case  WM_CHAR:
		if(bText)
		{
								
		}
		break;	
	case WM_LBUTTONUP:
		Move=false;	
		if(!bStartPolyline) 
		{	
			CrBitmap(hdc,rect);							
			SelectObject(CompabitibleDC, oldBitmap1);
			DeleteObject(CompabitibleBitmap);
			DeleteDC(CompabitibleDC);
		}
		else
		{						
			MoveToEx(CompabitibleDC,PrevPolylinePoint.x,PrevPolylinePoint.y,NULL);
			LineTo(CompabitibleDC,EndPoint.x,EndPoint.y);
			PrevPolylinePoint=EndPoint;
			
		}
		break;
	case WM_MOUSEMOVE:			
		EndPoint.x=LOWORD(lParam);	     
		EndPoint.y=HIWORD(lParam);	       		
		GetClientRect(hWnd,&rect);
		if(Move)
		{
			
			if(status==2)
			{
				DeleteObject(hPen);
				hPen=CreatePen(PS_SOLID,10,RGB(255,255,255));
				DeleteObject(SelectObject(hdc,hPen));	
				MoveToEx(hdc,PrevPoint.x,PrevPoint.y,NULL);
				LineTo(hdc,EndPoint.x,EndPoint.y);
				DeleteObject(hPen);
				hPen=CreatePen(PS_SOLID, Width, ccPen.rgbResult);
				DeleteObject(SelectObject(hdc,hPen));	

			}							
			if(status==3)
			{
				
				
					MoveToEx(hdc,PrevPoint.x,PrevPoint.y,NULL);
					LineTo(hdc,EndPoint.x,EndPoint.y);

			}
		
			if(status>3 && status<10)
			{								
				hdcMem = CreateCompatibleDC(hdc);				
				hBitmap = CreateCompatibleBitmap(hdc,rect.right,rect.bottom);							
				oldBitmap = SelectObject(hdcMem, hBitmap);
				FillRect(hdcMem,&rect,hBrush);				
				DeleteObject(SelectObject(hdcMem,hPen));
				DeleteObject(SelectObject(hdcMem,hBrush));
				BitBlt(hdcMem, 0, 0, rect.right, rect.bottom, CompabitibleDC, 0, 0, SRCCOPY);		
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
					LineTo(hdcMem,StartPoint.x,StartPoint.y-(StartPoint.y-EndPoint.y));
					LineTo(hdcMem,StartPoint.x,StartPoint.y);
					break;
				case 7:												
					Ellipse(hdcMem,StartPoint.x,StartPoint.y,StartPoint.x-(StartPoint.x-EndPoint.x),StartPoint.y-(StartPoint.y-EndPoint.y));
					break;
				case 8:									
					MoveToEx(hdcMem,PrevPolylinePoint.x,PrevPolylinePoint.y,NULL);
					LineTo(hdcMem,EndPoint.x,EndPoint.y);
					break;
				case 9:									
					MoveToEx(hdcMem,PrevPolylinePoint.x,PrevPolylinePoint.y,NULL);
					LineTo(hdcMem,EndPoint.x,EndPoint.y);
					break;
				case 20:
					BitBlt(hdc,EndPoint.x-StartPoint.x, EndPoint.y-StartPoint.y, rect.right-1, rect.bottom, CompabitibleDC, 0, 0, SRCCOPY);	
					break;
				default:	
					break;
			}	
			if(status>3 && status<10)
			{
				
				BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);					
				SelectObject(hdcMem, oldBitmap);
				DeleteObject(hBitmap);
				DeleteDC(hdcMem);	
			}
		}
		PrevPoint=EndPoint;
		//SetCapture(hWnd);
		break;
    case WM_PAINT:
		hdc1 = BeginPaint(hWnd, &ps);
		// TODO: добавьте любой код отрисовки...			 
		LdBitmap(hdc1, hWnd,rect);
		Cancel=false;
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

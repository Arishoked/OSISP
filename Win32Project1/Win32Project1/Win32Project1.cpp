#include "stdafx.h"
#include <windows.h>
#include <CommDlg.h>
#include <stdio.h>
#include <string>
#include "Win32Project1.h"
using namespace std;

#define MAX_LOADSTRING 100
#define ID_BUTTONTEXT 1
#define ID_BUTTONPEN 2
#define ID_BUTTONLINE 3

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

   hWnd = CreateWindow(szWindowClass, "Graphics Editor", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

HBITMAP hBitmap;
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
	HDC static hdc,hdc1,hdc2; 
	HDC static hdcFile;
	POINT static StartPoint,EndPoint,PrevPoint,StartPolylinePoint,PrevPolylinePoint,TextPoint;	
	RECT static rect;
	BOOL static bText=false,bTextStart=false;
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
	POINT static PointsPolygon[20];
	int static PointsCount=0;
	COLORREF static  crCustColor[16];
	CHOOSECOLOR static ccPen,ccBrush;
	OPENFILENAME static ofn;
	char static fullpath[256],filename[256],dir[256];
	HENHMETAFILE static hEnhMtf;
	ENHMETAHEADER static emh;
	PRINTDLG static pd;
	BOOL static bZoom;
	double static Delta,Scale;
	HANDLE static hFile;
	string static Text;
	DOCINFO di;
	switch (message)
	{
	case WM_CREATE:			
		ShowWindow(hWnd,SW_SHOWMAXIMIZED);
		hdc=GetDC(hWnd);
		AppendMenu(MainMenu,MF_POPUP,(UINT_PTR)SubMenuFile,"File");
		AppendMenu(MainMenu,MF_POPUP,(UINT_PTR)SubMenuDraw,"Draw");
		AppendMenu(MainMenu,MF_POPUP,(UINT_PTR)SubMenuAction,"Action");		
		AppendMenu(MainMenu,MF_POPUP,(UINT_PTR)SubMenuColor,"Color");
		AppendMenu(MainMenu,MF_POPUP,(UINT_PTR)SubMenuWidth,"Width");
	
		AppendMenu(SubMenuDraw, MF_STRING, 3, "Pen");
		AppendMenu(SubMenuDraw, MF_STRING, 4, "Line");	
		AppendMenu(SubMenuDraw, MF_STRING, 5, "Triangle");
		AppendMenu(SubMenuDraw, MF_STRING, 6, "Rectangle");
		AppendMenu(SubMenuDraw, MF_STRING, 7, "Ellipse");
		AppendMenu(SubMenuDraw, MF_STRING, 8, "Polyline");	
		AppendMenu(SubMenuDraw, MF_STRING, 9, "Polygon");			
		AppendMenu(SubMenuDraw, MF_STRING, 2, "Eraser");	
		AppendMenu(SubMenuDraw, MF_STRING, 1, "Text");	
		
		AppendMenu(SubMenuFile, MF_STRING, 13, "New");
		AppendMenu(SubMenuFile, MF_STRING, 11, "Open");		
		AppendMenu(SubMenuFile, MF_STRING, 10, "Save");
		AppendMenu(SubMenuFile, MF_STRING, 14, "Print");
		AppendMenu(SubMenuFile, MF_STRING, 12, "Exit");
		
		AppendMenu(SubMenuAction, MF_STRING, 20, "Pan");		
		AppendMenu(SubMenuAction, MF_STRING, 21, "Zoom");
		
		AppendMenu(SubMenuColor, MF_STRING, 30, "PenColor");
		AppendMenu(SubMenuColor, MF_STRING, 32, "TransparentFill");
		AppendMenu(SubMenuColor, MF_STRING, 31, "FillColor");	
		
		AppendMenu(SubMenuWidth, MF_STRING, 41, "1");
		AppendMenu(SubMenuWidth, MF_STRING, 42, "2");
		AppendMenu(SubMenuWidth, MF_STRING, 43, "3");
		AppendMenu(SubMenuWidth, MF_STRING, 44, "4");
		AppendMenu(SubMenuWidth, MF_STRING, 45, "5");
		SetMenu(hWnd, MainMenu);	
		break;	
	
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		
		// Разобрать выбор в меню:
		if(wParam>1 && wParam<10  || wParam==20) status=wParam;	
		if(wParam==1) 
		{
			bText=true;
			bTextStart=false;
			
			Text="";
		}
		else
		{
			bText=false;
		}
		if(status==8 || status==9) bPolyline=true;
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
				PointsCount=0;
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
				//DeleteObject(SelectObject(hdc,hPen));
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
		if(wParam==10)
		{
			
			ofn.lStructSize=sizeof(OPENFILENAME);
			ofn.hwndOwner=hWnd;
			ofn.hInstance=hInst; 
			ofn.lpstrFilter="Metafile (*.emf)\0*.emf\0Все файлы (*.*)\0*.*\0";
			ofn.nFilterIndex=1;
			ofn.lpstrFile=fullpath;
			ofn.nMaxFile=sizeof(fullpath);
			ofn.lpstrFileTitle=filename;
			ofn.nMaxFileTitle=sizeof(filename);
			ofn.lpstrInitialDir=dir;
			ofn.lpstrTitle="Save file as...";
			ofn.Flags=OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY|OFN_EXPLORER;
			if(GetSaveFileName(&ofn))
			{
				hdcFile=CreateEnhMetaFile(NULL,filename,NULL,NULL);
				BitBlt(hdcFile,0,0,rect.right,rect.bottom,hdc,0,0,SRCCOPY);
				hEnhMtf=CloseEnhMetaFile(hdcFile);
				DeleteEnhMetaFile(hEnhMtf);
			}
		}
		if(wParam==11)
		{
			ofn.lStructSize=sizeof(OPENFILENAME);
			ofn.hwndOwner=hWnd;
			ofn.hInstance=hInst; 
			ofn.lpstrFilter="Metafile (*.emf)\0*.emf\0Все файлы (*.*)\0*.*\0";
			ofn.nFilterIndex=1;
			ofn.lpstrFile=fullpath;
    		ofn.nMaxFile=sizeof(fullpath);
			ofn.lpstrFileTitle=filename;
			ofn.nMaxFileTitle=sizeof(filename);
			ofn.lpstrInitialDir=dir;
			ofn.lpstrTitle="Open file...";
			ofn.Flags=OFN_EXPLORER|OFN_CREATEPROMPT|OFN_ALLOWMULTISELECT;
			if(GetOpenFileName(&ofn))
			{
				hEnhMtf=GetEnhMetaFile(fullpath);
				GetEnhMetaFileHeader(hEnhMtf,sizeof(ENHMETAHEADER),&emh);
				SetRect(&rect,emh.rclBounds.left,emh.rclBounds.top,emh.rclBounds.right,emh.rclBounds.bottom);
				//SetWindowPos(hWnd,HWND_TOP,0,0,rect.right,rect.bottom,SWP_NOMOVE);
				PlayEnhMetaFile(hdc,hEnhMtf,&rect);			
				CrBitmap(hdc,rect);
				DeleteEnhMetaFile(hEnhMtf);
			}
		}
		
		if(wParam==12) PostQuitMessage(0);
		
		if(wParam==13)
		{
			FillRect(hdc,&rect,WHITE_BRUSH);	
		}
		if(wParam==14)
		{
			ZeroMemory(&pd, sizeof(pd));
			pd.lStructSize = sizeof(pd);
			pd.hwndOwner   = hWnd;
			pd.hDevMode    = NULL; // Не забудьте освободить или сохранить hDevMode
			pd.hDevNames   = NULL; // Не забудьте освободить или сохранить hDevNames
			pd.Flags       = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC;
			pd.nCopies     = 1;
			pd.nFromPage   = 0xFFFF;
			pd.nToPage     = 0xFFFF;
			pd.nMinPage    = 1;
			pd.nMaxPage    = 0xFFFF;


			if (PrintDlg(&pd)==TRUE)
			{
				int Rx = GetDeviceCaps(pd.hDC, LOGPIXELSX);
				int Ry = GetDeviceCaps(pd.hDC, LOGPIXELSY);
				di.cbSize=sizeof(DOCINFO);
				di.lpszDocName="Print Picture";
				di.fwType=NULL;
				di.lpszDatatype=NULL;
				di.lpszOutput=NULL;
				StartDoc(pd.hDC,&di);
				StartPage(pd.hDC);		
				GetClientRect(hWnd,&rect);
				int Rx1 = GetDeviceCaps(hdc, LOGPIXELSX);
				int Ry1 = GetDeviceCaps(hdc, LOGPIXELSY);
				StretchBlt(pd.hDC, 0, 0,(int)((float)(0.91*rect.right*Rx/Rx1)), (int)((float)(0.91*rect.bottom*Ry/Ry1)),
				hdc, 0, 0, rect.right, rect.bottom, SRCCOPY);
				EndPage(pd.hDC);
				EndDoc(pd.hDC);
				DeleteDC(pd.hDC);
			}
		}
		if(wParam==21) 
		{
			bZoom=true;
			Scale=1;			
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

		case WM_MOUSEWHEEL:
			if(bZoom)
			{
		
				GetClientRect(hWnd,&rect);
				CompabitibleDC = CreateCompatibleDC(hdc);				
				CompabitibleBitmap = CreateCompatibleBitmap(hdc,rect.right,rect.bottom);							
				oldBitmap1 = SelectObject(CompabitibleDC, CompabitibleBitmap);
				FillRect(CompabitibleDC,&rect,WHITE_BRUSH);
				LdBitmap(CompabitibleDC,hWnd,rect);	
				Delta = GET_WHEEL_DELTA_WPARAM(wParam);			
				if(Delta>0)Scale=Scale+0.03;
				if(Delta<0) Scale=Scale-0.03;
				FillRect(hdc,&rect,WHITE_BRUSH);
				StretchBlt(hdc,0,0,rect.right*Scale,rect.bottom*Scale,CompabitibleDC,0,0,rect.right,rect.bottom,SRCCOPY);
				SelectObject(CompabitibleDC, oldBitmap1);
				DeleteObject(CompabitibleBitmap);
				DeleteDC(CompabitibleDC);
			}
			
			
			break;

	case WM_LBUTTONDOWN:		
		if(bZoom)
		{
			bZoom=false;
			LdBitmap(hdc,hWnd,rect);

		}
		if(bText)
		if(!bTextStart)
		{
			TextPoint.x=LOWORD(lParam);	     
			TextPoint.y=HIWORD(lParam);	
			bTextStart=true;
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
			BitBlt(CompabitibleDC,0,0,rect.right-5,rect.bottom-5,hdc,0,0,SRCCOPY);	
		}
		Move=true;
		
		if(bPolyline)
		{
			PointsPolygon[PointsCount].x=StartPoint.x;
			PointsPolygon[PointsCount].y=StartPoint.y;
			PointsCount++;
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
				

				
				HPEN hPen1=CreatePen(PS_SOLID, Width, ccPen.rgbResult);
				HANDLE oldPen=SelectObject(CompabitibleDC,hPen1);
				SelectObject(CompabitibleDC,hBrush);
				Polygon(CompabitibleDC,PointsPolygon,PointsCount);
				DeleteObject(oldPen);						
			}
			PointsCount=0;	
			BitBlt(hdc,0,0,rect.right,rect.bottom,CompabitibleDC,0,0,SRCCOPY);
			CrBitmap(hdc,rect);							
			SelectObject(CompabitibleDC, oldBitmap1);
			DeleteObject(CompabitibleBitmap);
			DeleteDC(CompabitibleDC);
		}
		break;

		

	case WM_LBUTTONUP:
		Move=false;	
		if(!bStartPolyline && !bTextStart) 
		{	
			CrBitmap(hdc,rect);							
			SelectObject(CompabitibleDC, oldBitmap1);
			DeleteObject(CompabitibleBitmap);
			DeleteDC(CompabitibleDC);
		}
		else
		{						
			HPEN hPen1=CreatePen(PS_SOLID, Width, ccPen.rgbResult);
			HANDLE oldPen=SelectObject(CompabitibleDC,hPen1);
			SelectObject(CompabitibleDC,hBrush);
			MoveToEx(CompabitibleDC,PrevPolylinePoint.x,PrevPolylinePoint.y,NULL);
			LineTo(CompabitibleDC,EndPoint.x,EndPoint.y);
			PrevPolylinePoint=EndPoint;
			PointsPolygon[PointsCount].x=EndPoint.x;
			PointsPolygon[PointsCount].y=EndPoint.y;
			PointsCount++;
			DeleteObject(oldPen);						
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
			char c=(char)wParam;
			if(c==VK_RETURN)
			{
				bText=false;
				bTextStart=false;
				CrBitmap(hdc,rect);
			}
			else 
				if(c==VK_BACK)
					Text.pop_back();
				else
					Text+=c;
			LdBitmap(hdc,hWnd,rect);
			TextOut(hdc,TextPoint.x,TextPoint.y,Text.c_str(),strlen(Text.c_str()));								
		}
		break;

	case WM_MOUSEMOVE:			
		EndPoint.x=LOWORD(lParam);	     
		EndPoint.y=HIWORD(lParam);	       		
		GetClientRect(hWnd,&rect);
		if(Move && !bTextStart)
		{
			
			if(status==2)
			{
				DeleteObject(hPen);
				hPen=CreatePen(PS_SOLID,15,RGB(255,255,255));
				HANDLE oldPen=SelectObject(hdc,hPen);				
				MoveToEx(hdc,PrevPoint.x,PrevPoint.y,NULL);
				LineTo(hdc,EndPoint.x,EndPoint.y);
				DeleteObject(oldPen);
				DeleteObject(hPen);
				hPen=CreatePen(PS_SOLID, Width, ccPen.rgbResult);
				DeleteObject(SelectObject(hdc,hPen));	

			}							
			if(status==3)
			{			
					HANDLE oldPen=SelectObject(hdc,hPen);
					MoveToEx(hdc,PrevPoint.x,PrevPoint.y,NULL);
					LineTo(hdc,EndPoint.x,EndPoint.y);
					DeleteObject(oldPen);
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
					POINT Points[3];
					Points[1].x=StartPoint.x-(StartPoint.x-EndPoint.x);
					Points[1].y=StartPoint.y;
					Points[2].x=StartPoint.x-(StartPoint.x-EndPoint.x)/2;
					Points[2].y=StartPoint.y-(StartPoint.y-EndPoint.y);
					Points[0].x=StartPoint.x;
					Points[0].y=StartPoint.y;
					Polygon(hdcMem,Points,3);
					break;
				case 6:				
					Rectangle(hdcMem,StartPoint.x-(StartPoint.x-EndPoint.x),StartPoint.y,StartPoint.x,StartPoint.y-(StartPoint.y-EndPoint.y));
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

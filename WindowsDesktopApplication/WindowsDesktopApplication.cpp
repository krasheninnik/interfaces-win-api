// WindowsDesktopApplication.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "WindowsDesktopApplication.h"
#include <sstream>
#include <string>

#define MAX_LOADSTRING 100


// Global users variables:
HWND hWnd1;
HWND hWnd2;

HPEN hPen;
std::wstring textWithMousePosition = L"Here we go";
static const int textRectWidth = 120;
static const int textRectHeight = 20;


// from https://stackoverflow.com/questions/27220/how-to-convert-stdstring-to-lpcwstr-in-c-unicode
std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

void redrawMousePosition(HWND hWnd, RECT* rect, bool isNeededRedrawInSecondWindow) {
	InvalidateRect(hWnd, rect, TRUE);

	if (isNeededRedrawInSecondWindow) {
		//get hWnd of another Windows
		HWND hWndSecond = hWnd == hWnd1 ? hWnd2 : hWnd1;

		// calculate text Rectangle
		RECT rcClient;
		GetClientRect(hWndSecond, &rcClient);

		int textRectX = (rcClient.right - textRectWidth) / 2;
		int textRectY = 0; 

		RECT textRect = { textRectX, textRectY, textRectX + textRectWidth, textRectY + textRectHeight };
		
		// redraw
		InvalidateRect(hWndSecond, &textRect, TRUE);
	}
}

COLORREF setRedOrGreenPenColor(HWND hWnd) {
	static int count1 = 0;
	static int count2 = 0;
	static const COLORREF RED = RGB(255, 0, 0);
	static const COLORREF GREEN = RGB(0, 255, 0);

	// first init
	if (count1 == 0 && count2 == 0) {
		count1++;
		count2++;
		return RED;
	}

	// another click
	return ((hWnd == hWnd1 ? count1 : count2)++ % 2 == 0)? RED : GREEN;
}

std::wstring getTextWithMousePosition(POINT pt) {
	std::ostringstream oss;
	oss << "X: " << pt.x << " Y: " << pt.y;
	return s2ws(oss.str());
}

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

COLORREF hPenColor = setRedOrGreenPenColor(NULL);

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSDESKTOPAPPLICATION, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSDESKTOPAPPLICATION));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
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
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSDESKTOPAPPLICATION));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSDESKTOPAPPLICATION);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   hWnd1 = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   hWnd2 = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	   CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd1 or !hWnd2)
   {
      return FALSE;
   }

   ShowWindow(hWnd1, nCmdShow);
   UpdateWindow(hWnd1);

   ShowWindow(hWnd2, nCmdShow);
   UpdateWindow(hWnd2);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static POINT pt;           // current cursor location 
	static BOOL isRedrawMousePositionInSecondWindow = true;

	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	const int textRectX = (rcClient.right - textRectWidth) / 2;
	const int textRectY = 0; // rcClient.bottom / 4;
	
	RECT textRect = { textRectX, textRectY, textRectX + textRectWidth, textRectY + textRectHeight };

    switch (message)
    {
		// handle left mouse click 
	case WM_LBUTTONDOWN:
		GetCursorPos(&pt);
		hPenColor = setRedOrGreenPenColor(hWnd);
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_MOUSEMOVE: {
		GetCursorPos(&pt);
		textWithMousePosition = getTextWithMousePosition(pt);
		redrawMousePosition(hWnd, &textRect, isRedrawMousePositionInSecondWindow);
		break;
	}

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
			case ID_ACCEL_SECOND_SCREEN: 
				isRedrawMousePositionInSecondWindow = !isRedrawMousePositionInSecondWindow;
				break;

            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			
			// draw text
			SetTextColor(hdc, RGB(255, 0, 0));
			SetBkMode(hdc, TRANSPARENT);
			DrawText(hdc, textWithMousePosition.c_str(), -1, &textRect, DT_SINGLELINE | DT_NOCLIP);

			//SelectObject(hdc, oldPen);
			DeleteObject(hPen);

			// draw X
			hPen = CreatePen(PS_SOLID, 1, hPenColor);
			SelectObject(hdc, hPen);
			MoveToEx(hdc, 0, 0, NULL);
			LineTo(hdc, rcClient.right, rcClient.bottom);
			MoveToEx(hdc, 0, rcClient.bottom, NULL);
			LineTo(hdc, rcClient.right, 0);

            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
            EndPaint(hWnd, &ps);
        }
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

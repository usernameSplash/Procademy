// Lab_03_31.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "Lab_03_31.h"

#define MAX_LOADSTRING 100
#define GRID_SIZE 16
#define GRID_WIDTH 100
#define GRID_HEIGHT 50

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

HBRUSH g_hTileBrush;
HPEN g_hGridPen;
char g_Tile[GRID_HEIGHT][GRID_WIDTH];
bool g_bErase = false;
bool g_bDrag = false;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void RenderGrid(HDC hdc);
void RenderObstacle(HDC hdc);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB0331, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB0331));

    MSG msg;

    // 기본 메시지 루프입니다:
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
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB0331));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB0331);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        g_hGridPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
        g_hTileBrush = CreateSolidBrush(RGB(100, 100, 100));
        break;

    case WM_LBUTTONDOWN:
        g_bDrag = true;
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            int gridX = x / GRID_SIZE;
            int gridY = y / GRID_SIZE;

            if (g_Tile[gridY][gridX] == true)
            {
                g_bErase = true;
            }
            else
            {
                g_bErase = false;
            }

            g_Tile[gridY][gridX] = !g_bErase;
            InvalidateRect(hWnd, NULL, true);
        }
        break;

    case WM_LBUTTONUP:
        g_bDrag = false;
        break;

    case WM_MOUSEMOVE:
        if (g_bDrag == true)
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            int gridX = x / GRID_SIZE;
            int gridY = y / GRID_SIZE;

            g_Tile[gridY][gridX] = !g_bErase;
            InvalidateRect(hWnd, NULL, true);
        }
        break;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
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
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            RenderObstacle(hdc);
            RenderGrid(hdc);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        DeleteObject(g_hGridPen);
        DeleteObject(g_hTileBrush);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
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


void RenderGrid(HDC hdc)
{
    int x = 0;
    int y = 0;
    HPEN hOldPen = (HPEN)SelectObject(hdc, g_hGridPen);

    for (int widthCount = 0; widthCount <= GRID_WIDTH; widthCount++)
    {
        MoveToEx(hdc, x, 0, NULL);
        LineTo(hdc, x, GRID_HEIGHT * GRID_SIZE);
        x += GRID_SIZE;
    }

    for (int heightCount = 0; heightCount <= GRID_HEIGHT; heightCount++)
    {
        MoveToEx(hdc, 0, y, NULL);
        LineTo(hdc, GRID_WIDTH * GRID_SIZE, y);
        y += GRID_SIZE;
    }

    SelectObject(hdc, hOldPen);
}
void RenderObstacle(HDC hdc)
{
    int x;
    int y;

    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, g_hTileBrush);
    SelectObject(hdc, GetStockObject(NULL_PEN));

    for (int heightCount = 0; heightCount < GRID_HEIGHT; heightCount++)
    {
        for (int widthCount = 0; widthCount < GRID_WIDTH; widthCount++)
        {
            if (g_Tile[heightCount][widthCount])
            {
                x = widthCount * GRID_SIZE;
                y = heightCount * GRID_SIZE;
                Rectangle(hdc, x, y, x + GRID_SIZE + 2, y + GRID_SIZE + 2);
            }
        }
    }

    SelectObject(hdc, oldBrush);
}
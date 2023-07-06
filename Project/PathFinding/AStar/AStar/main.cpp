// main.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "main.h"

#include "Map.h"
#include "AStar.h"
#include "JPS.h"
#include "NodeHeap.h"
#include "MapController.h"

#include <cstdio>
using namespace std;

#define MAX_LOADSTRING 100

// 전역 변수:
PathFinder::Map* g_pMap = nullptr;
PathFinder::PathFinder* g_pPathFinder = nullptr;
PathFinder::AStar* g_pAStar = nullptr;
PathFinder::JPS* g_pJPS = nullptr;
PathFinder::MapController* g_pMapController = nullptr;

bool g_bSetStartNode = true; // when mouse rbutton pressed, set start node if this flag is true. or set dest node.

HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
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

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_ASTAR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ASTAR));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        while (g_pPathFinder && g_pPathFinder->IsPathFinding())
        {
            g_pPathFinder->PathFind();
            InvalidateRect(msg.hwnd, NULL, false);
            UpdateWindow(msg.hwnd);
            Sleep(1000);
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ASTAR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_ASTAR);
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
        {
            g_pMap = new PathFinder::Map(80, 40);
            g_pAStar = new PathFinder::AStar(g_pMap);
            g_pJPS = new PathFinder::JPS(g_pMap);
            g_pPathFinder = g_pAStar;

            g_pMapController = new PathFinder::MapController(g_pMap, g_pPathFinder);
            break;
        }
    case WM_LBUTTONDOWN:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);

            g_pMapController->StartDrag(x, y);
            g_pMapController->SetGrid(x, y);

            InvalidateRect(hWnd, NULL, true);
            break;
        }
    case WM_LBUTTONUP:
        {
            g_pMapController->EndDrag();
            break;
        }
    case WM_MOUSEMOVE:
        {
            if (g_pMapController->IsDragging())
            {
                int x = GET_X_LPARAM(lParam);
                int y = GET_Y_LPARAM(lParam);
                g_pMapController->SetGrid(x, y);
                InvalidateRect(hWnd, NULL, true);
            }
        break;
        }
    case WM_RBUTTONDOWN:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);

            if (g_bSetStartNode)
            {
                g_pMapController->SetStartGrid(x, y);
            }
            else
            {
                g_pMapController->SetDestGrid(x, y);
            }

            InvalidateRect(hWnd, NULL, true);

            break;
        }
    case WM_KEYDOWN:
        {
            switch (wParam)
            {
            case VK_RETURN:
                {
                    if (g_pPathFinder->IsPathFinding() == false)
                    {
                        g_pPathFinder->StartPathFinding();
                    }
                    break;
                }
            case 0x31: // 1 Key
                {
                    if (g_pPathFinder->IsPathFinding() == false && g_pPathFinder != g_pAStar)
                    {
                        g_pPathFinder = g_pAStar;
                        g_pMapController->BindPathFinder(g_pPathFinder);
                    }
                    break;
                }
            case 0x32: // 2 Key
                {
                    if (g_pPathFinder->IsPathFinding() == false && g_pPathFinder != g_pJPS)
                    {
                        g_pPathFinder = g_pJPS;
                        g_pMapController->BindPathFinder(g_pPathFinder);
                    }
                    break;
                }
            case 0x44: // D Key
                {
                    g_bSetStartNode = false;
                    break;
                }
            case 0x52: // R Key
                {
                    g_pMap->RemoveAllObstacles();
                    InvalidateRect(hWnd, NULL, true);
                    break;
                }
            case 0x53: // S Key
                {
                    g_bSetStartNode = true;
                    break;
                }
            default:
                break;
            }
        }
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
            g_pMapController->Render(hdc);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        {
            delete g_pMap;
            delete g_pPathFinder;
            delete g_pMapController;
            PostQuitMessage(0);
            break;
        }
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

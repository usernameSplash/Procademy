// Assignment_05_10.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#pragma comment(lib, "Ws2_32.lib")

#include <cstdio>

#include "framework.h"
#include "Assignment_05_10.h"
#include "RingBuffer.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
bool g_bClick = false;
HPEN g_hPen;
int g_oldX;
int g_oldY;
int g_curX;
int g_curY;

using namespace MyDataStructure;

#define UM_SOCKET (WM_USER + 1)
#define SERVER_PORT 25000

SOCKET g_ClientSocket;
SOCKADDR_IN g_ServerAddr;
RingBuffer g_RecvBuffer;
RingBuffer g_SendBuffer;
bool bConnect = false;
bool bSend = false;

FILE* logFile;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void PacketProc(HWND hWnd, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    HACCEL hAccelTable;
    MSG msg;

    logFile = fopen("log.txt", "w");

#pragma region WIN_INIT
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_ASSIGNMENT0510, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ASSIGNMENT0510));

    g_hPen = CreatePen(PS_SOLID, 5, RGB(0, 0, 0));

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
#pragma endregion

    fclose(logFile);
    closesocket(g_ClientSocket);
    WSACleanup();

    return (int)msg.wParam;
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

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ASSIGNMENT0510));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

#pragma region SOCKET_INIT
    WSAData wsa;
    int connectRet;
    int asyncSelectRet;
    int setSockOptRet;
    linger l;
    BOOL noDelayOpt;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        return 1;
    }

    g_ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (g_ClientSocket == INVALID_SOCKET)
    {
        goto CREATE_SOCKET_FAILED;
    }

    l.l_onoff = 1;
    l.l_linger = 0;
    setSockOptRet = setsockopt(g_ClientSocket, SOL_SOCKET, SO_LINGER, (char*)&l, sizeof(l));
    if (setSockOptRet == SOCKET_ERROR)
    {
        int errorCode = WSAGetLastError();
        fwprintf(logFile, L"Error : %d on %d line", errorCode, __LINE__);
        goto SOCKET_ERROR_OCCURRED;
    }

    noDelayOpt = TRUE;
    setSockOptRet = setsockopt(g_ClientSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&noDelayOpt, sizeof(noDelayOpt));
    if (setSockOptRet == SOCKET_ERROR)
    {
        int errorCode = WSAGetLastError();
        fwprintf(logFile, L"Error : %d on %d line", errorCode, __LINE__);
        goto SOCKET_ERROR_OCCURRED;
    }

    asyncSelectRet = WSAAsyncSelect(g_ClientSocket, hWnd, UM_SOCKET, FD_READ|FD_WRITE|FD_CONNECT|FD_CLOSE);
    if (asyncSelectRet == SOCKET_ERROR)
    {
        int errorCode = WSAGetLastError();
        fwprintf(logFile, L"Error : %d on %d line", errorCode, __LINE__);
        goto SOCKET_ERROR_OCCURRED;
    }

    g_ServerAddr.sin_family = AF_INET;
    //g_ServerAddr.sin_addr.s_addr = htonl((192 << 24) + (168 << 16) + (30 << 8) + 13);
    g_ServerAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    g_ServerAddr.sin_port = htons(SERVER_PORT);

    connectRet = connect(g_ClientSocket, (SOCKADDR*)&g_ServerAddr, sizeof(g_ServerAddr));
    if (connectRet == SOCKET_ERROR)
    {
        int errorCode = WSAGetLastError();
        if (errorCode != WSAEWOULDBLOCK)
        {
            fwprintf(logFile, L"Error : %d on %d line", errorCode, __LINE__);
            goto SOCKET_ERROR_OCCURRED;
        }
    }

#pragma endregion

    return TRUE;

SOCKET_ERROR_OCCURRED:
    closesocket(g_ClientSocket);
CREATE_SOCKET_FAILED:
    WSACleanup();
    return FALSE;
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

    case WM_LBUTTONDOWN:
        {
            g_bClick = true;
            g_oldX = GET_X_LPARAM(lParam);
            g_oldY = GET_Y_LPARAM(lParam);
            break;
        }

    case WM_LBUTTONUP:
        g_bClick = false;
        break;

    case WM_MOUSEMOVE:
        if (g_bClick)
        {
            g_curX = GET_X_LPARAM(lParam);
            g_curY = GET_Y_LPARAM(lParam);

            PacketHeader_t header;
            DrawPacket_t packet;

            header.length = sizeof(packet);
            packet = { g_oldX, g_oldY, g_curX, g_curY };

            g_SendBuffer.Enqueue((char*)&header, sizeof(header));
            g_SendBuffer.Enqueue((char*)&packet, sizeof(packet));

            g_oldX = g_curX;
            g_oldY = g_curY;

            if (bConnect && bSend)
            {
                while (g_SendBuffer.Size() > 0)
                {
                    char buf[512];
                    size_t peekRet;
                    int sendRet;

                    peekRet = g_SendBuffer.Peek(buf, 512);

                    sendRet = send(g_ClientSocket, buf, (int)peekRet, 0);
                    if (sendRet == SOCKET_ERROR)
                    {
                        int errorCode = WSAGetLastError();
                        if (errorCode == WSAEWOULDBLOCK)
                        {
                            bSend = false;
                            break;
                        }
                        else
                        {
                            fwprintf(logFile, L"Error : %d on %d line", errorCode, __LINE__);
                            bConnect = false;
                            DestroyWindow(hWnd);
                            break;
                        }
                    }
                    else
                    {
                        fwprintf(logFile, L"Send\n");
                        g_SendBuffer.Dequeue(peekRet);
                    }
                }
            }
        }
        break;

    case UM_SOCKET:
        if (WSAGETSELECTERROR(lParam))
        {
            fwprintf(logFile, L"Error : %d on %d line", WSAGETSELECTERROR(lParam), __LINE__);
            if (WSAGETSELECTERROR(lParam) == WSAETIMEDOUT)
            {
                DestroyWindow(hWnd);
            }
        }
        else
        {
            PacketProc(hWnd, wParam, lParam);
        }
        break;

    //case WM_PAINT:
    //    {
    //        PAINTSTRUCT ps;
    //        HDC hdc = BeginPaint(hWnd, &ps);
    //        // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...

    //        if (g_bClick)
    //        {
    //            HPEN hPenOld = (HPEN)SelectObject(hdc, g_hPen);

    //            MoveToEx(hdc, g_oldX, g_oldY, NULL);
    //            LineTo(hdc, g_curX, g_curY);

    //            SelectObject(hdc, hPenOld);

    //            g_oldX = g_curX;
    //            g_oldY = g_curY;
    //        }

    //        EndPaint(hWnd, &ps);
    //    }
    //    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void PacketProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    SOCKET clientSocket = (SOCKET)wParam;

    switch (WSAGETSELECTEVENT(lParam))
    {
    case FD_READ:
        {
            int recvRet;
            char buf[512];

            PacketHeader_t header;
            DrawPacket_t packet;
            size_t recvRet;
            size_t peekRet;

            fwprintf(logFile, L"Received\n");

            recvRet = recv(clientSocket, buf, sizeof(buf), 0);
            if (recvRet == SOCKET_ERROR)
            {
                int errorCode = WSAGetLastError();
                if (errorCode != WSAEWOULDBLOCK)
                {
                    fwprintf(logFile, L"Error : %d on %d line", errorCode, __LINE__);
                    DestroyWindow(hWnd);
                }
            }

            recvRet = g_RecvBuffer.Enqueue(buf, recvRet);
            if (recvRet == 0)
            {
                fwprintf(logFile, L"Error : RecvRingbuffer Overflow on %d line", __LINE__);
                DestroyWindow(hWnd);
                break;
            }

            if (g_RecvBuffer.Size() < sizeof(header))
            {
                break;
            }

            peekRet = g_RecvBuffer.Peek((char*)&header, sizeof(header));
            
            if (g_RecvBuffer.Size() < header.length)
            {
                break;
            }

            if (header.length != sizeof(packet))
            {
                //Invalid Header
                break;
            }

            g_RecvBuffer.Dequeue(peekRet);
            peekRet = g_RecvBuffer.Peek((char*)&packet, header.length);
            g_RecvBuffer.Dequeue(peekRet);

            HDC hdc = GetDC(hWnd);
            HPEN hPenOld = (HPEN)SelectObject(hdc, g_hPen);

            MoveToEx(hdc, packet.startX, packet.startY, NULL);
            LineTo(hdc, packet.endX, packet.endY);

            SelectObject(hdc, hPenOld);

            fwprintf(logFile, L"Draw From %d, %d To %d %d\n", packet.startX, packet.startY, packet.endX, packet.endY);

            break;
        }
    case FD_WRITE:
        {
            bSend = true;
            while (g_SendBuffer.Size() > 0)
            {
                char buf[512];
                size_t peekRet;
                int sendRet;

                peekRet = g_SendBuffer.Peek(buf, 512);

                sendRet = send(clientSocket, buf, (int)peekRet, 0);
                if (sendRet == SOCKET_ERROR)
                {
                    int errorCode = WSAGetLastError();
                    if (errorCode == WSAEWOULDBLOCK)
                    {
                        bSend = false;
                        break;
                    }
                    else 
                    {
                        fwprintf(logFile, L"Error : %d on %d line", errorCode, __LINE__);
                        bConnect = false;
                        DestroyWindow(hWnd);
                        break;
                    }
                }
                else
                {
                    g_SendBuffer.Dequeue(peekRet);
                }
            }
            break;
        }
    case FD_CONNECT:
        {
            fwprintf(logFile, L"Connected\n");
            bConnect = true;
            break;
        }
    case FD_CLOSE:
        {
            bConnect = false;
            closesocket(clientSocket);
            break;
        }
    }
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
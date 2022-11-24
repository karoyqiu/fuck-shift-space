// fuck-shift-space.cpp : 定义应用程序的入口点。
//

#include "pch.h"
#include "framework.h"
#include "fuck-shift-space.h"

#define MAX_LOADSTRING 100
#define HOTKEY_SHIFT_SPACE 0x33C4
#define HOTKEY_WIN_SPACE 0x33C5

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

// {4841AE59-5EE7-4D30-93DB-CAF2B872257B}
static const GUID GUID_ICON =
{ 0x4841ae59, 0x5ee7, 0x4d30, { 0x93, 0xdb, 0xca, 0xf2, 0xb8, 0x72, 0x25, 0x7b } };


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_FUCKSHIFTSPACE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FUCKSHIFTSPACE));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
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
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FUCKSHIFTSPACE));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_FUCKSHIFTSPACE);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    UNREFERENCED_PARAMETER(nCmdShow);

    hInst = hInstance; // 将实例句柄存储在全局变量中

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, HWND_MESSAGE, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    // 创建通知区域图标
    NOTIFYICONDATAW notifyData = { 0 };
    notifyData.cbSize = sizeof(notifyData);
    notifyData.hWnd = hWnd;
    notifyData.uFlags = NIF_MESSAGE | NIF_TIP | NIF_ICON | NIF_GUID;
    notifyData.uCallbackMessage = WM_USER;
    notifyData.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FUCKSHIFTSPACE));
    wcscpy_s(notifyData.szTip, szTitle);
    notifyData.guidItem = GUID_ICON;
    Shell_NotifyIconW(NIM_ADD, &notifyData);

    // 创建 Shift + 空格 热键
    if (RegisterHotKey(hWnd, HOTKEY_SHIFT_SPACE, MOD_SHIFT | MOD_NOREPEAT, 0x20))
    {
        OutputDebugStringW(L"Successfully registered Shift + Space global hotkey.\n");
    }

    if (RegisterHotKey(hWnd, HOTKEY_WIN_SPACE, MOD_WIN | MOD_NOREPEAT, 0x20))
    {
        OutputDebugStringW(L"Successfully registered Win + Space global hotkey.\n");
    }

    return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 分析菜单选择:
        switch (wmId)
        {
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_USER:
        if (lParam == WM_RBUTTONUP)
        {
            PostQuitMessage(0);
        }
        break;

    case WM_HOTKEY:
        switch (wParam)
        {
        case HOTKEY_SHIFT_SPACE:
        {
            // Shift + Space => Space + Shift
            INPUT inputs[5];
            ZeroMemory(inputs, sizeof(inputs));

            // shift up
            inputs[0].type = INPUT_KEYBOARD;
            inputs[0].ki.wVk = VK_SHIFT;
            inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;

            // space up
            inputs[1].type = INPUT_KEYBOARD;
            inputs[1].ki.wVk = VK_SPACE;
            inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

            // space down
            inputs[2].type = INPUT_KEYBOARD;
            inputs[2].ki.wVk = VK_SPACE;

            // space up
            inputs[3].type = INPUT_KEYBOARD;
            inputs[3].ki.wVk = VK_SPACE;
            inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

            // shift down
            inputs[4].type = INPUT_KEYBOARD;
            inputs[4].ki.wVk = VK_SHIFT;

            SendInput(_countof(inputs), inputs, sizeof(INPUT));
            break;
        }

        case HOTKEY_WIN_SPACE:
        {
            // Win + Space => Ctrl + Space
            INPUT inputs[6];
            ZeroMemory(inputs, sizeof(inputs));

            // win up
            inputs[0].type = INPUT_KEYBOARD;
            inputs[0].ki.wVk = VK_LWIN;
            inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;

            // space up
            inputs[1].type = INPUT_KEYBOARD;
            inputs[1].ki.wVk = VK_SPACE;
            inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

            // ctrl down
            inputs[2].type = INPUT_KEYBOARD;
            inputs[2].ki.wVk = VK_LCONTROL;

            // space down
            inputs[3].type = INPUT_KEYBOARD;
            inputs[3].ki.wVk = VK_SPACE;

            // space up
            inputs[4].type = INPUT_KEYBOARD;
            inputs[4].ki.wVk = VK_SPACE;
            inputs[4].ki.dwFlags = KEYEVENTF_KEYUP;

            // ctrl up
            inputs[5].type = INPUT_KEYBOARD;
            inputs[5].ki.wVk = VK_LCONTROL;
            inputs[5].ki.dwFlags = KEYEVENTF_KEYUP;

            SendInput(_countof(inputs), inputs, sizeof(INPUT));
            break;
        }

        default:
            break;
        }
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

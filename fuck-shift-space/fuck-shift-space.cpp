// fuck-shift-space.cpp : 定义应用程序的入口点。
//

#include "pch.h"
#include "framework.h"
#include "fuck-shift-space.h"

#include <unordered_map>
#include <unordered_set>

#define MAX_LOADSTRING      100
#define HOTKEY_SHIFT_SPACE  0x33C4
#define HOTKEY_WIN_SPACE    0x33C5
#define TIMER_MEMORY        1024

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
static ATOM                MyRegisterClass(HINSTANCE hInstance);
static BOOL                InitInstance(HINSTANCE, int);
static LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR    lpCmdLine,
                      _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。
    RegisterApplicationRestart(L"-r", 0);

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
static ATOM MyRegisterClass(HINSTANCE hInstance)
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
static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
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
    notifyData.uID = 1;
    notifyData.uFlags = NIF_MESSAGE | NIF_TIP | NIF_ICON;
    notifyData.uCallbackMessage = WM_USER;
    notifyData.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FUCKSHIFTSPACE));
    wcscpy_s(notifyData.szTip, szTitle);
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

    // 创建监控内存使用的计时器
    if (SetTimer(hWnd, TIMER_MEMORY, 3000, nullptr) == 0)
    {
        OutputDebugStringW(L"Failed to create memory watching timer.\n");
    }

    return TRUE;
}


static void Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    // 分析菜单选择:
    switch (id)
    {
    case IDM_EXIT:
        DestroyWindow(hwnd);
        break;
    default:
        FORWARD_WM_COMMAND(hwnd, id, hwndCtl, codeNotify, DefWindowProc);
        break;
    }
}


static void Cls_OnHotKey(HWND hwnd, int idHotKey, UINT fuModifiers, UINT vk)
{
    UNREFERENCED_PARAMETER(hwnd);
    UNREFERENCED_PARAMETER(fuModifiers);
    UNREFERENCED_PARAMETER(vk);

    switch (idHotKey)
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
}


static BOOL IsProcessExited(DWORD dwProcessID)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessID);

    if (hProcess)
    {
        DWORD dwExitCode = 0;

        if (GetExitCodeProcess(hProcess, &dwExitCode) && dwExitCode == STILL_ACTIVE)
        {
            CloseHandle(hProcess);
            return FALSE;
        }

        CloseHandle(hProcess);
    }

    return TRUE;
}


static void Cls_OnTimer(HWND hwnd, UINT id)
{
    UNREFERENCED_PARAMETER(hwnd);

    if (id != TIMER_MEMORY)
    {
        return;
    }

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnap == INVALID_HANDLE_VALUE)
    {
        return;
    }

    PROCESSENTRY32W pe32 = { 0 };
    pe32.dwSize = sizeof(pe32);

    if (!Process32FirstW(hSnap, &pe32))
    {
        return;
    }

    const auto now = GetTickCount64();
    static std::unordered_map<DWORD, ULONGLONG> firstSeen;
    std::unordered_set<DWORD> currentSeen;

    do
    {
        if (wcsstr(pe32.szExeFile, L"msedge.exe") != nullptr)
        {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);

            if (hProcess)
            {
                currentSeen.insert(pe32.th32ProcessID);
                auto iter = firstSeen.find(pe32.th32ProcessID);

                static const SIZE_T MAX_MEMORY = 1024 * 1024 * 1024;
                PROCESS_MEMORY_COUNTERS mem = { 0 };

                if (GetProcessMemoryInfo(hProcess, &mem, sizeof(mem)) && mem.PagefileUsage >= MAX_MEMORY)
                {
                    if (iter == firstSeen.end())
                    {
                        firstSeen.emplace(pe32.th32ProcessID, now);
                    }
                    else if (now - iter->second >= 10000)
                    {
                        TerminateProcess(hProcess, 0);
                        firstSeen.erase(iter);
                    }
                }
                else if (iter != firstSeen.end())
                {
                    firstSeen.erase(iter);
                }

                CloseHandle(hProcess);
            }
        }
        else if (wcsstr(pe32.szExeFile, L"yundetectservice.exe") != nullptr)
        {
            if (IsProcessExited(pe32.th32ParentProcessID))
            {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);

                if (hProcess)
                {
                    TerminateProcess(hProcess, 0);
                    CloseHandle(hProcess);
                }
            }
        }
    } while (Process32NextW(hSnap, &pe32));

    CloseHandle(hSnap);

    for (auto iter = firstSeen.begin(); iter != firstSeen.end();)
    {
        if (currentSeen.count(iter->first) == 0)
        {
            iter = firstSeen.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
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
static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        HANDLE_MSG(hWnd, WM_COMMAND, Cls_OnCommand);
        HANDLE_MSG(hWnd, WM_HOTKEY, Cls_OnHotKey);
        HANDLE_MSG(hWnd, WM_TIMER, Cls_OnTimer);

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_USER:
        if (lParam == WM_RBUTTONUP)
        {
            PostQuitMessage(0);
        }
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

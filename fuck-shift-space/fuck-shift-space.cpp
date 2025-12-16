// fuck-shift-space.cpp : 定义应用程序的入口点。
//

#include "pch.h"
#include "framework.h"
#include "fuck-shift-space.h"

#define MAX_LOADSTRING      100
#define HOTKEY_SHIFT_SPACE  0x33C4
#define HOTKEY_WIN_SPACE    0x33C5
#define TIMER_MEMORY        1024
#define TIMER_MEMORY_INTERVAL   3000

#define IMC_GETCONVERSIONMODE   0x0001
#define IMC_SETCONVERSIONMODE   0x0002

// 全局变量:
static HINSTANCE hInst;                                // 当前实例
static WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
static WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
static std::unordered_set<std::wstring> chineseApps;
static std::unordered_set<std::wstring> englishApps;
static HWINEVENTHOOK hEventHook = nullptr;

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


static void StringArrayToSet(LPCWSTR lpStringArray, std::unordered_set<std::wstring> *set)
{
    set->clear();
    LPCWSTR p = lpStringArray;

    while (*p != L'\0')
    {
        set->emplace(p);
        p += wcslen(p) + 1;
    }
}


static void LoadEnglishApps()
{
    WCHAR wszBuffer[1024] = { 0 };
    DWORD cbData = sizeof(wszBuffer);
    auto lResult = RegGetValueW(HKEY_CURRENT_USER, LR"(Software\karoyqiu\fuck-shift-space)", L"ChineseApps", RRF_RT_REG_MULTI_SZ, nullptr, &wszBuffer, &cbData);

    if (lResult == ERROR_SUCCESS)
    {
        StringArrayToSet(wszBuffer, &chineseApps);
    }

    cbData = sizeof(wszBuffer);
    lResult = RegGetValueW(HKEY_CURRENT_USER, LR"(Software\karoyqiu\fuck-shift-space)", L"EnglishApps", RRF_RT_REG_MULTI_SZ, nullptr, &wszBuffer, &cbData);

    if (lResult == ERROR_SUCCESS)
    {
        StringArrayToSet(wszBuffer, &englishApps);
    }
}

static void SwitchToEnglish(HWND hwnd, BOOL bEnglish)
{
    auto hIME = ImmGetDefaultIMEWnd(hwnd);
    Sleep(40);
    SendMessageW(hIME, WM_IME_CONTROL, IMC_SETCONVERSIONMODE, bEnglish ? 0 : 1025);
}


static void WinEventProc(HWINEVENTHOOK hWinEventHook,
                         DWORD event,
                         HWND hwnd,
                         LONG idObject,
                         LONG idChild,
                         DWORD idEventThread,
                         DWORD dwmsEventTime)
{
    UNREFERENCED_PARAMETER(hWinEventHook);
    UNREFERENCED_PARAMETER(event);
    UNREFERENCED_PARAMETER(idObject);
    UNREFERENCED_PARAMETER(idChild);
    UNREFERENCED_PARAMETER(idEventThread);
    UNREFERENCED_PARAMETER(dwmsEventTime);

    DWORD dwProcessID = 0;
    GetWindowThreadProcessId(hwnd, &dwProcessID);

    auto hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessID);

    if (hProcess == nullptr)
    {
        return;
    }

    WCHAR wszFilename[MAX_PATH] = { 0 };
    GetProcessImageFileNameW(hProcess, wszFilename, _countof(wszFilename));
    CloseHandle(hProcess);

    _wcslwr_s(wszFilename);
    const auto *p = wcsrchr(wszFilename, L'\\') + 1;
    std::wstring s(p);

    if (englishApps.find(p) != englishApps.end())
    {
        SwitchToEnglish(hwnd, TRUE);
    }
    else if (chineseApps.find(p) != chineseApps.end())
    {
        SwitchToEnglish(hwnd, FALSE);
    }
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
    if (SetTimer(hWnd, TIMER_MEMORY, TIMER_MEMORY_INTERVAL, nullptr) == 0)
    {
        OutputDebugStringW(L"Failed to create memory watching timer.\n");
    }

    LoadEnglishApps();
    hEventHook = SetWinEventHook(EVENT_OBJECT_FOCUS, EVENT_OBJECT_FOCUS, nullptr, WinEventProc, 0, 0,
                                 WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

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


static SIZE_T GetMemoryLimit()
{
    static const SIZE_T MB = 1024 * 1024;
    static const SIZE_T MAX_MEMORY = 2 * 1024 * MB;

    HKEY hKey = nullptr;
    SIZE_T nLimit = MAX_MEMORY;

    __try
    {
        auto lResult = RegOpenKeyExW(HKEY_CURRENT_USER, LR"(Software\karoyqiu\fuck-shift-space)", 0, KEY_READ, &hKey);

        if (lResult != ERROR_SUCCESS)
        {
            __leave;
        }

        DWORD dwData = 0;
        DWORD cbData = sizeof(dwData);
        lResult = RegQueryValueExW(hKey, L"MemoryLimitMB", nullptr, nullptr, (LPBYTE)&dwData, &cbData);

        if (lResult != ERROR_SUCCESS)
        {
            nLimit = MAX_MEMORY;
        }
        else
        {
            nLimit = dwData * MB;
        }
    }
    __finally
    {
        if (hKey)
        {
            RegCloseKey(hKey);
        }
    }

    return nLimit;
}


static void KillSomeApp()
{
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
        _wcslwr_s(pe32.szExeFile);

        if (wcsstr(pe32.szExeFile, L"msedge.exe") != nullptr)
        {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);

            if (hProcess)
            {
                currentSeen.insert(pe32.th32ProcessID);
                auto iter = firstSeen.find(pe32.th32ProcessID);

                const auto nLimit = GetMemoryLimit();
                PROCESS_MEMORY_COUNTERS mem = { 0 };

                if (GetProcessMemoryInfo(hProcess, &mem, sizeof(mem)) && mem.PagefileUsage >= nLimit)
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


static void RemoveDirectoryRecursively(LPCWSTR lpszDir)
{
    WCHAR wszFind[MAX_PATH] = { 0 };
    wcscpy_s(wszFind, lpszDir);
    wcscat_s(wszFind, L"\\*");

    WIN32_FIND_DATAW data = { 0 };
    auto hFind = FindFirstFileW(wszFind, &data);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (wcscmp(data.cFileName, L".") == 0 || wcscmp(data.cFileName, L"..") == 0)
            {
                continue;
            }

            WCHAR wszName[MAX_PATH] = { 0 };
            wcscpy_s(wszName, lpszDir);
            wcscat_s(wszName, L"\\");
            wcscat_s(wszName, data.cFileName);

            if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                RemoveDirectoryRecursively(wszName);
            }
            else
            {
                DeleteFileW(wszName);
            }
        } while (FindNextFileW(hFind, &data));

        FindClose(hFind);
    }
}


static void ClearTempDir()
{
    ULARGE_INTEGER ulFree = { 0 };
    ULARGE_INTEGER ulTotal = { 0 };

    if (!GetDiskFreeSpaceExW(L"R:\\", nullptr, &ulTotal, &ulFree))
    {
        return;
    }

    auto nMinimum = ulTotal.QuadPart / 10;

    if (ulFree.QuadPart < nMinimum)
    {
        RemoveDirectoryRecursively(L"R:\\temp");
    }
}


static void Cls_OnTimer(HWND hWnd, UINT id)
{
    if (id != TIMER_MEMORY)
    {
        return;
    }

    KillTimer(hWnd, TIMER_MEMORY);

    KillSomeApp();
    ClearTempDir();

    SetTimer(hWnd, TIMER_MEMORY, TIMER_MEMORY_INTERVAL, nullptr);
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
        if (hEventHook)
        {
            UnhookWinEvent(hEventHook);
            hEventHook = nullptr;
        }

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

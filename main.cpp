#include <windows.h>
#include <shellapi.h>
#include <winreg.h>
#include "resource.h"
#include "lang/switch.h"
#include <functional>
#include <iostream>
#include <io.h>
#include <fcntl.h>

HHOOK g_keyboardHook = NULL;
HWND g_hMainWnd = NULL;
bool g_isHookActive = true;
bool g_isRunOnStartup = false;
bool g_lshiftDown = false;
bool g_rshiftDown = false;
bool g_doubleshiftDown = false;

const WCHAR* REG_RUN_KEY = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
const WCHAR* APP_NAME = L"CapsLockLanguageSwitcher";

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CreateTrayIcon(HWND hWnd);
void RemoveTrayIcon(HWND hWnd);
void ShowContextMenu(HWND hWnd);
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
void ToggleStartupRegistration();
bool IsAppInStartup();

void CreateDebugConsole() {
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    std::cout << "Debug Console Initialized..." << std::endl;
}

void ForceCapsLockOff() {
    if (GetKeyState(VK_CAPITAL) & 0x0001) {
        INPUT inputs[2] = {};
        inputs[0].type = INPUT_KEYBOARD;
        inputs[0].ki.wVk = VK_CAPITAL;
        inputs[0].ki.dwFlags = 0;

        inputs[1].type = INPUT_KEYBOARD;
        inputs[1].ki.wVk = VK_CAPITAL;
        inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

        SendInput(2, inputs, (int)sizeof(INPUT));
    }
}

void CreateTrayIcon(HWND hWnd) {
    NOTIFYICONDATA nid = {};
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = ID_TRAY_ICON;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    // Receive icon events through this message
    nid.uCallbackMessage = WM_TRAY_ICON_MSG; 

    nid.hIcon = (HICON)LoadImage(
        GetModuleHandle(NULL),
        MAKEINTRESOURCE(IDI_APPICON),
        IMAGE_ICON,
        GetSystemMetrics(SM_CXSMICON),
        GetSystemMetrics(SM_CYSMICON),
        LR_DEFAULTCOLOR
    );

    wcscpy_s(nid.szTip, L"CapsLock 한/영 전환");

    Shell_NotifyIcon(NIM_ADD, &nid);
}


void RemoveTrayIcon(HWND hWnd) {
    NOTIFYICONDATA nid = {};
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = ID_TRAY_ICON;
    Shell_NotifyIcon(NIM_DELETE, &nid);
}

void ShowContextMenu(HWND hWnd) {
    // Gets the current mouse cursor position
    POINT pt;
    GetCursorPos(&pt); 

    HMENU hMenu = CreatePopupMenu();
    if (hMenu) {
        UINT toggle_flags = MF_BYPOSITION | MF_STRING;
        if (g_isHookActive) {
            toggle_flags |= MF_CHECKED;
        } 
        InsertMenu(hMenu, -1, toggle_flags, ID_MENU_TOGGLE, L"Enable");

        UINT startup_flags = MF_BYPOSITION | MF_STRING;
        if (g_isRunOnStartup) {
            startup_flags |=MF_CHECKED;
        }
        InsertMenu(hMenu, -1, startup_flags, ID_MENU_STARTUP, L"Run at Startup");
        InsertMenu(hMenu, -1, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
        InsertMenu(hMenu, -1, MF_BYPOSITION | MF_STRING, ID_MENU_EXIT, L"Exit");

		// Set the foreground window to ensure the menu is active
        SetForegroundWindow(hWnd);

		// Displays the context menu and loops for user selection or exit operation
        TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, NULL);
        PostMessage(hWnd, WM_NULL, 0, 0);
        DestroyMenu(hMenu);
    }
    
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if ((g_isHookActive && nCode == HC_ACTION)) {
        bool keydown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        bool keyup = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);
        KBDLLHOOKSTRUCT* pkbhs = (KBDLLHOOKSTRUCT*)lParam;
        switch (pkbhs->vkCode) {
            case VK_CAPITAL:
                if (keyup) {
                    PostMessage(g_hMainWnd, WM_TRIGGER_LANG_CHANGE, 0, 0);
                }
                return 1;
            case VK_LSHIFT:
            case VK_RSHIFT: {
                //Store shift down states
                if (pkbhs->vkCode == VK_LSHIFT) g_lshiftDown = keydown;
                if (pkbhs->vkCode == VK_RSHIFT) g_rshiftDown = keydown;
				
                // If both shift keys are down set the next flag for keyboard switch
                if (g_lshiftDown && g_rshiftDown) {
                    g_doubleshiftDown = true;
                    return 1; 
                }

                // Trigger on release of either key
                if (keyup && g_doubleshiftDown) {
					PostMessage(g_hMainWnd, WM_TRIGGER_SWITCH_STATE, 0, 0);
                    g_doubleshiftDown = false; 
                    return 1; 
                }

                if (!g_lshiftDown && !g_rshiftDown) {
                    g_doubleshiftDown = false;
                }
                break;
            }
        }
    }
    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}


LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
		// Custom messages re-routes main handle messages to foreground messages
        case WM_TRIGGER_LANG_CHANGE:
            LanguageChange(); 
            return 0;
        case WM_TRIGGER_SWITCH_STATE:
            SwitchKeyboardState(); 
            return 0;
        case WM_TRAY_ICON_MSG:
            switch(lParam) {
				// Show context menu on right-click
                case WM_RBUTTONUP:
                    ShowContextMenu(hWnd);
                    break;
            }
            return 0;
        case WM_COMMAND:
            // Looks for resource identifer in command message
            switch (LOWORD(wParam)) {
                case ID_MENU_TOGGLE:
                    g_isHookActive = !g_isHookActive;
                    break;
                case ID_MENU_STARTUP:
                    ToggleStartupRegistration();
                    break;
                // Quits application on close command
                case ID_MENU_EXIT:
                    DestroyWindow(hWnd);
                    break;
            }
            return 0;
        case WM_DESTROY:   
            // Quits on destroy message
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool IsAppInStartup() {
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, REG_RUN_KEY, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
        return false;
    }

    WCHAR szPath[MAX_PATH];
    DWORD dwSize = sizeof(szPath);
    bool bResult = (RegQueryValueEx(hKey, APP_NAME, NULL, NULL, (LPBYTE)szPath, &dwSize) == ERROR_SUCCESS);

    RegCloseKey(hKey);
    return bResult;
}

void ToggleStartupRegistration() {
    g_isRunOnStartup = !g_isRunOnStartup;

    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, REG_RUN_KEY, 0, KEY_WRITE, &hKey) != ERROR_SUCCESS) {
        return;
    }

    if (g_isRunOnStartup) {
        WCHAR szPath[MAX_PATH];
        GetModuleFileName(NULL, szPath, MAX_PATH);
        RegSetValueEx(hKey, APP_NAME, 0, REG_SZ, (LPBYTE)szPath, (DWORD)(wcslen(szPath) + 1) * sizeof(WCHAR));
    } else {
        RegDeleteValue(hKey, APP_NAME);
    }

    RegCloseKey(hKey);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const WCHAR CLASS_NAME[] = L"CapsLock Language Switcher Class";
	ForceCapsLockOff();
    //CreateDebugConsole();
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    g_hMainWnd = CreateWindowEx(
                    0, 
                    CLASS_NAME, 
                    L"CapsLock Language Switcher", 
                    WS_OVERLAPPEDWINDOW, 
                    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
                    NULL, 
                    NULL, 
                    hInstance, 
                    NULL);

    if (g_hMainWnd == NULL) {
        return 0;
    }
    // Startup Window Check from Windows Registry
    g_isRunOnStartup = IsAppInStartup();

    // Global hook, applied to all threads
    g_keyboardHook = SetWindowsHookEx(
                        WH_KEYBOARD_LL, 
                        LowLevelKeyboardProc, 
                        hInstance, 
                        0); 

    CreateTrayIcon(g_hMainWnd);

    // Message processing pump
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(g_keyboardHook);
    RemoveTrayIcon(g_hMainWnd);
    
    return (int)msg.wParam;
}
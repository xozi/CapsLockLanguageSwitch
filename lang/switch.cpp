#include <windows.h>
#include <ime.h>
#pragma comment(lib, "imm32.lib")

#include "switch.h"

// Language-specific headers
#include "en_us.h"
EN_US en_us;
#include "ja.h"
JA ja;
#include "kr.h"
KR kr;

#include <iostream>

void LanguageChange() {
    HWND hWnd = GetForegroundWindow();
    if (hWnd == NULL) return;
    PostMessage(hWnd, WM_INPUTLANGCHANGEREQUEST, INPUTLANGCHANGE_FORWARD, (LPARAM)HKL_NEXT);
}

WORD GetLangIdFromLocaleName(LPCWSTR localeName) {
    LCID lcid = LocaleNameToLCID(localeName, LOCALE_ALLOW_NEUTRAL_NAMES);
    return LANGIDFROMLCID(lcid);
}

int GetLanguageState() {
    HWND hWnd = GetForegroundWindow();
    DWORD threadId = GetWindowThreadProcessId(hWnd, NULL);
    HKL currentHkl = GetKeyboardLayout(threadId);
    WORD currentLangId = LOWORD(currentHkl);

    if (currentLangId == GetLangIdFromLocaleName(L"en-US")) return 1;
    if (currentLangId == GetLangIdFromLocaleName(L"ja-JP")) return 2;
    if (currentLangId == GetLangIdFromLocaleName(L"ko-KR")) return 3;
	return 0;
}

DWORD IME_State() {
    HWND hWnd = GetForegroundWindow();
    if (hWnd == NULL) return MAXDWORD;
    HWND hImeWnd = ImmGetDefaultIMEWnd(hWnd);
    if (hImeWnd == NULL) return MAXDWORD;
    //Check if IME is open
    if (SendMessage(hImeWnd, WM_IME_CONTROL, 0x0005, 0) != 0) {
        return SendMessage(hImeWnd, WM_IME_CONTROL, IMC_GETCONVERSIONMODE, 0);
    }
    else {
		return MAXDWORD;
    }
}

void SwitchKeyboardState() {
    switch (GetLanguageState()) {
        case 2:
            ja.SwitchToNextFromCurrent();
            break;
        case 3:
            kr.SwitchToNextFromCurrent();
		    break;
        default:
            break;
    }
}
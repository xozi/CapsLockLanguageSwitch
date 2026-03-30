#include "switch.h"
#include "kr.h"

void KR::SwitchToNextFromCurrent() {
    HWND hWnd = GetForegroundWindow();
    if (hWnd == NULL) return;

    HWND hImeWnd = ImmGetDefaultIMEWnd(hWnd);
    if (hImeWnd == NULL) return;

    LRESULT isOpen = SendMessage(hImeWnd, WM_IME_CONTROL, IMC_GETOPENSTATUS, 0);

	// Toggles between open and closed status (Hangul and Alphanumeric)
    int nextState = (isOpen != 0) ? 0 : 1;

    SendMessage(hImeWnd, WM_IME_CONTROL, IMC_SETOPENSTATUS, (LPARAM)nextState);

    DWORD convMode = (nextState == 1) ? IME_CMODE_NATIVE : IME_CMODE_ALPHANUMERIC;
    SendMessage(hImeWnd, WM_IME_CONTROL, IMC_SETCONVERSIONMODE, (LPARAM)convMode);
}
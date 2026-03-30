#include "switch.h"
#include "ja.h"

#include <iostream>

JA::JA() {
    // Default to Hiragana
    state = 3;
}

//Personal limit, I never use full-width alphanumeric or half-width katakana, adjust to preference.
const int state_limit = 3;

void JA::GetStateFromMap(DWORD dw) {
    if (dw == MAXDWORD) {
        // Direct input / IME closed is considered Alphanumeric
        state = 1; 
        return;
    }
    DWORD cleanDw = dw & (IME_CMODE_LANGUAGE | IME_CMODE_KATAKANA | IME_CMODE_FULLSHAPE | IME_CMODE_ROMAN);
    switch (cleanDw) {
        case 0:  state = 1; break; // Half Alpha
        case 25: state = 2; break; // Hiragana (0x19)
        case 27: state = 3; break; // Full Katakana (0x1B)
        case 24: state = 4; break; // Full Alpha (0x18)
        case 19: state = 5; break; // Half Katakana (0x13)
        default:
            return;
    }
}

void JA::StateIterate() {
	state = (state % state_limit) + 1;
}

void JA::SwitchKeyboardToState() {
    HWND hWnd = GetForegroundWindow();
    if (hWnd == NULL) return;
    HWND hImeWnd = ImmGetDefaultIMEWnd(hWnd);
    if (hImeWnd == NULL) return;

	DWORD conversionMode = StateMaps[state];
    DWORD sentenceMode = IME_SMODE_PHRASEPREDICT;
    // Conversion
    SendMessage(hImeWnd, WM_IME_CONTROL, IMC_SETCONVERSIONMODE, (LPARAM)conversionMode);
    // Force Open Status to update the IME mode immediately.
    // This forces alphanumeric half-width to run in Direct Input Mode.
    BOOL openStatus = (conversionMode != 0); // Open if not Alphanumeric
    SendMessage(hImeWnd, WM_IME_CONTROL, IMC_SETOPENSTATUS, (LPARAM)openStatus);
}

void JA::SwitchToNextFromCurrent() {
    GetStateFromMap(IME_State());
    StateIterate();
    SwitchKeyboardToState();
}
#pragma once
#include <windows.h>

class JA {
public:
    JA();
    int state;

    //LUT for Japanese 
    static constexpr DWORD StateMaps[6] = {
            // Index 0: Fallback (Hiragana Default)
            IME_CMODE_NATIVE | IME_CMODE_FULLSHAPE | IME_CMODE_ROMAN,

            // Index 1: Half-width Alphanumeric (A)
            IME_CMODE_ALPHANUMERIC,

            // Index 2: Hiragana (Ç†)
            IME_CMODE_NATIVE | IME_CMODE_FULLSHAPE | IME_CMODE_ROMAN,

            // Index 3: Full-width Katakana (ÉA)
            IME_CMODE_NATIVE | IME_CMODE_KATAKANA | IME_CMODE_FULLSHAPE | IME_CMODE_ROMAN,

            // Index 4: Full-width Alphanumeric (Ç`)
            IME_CMODE_ALPHANUMERIC | IME_CMODE_FULLSHAPE | IME_CMODE_ROMAN,

            // Index 5: Half-width Katakana (_±)
            IME_CMODE_NATIVE | IME_CMODE_KATAKANA | IME_CMODE_ROMAN
    };
	void GetStateFromMap(DWORD dw);
	void StateIterate();
    void SwitchKeyboardToState();
	void SwitchToNextFromCurrent();
};



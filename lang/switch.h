#pragma once
#include <windows.h>

#define WM_TRIGGER_LANG_CHANGE  (WM_APP + 1)
#define WM_TRIGGER_SWITCH_STATE (WM_APP + 2)

#ifndef IMC_GETOPENSTATUS
	#define IMC_GETOPENSTATUS 0x0005
#endif

#ifndef IMC_SETOPENSTATUS
	#define IMC_SETOPENSTATUS 0x0006
#endif

#ifndef IMC_GETCONVERSIONMODE
	#define IMC_GETCONVERSIONMODE 0x0001
#endif

#ifndef IMC_SETCONVERSIONMODE
	#define IMC_SETCONVERSIONMODE 0x0002
#endif



void LanguageChange();
int GetLanguageState();
DWORD IME_State();
void SwitchKeyboardState();

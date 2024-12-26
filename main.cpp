﻿#include "pch.h"
#include <iostream>
#include <windows.h>
#include "ui_utilities.h"

//using namespace Windows::Foundation;
static bool isWindowHidden = true;
static HWND wezHwnd;

// The callback function for the WH_KEYBOARD_LL hook
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	//The wParam and lParam parameters contain information about a keyboard message. (https://learn.microsoft.com/en-us/windows/win32/winmsg/lowlevelkeyboardproc)	
	if (nCode == HC_ACTION) {
		PKBDLLHOOKSTRUCT keypress = (PKBDLLHOOKSTRUCT)lParam;
		// Get the dimension of the monitor
		DimensionStruct monitorDim = GetMonitorDimension(wezHwnd);

		// Get the dimension of the window based on the percentage of the monitor
		WindowDimensionStruct winDim = GetWinDimensionByPercent(monitorDim, 100, 50);
		switch (wParam){
			// The WM_KEYDOWN message is posted to the window with the keyboard focus when a nonsystem key is pressed. A nonsystem key is a key that is pressed when the ALT key is not pressed.
			case WM_KEYDOWN:
				if (keypress->vkCode == VK_F1) {
					wcout << L"F1 Pressed" << endl;

					if (isWindowHidden) {
						// Set the window position and 
						SetWindowPos(wezHwnd, HWND_TOPMOST, 0, 0, winDim.width, winDim.height, SWP_SHOWWINDOW);
						isWindowHidden = false;
					}
					else {
						// Hide the window
						SetWindowPos(wezHwnd, HWND_TOPMOST, 0, 0, winDim.width, winDim.height, SWP_HIDEWINDOW);
						isWindowHidden = true;
					}

				}
				if (wezHwnd == nullptr) {
					wcerr << L"Error: Window not found" << endl;
					return 1;
				}

				break;
			case WM_SYSKEYDOWN:
				break;
			case WM_KEYUP:
				break;
			case WM_SYSKEYUP:
				break;
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}


int main() {
	//PrintWindows(true);
	//PrintDesktopWindows(true);
	
	wezHwnd = SearchWindow(L"org.wezfurlong.wezterm", NULL);



	//WH_KEYBOARD_LL hook
	HHOOK hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);

	// Keep this app running until we're told to stop
	MSG msg;
	while (!GetMessage(&msg, NULL, NULL, NULL)) {    //this while loop keeps the hook
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(hHook);

	return 0;
}

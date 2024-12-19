#include "pch.h"
#include <windows.h>
#include <iostream>
#include <string>
using namespace std;
using namespace winrt;
//using namespace Windows::Foundation;


// Print window information, bool for debug print all including hidden windows
void PrintWindowInfo(HWND hwnd, std::wstring startingText, bool printAll) {
	// Get the window title
	wchar_t title[256];
	GetWindowTextW(hwnd, title, sizeof(title) / sizeof(wchar_t));
	// Get the process ID
	DWORD dwProccess_id;
	GetWindowThreadProcessId(hwnd, &dwProccess_id);
	// Get the window class name
	wchar_t windowClassName[256];
	GetClassNameW(hwnd, windowClassName, sizeof(windowClassName) / sizeof(wchar_t));

	if (printAll == true) {
		std::wcout << startingText << hwnd
			<< L",\n Title: " << title
			<< L",\n windowClassName: " << windowClassName
			<< L",\n ProcessID: " << dwProccess_id
			<< "\n\n\n" << std::endl;
	}
	else {
		// only print the simple info
		std::wcout << startingText << hwnd
			<< L", \n Title: " << title
			<< L",\n windowClassName: " << windowClassName
			<< "\n" << std::endl;
	}
}

// Callback function that is called for each child window
BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam) {
	PrintWindowInfo(hwnd, L"Child window: ", false);
	return TRUE; // Continue enumeration
}

// Callback function that is called for each top-level window
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
	bool includeChildWindows = *reinterpret_cast<bool*>(lParam);

	PrintWindowInfo(hwnd, L"Top-level window: ", true);

	// Enumerate child windows of the current top-level window if includeChildWindows is true
	if (includeChildWindows) {
		EnumChildWindows(hwnd, EnumChildProc, 0);
	}
	return TRUE; // Continue enumeration
}


void PrintWindows(bool includeChildWindows) {
	// Enumerate all top-level windows
	EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&includeChildWindows));
	std::wcout << L"Finished listing windows." << std::endl;
}

// Callback function that is called for each top-level window
BOOL CALLBACK EnumDesktopWindowsProc(HWND hwnd, LPARAM lParam) {

	PrintWindowInfo(hwnd, L"Top-level window: ", true);

	return TRUE; // Continue enumeration
}

// Print all windows on the desktop
void PrintDesktopWindows(bool includeChildWindows) {
	// Enumerate all top-level windows
	EnumDesktopWindows(NULL, EnumDesktopWindowsProc, NULL);
	std::wcout << L"Finished listing windows." << std::endl;
}

// if only one param needed set NULL for the other
HWND SearchWindow(wstring windowClassName, wstring windowTitleName) {
	// Find the window with the title "Calculator"
	HWND parentHwnd = FindWindowW(windowClassName.c_str(), windowTitleName.c_str());
	PrintWindowInfo(parentHwnd, L"Found window: ", true);
	if (parentHwnd != NULL) {
		std::wcout << L"Found window with classname " << windowClassName << L" and title: " << windowTitleName << std::endl;
		PrintWindowInfo(parentHwnd, L"Found window: ", true);
		return parentHwnd;
	}
	else {
		std::wcout << L"Cannot find window with classname " << windowClassName << L" and title: " << windowTitleName << std::endl;
		return NULL;
	}
	
}

int main() {
	//PrintWindows(true);
	PrintDesktopWindows(true);
	HWND wezHwnd = SearchWindow(L"org.wezfurlong.wezterm", L"user");
	std::cin.get(); // Pause console

	return 0;
}

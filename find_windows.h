#pragma once

#include <windows.h>
#include <iostream>
#include <string>

using namespace std;
using namespace winrt;

// Function declarations
void PrintWindowInfo(HWND hwnd, wstring startingText, bool printAll);
BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam);
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
void PrintWindows(bool includeChildWindows);
BOOL CALLBACK EnumDesktopWindowsProc(HWND hwnd, LPARAM lParam);
void PrintDesktopWindows(bool includeChildWindows);
HWND SearchWindow(LPCWSTR windowClassName, LPCWSTR windowTitleName);

// Function definitions
void PrintWindowInfo(HWND hwnd, wstring startingText, bool printAll) {
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
        wcout << startingText << hwnd
            << L",\n Title: " << title
            << L",\n windowClassName: " << windowClassName
            << L",\n ProcessID: " << dwProccess_id
            << L"\n\n\n" << endl;
    }
    else {
        // only print the simple info
        wcout << startingText << hwnd
            << L", \n Title: " << title
            << L",\n windowClassName: " << windowClassName
            << L"\n" << endl;
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
    wcout << L"Finished listing windows." << endl;
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
    wcout << L"Finished listing windows." << endl;
}

// if only one param needed set NULL for the other
HWND SearchWindow(LPCWSTR windowClassName, LPCWSTR windowTitleName) {
    // Find the window with the title "Calculator"
    HWND parentHwnd = FindWindowW(windowClassName, windowTitleName);
	windowClassName = (windowClassName != NULL) ? windowClassName : L"";
	windowTitleName = (windowTitleName != NULL) ? windowTitleName : L"";
    PrintWindowInfo(parentHwnd, L"Found window: ", true);
    if (parentHwnd != NULL) {
        wcout << L"Found window with classname " << windowClassName << L" and title: " << windowTitleName << endl;
        PrintWindowInfo(parentHwnd, L"Found window: ", true);
        return parentHwnd;
    }
    else {
        wcerr << L"Cannot find window with classname " << windowClassName << L" and title: " << windowTitleName << endl;
        return NULL;
    }
}

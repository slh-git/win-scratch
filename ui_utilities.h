#pragma once

#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

// Function declarations
void PrintWindowInfo(HWND hwnd, wstring startingText, bool printAll);
BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam);
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
void PrintWindows(bool includeChildWindows);
BOOL CALLBACK EnumDesktopWindowsProc(HWND hwnd, LPARAM lParam);
void PrintDesktopWindows(bool includeChildWindows);
HWND SearchWindow(LPCWSTR windowClassName, LPCWSTR windowTitleName);
struct DimensionStruct;
struct WindowDimensionStruct;
DimensionStruct GetMonitorDimension(HWND hwnd);
WindowDimensionStruct GetWinDimensionByPercent(DimensionStruct monitorDim, int widthPercentage, int heightPercentage);


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

// Print all windows on the desktop for 
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


// Dimension Struct
struct DimensionStruct {
    int width;
    int height;
    void Print() {
        wcout << L"Dimension Width: " << width << L" Dimension Height: " << height << endl;
    }
};

// Dimension Struct but with x and y location of where to scratchpad window will be placed
struct WindowDimensionStruct {
    int x;
    int y;
    int width;
    int height;
    WindowDimensionStruct() {
        x = 0;
        y = 0;
        width = 200;
        height = 200;
    }
    WindowDimensionStruct(int x, int y, int width, int height) {
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
    }
    // Constructor to set the width and height of the window
    WindowDimensionStruct(int width, int height) {
        x = 0;
        y = 0;
        this->width = width;
        this->height = height;
    }
    void Print() {
        wcout << L"Window Position: (" << x << L", " << y << L") "
            << L"Dimension Width: " << width << L" Dimension Height: " << height << endl;
    }
};

// Get the dimension of the monitor
DimensionStruct GetMonitorDimensionPixel(HWND hwnd) {
    HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

    MONITORINFO mi;
    mi.cbSize = sizeof(mi);

    DimensionStruct dim;

    if (GetMonitorInfoW(hMonitor, &mi) != 0) {
        dim.width = abs(mi.rcMonitor.right - mi.rcMonitor.left);
        dim.height = abs(mi.rcMonitor.bottom - mi.rcMonitor.top);
    }
    else {
        wcerr << L"Error finding MonitorInfoW" << endl;
        dim.width = 0;
        dim.height = 0;
    }
    return dim;
}

// Get the dimension of the window based on the percentage of the monitor
WindowDimensionStruct GetWinDimensionPixels(DimensionStruct monitorDim, int widthPercentage, int heightPercentage) {
    return WindowDimensionStruct(
        monitorDim.width * widthPercentage / 100,
        monitorDim.height * heightPercentage / 100);
}

WindowDimensionStruct GetWindowDimensionPercent(DimensionStruct monitorDim, int windowWidthPx, int windowHeightPx) {
	return WindowDimensionStruct(
		windowWidthPx * 100 / monitorDim.width,
		windowHeightPx * 100 / monitorDim.height);
}
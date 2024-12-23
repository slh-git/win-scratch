#include "pch.h"
#include "find_windows.h"
#include <iostream>
#include <windows.h>

//using namespace Windows::Foundation;

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
DimensionStruct GetMonitorDimension(HWND hwnd) {
	HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

	MONITORINFO mi;
	mi.cbSize = sizeof(mi);

	DimensionStruct dim;

	if(GetMonitorInfoW(hMonitor, &mi) !=0) {
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
WindowDimensionStruct GetWinDimensionByPercent(DimensionStruct monitorDim, int widthPercentage, int heightPercentage) {
	return WindowDimensionStruct(
		monitorDim.width * widthPercentage / 100 , 
		monitorDim.height * heightPercentage / 100);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
int main() {
	//PrintWindows(true);
	PrintDesktopWindows(true);
	HWND wezHwnd = SearchWindow(L"org.wezfurlong.wezterm", NULL);
	if (wezHwnd == nullptr) {
		wcerr << L"Error: Window not found" << endl;
		return 1;
	}
	DimensionStruct monitorDim = GetMonitorDimension(wezHwnd);
	monitorDim.Print();

	WindowDimensionStruct winDim = GetWinDimensionByPercent(monitorDim, 100, 50);
	std::wcout << L"Percentage w and h" << winDim.width << winDim.height << endl;



	//Works TODO automate windows 
	SetWindowPos(wezHwnd, HWND_TOPMOST, 0, 0, winDim.width, winDim.height, SWP_SHOWWINDOW);
	Sleep(200);
	SetWindowPos(wezHwnd, HWND_TOPMOST, 0, 0, winDim.width, winDim.height, SWP_HIDEWINDOW);
	std::cin.get(); // Pause console

	return 0;
}

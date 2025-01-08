#include "pch.h"
#include "ui_utilities.h"
#include "toml++/toml.hpp"
#include "vk_codes.h"
#include <thread>
//using namespace Windows::Foundation;
//static bool isWindowHidden = true;
//static HWND wezHwnd;

DWORD tlsIndex;
//// The callback function for the WH_KEYBOARD_LL hook
//LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
//	//The wParam and lParam parameters contain information about a keyboard message. (https://learn.microsoft.com/en-us/windows/win32/winmsg/lowlevelkeyboardproc)	
//	if (nCode == HC_ACTION) {
//		PKBDLLHOOKSTRUCT keypress = (PKBDLLHOOKSTRUCT)lParam;
//		// Get the dimension of the monitor
//		DimensionStruct monitorDim = GetMonitorDimension(wezHwnd);
//
//		// Get the dimension of the window based on the percentage of the monitor
//		WindowDimensionStruct winDim = GetWinDimensionByPercent(monitorDim, 100, 50);
//
//		switch (wParam) {
//			// The WM_KEYDOWN message is posted to the window with the keyboard focus when a nonsystem key is pressed. A nonsystem key is a key that is pressed when the ALT key is not pressed.
//		case WM_KEYDOWN:
//			if (keypress->vkCode == vkCode) {
//				wcout << L"F1 Pressed" << endl;
//
//				if (isWindowHidden) {
//					// Set the window position and 
//					SetWindowPos(wezHwnd, HWND_TOPMOST, 0, 0, winDim.width, winDim.height, SWP_SHOWWINDOW);
//					isWindowHidden = false;
//				}
//				else {
//					// Hide the window
//					SetWindowPos(wezHwnd, HWND_TOPMOST, 0, 0, winDim.width, winDim.height, SWP_HIDEWINDOW);
//					isWindowHidden = true;
//				}
//
//			}
//			if (wezHwnd == nullptr) {
//				wcerr << L"Error: Window not found" << endl;
//				return 1;
//			}
//
//			break;
//		case WM_SYSKEYDOWN:
//			break;
//		case WM_KEYUP:
//			break;
//		case WM_SYSKEYUP:
//			break;
//		}
//	}
//	return CallNextHookEx(NULL, nCode, wParam, lParam);
//}

toml::table ParseToml(string filename) {
	toml::table table;
	try
	{
		table = toml::parse_file(filename);
		std::cout << table << "\n";
		
	}
	catch (const toml::parse_error& err)
	{
		std::cerr << "Parsing failed:\n" << err << "\n";
		return toml::table(); // Return an empty table in case of error
	}
	return table;
}

toml::table ParseToml(string filename, int index) {
	toml::table table;
	try
	{
		table = toml::parse_file(filename);
		std::cout << table << "\n";

	}
	catch (const toml::parse_error& err)
	{
		std::cerr << "Parsing failed:\n" << err << "\n";
		return toml::table(); // Return an empty table in case of error
	}
	return table;
}


// Use a struct to store the wezHwnd and the WindowDimensionStruct for multi-threading, add vkcode too
struct WindowThread {
	HWND hwnd;
	WindowDimensionStruct winDim;
	int vkCode;
	HANDLE hThread;

	WindowThread() {
		this->hwnd = nullptr;
		this->winDim = { 0,0,0,0 };
		this->vkCode = -1;
	}


	DWORD WINAPI WindowThreadFunction(LPVOID lpParam) {
		if (this->hwnd == nullptr) {
			wcerr << L"Error: Window not found" << endl;
			return 1;
		}

		////WH_KEYBOARD_LL hook
		HHOOK hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);

		// Keep this app running until we're told to stop
		MSG msg;
		while (!GetMessage(&msg, NULL, NULL, NULL)) {    //this while loop keeps the hook
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		UnhookWindowsHookEx(hHook);
	}
	WindowThread(LPCWSTR className, WindowDimensionStruct winDim, string keyName) {
		this->hwnd = SearchWindow(L"org.wezfurlong.wezterm", NULL);;
		this->winDim = winDim;
		this->vkCode = VKCodes::getVKCode(keyName);
		this->hThread = CreateThread(NULL, 0, WindowThreadFunction, this, 0, NULL);
	}
};



int main() {
	//PrintWindows(true);
	//PrintDesktopWindows(true);

	toml::table tomlTable = ParseToml("config.toml");
	WindowThread* scratchpadThreads = new WindowThread[tomlTable.size()];

	std::cout << tomlTable << "\n";
	int i = 0;
	for (const auto& [tableName, subNode] : tomlTable) {
		std::cout << tableName << "\n";

		auto subTable = subNode.as_table();
		WindowDimensionStruct winDim = {
			(int) subTable->at("width").as_integer()->get(),
			(int) subTable->at("height").as_integer()->get()
		};
		scratchpadThreads[i] = WindowThread(
			(LPCWSTR)subTable->at("classname").as_string(),
			winDim,
			subTable->get_as<string>("key")->get()
		);
		
			i++;
	}
	std::cout << tomlTable["Wezterm"]["classname"] << "\n";
	/*wezHwnd = SearchWindow(L"org.wezfurlong.wezterm", NULL);*/
	


	//Windows Thread experiment 
	tlsIndex = TlsAlloc();
	if (tlsIndex == TLS_OUT_OF_INDEXES)
	{
		printf("Failed to allocate TLS index.\n");
		return 1;
	}
	

	// spawn the threads 
	for (int i = 0; i < tomlTable.size(); i++) {
		/*std::cout << "Window " << i << ":\n";
		scratchpadThreads[i].winDim.Print();
		std::cout << "VK Code: " << scratchpadThreads[i].vkCode << "\n";*/
		HANDLE hThread = CreateThread(NULL, 0, WindowThreadFunction, &scratchpadThreads[i], 0, NULL);
	}




	std::cin.get();
	return 0;

}


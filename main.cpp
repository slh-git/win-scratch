#include "pch.h"
#include "ui_utilities.h"
#include "toml++/toml.hpp"
#include "vk_codes.h"
#include <thread>
#include <vector>
//using namespace Windows::Foundation;
// Function declaration
DWORD WINAPI WindowThreadFunction(LPVOID lpParam);
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

// Global variables
// Thread-local storage index for the WindowThread object
DWORD tlsIndex; 
/* After alloc it should automatically pull from each thread?*/



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
	int TLSIndex;
	bool isWindowHidden;
	WindowThread() {
		this->hwnd = nullptr;
		this->winDim = { 0,0,0,0 };
		this->vkCode = -1;
		this->hThread = nullptr;
		this->isWindowHidden = true;
	}

	WindowThread(LPCWSTR className, WindowDimensionStruct winDim, string keyName, int tlsIndex) {
		this->hwnd = SearchWindow(L"org.wezfurlong.wezterm", NULL);;
		this->winDim = winDim;
		this->vkCode = VKCodes::getVKCode(keyName);
		this->hThread = CreateThread(NULL, 0, WindowThreadFunction, this, 0, NULL);
		if (!hThread)
		{
			printf("Failed to create hthread. Error: %lu\n", GetLastError());
		}
		this->isWindowHidden = true;
	}
};

DWORD WINAPI WindowThreadFunction(LPVOID lpParam) {
	WindowThread* windowThread = (WindowThread*)lpParam;
	if (windowThread->hwnd == nullptr) {
		wcerr << L"Error: Window not found" << endl;
		return 1;
	}

	printf("windowThread tlsindex= %d\n", windowThread->TLSIndex);

	// Set thread-specific data in TLS
	if (!TlsSetValue(tlsIndex, windowThread))
	{
		printf("Failed to set TLS value.\n");
		free(windowThread);
		return 1;
	}

	//WH_KEYBOARD_LL hook
	HHOOK hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);

	// Keep this app running until we're told to stop
	MSG msg;
	while (!GetMessage(&msg, NULL, NULL, NULL)) {    //this while loop keeps the hook
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(hHook);
	return 1;
}

// The callback function for the WH_KEYBOARD_LL hook
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
		//The wParam and lParam parameters contain information about a keyboard message. (https://learn.microsoft.com/en-us/windows/win32/winmsg/lowlevelkeyboardproc)	
	//The wParam parameter specifies the virtual-key code of the key that generated the keystroke message.	
		if (nCode == HC_ACTION) {
			// Retrieve the WindowThread object from TLS
			WindowThread* windowThread = (WindowThread*)TlsGetValue(tlsIndex);
			if (windowThread == nullptr) {
				wcerr << L"Error: Failed to retrieve TLS value" << endl;
				return CallNextHookEx(NULL, nCode, wParam, lParam);
			}
			
			PKBDLLHOOKSTRUCT keypress = (PKBDLLHOOKSTRUCT)lParam;
			// Get the dimension of the monitor
			DimensionStruct monitorDim = GetMonitorDimension(windowThread->hwnd);
	
			// Get the dimension of the window based on the percentage of the monitor
			WindowDimensionStruct winDim = GetWinDimensionByPercent(monitorDim, 100, 50);
	
			switch (wParam) {
				// The WM_KEYDOWN message is posted to the window with the keyboard focus when a nonsystem key is pressed. A nonsystem key is a key that is pressed when the ALT key is not pressed.
			case WM_KEYDOWN:
				if (keypress->vkCode == windowThread->vkCode) {
					wcout << L"F1 Pressed" << endl;
					// Get the handle of the window	
					if (windowThread->isWindowHidden) {
						// Set the window position and 
						SetWindowPos(windowThread->hwnd, HWND_TOPMOST, 0, 0, winDim.width, winDim.height, SWP_SHOWWINDOW);
						windowThread->isWindowHidden = false;
					}
					else {
						// Hide the window
						SetWindowPos(windowThread->hwnd, HWND_TOPMOST, 0, 0, winDim.width, winDim.height, SWP_HIDEWINDOW);
						windowThread->isWindowHidden = true;
					}
	
				}
				if (windowThread->hwnd == nullptr) {
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
	PrintWindows(true);
	PrintDesktopWindows(true);
	//Windows Thread experiment 
	tlsIndex = TlsAlloc();
	if (tlsIndex == TLS_OUT_OF_INDEXES)
	{
		printf("Failed to allocate TLS index.\n");
		return 1;
	}

	toml::table tomlTable = ParseToml("config.toml");
	vector<WindowThread> scratchpadThreads;
	scratchpadThreads.reserve(tomlTable.size()); // Reserve space for efficiency

	std::cout << tomlTable << "\n";

	for (const auto& [tableName, subNode] : tomlTable) {
		std::cout << tableName << "\n";

		auto subTable = subNode.as_table();
		WindowDimensionStruct winDim = {
			static_cast<int>(subTable->at("width").as_integer()->get()),
			static_cast<int>(subTable->at("height").as_integer()->get())
		};
		scratchpadThreads.emplace_back(
			(LPCWSTR)subTable->at("classname").as_string(),
			winDim,
			subTable->get_as<std::string>("key")->get(),
			scratchpadThreads.size()
		);
	}


	std::cout << tomlTable["Wezterm"]["classname"] << "\n";
	




	// spawn the threads 
	for (int i = 0; i < tomlTable.size(); i++) {
		/*std::cout << "Window " << i << ":\n";
		scratchpadThreads[i].winDim.Print();
		std::cout << "VK Code: " << scratchpadThreads[i].vkCode << "\n";*/
	}




	std::cin.get();
	return 0;

}


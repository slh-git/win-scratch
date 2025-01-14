#include "pch.h"
#include "ui_utilities.h"
#include "toml++/toml.hpp"
#include "vk_codes.h"
#include <thread>
#include <vector>
#include <locale>
#include <codecvt>
#include <atomic>
// Function declaration
DWORD WINAPI WindowThreadFunction(LPVOID lpParam);
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
std::string wstring_to_utf8(const std::wstring& wstr);
std::wstring utf8_to_wstring(const std::string& str);
struct WindowThread;

// Global variables
DWORD tlsIndex; // Thread-local storage index for the WindowThread object
vector<WindowThread> scratchpadThreads;
atomic<bool> g_Running = TRUE;

// Use a struct to store the wezHwnd and the WindowDimensionStruct for multi-threading, add vkcode too
struct WindowThread {
	HWND hwnd;
	WindowDimensionStruct winDim;
	unsigned int vkCode;
	HANDLE hThread;
	bool isWindowHidden;
	HHOOK hHook;
	WindowThread() {
		this->hwnd = nullptr;
		this->winDim = { 0,0,0,0 };
		this->vkCode = -1;
		this->hThread = nullptr;
		this->isWindowHidden = true;
		this->hHook = NULL;

	}

	WindowThread(string className, WindowDimensionStruct winDim, string keyName) {
		this->hwnd = SearchWindow(utf8_to_wstring(className).c_str(), NULL);
		this->winDim = winDim;
		this->vkCode = VKCodes::getVKCode(keyName);
		this->hHook = NULL;
		this->hThread = CreateThread(NULL, 0, WindowThreadFunction, this, 0, NULL);
		if (!hThread)
		{
			printf("Failed to create hthread. Error: %lu\n", GetLastError());
		}
		this->isWindowHidden = true;
	}
};


// Function to convert UTF-8 string to wide string
std::wstring utf8_to_wstring(const std::string& str) {
	if (str.empty()) return std::wstring();
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

// Function to convert wide string to UTF-8 string
std::string wstring_to_utf8(const std::wstring& wstr) {
	if (wstr.empty()) return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}



// Function to parse the toml file
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



DWORD WINAPI WindowThreadFunction(LPVOID lpParam) {
	WindowThread* windowThread = (WindowThread*)lpParam;
	if (windowThread->hwnd == nullptr) {
		wcerr << L"Error: Window not found" << endl;
		return 1;
	}


	// Set thread-specific data in TLS
	if (!TlsSetValue(tlsIndex, windowThread))
	{
		printf("Failed to set TLS value.\n");
		free(windowThread);
		return 1;
	}

	//WH_KEYBOARD_LL hook
	HHOOK hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);

	windowThread->hHook = hHook;
	// Check if the hook was successfully set
	if (hHook == NULL) {
		printf("Failed to set hook. Error: %lu\n", GetLastError());
		return 1;
	}
	// Keep this app running until we're told to stop
	MSG msg;
	while (g_Running.load(memory_order_relaxed)) {
		// Process messages in the queue
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

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

// Console handler for graceful shutdown
BOOL WINAPI consoleHandler(DWORD signal) {

	if (signal == CTRL_C_EVENT || signal == CTRL_CLOSE_EVENT) {
		printf("Initiating graceful shutdown...\n");
		// Cleanup and close the program
		g_Running.store(FALSE, memory_order_relaxed);

		for (auto& thread : scratchpadThreads) {
			if (UnhookWindowsHookEx(thread.hHook)) {
				printf("Successfully unhooked keyboard hook for thread\n");
				thread.hHook = NULL;
			}
			else {
				printf("Failed to unhook keyboard hook. Error: %lu\n", GetLastError());
			}
		}

		for (auto& thread : scratchpadThreads) {
			if (thread.hThread) {
				PostThreadMessage(GetThreadId(thread.hThread), WM_QUIT, 0, 0);
			}
		}
		// final wait before shutdown
		for (auto& thread : scratchpadThreads) {
			if (thread.hThread) {
				// Wait with timeout
				DWORD waitResult = WaitForSingleObject(thread.hThread, 1000);
				if (waitResult == WAIT_TIMEOUT) {
					printf("Warning: Thread failed to exit within timeout\n");
				}
				CloseHandle(thread.hThread);
				thread.hThread = nullptr;
			}
		}
		if (tlsIndex != TLS_OUT_OF_INDEXES) {
			TlsFree(tlsIndex);
		}
		printf("Shutdown complete\n");
		return TRUE;

	}
	return FALSE;
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
	// Set up console handler for graceful shutdown
	if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
		printf("ERROR: Could not set control handler\n");
		return 1;
	}
	toml::table tomlTable = ParseToml("config.toml");

	scratchpadThreads.reserve(tomlTable.size()); // Reserve space for efficiency

	std::cout << tomlTable << "\n";

	// Iterate through the toml table to create threads
	for (const auto& [tableName, subNode] : tomlTable) {
		std::cout << tableName << "\n";

		auto subTable = subNode.as_table();
		WindowDimensionStruct winDim = {
			static_cast<int>(subTable->at("width").as_integer()->get()),
			static_cast<int>(subTable->at("height").as_integer()->get())
		};
		scratchpadThreads.emplace_back(
			subTable->at("classname").as_string()->get(),
			winDim,
			subTable->get_as<std::string>("key")->get()
		);
	}

	std::cin.get();
	// Wait for all thread
	for (auto& thread : scratchpadThreads) {
		WaitForSingleObject(thread.hThread, INFINITE);
		CloseHandle(thread.hThread);
	}
	TlsFree(tlsIndex);

	return 0;

}


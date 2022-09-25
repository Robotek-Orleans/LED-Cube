#pragma once

#if !defined(_WIN32)
#error "This code is only for Windows"
#endif

#include <windows.h>
#include <wingdi.h>
#include "../ledcube/Frame.h"

class LedCubeWindow
{
public:
	LedCubeWindow(HINSTANCE hInstance, int nShowCmd);
	~LedCubeWindow();
	int WINAPI process();
	static void setFrame(const Frame *frame);
	static int getFrameDelay();

private:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static void TimerProc(HWND hWnd, INT unnamedParam2, UINT_PTR unnamedParam3, DWORD localTimestamp);
	int getIColorPos(int x, int y);
	void PaintWindow(HWND hWnd);
	void PaintTimeout(HWND hWnd);

	HINSTANCE hInstance;
	HWND hWnd;
	UINT_PTR timer;
	const Frame *frame;
	int change_counter = 0; // Increased when data is received
	int update_counter = 0; // Increased by WindowProc
	uint8_t ****colors = nullptr;
};
#ifndef UNICODE
#define UNICODE
#endif

#include <iostream>
#include <windows.h>
#include <wingdi.h>

#include "../PatternThread.h"

#define PIXELS_PAR_LED 32 // Led = Rectangle de 32 * 32 pixels
#define PIXELS_WIDTH 256  // 8 leds * 32 pixels
#define PIXELS_HEIGHT 256 // 8 leds * 32 pixels

const uint16_t *colors;
bool *is_updated;

int getIColorPos(int x, int y)
{
	int pos_groupe = y / 2;
	bool pos_haut = y % 2;
	bool pos_gauche = x < PatternThread::WIDTH / 2;

	int i_color = pos_groupe * 48 + x;
	if (pos_gauche == pos_haut)
		i_color += 8;
	// Monter à la ligne suivante pour les parties en bas-droite et en haut-gauche
	// (En bas à droite car la partie de droite est inversée)

	// std::cout << "(" << x << "," << y << ") = " << i_color << std::endl;
	return i_color;
}

void PaintWindow(HDC hdc, RECT rcMaxPaint)
{
	int i_color;
	HBRUSH hBrush;
	int left, top, right, bottom;

	for (int y = 0; y < PatternThread::HEIGHT; y++)
	{
		top = PIXELS_PAR_LED * y;
		bottom = PIXELS_PAR_LED * (y + 1);

		for (int x = 0; x < PatternThread::WIDTH; x++)
		{
			left = PIXELS_PAR_LED * x;
			right = PIXELS_PAR_LED * (x + 1);

			i_color = getIColorPos(x, y);

			// TODO : Ajouter les exceptions sur certains rouges

			hBrush = CreateSolidBrush(RGB(colors[i_color] * 255 / 4095,
										  colors[i_color + 16] * 255 / 4095,
										  colors[i_color + 32] * 255 / 4095));
			if (hBrush == NULL)
			{
				std::cout << "[PaintWindow] CreateSolidBrush failed : " << RGB(colors[i_color] * 255 / 4095, colors[i_color + 16] * 255 / 4095, colors[i_color + 32] * 255 / 4095) << std::endl;
			}
			SelectObject(hdc, hBrush);
			Rectangle(hdc, left, top, right, bottom);

			DeleteObject(hBrush);
		}
	}
}

void TimerProc(HWND hWnd, INT unnamedParam2, UINT_PTR unnamedParam3, DWORD localTimestamp)
{
	if (*is_updated)
		return;
	if (IsIconic(hWnd))
		return;

	// N.B. ps.rcPaint is NULL here
	RECT rcPaint{0, 0, PIXELS_WIDTH, PIXELS_HEIGHT};
	InvalidateRect(hWnd, &rcPaint, false);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		if (!*is_updated)
		{
			// std::cout << "[WindowProc/WM_PAINT] HWND: " << hWnd << " " << wParam << " " << lParam << std::endl;
			HDC hdc = BeginPaint(hWnd, &ps);

			// std::cout << "[WindowProc/WM_PAINT] "
			// 		  << "{" << ps.rcPaint.left << "=>" << ps.rcPaint.right
			// 		  << ", " << ps.rcPaint.top << "=>" << ps.rcPaint.bottom << "}" << std::endl;
			PaintWindow(hdc, ps.rcPaint);
			EndPaint(hWnd, &ps);
			*is_updated = true;
		}

		return 0;
	}
	default:
		// Process other messages.
		// std::cout << "[WindowProc/" << uMsg << "] " << wParam << " " << lParam << std::endl;
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	std::cout << "[WinMain] starting..." << std::endl;

	// Register the window class.
	const wchar_t CLASS_NAME[] = L"Sample Window Class";
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	RegisterClass(&wc);

	// Create the window.
	HWND hWnd = CreateWindowEx(
		0,					 // Optional window styles.
		CLASS_NAME,			 // Window class
		L"Ledcube-Windows",	 // Window text
		WS_OVERLAPPEDWINDOW, // Window style

		// Position
		CW_USEDEFAULT, CW_USEDEFAULT,
		// Size
		PIXELS_WIDTH + 15, PIXELS_HEIGHT + 38,

		NULL,	   // Parent window
		NULL,	   // Menu
		hInstance, // Instance handle
		NULL	   // Additional application data
	);

	if (hWnd == NULL)
	{
		std::cout << "[WinMain] ERROR: Failed to create window" << std::endl;
		return 0;
	}
	std::cout << "[WinMain] Window created!" << std::endl;

	PatternThread pattern_thread;
	colors = pattern_thread.getPatternColors();
	is_updated = pattern_thread.isUpdated();
	pattern_thread.setDefaultPattern(lpCmdLine);

	ShowWindow(hWnd, nShowCmd);
	std::cout << "[WinMain] PaintWindow Timer #" << SetTimer(hWnd, 0, 50, (TIMERPROC)TimerProc) << " created! (20 FPS)" << std::endl;

	sleepms(10);
	pattern_thread.start();
	std::cout << "[WinMain] PatternThread Started" << std::endl;

	// Run the message loop.
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	pattern_thread.forcequit();

	return msg.wParam;
}

#if !defined(_WIN32)
#error "This code is only for Windows"
#endif

#include "ledcubewindow.h"

#include <iostream>
#include <windows.h>
#include <wingdi.h>

#define PIXELS_PAR_LED 32 // Led = Rectangle de 32 * 32 pixels
#define PIXELS_WIDTH 256  // 8 leds * 32 pixels
#define PIXELS_HEIGHT 256 // 8 leds * 32 pixels
#define FPS 20
#define DELAY_MS (1000 / FPS) // 50 ms

LedCubeWindow *instance = nullptr;

LedCubeWindow::LedCubeWindow(HINSTANCE hInstance, int nShowCmd)
	: hInstance(hInstance) {
	instance = this;
	std::cout << "[LedCubeWindow] starting..." << std::endl;
	colors = new uint8_t ***[ZLENGTH];
	for (int z = 0; z < ZLENGTH; ++z) {
		colors[z] = new uint8_t **[YLENGTH];
		for (int y = 0; y < YLENGTH; y++) {
			colors[z][y] = new uint8_t *[XLENGTH];
			for (int x = 0; x < XLENGTH; x++) {
				colors[z][y][x] = new uint8_t[3];
				colors[z][y][x][0] = 0;
				colors[z][y][x][1] = 0;
				colors[z][y][x][2] = 0;
			}
		}
	}

	// Register the window class.
	std::string CLASS_NAME = "Sample Window Class";
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME.c_str();
	RegisterClass(&wc);

	// Create the window.
	hWnd = CreateWindowEx(0,				   // Optional window styles.
						  CLASS_NAME.c_str(),  // Window class
						  "Ledcube-Windows",   // Window text
						  WS_OVERLAPPEDWINDOW, // Window style

						  // Position
						  CW_USEDEFAULT, CW_USEDEFAULT,
						  // Size
						  PIXELS_WIDTH + 15, PIXELS_HEIGHT + 38,

						  NULL,		 // Parent window
						  NULL,		 // Menu
						  hInstance, // Instance handle
						  NULL		 // Additional application data
	);

	if (hWnd == NULL) {
		std::cout << "[LedCubeWindow] ERROR: Failed to create window" << std::endl;
		exit(1);
	}
	std::cout << "[LedCubeWindow] Window created!" << std::endl;

	update_counter = 0;
	change_counter = 0;

	ShowWindow(hWnd, nShowCmd);
	timer = SetTimer(hWnd, 1, DELAY_MS, (TIMERPROC)TimerProc);
	std::cout << "[LedCubeWindow] PaintWindow Timer #" << timer << " created! (" << FPS << " FPS)" << std::endl;
}

LedCubeWindow::~LedCubeWindow() {
	instance = nullptr;
	KillTimer(hWnd, timer);
	std::cout << "[LedCubeWindow] Stopped!" << std::endl;
}

int WINAPI LedCubeWindow::process() {
	// Run the message loop.
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

void LedCubeWindow::setFrame(const Frame *frame) {
	if (instance != nullptr) {
		instance->frame = frame;
		if (frame != nullptr) frame->toRGBArray(instance->colors);
		instance->change_counter++;
	}
}

int LedCubeWindow::getFrameDelay() {
	return DELAY_MS;
}

LRESULT CALLBACK LedCubeWindow::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_PAINT: {
		instance->PaintWindow(hWnd);
		return 0;
	}
	default:
		// Process other messages.
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

void LedCubeWindow::TimerProc(HWND hWnd, INT unnamedParam2, UINT_PTR unnamedParam3, DWORD localTimestamp) {
	if (instance != nullptr) instance->PaintTimeout(hWnd);
}

int LedCubeWindow::getIColorPos(int x, int y) {
	int pos_groupe = y / 2;
	bool pos_haut = y % 2;
	bool pos_gauche = x < XLENGTH / 2;

	int i_color = pos_groupe * 48 + x;
	if (pos_gauche == pos_haut) i_color += 8;
	// Monter à la ligne suivante pour les parties en bas-droite et en haut-gauche
	// (En bas à droite car la partie de droite est inversée)

	// std::cout << "(" << x << "," << y << ") = " << i_color << std::endl;
	return i_color;
}

void LedCubeWindow::PaintWindow(HWND hWnd) {
	if (change_counter == update_counter) {
		return;
	}
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	update_counter = change_counter;
	RECT rcMaxPaint = ps.rcPaint;

	int r = 0, g = 0, b = 0;
	HBRUSH hBrush;
	int left, top, right, bottom;

	hBrush = CreateSolidBrush(RGB(0, 0, 0));
	SelectObject(hdc, hBrush);
	for (int y = 0; y < YLENGTH; ++y) {
		top = PIXELS_PAR_LED * y;
		bottom = PIXELS_PAR_LED * (y + 1);
		for (int x = 0; x < XLENGTH; ++x) {
			left = PIXELS_PAR_LED * x;
			right = PIXELS_PAR_LED * (x + 1);
			Rectangle(hdc, left + 8, top + 8, right - 8, bottom - 8);
		}
	}
	DeleteObject(hBrush);

	for (int z = 0; z < ZLENGTH; ++z) { // bottom->top
		top = PIXELS_PAR_LED * (7 - z);
		bottom = top + PIXELS_PAR_LED;
		for (int x = 0; x < XLENGTH; ++x) { // left->right
			left = PIXELS_PAR_LED * x;
			right = left + PIXELS_PAR_LED;

			for (int y = 0; y < YLENGTH; ++y) { // front->back

				r = colors[z][y][x][0];
				g = colors[z][y][x][1];
				b = colors[z][y][x][2];

				hBrush = CreateSolidBrush(RGB(r, g, b));
				if (hBrush == NULL) {
					std::cout << "[PaintWindow] CreateSolidBrush failed : " << RGB(r, g, b) << std::endl;
				}
				SelectObject(hdc, hBrush);

				RECT rc = {left + y, top + y, right - y, bottom - y}; // y0=front == biggest rectangle
				FrameRect(hdc, &rc, hBrush);

				DeleteObject(hBrush);
			}
		}
	}

	EndPaint(hWnd, &ps);
	ReleaseDC(hWnd, hdc); // always release hdc
}

void LedCubeWindow::PaintTimeout(HWND hWnd) {
	if (change_counter == update_counter) return;
	if (IsIconic(hWnd)) return;

	// N.B. ps.rcPaint is NULL here
	RECT rcPaint{0, 0, PIXELS_WIDTH, PIXELS_HEIGHT};
	InvalidateRect(hWnd, &rcPaint, false);
}

#pragma once
#include <windows.h>

class Editor {
private:
    HWND m_hWnd;
    HWND m_hwndTextView;
    HWND m_hwndStatusbar;

public:
    Editor(HWND hwnd);
    ~Editor();

    LONG WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

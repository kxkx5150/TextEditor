#pragma once
#include <windows.h>


UINT_PTR CALLBACK OpenHookProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int RectWidth(RECT* rect);
int RectHeight(RECT* rect);
void CenterWindow(HWND hwnd);

class Editor {
private:
    HINSTANCE m_mainhInst;

    HWND m_hWnd;
    HWND m_hwndTextView;
    HWND m_hwndStatusbar;

    BOOL m_fShowStatusbar = true;

    TCHAR g_szFileName[MAX_PATH];
    TCHAR g_szFileTitle[MAX_PATH];

public:
public:
    Editor(HINSTANCE hInst, HWND hwnd);
    ~Editor();

    void SetWindowFileName(HWND hwnd, TCHAR* szFileName, BOOL fModified);

    BOOL ShowOpenFileDlg(HWND hwnd, TCHAR* pstrFileName, TCHAR* pstrTitleName);
    BOOL DoOpenFile(HWND hwndMain, TCHAR* szFileName, TCHAR* szFileTitle);

    LONG WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    UINT CommandHandler(HWND hwnd, UINT nCtrlId, UINT nCtrlCode, HWND hwndFrom);
};

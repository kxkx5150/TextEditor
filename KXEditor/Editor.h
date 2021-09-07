#pragma once
#include <windows.h>
#include "../TextView/TextView_def.h"
#include "../TextView/TextView.h"
#include "resource.h"
#include "StatusBar.h"

#define COURIERNEW 1
#define LUCIDACONS 2
#define REGLOC _T("SOFTWARE\\kxkx5150\\kx_editor")

UINT_PTR CALLBACK OpenHookProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int RectWidth(RECT* rect);
int RectHeight(RECT* rect);
void CenterWindow(HWND hwnd);


class Editor {

public:
    static BOOL g_fFirstTime;

    HINSTANCE m_mainhInst;

    HWND m_hWnd;
    HWND m_hwndTextView;
    HWND m_hwndStatusbar;

    TCHAR g_szFileName[MAX_PATH];
    TCHAR g_szFileTitle[MAX_PATH];

    BOOL g_fShowStatusbar = true;

    HFONT g_hFont;
    LONG g_nFontSize;
    BOOL g_fFontBold;
    TCHAR g_szFontName[LF_FACESIZE];
    LONG g_nFontSmoothing;

    LONG g_nPaddingAbove;
    LONG g_nPaddingBelow;
    LONG g_fPaddingFlags;

    BOOL g_fSelMargin;
    BOOL g_fLineNumbers;
    BOOL g_fLongLines;
    int g_nLongLineLimit;
    BOOL g_fSaveOnExit;
    BOOL g_nHLCurLine;

    COLORREF g_rgbColourList[TXC_MAX_COLOURS];
    COLORREF g_rgbCustColours[16];
    COLORREF g_rgbAutoColourList[TXC_MAX_COLOURS] = {
        SYSCOL(COLOR_WINDOWTEXT), // foreground
        SYSCOL(COLOR_WINDOW), // background
        SYSCOL(COLOR_HIGHLIGHTTEXT), // selected text
        SYSCOL(COLOR_HIGHLIGHT), // selection
        SYSCOL(COLOR_WINDOWTEXT), // inactive selected text
        SYSCOL(COLOR_3DFACE), // inactive selection
        SYSCOL(COLOR_3DFACE), // selection margin#1
        SYSCOL(COLOR_3DHIGHLIGHT), // selection margin#2
        MIXED_SYSCOL(COLOR_3DSHADOW, COLOR_3DDKSHADOW), // line number text
        MIXED_SYSCOL2(COLOR_3DFACE, COLOR_WINDOW), // line number bg
        SYSCOL(COLOR_WINDOWTEXT), // long line text
        MIXED_SYSCOL(COLOR_3DFACE, COLOR_WINDOW), // long-line background
        SYSCOL(COLOR_WINDOWTEXT), // current line text
        RGB(230, 240, 255), // current line background
    };

public:
    Editor(HINSTANCE hInst, HWND hwnd);
    ~Editor();

    HWND getHwndTextView();

    void SetWindowFileName(HWND hwnd, TCHAR* szFileName, BOOL fModified);
    void SetWindSize(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    BOOL ShowOpenFileDlg(HWND hwnd, TCHAR* pstrFileName, TCHAR* pstrTitleName);
    BOOL DoOpenFile(HWND hwndMain, TCHAR* szFileName, TCHAR* szFileTitle);
    void OpenFile(HWND hwnd, TCHAR* szFile);
    void HandleDropFiles(HWND hwnd, HDROP hDrop);

    LONG WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    UINT CommandHandler(HWND hwnd, UINT nCtrlId, UINT nCtrlCode, HWND hwndFrom);

    BOOL GetSettingInt(HKEY hkey, TCHAR szKeyName[], LONG* pnReturnVal, LONG nDefault);
    BOOL GetSettingStr(HKEY hkey, TCHAR szKeyName[], TCHAR pszReturnStr[], DWORD nLength, TCHAR szDefault[]);
    void LoadRegSettings();
    int PointsToLogical(int nPointSize);
    BOOL GetSettingBin(HKEY hkey, TCHAR szKeyName[], PVOID pBuffer, LONG nLength);
    void ApplyRegSettings();
    HFONT EasyCreateFont(int nPointSize, BOOL fBold, DWORD dwQuality, TCHAR* szFace);




};

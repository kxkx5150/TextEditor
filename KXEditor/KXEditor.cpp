#include "KXEditor.h"
#include "../TextView/TextView.h"
#include <CRTDBG.H>
#pragma comment(lib, "uxtheme.lib")

Editor* g_ptv;
HINSTANCE hInst; // current instance
HWND g_hwndTextView;

WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
TCHAR g_szAppName[] = APP_TITLE;

BOOL g_fFirstTime = true;

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


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hpins, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_KXEDITOR, szWindowClass, MAX_LOADSTRING);
    EditorClass(hInstance);
    LoadRegSettings();

    if (!InitEditorClass(hInstance, nCmdShow))
        return FALSE;

    ApplyRegSettings();

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_KXEDITOR));
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

ATOM EditorClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_KXEDITOR));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_KXEDITOR);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}
BOOL InitEditorClass(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 800, 600, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd) {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

BOOL GetSettingInt(HKEY hkey, TCHAR szKeyName[], LONG* pnReturnVal, LONG nDefault)
{
    ULONG len = sizeof(nDefault);
    *pnReturnVal = nDefault;
    return !RegQueryValueEx(hkey, szKeyName, 0, 0, (BYTE*)pnReturnVal, &len);
}
BOOL GetSettingStr(HKEY hkey, TCHAR szKeyName[], TCHAR pszReturnStr[], DWORD nLength, TCHAR szDefault[])
{
    ULONG len = nLength * sizeof(TCHAR);
    lstrcpyn(pszReturnStr, szDefault, nLength);
    return !RegQueryValueEx(hkey, szKeyName, 0, 0, (BYTE*)pszReturnStr, &len);
}
void LoadRegSettings()
{
    HKEY hKey, hColKey;

    RegCreateKeyEx(HKEY_CURRENT_USER, REGLOC, 0, 0, 0, KEY_READ, 0, &hKey, 0);
    GetSettingInt(hKey, (TCHAR*)_T("FontSize"), &g_nFontSize, 10);
    GetSettingInt(hKey, (TCHAR*)_T("FontBold"), (LONG*)&g_fFontBold, FALSE);
    GetSettingStr(hKey, (TCHAR*)_T("FontName"), g_szFontName, LF_FACESIZE, (TCHAR*)_T("Consolas"));
    GetSettingInt(hKey, (TCHAR*)_T("FontSmooth"), &g_nFontSmoothing, DEFAULT_QUALITY);

    GetSettingInt(hKey, (TCHAR*)_T("PaddingAbove"), &g_nPaddingAbove, 0);
    GetSettingInt(hKey, (TCHAR*)_T("PaddingBelow"), &g_nPaddingBelow, 1);
    GetSettingInt(hKey, (TCHAR*)_T("PaddingFlags"), &g_fPaddingFlags, COURIERNEW | LUCIDACONS);

    GetSettingInt(hKey, (TCHAR*)_T("SelMargin"), (LONG*)&g_fSelMargin, TRUE);
    GetSettingInt(hKey, (TCHAR*)_T("LineNumbers"), (LONG*)&g_fLineNumbers, FALSE);
    GetSettingInt(hKey, (TCHAR*)_T("LongLines"), (LONG*)&g_fLongLines, TRUE);
    GetSettingInt(hKey, (TCHAR*)_T("LongLineLimit"), (LONG*)&g_nLongLineLimit, 80);
    GetSettingInt(hKey, (TCHAR*)_T("SaveOnExit"), (LONG*)&g_fSaveOnExit, TRUE);
    GetSettingInt(hKey, (TCHAR*)_T("HLCurLine"), (LONG*)&g_nHLCurLine, FALSE);

    //GetSettingInt(hKey, (TCHAR*)_T("AddExplorer"), (LONG*)&g_fAddToExplorer, FALSE);
    //GetSettingInt(hKey, (TCHAR*)_T("ReplaceNotepad"), (LONG*)&g_fReplaceNotepad, FALSE);
    //GetSettingInt(hKey, (TCHAR*)_T("ShowStatusbar"), (LONG*)&stbar, FALSE);

    RegCreateKeyEx(hKey, _T("Colours"), 0, 0, 0, KEY_READ, 0, &hColKey, 0);
    GetSettingInt(hColKey, (TCHAR*)_T("Foreground"), (LONG*)&g_rgbColourList[TXC_FOREGROUND], g_rgbAutoColourList[TXC_FOREGROUND]);
    GetSettingInt(hColKey, (TCHAR*)_T("Background"), (LONG*)&g_rgbColourList[TXC_BACKGROUND], g_rgbAutoColourList[TXC_BACKGROUND]);
    GetSettingInt(hColKey, (TCHAR*)_T("SelFG"), (LONG*)&g_rgbColourList[TXC_HIGHLIGHTTEXT], g_rgbAutoColourList[TXC_HIGHLIGHTTEXT]);
    GetSettingInt(hColKey, (TCHAR*)_T("SelBG"), (LONG*)&g_rgbColourList[TXC_HIGHLIGHT], g_rgbAutoColourList[TXC_HIGHLIGHT]);
    GetSettingInt(hColKey, (TCHAR*)_T("SelFG2"), (LONG*)&g_rgbColourList[TXC_HIGHLIGHTTEXT2], g_rgbAutoColourList[TXC_HIGHLIGHTTEXT2]);
    GetSettingInt(hColKey, (TCHAR*)_T("SelBG2"), (LONG*)&g_rgbColourList[TXC_HIGHLIGHT2], g_rgbAutoColourList[TXC_HIGHLIGHT2]);
    GetSettingInt(hColKey, (TCHAR*)_T("Margin1"), (LONG*)&g_rgbColourList[TXC_SELMARGIN1], g_rgbAutoColourList[TXC_SELMARGIN1]);
    GetSettingInt(hColKey, (TCHAR*)_T("Margin2"), (LONG*)&g_rgbColourList[TXC_SELMARGIN2], g_rgbAutoColourList[TXC_SELMARGIN2]);
    GetSettingInt(hColKey, (TCHAR*)_T("LinenoText"), (LONG*)&g_rgbColourList[TXC_LINENUMBERTEXT], g_rgbAutoColourList[TXC_LINENUMBERTEXT]);
    GetSettingInt(hColKey, (TCHAR*)_T("Lineno"), (LONG*)&g_rgbColourList[TXC_LINENUMBER], g_rgbAutoColourList[TXC_LINENUMBER]);
    GetSettingInt(hColKey, (TCHAR*)_T("LongLineText"), (LONG*)&g_rgbColourList[TXC_LONGLINETEXT], g_rgbAutoColourList[TXC_LONGLINETEXT]);
    GetSettingInt(hColKey, (TCHAR*)_T("LongLine"), (LONG*)&g_rgbColourList[TXC_LONGLINE], g_rgbAutoColourList[TXC_LONGLINE]);
    GetSettingInt(hColKey, (TCHAR*)_T("CurlineText"), (LONG*)&g_rgbColourList[TXC_CURRENTLINETEXT], g_rgbAutoColourList[TXC_CURRENTLINETEXT]);
    GetSettingInt(hColKey, (TCHAR*)_T("Curline"), (LONG*)&g_rgbColourList[TXC_CURRENTLINE], g_rgbAutoColourList[TXC_CURRENTLINE]);

    GetSettingBin(hColKey, (TCHAR*)_T("Custom"), g_rgbCustColours, sizeof(g_rgbCustColours));

    RegCloseKey(hColKey);
    RegCloseKey(hKey);
}
int PointsToLogical(int nPointSize)
{
    HDC hdc = GetDC(0);
    int nLogSize = -MulDiv(nPointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
    ReleaseDC(0, hdc);

    return nLogSize;
}
void ApplyRegSettings()
{
    if (g_hFont)
        DeleteObject(g_hFont);

    if (!g_hwndTextView)
        return;
    _RPTWN(_CRT_WARN, L"----g_hwndTextView %Xd\n", g_hwndTextView);

    g_hFont = EasyCreateFont(g_nFontSize, g_fFontBold, g_nFontSmoothing, g_szFontName);

    TextView_SetLineSpacing(g_hwndTextView, g_nPaddingAbove, g_nPaddingBelow);
    TextView_SetStyleBool(g_hwndTextView, TXS_SELMARGIN, g_fSelMargin);
    TextView_SetStyleBool(g_hwndTextView, TXS_LINENUMBERS, g_fLineNumbers);
    TextView_SetStyleBool(g_hwndTextView, TXS_LONGLINES, g_fLongLines);
    TextView_SetStyleBool(g_hwndTextView, TXS_HIGHLIGHTCURLINE, g_nHLCurLine);
    TextView_SetCaretWidth(g_hwndTextView, 2);
    TextView_SetLongLine(g_hwndTextView, g_nLongLineLimit);

    SendMessage(g_hwndTextView, WM_SETFONT, (WPARAM)g_hFont, 0);

    for (int i = 0; i < TXC_MAX_COLOURS; i++) {
        TextView_SetColor(g_hwndTextView, i, g_rgbColourList[i]);
    }
}
HFONT EasyCreateFont(int nPointSize, BOOL fBold, DWORD dwQuality, TCHAR* szFace)
{
    return CreateFont(PointsToLogical(nPointSize),
        0, 0, 0,
        fBold ? FW_BOLD : 0,
        0, 0, 0, DEFAULT_CHARSET, 0, 0,
        dwQuality,
        0,
        szFace);
}
BOOL GetSettingBin(HKEY hkey, TCHAR szKeyName[], PVOID pBuffer, LONG nLength)
{
    ZeroMemory(pBuffer, nLength);
    return !RegQueryValueEx(hkey, szKeyName, 0, 0, (BYTE*)pBuffer, (LPDWORD)&nLength);
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {

    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        switch (wmId) {

        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            if (g_ptv) {
                return g_ptv->WndProc(hWnd, message, wParam, lParam);
            } else {
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
    } break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    } break;

    case WM_CREATE: {
        if ((g_ptv = new Editor(hInst, hWnd)) == 0)
            return FALSE;
        g_hwndTextView = g_ptv->getHwndTextView();
    } break;

    case WM_DESTROY: {
        delete g_ptv;
        PostQuitMessage(0);
        DeleteObject(g_hFont);

    } break;

    default:
        if (g_ptv)
            return g_ptv->WndProc(hWnd, message, wParam, lParam);
        else
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}
UINT_PTR CALLBACK OpenHookProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_INITDIALOG:

        if (g_fFirstTime) {
            CenterWindow(GetParent(hwnd));
            g_fFirstTime = false;
        }

        return true;
    }

    return 0;
}
void CenterWindow(HWND hwnd)
{
    HWND hwndParent = GetParent(hwnd);
    RECT rcChild;
    RECT rcParent;
    int x, y;

    GetWindowRect(hwnd, &rcChild);
    GetWindowRect(hwndParent, &rcParent);

    x = rcParent.left + (RectWidth(&rcParent) - RectWidth(&rcChild)) / 2;
    y = rcParent.top + (RectHeight(&rcParent) - RectHeight(&rcChild)) / 2;

    MoveWindow(hwnd, max(0, x), max(0, y), RectWidth(&rcChild), RectHeight(&rcChild), TRUE);
}
int RectWidth(RECT* rect)
{
    return rect->right - rect->left;
}
int RectHeight(RECT* rect)
{
    return rect->bottom - rect->top;
}
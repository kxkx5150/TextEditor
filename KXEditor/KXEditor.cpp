#include "KXEditor.h"

// Global Variables:
Editor* g_ptv;
HINSTANCE hInst; // current instance

WCHAR szTitle[MAX_LOADSTRING]; // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING]; // the main window class name
TCHAR g_szAppName[] = APP_TITLE;

HFONT g_hFont;
LONG g_nFontSize;
BOOL g_fFontBold;
TCHAR g_szFontName[LF_FACESIZE];
LONG g_nFontSmoothing;


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

    //GetSettingInt(hKey, _T("PaddingAbove"), &g_nPaddingAbove, 0);
    //GetSettingInt(hKey, _T("PaddingBelow"), &g_nPaddingBelow, 1);
    //GetSettingInt(hKey, _T("PaddingFlags"), &g_fPaddingFlags, COURIERNEW | LUCIDACONS);

    //GetSettingInt(hKey, _T("SelMargin"), &g_fSelMargin, TRUE);
    //GetSettingInt(hKey, _T("LineNumbers"), &g_fLineNumbers, FALSE);
    //GetSettingInt(hKey, _T("LongLines"), &g_fLongLines, TRUE);
    //GetSettingInt(hKey, _T("LongLineLimit"), &g_nLongLineLimit, 80);
    //GetSettingInt(hKey, _T("SaveOnExit"), &g_fSaveOnExit, TRUE);
    //GetSettingInt(hKey, _T("HLCurLine"), &g_nHLCurLine, FALSE);

    //GetSettingInt(hKey, _T("AddExplorer"), &g_fAddToExplorer, FALSE);
    //GetSettingInt(hKey, _T("ReplaceNotepad"), &g_fReplaceNotepad, FALSE);
    //GetSettingInt(hKey, _T("ShowStatusbar"), &g_fShowStatusbar, FALSE);

    //// read the display colours
    //RegCreateKeyEx(hKey, _T("Colours"), 0, 0, 0, KEY_READ, 0, &hColKey, 0);

    //GetSettingInt(hColKey, _T("Foreground"), &g_rgbColourList[TXC_FOREGROUND], g_rgbAutoColourList[TXC_FOREGROUND]);
    //GetSettingInt(hColKey, _T("Background"), &g_rgbColourList[TXC_BACKGROUND], g_rgbAutoColourList[TXC_BACKGROUND]);
    //GetSettingInt(hColKey, _T("SelFG"), &g_rgbColourList[TXC_HIGHLIGHTTEXT], g_rgbAutoColourList[TXC_HIGHLIGHTTEXT]);
    //GetSettingInt(hColKey, _T("SelBG"), &g_rgbColourList[TXC_HIGHLIGHT], g_rgbAutoColourList[TXC_HIGHLIGHT]);
    //GetSettingInt(hColKey, _T("SelFG2"), &g_rgbColourList[TXC_HIGHLIGHTTEXT2], g_rgbAutoColourList[TXC_HIGHLIGHTTEXT2]);
    //GetSettingInt(hColKey, _T("SelBG2"), &g_rgbColourList[TXC_HIGHLIGHT2], g_rgbAutoColourList[TXC_HIGHLIGHT2]);
    //GetSettingInt(hColKey, _T("Margin1"), &g_rgbColourList[TXC_SELMARGIN1], g_rgbAutoColourList[TXC_SELMARGIN1]);
    //GetSettingInt(hColKey, _T("Margin2"), &g_rgbColourList[TXC_SELMARGIN2], g_rgbAutoColourList[TXC_SELMARGIN2]);
    //GetSettingInt(hColKey, _T("LinenoText"), &g_rgbColourList[TXC_LINENUMBERTEXT], g_rgbAutoColourList[TXC_LINENUMBERTEXT]);
    //GetSettingInt(hColKey, _T("Lineno"), &g_rgbColourList[TXC_LINENUMBER], g_rgbAutoColourList[TXC_LINENUMBER]);
    //GetSettingInt(hColKey, _T("LongLineText"), &g_rgbColourList[TXC_LONGLINETEXT], g_rgbAutoColourList[TXC_LONGLINETEXT]);
    //GetSettingInt(hColKey, _T("LongLine"), &g_rgbColourList[TXC_LONGLINE], g_rgbAutoColourList[TXC_LONGLINE]);
    //GetSettingInt(hColKey, _T("CurlineText"), &g_rgbColourList[TXC_CURRENTLINETEXT], g_rgbAutoColourList[TXC_CURRENTLINETEXT]);
    //GetSettingInt(hColKey, _T("Curline"), &g_rgbColourList[TXC_CURRENTLINE], g_rgbAutoColourList[TXC_CURRENTLINE]);

    //GetSettingBin(hColKey, _T("Custom"), g_rgbCustColours, sizeof(g_rgbCustColours));

    //RegCloseKey(hColKey);
    //RegCloseKey(hKey);
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

    g_hFont = EasyCreateFont(g_nFontSize, g_fFontBold, g_nFontSmoothing, g_szFontName);

    //TextView_SetLineSpacing(g_hwndTextView, g_nPaddingAbove, g_nPaddingBelow);

    //TextView_SetStyleBool(g_hwndTextView, TXS_SELMARGIN, g_fSelMargin);
    //TextView_SetStyleBool(g_hwndTextView, TXS_LINENUMBERS, g_fLineNumbers);
    //TextView_SetStyleBool(g_hwndTextView, TXS_LONGLINES, g_fLongLines);

    //TextView_SetStyleBool(g_hwndTextView, TXS_HIGHLIGHTCURLINE, g_nHLCurLine);

    //TextView_SetCaretWidth(g_hwndTextView, 2);
    //TextView_SetLongLine(g_hwndTextView, g_nLongLineLimit);

    //SendMessage(g_hwndTextView, WM_SETFONT, (WPARAM)g_hFont, 0);

    //for (int i = 0; i < TXC_MAX_COLOURS; i++) {
    //    TextView_SetColor(g_hwndTextView, i, g_rgbColourList[i]);
    //}
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

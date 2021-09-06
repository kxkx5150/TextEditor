#include "Editor.h"
#include <CRTDBG.H>

BOOL Editor::g_fFirstTime = false;

Editor::Editor(HINSTANCE hInst, HWND hwnd)
{
    LoadRegSettings();

    m_mainhInst = hInst;
    m_hWnd = hwnd;
    m_hwndTextView = CreateTextView(hwnd);
    m_hwndStatusbar = CreateStatusBar(hwnd);

    PostMessage(m_hWnd, WM_COMMAND, IDM_FILE_NEW, 0);

    ApplyRegSettings();
}
Editor::~Editor()
{
    DeleteObject(g_hFont);
}
HWND Editor::getHwndTextView()
{
    return m_hwndTextView;
}

void Editor::SetWindowFileName(HWND hwnd, TCHAR* szFileName, BOOL fModified)
{
    TCHAR ach[MAX_PATH + 4];
    TCHAR mod[4] = _T("");

    if (fModified)
        lstrcpy(mod, _T(" *"));

    wsprintf(ach, _T("%s - %s"), szFileName, mod);
    SetWindowText(hwnd, ach);
}
BOOL Editor::ShowOpenFileDlg(HWND hwnd, TCHAR* pstrFileName, TCHAR* pstrTitleName)
{
    LPCTSTR szFilter = _T("All Files (*.*)\0*.*\0\0");
    OPENFILENAME ofn = { sizeof(ofn) };

    ofn.hwndOwner = hwnd;
    ofn.hInstance = m_mainhInst;
    ofn.lpstrFilter = szFilter;
    ofn.lpstrFile = pstrFileName;
    ofn.lpstrFileTitle = pstrTitleName;
    ofn.lpfnHook = OpenHookProc;

    ofn.nFilterIndex = 1;
    ofn.nMaxFile = _MAX_PATH;
    ofn.nMaxFileTitle = _MAX_FNAME + _MAX_EXT;

    ofn.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | 0; //OFN_ENABLEHOOK			;

    return GetOpenFileName(&ofn);
}
BOOL Editor::DoOpenFile(HWND hwndMain, TCHAR* szFileName, TCHAR* szFileTitle)
{
    //int fmt, fmtlook[] = { IDM_VIEW_ASCII, IDM_VIEW_UTF8, IDM_VIEW_UTF16, IDM_VIEW_UTF16BE };

    if (TextView_OpenFile(m_hwndTextView, szFileName)) {
        SetWindowFileName(hwndMain, szFileTitle, FALSE);
        //g_fFileChanged = FALSE;

        //fmt = TextView_GetFormat(g_hwndTextView);

        //CheckMenuRadioItem(GetMenu(hwndMain),
        //    IDM_VIEW_ASCII, IDM_VIEW_UTF16BE,
        //    fmtlook[fmt], MF_BYCOMMAND);

        //NotifyFileChange(szFileName, hwndMain, 0);
        return TRUE;
    } else {
        //FmtErrorMsg(hwndMain, MB_OK | MB_ICONWARNING, GetLastError(), _T("Error opening \'%s\'\r\n\r\n"), szFileName);
        return FALSE;
    }
}

BOOL Editor::GetSettingInt(HKEY hkey, TCHAR szKeyName[], LONG* pnReturnVal, LONG nDefault)
{
    ULONG len = sizeof(nDefault);
    *pnReturnVal = nDefault;
    return !RegQueryValueEx(hkey, szKeyName, 0, 0, (BYTE*)pnReturnVal, &len);
}
BOOL Editor::GetSettingStr(HKEY hkey, TCHAR szKeyName[], TCHAR pszReturnStr[], DWORD nLength, TCHAR szDefault[])
{
    ULONG len = nLength * sizeof(TCHAR);
    TCHAR* rval = lstrcpyn(pszReturnStr, szDefault, nLength);
    return !RegQueryValueEx(hkey, szKeyName, 0, 0, (BYTE*)pszReturnStr, &len);
}
void Editor::LoadRegSettings()
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
int Editor::PointsToLogical(int nPointSize)
{
    HDC hdc = GetDC(0);
    int nLogSize = -MulDiv(nPointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
    ReleaseDC(0, hdc);

    return nLogSize;
}
void Editor::ApplyRegSettings()
{
    if (g_hFont)
        DeleteObject(g_hFont);

    if (!m_hwndTextView)
        return;
    _RPTWN(_CRT_WARN, L"----g_hwndTextView %Xd\n", m_hwndTextView);

    g_hFont = EasyCreateFont(g_nFontSize, g_fFontBold, g_nFontSmoothing, g_szFontName);

    TextView_SetLineSpacing(m_hwndTextView, g_nPaddingAbove, g_nPaddingBelow);
    TextView_SetStyleBool(m_hwndTextView, TXS_SELMARGIN, g_fSelMargin);
    TextView_SetStyleBool(m_hwndTextView, TXS_LINENUMBERS, g_fLineNumbers);
    TextView_SetStyleBool(m_hwndTextView, TXS_LONGLINES, g_fLongLines);
    TextView_SetStyleBool(m_hwndTextView, TXS_HIGHLIGHTCURLINE, g_nHLCurLine);
    TextView_SetCaretWidth(m_hwndTextView, 2);
    TextView_SetLongLine(m_hwndTextView, g_nLongLineLimit);

    SendMessage(m_hwndTextView, WM_SETFONT, (WPARAM)g_hFont, 0);

    for (int i = 0; i < TXC_MAX_COLOURS; i++) {
        TextView_SetColor(m_hwndTextView, i, g_rgbColourList[i]);
    }
}
HFONT Editor::EasyCreateFont(int nPointSize, BOOL fBold, DWORD dwQuality, TCHAR* szFace)
{
    return CreateFont(PointsToLogical(nPointSize),
        0, 0, 0,
        fBold ? FW_BOLD : 0,
        0, 0, 0, DEFAULT_CHARSET, 0, 0,
        dwQuality,
        0,
        szFace);
}
BOOL Editor::GetSettingBin(HKEY hkey, TCHAR szKeyName[], PVOID pBuffer, LONG nLength)
{
    ZeroMemory(pBuffer, nLength);
    return !RegQueryValueEx(hkey, szKeyName, 0, 0, (BYTE*)pBuffer, (LPDWORD)&nLength);
}

LONG WINAPI Editor::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {

    case WM_COMMAND: {
        int wmId = LOWORD(wParam);

        switch (wmId) {

        case IDM_ABOUT:
            break;

        default:
            return CommandHandler(hwnd, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
        }
    } break;

    case WM_SETFOCUS:
        SetFocus(m_hwndTextView);
        return 0;

    case WM_SIZE:
        int width, height, heightsb;
        RECT rect;
        HDWP hdwp;

        width = (short)LOWORD(lParam);
        height = (short)HIWORD(lParam);

        GetWindowRect(m_hwndStatusbar, &rect);
        heightsb = rect.bottom - rect.top;
        hdwp = BeginDeferWindowPos(3);

        if (g_fShowStatusbar) {
            DeferWindowPos(hdwp, m_hwndStatusbar, 0, 0, height - heightsb, width, heightsb, SWP_SHOWWINDOW);
            MoveWindow(m_hwndStatusbar, 0, height - heightsb, width, heightsb, TRUE);
            height -= heightsb;
        }

        DeferWindowPos(hdwp, m_hwndTextView, 0, 0, 0, width, height, SWP_SHOWWINDOW);
        MoveWindow(m_hwndTextView, 0, 0, width, height, TRUE);
        EndDeferWindowPos(hdwp);
        SetStatusBarParts(m_hwndStatusbar);

        return 0;

    case WM_DESTROY:
        return 0;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}
UINT Editor::CommandHandler(HWND hwnd, UINT nCtrlId, UINT nCtrlCode, HWND hwndFrom)
{
    switch (nCtrlId) {
    case IDM_FILE_NEW:
        SetWindowText(hwnd, L"Untitled");
        TextView_Clear(m_hwndTextView);
        //g_szFileTitle[0] = '\0';
        //g_fFileChanged = FALSE;
        return 0;

    case IDM_FILE_OPEN:
        if (ShowOpenFileDlg(hwnd, g_szFileName, g_szFileTitle)) {
            DoOpenFile(hwnd, g_szFileName, g_szFileTitle);
        }

        return 0;
    default:
        return 0;
    }
}

UINT_PTR CALLBACK OpenHookProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_INITDIALOG:

        if (Editor::g_fFirstTime) {
            CenterWindow(GetParent(hwnd));
            Editor::g_fFirstTime = false;
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

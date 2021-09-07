#include "Editor.h"
#include <CRTDBG.H>
#include <initguid.h>
#include <shlguid.h>
#include <shobjidl.h>

BOOL Editor::g_fFirstTime = false;

LPCTSTR encoding[] = {
    _T("UTF-8"),
    _T("UTF-8 BOM"),
    _T("UTF-16"),
    _T("UTF-16BE")
};

Editor::Editor(HINSTANCE hInst, HWND hwnd)
{
    LoadRegSettings();

    m_mainhInst = hInst;
    m_hWnd = hwnd;
    m_hwndTextView = CreateTextView(hwnd);
    m_hwndStatusbar = CreateStatusBar(hwnd);

    TextView* g_ptv = (TextView*)GetWindowLongPtr(hwnd, 0);

    PostMessage(m_hWnd, WM_COMMAND, IDM_FILE_NEW, 0);

    ApplyRegSettings();
}
Editor::~Editor()
{
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

void Editor::InitOpenFile(HWND hwnd, int fmt)
{
    switch (fmt) {
    case IDM_FILE_NEW:
        g_szFileTitle[0] = '\0';
        g_szFileName[0] = '\0';
        TextView_Clear(m_hwndTextView);
        DoOpenFile(hwnd, g_szFileName, g_szFileTitle);
        //g_fFileChanged = FALSE;
        break;

    default:
        if (ShowOpenFileDlg(hwnd, g_szFileName, g_szFileTitle)) {
            DoOpenFile(hwnd, g_szFileName, g_szFileTitle);
        }
        break;
    }
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
    if (_tcslen(szFileName) == 0 || TextView_OpenFile(m_hwndTextView, szFileName)) {
        int fmt, fmtlook[] = { IDM_VIEW_UTF8, IDM_VIEW_UTF8B, IDM_VIEW_UTF16, IDM_VIEW_UTF16BE };

        if (_tcslen(szFileName) == 0)
            SetWindowFileName(hwndMain,(TCHAR*)L"Untitled", FALSE);
        else
            SetWindowFileName(hwndMain, szFileTitle, FALSE);
        //g_fFileChanged = FALSE;
        fmt = TextView_GetFormat(m_hwndTextView);
        CheckMenuRadioItem(GetMenu(hwndMain),
            IDM_VIEW_UTF8, IDM_VIEW_UTF16BE,
            fmtlook[fmt], MF_BYCOMMAND);
        SetStatusBarText(m_hwndStatusbar, 2, 0, (TCHAR*)encoding[fmt]);

        //
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
    GetSettingStr(hKey, (TCHAR*)_T("FontName"), g_szFontName, LF_FACESIZE, (TCHAR*)_T("MS Gothic"));
    GetSettingInt(hKey, (TCHAR*)_T("FontSmooth"), &g_nFontSmoothing, DEFAULT_QUALITY);

    GetSettingInt(hKey, (TCHAR*)_T("PaddingAbove"), &g_nPaddingAbove, 0);
    GetSettingInt(hKey, (TCHAR*)_T("PaddingBelow"), &g_nPaddingBelow, 1);
    GetSettingInt(hKey, (TCHAR*)_T("PaddingFlags"), &g_fPaddingFlags, COURIERNEW | LUCIDACONS);

    GetSettingInt(hKey, (TCHAR*)_T("SelMargin"), (LONG*)&g_fSelMargin, TRUE);
    GetSettingInt(hKey, (TCHAR*)_T("LineNumbers"), (LONG*)&g_fLineNumbers, TRUE);
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
void Editor::SetWindSize(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
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
}
void Editor::OpenFile(HWND hwnd, TCHAR* szFile)
{
    TCHAR* name;
    //SaveFileData(g_szFileName, hwnd);
    ///_CRT_SECURE_NO_DEPRECATE
    //_tcscpy(g_szFileName, szFile);
    //name = _tcsrchr(g_szFileName, '\\');
    //_tcscpy(g_szFileTitle, name ? name + 1 : szFile);

    _tcscpy_s(g_szFileName, MAX_PATH, szFile);
    name = _tcsrchr(g_szFileName, '\\');
    _tcscpy_s(g_szFileTitle, MAX_PATH, name ? name + 1 : szFile);

    DoOpenFile(hwnd, g_szFileName, g_szFileTitle);
}
void Editor::HandleDropFiles(HWND hwnd, HDROP hDrop)
{
    TCHAR buf[MAX_PATH];

    if (DragQueryFile(hDrop, 0, buf, MAX_PATH)) {
        TCHAR tmp[MAX_PATH];
        if (ResolveShortcut(buf, tmp, MAX_PATH))
            lstrcpy(buf, tmp);

        OpenFile(hwnd, buf);
    }

    DragFinish(hDrop);
}
BOOL Editor::ResolveShortcut(TCHAR* pszShortcut, TCHAR* pszFilePath, int nPathLen)
{
    //IShellLink* psl;
    //SHFILEINFO info = { 0 };
    //IPersistFile* ppf;

    //*pszFilePath = 0; // assume failure

    //// retrieve file's shell-attributes
    //if ((SHGetFileInfo(pszShortcut, 0, &info, sizeof(info), SHGFI_ATTRIBUTES) == 0))
    //    return FALSE;

    //// not a shortcut?
    //if (!(info.dwAttributes & SFGAO_LINK)) {
    //    lstrcpyn(pszFilePath, pszShortcut, nPathLen);
    //    return TRUE;
    //}

    //// obtain the IShellLink interface
    //if (FAILED(CoCreateInstance(&CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, &IID_IShellLink, (LPVOID*)&psl)))
    //    return FALSE;

    //if (SUCCEEDED(psl->lpVtbl->QueryInterface(psl, &IID_IPersistFile, (LPVOID*)&ppf))) {
    //    if (SUCCEEDED(ppf->lpVtbl->Load(ppf, pszShortcut, STGM_READ))) {
    //        // Resolve the link, this may post UI to find the link
    //        if (SUCCEEDED(psl->lpVtbl->Resolve(psl, 0, SLR_NO_UI))) {
    //            psl->lpVtbl->GetPath(psl, pszFilePath, nPathLen, NULL, 0);
    //            ppf->lpVtbl->Release(ppf);
    //            psl->lpVtbl->Release(psl);
    //            return TRUE;
    //        }
    //    }

    //    ppf->lpVtbl->Release(ppf);
    //}

    //psl->lpVtbl->Release(psl);
    return FALSE;
}

UINT Editor::NotifyHandler(HWND hwnd, LPNMHDR nmhdr)
{
    switch (nmhdr->code) {

    case TVN_CHANGED:
        //if (g_szFileTitle[0]) {
        //    BOOL fModified = TextView_CanUndo(g_hwndTextView);

        //    if (fModified != g_fFileChanged) {
        //        SetWindowFileName(hwnd, g_szFileTitle, fModified);
        //        g_fFileChanged = fModified;
        //    }
        //}
        break;

    case TVN_CURSOR_CHANGE:
        SetStatusBarText(m_hwndStatusbar, 1, 0, (TCHAR*)_T(" Ln %d, Col %d"),
            TextView_GetCurLine(m_hwndTextView) + 1,
            TextView_GetCurCol(m_hwndTextView) + 1);

        break;

    case TVN_EDITMODE_CHANGE:

        break;

    default:
        break;
    }

    return 0;
}

LONG WINAPI Editor::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {

    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
        case IDM_ABOUT:
            break;
        default:
            return CommandHandler(hwnd, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
        }
    } break;

    case WM_SETFOCUS: {
        SetFocus(m_hwndTextView);
    } break;

    case WM_SIZE: {
        SetWindSize(hwnd, msg, wParam, lParam);
    } break;

    case WM_NOTIFY: {
        NotifyHandler(hwnd, (LPNMHDR)lParam);
    } break;

    case WM_DROPFILES: {
        HandleDropFiles(hwnd, (HDROP)wParam);
    } break;

    case WM_DESTROY: {
        DeleteObject(g_hFont);
    } break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}
UINT Editor::CommandHandler(HWND hwnd, UINT nCtrlId, UINT nCtrlCode, HWND hwndFrom)
{
    switch (nCtrlId) {
    case IDM_FILE_NEW:
        InitOpenFile(hwnd, IDM_FILE_NEW);
        return 0;

    case IDM_FILE_OPEN:
        InitOpenFile(hwnd, IDM_FILE_OPEN);
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

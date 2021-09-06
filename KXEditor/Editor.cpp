#include "Editor.h"
#include "../TextView/TextView_def.h"
#include "StatusBar.h"
#include "resource.h"

BOOL g_fFirstTime = true;

Editor::Editor(HINSTANCE hInst, HWND hwnd)
{
    m_mainhInst = hInst;
    m_hWnd = hwnd;
    m_hwndTextView = CreateTextView(hwnd);
    m_hwndStatusbar = CreateStatusBar(hwnd);

    PostMessage(m_hWnd, WM_COMMAND, IDM_FILE_NEW, 0);
}
Editor::~Editor()
{
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

    // flags to control appearance of open-file dialog
    ofn.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | 0; //OFN_ENABLEHOOK			;

    return GetOpenFileName(&ofn);
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

        if (m_fShowStatusbar) {
            DeferWindowPos(hdwp, m_hwndStatusbar, 0, 0, height - heightsb, width, heightsb, SWP_SHOWWINDOW);
            MoveWindow(m_hwndStatusbar, 0, height - heightsb, width, heightsb, TRUE);
            height -= heightsb;
        }

        DeferWindowPos(hdwp, m_hwndTextView, 0, 0, 0, width, height, SWP_SHOWWINDOW);
        MoveWindow(m_hwndTextView, 0, 0, width, height, TRUE);
        EndDeferWindowPos(hdwp);
        SetStatusBarParts(m_hwndStatusbar);

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

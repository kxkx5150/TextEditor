//
//	Neatpad - Simple Text Editor application
//
//	www.catch22.net
//	Written by J Brown 2004-2006
//
//	Freeware
//

#include "StatusBar.h"
#include "KXEditor.h"

DWORD dwStatusBarStyles = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NOPARENTALIGN | CCS_NOMOVEY | //| CCS_NORESIZE
    SBT_NOBORDERS | SBARS_SIZEGRIP;

//	Process WM_MENUSELECT message to display menu-item hints in statusbar
//
int StatusBarMenuSelect(HWND hwnd, HWND hwndSB, WPARAM wParam, LPARAM lParam)
{
    //UINT lookup[] = { 0, 0 };

    //// Display helpful text in status bar
    //MenuHelp(WM_MENUSELECT, wParam, lParam, GetMenu(hwnd), g_hResourceModule,
    //	hwndSB, (UINT *)lookup);

    return 0;
}

//
//	Create each menubar pane. Must be called whenever the statusbar changes size,
//  so call each time the main-window gets a WM_SIZE
//
void SetStatusBarParts(HWND hwndSB)
{
    RECT r;
    HWND hwndParent = GetParent(hwndSB);
    int parts[MAX_STATUS_PARTS];
    int parentwidth;

    GetClientRect(hwndParent, &r);

    parentwidth = r.right < 400 ? 400 : r.right;
    parts[0] = parentwidth - 250;
    parts[1] = parentwidth - 70;
    parts[2] = parentwidth; //-1;

    // Tell the status bar to create the window parts.
    SendMessage(hwndSB, SB_SETPARTS, MAX_STATUS_PARTS, (LPARAM)parts);
}

//
//	sprintf-style wrapper for setting statubar pane text
//
void SetStatusBarText(HWND hwndSB, UINT nPart, UINT uStyle, TCHAR* fmt, ...)
{
    TCHAR tmpbuf[100];
    va_list args;
    va_start(args, fmt);
    vswprintf(tmpbuf,100, fmt, args);
    va_end(args);
    SendMessage(hwndSB, SB_SETTEXT, (WPARAM)(nPart | uStyle), (LPARAM)(LPCWSTR)tmpbuf);
}

//
//	Create Neatpad's statusbar
//
HWND CreateStatusBar(HWND hwndParent)
{
    HWND hwndSB;
    hwndSB = CreateStatusWindow(dwStatusBarStyles, _T(""), hwndParent, 2);
    SetStatusBarParts(hwndSB);
    TCHAR tc1[] = _T("");
    TCHAR tc2[] = _T(" Ln %d, Col %d");
    TCHAR tc3[] = _T(" INS");
    SetStatusBarText(hwndSB, 0, 1, tc1);
    SetStatusBarText(hwndSB, 1, 0, tc2, 1, 1);
    SetStatusBarText(hwndSB, 2, 0, tc3);
    return hwndSB;
}

#include "Editor.h"
#include "../TextView/TextView_def.h"
#include "StatusBar.h"
#include "resource.h"

Editor::Editor(HWND hwnd)
{
    m_hWnd = hwnd;
    m_hwndTextView = CreateTextView(hwnd);
    m_hwndStatusbar = CreateStatusBar(hwnd);
}
Editor::~Editor()
{
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
            return DefWindowProc(hwnd, msg, wParam, lParam);
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

        //if (g_fShowStatusbar)
        //{
        //	DeferWindowPos(hdwp, g_hwndStatusbar, 0, 0, height - heightsb, width, heightsb, SWP_SHOWWINDOW);
        //	//	MoveWindow(g_hwndStatusbar, 0, height - heightsb, width, heightsb, TRUE);
        //	height -= heightsb;
        //}

        DeferWindowPos(hdwp, m_hwndTextView, 0, 0, 0, width, height, SWP_SHOWWINDOW);
        //MoveWindow(g_hwndTextView, 0, 0, width, height, TRUE);

        EndDeferWindowPos(hdwp);

        //SetStatusBarParts(g_hwndStatusbar);

        return 0;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

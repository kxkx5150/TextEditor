#pragma once
#include <windows.h>
#include <commctrl.h>
#include <stdarg.h>
#include <tchar.h>

#define _CRT_SECURE_NO_DEPRECATE
#define MAX_STATUS_PARTS 3

int StatusBarMenuSelect(HWND hwnd, HWND hwndSB, WPARAM wParam, LPARAM lParam);
void SetStatusBarParts(HWND hwndSB);
void SetStatusBarText(HWND hwndSB, UINT nPart, UINT uStyle, TCHAR* fmt, ...);
HWND CreateStatusBar(HWND hwndParent);

#pragma once
#define WIN32_LEAN_AND_MEAN
#include "resource.h"
#include "targetver.h"
#include <malloc.h>
#include <memory.h>
#include <stdlib.h>
#include <tchar.h>
#include <windows.h>
#include "../TextView/TextView_def.h"
#include "Editor.h"
#include "StatusBar.h"

#define MAX_LOADSTRING 100
#define APP_TITLE _T("KXEditor")
#define REGLOC _T("SOFTWARE\\kxkx5150\\kxeditor")

#define COURIERNEW 1
#define LUCIDACONS 2

ATOM EditorClass(HINSTANCE hInstance);
BOOL InitEditorClass(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

BOOL GetSettingInt(HKEY hkey, TCHAR szKeyName[], LONG* pnReturnVal, LONG nDefault);
BOOL GetSettingStr(HKEY hkey, TCHAR szKeyName[], TCHAR pszReturnStr[], DWORD nLength, TCHAR szDefault[]);
BOOL GetSettingBin(HKEY hkey, TCHAR szKeyName[], PVOID pBuffer, LONG nLength);
void LoadRegSettings();
void ApplyRegSettings();

HFONT EasyCreateFont(int nPointSize, BOOL fBold, DWORD dwQuality, TCHAR* szFace);
int PointsToLogical(int nPointSize);


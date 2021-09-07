#pragma once
#define WIN32_LEAN_AND_MEAN
#include "resource.h"
#include "targetver.h"
#include <malloc.h>
#include <memory.h>
#include <stdlib.h>
#include <tchar.h>
#include <windows.h>
#include "Editor.h"

#define MAX_LOADSTRING 100
#define APP_TITLE _T("KXEditor")

ATOM EditorClass(HINSTANCE hInstance);
BOOL InitEditorClass(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

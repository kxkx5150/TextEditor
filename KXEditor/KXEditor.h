#pragma once
#define WIN32_LEAN_AND_MEAN
#include "targetver.h"
#include "resource.h"
#include <malloc.h>
#include <memory.h>
#include <stdlib.h>
#include <tchar.h>
#include <windows.h>

#define MAX_LOADSTRING 100
#define APP_TITLE _T("KXEditor")
#define REGLOC _T("SOFTWARE\\kxkx5150\\kxeditor")

ATOM EditorClass(HINSTANCE hInstance);
BOOL InitEditorClass(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

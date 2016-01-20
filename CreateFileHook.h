#pragma once
#include "dinput8.h"

typedef HANDLE(WINAPI *tCreateFileW)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
extern tCreateFileW oCreateFileW;
HANDLE WINAPI HCreateFileW(LPCWSTR fileName, DWORD access, DWORD share, LPSECURITY_ATTRIBUTES sec, DWORD disp, DWORD flags, HANDLE templ);
#pragma once

namespace Win32 {

    enum WindowType : DWORD {
        STATIC      = WS_OVERLAPPED,
        RESIZEABLE  = WS_SIZEBOX,
        POPUP       = WS_POPUP
    };

    namespace Utils {

        BOOL AddBitmap(CONST WCHAR* szFileName, HDC hWinDC, INT x = 0, INT y = 0);

        /// Sets and clears style flags for a particular window.
        inline VOID ModifyWindowStyle(HWND hWnd, DWORD flagsToDisable, DWORD flagsToEnable)
        {
            DWORD style = GetWindowLong(hWnd, GWL_STYLE);
            SetWindowLong(hWnd, GWL_STYLE, (style & ~flagsToDisable) | flagsToEnable);
        }

        /// Sets and clears extended style flags for a particular window.
        inline VOID ModifyWindowExStyle(HWND hWnd, DWORD flagsToDisable, DWORD flagsToEnable)
        {
            DWORD exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
            SetWindowLong(hWnd, GWL_EXSTYLE, (exStyle & ~flagsToDisable) | flagsToEnable);
        }


        inline BOOL HasStyle(HWND hwnd, DWORD style) {
            DWORD dwStyle = (DWORD)GetWindowLong(hwnd, GWL_STYLE);
            if ((dwStyle & style) != 0) return TRUE;
            return FALSE;
        }


        /// Sets and clears style flags for a particular window.
        inline VOID ModifyClassStyle(HWND hWnd, DWORD flagsToDisable, DWORD flagsToEnable)
        {
            DWORD style = GetWindowLong(hWnd, GCL_STYLE);
            SetClassLong(hWnd, GCL_STYLE, (style & ~flagsToDisable) | flagsToEnable);
        }

        inline BOOL IsWindowFullscreen(HWND hWnd)
        {
            WINDOWPLACEMENT placement = { };
            GetWindowPlacement(hWnd, &placement);

            if (placement.showCmd == SW_MAXIMIZE)
                return TRUE;
            return FALSE;
        }

        inline VOID MaximizeWindow(HWND hwnd) {
            WINDOWPLACEMENT wPos = { };
            GetWindowPlacement(hwnd, &wPos);
            if (wPos.showCmd == SW_MAXIMIZE) ShowWindow(hwnd, SW_NORMAL);
            else  ShowWindow(hwnd, SW_MAXIMIZE);
        }
    }
}
#include"window.h"
WNDPROC originalWndProc = nullptr;
// 全局变量：保存原始的窗口样式和扩展样式
DWORD originalStyle = 0;
DWORD originalExStyle = 0;



void EnableMousePassthrough(HWND hwnd)
{
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    exStyle |= WS_EX_TRANSPARENT | WS_EX_LAYERED;
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);
}

void DisableMousePassthrough(HWND hwnd)
{
    
    // 恢复标准样式
    SetWindowLongPtr(hwnd, GWL_STYLE, originalStyle);

    // 恢复扩展样式
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, originalExStyle);

    // 刷新窗口布局以应用更改
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    
}

// 保存当前窗口的样式和扩展样式
void SaveWindowStyles(HWND hwnd) {
    // 保存标准样式
    originalStyle = GetWindowLongPtr(hwnd, GWL_STYLE);

    // 保存扩展样式
    originalExStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
}


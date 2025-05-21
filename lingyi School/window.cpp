#include"window.h"
WNDPROC originalWndProc = nullptr;
// ȫ�ֱ���������ԭʼ�Ĵ�����ʽ����չ��ʽ
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
    
    // �ָ���׼��ʽ
    SetWindowLongPtr(hwnd, GWL_STYLE, originalStyle);

    // �ָ���չ��ʽ
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, originalExStyle);

    // ˢ�´��ڲ�����Ӧ�ø���
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    
}

// ���浱ǰ���ڵ���ʽ����չ��ʽ
void SaveWindowStyles(HWND hwnd) {
    // �����׼��ʽ
    originalStyle = GetWindowLongPtr(hwnd, GWL_STYLE);

    // ������չ��ʽ
    originalExStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
}


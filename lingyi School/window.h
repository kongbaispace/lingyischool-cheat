#pragma once
#include<glfw3.h>
#include<Windows.h>
// ������괩͸
void EnableMousePassthrough(HWND hwnd);
// ������괩͸���ָ�ԭʼ���ڹ���
void DisableMousePassthrough(HWND hwnd);

// ���浱ǰ���ڵ���ʽ����չ��ʽ
void SaveWindowStyles(HWND hwnd);
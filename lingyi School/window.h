#pragma once
#include<glfw3.h>
#include<Windows.h>
// 启用鼠标穿透
void EnableMousePassthrough(HWND hwnd);
// 禁用鼠标穿透并恢复原始窗口过程
void DisableMousePassthrough(HWND hwnd);

// 保存当前窗口的样式和扩展样式
void SaveWindowStyles(HWND hwnd);
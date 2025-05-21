#pragma once
#include<imgui/imgui.h>
#include"imgui/imgui_internal.h"
#include"imgui/imstb_rectpack.h"
#include"imgui/imstb_textedit.h"
#include<imgui/imgui_impl_glfw.h>
#include<imgui/imgui_impl_opengl3.h>
#include"handle/handle.h"

extern HANDLE hp;
extern DWORD64 base;
void menu();//≤Àµ•‰÷»æ

#define U8(Str) reinterpret_cast<const char*>((u8##Str))
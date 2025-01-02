#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define __STDC_LIB_EXT1__
#define STB_IMAGE_WRITE_IMPLEMENTATION


#include <regex>
#include "stb_image_write.h"
#include"glad/glad.h"
#include"glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "glfw3native.h"

#include<atlimage.h>

#include<Windows.h>
#include<mmsystem.h>
#include<tchar.h>
#include<sstream>
#include<cstring>
#include<iostream>
#include<format>
#include <TlHelp32.h>
#include <exception>
#include <psapi.h>
#include"window.h"

#include"menu.h"

#include"handle\handle.h"
#pragma comment(lib, "winmm.lib")
bool Keykg = false;
bool KeyE = false;
bool menudone = true;

extern bool dg;

DWORD pid=0;
HANDLE hp=nullptr;
RECT rect;
DWORD64 base;
DWORD64 uworld;
float matrix[16]{ 0 };
vec3 clipCoords, pos;
vec3 xyz{ 0,0,0 }, play{0};

float sy = 0.0f;
int sy2 = 0;

HWND mywindow=nullptr;  //原生win32句柄

int width = 0;
int height = 0;

// 全局变量：跟踪鼠标穿透状态
bool isMousePassthroughEnabled = false;

// 全局变量：记录 Insert 键是否被按下
bool isInsertPressed = false;

void ResizeWindowToMatchClientArea(GLFWwindow* window, HWND targetWindow) {
    // 获取目标窗口的客户区矩形
    RECT clientRect;
    GetClientRect(targetWindow, &clientRect);

    // 将客户区矩形转换为屏幕坐标
    POINT topLeft = { clientRect.left, clientRect.top };
    ClientToScreen(targetWindow, &topLeft);
    POINT bottomRight = { clientRect.right, clientRect.bottom };
    ClientToScreen(targetWindow, &bottomRight);

    // 计算客户区的宽度和高度
    int width = bottomRight.x - topLeft.x;
    int height = bottomRight.y - topLeft.y;

    // 设置窗口位置
    glfwSetWindowPos(window, topLeft.x, topLeft.y);

    // 设置窗口大小（客户区大小）
    glfwSetWindowSize(window, width, height);
}

void onPlusPressed(HANDLE hp)
{
    uint64_t py = Read<uint64_t>(hp, base+0x4B73F58);
    ReadProcessMemory(hp, (void*)(py + 0x904), &sy, 4, 0);
    sy2 = (int)sy;
    sy2++;
    sy = (float)sy2;
    WriteProcessMemory(hp, (void*)(py + 0x904), &sy, 4, 0);

}

void onMinusPressed(HANDLE hp)
{
    uint64_t py = Read<uint64_t>(hp, base + 0x4B73F58);
    ReadProcessMemory(hp, (void*)(py + 0x904), &sy, 4, 0);
    sy2 = (int)sy;
    sy2--;
    sy = (float)sy2;
    WriteProcessMemory(hp, (void*)(py + 0x904), &sy, 4, 0);
}

void onSpacePressed();

void onSpaceReleased();

RECT getRect(HWND hwnd) {
    RECT rect;
    GetWindowRect(hwnd, &rect);
    return rect;
}

std::string getnameforid(uint32_t id)
{
    FNameEntryHandle Handle(id);

    uint64_t nameptr = Read<__int64>(hp, base + 0x49F3280 + 0x10 + (Handle.Block * 8));
    if (!nameptr)
        return "NULL!";
    nameptr += 0x2 * Handle.Offset;
    uint32_t namelen = Read<uint16_t>(hp, nameptr) >> 6;

    CHAR name[1024];

    ReadProcessMemory(hp, (void*)(nameptr + 0x2), name, namelen, NULL);

    return std::string(name, namelen);
}

void getgworld();

void getmatrix();

void drawline();

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            switch (p->vkCode) {
            case VK_OEM_PLUS: // 主键盘上的 =
                onPlusPressed(hp);
                break;
            case VK_OEM_MINUS: // 主键盘上的 -
                onMinusPressed(hp);
                break;
            case VK_SPACE: // 空格键按下
                onSpacePressed();
                break;
            case VK_INSERT: // Insert 键按下
                isInsertPressed = true;
                break;
            }
        }
        else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
            switch (p->vkCode) {
            case VK_SPACE: // 空格键松开
                onSpaceReleased();
                break;
            case VK_INSERT: // Insert 键松开
                if (isInsertPressed) {
                    // 切换鼠标穿透状态
                    isMousePassthroughEnabled = !isMousePassthroughEnabled;

                    // 调用相应的函数
                    if (isMousePassthroughEnabled) {
                        EnableMousePassthrough(mywindow);
                        menudone = false;
                    }
                    else {
                        DisableMousePassthrough(mywindow);
                        menudone = true;
                    }

                    // 重置 Insert 按键状态
                    isInsertPressed = false;
                }
                break;
            }
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void framebuffer_size_callback(GLFWwindow* window, int x, int y) {
    width = x;
    height = y;
}

int main(int argc, char* argv[])
{

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_FLOATING, true);
    glfwWindowHint(GLFW_RESIZABLE, false);
    glfwWindowHint(GLFW_MAXIMIZED, true);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);
    //glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);          //鼠标穿透
    
    GLFWwindow* window = glfwCreateWindow(1000, 800, "我的窗口", NULL, NULL);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    HWND targetWindow = FindWindow(L"UnrealWindow", L"MysteriousSchool  ");
    pid = GetPID(L"MysteriousSchool-Win64-Shipping.exe");
    hp = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    base = GetModuleBase(L"MysteriousSchool-Win64-Shipping.exe", hp);
    std::thread gameworld(getgworld);   //获取win32原生窗口句柄
    gameworld.detach();                           //开启线程 读取gworld

    glfwSetWindowAttrib(window, GLFW_DECORATED, false);
    HHOOK hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);    //设置键盘事件
    mywindow = glfwGetWin32Window(window);
    SaveWindowStyles(mywindow);

    ShowWindow(GetConsoleWindow(), SW_HIDE);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.IniFilename = nullptr;
    
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0, 0, 0, 120);

    ImGui::StyleColorsDark();
    ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\simkai.ttf", 15.0f, nullptr, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    ImVec4 clear_color = ImVec4(1.0f, 1.0f, 1.0f, 0.00f);
    
    

    printf("Base:%p\n",base);
    while (!glfwWindowShouldClose(window)) {
        
        
        if (targetWindow != NULL) {
            rect = getRect(mywindow);

            //glfwSetWindowPos(window, rect.left, rect.top);
            //glfwSetWindowSize(window, rect.right - rect.left, rect.bottom - rect.top);
            ResizeWindowToMatchClientArea(window, targetWindow);

            glfwSetWindowAttrib(window, GLFW_FOCUSED, GLFW_TRUE);
            SetWindowPos(glfwGetWin32Window(window), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        glClearColor(clear_color.x * clear_color.w,
            clear_color.y * clear_color.w,
            clear_color.z * clear_color.w,
            clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        getmatrix();
        printf("%p\n",getmyselfaddress(hp, base));
        
        drawline();

        if (menudone)
        {
            menu();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
    UnhookWindowsHookEx(hHook);
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
        return 0;
}

void getmatrix()
{
    uint64_t 缓冲1 = 0, 缓冲2 = 0;
    缓冲1 = Read<uint64_t>(hp, base + 0x0466F3E0);
    缓冲2 = Read<uint64_t>(hp, 缓冲1 + 0x20);
    ReadProcessMemory(hp, (void*)(缓冲2 + 0x280), matrix, 64, 0);

  /*  for (int i = 0; i < 16; i++)
    {
        printf("%f  ",matrix[i]);
        if (i + 1 % 4 == 0)
        {
            printf("\n");
        }
    }
    */system("cls");   //遍历矩阵
}

void drawline()
{

    DWORD64 ulevel = Read<uint64_t>(hp, uworld + 0x30);
    DWORD64 Array = Read<DWORD64>(hp, ulevel + 0x98);
    int count = Read<int>(hp, ulevel + 0x98 + 8);
    float vorx, vory, vorz, vory2, row;

    float ld=0;
    uint64_t py = Read<uint64_t>(hp, base + 0x4B73F58);
    ReadProcessMemory(hp, (void*)(py + 0x904), &ld, 4, 0);
    std::string liangdu="当前亮度: ";
    liangdu += std::to_string((int)ld);
    ImGui::GetForegroundDrawList()->AddText({40,(float)height-40}, ImColor(0, 246, 0), liangdu.c_str());


    for (int i = 0; i < count; i++)
    {
        DWORD64 objectaddress = Read<DWORD64>(hp, Array + (i * 8));

        uint32_t objectid = Read<uint32_t>(hp, (objectaddress + 0x18));

        DWORD64 roootcomp = Read<uint64_t>(hp, objectaddress + 0x130);
        ReadProcessMemory(hp, (void*)(roootcomp + 0x1d0), &pos.x, 4, 0);
        ReadProcessMemory(hp, (void*)(roootcomp + 0x1d0 + (0x4 * 1)), &pos.y, 4, 0);
        ReadProcessMemory(hp, (void*)(roootcomp + 0x1d0 + (0x4 * 2)), &pos.z, 4, 0);

       

        clipCoords.x = pos.x * matrix[0] + pos.y * matrix[4] + pos.z * matrix[8] + matrix[12];
        clipCoords.y = pos.x * matrix[1] + pos.y * matrix[5] + pos.z * matrix[9] + matrix[13];
        clipCoords.z = pos.x * matrix[2] + pos.y * matrix[6] + pos.z * matrix[10] + matrix[14];
        clipCoords.w = pos.x * matrix[3] + pos.y * matrix[7] + pos.z * matrix[11] + matrix[15];


            
        vec3 NDC;
        NDC.x = clipCoords.x / clipCoords.w;
        NDC.y = clipCoords.y / clipCoords.w;
        NDC.z = clipCoords.z / clipCoords.w;
        float x = (width / 2 * NDC.x) + (NDC.x + width / 2);
        float y = -(height / 2 * NDC.y) + (NDC.y + height / 2);

        vorz = pos.x * matrix[3] + pos.y * matrix[7] + pos.z * matrix[11] + matrix[15];
        row = 1 / vorz;
        vorx = (width/2) + (pos.x * matrix[0] + pos.y * matrix[4] + pos.z * matrix[8] + matrix[12]) * row * (width / 2);
        vory = (height / 2) - (pos.x * matrix[1] + pos.y * matrix[5] + pos.z * matrix[9] + matrix[13]) * row * (height/2);
        vory2= (height / 2) - (pos.x * matrix[1] + pos.y * matrix[5] + (pos.z + 160) * matrix[9] + matrix[13]) * row * (height / 2);
        vec3 ckzpm;
        ckzpm.x = vorx - (vory2 - vory) / 4;
        ckzpm.y = vory - (vory2 - vory) / 2;
        ckzpm.z=vory2-vory;
        ckzpm.w=(vory2-vory)/2;
        std::string objectName = getnameforid(objectid).c_str();
        
        std::regex e("ghost", std::regex_constants::icase);
        if (std::regex_search(objectName, e))
        ImGui::GetForegroundDrawList()->AddText({ (float)(width/2-20),100 }, ImColor(255, 255, 239), "有鬼小心!");
        


        std::regex fz("fuzhou", std::regex_constants::icase);
        std::regex xlg("gui", std::regex_constants::icase);
        std::regex player("char", std::regex_constants::icase);
        std::regex playerlinghun("linghun", std::regex_constants::icase);
        if (clipCoords.w < 0.001f)
            continue;
        
        if (std::regex_search(objectName, e))
        {
            /*WriteProcessMemory(hp, (void*)(roootcomp + 0x1d0), &xyz.x, 4, 0);
            WriteProcessMemory(hp, (void*)(roootcomp + 0x1d0 + (0x4 * 1)), &xyz.y, 4, 0);
            WriteProcessMemory(hp, (void*)(roootcomp + 0x1d0 + (0x4 * 2)), &xyz.z, 4, 0);*/
            ImGui::GetForegroundDrawList()->AddRect({ ckzpm.x + ckzpm.w ,ckzpm.y }, { ckzpm.x ,ckzpm.y + ckzpm.z }, ImColor(255, 0, 0));
            ImGui::GetForegroundDrawList()->AddText({ ckzpm.x + (+ckzpm.w / 2) ,ckzpm.y + ckzpm.z - 15.0f }, ImColor(255, 0, 0), "死鬼");
            ImGui::GetForegroundDrawList()->AddLine({(float)width/2,0},{ ckzpm.x + (+ckzpm.w / 2) ,ckzpm.y + ckzpm.z - 15.0f },ImColor(255,255,255));
        }
        

        if (std::regex_search(objectName, player))
        {
            const DWORD64 HEALTH_OFFSET = 0x7AC;

            // 检查玩家指针是否匹配
            if (objectaddress != getmyselfaddress(hp, base)) 
            {
                // 绘制矩形和文本
                ImGui::GetForegroundDrawList()->AddRect(
                    ImVec2(ckzpm.x + ckzpm.w, ckzpm.y),
                    ImVec2(ckzpm.x, ckzpm.y + ckzpm.z),
                    IM_COL32(0, 225, 0, 255)
                );
                ImGui::GetForegroundDrawList()->AddText(
                    ImVec2(ckzpm.x + (ckzpm.w / 2), ckzpm.y + ckzpm.z - 15.0f),
                    IM_COL32(0, 255, 239, 255),
                    "玩家"
                );

                // 设置玩家的最大电量和当前电量
            }
            if (objectaddress == getmyselfaddress(hp, base)) {
                float maxHealth = 10000.0f;
                WriteProcessMemory(hp, (void*)(objectaddress + HEALTH_OFFSET - sizeof(float)), &maxHealth, sizeof(float), nullptr);
                WriteProcessMemory(hp, (void*)(objectaddress + HEALTH_OFFSET), &maxHealth, sizeof(float), nullptr);
            }
        }
        if (std::regex_search(objectName, playerlinghun))
        {
            ImGui::GetForegroundDrawList()->AddRect({ ckzpm.x + ckzpm.w ,ckzpm.y }, { ckzpm.x ,ckzpm.y + ckzpm.z }, ImColor(0, 225, 0));
            ImGui::GetForegroundDrawList()->AddText({ ckzpm.x + (+ckzpm.w / 2) ,ckzpm.y + ckzpm.z - 15.0f }, ImColor(0, 255, 239), "灵魂");
            ImGui::GetForegroundDrawList()->AddLine({ (float)width / 2,0 }, { ckzpm.x + (+ckzpm.w / 2) ,ckzpm.y + ckzpm.z - 15.0f }, ImColor(255, 255, 255));
        }
        if (std::regex_search(objectName, fz))
        {
            ImGui::GetForegroundDrawList()->AddText({ x,y }, ImColor(0, 255, 239), "符咒");
        }

        if (std::regex_search(objectName, xlg))
        {
            /*WriteProcessMemory(hp, (void*)(roootcomp + 0x1d0), &xyz.x, 4, 0);
            WriteProcessMemory(hp, (void*)(roootcomp + 0x1d0 + (0x4 * 1)), &xyz.y, 4, 0);
            WriteProcessMemory(hp, (void*)(roootcomp + 0x1d0 + (0x4 * 2)), &xyz.z, 4, 0);*/
            ImGui::GetForegroundDrawList()->AddRect({ ckzpm.x + ckzpm.w ,ckzpm.y }, { ckzpm.x ,ckzpm.y + ckzpm.z }, ImColor(0, 225, 0));
            ImGui::GetForegroundDrawList()->AddText({ ckzpm.x + (+ckzpm.w / 2) ,ckzpm.y + ckzpm.z - 15.0f }, ImColor(0, 255, 239), "巡逻鬼");
            ImGui::GetForegroundDrawList()->AddLine({ (float)width / 2,0 }, { ckzpm.x + (+ckzpm.w / 2) ,ckzpm.y + ckzpm.z - 15.0f }, ImColor(255, 255, 255));
        }
        //ImGui::GetForegroundDrawList()->AddText({ x,y }, ImColor(0, 255, 239), objectName.c_str());
        std::regex danren("danren", std::regex_constants::icase);
        if (std::regex_search(objectName, danren))
        {
            ImGui::GetForegroundDrawList()->AddLine({ (float)width/2,0 }, { ckzpm.x + (+ckzpm.w / 2) ,ckzpm.y + ckzpm.z - 15.0f }, ImColor(255, 255, 255));
            ImGui::GetForegroundDrawList()->AddRect({ ckzpm.x+ckzpm.w ,ckzpm.y }, { ckzpm.x ,ckzpm.y + ckzpm.z },ImColor(0,225,0));
            //ImGui::GetForegroundDrawList()->AddLine({ ckzpm.x ,ckzpm.y }, { ckzpm.x+ckzpm.w ,ckzpm.y }, ImColor(255, 0, 0));
            //ImGui::GetForegroundDrawList()->AddLine({ ckzpm.x+ckzpm.w ,ckzpm.y }, { ckzpm.x + ckzpm.w ,ckzpm.y + ckzpm.z }, ImColor(255, 0, 0)); //从左下到左上
            //ImGui::GetForegroundDrawList()->AddLine({ ckzpm.x + ckzpm.w ,ckzpm.y + ckzpm.z }, { ckzpm.x ,ckzpm.y + ckzpm.z }, ImColor(255, 0, 0)); //从左上到右上
            //ImGui::GetForegroundDrawList()->AddLine({ ckzpm.x ,ckzpm.y + ckzpm.z }, { ckzpm.x ,ckzpm.y }, ImColor(255, 0, 0));   //从右上到右下
            //ImGui::GetForegroundDrawList()->AddLine({ ckzpm.x + (+ckzpm.w / 2) ,ckzpm.y + ckzpm.z }, { ckzpm.x + (+ckzpm.w / 2) ,ckzpm.y }, ImColor(0,255,0));       //从头到脚
            ImGui::GetForegroundDrawList()->AddText({ ckzpm.x + (+ckzpm.w/2) ,ckzpm.y + ckzpm.z-15.0f}, ImColor(0, 255, 239), "NPC");
        }
        
        
        std::regex ai("ai", std::regex_constants::icase);
        if (dg) {
            if (std::regex_search(objectName, ai)) //关闭寻路
            {
                bool z = 0;
                DWORD64 py = Read<uint64_t>(hp, objectaddress + 0x2d8);
                py = Read<uint64_t>(hp, py + 0xE8);
                WriteProcessMemory(hp, (void*)(py + 0x168), &z, 1, 0);
            }
        }

        else if (!dg) {
            if (std::regex_search(objectName, ai))
            {
                bool z = 1;                        //开启寻路
                DWORD64 py = Read<uint64_t>(hp, objectaddress + 0x2d8);
                py = Read<uint64_t>(hp, py + 0xE8);
                WriteProcessMemory(hp, (void*)(py + 0x168), &z, 1, 0);
            }
        }

        std::regex sdt("shoudiantong", std::regex_constants::icase);
        if (std::regex_search(objectName, sdt))
        {
            ImGui::GetForegroundDrawList()->AddText({ x,y }, ImColor(0, 255, 239), std::to_string(objectaddress).c_str());
        }



    }
}

void onSpacePressed()
{
    uint64_t address = getmyselfaddress(hp, base);
    unsigned short write = 0x0;
    WriteProcessMemory(hp, (void*)(address + 0x5C),&write,2,0);

}

void onSpaceReleased()
{
    uint64_t address = getmyselfaddress(hp, base);
    unsigned short write = 0x89;
    WriteProcessMemory(hp, (void*)(address + 0x5C), &write, 2, 0);
}

void getgworld()
{
    while (1)
    {
        uworld = Read<uint64_t>(hp, base + 0x4B77870);
    }
}
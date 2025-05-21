#include"menu.h"


bool c1=false;
bool dg = false;	//定怪

float changjinld = 5.f;	  //场景亮度
float tygd = 350.f;                  //跳跃高度
float ydsd = 400.f;                    //移动速度

int jumpcount = 1;


void liangdu()						  //亮度
{
	uint64_t py = Read<uint64_t>(hp, base + 0x4B73F58);
	WriteProcessMemory(hp, (void*)(py + 0x904), &changjinld, 4, 0);
}

void tiaoye()				 //跳跃次数
{
	WriteProcessMemory(hp, (void*)(getmyselfaddress(hp, base) + 0x344), &jumpcount, 4, 0);
}

void tiaoygd()				//跳跃高度
{
	uint64_t py = Read<uint64_t>(hp, getmyselfaddress(hp, base) + 0x288);
	WriteProcessMemory(hp, (void*)(py + 0x158), &tygd, 4, 0);
}

void downguanxin()			//开启惯性
{
	const float i = 0.5f;
	uint64_t py = Read<uint64_t>(hp, getmyselfaddress(hp, base) + 0x288);
	WriteProcessMemory(hp, (void*)(py + 0x1c4), &i, 4, 0);
}

void upguanxin()			//关闭惯性
{
	const float i = 1.0f;
	uint64_t py = Read<uint64_t>(hp, getmyselfaddress(hp, base) + 0x288);
	WriteProcessMemory(hp, (void*)(py + 0x1c4), &i, 4, 0);
}

void yidongsud()
{
	uint64_t py = Read<uint64_t>(hp, getmyselfaddress(hp, base) + 0x288);
	WriteProcessMemory(hp, (void*)(py + 0x18C), &ydsd, 4, 0);
}


void menu()
{
	ImGui::SetNextWindowSize({ 350,550 }, ImGuiCond_Once);
	ImGui::Begin("##a", NULL, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar);
	
	ImGui::SliderFloat(U8("亮度"), &changjinld, 2.0f, 100.0f);
	
	ImGui::SliderFloat(U8("移动速度"), &ydsd, 400.f, 5000.f);

	if (ImGui::SliderFloat(U8("跳跃高度"), &tygd, 0.f, 5000.f))
	{
		tiaoygd();
	}
	
	
	
	if (ImGui::SliderInt(U8("跳跃次数"), &jumpcount, 1, 1000))
	{
		tiaoye();
	}

	
	
	
	ImGui::Checkbox(U8("去除惯性"), &c1);
	if (c1) {
		upguanxin();
	}
	else if(!c1) {
		downguanxin();
	}

	ImGui::Checkbox(U8("定住小鬼"), &dg);

	ImGui::End();
	liangdu();
	yidongsud();
}
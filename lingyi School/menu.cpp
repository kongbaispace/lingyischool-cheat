#include"menu.h"


bool c1=false;
bool dg = false;	//����

float changjinld = 5.f;	  //��������
float tygd = 350.f;                  //��Ծ�߶�
float ydsd = 400.f;                    //�ƶ��ٶ�

int jumpcount = 1;


void liangdu()						  //����
{
	uint64_t py = Read<uint64_t>(hp, base + 0x4B73F58);
	WriteProcessMemory(hp, (void*)(py + 0x904), &changjinld, 4, 0);
}

void tiaoye()				 //��Ծ����
{
	WriteProcessMemory(hp, (void*)(getmyselfaddress(hp, base) + 0x344), &jumpcount, 4, 0);
}

void tiaoygd()				//��Ծ�߶�
{
	uint64_t py = Read<uint64_t>(hp, getmyselfaddress(hp, base) + 0x288);
	WriteProcessMemory(hp, (void*)(py + 0x158), &tygd, 4, 0);
}

void downguanxin()			//��������
{
	const float i = 0.5f;
	uint64_t py = Read<uint64_t>(hp, getmyselfaddress(hp, base) + 0x288);
	WriteProcessMemory(hp, (void*)(py + 0x1c4), &i, 4, 0);
}

void upguanxin()			//�رչ���
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
	
	ImGui::SliderFloat(U8("����"), &changjinld, 2.0f, 100.0f);
	
	ImGui::SliderFloat(U8("�ƶ��ٶ�"), &ydsd, 400.f, 5000.f);

	if (ImGui::SliderFloat(U8("��Ծ�߶�"), &tygd, 0.f, 5000.f))
	{
		tiaoygd();
	}
	
	
	
	if (ImGui::SliderInt(U8("��Ծ����"), &jumpcount, 1, 1000))
	{
		tiaoye();
	}

	
	
	
	ImGui::Checkbox(U8("ȥ������"), &c1);
	if (c1) {
		upguanxin();
	}
	else if(!c1) {
		downguanxin();
	}

	ImGui::Checkbox(U8("��סС��"), &dg);

	ImGui::End();
	liangdu();
	yidongsud();
}
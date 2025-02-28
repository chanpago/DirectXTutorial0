////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////



//////////////
// INCLUDES //
//////////////
#include <iostream>
#include <fstream>
using namespace std;


#include "systemclass.h"
#include <Windows.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "DirectXTK.lib")



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System;
	bool result;


	//�ý��� ��ä ����
	System = new SystemClass;

	// �ý��� ��ü�� �ʱ�ȭ�ϰ� run ����
	result = System->Initialize();
	if (result)
	{
		System->Run();
	}

	// system��ü�� �����ϰ� �޸𸮸� ��ȯ
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}
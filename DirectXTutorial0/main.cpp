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


	//시스템 객채 생성
	System = new SystemClass;

	// 시스템 객체를 초기화하고 run 실행
	result = System->Initialize();
	if (result)
	{
		System->Run();
	}

	// system객체를 종료하고 메모리를 반환
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}
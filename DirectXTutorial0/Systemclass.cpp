////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "systemclass.h"


SystemClass::SystemClass()
{
	m_Input = 0;
	m_Application = 0;
}


SystemClass::SystemClass(const SystemClass& other)
{
}


SystemClass::~SystemClass()
{
}


bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;


	// 변수를 함수로 보내기 전에 화면의 너비와 높이를 0으로 초기화합니다.
	screenWidth = 0;
	screenHeight = 0;

	// Windows API를 초기화합니다.
	InitializeWindows(screenWidth, screenHeight);

	// 입력 객체를 생성하고 초기화합니다. 이 객체는 사용자로부터 키보드 입력을 읽는 데 사용됩니다.
	m_Input = new InputClass;

	result = m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight);
	if (!result)
	{
		return false;
	}

	// 애플리케이션 클래스 객체를 생성하고 초기화합니다. 이 객체는 이 애플리케이션의 모든 그래픽 렌더링을 처리합니다.
	m_Application = new ApplicationClass;

	result = m_Application->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}

	return true;
}


void SystemClass::Shutdown()
{
	// 애플리케이션 클래스 객체 해제
	if (m_Application)
	{
		m_Application->Shutdown();
		delete m_Application;
		m_Application = 0;
	}

	// 입력 클래스 객체 해제
	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	// 윈도우 종료
	ShutdownWindows();

	return;
}


void SystemClass::Run()
{
	MSG msg;
	bool done, result;


	// 메시지 구조체 초기화
	ZeroMemory(&msg, sizeof(MSG));

	// 윈도우 종료 메시지가 올 때까지 루프 실행
	done = false;
	while (!done)
	{
		// 윈도우 메시지 처리
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// 종료 메시지라면 루프 탈출
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// 프레임 처리
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}
	}

	return;
}


bool SystemClass::Frame()
{
	bool result;


	// 입력 프레임 처리
	result = m_Input->Frame();
	if (!result)
	{
		return false;
	}

	// 애플리케이션 프레임 처리
	result = m_Application->Frame(m_Input);
	if (!result)
	{
		return false;
	}

	return true;
}


LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}


void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;


	// 외부 포인터를 이 객체로 설정합니다.
	ApplicationHandle = this;

	// 이 애플리케이션의 인스턴스를 가져옵니다.
	m_hinstance = GetModuleHandle(NULL);

	// 애플리케이션 이름 설정
	m_applicationName = L"Engine";

	// 윈도우 클래스를 기본 설정으로 초기화
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// 윈도우 클래스 등록
	RegisterClassEx(&wc);

	// 화면 해상도 가져오기
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// 풀스크린 여부에 따라 설정
	if (FULL_SCREEN)
	{
		// 풀스크린일 경우 전체 화면에 맞게 설정하고 32비트 색상 사용
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// 디스플레이 설정 변경
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// 화면 좌측 상단에 윈도우 위치 설정
		posX = posY = 0;
	}
	else
	{
		// 윈도우 모드일 경우 해상도 800x600으로 설정
		screenWidth = 800;
		screenHeight = 600;

		// 화면 중앙에 윈도우 배치
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// 창 생성 및 핸들 가져오기
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// 윈도우 표시 및 포커스 설정
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// 마우스 커서 숨기기
	ShowCursor(false);


	RECT rect;

	// 현재 창의 클라이언트 영역 가져오기
	GetClientRect(m_hwnd, &rect);

	// 클라이언트 좌표를 화면 좌표로 변환
	POINT ul = { rect.left, rect.top };    // 좌상단
	POINT lr = { rect.right, rect.bottom }; // 우하단

	ClientToScreen(m_hwnd, &ul);
	ClientToScreen(m_hwnd, &lr);

	rect.left = ul.x;
	rect.top = ul.y;
	rect.right = lr.x;
	rect.bottom = lr.y;

	// 마우스 커서를 클라이언트 영역 내로 제한
	ClipCursor(&rect);

	return;
}


void SystemClass::ShutdownWindows()
{
	// 마우스 커서 표시
	ShowCursor(true);

	// 풀스크린 모드였다면 디스플레이 설정 복원
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// 윈도우 제거
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// 애플리케이션 인스턴스 해제
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// 애플리케이션 핸들 해제
	ApplicationHandle = NULL;

	return;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// 윈도우가 파괴될 때
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	// 윈도우가 닫힐 때
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// 그 외 메시지는 SystemClass로 전달
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}
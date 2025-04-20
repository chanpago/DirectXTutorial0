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


	// ������ �Լ��� ������ ���� ȭ���� �ʺ�� ���̸� 0���� �ʱ�ȭ�մϴ�.
	screenWidth = 0;
	screenHeight = 0;

	// Windows API�� �ʱ�ȭ�մϴ�.
	InitializeWindows(screenWidth, screenHeight);

	// �Է� ��ü�� �����ϰ� �ʱ�ȭ�մϴ�. �� ��ü�� ����ڷκ��� Ű���� �Է��� �д� �� ���˴ϴ�.
	m_Input = new InputClass;

	result = m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight);
	if (!result)
	{
		return false;
	}

	// ���ø����̼� Ŭ���� ��ü�� �����ϰ� �ʱ�ȭ�մϴ�. �� ��ü�� �� ���ø����̼��� ��� �׷��� �������� ó���մϴ�.
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
	// ���ø����̼� Ŭ���� ��ü ����
	if (m_Application)
	{
		m_Application->Shutdown();
		delete m_Application;
		m_Application = 0;
	}

	// �Է� Ŭ���� ��ü ����
	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	// ������ ����
	ShutdownWindows();

	return;
}


void SystemClass::Run()
{
	MSG msg;
	bool done, result;


	// �޽��� ����ü �ʱ�ȭ
	ZeroMemory(&msg, sizeof(MSG));

	// ������ ���� �޽����� �� ������ ���� ����
	done = false;
	while (!done)
	{
		// ������ �޽��� ó��
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// ���� �޽������ ���� Ż��
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// ������ ó��
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


	// �Է� ������ ó��
	result = m_Input->Frame();
	if (!result)
	{
		return false;
	}

	// ���ø����̼� ������ ó��
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


	// �ܺ� �����͸� �� ��ü�� �����մϴ�.
	ApplicationHandle = this;

	// �� ���ø����̼��� �ν��Ͻ��� �����ɴϴ�.
	m_hinstance = GetModuleHandle(NULL);

	// ���ø����̼� �̸� ����
	m_applicationName = L"Engine";

	// ������ Ŭ������ �⺻ �������� �ʱ�ȭ
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

	// ������ Ŭ���� ���
	RegisterClassEx(&wc);

	// ȭ�� �ػ� ��������
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Ǯ��ũ�� ���ο� ���� ����
	if (FULL_SCREEN)
	{
		// Ǯ��ũ���� ��� ��ü ȭ�鿡 �°� �����ϰ� 32��Ʈ ���� ���
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// ���÷��� ���� ����
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// ȭ�� ���� ��ܿ� ������ ��ġ ����
		posX = posY = 0;
	}
	else
	{
		// ������ ����� ��� �ػ� 800x600���� ����
		screenWidth = 800;
		screenHeight = 600;

		// ȭ�� �߾ӿ� ������ ��ġ
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// â ���� �� �ڵ� ��������
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// ������ ǥ�� �� ��Ŀ�� ����
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// ���콺 Ŀ�� �����
	ShowCursor(false);


	RECT rect;

	// ���� â�� Ŭ���̾�Ʈ ���� ��������
	GetClientRect(m_hwnd, &rect);

	// Ŭ���̾�Ʈ ��ǥ�� ȭ�� ��ǥ�� ��ȯ
	POINT ul = { rect.left, rect.top };    // �»��
	POINT lr = { rect.right, rect.bottom }; // ���ϴ�

	ClientToScreen(m_hwnd, &ul);
	ClientToScreen(m_hwnd, &lr);

	rect.left = ul.x;
	rect.top = ul.y;
	rect.right = lr.x;
	rect.bottom = lr.y;

	// ���콺 Ŀ���� Ŭ���̾�Ʈ ���� ���� ����
	ClipCursor(&rect);

	return;
}


void SystemClass::ShutdownWindows()
{
	// ���콺 Ŀ�� ǥ��
	ShowCursor(true);

	// Ǯ��ũ�� ��忴�ٸ� ���÷��� ���� ����
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// ������ ����
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// ���ø����̼� �ν��Ͻ� ����
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// ���ø����̼� �ڵ� ����
	ApplicationHandle = NULL;

	return;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// �����찡 �ı��� ��
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	// �����찡 ���� ��
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// �� �� �޽����� SystemClass�� ����
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}
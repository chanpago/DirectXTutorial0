////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "inputclass.h"


InputClass::InputClass()
{
	m_directInput = 0;
	m_keyboard = 0;
	m_mouse = 0;
}


InputClass::InputClass(const InputClass& other)
{
}


InputClass::~InputClass()
{
}


bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	// ���콺 Ŀ�� ��ġ ��꿡 ����� ȭ�� ũ�⸦ �����մϴ�.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// ���콺�� �ʱ� ��ġ�� �����մϴ�.
	m_mouseX = 0;
	m_mouseY = 0;

	m_playerkeyboardX = 0;
	m_playerkeyboardY = 0;

	// DirectInput ���� �������̽��� �ʱ�ȭ�մϴ�.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Ű���带 ���� DirectInput �������̽��� �ʱ�ȭ�մϴ�.
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Ű������ ������ ������ �����մϴ� (���� ���ǵ� ���� ���).
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		return false;
	}

	// Ű������ ���� ������ �����մϴ� (�ٸ� ���α׷��� �������� ����).
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// Ű���� ��ġ�� ȹ���մϴ�.
	result = m_keyboard->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	// ���콺�� ���� DirectInput �������̽��� �ʱ�ȭ�մϴ�.
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// ���콺�� ������ ������ �����մϴ� (���� ���ǵ� ���� ���).
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
	{
		return false;
	}

	// ���콺�� ���� ������ �����մϴ� (�ٸ� ���α׷��� ���� ����).
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// ���콺 ��ġ�� ȹ���մϴ�.
	result = m_mouse->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	return true;
}


void InputClass::Shutdown()
{
	// ���콺 ����
	if (m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	// Ű���� ����
	if (m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	// DirectInput ���� �������̽� ����
	if (m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}

	return;
}


bool InputClass::Frame()
{
	bool result;

	// Ű���� ���¸� �н��ϴ�.
	result = ReadKeyboard();
	if (!result)
	{
		return false;
	}

	// ���콺 ���¸� �н��ϴ�.
	result = ReadMouse();
	if (!result)
	{
		return false;
	}

	// ���콺 �� Ű������ �Է� ��ȭ�� ó���մϴ�.
	ProcessInput();

	return true;
}


bool InputClass::ReadKeyboard()
{
	HRESULT result;

	// Ű���� ��ġ ���¸� �н��ϴ�.
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result))
	{
		// �Է��� ���� ��� �ٽ� ȹ���� �õ��մϴ�.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}


bool InputClass::ReadMouse()
{
	HRESULT result;

	// ���콺 ��ġ ���¸� �н��ϴ�.
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(result))
	{
		// �Է��� ���� ��� �ٽ� ȹ���� �õ��մϴ�.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}


void InputClass::ProcessInput()
{
	// �̹� ������ ������ ���콺 �̵� ���� �������� Ŀ�� ��ġ�� ������Ʈ�մϴ�.
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	// ���콺�� ȭ�� �ٱ����� ������ �ʵ��� �����մϴ�.
	if (m_mouseX < 0) { m_mouseX = 0; }
	if (m_mouseY < 0) { m_mouseY = 0; }

	if (m_mouseX > m_screenWidth) { m_mouseX = m_screenWidth; }
	if (m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }

	return;
}


bool InputClass::IsEscapePressed()
{
	// ESC Ű�� ���ȴ��� ��Ʈ ����ŷ���� Ȯ���մϴ�.
	if (m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
}


void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
	return;
}


bool InputClass::IsKeyPressed(int key)
{
	// Ư�� Ű�� ���ȴ��� Ȯ��
	return (m_keyboardState[key] & 0x80);
}


bool InputClass::IsMousePressed()
{
	// ���� ���콺 ��ư�� ���ȴ��� Ȯ��
	if (m_mouseState.rgbButtons[0] & 0x80)
	{
		return true;
	}

	return false;
}

int InputClass::GetMouseXLocation()
{
	return m_mouseX;
}

int InputClass::GetMouseYLocation()
{
	return m_mouseY;
}

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

	// 마우스 커서 위치 계산에 사용할 화면 크기를 저장합니다.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// 마우스의 초기 위치를 설정합니다.
	m_mouseX = 0;
	m_mouseY = 0;

	m_playerkeyboardX = 0;
	m_playerkeyboardY = 0;

	// DirectInput 메인 인터페이스를 초기화합니다.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 키보드를 위한 DirectInput 인터페이스를 초기화합니다.
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 키보드의 데이터 포맷을 설정합니다 (사전 정의된 포맷 사용).
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		return false;
	}

	// 키보드의 협동 수준을 설정합니다 (다른 프로그램과 공유하지 않음).
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// 키보드 장치를 획득합니다.
	result = m_keyboard->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	// 마우스를 위한 DirectInput 인터페이스를 초기화합니다.
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 마우스의 데이터 포맷을 설정합니다 (사전 정의된 포맷 사용).
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
	{
		return false;
	}

	// 마우스의 협동 수준을 설정합니다 (다른 프로그램과 공유 가능).
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// 마우스 장치를 획득합니다.
	result = m_mouse->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	return true;
}


void InputClass::Shutdown()
{
	// 마우스 해제
	if (m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	// 키보드 해제
	if (m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	// DirectInput 메인 인터페이스 해제
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

	// 키보드 상태를 읽습니다.
	result = ReadKeyboard();
	if (!result)
	{
		return false;
	}

	// 마우스 상태를 읽습니다.
	result = ReadMouse();
	if (!result)
	{
		return false;
	}

	// 마우스 및 키보드의 입력 변화를 처리합니다.
	ProcessInput();

	return true;
}


bool InputClass::ReadKeyboard()
{
	HRESULT result;

	// 키보드 장치 상태를 읽습니다.
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result))
	{
		// 입력이 끊긴 경우 다시 획득을 시도합니다.
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

	// 마우스 장치 상태를 읽습니다.
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(result))
	{
		// 입력이 끊긴 경우 다시 획득을 시도합니다.
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
	// 이번 프레임 동안의 마우스 이동 값을 기준으로 커서 위치를 업데이트합니다.
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	// 마우스가 화면 바깥으로 나가지 않도록 제한합니다.
	if (m_mouseX < 0) { m_mouseX = 0; }
	if (m_mouseY < 0) { m_mouseY = 0; }

	if (m_mouseX > m_screenWidth) { m_mouseX = m_screenWidth; }
	if (m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }

	return;
}


bool InputClass::IsEscapePressed()
{
	// ESC 키가 눌렸는지 비트 마스킹으로 확인합니다.
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
	// 특정 키가 눌렸는지 확인
	return (m_keyboardState[key] & 0x80);
}


bool InputClass::IsMousePressed()
{
	// 왼쪽 마우스 버튼이 눌렸는지 확인
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

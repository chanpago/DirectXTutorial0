////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"
#include <ctime>



ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Cursor = 0;
	m_FontShader = 0;
	m_Font = 0;
	m_TextString1 = 0;
	m_TextString2 = 0;
	m_MouseStrings = 0;
	m_Gameover = false;
	m_Gamestart = false;
	m_Gameclear = false;
	m_Lobby = true;
	m_Keyboardalbe = false;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	char testString1[32], testString2[32];
	char mouseString1[32], mouseString2[32], mouseString3[32], timeString[32];
	bool result;

	srand((unsigned int)time(NULL));
	int randomnum = rand() % 3;

	//  Direct3D 객체를 생성하고 초기화합니다.
	m_Direct3D = new D3DClass;

	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// 카메라 객체를 생성합니다.
	m_Camera = new CameraClass;

	// 카메라의 초기 위치를 설정하고 렌더링합니다.
	m_Camera->SetPosition(0.0f, 2.0f, -12.0f);
	m_Camera->Render();

	// 텍스처 셰이더 객체를 생성하고 초기화합니다.
	m_TextureShader = new TextureShaderClass;

	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// 비트맵 파일 이름을 설정합니다.
	strcpy_s(bitmapFilename, "../Engine/data/cursor.tga");
	strcpy_s(rockbitmapFilename, "../Engine/data/Rock.tga");
	strcpy_s(scissorbitmapFilename, "../Engine/data/Scissor.tga");
	strcpy_s(paperbitmapFilename, "../Engine/data/Paper.tga");
	strcpy_s(GameLogoFilename, "../Engine/data/tga/rsp.tga");
	
	m_GameLogo = new BitmapClass;
	m_GameLogo->SetPlayer();
	result = m_GameLogo->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, GameLogoFilename, 10, 10);
	if (!result)
	{
		return false;
	}
	m_GameLogo->SetSize(256, 106);

	// 게임 로고 비트맵 생성 및 초기화
	m_Cursor = new BitmapClass;
	m_Cursor->SetPlayer();
	result = m_Cursor->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, bitmapFilename,10,10);
	if (!result)
	{
		return false;
	}

	// 플레이어 캐릭터 비트맵 생성 및 초기화
	m_PlayerCharacter = new BitmapClass;
	m_PlayerCharacter->SetPlayer();
	if (randomnum == 0)
	{
		result = m_PlayerCharacter->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, rockbitmapFilename, 10, 10);
		m_PlayerCharacter->Settype(0);
		if (!result)
		{
			return false;
		}
	}
	else if (randomnum == 1)
	{
		result = m_PlayerCharacter->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, scissorbitmapFilename, 10, 10);
		m_PlayerCharacter->Settype(1);
		if (!result)
		{
			return false;
		}
	}
	else if (randomnum == 2)
	{
		result = m_PlayerCharacter->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, paperbitmapFilename, 10, 10);
		m_PlayerCharacter->Settype(2);
		if (!result)
		{
			return false;
		}
	}
	
	// 폰트 셰이더 객체 생성 및 초기화
	m_FontShader = new FontShaderClass;

	result = m_FontShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}

	// 폰트 객체 생성 및 초기화
	m_Font = new FontClass;

	result = m_Font->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), 0);
	if (!result)
	{
		return false;
	}

	// 초기 마우스 문자열 설정
	strcpy_s(mouseString1, "Mouse X: 0");
	strcpy_s(mouseString2, "Mouse Y: 0");
	strcpy_s(mouseString3, "Mouse Button: No");



	// 마우스 문자열 출력용 텍스트 객체 배열 생성 및 초기화
	m_MouseStrings = new TextClass[3];

	result = m_MouseStrings[0].Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, 32, m_Font, mouseString1, 10, 10, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	result = m_MouseStrings[1].Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, 32, m_Font, mouseString1, 10, 35, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	result = m_MouseStrings[2].Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, 32, m_Font, mouseString1, 10, 60, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	// UI 요소 초기화(버튼, 텍스처 등)
	InitializeUI();

	// 게임 상태 초기화
	m_isGameStarted = false; 
	m_elapsedTime = 0.0f;

	// 타이머 텍스트 초기화
	m_Font->SetFontSize(20.0f);
	m_TimerText = new TextClass;
	char initialTimeString[32];
	sprintf_s(initialTimeString, "Time: %.2f", m_elapsedTime);
	result = m_TimerText->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, 32, m_Font, initialTimeString, 650, 10, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	
	m_Font->SetFontSize(50.0f);
	m_ClearTimerText = new TextClass;
	result = m_ClearTimerText->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, 32, m_Font, initialTimeString, 400, 200, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	rockTex = TextureCache::GetTexture(rockbitmapFilename, m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext());
	scissorTex = TextureCache::GetTexture(scissorbitmapFilename, m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext());
	paperTex = TextureCache::GetTexture(paperbitmapFilename, m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext());

	RenderCharacters(800, 600);

	return true;
}


void ApplicationClass::Shutdown()
{
	// 마우스 문자열 텍스트 객체 해제
	for (auto character : Characters)
	{
		if (character)
		{
			character->Shutdown(); // BitmapClass의 리소스 정리
			delete character;      // 메모리 해제
		}
	}

	TextureCache::ShutdownAll();

	// UI 텍스처 해제
	if (m_GameoverTexture) m_GameoverTexture.Reset();
	if (m_ExitTexture) m_ExitTexture.Reset();
	if (m_RetryTexture) m_RetryTexture.Reset();

	// 클리어 타이머 텍스트 해제
	if (m_ClearTimerText)
	{
		m_ClearTimerText->Shutdown();
		delete m_ClearTimerText;
		m_ClearTimerText = 0;
	}

	// 타이머 텍스트 및 폰트 해제
	if (m_TimerText)
	{
		m_Font->Shutdown();
		delete m_TimerText;
		m_TimerText = 0;
	}

	// 마우스 문자열 텍스트 해제
	if (m_MouseStrings)
	{
		m_MouseStrings[0].Shutdown();
		m_MouseStrings[1].Shutdown();
		m_MouseStrings[2].Shutdown();

		delete[] m_MouseStrings;
		m_MouseStrings = 0;
	}

	// 폰트 해제
	if (m_Font)
	{
		m_Font->Shutdown();
		delete m_Font;
		m_Font = 0;
	}

	// 폰트 셰이더 해제
	if (m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	// 플레이어 캐릭터 해제
	if (m_PlayerCharacter)
	{
		m_PlayerCharacter->Shutdown();
		delete m_PlayerCharacter;
		m_PlayerCharacter = 0;
	}

	// 커서 비트맵 해제
	if (m_Cursor)
	{
		m_Cursor->Shutdown();
		delete m_Cursor;
		m_Cursor = 0;
	}

	// 게임 로고 해제
	if (m_GameLogo)
	{
		m_GameLogo->Shutdown();
		delete m_GameLogo;
		m_GameLogo = 0;
	}
	
	// 텍스처 셰이더 해제
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// 카메라 해제
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Direct3D 해제
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	return;
}


bool ApplicationClass::Frame(InputClass* Input)
{
	int mouseX, mouseY, keyboardX, keyboardY;
	bool result, mouseDown;

	// ESC 키가 눌렸는지 확인하고 눌렸으면 프로그램 종료
	if (Input->IsEscapePressed())
	{
		return false;
	}

	// 입력 객체에서 마우스 위치를 가져옵니다.
	Input->GetMouseLocation(mouseX, mouseY);

	// 마우스 클릭 여부 확인
	mouseDown = Input->IsMousePressed();

	// 마우스 좌표 문자열 업데이트
	result = UpdateMouseStrings(mouseX, mouseY, mouseDown);
	if (!result)
	{
		return false;
	}

	// 로비 상태일 경우 로비 화면 렌더링
	if (m_Lobby)
	{
		DirectX::XMFLOAT2 buttonPos(400, 450);
		float buttonWidth = 512 * 0.25f;  
		float buttonHeight = 271 * 0.25f;

		float left = buttonPos.x - buttonWidth / 2;
		float right = buttonPos.x + buttonWidth / 2;
		float top = buttonPos.y - buttonHeight / 2;
		float bottom = buttonPos.y + buttonHeight / 2;

		// 마우스 클릭으로 게임 시작
		if (mouseX >= left && mouseX <= right && mouseY >= top && mouseY <= bottom && mouseDown)
		{
			m_Lobby = false;
			m_Gamestart = true;
			m_Keyboardalbe = true;
			m_startTime = std::chrono::steady_clock::now();
			SetCharacters(800, 600);
		}
		result = RenderLobby(mouseX, mouseY);
		if (!result)
		{
			return false;
		}
	}

	// 게임이 시작된 상태라면 게임 로직 처리
	if (m_Gamestart)
	{
		
		const int moveSpeed = 5;  // 이동 속도

		if (m_Keyboardalbe)
		{
			if (Input->IsKeyPressed(DIK_UP)) playerY -= moveSpeed;      // ↑ 방향키 → 위로 이동
			if (Input->IsKeyPressed(DIK_DOWN)) playerY += moveSpeed;    // ↓ 방향키 → 아래로 이동
			if (Input->IsKeyPressed(DIK_LEFT)) playerX -= moveSpeed;    // ← 방향키 → 왼쪽 이동
			if (Input->IsKeyPressed(DIK_RIGHT)) playerX += moveSpeed;   // → 방향키 → 오른쪽 이동
		}
		// 화면 경계를 벗어나지 않도록 처리
		if (playerX < 0) playerX = 0;
		if (playerY < 0) playerY = 0;
		if (playerX > 800 - 50) playerX = 800 - 50; // 화면 너비 800, 캐릭터 크기 50px
		if (playerY > 600 - 50) playerY = 600 - 50; // 화면 높이 600

		// 게임 플레이 화면 렌더링
		result = RenderGameStart(playerX, playerY, Input);
		if (!result)
		{
			return false;
		}
	}

	// 게임 오버 처리
	if (m_Gameover)
	{		
		
		result = GameOver(Input);
		if (!result)
		{
			return false;
		}
		m_PlayerCharacter->SetPlayerChange(false);		
		
	}
	// 게임 클리어 처리
	else if (m_Gameclear)
	{
		result = GameClear(Input);
		if (!result)
		{
			return false;
		}
	}
	return true;
}

bool ApplicationClass::RenderLobby(int mouseX, int mouseY)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	int i;
	bool result;

	// 화면을 지우고 렌더링 시작
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 행렬 불러오기
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// Z버퍼 끄고 알파 블렌딩 활성화
	m_Direct3D->TurnZBufferOff();
	m_Direct3D->EnableAlphaBlending();

	// 로고 렌더링
	result = RogoRendering(worldMatrix, viewMatrix, orthoMatrix);
	if (!result) return false;

	// 시작 버튼 렌더링
	result = RenderStartButton();
	if (!result) return false;

	// 마우스 정보 텍스트 렌더링
	for (i = 0; i < 3; i++)
	{
		m_MouseStrings[i].Render(m_Direct3D->GetDeviceContext());
		result = m_FontShader->Render(m_Direct3D->GetDeviceContext(), m_MouseStrings[i].GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Font->GetTexture(), m_MouseStrings[i].GetPixelColor());
		if (!result) return false;
	}

	m_Direct3D->DisableAlphaBlending();

	// 커서 렌더링
	result = CursorRendering(true, mouseX, mouseY, worldMatrix, viewMatrix, orthoMatrix);
	if (!result) return false;

	// Z버퍼 다시 켜기
	m_Direct3D->TurnZBufferOn();

	// 렌더링 완료 후 출력
	m_Direct3D->EndScene();

	return true;
}


bool ApplicationClass::RenderGameStart(int keyboardX, int keyboardY, InputClass* Input)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	int i;
	bool result;
	int mouseX, mouseY;

	// 입력 클래스로부터 마우스 좌표를 가져옵니다.
	Input->GetMouseLocation(mouseX, mouseY);

	// 화면을 초기화하고 렌더링을 시작합니다.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라와 D3D 객체로부터 행렬들을 얻습니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// 2D 렌더링을 위해 Z 버퍼를 끄고 알파 블렌딩을 활성화합니다.
	m_Direct3D->TurnZBufferOff();
	m_Direct3D->EnableAlphaBlending();

	// 클리어 조건이 충족되었는지 확인합니다.
	if (CheckClearGame())
	{
		m_Gameclear = true;
		m_Keyboardalbe = false;
		return true;
	}

	// 게임 오버가 아니라면 타이머를 업데이트합니다.
	if (!m_Gameover)
	{
		UpdateTimer();
	}

	// 타이머 텍스트를 렌더링합니다.
	m_TimerText->Render(m_Direct3D->GetDeviceContext());
	m_FontShader->Render(m_Direct3D->GetDeviceContext(), m_TimerText->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Font->GetTexture(), m_TimerText->GetPixelColor());

	// 플레이어와 AI 캐릭터 간의 상호작용 처리
	for (int i = 0; i < 15; i++)
	{
		if (m_PlayerCharacter->ResolveInteraction(Characters[i]))
		{
			// 플레이어가 가위바위보 중 무엇인지에 따라 판정
			if (m_PlayerCharacter->Gettype() == 0) // 바위
			{
				if (Characters[i]->Gettype() == 1) // 가위 → 이김
				{
					Characters[i]->SetTexture(rockTex);
					Characters[i]->Settype(0);
				}
				if (Characters[i]->Gettype() == 2) // 보 → 짐
				{
					m_PlayerCharacter->SetPlayerChange(true);
				}
			}
			else if (m_PlayerCharacter->Gettype() == 1) // 가위
			{
				if (Characters[i]->Gettype() == 2) // 보 → 이김
				{
					Characters[i]->SetTexture(scissorTex);
					Characters[i]->Settype(1);
				}
				if (Characters[i]->Gettype() == 0) // 바위 → 짐
				{
					m_PlayerCharacter->SetPlayerChange(true);
				}
			}
			else if (m_PlayerCharacter->Gettype() == 2) // 보
			{
				if (Characters[i]->Gettype() == 0) // 바위 → 이김
				{
					Characters[i]->SetTexture(paperTex);
					Characters[i]->Settype(2);
				}
				if (Characters[i]->Gettype() == 1) // 가위 → 짐
				{
					m_PlayerCharacter->SetPlayerChange(true);
				}
			}
		}

		// AI 캐릭터 렌더링
		Characters[i]->Render(m_Direct3D->GetDeviceContext(), Characters[i]->GetpositionX(), Characters[i]->GetpositionY());
		m_TextureShader->Render(m_Direct3D->GetDeviceContext(), Characters[i]->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, Characters[i]->GetTexture());
	}

	// AI 캐릭터들 간의 상호작용 처리
	for (int i = 0; i < 15; i++)
	{
		for (int j = i + 1; j < 15; j++)
		{
			if (i != j)
			{
				if (Characters[i]->ResolveInteraction(Characters[j]))
				{
					if (Characters[i]->Gettype() == 0) // 바위
					{
						if (Characters[j]->Gettype() == 1) //가위
						{
							Characters[j]->SetTexture(rockTex);
							Characters[j]->Settype(0);
						}
						if (Characters[j]->Gettype() == 2)//보
						{
							Characters[i]->SetTexture(paperTex);
							Characters[i]->Settype(2);
						}
					}
					else if (Characters[i]->Gettype() == 1) // 가위
					{
						if (Characters[j]->Gettype() == 0) // 바위
						{
							Characters[i]->SetTexture(rockTex);
							Characters[i]->Settype(0);
						}
						if (Characters[j]->Gettype() == 2) // 보
						{
							Characters[j]->SetTexture(scissorTex);
							Characters[j]->Settype(1);
						}
					}
					else if (Characters[i]->Gettype() == 2) // 보
					{
						if (Characters[j]->Gettype() == 0)//바위
						{
							Characters[j]->SetTexture(paperTex);
							Characters[j]->Settype(2);
						}
						if (Characters[j]->Gettype() == 1) // 가위
						{
							Characters[i]->SetTexture(scissorTex);
							Characters[i]->Settype(1);
						}
					}
				}
			}
		}

		// AI 캐릭터 렌더링
		Characters[i]->Render(m_Direct3D->GetDeviceContext(), Characters[i]->GetpositionX(), Characters[i]->GetpositionY());
		m_TextureShader->Render(m_Direct3D->GetDeviceContext(), Characters[i]->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, Characters[i]->GetTexture());
	}

	// 플레이어 캐릭터 렌더링 준비
	result = m_PlayerCharacter->Render(m_Direct3D->GetDeviceContext(), keyboardX, keyboardY);
	if (!result) return false;

	// 플레이어 캐릭터 텍스처 렌더링
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_PlayerCharacter->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_PlayerCharacter->GetTexture());
	if (!result) return false;

	// 플레이어가 상호작용 결과로 상태가 변경되었을 경우 → 게임 오버
	if (m_PlayerCharacter->GetPlayerChange() == true)
	{
		m_Gameover = true;
		m_Keyboardalbe = false;
	}

	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼 다시 활성화
	m_Direct3D->TurnZBufferOn();

	// 렌더링된 화면을 출력
	m_Direct3D->EndScene();

	return true;
}


bool ApplicationClass::UpdateMouseStrings(int mouseX, int mouseY, bool mouseDown)
{
	char tempString[16], finalString[32];
	bool result;

	// 마우스 X 좌표를 문자열로 변환하여 렌더링 텍스트 생성
	sprintf_s(tempString, "%d", mouseX);
	strcpy_s(finalString, "Mouse X: ");
	strcat_s(finalString, tempString);
	result = m_MouseStrings[0].UpdateText(m_Direct3D->GetDeviceContext(), m_Font, finalString, 10, 10, 1.0f, 1.0f, 1.0f);
	if (!result) return false;

	// 마우스 Y 좌표를 문자열로 변환하여 렌더링 텍스트 생성
	sprintf_s(tempString, "%d", mouseY);
	strcpy_s(finalString, "Mouse Y: ");
	strcat_s(finalString, tempString);
	result = m_MouseStrings[1].UpdateText(m_Direct3D->GetDeviceContext(), m_Font, finalString, 10, 35, 1.0f, 1.0f, 1.0f);
	if (!result) return false;

	// 마우스 버튼 클릭 상태 업데이트
	if (mouseDown)
		strcpy_s(finalString, "Mouse Button: Yes");
	else
		strcpy_s(finalString, "Mouse Button: No");
	result = m_MouseStrings[2].UpdateText(m_Direct3D->GetDeviceContext(), m_Font, finalString, 10, 60, 1.0f, 1.0f, 1.0f);
	if (!result) return false;

	return true;
}

bool ApplicationClass::UpdatePlayerCharacter(int NextX, int NextY, bool)
{
	bool result;

	result = m_PlayerCharacter->Render(m_Direct3D->GetDeviceContext(), NextX, NextY);
	if (!result)
	{
		return false;
	}

	return true;
}

bool ApplicationClass::AIRender()
{
	return false;
}

bool ApplicationClass::GameOver(InputClass* Input)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	bool result, mouseDown;
	int mouseX, mouseY;

	// 장면을 시작하기 위해 버퍼를 지웁니다.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라와 D3D 객체로부터 월드, 뷰, 직교 투영 행렬을 가져옵니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// 2D 렌더링을 시작하기 위해 Z 버퍼를 끄고 알파 블렌딩을 켭니다.
	m_Direct3D->TurnZBufferOff();
	m_Direct3D->EnableAlphaBlending();

	// 마우스 클릭 여부를 확인합니다.
	mouseDown = Input->IsMousePressed();
	Input->GetMouseLocation(mouseX, mouseY);

	// 게임 오버 상태에서는 캐릭터들을 멈춥니다.
	for (int i = 0; i < 15; i++)
	{
		Characters[i]->Setspeed(0.0f, 0.0f);
	}

	// "Game Over" 텍스처 렌더링
	if (m_spriteBatch && m_GameoverTexture)
	{
		m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred);

		// 버튼 위치 설정
		DirectX::XMFLOAT2 position(400, 300);
		DirectX::XMFLOAT2 scale(0.5f, 0.5f);  // 텍스처를 절반 크기로 렌더링

		// 텍스처 크기 정보 추출
		D3D11_TEXTURE2D_DESC textureDesc;
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		m_GameoverTexture->GetResource(resource.GetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		resource.As(&texture);
		texture->GetDesc(&textureDesc);

		// 원점을 중앙으로 설정
		DirectX::XMFLOAT2 origin(textureDesc.Width / 2.0f, textureDesc.Height / 2.0f);

		// 게임오버 텍스처 렌더링
		m_spriteBatch->Draw(m_GameoverTexture.Get(), position, nullptr, DirectX::Colors::White, 0.0f, origin, scale);

		m_spriteBatch->End();
	}

	// "EXIT" 버튼 렌더링 및 클릭 처리
	if (m_spriteBatch && m_ExitTexture)
	{
		m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred);

		DirectX::XMFLOAT2 position(200, 500);
		DirectX::XMFLOAT2 scale(0.3f, 0.3f);  // 크기 조정

		D3D11_TEXTURE2D_DESC textureDesc;
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		m_ExitTexture->GetResource(resource.GetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		resource.As(&texture);
		texture->GetDesc(&textureDesc);

		DirectX::XMFLOAT2 origin(textureDesc.Width / 2.0f, textureDesc.Height / 2.0f);

		m_spriteBatch->Draw(m_ExitTexture.Get(), position, nullptr, DirectX::Colors::White, 0.0f, origin, scale);

		m_spriteBatch->End();

		// 마우스 클릭 범위 감지
		float buttonWidth = 600 * 0.3f;
		float buttonHeight = 160 * 0.3f;

		float left = position.x - buttonWidth / 2;
		float right = position.x + buttonWidth / 2;
		float top = position.y - buttonHeight / 2;
		float bottom = position.y + buttonHeight / 2;

		if (mouseX >= left && mouseX <= right &&
			mouseY >= top && mouseY <= bottom && mouseDown)
		{
			// "EXIT" 클릭 시 초기 상태로 리셋
			m_Lobby = true;
			m_Gamestart = false;
			m_Keyboardalbe = true;
			m_Gameover = false;
			SetCharacters(800, 600);

			return true;
		}
	}

	// "RETRY" 버튼 렌더링 및 클릭 처리
	if (m_spriteBatch && m_RetryTexture)
	{
		m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred);

		DirectX::XMFLOAT2 position(600, 500);
		DirectX::XMFLOAT2 scale(0.3f, 0.3f);

		D3D11_TEXTURE2D_DESC textureDesc;
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		m_RetryTexture->GetResource(resource.GetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		resource.As(&texture);
		texture->GetDesc(&textureDesc);

		DirectX::XMFLOAT2 origin(textureDesc.Width / 2.0f, textureDesc.Height / 2.0f);

		m_spriteBatch->Draw(m_RetryTexture.Get(), position, nullptr, DirectX::Colors::White, 0.0f, origin, scale);

		m_spriteBatch->End();

		float buttonWidth = 600 * 0.3f;
		float buttonHeight = 500 * 0.3f;

		float left = position.x - buttonWidth / 2;
		float right = position.x + buttonWidth / 2;
		float top = position.y - buttonHeight / 2;
		float bottom = position.y + buttonHeight / 2;

		if (mouseX >= left && mouseX <= right &&
			mouseY >= top && mouseY <= bottom && mouseDown)
		{
			// "RETRY" 클릭 시 게임 재시작
			m_startTime = std::chrono::steady_clock::now();
			m_Lobby = false;
			m_Gamestart = true;
			m_Gameover = false;
			m_Keyboardalbe = true;
			SetCharacters(800, 600);

			return true;
		}
	}

	// 커서 렌더링
	result = CursorRendering(true, mouseX, mouseY, worldMatrix, viewMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	// 2D 렌더링이 완료되었으므로 Z 버퍼 다시 활성화
	m_Direct3D->TurnZBufferOn();

	// 최종 장면을 화면에 출력
	m_Direct3D->EndScene();

	// UI 창이 표시되고 종료 상태가 됨
	return true;
}


bool ApplicationClass::GameClear(InputClass* Input)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	bool result, mouseDown;
	int mouseX, mouseY;

	// 장면을 시작하기 위해 버퍼를 클리어합니다.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라 및 D3D 객체로부터 월드, 뷰, 직교 행렬을 가져옵니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// 2D 렌더링을 위해 Z버퍼를 끄고 알파 블렌딩을 활성화합니다.
	m_Direct3D->TurnZBufferOff();
	m_Direct3D->EnableAlphaBlending();

	// 마우스 입력을 확인합니다.
	mouseDown = Input->IsMousePressed();
	Input->GetMouseLocation(mouseX, mouseY);

	// 모든 캐릭터의 속도를 0으로 설정하여 정지시킵니다.
	for (int i = 0; i < 15; i++)
	{
		Characters[i]->Setspeed(0.0f, 0.0f);
	}

	// "게임 클리어" 텍스처를 화면 중앙에 렌더링
	if (m_spriteBatch && m_GameclearTexture)
	{
		m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred);

		// 위치와 크기 설정
		DirectX::XMFLOAT2 position(400, 300);
		DirectX::XMFLOAT2 scale(0.3f, 0.3f);  // 30% 크기로 조정

		// 텍스처 크기 가져오기
		D3D11_TEXTURE2D_DESC textureDesc;
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		m_GameclearTexture->GetResource(resource.GetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		resource.As(&texture);
		texture->GetDesc(&textureDesc);

		// 중앙 기준점 설정
		DirectX::XMFLOAT2 origin(textureDesc.Width / 2.0f, textureDesc.Height / 2.0f);

		// 텍스처 출력
		m_spriteBatch->Draw(m_GameclearTexture.Get(), position, nullptr, DirectX::Colors::White, 0.0f, origin, scale);
		m_spriteBatch->End();
	}

	// "EXIT" 버튼 처리
	if (m_spriteBatch && m_ExitTexture)
	{
		m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred);

		DirectX::XMFLOAT2 position(200, 500);
		DirectX::XMFLOAT2 scale(0.3f, 0.3f);

		D3D11_TEXTURE2D_DESC textureDesc;
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		m_ExitTexture->GetResource(resource.GetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		resource.As(&texture);
		texture->GetDesc(&textureDesc);

		DirectX::XMFLOAT2 origin(textureDesc.Width / 2.0f, textureDesc.Height / 2.0f);

		m_spriteBatch->Draw(m_ExitTexture.Get(), position, nullptr, DirectX::Colors::White, 0.0f, origin, scale);
		m_spriteBatch->End();

		// 클릭 영역 계산
		float buttonWidth = 600 * 0.3f;
		float buttonHeight = 160 * 0.3f;
		float left = position.x - buttonWidth / 2;
		float right = position.x + buttonWidth / 2;
		float top = position.y - buttonHeight / 2;
		float bottom = position.y + buttonHeight / 2;

		// 클릭 감지
		if (mouseX >= left && mouseX <= right &&
			mouseY >= top && mouseY <= bottom && mouseDown)
		{
			// 로비로 돌아감
			m_Lobby = true;
			m_Gamestart = false;
			m_Keyboardalbe = true;
			m_Gameclear = false;
			SetCharacters(800, 600);
			return true;
		}
	}

	// "RETRY" 버튼 처리
	if (m_spriteBatch && m_RetryTexture)
	{
		m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred);

		DirectX::XMFLOAT2 position(600, 500);
		DirectX::XMFLOAT2 scale(0.3f, 0.3f);

		D3D11_TEXTURE2D_DESC textureDesc;
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		m_RetryTexture->GetResource(resource.GetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		resource.As(&texture);
		texture->GetDesc(&textureDesc);

		DirectX::XMFLOAT2 origin(textureDesc.Width / 2.0f, textureDesc.Height / 2.0f);

		m_spriteBatch->Draw(m_RetryTexture.Get(), position, nullptr, DirectX::Colors::White, 0.0f, origin, scale);
		m_spriteBatch->End();

		float buttonWidth = 600 * 0.3f;
		float buttonHeight = 500 * 0.3f;
		float left = position.x - buttonWidth / 2;
		float right = position.x + buttonWidth / 2;
		float top = position.y - buttonHeight / 2;
		float bottom = position.y + buttonHeight / 2;

		if (mouseX >= left && mouseX <= right &&
			mouseY >= top && mouseY <= bottom && mouseDown)
		{
			// 게임 재시작
			m_startTime = std::chrono::steady_clock::now();
			m_Lobby = false;
			m_Gamestart = true;
			m_Gameclear = false;
			m_Keyboardalbe = true;
			SetCharacters(800, 600);
			return true;
		}
	}

	// 클리어 시간 텍스트 업데이트 및 렌더링
	m_Font->SetFontSize(50.0f);
	sprintf_s(initialTimeString, "Time: %.2f", m_elapsedTime);
	result = m_ClearTimerText->UpdateText(m_Direct3D->GetDeviceContext(), m_Font, initialTimeString, 370, 200, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}
	m_ClearTimerText->Render(m_Direct3D->GetDeviceContext());
	m_FontShader->Render(m_Direct3D->GetDeviceContext(), m_ClearTimerText->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Font->GetTexture(), m_ClearTimerText->GetPixelColor());

	// 커서 렌더링
	result = CursorRendering(true, mouseX, mouseY, worldMatrix, viewMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	// 2D 렌더링 완료 후 Z버퍼 다시 켜기
	m_Direct3D->TurnZBufferOn();

	// 최종 장면을 화면에 출력
	m_Direct3D->EndScene();

	// 종료 처리 완료
	return true;
}


bool ApplicationClass::CursorRendering(bool m_showCursor, int mouseX, int mouseY, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX orthoMatrix)
{
	bool result;

	// 커서가 보이도록 설정되었을 때만 렌더링
	if (m_showCursor)  
	{
		result = m_Cursor->Render(m_Direct3D->GetDeviceContext(), mouseX, mouseY);
		if (!result)
		{
			return false;
		}

		result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Cursor->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Cursor->GetTexture());
		if (!result)
		{
			return false;
		}
		return true;
	}
	return false;
}

bool ApplicationClass::RogoRendering(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX orthoMatrix)
{
	bool result;

	result = m_GameLogo->Render(m_Direct3D->GetDeviceContext(), 280, 150);
	if (!result)
	{
		return false;
	}

	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_GameLogo->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_GameLogo->GetTexture());
	if (!result)
	{
		return false;
	}
	return true;
	
}

void ApplicationClass::RenderCharacters(int screenWidth, int screenHeight)
{
	//ClearCharacters();
	for (int i = 0; i < 5; i++)
	{
		int randomX = rand() % screenWidth;
		int randomY = rand() % screenHeight;

		BitmapClass* newCharacter = new BitmapClass();
		newCharacter->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, rockbitmapFilename, randomX, randomY);
		//newCharacter->SetTexture(rockTex); // 캐시에서 공유
		newCharacter->Settype(0);
		newCharacter->SetNotPlayer();
		Characters.push_back(newCharacter); // 그룹 i에 추가
	}

	for (int i = 5; i < 10; i++)
	{
		int randomX = rand() % screenWidth;
		int randomY = rand() % screenHeight;

		BitmapClass* newCharacter = new BitmapClass();
		newCharacter->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, scissorbitmapFilename, randomX, randomY);
		//newCharacter->SetTexture(scissorTex); // 캐시에서 공유
		newCharacter->Settype(1);
		newCharacter->SetNotPlayer();
		Characters.push_back(newCharacter); // 그룹 i에 추가
	}

	for (int i = 10; i < 15; i++)
	{
		int randomX = rand() % screenWidth;
		int randomY = rand() % screenHeight;

		BitmapClass* newCharacter = new BitmapClass();
		newCharacter->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, paperbitmapFilename, randomX, randomY); // textureFilename은 무시
		//newCharacter->SetTexture(paperTex); // 캐시에서 공유
		newCharacter->Settype(2);
		newCharacter->SetNotPlayer();
		Characters.push_back(newCharacter); // 그룹 i에 추가
	}
}

void ApplicationClass::SetCharacters(int screenWidth, int screenHeight)
{
	OutputDebugStringA("SetCharacters 호출됨\n");
	char buffer[128];

	for (int i = 0; i < 5; i++)
	{
		int randomX = rand() % screenWidth;
		int randomY = rand() % screenHeight;

		// 이 부분에 텍스처 교체하는 코드
		Characters[i]->SetTexture(rockTex);
		Characters[i]->Settype(0);
		Characters[i]->Setposition(randomX,randomY);
		Characters[i]->SetNotPlayer();		
		float dx = ((rand() % 2001) / 1000.0f) - 1.0f;
		float dy = ((rand() % 2001) / 1000.0f) - 1.0f;
		Characters[i]->Setspeed(dx, dy);

		sprintf_s(buffer, "[SetCharacters] Character %d (Rock) Speed: %.2f, %.2f\n", i, dx, dy);
		OutputDebugStringA(buffer);
	}

	for (int i = 5; i < 10; i++)
	{
		int randomX = rand() % screenWidth;
		int randomY = rand() % screenHeight;

		// 이 부분에 텍스처 교체하는 코드
		Characters[i]->SetTexture(scissorTex);
		Characters[i]->Settype(1);
		Characters[i]->Setposition(randomX, randomY);
		Characters[i]->SetNotPlayer();
		float dx = ((rand() % 2001) / 1000.0f) - 1.0f;
		float dy = ((rand() % 2001) / 1000.0f) - 1.0f;
		Characters[i]->Setspeed(dx, dy);

		sprintf_s(buffer, "[SetCharacters] Character %d (Scissor) Speed: %.2f, %.2f\n", i, dx, dy);
		OutputDebugStringA(buffer);
	}

	for (int i = 10; i < 15; i++)
	{
		int randomX = rand() % screenWidth;
		int randomY = rand() % screenHeight;

		// 이 부분에 텍스처 교체하는 코드
		Characters[i]->SetTexture(paperTex);
		Characters[i]->Settype(2);
		Characters[i]->Setposition(randomX, randomY);
		Characters[i]->SetNotPlayer();
		float dx = ((rand() % 2001) / 1000.0f) - 1.0f;
		float dy = ((rand() % 2001) / 1000.0f) - 1.0f;
		Characters[i]->Setspeed(dx, dy);

		sprintf_s(buffer, "[SetCharacters] Character %d (Paper) Speed: %.2f, %.2f\n", i, dx, dy);
		OutputDebugStringA(buffer);
	}
}

void ApplicationClass::ClearCharacters()
{
	// Characters 벡터에 저장된 모든 동적 객체를 삭제
	for (auto character : Characters)
	{
		if (character)
		{
			character->Shutdown(); // BitmapClass의 리소스 정리
			//delete character;      // 메모리 해제
		}
	}

	// 벡터를 비움
	Characters.clear();
}

void ApplicationClass::ResetUI()
{
	// 모든 UI 상태 초기화
	m_Gameover = false;
	m_Gamestart = false;
	m_Gameclear = false;
	m_Lobby = true;

	// 게임 시작 상태 리셋
	m_isGameStarted = false;
	m_elapsedTime = 0.0f;
	m_Keyboardalbe = false;

	// 텍스처 리셋 (불필요한 텍스처가 남아있지 않도록)
	m_GameoverTexture.Reset();
	m_ExitTexture.Reset();
	m_RetryTexture.Reset();

	// GameOver UI가 남아 있지 않도록 화면 초기화
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	m_Direct3D->TurnZBufferOff();
	m_Direct3D->EnableAlphaBlending();

	// 로비 UI 렌더링
	RenderLobby(400, 450);

	m_Direct3D->DisableAlphaBlending();
	m_Direct3D->TurnZBufferOn();
	m_Direct3D->EndScene();
}

void ApplicationClass::RenderText(float, char*, int, int)
{

}

bool ApplicationClass::CheckClearGame()
{
	int check = m_PlayerCharacter->Gettype();

	for (int i = 0; i < 15; i++)
	{
		if (Characters[i]->Gettype() != check)
		{
			return false;
		}
	}

	return true;
}



void ApplicationClass::InitializeUI()
{
	// SpriteBatch 생성
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(m_Direct3D->GetDeviceContext());

	// 버튼 텍스처 로드 (button.tga 또는 button.png 사용 가능)
	HRESULT hr = DirectX::CreateWICTextureFromFile(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(),
		L"../Engine/data/start.png", nullptr, &m_buttonTexture);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"버튼 텍스처 로드 실패!", L"Error", MB_OK);
	}


	// "종료 버튼" 텍스처 로드
	hr = DirectX::CreateWICTextureFromFile(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(),
		L"../Engine/data/png/exit.png", nullptr, &m_ExitTexture);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"종료 버튼 텍스처 로드 실패!", L"Error", MB_OK);
	}

	// "재시도 버튼" 텍스처 로드
	hr = DirectX::CreateWICTextureFromFile(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(),
		L"../Engine/data/png/retry.png", nullptr, &m_RetryTexture);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"재시도 버튼 텍스처 로드 실패!", L"Error", MB_OK);
	}

	// "게임오버 버튼" 텍스처 로드
	hr = DirectX::CreateWICTextureFromFile(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(),
		L"../Engine/data/png/Gameover.png", nullptr, &m_GameoverTexture);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"재시도 버튼 텍스처 로드 실패!", L"Error", MB_OK);
	}

	// "게임 클리어 버튼" 텍스처 로드
	hr = DirectX::CreateWICTextureFromFile(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(),
		L"../Engine/data/png/Gameclear.png", nullptr, &m_GameclearTexture);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"재시도 버튼 텍스처 로드 실패!", L"Error", MB_OK);
	}
}

bool ApplicationClass::RenderStartButton()
{
	if (m_spriteBatch && m_buttonTexture)
	{
		m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred);

		// 버튼의 위치
		DirectX::XMFLOAT2 position(400, 450);
		DirectX::XMFLOAT2 scale(0.25f, 0.25f);  // 크기 조정 (예: 25%) 128 70

		// 텍스처 크기 가져오기
		D3D11_TEXTURE2D_DESC textureDesc;
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		m_buttonTexture->GetResource(resource.GetAddressOf());

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		resource.As(&texture);
		texture->GetDesc(&textureDesc);

		// 원점 (중앙)
		DirectX::XMFLOAT2 origin(textureDesc.Width / 2.0f, textureDesc.Height / 2.0f);

		// 중앙 정렬
		m_spriteBatch->Draw(m_buttonTexture.Get(), position, nullptr, DirectX::Colors::White, 0.0f,
			origin, scale);

		m_spriteBatch->End();

		return true;
	}
	return false;
}

void ApplicationClass::UpdateTimer()
{

	// 현재 시간 가져오기
	auto now = std::chrono::steady_clock::now();

	// 경과한 시간(밀리초 단위)
	auto duration = std::chrono::duration<double>(now - m_startTime);
	m_elapsedTime = duration.count(); // 초 단위 소수점 포함

	// 새로운 시간 문자열 생성 (1/10초, 1/100초 포함)
	char timeString[32];
	sprintf_s(timeString, "Time: %.2f", m_elapsedTime);

	// 텍스트 업데이트
	m_TimerText->UpdateText(m_Direct3D->GetDeviceContext(), m_Font, timeString, 700, 10, 1.0f, 1.0f, 1.0f);
}


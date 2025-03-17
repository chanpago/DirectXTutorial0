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

	// Create and initialize the Direct3D object.
	m_Direct3D = new D3DClass;

	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 2.0f, -12.0f);
	m_Camera->Render();

	// Create and initialize the texture shader object.
	m_TextureShader = new TextureShaderClass;

	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// Set the file name of the bitmap file.
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

	// Create and initialize the bitmap object.
	m_Cursor = new BitmapClass;
	m_Cursor->SetPlayer();
	result = m_Cursor->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, bitmapFilename,10,10);
	if (!result)
	{
		return false;
	}


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
	
	// Create and initialize the font shader object.
	m_FontShader = new FontShaderClass;

	result = m_FontShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the font object.
	m_Font = new FontClass;

	result = m_Font->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), 0);
	if (!result)
	{
		return false;
	}

	// Set the initial mouse strings.
	strcpy_s(mouseString1, "Mouse X: 0");
	strcpy_s(mouseString2, "Mouse Y: 0");
	strcpy_s(mouseString3, "Mouse Button: No");



	// Create and initialize the text objects for the mouse strings.
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

	InitializeUI();
	//Characters.resize(16);
	//RenderCharacters(screenWidth,screenHeight);


	m_isGameStarted = false; // 게임이 아직 시작되지 않음
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

	return true;
}


void ApplicationClass::Shutdown()
{
	// Release the text objects for the mouse strings.

	for (auto character : Characters)
	{
		if (character)
		{
			character->Shutdown(); // BitmapClass의 리소스 정리
			delete character;      // 메모리 해제
		}
	}

	if (m_GameoverTexture) {
		m_GameoverTexture.Reset();
	}

	if (m_ExitTexture) {
		m_ExitTexture.Reset();
	}

	if (m_RetryTexture) {
		m_RetryTexture.Reset();
	}

	if (m_ClearTimerText)
	{
		m_ClearTimerText->Shutdown();
		delete m_ClearTimerText;
		m_ClearTimerText = 0;
	}


	if (m_TimerText)
	{
		m_Font->Shutdown();
		delete m_TimerText;
		m_TimerText = 0;
	}


	if (m_MouseStrings)
	{
		m_MouseStrings[0].Shutdown();
		m_MouseStrings[1].Shutdown();
		m_MouseStrings[2].Shutdown();

		delete[] m_MouseStrings;
		m_MouseStrings = 0;
	}

	// Release the font object.
	if (m_Font)
	{
		m_Font->Shutdown();
		delete m_Font;
		m_Font = 0;
	}

	// Release the font shader object.
	if (m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	if (m_PlayerCharacter)
	{
		m_PlayerCharacter->Shutdown();
		delete m_PlayerCharacter;
		m_PlayerCharacter = 0;
	}

	// Release the bitmap object.
	if (m_Cursor)
	{
		m_Cursor->Shutdown();
		delete m_Cursor;
		m_Cursor = 0;
	}

	if (m_GameLogo)
	{
		m_GameLogo->Shutdown();
		delete m_GameLogo;
		m_GameLogo = 0;
	}
	

	// Release the texture shader object.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the Direct3D object.
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

	// Check if the user pressed escape and wants to exit the application.
	if (Input->IsEscapePressed())
	{
		return false;
	}

	// Get the location of the mouse from the input object,
	Input->GetMouseLocation(mouseX, mouseY);

	// Check if the mouse has been pressed.
	mouseDown = Input->IsMousePressed();

	// Update the mouse strings each frame.
	result = UpdateMouseStrings(mouseX, mouseY, mouseDown);
	if (!result)
	{
		return false;
	}

	if (m_Lobby)
	{
		DirectX::XMFLOAT2 buttonPos(400, 450);
		float buttonWidth = 512 * 0.25f;  // 텍스처 크기의 25% (예제 스케일 적용)
		float buttonHeight = 271 * 0.25f;

		float left = buttonPos.x - buttonWidth / 2;
		float right = buttonPos.x + buttonWidth / 2;
		float top = buttonPos.y - buttonHeight / 2;
		float bottom = buttonPos.y + buttonHeight / 2;

		if (mouseX >= left && mouseX <= right &&
			mouseY >= top && mouseY <= bottom && mouseDown)
		{
			m_Lobby = false;
			m_Gamestart = true;
			m_Keyboardalbe = true;
			m_startTime = std::chrono::steady_clock::now();
			RenderCharacters(800, 600);
		}
		result = RenderLobby(mouseX, mouseY);
		if (!result)
		{
			return false;
		}
	}
	if (m_Gamestart)
	{
		
		int moveSpeed = 5;  // 이동 속도

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
		if (playerX > 800 - 50) playerX = 800 - 50; // 예제: 화면 너비 800, 캐릭터 크기 50px
		if (playerY > 600 - 50) playerY = 600 - 50; // 예제: 화면 높이 600

		// Render the graphics scene.
		result = RenderGameStart(playerX, playerY, Input);
		if (!result)
		{
			return false;
		}
	}
	if (m_Gameover)
	{		
		
		result = GameOver(Input);
		if (!result)
		{
			return false;
		}
		m_PlayerCharacter->SetPlayerChange(false);		
		
	}
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

	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);


	// Turn off the Z buffer to begin all 2D rendering.
	m_Direct3D->TurnZBufferOff();
	m_Direct3D->EnableAlphaBlending();

	
	result = RogoRendering(worldMatrix, viewMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}
	

	//Render startbutton texture, setting position, scale
	result = RenderStartButton();
	if (!result)
	{
		return false;
	}
	// Render the mouse text strings using the font shader.
	for (i = 0; i < 3; i++)
	{
		m_MouseStrings[i].Render(m_Direct3D->GetDeviceContext());

		result = m_FontShader->Render(m_Direct3D->GetDeviceContext(), m_MouseStrings[i].GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
			m_Font->GetTexture(), m_MouseStrings[i].GetPixelColor());
		if (!result)
		{
			return false;
		}
	}
	
	m_Direct3D->DisableAlphaBlending();

	//커서 랜더링
	result = CursorRendering(true, mouseX, mouseY, worldMatrix, viewMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_Direct3D->TurnZBufferOn();

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}


bool ApplicationClass::RenderGameStart(int keyboardX, int keyboardY, InputClass* Input)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	int i;
	bool result;
	int mouseX, mouseY;

	Input->GetMouseLocation(mouseX, mouseY);


	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);


	// Turn off the Z buffer to begin all 2D rendering.
	m_Direct3D->TurnZBufferOff();
	m_Direct3D->EnableAlphaBlending();

	if (CheckClearGame())
	{
		m_Gameclear = true;
		m_Keyboardalbe = false;
		return true;
	}

	if (!m_Gameover)
	{
		UpdateTimer();
	}
	

	m_TimerText->Render(m_Direct3D->GetDeviceContext());
	m_FontShader->Render(m_Direct3D->GetDeviceContext(), m_TimerText->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Font->GetTexture(), m_TimerText->GetPixelColor());


	for (int i = 0; i < 15; i++)
	{
		if (m_PlayerCharacter->ResolveInteraction(Characters[i]))
		{
			if (m_PlayerCharacter->Gettype() == 0)
			{
				if (Characters[i]->Gettype() == 1)
				{
					Characters[i]->ChangeTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), rockbitmapFilename);
					Characters[i]->Settype(0);
				}
				if (Characters[i]->Gettype() == 2)
				{
					m_PlayerCharacter->SetPlayerChange(true);
				}			
			}
			if (m_PlayerCharacter->Gettype() == 1)
			{
				if (Characters[i]->Gettype() == 2)
				{
					Characters[i]->ChangeTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), scissorbitmapFilename);
					Characters[i]->Settype(1);
				}
				if (Characters[i]->Gettype() == 0)
				{
					m_PlayerCharacter->SetPlayerChange(true);
				}
			}
			if (m_PlayerCharacter->Gettype() == 2)
			{
				if (Characters[i]->Gettype() == 0)
				{
					Characters[i]->ChangeTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), paperbitmapFilename);
					Characters[i]->Settype(2);
				}
				if (Characters[i]->Gettype() == 1)
				{
					m_PlayerCharacter->SetPlayerChange(true);
				}
			}
		}
		// 렌더링
		Characters[i]->Render(m_Direct3D->GetDeviceContext(), Characters[i]->GetpositionX(), Characters[i]->GetpositionY());
		m_TextureShader->Render(m_Direct3D->GetDeviceContext(), Characters[i]->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, Characters[i]->GetTexture());
	}

	// AI 캐릭터 업데이트 및 충돌 처리
	for (int i = 0; i < 15; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			if (i != j)
			{
				if (Characters[i]->ResolveInteraction(Characters[j]))
				{
					if (Characters[i]->Gettype() == 0 )
					{
						if (Characters[j]->Gettype() == 1)
						{
							Characters[j]->ChangeTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), rockbitmapFilename);
							Characters[j]->Settype(0);
						}
						if (Characters[j]->Gettype() == 2)
						{
							Characters[i]->ChangeTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), paperbitmapFilename);
							Characters[i]->Settype(2);
						}
					}
					else if (Characters[i]->Gettype() == 1)
					{
						if (Characters[j]->Gettype() == 0)
						{
							Characters[i]->ChangeTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), rockbitmapFilename);
							Characters[i]->Settype(0);
						}
						if (Characters[j]->Gettype() == 2)
						{
							Characters[j]->ChangeTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), scissorbitmapFilename);
							Characters[j]->Settype(1);
						}
					}
					else if (Characters[i]->Gettype() == 2)
					{
						if (Characters[j]->Gettype() == 0)
						{
							Characters[j]->ChangeTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), paperbitmapFilename);
							Characters[j]->Settype(2);
						}
						if (Characters[j]->Gettype() == 1)
						{
							Characters[i]->ChangeTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), scissorbitmapFilename);
							Characters[i]->Settype(1);
						}
					}
				}
			}
		}
	
		// 렌더링
		Characters[i]->Render(m_Direct3D->GetDeviceContext(), Characters[i]->GetpositionX(), Characters[i]->GetpositionY());
		m_TextureShader->Render(m_Direct3D->GetDeviceContext(),Characters[i]->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, Characters[i]->GetTexture());
	}

	// Put the bitmap vertex and index buffers on the graphics pipeline to prepare them for drawing.
	result = m_PlayerCharacter->Render(m_Direct3D->GetDeviceContext(), keyboardX, keyboardY);
	if (!result)
	{
		return false;
	}

	// Render the bitmap with the texture shader.
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_PlayerCharacter->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_PlayerCharacter->GetTexture());
	if (!result)
	{
		return false;
	}

	if (m_PlayerCharacter->GetPlayerChange() == true)
	{		
		m_Gameover = true;
		m_Keyboardalbe = false;	
	}

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_Direct3D->TurnZBufferOn();

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}

bool ApplicationClass::UpdateMouseStrings(int mouseX, int mouseY, bool mouseDown)
{
	char tempString[16], finalString[32];
	bool result;

	// Convert the mouse X integer to string format.
	sprintf_s(tempString, "%d", mouseX);

	// Setup the mouse X string.
	strcpy_s(finalString, "Mouse X: ");
	strcat_s(finalString, tempString);

	// Update the sentence vertex buffer with the new string information.
	result = m_MouseStrings[0].UpdateText(m_Direct3D->GetDeviceContext(), m_Font, finalString, 10, 10, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	// Convert the mouse Y integer to string format.
	sprintf_s(tempString, "%d", mouseY);

	// Setup the mouse Y string.
	strcpy_s(finalString, "Mouse Y: ");
	strcat_s(finalString, tempString);

	// Update the sentence vertex buffer with the new string information.
	result = m_MouseStrings[1].UpdateText(m_Direct3D->GetDeviceContext(), m_Font, finalString, 10, 35, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	// Setup the mouse button string.
	if (mouseDown)
	{
		strcpy_s(finalString, "Mouse Button: Yes");
	}
	else
	{
		strcpy_s(finalString, "Mouse Button: No");
	}

	// Update the sentence vertex buffer with the new string information.
	result = m_MouseStrings[2].UpdateText(m_Direct3D->GetDeviceContext(), m_Font, finalString, 10, 60, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

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

	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);


	// Turn off the Z buffer to begin all 2D rendering.
	m_Direct3D->TurnZBufferOff();
	m_Direct3D->EnableAlphaBlending();

	// Check if the mouse has been pressed.
	mouseDown = Input->IsMousePressed();
	Input->GetMouseLocation(mouseX, mouseY);

	for (int i = 0; i < 15; i++)
	{
		Characters[i]->Setspeed(0, 0);
	}

	if (m_spriteBatch && m_GameoverTexture)
	{
		m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred);

		// 버튼의 위치
		DirectX::XMFLOAT2 position(400, 300);
		DirectX::XMFLOAT2 scale(0.5f, 0.5f);  // 크기 조정 (예: 25%)

		// 텍스처 크기 가져오기
		D3D11_TEXTURE2D_DESC textureDesc;
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		m_GameoverTexture->GetResource(resource.GetAddressOf());

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		resource.As(&texture);
		texture->GetDesc(&textureDesc);

		// 원점 (중앙)
		DirectX::XMFLOAT2 origin(textureDesc.Width / 2.0f, textureDesc.Height / 2.0f);

		// 중앙 정렬
		m_spriteBatch->Draw(m_GameoverTexture.Get(), position, nullptr, DirectX::Colors::White, 0.0f,
			origin, scale);

		m_spriteBatch->End();
	}
	if (m_spriteBatch && m_ExitTexture)
	{
		m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred);

		// 버튼의 위치
		DirectX::XMFLOAT2 position(200, 500);
		DirectX::XMFLOAT2 scale(0.3f, 0.3f);  // 크기 조정 (예: 25%)

		// 텍스처 크기 가져오기
		D3D11_TEXTURE2D_DESC textureDesc;
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		m_ExitTexture->GetResource(resource.GetAddressOf());

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		resource.As(&texture);
		texture->GetDesc(&textureDesc);

		// 원점 (중앙)
		DirectX::XMFLOAT2 origin(textureDesc.Width / 2.0f, textureDesc.Height / 2.0f);

		// 중앙 정렬
		m_spriteBatch->Draw(m_ExitTexture.Get(), position, nullptr, DirectX::Colors::White, 0.0f,
			origin, scale);

		m_spriteBatch->End();



		float buttonWidth = 600 * 0.3f;  // 텍스처 크기의 30% (예제 스케일 적용)
		float buttonHeight = 160 * 0.3f;

		float left = position.x - buttonWidth / 2;
		float right = position.x + buttonWidth / 2;
		float top = position.y - buttonHeight / 2;
		float bottom = position.y + buttonHeight / 2;

		if (mouseX >= left && mouseX <= right &&
			mouseY >= top && mouseY <= bottom && mouseDown)
		{
			m_Lobby = true;
			m_Gamestart = false;
			m_Keyboardalbe = true;
			m_Gameover = false;
			RenderCharacters(800, 600);

			return true;
		}

	}
	if (m_spriteBatch && m_RetryTexture)
	{
		m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred);

		// 버튼의 위치
		DirectX::XMFLOAT2 position(600, 500);
		DirectX::XMFLOAT2 scale(0.3f, 0.3f);  // 크기 조정 (예: 25%)

		// 텍스처 크기 가져오기
		D3D11_TEXTURE2D_DESC textureDesc;
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		m_RetryTexture->GetResource(resource.GetAddressOf());

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		resource.As(&texture);
		texture->GetDesc(&textureDesc);

		// 원점 (중앙)
		DirectX::XMFLOAT2 origin(textureDesc.Width / 2.0f, textureDesc.Height / 2.0f);

		// 중앙 정렬
		m_spriteBatch->Draw(m_RetryTexture.Get(), position, nullptr, DirectX::Colors::White, 0.0f,
			origin, scale);

		m_spriteBatch->End();

		float buttonWidth = 600 * 0.3f;  // 텍스처 크기의 30% (예제 스케일 적용)
		float buttonHeight = 500 * 0.3f;

		float left = position.x - buttonWidth / 2;
		float right = position.x + buttonWidth / 2;
		float top = position.y - buttonHeight / 2;
		float bottom = position.y + buttonHeight / 2;

		if (mouseX >= left && mouseX <= right &&
			mouseY >= top && mouseY <= bottom && mouseDown)
		{
			m_startTime = std::chrono::steady_clock::now();
			m_Lobby = false;
			m_Gamestart = true;
			m_Gameover = false;
			m_Keyboardalbe = true;
			RenderCharacters(800, 600);

			return true;
		}
	}


	//커서 랜더링
	result = CursorRendering(true, mouseX, mouseY, worldMatrix, viewMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}


	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_Direct3D->TurnZBufferOn();

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	// ui창 띄우기 + 시간 종료
	//종료 프로세스 구현
	return true;
}

bool ApplicationClass::GameClear(InputClass* Input)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	bool result, mouseDown;
	int mouseX, mouseY;

	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);


	// Turn off the Z buffer to begin all 2D rendering.
	m_Direct3D->TurnZBufferOff();
	m_Direct3D->EnableAlphaBlending();

	// Check if the mouse has been pressed.
	mouseDown = Input->IsMousePressed();
	Input->GetMouseLocation(mouseX, mouseY);

	for (int i = 0; i < 15; i++)
	{
		Characters[i]->Setspeed(0, 0);
	}

	if (m_spriteBatch && m_GameclearTexture)
	{
		m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred);

		// 버튼의 위치
		DirectX::XMFLOAT2 position(400, 300);
		DirectX::XMFLOAT2 scale(0.3f, 0.3f);  // 크기 조정 (예: 25%)

		// 텍스처 크기 가져오기
		D3D11_TEXTURE2D_DESC textureDesc;
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		m_GameclearTexture->GetResource(resource.GetAddressOf());

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		resource.As(&texture);
		texture->GetDesc(&textureDesc);

		// 원점 (중앙)
		DirectX::XMFLOAT2 origin(textureDesc.Width / 2.0f, textureDesc.Height / 2.0f);

		// 중앙 정렬
		m_spriteBatch->Draw(m_GameclearTexture.Get(), position, nullptr, DirectX::Colors::White, 0.0f,
			origin, scale);

		m_spriteBatch->End();
	}
	if (m_spriteBatch && m_ExitTexture)
	{
		m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred);

		// 버튼의 위치
		DirectX::XMFLOAT2 position(200, 500);
		DirectX::XMFLOAT2 scale(0.3f, 0.3f);  // 크기 조정 (예: 25%)

		// 텍스처 크기 가져오기
		D3D11_TEXTURE2D_DESC textureDesc;
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		m_ExitTexture->GetResource(resource.GetAddressOf());

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		resource.As(&texture);
		texture->GetDesc(&textureDesc);

		// 원점 (중앙)
		DirectX::XMFLOAT2 origin(textureDesc.Width / 2.0f, textureDesc.Height / 2.0f);

		// 중앙 정렬
		m_spriteBatch->Draw(m_ExitTexture.Get(), position, nullptr, DirectX::Colors::White, 0.0f,
			origin, scale);

		m_spriteBatch->End();



		float buttonWidth = 600 * 0.3f;  // 텍스처 크기의 30% (예제 스케일 적용)
		float buttonHeight = 160 * 0.3f;

		float left = position.x - buttonWidth / 2;
		float right = position.x + buttonWidth / 2;
		float top = position.y - buttonHeight / 2;
		float bottom = position.y + buttonHeight / 2;

		if (mouseX >= left && mouseX <= right &&
			mouseY >= top && mouseY <= bottom && mouseDown)
		{
			m_Lobby = true;
			m_Gamestart = false;
			m_Keyboardalbe = true;
			m_Gameclear = false;
			RenderCharacters(800, 600);
			return true;
		}

	}
	if (m_spriteBatch && m_RetryTexture)
	{
		m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred);

		// 버튼의 위치
		DirectX::XMFLOAT2 position(600, 500);
		DirectX::XMFLOAT2 scale(0.3f, 0.3f);  // 크기 조정 (예: 25%)

		// 텍스처 크기 가져오기
		D3D11_TEXTURE2D_DESC textureDesc;
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		m_RetryTexture->GetResource(resource.GetAddressOf());

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		resource.As(&texture);
		texture->GetDesc(&textureDesc);

		// 원점 (중앙)
		DirectX::XMFLOAT2 origin(textureDesc.Width / 2.0f, textureDesc.Height / 2.0f);

		// 중앙 정렬
		m_spriteBatch->Draw(m_RetryTexture.Get(), position, nullptr, DirectX::Colors::White, 0.0f,
			origin, scale);

		m_spriteBatch->End();

		float buttonWidth = 600 * 0.3f;  // 텍스처 크기의 30% (예제 스케일 적용)
		float buttonHeight = 500 * 0.3f;

		float left = position.x - buttonWidth / 2;
		float right = position.x + buttonWidth / 2;
		float top = position.y - buttonHeight / 2;
		float bottom = position.y + buttonHeight / 2;

		if (mouseX >= left && mouseX <= right &&
			mouseY >= top && mouseY <= bottom && mouseDown)
		{
			m_startTime = std::chrono::steady_clock::now();
			m_Lobby = false;
			m_Gamestart = true;
			m_Gameclear = false;
			m_Keyboardalbe = true;
			RenderCharacters(800, 600);
			return true;
		}
	}

	m_Font->SetFontSize(50.0f);

	
	// 타이머 텍스트 초기화
	//char initialTimeString[32];
	sprintf_s(initialTimeString, "Time: %.2f", m_elapsedTime);
	result = m_ClearTimerText->UpdateText(m_Direct3D->GetDeviceContext(), m_Font, initialTimeString, 370, 200, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	m_ClearTimerText->Render(m_Direct3D->GetDeviceContext());
	m_FontShader->Render(m_Direct3D->GetDeviceContext(), m_ClearTimerText->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Font->GetTexture(), m_ClearTimerText->GetPixelColor());
	

	//커서 랜더링
	result = CursorRendering(true, mouseX, mouseY, worldMatrix, viewMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_Direct3D->TurnZBufferOn();

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	//종료 프로세스 구현
	return true;
}

bool ApplicationClass::CursorRendering(bool m_showCursor, int mouseX, int mouseY, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX orthoMatrix)
{
	bool result;

	if (m_showCursor)  // 커서가 보이도록 설정되었을 때만 렌더링
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
	ClearCharacters();
	for (int i = 0; i < 5; i++)
	{
		int randomX = rand() % screenWidth;
		int randomY = rand() % screenHeight;

		BitmapClass* newCharacter = new BitmapClass();
		newCharacter->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, rockbitmapFilename, randomX, randomY);
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
		newCharacter->Settype(1);
		newCharacter->SetNotPlayer();
		Characters.push_back(newCharacter); // 그룹 i에 추가
	}

	for (int i = 10; i < 15; i++)
	{
		int randomX = rand() % screenWidth;
		int randomY = rand() % screenHeight;

		BitmapClass* newCharacter = new BitmapClass();
		newCharacter->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, paperbitmapFilename, randomX, randomY);
		newCharacter->Settype(2);
		newCharacter->SetNotPlayer();
		Characters.push_back(newCharacter); // 그룹 i에 추가
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
			delete character;      // 메모리 해제
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


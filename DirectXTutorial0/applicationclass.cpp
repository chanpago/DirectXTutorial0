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

	//  Direct3D ��ü�� �����ϰ� �ʱ�ȭ�մϴ�.
	m_Direct3D = new D3DClass;

	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// ī�޶� ��ü�� �����մϴ�.
	m_Camera = new CameraClass;

	// ī�޶��� �ʱ� ��ġ�� �����ϰ� �������մϴ�.
	m_Camera->SetPosition(0.0f, 2.0f, -12.0f);
	m_Camera->Render();

	// �ؽ�ó ���̴� ��ü�� �����ϰ� �ʱ�ȭ�մϴ�.
	m_TextureShader = new TextureShaderClass;

	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// ��Ʈ�� ���� �̸��� �����մϴ�.
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

	// ���� �ΰ� ��Ʈ�� ���� �� �ʱ�ȭ
	m_Cursor = new BitmapClass;
	m_Cursor->SetPlayer();
	result = m_Cursor->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, bitmapFilename,10,10);
	if (!result)
	{
		return false;
	}

	// �÷��̾� ĳ���� ��Ʈ�� ���� �� �ʱ�ȭ
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
	
	// ��Ʈ ���̴� ��ü ���� �� �ʱ�ȭ
	m_FontShader = new FontShaderClass;

	result = m_FontShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}

	// ��Ʈ ��ü ���� �� �ʱ�ȭ
	m_Font = new FontClass;

	result = m_Font->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), 0);
	if (!result)
	{
		return false;
	}

	// �ʱ� ���콺 ���ڿ� ����
	strcpy_s(mouseString1, "Mouse X: 0");
	strcpy_s(mouseString2, "Mouse Y: 0");
	strcpy_s(mouseString3, "Mouse Button: No");



	// ���콺 ���ڿ� ��¿� �ؽ�Ʈ ��ü �迭 ���� �� �ʱ�ȭ
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

	// UI ��� �ʱ�ȭ(��ư, �ؽ�ó ��)
	InitializeUI();

	// ���� ���� �ʱ�ȭ
	m_isGameStarted = false; 
	m_elapsedTime = 0.0f;

	// Ÿ�̸� �ؽ�Ʈ �ʱ�ȭ
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
	// ���콺 ���ڿ� �ؽ�Ʈ ��ü ����
	for (auto character : Characters)
	{
		if (character)
		{
			character->Shutdown(); // BitmapClass�� ���ҽ� ����
			delete character;      // �޸� ����
		}
	}

	TextureCache::ShutdownAll();

	// UI �ؽ�ó ����
	if (m_GameoverTexture) m_GameoverTexture.Reset();
	if (m_ExitTexture) m_ExitTexture.Reset();
	if (m_RetryTexture) m_RetryTexture.Reset();

	// Ŭ���� Ÿ�̸� �ؽ�Ʈ ����
	if (m_ClearTimerText)
	{
		m_ClearTimerText->Shutdown();
		delete m_ClearTimerText;
		m_ClearTimerText = 0;
	}

	// Ÿ�̸� �ؽ�Ʈ �� ��Ʈ ����
	if (m_TimerText)
	{
		m_Font->Shutdown();
		delete m_TimerText;
		m_TimerText = 0;
	}

	// ���콺 ���ڿ� �ؽ�Ʈ ����
	if (m_MouseStrings)
	{
		m_MouseStrings[0].Shutdown();
		m_MouseStrings[1].Shutdown();
		m_MouseStrings[2].Shutdown();

		delete[] m_MouseStrings;
		m_MouseStrings = 0;
	}

	// ��Ʈ ����
	if (m_Font)
	{
		m_Font->Shutdown();
		delete m_Font;
		m_Font = 0;
	}

	// ��Ʈ ���̴� ����
	if (m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	// �÷��̾� ĳ���� ����
	if (m_PlayerCharacter)
	{
		m_PlayerCharacter->Shutdown();
		delete m_PlayerCharacter;
		m_PlayerCharacter = 0;
	}

	// Ŀ�� ��Ʈ�� ����
	if (m_Cursor)
	{
		m_Cursor->Shutdown();
		delete m_Cursor;
		m_Cursor = 0;
	}

	// ���� �ΰ� ����
	if (m_GameLogo)
	{
		m_GameLogo->Shutdown();
		delete m_GameLogo;
		m_GameLogo = 0;
	}
	
	// �ؽ�ó ���̴� ����
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// ī�޶� ����
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Direct3D ����
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

	// ESC Ű�� ���ȴ��� Ȯ���ϰ� �������� ���α׷� ����
	if (Input->IsEscapePressed())
	{
		return false;
	}

	// �Է� ��ü���� ���콺 ��ġ�� �����ɴϴ�.
	Input->GetMouseLocation(mouseX, mouseY);

	// ���콺 Ŭ�� ���� Ȯ��
	mouseDown = Input->IsMousePressed();

	// ���콺 ��ǥ ���ڿ� ������Ʈ
	result = UpdateMouseStrings(mouseX, mouseY, mouseDown);
	if (!result)
	{
		return false;
	}

	// �κ� ������ ��� �κ� ȭ�� ������
	if (m_Lobby)
	{
		DirectX::XMFLOAT2 buttonPos(400, 450);
		float buttonWidth = 512 * 0.25f;  
		float buttonHeight = 271 * 0.25f;

		float left = buttonPos.x - buttonWidth / 2;
		float right = buttonPos.x + buttonWidth / 2;
		float top = buttonPos.y - buttonHeight / 2;
		float bottom = buttonPos.y + buttonHeight / 2;

		// ���콺 Ŭ������ ���� ����
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

	// ������ ���۵� ���¶�� ���� ���� ó��
	if (m_Gamestart)
	{
		
		const int moveSpeed = 5;  // �̵� �ӵ�

		if (m_Keyboardalbe)
		{
			if (Input->IsKeyPressed(DIK_UP)) playerY -= moveSpeed;      // �� ����Ű �� ���� �̵�
			if (Input->IsKeyPressed(DIK_DOWN)) playerY += moveSpeed;    // �� ����Ű �� �Ʒ��� �̵�
			if (Input->IsKeyPressed(DIK_LEFT)) playerX -= moveSpeed;    // �� ����Ű �� ���� �̵�
			if (Input->IsKeyPressed(DIK_RIGHT)) playerX += moveSpeed;   // �� ����Ű �� ������ �̵�
		}
		// ȭ�� ��踦 ����� �ʵ��� ó��
		if (playerX < 0) playerX = 0;
		if (playerY < 0) playerY = 0;
		if (playerX > 800 - 50) playerX = 800 - 50; // ȭ�� �ʺ� 800, ĳ���� ũ�� 50px
		if (playerY > 600 - 50) playerY = 600 - 50; // ȭ�� ���� 600

		// ���� �÷��� ȭ�� ������
		result = RenderGameStart(playerX, playerY, Input);
		if (!result)
		{
			return false;
		}
	}

	// ���� ���� ó��
	if (m_Gameover)
	{		
		
		result = GameOver(Input);
		if (!result)
		{
			return false;
		}
		m_PlayerCharacter->SetPlayerChange(false);		
		
	}
	// ���� Ŭ���� ó��
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

	// ȭ���� ����� ������ ����
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ��� �ҷ�����
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// Z���� ���� ���� ���� Ȱ��ȭ
	m_Direct3D->TurnZBufferOff();
	m_Direct3D->EnableAlphaBlending();

	// �ΰ� ������
	result = RogoRendering(worldMatrix, viewMatrix, orthoMatrix);
	if (!result) return false;

	// ���� ��ư ������
	result = RenderStartButton();
	if (!result) return false;

	// ���콺 ���� �ؽ�Ʈ ������
	for (i = 0; i < 3; i++)
	{
		m_MouseStrings[i].Render(m_Direct3D->GetDeviceContext());
		result = m_FontShader->Render(m_Direct3D->GetDeviceContext(), m_MouseStrings[i].GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Font->GetTexture(), m_MouseStrings[i].GetPixelColor());
		if (!result) return false;
	}

	m_Direct3D->DisableAlphaBlending();

	// Ŀ�� ������
	result = CursorRendering(true, mouseX, mouseY, worldMatrix, viewMatrix, orthoMatrix);
	if (!result) return false;

	// Z���� �ٽ� �ѱ�
	m_Direct3D->TurnZBufferOn();

	// ������ �Ϸ� �� ���
	m_Direct3D->EndScene();

	return true;
}


bool ApplicationClass::RenderGameStart(int keyboardX, int keyboardY, InputClass* Input)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	int i;
	bool result;
	int mouseX, mouseY;

	// �Է� Ŭ�����κ��� ���콺 ��ǥ�� �����ɴϴ�.
	Input->GetMouseLocation(mouseX, mouseY);

	// ȭ���� �ʱ�ȭ�ϰ� �������� �����մϴ�.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶�� D3D ��ü�κ��� ��ĵ��� ����ϴ�.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// 2D �������� ���� Z ���۸� ���� ���� ������ Ȱ��ȭ�մϴ�.
	m_Direct3D->TurnZBufferOff();
	m_Direct3D->EnableAlphaBlending();

	// Ŭ���� ������ �����Ǿ����� Ȯ���մϴ�.
	if (CheckClearGame())
	{
		m_Gameclear = true;
		m_Keyboardalbe = false;
		return true;
	}

	// ���� ������ �ƴ϶�� Ÿ�̸Ӹ� ������Ʈ�մϴ�.
	if (!m_Gameover)
	{
		UpdateTimer();
	}

	// Ÿ�̸� �ؽ�Ʈ�� �������մϴ�.
	m_TimerText->Render(m_Direct3D->GetDeviceContext());
	m_FontShader->Render(m_Direct3D->GetDeviceContext(), m_TimerText->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Font->GetTexture(), m_TimerText->GetPixelColor());

	// �÷��̾�� AI ĳ���� ���� ��ȣ�ۿ� ó��
	for (int i = 0; i < 15; i++)
	{
		if (m_PlayerCharacter->ResolveInteraction(Characters[i]))
		{
			// �÷��̾ ���������� �� ���������� ���� ����
			if (m_PlayerCharacter->Gettype() == 0) // ����
			{
				if (Characters[i]->Gettype() == 1) // ���� �� �̱�
				{
					Characters[i]->SetTexture(rockTex);
					Characters[i]->Settype(0);
				}
				if (Characters[i]->Gettype() == 2) // �� �� ��
				{
					m_PlayerCharacter->SetPlayerChange(true);
				}
			}
			else if (m_PlayerCharacter->Gettype() == 1) // ����
			{
				if (Characters[i]->Gettype() == 2) // �� �� �̱�
				{
					Characters[i]->SetTexture(scissorTex);
					Characters[i]->Settype(1);
				}
				if (Characters[i]->Gettype() == 0) // ���� �� ��
				{
					m_PlayerCharacter->SetPlayerChange(true);
				}
			}
			else if (m_PlayerCharacter->Gettype() == 2) // ��
			{
				if (Characters[i]->Gettype() == 0) // ���� �� �̱�
				{
					Characters[i]->SetTexture(paperTex);
					Characters[i]->Settype(2);
				}
				if (Characters[i]->Gettype() == 1) // ���� �� ��
				{
					m_PlayerCharacter->SetPlayerChange(true);
				}
			}
		}

		// AI ĳ���� ������
		Characters[i]->Render(m_Direct3D->GetDeviceContext(), Characters[i]->GetpositionX(), Characters[i]->GetpositionY());
		m_TextureShader->Render(m_Direct3D->GetDeviceContext(), Characters[i]->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, Characters[i]->GetTexture());
	}

	// AI ĳ���͵� ���� ��ȣ�ۿ� ó��
	for (int i = 0; i < 15; i++)
	{
		for (int j = i + 1; j < 15; j++)
		{
			if (i != j)
			{
				if (Characters[i]->ResolveInteraction(Characters[j]))
				{
					if (Characters[i]->Gettype() == 0) // ����
					{
						if (Characters[j]->Gettype() == 1) //����
						{
							Characters[j]->SetTexture(rockTex);
							Characters[j]->Settype(0);
						}
						if (Characters[j]->Gettype() == 2)//��
						{
							Characters[i]->SetTexture(paperTex);
							Characters[i]->Settype(2);
						}
					}
					else if (Characters[i]->Gettype() == 1) // ����
					{
						if (Characters[j]->Gettype() == 0) // ����
						{
							Characters[i]->SetTexture(rockTex);
							Characters[i]->Settype(0);
						}
						if (Characters[j]->Gettype() == 2) // ��
						{
							Characters[j]->SetTexture(scissorTex);
							Characters[j]->Settype(1);
						}
					}
					else if (Characters[i]->Gettype() == 2) // ��
					{
						if (Characters[j]->Gettype() == 0)//����
						{
							Characters[j]->SetTexture(paperTex);
							Characters[j]->Settype(2);
						}
						if (Characters[j]->Gettype() == 1) // ����
						{
							Characters[i]->SetTexture(scissorTex);
							Characters[i]->Settype(1);
						}
					}
				}
			}
		}

		// AI ĳ���� ������
		Characters[i]->Render(m_Direct3D->GetDeviceContext(), Characters[i]->GetpositionX(), Characters[i]->GetpositionY());
		m_TextureShader->Render(m_Direct3D->GetDeviceContext(), Characters[i]->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, Characters[i]->GetTexture());
	}

	// �÷��̾� ĳ���� ������ �غ�
	result = m_PlayerCharacter->Render(m_Direct3D->GetDeviceContext(), keyboardX, keyboardY);
	if (!result) return false;

	// �÷��̾� ĳ���� �ؽ�ó ������
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_PlayerCharacter->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_PlayerCharacter->GetTexture());
	if (!result) return false;

	// �÷��̾ ��ȣ�ۿ� ����� ���°� ����Ǿ��� ��� �� ���� ����
	if (m_PlayerCharacter->GetPlayerChange() == true)
	{
		m_Gameover = true;
		m_Keyboardalbe = false;
	}

	// ��� 2D �������� �Ϸ�Ǿ����Ƿ� Z ���� �ٽ� Ȱ��ȭ
	m_Direct3D->TurnZBufferOn();

	// �������� ȭ���� ���
	m_Direct3D->EndScene();

	return true;
}


bool ApplicationClass::UpdateMouseStrings(int mouseX, int mouseY, bool mouseDown)
{
	char tempString[16], finalString[32];
	bool result;

	// ���콺 X ��ǥ�� ���ڿ��� ��ȯ�Ͽ� ������ �ؽ�Ʈ ����
	sprintf_s(tempString, "%d", mouseX);
	strcpy_s(finalString, "Mouse X: ");
	strcat_s(finalString, tempString);
	result = m_MouseStrings[0].UpdateText(m_Direct3D->GetDeviceContext(), m_Font, finalString, 10, 10, 1.0f, 1.0f, 1.0f);
	if (!result) return false;

	// ���콺 Y ��ǥ�� ���ڿ��� ��ȯ�Ͽ� ������ �ؽ�Ʈ ����
	sprintf_s(tempString, "%d", mouseY);
	strcpy_s(finalString, "Mouse Y: ");
	strcat_s(finalString, tempString);
	result = m_MouseStrings[1].UpdateText(m_Direct3D->GetDeviceContext(), m_Font, finalString, 10, 35, 1.0f, 1.0f, 1.0f);
	if (!result) return false;

	// ���콺 ��ư Ŭ�� ���� ������Ʈ
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

	// ����� �����ϱ� ���� ���۸� ����ϴ�.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶�� D3D ��ü�κ��� ����, ��, ���� ���� ����� �����ɴϴ�.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// 2D �������� �����ϱ� ���� Z ���۸� ���� ���� ������ �մϴ�.
	m_Direct3D->TurnZBufferOff();
	m_Direct3D->EnableAlphaBlending();

	// ���콺 Ŭ�� ���θ� Ȯ���մϴ�.
	mouseDown = Input->IsMousePressed();
	Input->GetMouseLocation(mouseX, mouseY);

	// ���� ���� ���¿����� ĳ���͵��� ����ϴ�.
	for (int i = 0; i < 15; i++)
	{
		Characters[i]->Setspeed(0.0f, 0.0f);
	}

	// "Game Over" �ؽ�ó ������
	if (m_spriteBatch && m_GameoverTexture)
	{
		m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred);

		// ��ư ��ġ ����
		DirectX::XMFLOAT2 position(400, 300);
		DirectX::XMFLOAT2 scale(0.5f, 0.5f);  // �ؽ�ó�� ���� ũ��� ������

		// �ؽ�ó ũ�� ���� ����
		D3D11_TEXTURE2D_DESC textureDesc;
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		m_GameoverTexture->GetResource(resource.GetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		resource.As(&texture);
		texture->GetDesc(&textureDesc);

		// ������ �߾����� ����
		DirectX::XMFLOAT2 origin(textureDesc.Width / 2.0f, textureDesc.Height / 2.0f);

		// ���ӿ��� �ؽ�ó ������
		m_spriteBatch->Draw(m_GameoverTexture.Get(), position, nullptr, DirectX::Colors::White, 0.0f, origin, scale);

		m_spriteBatch->End();
	}

	// "EXIT" ��ư ������ �� Ŭ�� ó��
	if (m_spriteBatch && m_ExitTexture)
	{
		m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred);

		DirectX::XMFLOAT2 position(200, 500);
		DirectX::XMFLOAT2 scale(0.3f, 0.3f);  // ũ�� ����

		D3D11_TEXTURE2D_DESC textureDesc;
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		m_ExitTexture->GetResource(resource.GetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		resource.As(&texture);
		texture->GetDesc(&textureDesc);

		DirectX::XMFLOAT2 origin(textureDesc.Width / 2.0f, textureDesc.Height / 2.0f);

		m_spriteBatch->Draw(m_ExitTexture.Get(), position, nullptr, DirectX::Colors::White, 0.0f, origin, scale);

		m_spriteBatch->End();

		// ���콺 Ŭ�� ���� ����
		float buttonWidth = 600 * 0.3f;
		float buttonHeight = 160 * 0.3f;

		float left = position.x - buttonWidth / 2;
		float right = position.x + buttonWidth / 2;
		float top = position.y - buttonHeight / 2;
		float bottom = position.y + buttonHeight / 2;

		if (mouseX >= left && mouseX <= right &&
			mouseY >= top && mouseY <= bottom && mouseDown)
		{
			// "EXIT" Ŭ�� �� �ʱ� ���·� ����
			m_Lobby = true;
			m_Gamestart = false;
			m_Keyboardalbe = true;
			m_Gameover = false;
			SetCharacters(800, 600);

			return true;
		}
	}

	// "RETRY" ��ư ������ �� Ŭ�� ó��
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
			// "RETRY" Ŭ�� �� ���� �����
			m_startTime = std::chrono::steady_clock::now();
			m_Lobby = false;
			m_Gamestart = true;
			m_Gameover = false;
			m_Keyboardalbe = true;
			SetCharacters(800, 600);

			return true;
		}
	}

	// Ŀ�� ������
	result = CursorRendering(true, mouseX, mouseY, worldMatrix, viewMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	// 2D �������� �Ϸ�Ǿ����Ƿ� Z ���� �ٽ� Ȱ��ȭ
	m_Direct3D->TurnZBufferOn();

	// ���� ����� ȭ�鿡 ���
	m_Direct3D->EndScene();

	// UI â�� ǥ�õǰ� ���� ���°� ��
	return true;
}


bool ApplicationClass::GameClear(InputClass* Input)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	bool result, mouseDown;
	int mouseX, mouseY;

	// ����� �����ϱ� ���� ���۸� Ŭ�����մϴ�.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶� �� D3D ��ü�κ��� ����, ��, ���� ����� �����ɴϴ�.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// 2D �������� ���� Z���۸� ���� ���� ������ Ȱ��ȭ�մϴ�.
	m_Direct3D->TurnZBufferOff();
	m_Direct3D->EnableAlphaBlending();

	// ���콺 �Է��� Ȯ���մϴ�.
	mouseDown = Input->IsMousePressed();
	Input->GetMouseLocation(mouseX, mouseY);

	// ��� ĳ������ �ӵ��� 0���� �����Ͽ� ������ŵ�ϴ�.
	for (int i = 0; i < 15; i++)
	{
		Characters[i]->Setspeed(0.0f, 0.0f);
	}

	// "���� Ŭ����" �ؽ�ó�� ȭ�� �߾ӿ� ������
	if (m_spriteBatch && m_GameclearTexture)
	{
		m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred);

		// ��ġ�� ũ�� ����
		DirectX::XMFLOAT2 position(400, 300);
		DirectX::XMFLOAT2 scale(0.3f, 0.3f);  // 30% ũ��� ����

		// �ؽ�ó ũ�� ��������
		D3D11_TEXTURE2D_DESC textureDesc;
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		m_GameclearTexture->GetResource(resource.GetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		resource.As(&texture);
		texture->GetDesc(&textureDesc);

		// �߾� ������ ����
		DirectX::XMFLOAT2 origin(textureDesc.Width / 2.0f, textureDesc.Height / 2.0f);

		// �ؽ�ó ���
		m_spriteBatch->Draw(m_GameclearTexture.Get(), position, nullptr, DirectX::Colors::White, 0.0f, origin, scale);
		m_spriteBatch->End();
	}

	// "EXIT" ��ư ó��
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

		// Ŭ�� ���� ���
		float buttonWidth = 600 * 0.3f;
		float buttonHeight = 160 * 0.3f;
		float left = position.x - buttonWidth / 2;
		float right = position.x + buttonWidth / 2;
		float top = position.y - buttonHeight / 2;
		float bottom = position.y + buttonHeight / 2;

		// Ŭ�� ����
		if (mouseX >= left && mouseX <= right &&
			mouseY >= top && mouseY <= bottom && mouseDown)
		{
			// �κ�� ���ư�
			m_Lobby = true;
			m_Gamestart = false;
			m_Keyboardalbe = true;
			m_Gameclear = false;
			SetCharacters(800, 600);
			return true;
		}
	}

	// "RETRY" ��ư ó��
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
			// ���� �����
			m_startTime = std::chrono::steady_clock::now();
			m_Lobby = false;
			m_Gamestart = true;
			m_Gameclear = false;
			m_Keyboardalbe = true;
			SetCharacters(800, 600);
			return true;
		}
	}

	// Ŭ���� �ð� �ؽ�Ʈ ������Ʈ �� ������
	m_Font->SetFontSize(50.0f);
	sprintf_s(initialTimeString, "Time: %.2f", m_elapsedTime);
	result = m_ClearTimerText->UpdateText(m_Direct3D->GetDeviceContext(), m_Font, initialTimeString, 370, 200, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}
	m_ClearTimerText->Render(m_Direct3D->GetDeviceContext());
	m_FontShader->Render(m_Direct3D->GetDeviceContext(), m_ClearTimerText->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Font->GetTexture(), m_ClearTimerText->GetPixelColor());

	// Ŀ�� ������
	result = CursorRendering(true, mouseX, mouseY, worldMatrix, viewMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	// 2D ������ �Ϸ� �� Z���� �ٽ� �ѱ�
	m_Direct3D->TurnZBufferOn();

	// ���� ����� ȭ�鿡 ���
	m_Direct3D->EndScene();

	// ���� ó�� �Ϸ�
	return true;
}


bool ApplicationClass::CursorRendering(bool m_showCursor, int mouseX, int mouseY, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX orthoMatrix)
{
	bool result;

	// Ŀ���� ���̵��� �����Ǿ��� ���� ������
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
		//newCharacter->SetTexture(rockTex); // ĳ�ÿ��� ����
		newCharacter->Settype(0);
		newCharacter->SetNotPlayer();
		Characters.push_back(newCharacter); // �׷� i�� �߰�
	}

	for (int i = 5; i < 10; i++)
	{
		int randomX = rand() % screenWidth;
		int randomY = rand() % screenHeight;

		BitmapClass* newCharacter = new BitmapClass();
		newCharacter->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, scissorbitmapFilename, randomX, randomY);
		//newCharacter->SetTexture(scissorTex); // ĳ�ÿ��� ����
		newCharacter->Settype(1);
		newCharacter->SetNotPlayer();
		Characters.push_back(newCharacter); // �׷� i�� �߰�
	}

	for (int i = 10; i < 15; i++)
	{
		int randomX = rand() % screenWidth;
		int randomY = rand() % screenHeight;

		BitmapClass* newCharacter = new BitmapClass();
		newCharacter->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, paperbitmapFilename, randomX, randomY); // textureFilename�� ����
		//newCharacter->SetTexture(paperTex); // ĳ�ÿ��� ����
		newCharacter->Settype(2);
		newCharacter->SetNotPlayer();
		Characters.push_back(newCharacter); // �׷� i�� �߰�
	}
}

void ApplicationClass::SetCharacters(int screenWidth, int screenHeight)
{
	OutputDebugStringA("SetCharacters ȣ���\n");
	char buffer[128];

	for (int i = 0; i < 5; i++)
	{
		int randomX = rand() % screenWidth;
		int randomY = rand() % screenHeight;

		// �� �κп� �ؽ�ó ��ü�ϴ� �ڵ�
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

		// �� �κп� �ؽ�ó ��ü�ϴ� �ڵ�
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

		// �� �κп� �ؽ�ó ��ü�ϴ� �ڵ�
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
	// Characters ���Ϳ� ����� ��� ���� ��ü�� ����
	for (auto character : Characters)
	{
		if (character)
		{
			character->Shutdown(); // BitmapClass�� ���ҽ� ����
			//delete character;      // �޸� ����
		}
	}

	// ���͸� ���
	Characters.clear();
}

void ApplicationClass::ResetUI()
{
	// ��� UI ���� �ʱ�ȭ
	m_Gameover = false;
	m_Gamestart = false;
	m_Gameclear = false;
	m_Lobby = true;

	// ���� ���� ���� ����
	m_isGameStarted = false;
	m_elapsedTime = 0.0f;
	m_Keyboardalbe = false;

	// �ؽ�ó ���� (���ʿ��� �ؽ�ó�� �������� �ʵ���)
	m_GameoverTexture.Reset();
	m_ExitTexture.Reset();
	m_RetryTexture.Reset();

	// GameOver UI�� ���� ���� �ʵ��� ȭ�� �ʱ�ȭ
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	m_Direct3D->TurnZBufferOff();
	m_Direct3D->EnableAlphaBlending();

	// �κ� UI ������
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
	// SpriteBatch ����
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(m_Direct3D->GetDeviceContext());

	// ��ư �ؽ�ó �ε� (button.tga �Ǵ� button.png ��� ����)
	HRESULT hr = DirectX::CreateWICTextureFromFile(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(),
		L"../Engine/data/start.png", nullptr, &m_buttonTexture);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"��ư �ؽ�ó �ε� ����!", L"Error", MB_OK);
	}


	// "���� ��ư" �ؽ�ó �ε�
	hr = DirectX::CreateWICTextureFromFile(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(),
		L"../Engine/data/png/exit.png", nullptr, &m_ExitTexture);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"���� ��ư �ؽ�ó �ε� ����!", L"Error", MB_OK);
	}

	// "��õ� ��ư" �ؽ�ó �ε�
	hr = DirectX::CreateWICTextureFromFile(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(),
		L"../Engine/data/png/retry.png", nullptr, &m_RetryTexture);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"��õ� ��ư �ؽ�ó �ε� ����!", L"Error", MB_OK);
	}

	// "���ӿ��� ��ư" �ؽ�ó �ε�
	hr = DirectX::CreateWICTextureFromFile(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(),
		L"../Engine/data/png/Gameover.png", nullptr, &m_GameoverTexture);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"��õ� ��ư �ؽ�ó �ε� ����!", L"Error", MB_OK);
	}

	// "���� Ŭ���� ��ư" �ؽ�ó �ε�
	hr = DirectX::CreateWICTextureFromFile(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(),
		L"../Engine/data/png/Gameclear.png", nullptr, &m_GameclearTexture);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"��õ� ��ư �ؽ�ó �ε� ����!", L"Error", MB_OK);
	}
}

bool ApplicationClass::RenderStartButton()
{
	if (m_spriteBatch && m_buttonTexture)
	{
		m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred);

		// ��ư�� ��ġ
		DirectX::XMFLOAT2 position(400, 450);
		DirectX::XMFLOAT2 scale(0.25f, 0.25f);  // ũ�� ���� (��: 25%) 128 70

		// �ؽ�ó ũ�� ��������
		D3D11_TEXTURE2D_DESC textureDesc;
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		m_buttonTexture->GetResource(resource.GetAddressOf());

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		resource.As(&texture);
		texture->GetDesc(&textureDesc);

		// ���� (�߾�)
		DirectX::XMFLOAT2 origin(textureDesc.Width / 2.0f, textureDesc.Height / 2.0f);

		// �߾� ����
		m_spriteBatch->Draw(m_buttonTexture.Get(), position, nullptr, DirectX::Colors::White, 0.0f,
			origin, scale);

		m_spriteBatch->End();

		return true;
	}
	return false;
}

void ApplicationClass::UpdateTimer()
{

	// ���� �ð� ��������
	auto now = std::chrono::steady_clock::now();

	// ����� �ð�(�и��� ����)
	auto duration = std::chrono::duration<double>(now - m_startTime);
	m_elapsedTime = duration.count(); // �� ���� �Ҽ��� ����

	// ���ο� �ð� ���ڿ� ���� (1/10��, 1/100�� ����)
	char timeString[32];
	sprintf_s(timeString, "Time: %.2f", m_elapsedTime);

	// �ؽ�Ʈ ������Ʈ
	m_TimerText->UpdateText(m_Direct3D->GetDeviceContext(), m_Font, timeString, 700, 10, 1.0f, 1.0f, 1.0f);
}


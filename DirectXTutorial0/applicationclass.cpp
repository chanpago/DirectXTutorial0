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
	char bitmapFilename[128], rockbitmapFilename[128], scissorbitmapFilename[128], paperbitmapFilename[128];
	char mouseString1[32], mouseString2[32], mouseString3[32];
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
		if (!result)
		{
			return false;
		}
	}
	else if (randomnum == 1)
	{
		result = m_PlayerCharacter->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, scissorbitmapFilename, 10, 10);
		if (!result)
		{
			return false;
		}
	}
	else if (randomnum == 2)
	{
		result = m_PlayerCharacter->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, paperbitmapFilename, 10, 10);
		if (!result)
		{
			return false;
		}
	}
	

	// Create and initialize the bitmap object.
	m_RockCharacter = new BitmapClass;

	result = m_RockCharacter->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, rockbitmapFilename, 10, 10);
	if (!result)
	{
		return false;
	}

	// Create and initialize the bitmap object.
	m_ScissorCharacter = new BitmapClass;

	result = m_ScissorCharacter->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, scissorbitmapFilename, 10, 10);
	if (!result)
	{
		return false;
	}

	// Create and initialize the bitmap object.
	m_PaperCharacter = new BitmapClass;

	result = m_PaperCharacter->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, paperbitmapFilename, 10, 10);
	if (!result)
	{
		return false;
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


	return true;
}


void ApplicationClass::Shutdown()
{
	// Release the text objects for the mouse strings.
	if (m_MouseStrings)
	{
		m_MouseStrings[0].Shutdown();
		m_MouseStrings[1].Shutdown();
		m_MouseStrings[2].Shutdown();

		delete[] m_MouseStrings;
		m_MouseStrings = 0;
	}

	// Release the text string objects.
	if (m_TextString2)
	{
		m_TextString2->Shutdown();
		delete m_TextString2;
		m_TextString2 = 0;
	}

	if (m_TextString1)
	{
		m_TextString1->Shutdown();
		delete m_TextString1;
		m_TextString1 = 0;
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

	// Release the bitmap object.
	if (m_Cursor)
	{
		m_Cursor->Shutdown();
		delete m_Cursor;
		m_Cursor = 0;
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

	// 버튼 클릭 여부 확인 (버튼이 보일 때만 검사)
	if (m_showStartButton)
	{
		DirectX::XMFLOAT2 buttonPos(400, 450);
		float buttonWidth = 256 * 0.25f;  // 텍스처 크기의 25% (예제 스케일 적용)
		float buttonHeight = 128 * 0.25f;

		float left = buttonPos.x - buttonWidth / 2;
		float right = buttonPos.x + buttonWidth / 2;
		float top = buttonPos.y - buttonHeight / 2;
		float bottom = buttonPos.y + buttonHeight / 2;

		if (mouseX >= left && mouseX <= right &&
			mouseY >= top && mouseY <= bottom && mouseDown)
		{
			// 버튼 클릭됨 -> 숨김 처리
			m_showStartButton = false;
		}
		result = Render(mouseX, mouseY);
		if (!result)
		{
			return false;
		}
	}
	else if (!m_showStartButton)
	{
		int moveSpeed = 5;  // 이동 속도

		if (Input->IsKeyPressed(DIK_UP)) playerY -= moveSpeed;      // ↑ 방향키 → 위로 이동
		if (Input->IsKeyPressed(DIK_DOWN)) playerY += moveSpeed;    // ↓ 방향키 → 아래로 이동
		if (Input->IsKeyPressed(DIK_LEFT)) playerX -= moveSpeed;    // ← 방향키 → 왼쪽 이동
		if (Input->IsKeyPressed(DIK_RIGHT)) playerX += moveSpeed;   // → 방향키 → 오른쪽 이동

		// 화면 경계를 벗어나지 않도록 처리
		if (playerX < 0) playerX = 0;
		if (playerY < 0) playerY = 0;
		if (playerX > 800 - 50) playerX = 800 - 50; // 예제: 화면 너비 800, 캐릭터 크기 50px
		if (playerY > 600 - 50) playerY = 600 - 50; // 예제: 화면 높이 600

		// Render the graphics scene.
		result = RenderGameStart(playerX, playerY);
		if (!result)
		{
			return false;
		}
	}
	

	return true;
}




bool ApplicationClass::Render(int mouseX, int mouseY)
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

	RenderUI();



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



	// Put the bitmap vertex and index buffers on the graphics pipeline to prepare them for drawing.
	result = m_Cursor->Render(m_Direct3D->GetDeviceContext(), mouseX, mouseY);
	if (!result)
	{
		return false;
	}

	// Render the bitmap with the texture shader.
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Cursor->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Cursor->GetTexture());
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

bool ApplicationClass::RenderGameStart(int keyboardX, int keyboardY)
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


	for (int i = 0; i < 15; i++)
	{
		Characters[i]->Render(m_Direct3D->GetDeviceContext(), 0, 0);
		m_TextureShader->Render(m_Direct3D->GetDeviceContext(), Characters[i]->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, Characters[i]->GetTexture());
		
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
}

void ApplicationClass::RenderUI()
{
	if (m_spriteBatch && m_buttonTexture && m_showStartButton)
	{
		m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred);

		// 버튼의 위치
		DirectX::XMFLOAT2 position(400, 450);
		DirectX::XMFLOAT2 scale(0.25f, 0.25f);  // 크기 조정 (예: 25%)

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
	}
}

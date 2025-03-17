////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_



///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "inputclass.h"
#include "cameraclass.h"
#include "textureshaderclass.h"
#include "bitmapclass.h"

#include "playerbitmap.h"
#include "fontshaderclass.h"
#include "fontclass.h"
#include "textclass.h"
#include <SpriteBatch.h>
#include <vector>
#include <WICTextureLoader.h>
#include <chrono>
#include <wrl/client.h> // Microsoft::WRL::ComPtr 사용


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


////////////////////////////////////////////////////////////////////////////////
// Class name: ApplicationClass
////////////////////////////////////////////////////////////////////////////////
class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(InputClass*);

	void InitializeUI();
	bool RenderStartButton();
	void UpdateTimer();


private:
	bool RenderLobby(int,int);
	bool RenderGameStart(int, int, InputClass*);
	bool UpdateMouseStrings(int, int, bool);
	bool UpdatePlayerCharacter(int, int, bool);

	bool AIRender();
	bool GameOver(InputClass*);
	bool GameClear(InputClass*);

	bool CursorRendering(bool, int, int, XMMATRIX, XMMATRIX, XMMATRIX);
	bool RogoRendering(XMMATRIX, XMMATRIX, XMMATRIX);
	void RenderCharacters(int,int);
	void ClearCharacters();
	void ResetUI();

	void RenderText(float, char*, int, int);
	bool CheckClearGame();

private:
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	TextureShaderClass* m_TextureShader;
	BitmapClass* m_Cursor;
	BitmapClass* m_GameOverBitmap;
	BitmapClass* m_GameClear;
	BitmapClass* m_GameLogo;
	FontShaderClass* m_FontShader;
	FontClass* m_Font;
	TextClass* m_TextString1, * m_TextString2;
	TextClass* m_MouseStrings;

	BitmapClass* m_PlayerCharacter;


	vector<BitmapClass*> Characters;
	BitmapClass* m_RockCharacter;
	BitmapClass* m_ScissorCharacter;
	BitmapClass* m_PaperCharacter;


	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_buttonTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ExitTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_RetryTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_GameoverTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_GameclearTexture;
	bool m_showStartButton = true; // 버튼 표시 여부 (초기값: true)


	int playerX;
	int playerY;


	char bitmapFilename[128], rockbitmapFilename[128], scissorbitmapFilename[128], paperbitmapFilename[128], initialTimeString[32], GameLogoFilename[32];

	std::chrono::steady_clock::time_point m_startTime; // 게임 시작 시간
	double m_elapsedTime; // 경과한 시간 (초 단위, 소수점 포함)
	TextClass* m_TimerText; // 화면에 출력할 타이머 텍스트
	TextClass* m_ClearTimerText;
	bool m_isGameStarted; // 게임이 시작되었는지 여부

	bool m_Keyboardalbe;
	bool m_ShowCursor;
	bool m_Gameover;
	bool m_Gamestart;
	bool m_Gameclear;
	bool m_Lobby;
	bool m_Rc;
};

#endif
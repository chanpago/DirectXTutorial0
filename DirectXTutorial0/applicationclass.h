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
#include "rockbitmap.h"
#include "scissorbitmap.h"
#include "paperbitmap.h"
#include "fontshaderclass.h"
#include "fontclass.h"
#include "textclass.h"
#include <SpriteBatch.h>
#include <vector>
#include <WICTextureLoader.h>
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
	void RenderUI();

private:
	bool Render(int,int);
	bool RenderGameStart(int, int);
	bool UpdateMouseStrings(int, int, bool);
	bool UpdatePlayerCharacter(int, int, bool);

	bool AIRender();

private:
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	TextureShaderClass* m_TextureShader;
	BitmapClass* m_Cursor;
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
	bool m_showStartButton = true; // 버튼 표시 여부 (초기값: true)


	int playerX;
	int playerY;

	RockBitmapClass** RockBitmap;



};

#endif
////////////////////////////////////////////////////////////////////////////////
// Filename: bitmapclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _BITMAPCLASS_H_
#define _BITMAPCLASS_H_

//////////////
// INCLUDES //
//////////////
#include <directxmath.h>
using namespace DirectX;

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: BitmapClass
////////////////////////////////////////////////////////////////////////////////
class BitmapClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	BitmapClass();
	BitmapClass(const BitmapClass&);
	~BitmapClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, int, int, char*, int, int);
	void Shutdown();
	bool Render(ID3D11DeviceContext*,int ,int);

	int GetIndexCount();
	int GetpositionX();
	int GetpositionY();
	int GetspeedX();
	int GetspeedY();
	int GetprespeedX();
	int GetprespeedY();
	int Gettype();
	bool GetPlayerChange();

	ID3D11ShaderResourceView* GetTexture();

	void SetRenderLocation(int, int);
	void SetNotPlayer();
	void SetPlayer();
	void Settype(int);
	void Setposition(int, int);
	void Setspeed(float, float);
	void Setprespeed(int, int);
	void SetPlayerChange(bool);
	void SetSize(int, int);
	void SetTexture(TextureClass* texture);

	void UpdatePosition();

	bool CheckCollision(BitmapClass* other);
	bool ResolveInteraction(BitmapClass* other);
	void ChangeTexture(ID3D11Device*, ID3D11DeviceContext*, char*);

	bool Checkcollision[15];
	void SetCheckcollision();


private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext*, int, int);
	void RenderBuffers(ID3D11DeviceContext*);
	
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);

	void ReleaseTexture();
	 // 위치 업데이트 함수 추가

	


private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount, m_screenWidth, m_screenHeight, m_bitmapWidth, m_bitmapHeight, m_prevPosX, m_prevPosY;
	TextureClass* m_Texture;
	float m_speedX, m_speedY, m_renderX, m_renderY; // 속력 변수 추가
	int m_prespeedX, m_prespeedY;
	int m_type;
	bool IsPlayer;
	bool m_PlayerChange;
};

#endif
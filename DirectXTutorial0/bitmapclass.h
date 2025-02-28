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
	ID3D11ShaderResourceView* GetTexture();

	void SetRenderLocation(int, int);
	void SetNotPlayer();
	void SetPlayer();
	void Settype(int);

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext*, int, int);
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();
	void UpdatePosition(); // ��ġ ������Ʈ �Լ� �߰�

	bool IsPlayer;

private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount, m_screenWidth, m_screenHeight, m_bitmapWidth, m_bitmapHeight, m_renderX, m_renderY, m_prevPosX, m_prevPosY;
	TextureClass* m_Texture;
	int m_speedX, m_speedY; // �ӷ� ���� �߰�

	int type;
};

#endif
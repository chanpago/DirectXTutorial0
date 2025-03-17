////////////////////////////////////////////////////////////////////////////////
// Filename: bitmapclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "bitmapclass.h"
enum { ROCK, SCISSOR, PAPER };

BitmapClass::BitmapClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
	IsPlayer = false;
	// 랜덤한 속도 설정 (-3 ~ 3 사이의 속도)
	m_speedX = ((rand() % 2001) / 1000.0f) - 1.0f;
	m_speedY = ((rand() % 2001) / 1000.0f) - 1.0f;
	//m_speedX = 0;
	//m_speedY = 0;
	m_prespeedX = m_speedX;
	m_prespeedY = m_speedY;
	m_PlayerChange = false;
}



BitmapClass::BitmapClass(const BitmapClass& other)
{
}


BitmapClass::~BitmapClass()
{
}


bool BitmapClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int screenWidth, int screenHeight, char* textureFilename, int renderX, int renderY)
{
	bool result;


	// Store the screen size.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Store where the bitmap should be rendered to.
	m_renderX = renderX;
	m_renderY = renderY;

	// Initialize the vertex and index buffer that hold the geometry for the bitmap quad.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// Load the texture for this bitmap.
	result = LoadTexture(device, deviceContext, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}


void BitmapClass::Shutdown()
{
	// Release the bitmap texture.
	ReleaseTexture();

	// Release the vertex and index buffers.
	ShutdownBuffers();

	return;
}


bool BitmapClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	bool result;

	if (!IsPlayer)
	{
		UpdatePosition();
		result = UpdateBuffers(deviceContext, m_renderX, m_renderY);
		if (!result)
		{
			return false;
		}
	}
	else
	{
		result = UpdateBuffers(deviceContext, positionX, positionY);
		if (!result)
		{
			return false;
		}
	}

	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return true;
}


int BitmapClass::GetIndexCount()
{
	return m_indexCount;
}

int BitmapClass::GetpositionX()
{
	return m_renderX;
}

int BitmapClass::GetpositionY()
{
	return m_renderY;
}

int BitmapClass::GetspeedX()
{
	return m_speedX;
}

int BitmapClass::GetspeedY()
{
	return m_speedY;
}

int BitmapClass::GetprespeedX()
{
	return m_prespeedX;
}

int BitmapClass::GetprespeedY()
{
	return m_prespeedY;
}

int BitmapClass::Gettype()
{
	return m_type;
}

bool BitmapClass::GetPlayerChange()
{
	return m_PlayerChange;
}


ID3D11ShaderResourceView* BitmapClass::GetTexture()
{
	return m_Texture->GetTexture();
}


void BitmapClass::SetCheckcollision()
{
	for (int i = 0; i < 15; i++)
	{
		Checkcollision[i] = false;
	}
}

bool BitmapClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;


	// Initialize the previous rendering position to negative one.
	m_prevPosX = -1;
	m_prevPosY = -1;

	// Set the number of vertices in the vertex array.
	m_vertexCount = 6;

	// Set the number of indices in the index array.
	m_indexCount = m_vertexCount;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];

	// Create the index array.
	indices = new unsigned long[m_indexCount];

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// Load the index array with data.
	for (i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}


void BitmapClass::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}


bool BitmapClass::UpdateBuffers(ID3D11DeviceContext* deviceContent, int positionX, int positionY)
{
	float left, right, top, bottom;
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* dataPtr;
	HRESULT result;


	// If the position we are rendering this bitmap to hasn't changed then don't update the vertex buffer.
	if ((m_prevPosX == positionX) && (m_prevPosY == positionY))
	{
		return true;
	}

	// If the rendering location has changed then store the new position and update the vertex buffer.
	m_prevPosX = positionX;
	m_prevPosY = positionY;

	m_renderX = positionX;
	m_renderY = positionY;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];

	// Calculate the screen coordinates of the left side of the bitmap.
	left = (float)((m_screenWidth / 2) * -1) + (float)positionX;

	// Calculate the screen coordinates of the right side of the bitmap.
	right = left + (float)m_bitmapWidth;

	// Calculate the screen coordinates of the top of the bitmap.
	top = (float)(m_screenHeight / 2) - (float)positionY;

	// Calculate the screen coordinates of the bottom of the bitmap.
	bottom = top - (float)m_bitmapHeight;

	// Load the vertex array with data.
	// First triangle.
	vertices[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

	// Second triangle.
	vertices[3].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[4].position = XMFLOAT3(right, top, 0.0f);  // Top right.
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

	vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

	// Lock the vertex buffer.
	result = deviceContent->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(dataPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// Unlock the vertex buffer.
	deviceContent->Unmap(m_vertexBuffer, 0);

	// Release the pointer reference.
	dataPtr = 0;

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = 0;

	return true;
}


void BitmapClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}


bool BitmapClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;


	// Create and initialize the texture object.
	m_Texture = new TextureClass;

	result = m_Texture->Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	// Store the size in pixels that this bitmap should be rendered at.
	m_bitmapWidth = 32;
	m_bitmapHeight = 32;

	return true;
}


void BitmapClass::ReleaseTexture()
{
	// Release the texture object.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}


void BitmapClass::SetRenderLocation(int x, int y)
{
	m_renderX = x;
	m_renderY = y;
	return;
}

void BitmapClass::SetNotPlayer()
{
	IsPlayer = false;
}

void BitmapClass::SetPlayer()
{
	IsPlayer = true;
}

void BitmapClass::Settype(int newtype)
{
	m_type = newtype;
}

void BitmapClass::Setposition(int newpositionX, int newpositionY)
{
	m_renderX = newpositionX;
	m_renderY = newpositionY;
}

void BitmapClass::Setspeed(int newspeedX, int newspeedY)
{
	m_speedX = newspeedX;
	m_speedY = newspeedY;
}

void BitmapClass::Setprespeed(int newprespeedX, int newprespeedY)
{
	m_prespeedX = newprespeedX;
	m_prespeedY = newprespeedY;
}

void BitmapClass::SetPlayerChange(bool bt)
{
	m_PlayerChange = bt;
}

void BitmapClass::SetSize(int width, int height)
{
	m_bitmapWidth = width;
	m_bitmapHeight = height;
}


// 위치 업데이트 함수
void BitmapClass::UpdatePosition()
{
	m_renderX += m_speedX;
	m_renderY += m_speedY;

	if (m_renderX < 0)
	{
		m_renderX = 0;
		m_speedX = -m_speedX;
	}
	else if (m_renderX > m_screenWidth - m_bitmapWidth)
	{
		m_renderX = m_screenWidth - m_bitmapWidth;
		m_speedX = -m_speedX;
	}

	if (m_renderY < 0)
	{
		m_renderY = 0;
		m_speedY = -m_speedY;
	}
	else if (m_renderY > m_screenHeight - m_bitmapHeight)
	{
		m_renderY = m_screenHeight - m_bitmapHeight;
		m_speedY = -m_speedY;
	}
}


bool BitmapClass::CheckCollision(BitmapClass* other)
{
	float dx = (float)(m_renderX - other->m_renderX);
	float dy = (float)(m_renderY - other->m_renderY);
	float distance = sqrt(dx * dx + dy * dy);

	float collisionDistance = (m_bitmapWidth + other->m_bitmapWidth) / 2.0f;

	return (distance < collisionDistance);
}


bool BitmapClass::ResolveInteraction(BitmapClass* other)
{
	if (CheckCollision(other))
	{
		float dx = (float)(other->m_renderX - m_renderX);
		float dy = (float)(other->m_renderY - m_renderY);
		float distance = sqrt(dx * dx + dy * dy);

		if (distance == 0.0f) return false; // 같은 위치에 있으면 충돌 없음

		// 충돌 방향 단위 벡터 (법선 벡터)
		float nx = dx / distance;
		float ny = dy / distance;

		// 속도를 법선과 접선 방향으로 분해
		float v1n = m_speedX * nx + m_speedY * ny;
		float v2n = other->m_speedX * nx + other->m_speedY * ny;

		float v1t = -m_speedX * ny + m_speedY * nx;
		float v2t = -other->m_speedX * ny + other->m_speedY * nx;


		float v1n_after = v2n;
		float v2n_after = v1n;


		m_speedX = v1n_after * nx - v1t * ny;
		m_speedY = v1n_after * ny + v1t * nx;
		other->m_speedX = v2n_after * nx - v2t * ny;
		other->m_speedY = v2n_after * ny + v2t * nx;

		float overlap = ((m_bitmapWidth + other->m_bitmapWidth) / 2.0f) - distance;
		m_renderX -= overlap * nx * 0.5f;
		m_renderY -= overlap * ny * 0.5f;
		other->m_renderX += overlap * nx * 0.5f;
		other->m_renderY += overlap * ny * 0.5f;

		return true;
	}
	return false;
}

void BitmapClass::ChangeTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* newFilename)
{
	if (m_Texture)
	{
		// 기존 텍스처 객체를 삭제하지 않고 내용만 변경
		m_Texture->UpdateTexture(device, deviceContext, newFilename);
	}
}
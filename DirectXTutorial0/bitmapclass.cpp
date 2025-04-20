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
	// ������ �ӵ� ���� (-3 ~ 3 ������ �ӵ�)
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

	// ȭ�� ũ�� ����
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// ��Ʈ���� �������� ��ġ ����
	m_renderX = renderX;
	m_renderY = renderY;

	// ��Ʈ�� �簢��(quad)�� ������ �ε��� ���� �ʱ�ȭ
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// ��Ʈ�ʿ� ����� �ؽ�ó �ε�
	result = LoadTexture(device, deviceContext, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}


void BitmapClass::Shutdown()
{
	// ��Ʈ�� �ؽ�ó ����
	ReleaseTexture();

	// ���� �� �ε��� ���� ����
	ShutdownBuffers();

	return;
}

bool BitmapClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	bool result;

	if (!IsPlayer)
	{
		// �÷��̾ �ƴ϶�� ������ ��ġ�� ������ ��ġ ������Ʈ
		UpdatePosition();
		result = UpdateBuffers(deviceContext, m_renderX, m_renderY);
		if (!result)
		{
			return false;
		}
	}
	else
	{
		// �÷��̾��� ���޹��� ��ǥ�� ������ ��ġ ������Ʈ
		result = UpdateBuffers(deviceContext, positionX, positionY);
		if (!result)
		{
			return false;
		}
	}

	// ���� �� �ε��� ���۸� �׷��Ƚ� ���������ο� ���ε��Ͽ� ���� �غ�
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

	// ���� ������ ��ġ�� -1�� �ʱ�ȭ�մϴ�.
	m_prevPosX = -1;
	m_prevPosY = -1;

	// ���� �迭�� �� ���� ������ �����մϴ�.
	m_vertexCount = 6;

	// �ε��� �迭�� �� �ε��� ������ �����մϴ�. (���� ������ ����)
	m_indexCount = m_vertexCount;

	// ���� �迭�� �����մϴ�.
	vertices = new VertexType[m_vertexCount];

	// �ε��� �迭�� �����մϴ�.
	indices = new unsigned long[m_indexCount];

	// ���� �迭�� ó������ 0���� �ʱ�ȭ�մϴ�.
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// �ε��� �迭�� �����͸� ä��ϴ�.
	for (i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

	// ���� ���� ���ۿ� ���� ������ �����մϴ�.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// ���� �����͸� ����Ű�� ���긮�ҽ� ����ü�� �����մϴ�.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ���۸� �����մϴ�.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// �ε��� ���ۿ� ���� ������ �����մϴ�.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// �ε��� �����͸� ����Ű�� ���긮�ҽ� ����ü�� �����մϴ�.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �ε��� ���۸� �����մϴ�.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// ���� �� �ε��� ���۰� �����ǰ� �����Ͱ� ����Ǿ����Ƿ�, �迭�� �����մϴ�.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void BitmapClass::ShutdownBuffers()
{
	// �ε��� ���۸� �����մϴ�.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// ���� ���۸� �����մϴ�.
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

	// �������� ��Ʈ���� ��ġ�� ������ ���ٸ� ���� ���۸� ������ �ʿ䰡 �����ϴ�.
	if ((m_prevPosX == positionX) && (m_prevPosY == positionY))
	{
		return true;
	}

	// ������ ��ġ�� ����Ǿ��� ���, �� ��ġ�� �����ϰ� ���� ���۸� �����մϴ�.
	m_prevPosX = positionX;
	m_prevPosY = positionY;

	m_renderX = positionX;
	m_renderY = positionY;

	// ���� �迭�� �����մϴ�.
	vertices = new VertexType[m_vertexCount];

	// ��Ʈ���� ���� ȭ�� ��ǥ�� ����մϴ�.
	left = (float)((m_screenWidth / 2) * -1) + (float)positionX;

	// ��Ʈ���� ������ ȭ�� ��ǥ�� ����մϴ�.
	right = left + (float)m_bitmapWidth;

	// ��Ʈ���� ��� ȭ�� ��ǥ�� ����մϴ�.
	top = (float)(m_screenHeight / 2) - (float)positionY;

	// ��Ʈ���� �ϴ� ȭ�� ��ǥ�� ����մϴ�.
	bottom = top - (float)m_bitmapHeight;

	// ���� �迭�� ������ ä���
	// ù ��° �ﰢ��.
	vertices[0].position = XMFLOAT3(left, top, 0.0f);  // �»��
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // ���ϴ�
	vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // ���ϴ�
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

	// �� ��° �ﰢ��.
	vertices[3].position = XMFLOAT3(left, top, 0.0f);  // �»��
	vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[4].position = XMFLOAT3(right, top, 0.0f);  // ����
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

	vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // ���ϴ�
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

	// ���� ���۸� ��޴ϴ�.
	result = deviceContent->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// ���� ������ �����͸� ����Ű�� �����͸� �����ɴϴ�.
	dataPtr = (VertexType*)mappedResource.pData;

	// ���� ���ۿ� �����͸� �����մϴ�.
	memcpy(dataPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// ���� ������ ����� �����մϴ�.
	deviceContent->Unmap(m_vertexBuffer, 0);

	// ������ ���� ����.
	dataPtr = 0;

	// ���� �迭�� �� �̻� �ʿ� �����Ƿ� �����մϴ�.
	delete[] vertices;
	vertices = 0;

	return true;
}


void BitmapClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// ���� ������ ��Ʈ���̵�� ������ ����.
	stride = sizeof(VertexType);
	offset = 0;

	// �Է� ������(Input Assembler)�� ���� ���۸� �����Ͽ� ������ �����ϵ��� Ȱ��ȭ.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// �Է� �����⿡ �ε��� ���۸� �����Ͽ� ������ �����ϵ��� Ȱ��ȭ.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// �� ���� ���ۿ��� � ������ ����(������Ƽ��)�� �׸� ������ ����. ���⼭�� �ﰢ�� ����Ʈ.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}


bool BitmapClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;

	// �ؽ�ó ��ü�� �����ϰ� �ʱ�ȭ.
	m_Texture = new TextureClass;

	result = m_Texture->Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	// �� ��Ʈ���� �������� �ȼ� ���� ũ�� ����.
	m_bitmapWidth = 32;
	m_bitmapHeight = 32;

	return true;
}


void BitmapClass::ReleaseTexture()
{
	// �ؽ�ó ��ü ����.
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


// ��ġ ������Ʈ �Լ�
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

		if (distance == 0.0f) return false; // ���� ��ġ�� ������ �浹 ����

		// �浹 ���� ���� ���� (���� ����)
		float nx = dx / distance;
		float ny = dy / distance;

		// �ӵ��� ������ ���� �������� ����
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
		// ���� �ؽ�ó ��ü�� �������� �ʰ� ���븸 ����
		m_Texture->UpdateTexture(device, deviceContext, newFilename);
	}
}
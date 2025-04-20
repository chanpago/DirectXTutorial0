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

	// 화면 크기 저장
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// 비트맵을 렌더링할 위치 저장
	m_renderX = renderX;
	m_renderY = renderY;

	// 비트맵 사각형(quad)의 정점과 인덱스 버퍼 초기화
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// 비트맵에 사용할 텍스처 로드
	result = LoadTexture(device, deviceContext, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}


void BitmapClass::Shutdown()
{
	// 비트맵 텍스처 해제
	ReleaseTexture();

	// 정점 및 인덱스 버퍼 해제
	ShutdownBuffers();

	return;
}

bool BitmapClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	bool result;

	if (!IsPlayer)
	{
		// 플레이어가 아니라면 고정된 위치로 렌더링 위치 업데이트
		UpdatePosition();
		result = UpdateBuffers(deviceContext, m_renderX, m_renderY);
		if (!result)
		{
			return false;
		}
	}
	else
	{
		// 플레이어라면 전달받은 좌표로 렌더링 위치 업데이트
		result = UpdateBuffers(deviceContext, positionX, positionY);
		if (!result)
		{
			return false;
		}
	}

	// 정점 및 인덱스 버퍼를 그래픽스 파이프라인에 바인딩하여 렌더 준비
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

	// 이전 렌더링 위치를 -1로 초기화합니다.
	m_prevPosX = -1;
	m_prevPosY = -1;

	// 정점 배열에 들어갈 정점 개수를 설정합니다.
	m_vertexCount = 6;

	// 인덱스 배열에 들어갈 인덱스 개수를 설정합니다. (정점 개수와 동일)
	m_indexCount = m_vertexCount;

	// 정점 배열을 생성합니다.
	vertices = new VertexType[m_vertexCount];

	// 인덱스 배열을 생성합니다.
	indices = new unsigned long[m_indexCount];

	// 정점 배열을 처음에는 0으로 초기화합니다.
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// 인덱스 배열에 데이터를 채웁니다.
	for (i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

	// 동적 정점 버퍼에 대한 설명을 설정합니다.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 정점 데이터를 가리키는 서브리소스 구조체를 설정합니다.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 정점 버퍼를 생성합니다.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 인덱스 버퍼에 대한 설명을 설정합니다.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 인덱스 데이터를 가리키는 서브리소스 구조체를 설정합니다.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 생성합니다.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 정점 및 인덱스 버퍼가 생성되고 데이터가 복사되었으므로, 배열은 해제합니다.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void BitmapClass::ShutdownBuffers()
{
	// 인덱스 버퍼를 해제합니다.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// 정점 버퍼를 해제합니다.
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

	// 렌더링할 비트맵의 위치가 이전과 같다면 정점 버퍼를 갱신할 필요가 없습니다.
	if ((m_prevPosX == positionX) && (m_prevPosY == positionY))
	{
		return true;
	}

	// 렌더링 위치가 변경되었을 경우, 새 위치를 저장하고 정점 버퍼를 갱신합니다.
	m_prevPosX = positionX;
	m_prevPosY = positionY;

	m_renderX = positionX;
	m_renderY = positionY;

	// 정점 배열을 생성합니다.
	vertices = new VertexType[m_vertexCount];

	// 비트맵의 왼쪽 화면 좌표를 계산합니다.
	left = (float)((m_screenWidth / 2) * -1) + (float)positionX;

	// 비트맵의 오른쪽 화면 좌표를 계산합니다.
	right = left + (float)m_bitmapWidth;

	// 비트맵의 상단 화면 좌표를 계산합니다.
	top = (float)(m_screenHeight / 2) - (float)positionY;

	// 비트맵의 하단 화면 좌표를 계산합니다.
	bottom = top - (float)m_bitmapHeight;

	// 정점 배열에 데이터 채우기
	// 첫 번째 삼각형.
	vertices[0].position = XMFLOAT3(left, top, 0.0f);  // 좌상단
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // 우하단
	vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // 좌하단
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

	// 두 번째 삼각형.
	vertices[3].position = XMFLOAT3(left, top, 0.0f);  // 좌상단
	vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[4].position = XMFLOAT3(right, top, 0.0f);  // 우상단
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

	vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // 우하단
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

	// 정점 버퍼를 잠급니다.
	result = deviceContent->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// 정점 버퍼의 데이터를 가리키는 포인터를 가져옵니다.
	dataPtr = (VertexType*)mappedResource.pData;

	// 정점 버퍼에 데이터를 복사합니다.
	memcpy(dataPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// 정점 버퍼의 잠금을 해제합니다.
	deviceContent->Unmap(m_vertexBuffer, 0);

	// 포인터 참조 해제.
	dataPtr = 0;

	// 정점 배열은 더 이상 필요 없으므로 해제합니다.
	delete[] vertices;
	vertices = 0;

	return true;
}


void BitmapClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// 정점 버퍼의 스트라이드와 오프셋 설정.
	stride = sizeof(VertexType);
	offset = 0;

	// 입력 조립기(Input Assembler)에 정점 버퍼를 설정하여 렌더링 가능하도록 활성화.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// 입력 조립기에 인덱스 버퍼를 설정하여 렌더링 가능하도록 활성화.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 이 정점 버퍼에서 어떤 종류의 도형(프리미티브)을 그릴 것인지 설정. 여기서는 삼각형 리스트.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}


bool BitmapClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;

	// 텍스처 객체를 생성하고 초기화.
	m_Texture = new TextureClass;

	result = m_Texture->Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	// 이 비트맵이 렌더링될 픽셀 단위 크기 저장.
	m_bitmapWidth = 32;
	m_bitmapHeight = 32;

	return true;
}


void BitmapClass::ReleaseTexture()
{
	// 텍스처 객체 해제.
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
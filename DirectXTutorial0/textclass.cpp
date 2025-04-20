///////////////////////////////////////////////////////////////////////////////
// Filename: textclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "textclass.h"


TextClass::TextClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}


TextClass::TextClass(const TextClass& other)
{
}


TextClass::~TextClass()
{
}


bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int screenWidth, int screenHeight, int maxLength, FontClass* Font, char* text,
	int positionX, int positionY, float red, float green, float blue)
{
	bool result;

	// 화면의 너비와 높이를 저장한다.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// 출력 가능한 최대 문자열 길이를 저장한다.
	m_maxLength = maxLength;

	// 문장을 초기화한다.
	result = InitializeBuffers(device, deviceContext, Font, text, positionX, positionY, red, green, blue);
	if (!result)
	{
		return false;
	}

	return true;
}

void TextClass::Shutdown()
{
	// 정점 버퍼와 인덱스 버퍼를 해제한다.
	ShutdownBuffers();

	return;
}

void TextClass::Render(ID3D11DeviceContext* deviceContext)
{
	// 텍스트를 그리기 위해 정점/인덱스 버퍼를 그래픽 파이프라인에 바인딩한다.
	RenderBuffers(deviceContext);

	return;
}


int TextClass::GetIndexCount()
{
	return m_indexCount;
}


bool TextClass::InitializeBuffers(ID3D11Device* device, ID3D11DeviceContext* deviceContext, FontClass* Font, char* text, int positionX, int positionY, float red, float green, float blue)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	// 정점 개수 및 인덱스 개수를 설정한다.
	m_vertexCount = 6 * m_maxLength;  // 각 문자당 6개의 정점(2개의 삼각형)
	m_indexCount = m_vertexCount;

	// 정점 배열을 생성한다.
	vertices = new VertexType[m_vertexCount];

	// 인덱스 배열을 생성한다.
	indices = new unsigned long[m_indexCount];

	// 정점 배열을 0으로 초기화한다.
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// 인덱스 배열을 초기화한다.
	for (i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

	// 동적 정점 버퍼의 설명을 설정한다.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 서브리소스 구조체에 정점 데이터의 포인터를 넘겨준다.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 정점 버퍼를 생성한다.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 정적인 인덱스 버퍼의 설명을 설정한다.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 서브리소스 구조체에 인덱스 데이터의 포인터를 넘겨준다.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 생성한다.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 더 이상 필요 없는 정점 배열을 해제한다.
	delete[] vertices;
	vertices = 0;

	// 더 이상 필요 없는 인덱스 배열을 해제한다.
	delete[] indices;
	indices = 0;

	// 텍스트 데이터를 정점 버퍼에 추가한다.
	result = UpdateText(deviceContext, Font, text, positionX, positionY, red, green, blue);
	if (!result)
	{
		return false;
	}

	return true;
}



void TextClass::ShutdownBuffers()
{
	// 인덱스 버퍼를 해제한다.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// 정점 버퍼를 해제한다.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}



bool TextClass::UpdateText(ID3D11DeviceContext* deviceContext, FontClass* Font, char* text, int positionX, int positionY, float red, float green, float blue)
{
	int numLetters;
	VertexType* vertices;
	float drawX, drawY;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;


	// 문장의 색상을 저장한다.
	m_pixelColor = XMFLOAT4(red, green, blue, 1.0f);

	// 문장에 포함된 문자의 수를 가져온다.
	numLetters = (int)strlen(text);

	// 버퍼 오버플로우 방지를 위해 최대 길이를 초과하는지 검사한다.
	if (numLetters > m_maxLength)
	{
		return false;
	}

	// 정점 배열을 생성한다.
	vertices = new VertexType[m_vertexCount];

	// 정점 배열을 0으로 초기화한다.
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// 화면 상에서 문장을 그리기 시작할 X, Y 픽셀 위치를 계산한다.
	drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
	drawY = (float)((m_screenHeight / 2) - positionY);

	// Font 클래스를 사용해 문장 텍스트와 위치를 기반으로 정점 배열을 생성한다.
	Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	// 정점 버퍼에 데이터를 쓰기 위해 락을 건다.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// 정점 버퍼의 데이터 포인터를 얻는다.
	verticesPtr = (VertexType*)mappedResource.pData;

	// 생성한 정점 데이터를 정점 버퍼에 복사한다.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// 정점 버퍼의 잠금을 해제한다.
	deviceContext->Unmap(m_vertexBuffer, 0);

	// 더 이상 필요 없는 정점 배열을 해제한다.
	delete[] vertices;
	vertices = 0;

	return true;
}


void TextClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride, offset;


	// 정점 버퍼의 스트라이드(한 정점의 크기)와 오프셋을 설정한다.
	stride = sizeof(VertexType);
	offset = 0;

	// 정점 버퍼를 Input Assembler에 바인딩하여 렌더링할 수 있도록 설정한다.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// 인덱스 버퍼를 Input Assembler에 바인딩하여 렌더링할 수 있도록 설정한다.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 이 정점 버퍼에서 렌더링할 기본 도형 타입을 설정한다. 여기서는 삼각형 리스트로 설정한다.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}


XMFLOAT4 TextClass::GetPixelColor()
{
	return m_pixelColor;
}
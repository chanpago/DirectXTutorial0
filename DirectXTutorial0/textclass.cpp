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

	// ȭ���� �ʺ�� ���̸� �����Ѵ�.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// ��� ������ �ִ� ���ڿ� ���̸� �����Ѵ�.
	m_maxLength = maxLength;

	// ������ �ʱ�ȭ�Ѵ�.
	result = InitializeBuffers(device, deviceContext, Font, text, positionX, positionY, red, green, blue);
	if (!result)
	{
		return false;
	}

	return true;
}

void TextClass::Shutdown()
{
	// ���� ���ۿ� �ε��� ���۸� �����Ѵ�.
	ShutdownBuffers();

	return;
}

void TextClass::Render(ID3D11DeviceContext* deviceContext)
{
	// �ؽ�Ʈ�� �׸��� ���� ����/�ε��� ���۸� �׷��� ���������ο� ���ε��Ѵ�.
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

	// ���� ���� �� �ε��� ������ �����Ѵ�.
	m_vertexCount = 6 * m_maxLength;  // �� ���ڴ� 6���� ����(2���� �ﰢ��)
	m_indexCount = m_vertexCount;

	// ���� �迭�� �����Ѵ�.
	vertices = new VertexType[m_vertexCount];

	// �ε��� �迭�� �����Ѵ�.
	indices = new unsigned long[m_indexCount];

	// ���� �迭�� 0���� �ʱ�ȭ�Ѵ�.
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// �ε��� �迭�� �ʱ�ȭ�Ѵ�.
	for (i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

	// ���� ���� ������ ������ �����Ѵ�.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// ���긮�ҽ� ����ü�� ���� �������� �����͸� �Ѱ��ش�.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ���۸� �����Ѵ�.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// ������ �ε��� ������ ������ �����Ѵ�.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// ���긮�ҽ� ����ü�� �ε��� �������� �����͸� �Ѱ��ش�.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �ε��� ���۸� �����Ѵ�.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// �� �̻� �ʿ� ���� ���� �迭�� �����Ѵ�.
	delete[] vertices;
	vertices = 0;

	// �� �̻� �ʿ� ���� �ε��� �迭�� �����Ѵ�.
	delete[] indices;
	indices = 0;

	// �ؽ�Ʈ �����͸� ���� ���ۿ� �߰��Ѵ�.
	result = UpdateText(deviceContext, Font, text, positionX, positionY, red, green, blue);
	if (!result)
	{
		return false;
	}

	return true;
}



void TextClass::ShutdownBuffers()
{
	// �ε��� ���۸� �����Ѵ�.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// ���� ���۸� �����Ѵ�.
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


	// ������ ������ �����Ѵ�.
	m_pixelColor = XMFLOAT4(red, green, blue, 1.0f);

	// ���忡 ���Ե� ������ ���� �����´�.
	numLetters = (int)strlen(text);

	// ���� �����÷ο� ������ ���� �ִ� ���̸� �ʰ��ϴ��� �˻��Ѵ�.
	if (numLetters > m_maxLength)
	{
		return false;
	}

	// ���� �迭�� �����Ѵ�.
	vertices = new VertexType[m_vertexCount];

	// ���� �迭�� 0���� �ʱ�ȭ�Ѵ�.
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// ȭ�� �󿡼� ������ �׸��� ������ X, Y �ȼ� ��ġ�� ����Ѵ�.
	drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
	drawY = (float)((m_screenHeight / 2) - positionY);

	// Font Ŭ������ ����� ���� �ؽ�Ʈ�� ��ġ�� ������� ���� �迭�� �����Ѵ�.
	Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	// ���� ���ۿ� �����͸� ���� ���� ���� �Ǵ�.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// ���� ������ ������ �����͸� ��´�.
	verticesPtr = (VertexType*)mappedResource.pData;

	// ������ ���� �����͸� ���� ���ۿ� �����Ѵ�.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// ���� ������ ����� �����Ѵ�.
	deviceContext->Unmap(m_vertexBuffer, 0);

	// �� �̻� �ʿ� ���� ���� �迭�� �����Ѵ�.
	delete[] vertices;
	vertices = 0;

	return true;
}


void TextClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride, offset;


	// ���� ������ ��Ʈ���̵�(�� ������ ũ��)�� �������� �����Ѵ�.
	stride = sizeof(VertexType);
	offset = 0;

	// ���� ���۸� Input Assembler�� ���ε��Ͽ� �������� �� �ֵ��� �����Ѵ�.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// �ε��� ���۸� Input Assembler�� ���ε��Ͽ� �������� �� �ֵ��� �����Ѵ�.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// �� ���� ���ۿ��� �������� �⺻ ���� Ÿ���� �����Ѵ�. ���⼭�� �ﰢ�� ����Ʈ�� �����Ѵ�.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}


XMFLOAT4 TextClass::GetPixelColor()
{
	return m_pixelColor;
}
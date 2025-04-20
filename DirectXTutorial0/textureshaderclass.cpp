////////////////////////////////////////////////////////////////////////////////
// Filename: textureshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "textureshaderclass.h"


TextureShaderClass::TextureShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_sampleState = 0;
}


TextureShaderClass::TextureShaderClass(const TextureShaderClass& other)
{
}


TextureShaderClass::~TextureShaderClass()
{
}


bool TextureShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	wchar_t vsFilename[128];
	wchar_t psFilename[128];
	int error;

	// ���� ���̴� ���� �̸��� �����Ѵ�.
	error = wcscpy_s(vsFilename, 128, L"../Engine/texture.vs");
	if (error != 0)
	{
		return false;
	}

	// �ȼ� ���̴� ���� �̸��� �����Ѵ�.
	error = wcscpy_s(psFilename, 128, L"../Engine/texture.ps");
	if (error != 0)
	{
		return false;
	}

	// ���� ���̴��� �ȼ� ���̴��� �ʱ�ȭ�Ѵ�.
	result = InitializeShader(device, hwnd, vsFilename, psFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void TextureShaderClass::Shutdown()
{
	// ���� ���̴��� �ȼ� ���̴� �� ���õ� ��ü���� �����Ѵ�.
	ShutdownShader();

	return;
}


bool TextureShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	bool result;

	// �������� ����� ���̴� �Ķ����(���, �ؽ�ó ��)�� �����Ѵ�.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture);
	if (!result)
	{
		return false;
	}

	// �غ�� ���۵��� ���̴��� �������Ѵ�.
	RenderShader(deviceContext, indexCount);

	return true;
}



bool TextureShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	// �� �Լ����� ����� �����͵��� null�� �ʱ�ȭ.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// ���ؽ� ���̴� �ڵ带 �������Ѵ�.
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// ���̴� �����Ͽ� �������� ��� ���� �޽����� �ִٸ� ���.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// ���� �޽����� ���ٸ� ���̴� ������ ã�� �� ������ �˸�.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// �ȼ� ���̴� �ڵ带 �������Ѵ�.
	result = D3DCompileFromFile(psFilename, NULL, NULL, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// ���̴� �����Ͽ� �������� ��� ���� �޽����� �ִٸ� ���.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// ���� �޽����� ���ٸ� ���̴� ������ ã�� �� ������ �˸�.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// ���ۿ��� ���ؽ� ���̴��� �����Ѵ�.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// ���ۿ��� �ȼ� ���̴��� �����Ѵ�.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// ���ؽ� �Է� ���̾ƿ��� �����Ѵ�.
	// �� ������ ModelClass �� ���̴����� ������ VertexType ������ ��ġ�ؾ� �Ѵ�.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// ���̾ƿ� ����� ������ ����Ѵ�.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// ���ؽ� �Է� ���̾ƿ��� �����Ѵ�.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	// �� �̻� �ʿ� �����Ƿ� ���ؽ� ���̴� �� �ȼ� ���̴� ���۸� �����Ѵ�.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// ���ؽ� ���̴� ���� ��� ��� ���ۿ� ���� ������ �����Ѵ�.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// �� Ŭ�������� ���ؽ� ���̴� ��� ���ۿ� ������ �� �ֵ��� ��� ���� �����͸� �����Ѵ�.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// �ؽ�ó ���÷� ���¿� ���� ������ �����Ѵ�.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// �ؽ�ó ���÷� ���¸� �����Ѵ�.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}



void TextureShaderClass::ShutdownShader()
{
	// ���÷� ���� ����
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	// ��� ��� ���� ����
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// �Է� ���̾ƿ� ����
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// �ȼ� ���̴� ����
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// ���ؽ� ���̴� ����
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}

void TextureShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;

	// ���� �޽��� �ؽ�Ʈ ���ۿ� ���� ������ ��������
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// �޽����� ���� ��������
	bufferSize = errorMessage->GetBufferSize();

	// ���� �޽����� ������ ���� ����
	fout.open("shader-error.txt");

	// ���� �޽����� ���Ͽ� ���
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// ���� �ݱ�
	fout.close();

	// ���� �޽��� ����
	errorMessage->Release();
	errorMessage = 0;

	// ����ڿ��� �޽��� �ڽ��� ������ ���� Ȯ�� �˸�
	MessageBox(hwnd, L"���̴� ������ ���� �߻�. shader-error.txt ������ Ȯ���ϼ���.", shaderFilename, MB_OK);

	return;
}



bool TextureShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	// ���̴����� ����� �� �ֵ��� ����� ��ġ(transpose)
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// ��� ���۸� ��� -> �� �� �ֵ��� ����
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// ��� ������ �����Ϳ� ������ ������ ȹ��
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// ��� ������ ����
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// ��� ������ ��� ����
	deviceContext->Unmap(m_matrixBuffer, 0);

	// ���ؽ� ���̴��� ��� ���� ����
	bufferNumber = 0;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// �ȼ� ���̴��� �ؽ�ó ���ҽ� ����
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void TextureShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// �Է� ���̾ƿ� ���� (���ؽ� ���� ����)
	deviceContext->IASetInputLayout(m_layout);

	// ���ؽ� ���̴�, �ȼ� ���̴� ����
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// �ȼ� ���̴��� ���÷� ���� ����
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// �ε����� �̿��� �ﰢ�� ������ ����
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

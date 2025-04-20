////////////////////////////////////////////////////////////////////////////////
// Filename: fontshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "fontshaderclass.h"


FontShaderClass::FontShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_sampleState = 0;
	m_pixelBuffer = 0;
}


FontShaderClass::FontShaderClass(const FontShaderClass& other)
{
}


FontShaderClass::~FontShaderClass()
{
}


bool FontShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	wchar_t vsFilename[128];
	wchar_t psFilename[128];
	int error;

	// ���ؽ� ���̴� ���� �̸� ����.
	error = wcscpy_s(vsFilename, 128, L"../Engine/font.vs");
	if (error != 0)
	{
		return false;
	}

	// �ȼ� ���̴� ���� �̸� ����.
	error = wcscpy_s(psFilename, 128, L"../Engine/font.ps");
	if (error != 0)
	{
		return false;
	}

	// ���ؽ� ���̴��� �ȼ� ���̴��� �ʱ�ȭ.
	result = InitializeShader(device, hwnd, vsFilename, psFilename);
	if (!result)
	{
		return false;
	}

	return true;
}


void FontShaderClass::Shutdown()
{
	// ���ؽ� ���̴��� �ȼ� ���̴� �� ���� ��ü���� ����.
	ShutdownShader();

	return;
}


bool FontShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 pixelColor)
{
	bool result;

	// �������� ����� ���̴� �Ķ���� ����.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, pixelColor);
	if (!result)
	{
		return false;
	}

	// ������ ���۵��� ���̴��� ������.
	RenderShader(deviceContext, indexCount);

	return true;
}


bool FontShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC pixelBufferDesc;

	// �� �Լ����� ����� �����͵��� nullptr�� �ʱ�ȭ.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// ���ؽ� ���̴� �ڵ带 ������.
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "FontVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// ���̴� �����Ͽ� �����ϸ�, ���� �޽����� ����Ǿ����� Ȯ��.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// ���� �޽����� ������ ���̴� ������ ã�� ���� ��.
		else
		{
			MessageBox(hwnd, vsFilename, L"���̴� ������ ã�� �� �����ϴ�.", MB_OK);
		}
		return false;
	}

	// �ȼ� ���̴� �ڵ带 ������.
	result = D3DCompileFromFile(psFilename, NULL, NULL, "FontPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// ���̴� ������ ���� �� ���� �޽��� Ȯ��.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// ���� �޽����� ������ ���� ��ü�� ã�� �� ���� ���.
		else
		{
			MessageBox(hwnd, psFilename, L"���̴� ������ ã�� �� �����ϴ�.", MB_OK);
		}
		return false;
	}

	// ���۷κ��� ���ؽ� ���̴� ����.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// ���۷κ��� �ȼ� ���̴� ����.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// ���ؽ� �Է� ���̾ƿ� ���� ����.
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

	// �Է� ���̾ƿ��� ��� ���� ����.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// ���ؽ� �Է� ���̾ƿ� ����.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	// �� �̻� �ʿ����� �����Ƿ� ���̴� ���� ����.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// ���ؽ� ���̴����� ����� ��� ��� ������ ���� ���� ���� ����.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// ���ؽ� ���̴����� ��� ���۸� ������ �� �ֵ��� ���� ����.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// �ؽ�ó ���÷� ���� ���� ����.
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

	// �ؽ�ó ���÷� ���� ����.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	// �ȼ� ���̴����� ����� �ȼ� ��� ������ ���� ���� ���� ����.
	pixelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pixelBufferDesc.ByteWidth = sizeof(PixelBufferType);
	pixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pixelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pixelBufferDesc.MiscFlags = 0;
	pixelBufferDesc.StructureByteStride = 0;

	// �ȼ� ���̴� ��� ���� ������ ���� ���� ����.
	result = device->CreateBuffer(&pixelBufferDesc, NULL, &m_pixelBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}


void FontShaderClass::ShutdownShader()
{
	// �ȼ� ���̴� ��� ���� ����
	if (m_pixelBuffer)
	{
		m_pixelBuffer->Release();
		m_pixelBuffer = 0;
	}

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



void FontShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;

	// ���� �޽��� �ؽ�Ʈ ���ۿ� ���� �����͸� ������
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// �޽����� ���̸� ������
	bufferSize = errorMessage->GetBufferSize();

	// ���� �޽����� ����� ������ ��
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

	// ���̴� ������ ������ �˸��� �޽��� �ڽ��� ȭ�鿡 ���
	MessageBox(hwnd, L"���̴� ������ ����. shader-error.txt ������ Ȯ���ϼ���.", shaderFilename, MB_OK);

	return;
}


bool FontShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 pixelColor)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;
	PixelBufferType* dataPtr2;

	// ���̴����� ����� �� �ֵ��� ��ĵ��� ��ġ(transpose)�Ѵ�.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// ��� ���۸� �ᰡ�� �����͸� �� �� �ְ� �����.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// ��� ���� �����Ϳ� ������ �����͸� �����´�.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// ��� �����͸� ��� ���ۿ� �����Ѵ�.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// ��� ������ ����� �����Ѵ�.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// ���� ���̴����� ����� ��� ���� ���� ��ȣ�� �����Ѵ�.
	bufferNumber = 0;

	// ���� ���̴��� ��� ���۸� �����Ѵ�.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// �ȼ� ���̴��� ����� �ؽ�ó ���ҽ��� �����Ѵ�.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	// �ȼ� ��� ���۸� �ᰡ�� �����͸� �� �� �ְ� �����.
	result = deviceContext->Map(m_pixelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// �ȼ� ���̴� ��� ���ۿ� ������ �����͸� �����´�.
	dataPtr2 = (PixelBufferType*)mappedResource.pData;

	// �ȼ� ������ ��� ���ۿ� �����Ѵ�.
	dataPtr2->pixelColor = pixelColor;

	// �ȼ� ��� ������ ����� �����Ѵ�.
	deviceContext->Unmap(m_pixelBuffer, 0);

	// �ȼ� ���̴����� ����� ��� ���� ���� ��ȣ�� �����Ѵ�.
	bufferNumber = 0;

	// �ȼ� ���̴��� ��� ���۸� �����Ѵ�.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_pixelBuffer);

	return true;
}


void FontShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// ���� �Է� ���̾ƿ��� �����Ѵ�.
	deviceContext->IASetInputLayout(m_layout);

	// �ﰢ���� �������� �� ����� ���� �� �ȼ� ���̴��� �����Ѵ�.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// �ȼ� ���̴����� ����� ���÷� ���¸� �����Ѵ�.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// �ﰢ���� �������Ѵ�.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

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

	// 버텍스 셰이더 파일 이름 설정.
	error = wcscpy_s(vsFilename, 128, L"../Engine/font.vs");
	if (error != 0)
	{
		return false;
	}

	// 픽셀 셰이더 파일 이름 설정.
	error = wcscpy_s(psFilename, 128, L"../Engine/font.ps");
	if (error != 0)
	{
		return false;
	}

	// 버텍스 셰이더와 픽셀 셰이더를 초기화.
	result = InitializeShader(device, hwnd, vsFilename, psFilename);
	if (!result)
	{
		return false;
	}

	return true;
}


void FontShaderClass::Shutdown()
{
	// 버텍스 셰이더와 픽셀 셰이더 및 관련 객체들을 해제.
	ShutdownShader();

	return;
}


bool FontShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 pixelColor)
{
	bool result;

	// 렌더링에 사용할 셰이더 파라미터 설정.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, pixelColor);
	if (!result)
	{
		return false;
	}

	// 설정된 버퍼들을 셰이더로 렌더링.
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

	// 이 함수에서 사용할 포인터들을 nullptr로 초기화.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// 버텍스 셰이더 코드를 컴파일.
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "FontVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// 셰이더 컴파일에 실패하면, 오류 메시지가 저장되었는지 확인.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// 오류 메시지가 없으면 셰이더 파일을 찾지 못한 것.
		else
		{
			MessageBox(hwnd, vsFilename, L"셰이더 파일을 찾을 수 없습니다.", MB_OK);
		}
		return false;
	}

	// 픽셀 셰이더 코드를 컴파일.
	result = D3DCompileFromFile(psFilename, NULL, NULL, "FontPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// 셰이더 컴파일 실패 시 오류 메시지 확인.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// 오류 메시지가 없으면 파일 자체를 찾을 수 없는 경우.
		else
		{
			MessageBox(hwnd, psFilename, L"셰이더 파일을 찾을 수 없습니다.", MB_OK);
		}
		return false;
	}

	// 버퍼로부터 버텍스 셰이더 생성.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// 버퍼로부터 픽셀 셰이더 생성.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// 버텍스 입력 레이아웃 설명 생성.
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

	// 입력 레이아웃의 요소 개수 구함.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// 버텍스 입력 레이아웃 생성.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	// 더 이상 필요하지 않으므로 셰이더 버퍼 해제.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// 버텍스 셰이더에서 사용할 행렬 상수 버퍼의 동적 버퍼 설명 생성.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 버텍스 셰이더에서 상수 버퍼를 접근할 수 있도록 버퍼 생성.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 텍스처 샘플러 상태 설명 생성.
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

	// 텍스처 샘플러 상태 생성.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	// 픽셀 셰이더에서 사용할 픽셀 상수 버퍼의 동적 버퍼 설명 생성.
	pixelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pixelBufferDesc.ByteWidth = sizeof(PixelBufferType);
	pixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pixelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pixelBufferDesc.MiscFlags = 0;
	pixelBufferDesc.StructureByteStride = 0;

	// 픽셀 셰이더 상수 버퍼 접근을 위한 버퍼 생성.
	result = device->CreateBuffer(&pixelBufferDesc, NULL, &m_pixelBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}


void FontShaderClass::ShutdownShader()
{
	// 픽셀 셰이더 상수 버퍼 해제
	if (m_pixelBuffer)
	{
		m_pixelBuffer->Release();
		m_pixelBuffer = 0;
	}

	// 샘플러 상태 해제
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	// 행렬 상수 버퍼 해제
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// 입력 레이아웃 해제
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// 픽셀 셰이더 해제
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// 버텍스 셰이더 해제
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

	// 에러 메시지 텍스트 버퍼에 대한 포인터를 가져옴
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// 메시지의 길이를 가져옴
	bufferSize = errorMessage->GetBufferSize();

	// 에러 메시지를 기록할 파일을 엶
	fout.open("shader-error.txt");

	// 에러 메시지를 파일에 출력
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// 파일 닫기
	fout.close();

	// 에러 메시지 해제
	errorMessage->Release();
	errorMessage = 0;

	// 셰이더 컴파일 오류를 알리는 메시지 박스를 화면에 출력
	MessageBox(hwnd, L"셰이더 컴파일 에러. shader-error.txt 파일을 확인하세요.", shaderFilename, MB_OK);

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

	// 셰이더에서 사용할 수 있도록 행렬들을 전치(transpose)한다.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// 상수 버퍼를 잠가서 데이터를 쓸 수 있게 만든다.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// 상수 버퍼 데이터에 접근할 포인터를 가져온다.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// 행렬 데이터를 상수 버퍼에 복사한다.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// 상수 버퍼의 잠금을 해제한다.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// 정점 셰이더에서 사용할 상수 버퍼 슬롯 번호를 설정한다.
	bufferNumber = 0;

	// 정점 셰이더에 상수 버퍼를 설정한다.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// 픽셀 셰이더에 사용할 텍스처 리소스를 설정한다.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	// 픽셀 상수 버퍼를 잠가서 데이터를 쓸 수 있게 만든다.
	result = deviceContext->Map(m_pixelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// 픽셀 셰이더 상수 버퍼에 접근할 포인터를 가져온다.
	dataPtr2 = (PixelBufferType*)mappedResource.pData;

	// 픽셀 색상을 상수 버퍼에 복사한다.
	dataPtr2->pixelColor = pixelColor;

	// 픽셀 상수 버퍼의 잠금을 해제한다.
	deviceContext->Unmap(m_pixelBuffer, 0);

	// 픽셀 셰이더에서 사용할 상수 버퍼 슬롯 번호를 설정한다.
	bufferNumber = 0;

	// 픽셀 셰이더에 상수 버퍼를 설정한다.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_pixelBuffer);

	return true;
}


void FontShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// 정점 입력 레이아웃을 설정한다.
	deviceContext->IASetInputLayout(m_layout);

	// 삼각형을 렌더링할 때 사용할 정점 및 픽셀 셰이더를 설정한다.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// 픽셀 셰이더에서 사용할 샘플러 상태를 설정한다.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// 삼각형을 렌더링한다.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

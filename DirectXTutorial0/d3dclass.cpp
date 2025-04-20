////////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "d3dclass.h"


D3DClass::D3DClass()
{
	m_swapChain = 0;
	m_device = 0;
	m_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
	m_depthDisabledStencilState = 0;

	m_alphaEnableBlendingState = 0;
	m_alphaDisableBlendingState = 0;

}


D3DClass::D3DClass(const D3DClass& other)
{
}


D3DClass::~D3DClass()
{
}


bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator;
	unsigned long long stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	float fieldOfView, screenAspect;
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;

	D3D11_BLEND_DESC blendStateDescription;


	// 수직 동기화 설정 저장
	m_vsync_enabled = vsync;

	// DirectX 그래픽 인터페이스 팩토리 생성
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result)) {
		return false;
	}

	// 기본 그래픽 어댑터(비디오 카드) 가져오기
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result)) {
		return false;
	}

	// 어댑터의 출력(모니터) 열거
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result)) {
		return false;
	}

	// 지정된 포맷(DXGI_FORMAT_R8G8B8A8_UNORM)을 지원하는 모드 수 얻기
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result)) {
		return false;
	}

	// 모든 가능한 디스플레이 모드를 저장할 리스트 생성
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList) {
		return false;
	}

	// 디스플레이 모드 리스트 채우기
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result)) {
		return false;
	}

	// 화면 해상도와 일치하는 모드를 찾아서 해당 주사율의 분자와 분모 저장
	for (i = 0; i < numModes; i++) {
		if (displayModeList[i].Width == (unsigned int)screenWidth) {
			if (displayModeList[i].Height == (unsigned int)screenHeight) {
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// 어댑터(비디오 카드) 설명 얻기
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result)) {
		return false;
	}

	// 비디오 카드 메모리를 MB 단위로 저장
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// 비디오 카드 이름을 문자 배열로 변환하여 저장
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0) {
		return false;
	}

	// 디스플레이 모드 리스트 해제
	delete[] displayModeList;
	displayModeList = 0;

	// 어댑터 출력 해제
	adapterOutput->Release();
	adapterOutput = 0;

	// 어댑터 해제
	adapter->Release();
	adapter = 0;

	// 팩토리 해제
	factory->Release();
	factory = 0;

	// 스왑 체인 설명 초기화
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// 백 버퍼를 1개로 설정
	swapChainDesc.BufferCount = 1;

	// 백 버퍼의 너비와 높이 설정
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// 백 버퍼 포맷 설정 (32비트)
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// 주사율 설정
	if (m_vsync_enabled) {
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else {
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// 백 버퍼의 용도 설정
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// 렌더링할 윈도우 핸들 지정
	swapChainDesc.OutputWindow = hwnd;

	// 멀티샘플링 비활성화
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// 전체화면 또는 창 모드 설정
	if (fullscreen) {
		swapChainDesc.Windowed = false;
	}
	else {
		swapChainDesc.Windowed = true;
	}

	// 스캔 라인 순서 및 스케일 설정 (지정되지 않음)
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 백 버퍼 내용 프레젠트 후 폐기
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// 고급 플래그 사용 안함
	swapChainDesc.Flags = 0;

	// DirectX 11 기능 수준 설정
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// 스왑 체인, 디바이스, 디바이스 컨텍스트 생성
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result)) {
		return false;
	}

	// 백 버퍼 포인터 가져오기
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result)) {
		return false;
	}

	// 백 버퍼를 기반으로 렌더 타겟 뷰 생성
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result)) {
		return false;
	}

	// 더 이상 필요 없으므로 백 버퍼 포인터 해제
	backBufferPtr->Release();
	backBufferPtr = 0;


	// 깊이 버퍼의 설명 초기화
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// 깊이 버퍼의 설명 설정
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// 설정한 설명을 사용하여 깊이 버퍼용 텍스처 생성
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 스텐실 상태의 설명 초기화
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// 스텐실 상태의 설명 설정
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// 픽셀이 전면일 때의 스텐실 동작 설정
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 픽셀이 후면일 때의 스텐실 동작 설정
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 깊이/스텐실 상태 생성
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	// 깊이/스텐실 상태를 출력 병합 단계에 설정
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// 깊이/스텐실 뷰 설명 초기화
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// 깊이/스텐실 뷰 설명 설정
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// 깊이/스텐실 뷰 생성
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	// 렌더 타겟 뷰와 깊이/스텐실 버퍼를 출력 렌더 파이프라인에 바인딩
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// 폴리곤을 어떻게 그리고 무엇을 그릴지 결정할 래스터 설명 설정
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// 설정한 설명을 사용하여 래스터라이저 상태 생성
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
	{
		return false;
	}

	// 래스터라이저 상태 설정
	m_deviceContext->RSSetState(m_rasterState);

	// 렌더링을 위한 뷰포트 설정
	m_viewport.Width = (float)screenWidth;
	m_viewport.Height = (float)screenHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	// 뷰포트 생성
	m_deviceContext->RSSetViewports(1, &m_viewport);

	// 투영 행렬 설정
	fieldOfView = 3.141592654f / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// 3D 렌더링을 위한 투영 행렬 생성
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	// 월드 행렬을 단위 행렬로 초기화
	m_worldMatrix = XMMatrixIdentity();

	// 2D 렌더링을 위한 직교 투영 행렬 생성
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	// 두 번째 깊이/스텐실 상태를 설정하기 전에 초기화
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	// 2D 렌더링을 위한 Z 버퍼 비활성화 깊이/스텐실 상태 생성
	// 차이점은 DepthEnable이 false로 설정된 것이고, 나머지 값은 동일
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 해당 설명을 사용하여 상태 생성
	result = m_device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
	if (FAILED(result))
	{
		return false;
	}


	// 블렌드 상태 설명 초기화
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// 알파 블렌딩이 활성화된 블렌드 상태 설명 생성
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;                    // 소스 색상: 그대로 사용
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;         // 대상 색상: (1 - 소스 알파)
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;                  // 혼합 연산: 더하기
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;               // 소스 알파: 그대로 사용
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;             // 대상 알파: 0
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;             // 알파 혼합 연산: 더하기
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;                  // RGBA 전부 출력

	// 해당 설명을 사용하여 블렌드 상태 생성 (알파 블렌딩 활성 상태)
	result = m_device->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState);
	if (FAILED(result))
	{
		return false;
	}

	// 알파 블렌딩이 비활성화된 블렌드 상태 설명으로 수정
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

	// 해당 설명을 사용하여 블렌드 상태 생성 (알파 블렌딩 비활성 상태)
	result = m_device->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}


void D3DClass::Shutdown()
{
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if (m_alphaEnableBlendingState)
	{
		m_alphaEnableBlendingState->Release();
		m_alphaEnableBlendingState = 0;
	}

	if (m_alphaDisableBlendingState)
	{
		m_alphaDisableBlendingState->Release();
		m_alphaDisableBlendingState = 0;
	}

	if (m_depthDisabledStencilState)
	{
		m_depthDisabledStencilState->Release();
		m_depthDisabledStencilState = 0;
	}


	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	return;
}


void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];

	// 렌더 타겟을 초기화할 색상을 설정
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// 백 버퍼(렌더 타겟)를 지정된 색상으로 지움
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// 깊이 버퍼(Depth Stencil View)를 초기화 (깊이: 1.0, 스텐실: 0)
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void D3DClass::EndScene()
{
	// 렌더링이 완료된 백 버퍼를 화면에 출력 (스왑 체인 교체)
	if (m_vsync_enabled)
	{
		// 수직 동기화를 사용하여 화면 주사율에 맞춰 표시 (화면 찢어짐 방지)
		m_swapChain->Present(1, 0);
	}
	else
	{
		// 가능한 한 빠르게 백 버퍼를 전환 (최대 FPS 출력)
		m_swapChain->Present(0, 0);
	}

	return;
}



ID3D11Device* D3DClass::GetDevice()
{
	return m_device;
}


ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_deviceContext;
}


void D3DClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}


void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}


void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}


void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}


void D3DClass::SetBackBufferRenderTarget()
{
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	return;
}


void D3DClass::ResetViewport()
{
	// Set the viewport.
	m_deviceContext->RSSetViewports(1, &m_viewport);

	return;
}

void D3DClass::TurnZBufferOn()
{
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
	return;
}


void D3DClass::TurnZBufferOff()
{
	m_deviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
	return;
}

void D3DClass::EnableAlphaBlending()
{
	float blendFactor[4];

	// 블렌드 팩터 값을 설정 (보통 0으로 설정)
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	// 알파 블렌딩을 활성화
	m_deviceContext->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);

	return;
}

void D3DClass::DisableAlphaBlending()
{
	float blendFactor[4];

	// 블렌드 팩터 값을 설정 (보통 0으로 설정)
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	// 알파 블렌딩을 비활성화
	m_deviceContext->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);

	return;
}

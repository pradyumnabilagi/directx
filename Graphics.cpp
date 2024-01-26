#include"Graphics.h"


Graphics::Graphics():pSwapChain(NULL),pDevice(NULL),pDeviceContext(NULL),pTarget(NULL),pDSV(NULL),pDSState(NULL), pDepthStencil(NULL)
{}

Graphics::~Graphics() {
	this->Release();
}

void Graphics::Release() {
	if (pTarget)
		pTarget->Release();
	if (pSwapChain)
		pSwapChain->Release();
	if (pDeviceContext)
		pDeviceContext->Release();
	if (pDevice)
		pDevice->Release();
	if (pDSV)
		pDSV->Release();
	if (pDSState)
		pDSState->Release();
	if (pDepthStencil)
		pDepthStencil->Release();
	pDepthStencil = NULL;
	pDSState = NULL;
	pDSV = NULL;
	pSwapChain = NULL;
	pDeviceContext = NULL;
	pDevice = NULL;
	pTarget = NULL;
}

bool Graphics::Init(HWND hwnd)
{
	DXGI_SWAP_CHAIN_DESC sd = { 0 };
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hwnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	HRESULT hr=D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, 0, D3D11_SDK_VERSION, &sd, &pSwapChain, &pDevice, NULL, &pDeviceContext);
	if (FAILED(hr))
	{
		return false;
	}

	ID3D11Resource* pBackBuffer = NULL;
	
	hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), (void**)&pBackBuffer);
	if (FAILED(hr))
	{
		this->Release();
		return false;
	}

	hr = pDevice->CreateRenderTargetView(pBackBuffer, NULL, &pTarget);
	if (FAILED(hr))
	{
		this->Release();
		return false;
	}

	if (pBackBuffer)
		pBackBuffer->Release();

	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	if (FAILED(pDevice->CreateDepthStencilState(&dsDesc, &pDSState)))
	{
		this->Release();
		return false;
	}

	pDeviceContext->OMSetDepthStencilState(pDSState, 1u);
	

	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = 800u;
	descDepth.Height = 600u;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	if (FAILED(pDevice->CreateTexture2D(&descDepth,NULL,&pDepthStencil)))
	{
		this->Release();
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;
	if (FAILED(pDevice->CreateDepthStencilView(pDepthStencil, &descDSV,&pDSV)))
	{
		this->Release();
		return false;
	}
	

	pDeviceContext->OMSetRenderTargets(1u, &pTarget,pDSV);


	
	
	return true;
}

bool Graphics::EndFrame()
{
	HRESULT hr = this->pSwapChain->Present(1u, 0u);
	if (FAILED(hr)) {
		return false;
	}
	return true;
}


void Graphics::ClearBuffer(float r, float g, float b)
{
	const float color[] = { r,g,b,1 };
	this->pDeviceContext->ClearRenderTargetView(this->pTarget, color);
	this->pDeviceContext->ClearDepthStencilView(this->pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

/*
void Graphics::DrawTriangle()
{
	const Vertex vertices[] =
	{
		{0.0f,0.5f,255,255,0,0},
		{0.5f,-0.5f,255,0,255,0},
		{-0.5f,-0.5f,0,255,255,0},
		{0.3f,0.3f,128,128,128,0},
		{-0.3f,0.3f,0,255,0,0}
	};

	ID3D11Buffer* pVertexBuffer = NULL, * pIndexBuffer = NULL;
	ID3D11InputLayout* pInputLayout = NULL;
	ID3D11VertexShader* pVertexShader = NULL;
	ID3DBlob* pBlob = NULL;
	ID3D11PixelShader* pPixelShader = NULL;

	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeof(vertices);
	bd.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices;
	// createing the vertex buffer
	if (FAILED(pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer)))
	{
		ReleasePtrs(&pVertexBuffer, &pIndexBuffer, &pInputLayout, &pVertexShader, &pBlob, &pPixelShader);
		return;
	}
	
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	// setting the vertex buffer
	pDeviceContext->IASetVertexBuffers(0u, 1u, &pVertexBuffer, &stride, &offset);
	
	// create Index buffer
	const unsigned short indices[] = {
		0,1,2,
		0,3,1,
		4,0,2
	};

	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = sizeof(indices);
	ibd.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA  isd = {};
	isd.pSysMem = indices;
	if (FAILED(pDevice->CreateBuffer(&ibd,&isd,&pIndexBuffer)))
	{
		ReleasePtrs(&pVertexBuffer, &pIndexBuffer, &pInputLayout, &pVertexShader, &pBlob, &pPixelShader);
		return;
	}

	pDeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0u);

	//creating the pixel shader
	if (FAILED(D3DReadFileToBlob(L"PixelShader.cso", &pBlob)))
	{
		ReleasePtrs(&pVertexBuffer, &pIndexBuffer, &pInputLayout, &pVertexShader, &pBlob, &pPixelShader);
		return;
	}

	if (FAILED(pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pPixelShader)))
	{
		ReleasePtrs(&pVertexBuffer, &pIndexBuffer, &pInputLayout, &pVertexShader, &pBlob, &pPixelShader);
		return;
	}
	// setting the pixel shader
	pDeviceContext->PSSetShader(pPixelShader, NULL, 0u);

	if (pBlob)
		pBlob->Release();
	pBlob = NULL;
	// creating the vertex shader
	if (FAILED(D3DReadFileToBlob(L"VertexShader.cso", &pBlob)))
	{
		ReleasePtrs(&pVertexBuffer, &pIndexBuffer, &pInputLayout, &pVertexShader, &pBlob, &pPixelShader);
		return;
	}

	if (FAILED(pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pVertexShader)))
	{
		ReleasePtrs(&pVertexBuffer, &pIndexBuffer, &pInputLayout, &pVertexShader, &pBlob, &pPixelShader);
		return;
	}
	// setting the vertexshader
	pDeviceContext->VSSetShader(pVertexShader, NULL, 0u);

	// creating the input layout
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"Position",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"Color",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,8u,D3D11_INPUT_PER_VERTEX_DATA,0}
	};
	
	if (FAILED(pDevice->CreateInputLayout(ied, (UINT)std::size(ied), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout)))
	{
		ReleasePtrs(&pVertexBuffer, &pIndexBuffer, &pInputLayout, &pVertexShader, &pBlob, &pPixelShader);
		return;
	}
	// setting the input layout
	pDeviceContext->IASetInputLayout(pInputLayout);
	// setting the output render target
	pDeviceContext->OMSetRenderTargets(1u, &pTarget, NULL);
	// setting the primitive
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// setting the viewport
	D3D11_VIEWPORT vp;
	vp.Width = 800;
	vp.Height = 600;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pDeviceContext->RSSetViewports(1u, &vp);
	// finally drawing
	pDeviceContext->DrawIndexed((UINT)std::size(indices),0u,0u);

	ReleasePtrs(&pVertexBuffer, &pIndexBuffer, &pInputLayout, &pVertexShader, &pBlob, &pPixelShader);
	
}
*/

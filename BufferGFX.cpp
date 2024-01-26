#include<d3dcompiler.h>
#include<vector>
#include<DirectXMath.h>
#include"BufferGFX.h"


struct ConstantBuffer
{
	DirectX::XMMATRIX transform;
};
BufferGFX::BufferGFX(): nIndex(0),pVertexBuffer(NULL),pIndexBuffer(NULL),pInputLayout(NULL),pVertexShader(NULL),pPixelShader(NULL),graphics(NULL),pConstantBuffer(NULL)
{}

BufferGFX::~BufferGFX()
{
	this->Release();
}


void BufferGFX::Release()
{
	if (pVertexBuffer) {
		(pVertexBuffer)->Release();
		pVertexBuffer = NULL;
	}
	if (pIndexBuffer) {
		(pIndexBuffer)->Release();
		pIndexBuffer = NULL;
	}
	if (pInputLayout) {
		(pInputLayout)->Release();
		pInputLayout = NULL;
	}
	if (pVertexShader) {
		(pVertexShader)->Release();
		pVertexShader = NULL;
	}
	if (pPixelShader) {
		(pPixelShader)->Release();
		pPixelShader = NULL;
	}
	if (pConstantBuffer) {
		pConstantBuffer->Release();
		pConstantBuffer = NULL;
	}
}


bool BufferGFX::Init(Graphics* graphics, Vertex* vertices,int nVertices, unsigned short* indices,int nIndices)
{
	this->graphics = graphics;
	nIndex = nIndices;

	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeof(Vertex)*nVertices;
	bd.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices;
	if (FAILED(graphics->pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer)))
	{
		this->Release();
		return false;
	}


	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = sizeof(unsigned short)*nIndices;
	ibd.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices;
	if (FAILED(graphics->pDevice->CreateBuffer(&ibd,&isd,&pIndexBuffer)))
	{
		this->Release();
		return false;
	}


	float cb[4][4] = {
		{1,0,0,0},
		{0,1,0,0},
		{0,0,1,0},
		{0,0,0,1}
	};
	D3D11_BUFFER_DESC cbd = {};
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(cb);
	cbd.StructureByteStride = 0u;

	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &cb;
	if (FAILED(graphics->pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer)))
	{
		this->Release();
		return false;
	}
	
	

	ID3DBlob* pBlob=NULL;

	if (FAILED(D3DReadFileToBlob(L"PixelShader.cso", &pBlob)))
	{
		this->Release();
		return false;
	}

	if (FAILED(graphics->pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pPixelShader)))
	{
		if (pBlob)
		{
			pBlob->Release();
		}
		this->Release();
		return false;
	}

	if (pBlob)
	{
		pBlob->Release();
		pBlob = NULL;
	}

	if (FAILED(D3DReadFileToBlob(L"VertexShader.cso", &pBlob)))
	{
		this->Release();
		return false;
	}

	if (FAILED(graphics->pDevice->CreateVertexShader(pBlob->GetBufferPointer(),pBlob->GetBufferSize(),NULL,&pVertexShader)))
	{
		if (pBlob)
		{
			pBlob->Release();
		}
		this->Release();
		return false;
	}

	

	const D3D11_INPUT_ELEMENT_DESC ied[] = {
		{"Position",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"Color",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,16u,D3D11_INPUT_PER_VERTEX_DATA,0}
	};
	
	if (FAILED(graphics->pDevice->CreateInputLayout(ied,(UINT)std::size(ied),pBlob->GetBufferPointer(),pBlob->GetBufferSize(),&pInputLayout)))
	{
		if (pBlob)
		{
			pBlob->Release();
		}
		this->Release();
		return false;
	}

	if (pBlob)
	{
		pBlob->Release();
		pBlob = NULL;
	}
	return true;
}

void BufferGFX::InitContext()
{
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;

	graphics->pDeviceContext->IASetVertexBuffers(0u,1u,&pVertexBuffer,&stride,&offset);

	graphics->pDeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0u);

	graphics->pDeviceContext->VSSetConstantBuffers(0u, 1u, &pConstantBuffer);

	graphics->pDeviceContext->PSSetShader(pPixelShader, NULL, 0u);

	graphics->pDeviceContext->VSSetShader(pVertexShader, NULL, 0u);

	// setting the input layout
	graphics->pDeviceContext->IASetInputLayout(pInputLayout);
	// setting the primitive
	graphics->pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// setting the viewport
	D3D11_VIEWPORT vp;
	vp.Width = 800;
	vp.Height = 600;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	graphics->pDeviceContext->RSSetViewports(1u, &vp);
}

void BufferGFX::Draw()
{
	if (FAILED(graphics->pDeviceContext->Map(pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return;
	}
	float cd[4][4] = {
		{1,0,0,0},
		{0,1,0,0},
		{0,0,1,0},
		{0,0,0,1}
	};
	memcpy(mappedResource.pData, cd, sizeof(cd));
	graphics->pDeviceContext->Unmap(pConstantBuffer, 0);
	graphics->pDeviceContext->DrawIndexed(nIndex, 0u, 0u);
	
}

#pragma once
#include<d3d11.h>
#include"Graphics.h"

struct Vertex
{
	float x;
	float y;
	float z;
	float w;
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

class BufferGFX
{
	ID3D11Buffer* pVertexBuffer;
	ID3D11Buffer* pIndexBuffer;
	ID3D11Buffer* pConstantBuffer;
	int nIndex;
	ID3D11InputLayout* pInputLayout;
	ID3D11VertexShader* pVertexShader;
	ID3D11PixelShader* pPixelShader;
	Graphics* graphics;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
public:
	BufferGFX();
	~BufferGFX();
	void Release();
	bool Init(Graphics* graphics,Vertex* vertices,int nVertices,unsigned short* indices,int nIndices);
	void InitContext();
	void Draw();

};


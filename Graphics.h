#pragma once
#include<d3d11.h>

class BufferGFX;

class Graphics {
	IDXGISwapChain* pSwapChain;
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pDeviceContext;
	ID3D11RenderTargetView* pTarget;
	ID3D11DepthStencilView* pDSV;
	ID3D11DepthStencilState* pDSState ;
	ID3D11Texture2D* pDepthStencil ;
	friend class BufferGFX;
public:
	Graphics();
	~Graphics();
	bool Init(HWND hwnd);
	void Release();
	void ClearBuffer(float r, float g, float b);
	bool EndFrame();
};

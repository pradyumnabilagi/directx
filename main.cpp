#include<Windows.h>
#include"BufferGFX.h"

LRESULT CALLBACK WindowProc(HWND hend, UINT uMsg, WPARAM wp, LPARAM lp);

int CALLBACK WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	WNDCLASSEX wc = {0};
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"WindowClass";
	
	if (!RegisterClassEx(&wc))
	{
		return -1;
	}

	RECT windowRect = { 0,0,800,600 };
	AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW);

	HWND hwnd = CreateWindowEx(
		0,
		L"WindowClass",
		L"My Window",
		WS_OVERLAPPEDWINDOW,
		100,
		100,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hwnd)
	{
		return -1;
	}
	

	Graphics myGraphics;
	if (!myGraphics.Init(hwnd))
	{
		return -1;
	}

	BufferGFX bufferObject;
	Vertex vertices[] = {
		{0.0f,0.5f,0.5f,1.0f,255,128,255,0},
		{0.5f,-0.5f,0.5f,1.0f,255,128,255,0},
		{-0.5f,-0.5f,0.5f,1.0f,255,128,255,0}
	};
	unsigned short indices[] = {
		0,1,2
	};

	if (!bufferObject.Init(&myGraphics,vertices,3,indices,3))
	{
		return -1;
	}
	bufferObject.InitContext();

	ShowWindow(hwnd, SW_SHOW);

	MSG msg = {0};
	msg.message = WM_NULL;
	while (msg.message != WM_QUIT)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// event loop
		myGraphics.ClearBuffer(1, 0, 0);
		bufferObject.Draw();
		myGraphics.EndFrame();
	}
	myGraphics.Release();
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
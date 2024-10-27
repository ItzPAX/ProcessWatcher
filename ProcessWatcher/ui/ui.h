#pragma once

class empax_ui
{
public:
	LPDIRECT3D9              g_pD3D = nullptr;
	LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
	bool                     g_DeviceLost = false;
	D3DPRESENT_PARAMETERS    g_d3dpp = {};

	UINT wnd_height;
	UINT wnd_width;

public:
	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
	void ResetDevice();

public:
	empax_ui(Win32::Window* window);
	~empax_ui();
	void on_present();
};
#include "includes.h"
#include "w32Window.h"

#define DCX_USESTYLE 0x00010000

namespace Win32 {

	Window::Window(std::wstring title, HICON icon, WindowType type)
		: Win32::SubObject(title, title, icon), m_Type(type)
	{
		Size(DEFAULT_WIDTH, DEFAULT_HEIGHT);
	}

	Window::~Window()
	{
		ReleaseDC(m_Handle, m_DC);
		DestroyWindow(m_Handle);
	}

	VOID Window::Initialize()
	{
		RECT desktop;
		const HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &desktop);

		RECT R = { 0, 0, m_Size.cx, m_Size.cy };
		AdjustWindowRect(&R, m_Type, false);
		int width = R.right - R.left;
		int height = R.bottom - R.top;

		m_Handle = CreateWindow(m_Class.c_str(), m_Title.c_str(),
			m_Type, ((desktop.right / 2) - (m_Size.cx / 2)), ((desktop.bottom / 2) - (m_Size.cy / 2)), m_Size.cx, m_Size.cy, 0, 0, GetModuleHandle(NULL), (void*)this);

		ShowWindow(m_Handle, SW_SHOW);
		UpdateWindow(m_Handle);

		// Creating OpenGL Render Context

		m_DC = GetDC(m_Handle);

		int pixelFormat = 0;
		UINT numFormats = 0;
	}

	VOID Window::Present(bool bVsync)
	{
		SwapBuffers(m_DC);
	}

	LRESULT Window::MessageHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam))
			return true;

		switch (message)
		{
		case WM_NCCREATE:		{ OnNonClientCreate();										}	return TRUE;
		case WM_NCACTIVATE:		{ OnNonClientActivate(LOWORD(wParam) != WA_INACTIVE);		}   return TRUE;
		case WM_NCPAINT:		{ OnNonClientPaint(reinterpret_cast<HRGN>(wParam));			}	return FALSE;
		case WM_NCLBUTTONDOWN:	{ OnNonClientLeftMouseButtonDown();							}	break;
		case WM_NCLBUTTONDBLCLK:{ Win32::Utils::MaximizeWindow(Handle());					}	return FALSE;
		case WM_SIZE:			{ OnResize(wParam, lParam);									}	return FALSE;

		case WM_PAINT:			{ OnPaint();												}	break;
		case WM_GETMINMAXINFO:	{ OnGetMinMaxInfo(reinterpret_cast<MINMAXINFO*>(lParam));	}	return FALSE;
		case WM_TIMER:			{ RedrawWindow();											}	break;
		case WM_EXITSIZEMOVE:	{ OnExitSizeMove();											}	break;
		case WM_INPUT:			{ OnInput(wParam, lParam);									}	return FALSE;

		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
			break;
		}

		return SubObject::MessageHandler(hwnd, message, wParam, lParam);
	}

	VOID Window::SetTitle(LPCWSTR title)
	{
		SetWindowText(Handle(), title);
	}

	VOID Window::MakeCurrentContext()
	{

	}

	VOID Window::OnNonClientCreate()
	{
		SetTimer(Handle(), 1, 100, NULL);
		SetWindowTheme(Handle(), L"", L"");

		Win32::Utils::ModifyClassStyle(Handle(), 0, CS_DROPSHADOW);

		Win32::Caption::AddCaptionButton(new CaptionButton(L"X", CB_CLOSE));
		Win32::Caption::AddCaptionButton(new CaptionButton(L"🗖", CB_MAXIMIZE));
		Win32::Caption::AddCaptionButton(new CaptionButton(L"🗕", CB_MINIMIZE));
	}

	VOID Window::OnNonClientActivate(BOOL active)
	{
		Active(active);
	}

	VOID Window::OnNonClientPaint(HRGN region) 
	{
		// Start draw
		HDC hdc = GetDCEx(Handle(), region, DCX_WINDOW | DCX_INTERSECTRGN | DCX_USESTYLE);

		RECT rect;
		GetWindowRect(Handle(), &rect);

		SIZE size = SIZE{ rect.right - rect.left, rect.bottom - rect.top };

		HBITMAP hbmMem = CreateCompatibleBitmap(hdc, size.cx, size.cy);
		HANDLE hOld = SelectObject(hdc, hbmMem);

		// Draw
		HBRUSH brush = CreateSolidBrush(RGB(46, 46, 46));

		RECT newRect = RECT{ 0, 0, size.cx, size.cy };

		FillRect(hdc, &newRect, brush);

		if (Active()) {
			brush = CreateSolidBrush(RGB(60, 120, 200));
			FrameRect(hdc, &newRect, brush);
		}

		PaintCaption(hdc);

		DeleteObject(brush);

		// End Draw
		BitBlt(hdc, 0, 0, size.cx, size.cy, hdc, 0, 0, SRCCOPY);
		
		SelectObject(hdc, hOld);
		DeleteObject(hbmMem);

		ReleaseDC(Handle(), hdc);
	}

	VOID Window::RedrawWindow() 
	{
		// Reset window
		SetWindowPos(Handle(), 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_DRAWFRAME | SWP_FRAMECHANGED);
		SendMessage(Handle(), WM_PAINT, 0, 0);
	}

	VOID Window::PaintCaption(HDC hdc)
	{
		RECT rect;
		GetWindowRect(Handle(), &rect);

		SIZE size = SIZE{ rect.right - rect.left, rect.bottom - rect.top };

		if (ShowTitle())
		{
			rect = RECT{ 0, 0, size.cx, 30 };

			SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc, Active() ? RGB(255, 255, 255) : RGB(92, 92, 92));

			DrawText(hdc, m_Title.c_str(), (int)wcslen(m_Title.c_str()), &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
		}

		POINT pt;
		GetCursorPos(&pt);
		GetWindowRect(Handle(), &rect);

		pt.x -= rect.left;
		pt.y -= rect.top;

		int spacing = 0;
		for (auto& button : Caption::CaptionButtons()) {
			spacing += button->m_Width;
			button->m_Rect = RECT{ size.cx - spacing, 0, size.cx - spacing + button->m_Width, 30 };

			// collision check with mouse and box
			if (button->m_Rect.left < pt.x && button->m_Rect.right > pt.x && button->m_Rect.top < pt.y && button->m_Rect.bottom > pt.y)
			{
				HBRUSH brush = CreateSolidBrush(RGB(70, 80, 92));
				FillRect(hdc, &button->m_Rect, brush);

				DeleteObject(brush);
			}

			if (button->m_Text.compare(L"🗖") == 0 && Win32::Utils::IsWindowFullscreen(Handle()))
				button->m_Text = L"🗗";
			else if (button->m_Text.compare(L"🗗") == 0 && !Win32::Utils::IsWindowFullscreen(Handle()))
				button->m_Text = L"🗖";

			DrawText(hdc, button->m_Text.c_str(), (int)wcslen(button->m_Text.c_str()), &button->m_Rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
		}
	}

	VOID Window::OnNonClientLeftMouseButtonDown()
	{
		POINT pt;
		GetCursorPos(&pt);

		RECT rect;
		GetWindowRect(Handle(), &rect);

		pt.x -= rect.left;
		pt.y -= rect.top;

		int spacing = 0;
		for (auto& button : Caption::CaptionButtons()) {
			// collision check with mouse and box
			if (button->m_Rect.left < pt.x && button->m_Rect.right > pt.x && button->m_Rect.top < pt.y && button->m_Rect.bottom > pt.y)
			{
				switch (button->m_Command)
				{
				case CB_CLOSE: { 
					SendMessage(Handle(), WM_QUIT, 0, 0);
					exit(1);
				} 
					break;
				case CB_MINIMIZE: { ShowWindow(Handle(), SW_MINIMIZE); } break;
				case CB_MAXIMIZE: { Win32::Utils::MaximizeWindow(Handle()); } break;
				}
			}
		}
	}

	VOID Window::OnGetMinMaxInfo(MINMAXINFO* minmax) 
	{
		RECT WorkArea; SystemParametersInfo(SPI_GETWORKAREA, 0, &WorkArea, 0);
		minmax->ptMaxSize.x = (WorkArea.right - WorkArea.left);
		minmax->ptMaxSize.y = (WorkArea.bottom - WorkArea.top);
		minmax->ptMaxPosition.x = WorkArea.left;
		minmax->ptMaxPosition.y = WorkArea.top;
		minmax->ptMinTrackSize.x = 400;
		minmax->ptMinTrackSize.y = 300;
	}

	VOID Window::OnExitSizeMove() 
	{
		RECT rect;
		GetWindowRect(Handle(), &rect);
		RECT WorkArea; SystemParametersInfo(SPI_GETWORKAREA, 0, &WorkArea, 0);
		if (rect.top < WorkArea.top + 10 && !Win32::Utils::IsWindowFullscreen(Handle()))
			Win32::Utils::MaximizeWindow(Handle());
	}

	VOID Window::OnPaint()
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(Handle(), &ps);

		RECT rc;
		GetClientRect(Handle(), &rc);

		HBRUSH brush = CreateSolidBrush(RGB(36, 36, 36));

		FillRect(hdc, &rc, brush);

		DeleteObject(brush);

		EndPaint(Handle(), &ps);
	}

	VOID Win32::Window::OnResize(WPARAM wParam, LPARAM lParam)
	{
		if (m_width && m_height)
		{
			*m_width = (UINT)LOWORD(lParam);
			*m_height = (UINT)HIWORD(lParam);
		}
	}

	VOID Win32::Window::OnInput(WPARAM wParam, LPARAM lParam)
	{
		auto inp = GET_RAWINPUT_CODE_WPARAM(wParam);
		if (inp == RIM_INPUTSINK)
			return;

		RAWINPUT inputBuffer;
		UINT rawInputSize = sizeof(inputBuffer);
		GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, &inputBuffer, &rawInputSize, sizeof(RAWINPUTHEADER));

		auto mouse = inputBuffer.data.mouse;
		//m_MouseSpeed = glm::vec2(mouse.lLastX, mouse.lLastY);

		DefWindowProc(Handle(), WM_INPUT, wParam, lParam);
	}
}
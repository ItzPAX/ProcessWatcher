#pragma once

// Windows API
#ifdef WIN32
#include <Windows.h>
#include <windowsx.h>
#endif

// STL Container
#include <string>
#include <sstream>
#include <list>
#include <comdef.h>
#include <deque>
#include <unordered_map>

// error handling
#include <assert.h>
#include <stdexcept>

// Pointers
#include <memory>

// I/O
#include <fstream>
#include <filesystem>
#include <direct.h>
#include <thread>

// Time
#include <chrono>


// imgui
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

// directx
#include <d3d9.h>
#include <tchar.h>

#include "imgui/imgui_internal.h"

// window defs
#define DEFAULT_WIDTH 1000
#define DEFAULT_HEIGHT 600

// Core engine features
#ifdef WIN32
#include "Win32/w32Win32Utils.h"
#include "Win32/w32SubObject.h"
#include "Win32/w32Caption.h"
#include "Win32/w32Window.h"
#endif

#include "ui/ui.h"
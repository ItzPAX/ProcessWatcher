#include "includes.h"
#include "ui.h"

bool empax_ui::CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void empax_ui::CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = nullptr; }
}

void empax_ui::ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

empax_ui::empax_ui(Win32::Window* window)
{
    window->m_height = &wnd_height;
    window->m_width = &wnd_width;

    // Initialize Direct3D
    if (!CreateDeviceD3D(window->Handle()))
    {
        CleanupDeviceD3D();
        return;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(window->Handle());
    ImGui_ImplDX9_Init(g_pd3dDevice);
}

empax_ui::~empax_ui()
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
}

void empax_ui::on_present(std::vector<std::string>& logs, ImportInfo* import_info, int import_count)
{

    // Handle lost D3D9 device
    if (g_DeviceLost)
    {
        HRESULT hr = g_pd3dDevice->TestCooperativeLevel();
        if (hr == D3DERR_DEVICELOST)
        {
            Sleep(10);
            return;
        }
        if (hr == D3DERR_DEVICENOTRESET)
            ResetDevice();
        g_DeviceLost = false;
    }

    if (wnd_width != 0 || wnd_height != 0)
    {
        g_d3dpp.BackBufferWidth = wnd_width;
        g_d3dpp.BackBufferHeight = wnd_height;
        wnd_width = wnd_height = 0;
        ResetDevice();
    }

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // UI CODE
    ImGui::Begin("Logs");
    ImGui::BeginListBox("##LogStrings", ImVec2(-1, -1));
    int log_idx = 0;
    for (auto& str : logs)
    {
        bool f = false;
        ImGui::Selectable((str + "##" + std::to_string(log_idx)).c_str(), &f);
        log_idx++;
    }
    ImGui::EndListBox();
    ImGui::End();


    ImGui::Begin("Imports");
    ImGui::BeginListBox("##ImportStrings", ImVec2(-1, -1));
    static int active_import = 0;
    for (int i = 0; i < import_count; i++)
    {
        ImportInfo info = import_info[i];
        std::string display(std::string(info.module_name) + ':' + std::string(info.name) + "##" + std::to_string(i));

        bool f = i == active_import;
        if (ImGui::Selectable(display.c_str(), &f))
            active_import = i;
    }
    ImGui::EndListBox();
    ImGui::End();

    // Rendering
    ImGui::EndFrame();
    g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
    g_pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
    if (g_pd3dDevice->BeginScene() >= 0)
    {
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        g_pd3dDevice->EndScene();
    }
    HRESULT result = g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);
    if (result == D3DERR_DEVICELOST)
        g_DeviceLost = true;
}
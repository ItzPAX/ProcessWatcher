#include "includes.h"

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
	comm::init();
	Win32::Window window(L"ProcessWatcher", NULL);

	// create window
	window.RegisterNewClass();
	window.Initialize();

	empax_ui ui(&window);

	window.m_height = &ui.wnd_height;
	window.m_width = &ui.wnd_width;

	std::chrono::system_clock::time_point m_PreviousTime;
	std::vector<std::string> logs;

	ImportInfo import_info[1024];
	int import_count;

	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		// If there are Window messages process them
		if (PeekMessage(&msg, HWND(), NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			comm::read_import_data(import_info, &import_count);
			comm::read_log_data(logs);
			ui.on_present(logs, import_info, import_count);
		}
	}

	return 0;
}
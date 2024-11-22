#include <iostream>
#include <windows.h>

namespace comm
{
    struct SharedData {
        volatile LONG readyFlag;  // 0 = not ready, 1 = ready
        char data[1024];          // String data
    };

    static SharedData* pBuf;

    static void init()
    {
        HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, L"ProcWatchSharedMem");
        if (hMapFile)
        {
            pBuf = (SharedData*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 1024);
        }
    }

    static void write_log_data(std::string data)
    {
        CopyMemory((PVOID)pBuf->data, data.c_str(), strlen(data.c_str()) + 1);
        pBuf->readyFlag = 1;
    }
}

int main()
{
    int m1 = 10;
    int m2 = 20;

	while (true)
	{
        Sleep(5000);
		//MessageBoxA(NULL, "Hello :)", "TestApp", NULL);
	}
	return 0;
}
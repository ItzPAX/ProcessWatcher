#include "hooklib.h"

HookLib::HookLib()
{
	imageBase = GetModuleHandleA(NULL);
	PIMAGE_DOS_HEADER dosHeaders = (PIMAGE_DOS_HEADER)imageBase;
	PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((DWORD_PTR)imageBase + dosHeaders->e_lfanew);

	IMAGE_DATA_DIRECTORY importsDirectory = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	importDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)(importsDirectory.VirtualAddress + (DWORD_PTR)imageBase);
}

void HookLib::InstallHooks(std::vector<HOOK_DATA> data)
{
	PIMAGE_IMPORT_BY_NAME functionName = NULL;
	LPCSTR libraryName = NULL;

	while (importDescriptor->Name != NULL)
	{
		libraryName = (LPCSTR)importDescriptor->Name + (DWORD_PTR)imageBase;
		HMODULE library = LoadLibraryA(libraryName);

		if (library)
		{
			PIMAGE_THUNK_DATA originalFirstThunk = NULL, firstThunk = NULL;
			originalFirstThunk = (PIMAGE_THUNK_DATA)((DWORD_PTR)imageBase + importDescriptor->OriginalFirstThunk);
			firstThunk = (PIMAGE_THUNK_DATA)((DWORD_PTR)imageBase + importDescriptor->FirstThunk);

			while (originalFirstThunk->u1.AddressOfData != NULL)
			{
				for (auto& hk : data)
				{
					functionName = (PIMAGE_IMPORT_BY_NAME)((DWORD_PTR)imageBase + originalFirstThunk->u1.AddressOfData);

					printf("%s, %s, %d\n", functionName->Name, hk.name.c_str(), strcmp(functionName->Name, hk.name.c_str()));

					// find function
					if (std::string(functionName->Name).compare(hk.name) == 0)
					{
						SIZE_T bytesWritten = 0;
						DWORD oldProtect = 0;
						VirtualProtect((LPVOID)(&firstThunk->u1.Function), 8, PAGE_READWRITE, &oldProtect);
						firstThunk->u1.Function = (DWORD_PTR)hk.hk_func;	// swap to new pointer
						VirtualProtect((LPVOID)(&firstThunk->u1.Function), 8, oldProtect, &oldProtect);
					}
				}

				++originalFirstThunk;
				++firstThunk;
			}
		}

		importDescriptor++;
	}
}
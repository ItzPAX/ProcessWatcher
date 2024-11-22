#pragma once

#include <Windows.h>
#include <unordered_map>
#include <string>

struct HOOK_DATA
{
    std::string name;
    void* hk_func;
};

struct ImportInfo;
class HookLib
{
private:
	LPVOID imageBase;
	PIMAGE_IMPORT_DESCRIPTOR importDescriptor;

public:
	HookLib();
	~HookLib() = default;

public:
	void InstallHooks(std::vector<HOOK_DATA> data);
	void FillImportData(ImportInfo* info, int& import_count);
};
#pragma once

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
	void InstallHook(std::string func_name, void* new_func, void** orig);
	void FillImportData(ImportInfo* info, int& import_count);
};
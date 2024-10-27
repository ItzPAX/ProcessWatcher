#pragma once
#include "hooklib.h"
#include "communication.hpp"
#include <Windows.h>
#include <unordered_map>

namespace templates
{
    using t_MessageBoxA = int (WINAPI*)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
    static t_MessageBoxA o_MsgBoxA = MessageBoxA;

    using t_Sleep = void (WINAPI*)(DWORD ms);
    static t_Sleep o_Sleep = Sleep;

    using t_memcpy = void* (WINAPI*)(void* dst, const void* src, size_t size);
    static t_memcpy o_memcpy = memcpy;
}

namespace hk_functions
{
    static int hk_MessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
    {
        LOG_FUNCTION_CALL("MessageBoxA", hWnd, lpText, lpCaption, uType);
        return templates::o_MsgBoxA(hWnd, lpText, lpCaption, uType);
    }

    static void hk_Sleep(DWORD ms)
    {
        LOG_FUNCTION_CALL("Sleep", ms);
        return templates::o_Sleep(ms);
    }

    static void* hk_memcpy(void* dst, const void* src, size_t size)
    {
        LOG_FUNCTION_CALL("memcpy", dst, src, size);
        return templates::o_memcpy(dst, src, size);
    }
}

namespace sys_hooks
{
    static HookLib hl;
    static std::vector<HOOK_DATA> data;

    static void install_hooks()
    {
        data.push_back({ "MessageBoxA" , (void*)hk_functions::hk_MessageBoxA });
        data.push_back({ "Sleep" , (void*)hk_functions::hk_Sleep });
        data.push_back({ "memcpy" , (void*)hk_functions::hk_memcpy });

        hl.InstallHooks(data);
    }
}
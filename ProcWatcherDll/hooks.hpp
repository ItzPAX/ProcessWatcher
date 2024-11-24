#pragma once
#include "includes.h"

namespace templates
{
    using t_Sleep = void (WINAPI*)(DWORD ms);
    static t_Sleep o_Sleep;
}

namespace hk_functions
{
    static void hk_Sleep(DWORD ms)
    {
        LOG_FUNCTION_CALL("Sleep", ms);
        return templates::o_Sleep(ms);
    }
}

namespace sys_hooks
{
    static void install_hooks(HookLib hl)
    {
        hl.InstallHook("Sleep", (void*)hk_functions::hk_Sleep, (void**) & templates::o_Sleep);
    }
}
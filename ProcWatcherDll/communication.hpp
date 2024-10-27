#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
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
        InterlockedExchange(&pBuf->readyFlag, 1);
    }
}

#define LOG_FUNCTION_CALL(function_name, ...) log_call(function_name, __VA_ARGS__)

template <typename T>
std::string argToString(T&& arg) {
    if constexpr (std::is_same_v<std::decay_t<T>, const char*> || std::is_same_v<std::decay_t<T>, char*>)
        return std::string(arg);
    else if constexpr (std::is_same_v<std::decay_t<T>, bool>)
        return arg ? "true" : "false";
    else if constexpr (std::is_arithmetic_v<std::decay_t<T>>)
        return std::to_string(arg);
    else {
        std::ostringstream oss;
        oss << std::forward<T>(arg);
        return oss.str();
    }
}

template <typename... Args>
void log_call(const char* function_name, Args&&... args) {
    std::stringstream ss;
    ss << "[ProcessWatcher] -> Function: " << function_name << " called with arguments: ";

    bool first = true;
    ((ss << (first ? "" : ", ") << argToString(std::forward<Args>(args)), first = false), ...);

    comm::write_log_data(ss.str());
}
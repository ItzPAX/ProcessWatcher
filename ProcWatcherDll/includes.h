#pragma once

#include <Windows.h>
#include <unordered_map>
#include <string>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>

enum ARG_TYPES : int
{
    U32 = 0,
    U64,
    S32,
    S64,
    F32,
    F64
};

#include "hooklib.h"
#include "communication.hpp"
static communication comm;

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

    comm.write_log_data(ss.str());
}

#include "hooks.hpp"
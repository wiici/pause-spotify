#pragma once

#include <string>
#include <Windows.h>

#include <version>

using pid_t = DWORD;

std::string utf16_to_utf8(const std::wstring& utf16_string);

std::string GetProcessExecName(const pid_t pid);

std::string GetLastErrorMessage();
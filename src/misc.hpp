#pragma once

#include <string>

std::string utf16_to_utf8(const std::wstring& utf16_string);

std::string GetProcessExecName(const unsigned long pid);

std::string GetLastErrorMessage();
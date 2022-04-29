#pragma once

#include <spdlog/spdlog.h>
#include <cassert>
#include <comdef.h>


#define EXIT_ON_ERROR(hres)                                                    \
    if (FAILED(hres)) {                                                        \
        goto done;                                                             \
    }
#define SAFE_RELEASE(comResource)                                              \
    if ((comResource) != nullptr) {                                            \
        (comResource)->Release();                                              \
        (comResource) = NULL;                                                  \
    }

void logHRESULT(HRESULT hresult, const char* fullFilenamePath,
                size_t lineNumber);

#define GET_NAME(line) hr_##line##_var
#define VAR(line) GET_NAME(line)
#define HR_VAR_NAME VAR(__LINE__)

#define CHECK_HR(x)                                                            \
    HRESULT HR_VAR_NAME = x;                                                   \
    if (FAILED(HR_VAR_NAME)) {                                                 \
        logHRESULT(HR_VAR_NAME, __FILE__, __LINE__);                           \
        throw std::runtime_error("Runtime error related to winapi.");          \
    }

std::string utf16_to_utf8(const wchar_t* utf16_string);

std::string GetProcessExecName(const DWORD pid);

std::string GetLastErrorMessage();
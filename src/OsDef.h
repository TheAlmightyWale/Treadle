#pragma once
#ifdef __linux__

#include <unistd.h>
#include <sys/syscall.h>

#endif // __linux__

#ifdef _WIN64

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#endif
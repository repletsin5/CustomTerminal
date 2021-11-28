#pragma once
#if defined(_WIN32) && defined(TERMINAL_EXPORTS)
#define TERMINAL_API __declspec(dllexport)
#elif defined(_WIN32) && defined(TERMINAL_DLL)
#define TERMINAL_API __declspec(dllimport)
#else
#define TERMINAL_API
#endif

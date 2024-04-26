#ifdef TEST
#include <cstdio>
#include <iostream>
#endif 
#include "C_macros/assert_exit/assert_exit.h"
#include <cstdbool>
#include <vector>
using std::vector;
#include <cwchar>
#ifdef _WIN32
//#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <tlhelp32.h>
typedef HWND WindowHandle;
#elif defined(__unix__) && defined(WITH_WAYLAND)
#include <wayland-client.h>
typedef struct wl_surface* WindowHandle;
#error Currently not supported
#elif defined(__unix__) && defined(X11)
#include <X11/Xlib.h>
typedef Window WindowHandle;
#error Currently not supported
#else
#error Unsupported platform or configuration
#endif

#ifdef SAVE_OPENCV
#include <opencv2/opencv.hpp>
#elif !defined(SAVE_BMP)
#error SAVE METHOD NOT DECLARED
#endif

void * CaptureWindow(const wchar_t* exeName);

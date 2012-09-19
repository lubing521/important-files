#pragma once
#pragma warning(disable:4996)

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#ifndef WINVER
#define WINVER 0x0500
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0500
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0600
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "../../include/frame.h"
#include "../../include/irtdatasvr.h"
#include "../../include/Extend STL/UnicodeStl.h"

#pragma comment(lib, "ws2_32.lib")

#pragma once
#pragma warning(disable:4996)

#ifndef WINVER
#define WINVER 0x0500
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0502
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0500
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0600
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


#include <stdio.h>
#include <tchar.h>
#include "../../include/Extend STL/UnicodeStl.h"
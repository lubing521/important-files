// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>
#include <Shlwapi.h>


#include<stdlib.h> 
#include<crtdbg.h>

#ifdef _DEBUG   
//#define _CRTDBG_MAP_ALLOC   
//#define new new( _NORMAL_BLOCK, __FILE__, __LINE__)   
#define malloc(n) _malloc_dbg(n, _NORMAL_BLOCK, __FILE__, __LINE__)   
#endif
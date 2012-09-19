// Business.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"


#include "BusinessPlug.hpp"

__declspec(dllexport) i8desk::ISvrPlug* WINAPI CreateSvrPlug(DWORD /*Reserved*/)
{
	return new i8desk::BusinessPlug;
}
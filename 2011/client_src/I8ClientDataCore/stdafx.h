// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <afxwin.h>         // MFC 核心组件和标准组件
#include <assert.h>

inline void I8_ThrowError(LPCTSTR lpszError)
{
    MessageBox(NULL, lpszError, _T("代码错误"), 0);
    _asm { int 3 }
}
// TODO: 在此处引用程序需要的其他头文件

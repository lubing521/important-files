#ifndef __hy_inc__
#define __hy_inc__

#include <string>

bool hy_DriverProtected(wchar_t drv);
bool hy_LoadVirtualDevice(wchar_t drv);
std::wstring hy_ConvertPath(const std::wstring& path);
bool hy_CreateDirectory(const std::wstring& dir);
bool hy_GetDiskRoomSize(wchar_t drv, PLARGE_INTEGER pliSize);
bool hy_ExueteUpdateDir(LPCWSTR dir);
void hy_DyncaRefreDriver(wchar_t drv);
#endif
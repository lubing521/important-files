#ifndef __i8desk_cpu_inc__
#define __i8desk_cpu_inc__

#include <string>

namespace i8desk
{
	DWORD i8desk_GetCpuUsage();
	DWORD i8desk_GetMemoryUsage(DWORD& dwLeft);
	bool i8desk_GetDriverInfo(char chDrv, DWORD& dwType, DWORD& dwSize, 
		DWORD& dwUsedSize, DWORD& dwFreeSize);
	DWORD i8desk_GetDbLastBackupTime();
	bool i8desk_IsProcessExist(LPCTSTR szProcName);
	std::string GetServerDeviceInfo();

	DWORD i8desk_GetNetCardUsage();



	namespace internal
	{
		// 由服务端获取
		/*static const i8desk::std_string fileList[] =
		{
		_T("I8VDiskSvr.exe"),
		GAMEUPDATE_MODULE_NAME,
		SYNC_MODULE_NAME,
		VDISK_MODULE_NAME
		};*/

		static const std_string tmpFile = _T("VDiskUpdateTmp"); 
	}

#if !defined(MAKEULONGLONG)
#define MAKEULONGLONG(low,high) \
	((ULONGLONG)(((DWORD)(low)) | ((ULONGLONG)((DWORD)(high))) << 32))
#endif

	inline BOOL GetFileSize(LPCTSTR lpszFileName, ULONGLONG &ulFileSize)
	{
		WIN32_FILE_ATTRIBUTE_DATA fad = {0};
		if( ::GetFileAttributesEx(lpszFileName, GetFileExInfoStandard, &fad) ) 
		{
			ulFileSize = MAKEULONGLONG(fad.nFileSizeLow, fad.nFileSizeHigh);
			return TRUE;
		}
		return FALSE;
	}
	void GetFileCrc32(LPCTSTR lpFileName, DWORD& dwCrc32);
	

}

#endif
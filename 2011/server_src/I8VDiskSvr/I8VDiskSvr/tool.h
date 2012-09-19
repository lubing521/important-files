#ifndef __i8desk_cpu_inc__
#define __i8desk_cpu_inc__


namespace i8desk
{
	DWORD i8desk_GetCpuUsage();
	DWORD i8desk_GetMemoryUsage(unsigned __int64& dwLeft);
	bool i8desk_GetDriverInfo(TCHAR chDrv, DWORD& dwType, DWORD& dwSize, 
		DWORD& dwUsedSize, DWORD& dwFreeSize);
	DWORD i8desk_GetDbLastBackupTime();
	bool i8desk_IsProcessExist(LPCTSTR szProcName);
	stdex::tString GetServerDeviceInfo();

	DWORD i8desk_GetNetCardUsage();



	namespace internal
	{
		static const stdex::tString tmpFile = _T("VDiskUpdateTmp"); 
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
	
	bool IsLocalMachine(const std::string &ip);
}

#endif
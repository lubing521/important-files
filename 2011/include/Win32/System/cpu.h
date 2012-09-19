#ifndef __i8desk_cpu_inc__
#define __i8desk_cpu_inc__

#include <winioctl.h>
#include <psapi.h>

#pragma comment(lib, "psapi.lib")

namespace i8desk
{
#define SystemBasicInformation       0
#define SystemPerformanceInformation 2
#define SystemTimeInformation        3

#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))

	typedef struct
	{
		DWORD dwUnknown1;
		ULONG uKeMaximumIncrement;
		ULONG uPageSize;
		ULONG uMmNumberOfPhysicalPages;
		ULONG uMmLowestPhysicalPage;
		ULONG uMmHighestPhysicalPage;
		ULONG uAllocationGranularity;
		PVOID pLowestUserAddress;
		PVOID pMmHighestUserAddress;
		ULONG uKeActiveProcessors;
		BYTE bKeNumberProcessors;
		BYTE bUnknown2;
		WORD wUnknown3;
	}
	SYSTEM_BASIC_INFORMATION;

	typedef struct
	{
		LARGE_INTEGER liIdleTime;
		DWORD dwSpare[76];
	}
	SYSTEM_PERFORMANCE_INFORMATION;

	typedef struct
	{
		LARGE_INTEGER liKeBootTime;
		LARGE_INTEGER liKeSystemTime;
		LARGE_INTEGER liExpTimeZoneBias;
		ULONG uCurrentTimeZoneId;
		DWORD dwReserved;
	}
	SYSTEM_TIME_INFORMATION;


	typedef struct _THREAD_INFO
	{
		LARGE_INTEGER CreateTime;
		DWORD dwUnknown1;
		DWORD dwStartAddress;
		DWORD StartEIP;
		DWORD dwOwnerPID;
		DWORD dwThreadId;
		DWORD dwCurrentPriority;
		DWORD dwBasePriority;
		DWORD dwContextSwitches;
		DWORD Unknown;
		DWORD WaitReason;

	}
	THREADINFO, *PTHREADINFO;

	typedef struct _UNICODE_STRING
	{
		USHORT Length;
		USHORT MaxLength;
		PWSTR Buffer;
	}
	UNICODE_STRING;

	typedef struct _PROCESS_INFO
	{
		DWORD dwOffset;
		DWORD dwThreadsCount;
		DWORD dwUnused1[6];
		LARGE_INTEGER CreateTime;
		LARGE_INTEGER UserTime;
		LARGE_INTEGER KernelTime;
		UNICODE_STRING ProcessName;

		DWORD dwBasePriority;
		DWORD dwProcessID;
		DWORD dwParentProcessId;
		DWORD dwHandleCount;
		DWORD dwUnused3[2];

		DWORD dwVirtualBytesPeak;
		DWORD dwVirtualBytes;
		ULONG dwPageFaults;
		DWORD dwWorkingSetPeak;
		DWORD dwWorkingSet;
		DWORD dwQuotaPeakPagedPoolUsage;
		DWORD dwQuotaPagedPoolUsage;
		DWORD dwQuotaPeakNonPagedPoolUsage;
		DWORD dwQuotaNonPagedPoolUsage;
		DWORD dwPageFileUsage;
		DWORD dwPageFileUsagePeak;

		DWORD dCommitCharge;
		THREADINFO ThreadSysInfo[1];

	}
	PROCESSINFO, *PPROCESSINFO;

	inline int GetPIDByName(LPCTSTR name)
	{
		PROCESSENTRY32 pe32;
		//   在使用这个结构之前，先设置它的大小
		pe32.dwSize = sizeof(pe32);

		//   给系统内的所有进程拍一个快照
		HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
			return -1;

		//   遍历进程快照，轮流显示每个进程的信息
		BOOL bMore = ::Process32First(hProcessSnap, &pe32);
		while (bMore)
		{
			if (_tcsicmp(pe32.szExeFile, name) == 0)
			{
				return pe32.th32ProcessID;
				::CloseHandle(hProcessSnap);
			}

			bMore = ::Process32Next(hProcessSnap, &pe32);
		}
		//   不要忘记清除掉snapshot对象
		::CloseHandle(hProcessSnap);
		return -1;
	}

	inline DWORD i8desk_GetProcessCpuUsage(DWORD id)
	{
		DWORD time1 = ::GetTickCount();
		ULONGLONG kernelTime1 = 0, userTime1 = 0;
		ULONGLONG createTime1 = 0, exitTime1 = 0;
		HANDLE handle = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, id);
		if( handle == INVALID_HANDLE_VALUE )
			return 0;

		::GetProcessTimes(handle, (FILETIME *)&createTime1, (FILETIME *)&exitTime1, (FILETIME *)&kernelTime1, (FILETIME *)&userTime1);

		::Sleep(100);

		DWORD time2 = ::GetTickCount();
		ULONGLONG kernelTime2 = 0, userTime2 = 0;
		ULONGLONG createTime2 = 0, exitTime2 = 0;
		::GetProcessTimes(handle, (FILETIME *)&createTime2, (FILETIME *)&exitTime2, (FILETIME *)&kernelTime2, (FILETIME *)&userTime2);


		ULONGLONG idle = 0, kernel = 0, user = 0;
		idle = time2 - time1;
		kernel = kernelTime2 - kernelTime1;
		user = userTime2 - userTime1 + 1;

		return (DWORD)((kernel + user) / 1000 / idle);
	}

	inline DWORD i8desk_GetCpuUsage()
	{
		DWORD dwCpuUsges = 0;
		static LARGE_INTEGER m_liOldIdleTime = {0};
		static LARGE_INTEGER m_liOldSystemTime = {0};
		SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;
		SYSTEM_TIME_INFORMATION SysTimeInfo;
		SYSTEM_BASIC_INFORMATION SysBaseInfo;
		double dbIdleTime;
		double dbSystemTime;
		LONG status;
		typedef LONG (WINAPI *PROCNTQSI)(UINT, PVOID, ULONG, PULONG);
		PROCNTQSI NtQuerySystemInformation;

		NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")),
			"NtQuerySystemInformation");

		if (!NtQuerySystemInformation)
			return 0;

		// get number of processors in the system
		status = NtQuerySystemInformation(SystemBasicInformation,
			&SysBaseInfo, sizeof(SysBaseInfo), NULL);

		if (status != NO_ERROR)
			return 0;

		status = NtQuerySystemInformation(SystemTimeInformation,
			&SysTimeInfo, sizeof(SysTimeInfo), 0);
		if (status != NO_ERROR)
			return 0;

		// get new CPU's idle time
		status = NtQuerySystemInformation(SystemPerformanceInformation,
			&SysPerfInfo, sizeof(SysPerfInfo), NULL);

		if (status != NO_ERROR)
			return 0;

		// if it's a first call - skip it
		if (m_liOldIdleTime.QuadPart != 0)
		{
			// CurrentValue = NewValue - OldValue
			dbIdleTime = Li2Double(SysPerfInfo.liIdleTime) - Li2Double(m_liOldIdleTime);
			dbSystemTime = Li2Double(SysTimeInfo.liKeSystemTime) - Li2Double(m_liOldSystemTime);

			// CurrentCpuIdle = IdleTime / SystemTime
			dbIdleTime = dbIdleTime / dbSystemTime;

			// CurrentCpuUsage% = 100 - (CurrentCpuIdle * 100) / NumberOfProcessors
			dbIdleTime = 100.0 - dbIdleTime * 100.0 / (double)SysBaseInfo.bKeNumberProcessors + 0.5;

			dwCpuUsges = (UINT)dbIdleTime;
		}

		// store new CPU's idle and system time
		m_liOldIdleTime = SysPerfInfo.liIdleTime;
		m_liOldSystemTime = SysTimeInfo.liKeSystemTime;
		return dwCpuUsges;
	}

	inline DWORD i8desk_GetMemoryUsage()
	{
		MEMORYSTATUS memory = {sizeof(MEMORYSTATUS)};
		GlobalMemoryStatus(&memory);
		return memory.dwMemoryLoad;
	}

	inline bool i8desk_GetDriverInfo(TCHAR chDrv, DWORD& dwType, ulonglong& dwSize,
		ulonglong& dwUsedSize, ulonglong& dwFreeSize)
	{
		TCHAR drv[] = _T("X:");
		drv[0] = chDrv;

		TCHAR szSysName[MAX_PATH] = {0};
		GetVolumeInformation(drv, NULL, 0, NULL, 0, NULL, szSysName, MAX_PATH);
		if (lstrcmpi(szSysName, _T("NTFS")) == 0)
			dwType = 2;
		else if (lstrcmpi(szSysName, _T("FAT32")) == 0)
			dwType = 1;
		else
			dwType = 0;

		dwSize = dwUsedSize = dwFreeSize = 0;
		ULARGE_INTEGER lSize = {0}
		, lFree = {0};
		GetDiskFreeSpaceEx(drv, &lFree, &lSize, NULL);
		dwSize = lSize.QuadPart / 1024;
		dwFreeSize = lFree.QuadPart / 1024;
		dwUsedSize = dwSize - dwFreeSize;
		return true;
	}

	inline DWORD i8desk_GetDbLastBackupTime()
	{
		TCHAR szFile[MAX_PATH] = {0};
		GetModuleFileName(NULL, szFile, sizeof(szFile));
		PathRemoveFileSpec(szFile);
		PathAddBackslash(szFile);
		lstrcat(szFile, TEXT("Data\\Backup.ini"));
		return GetPrivateProfileInt(TEXT("System"), TEXT("LastBackupDate"), 0, szFile);
	}

	//for example:"i8desksvr.exe"
	inline bool i8desk_IsProcessExist(LPCTSTR szProcName)
	{
		DWORD aProcesses[1024] = {0}, cbNeeded, cProcesses;
		TCHAR path[MAX_PATH] = {0};
		if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
			return false;
		cProcesses = cbNeeded / sizeof(DWORD);
		for (DWORD i = 0; i < cProcesses; i++ )
		{
			HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE, FALSE, aProcesses[i]);
			if (hProcess != NULL)
			{
				GetModuleBaseName(hProcess, NULL, path, MAX_PATH);
				if (lstrcmpi(path, szProcName) == 0)
				{
					CloseHandle(hProcess);
					return true;
				}
				CloseHandle(hProcess);
			}
		}
		return false;
	}
}

#endif
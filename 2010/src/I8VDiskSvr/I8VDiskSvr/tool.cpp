#include "stdafx.h"
#include "tool.h"
#include <winioctl.h>
#include <Shlwapi.h>
#include <psapi.h>
#include <wbemidl.h>
#include <comdef.h>
#include <atlenc.h>
#include <IPHlpApi.h>
#include <wbemidl.h>
#include <vector>
#include <string>
#include <sstream>


#include "../../../include/Utility/SmartHandle.hpp"
#include "../../../include/Utility/SmartPtr.hpp"
#include "../../../include/Utility/utility.h"

using namespace std;

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "Wbemuuid")
#pragma comment(lib, "iphlpapi.lib")

namespace i8desk
{
#define SystemBasicInformation       0
#define SystemPerformanceInformation 2
#define SystemTimeInformation        3

#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))

	typedef struct
	{
		DWORD   dwUnknown1;
		ULONG   uKeMaximumIncrement;
		ULONG   uPageSize;
		ULONG   uMmNumberOfPhysicalPages;
		ULONG   uMmLowestPhysicalPage;
		ULONG   uMmHighestPhysicalPage;
		ULONG   uAllocationGranularity;
		PVOID   pLowestUserAddress;
		PVOID   pMmHighestUserAddress;
		ULONG   uKeActiveProcessors;
		BYTE    bKeNumberProcessors;
		BYTE    bUnknown2;
		WORD    wUnknown3;
	} SYSTEM_BASIC_INFORMATION;

	typedef struct
	{
		LARGE_INTEGER   liIdleTime;
		DWORD           dwSpare[76];
	} SYSTEM_PERFORMANCE_INFORMATION;

	typedef struct
	{
		LARGE_INTEGER liKeBootTime;
		LARGE_INTEGER liKeSystemTime;
		LARGE_INTEGER liExpTimeZoneBias;
		ULONG         uCurrentTimeZoneId;
		DWORD         dwReserved;
	} SYSTEM_TIME_INFORMATION;

	DWORD i8desk_GetCpuUsage()
	{
		DWORD dwCpuUsges = 0;
		static LARGE_INTEGER   m_liOldIdleTime = {0};
		static LARGE_INTEGER   m_liOldSystemTime = {0};
		SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;
		SYSTEM_TIME_INFORMATION        SysTimeInfo;
		SYSTEM_BASIC_INFORMATION       SysBaseInfo;
		double                         dbIdleTime;
		double                         dbSystemTime;
		LONG                           status;
		typedef LONG (WINAPI *PROCNTQSI)(UINT,PVOID,ULONG,PULONG);
		PROCNTQSI NtQuerySystemInformation;

		NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")),
			"NtQuerySystemInformation");

		if (!NtQuerySystemInformation)
			return 0;

		// get number of processors in the system
		status = NtQuerySystemInformation(SystemBasicInformation,
			&SysBaseInfo,sizeof(SysBaseInfo),NULL);

		if (status != NO_ERROR)
			return 0;

		status = NtQuerySystemInformation(SystemTimeInformation,
			&SysTimeInfo,sizeof(SysTimeInfo),0);
		if (status!=NO_ERROR)
			return 0;

		// get new CPU's idle time
		status = NtQuerySystemInformation(SystemPerformanceInformation,
			&SysPerfInfo,sizeof(SysPerfInfo),NULL);

		if (status != NO_ERROR)
			return 0;

		// if it's a first call - skip it
		if (m_liOldIdleTime.QuadPart != 0)
		{
			// CurrentValue = NewValue - OldValue
			dbIdleTime   = Li2Double(SysPerfInfo.liIdleTime) - Li2Double(m_liOldIdleTime);
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

	DWORD i8desk_GetMemoryUsage(DWORD& dwLeft)
	{
		MEMORYSTATUS memory = {sizeof(MEMORYSTATUS)};
		GlobalMemoryStatus(&memory);
		dwLeft = memory.dwAvailPhys / 1024;
		return memory.dwMemoryLoad;
	}

	bool i8desk_GetDriverInfo(char chDrv, DWORD& dwType, DWORD& dwSize, 
		DWORD& dwUsedSize, DWORD& dwFreeSize)
	{
		char drv[] = "X:\\";
		drv[0] = chDrv;

		dwType = 0;	//δ֪
		char szSysName[MAX_PATH] = {0};
		if (GetVolumeInformationA(drv, NULL, 0, NULL, 0, NULL, szSysName, MAX_PATH))
		{
			if (lstrcmpiA(szSysName, "NTFS") == 0)
				dwType = 2;
			else if (lstrcmpiA(szSysName, "FAT32") == 0)
				dwType = 1;
		}

		dwSize = dwUsedSize = dwFreeSize = 0;
		ULARGE_INTEGER lSize = {0}, lFree = {0};
		if (GetDiskFreeSpaceEx(drv, NULL, &lSize, &lFree))
		{
			dwSize = DWORD(lSize.QuadPart / 1024);
			dwFreeSize = DWORD(lFree.QuadPart / 1024);
			if (dwSize < dwFreeSize)
				dwSize = dwFreeSize = 0;
			dwUsedSize = dwSize - dwFreeSize;
		}
		return true;
	}

	DWORD i8desk_GetDbLastBackupTime()
	{
		TCHAR szFile[MAX_PATH] = {0};
		GetModuleFileName(NULL, szFile, sizeof(szFile));
		PathRemoveFileSpec(szFile);
		PathAddBackslash(szFile);
		lstrcat(szFile, TEXT("Data\\Backup.ini"));
		return GetPrivateProfileInt(TEXT("System"), TEXT("LastBackupDate"), 0, szFile); 
	}

	//for example:"i8desksvr.exe"
	bool i8desk_IsProcessExist(LPCTSTR szProcName)
	{
		DWORD aProcesses[1024] = {0}, cbNeeded, cProcesses;
		char path[MAX_PATH] = {0};
		if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
			return	false;
		cProcesses = cbNeeded / sizeof(DWORD);
		for (DWORD i = 0; i < cProcesses; i++ )
		{
			HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|PROCESS_TERMINATE, FALSE, aProcesses[i]);
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

	string handleStr(LPCSTR src)
	{
		string dst = src;
		for (size_t idx=0; idx<dst.size(); idx++)
		{
			if (dst[idx] == ',' || dst[idx] == '|')
			{
				dst[idx] = ' ';
			}
		}
		return dst;
	}

	string handleStr(string str)
	{
		return handleStr(str.c_str());
	}

	_bstr_t GetDevInfoItem(IWbemServices* pService, LPCSTR lpszQuery, LPCSTR lpszField)
	{
		CComVariant variant("");
		IEnumWbemClassObject *pEnumObject  = NULL;
		if (S_OK == pService->ExecQuery(CComBSTR("WQL"), CComBSTR(lpszQuery), WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumObject))
		{
			ULONG dwCount = 1, dwRetSize;
			IWbemClassObject *pClassObject = NULL;
			if(S_OK == pEnumObject->Reset())
			{
				if (S_OK == pEnumObject->Next(5000, dwCount, &pClassObject, &dwRetSize))
				{
					pClassObject->Get(CComBSTR(lpszField),  0, &variant , NULL , 0);
					pClassObject->Release();
				}
			}
			pEnumObject->Release();
		}

		return _bstr_t(variant);
	}

	string GetMacAddr(string& description)
	{
		description.clear();

		TCHAR szMac[20] = TEXT("00-00-00-00-00-00");
		IP_ADAPTER_INFO adapter[10] = {0};
		DWORD dwSize = sizeof(adapter);
		if (ERROR_SUCCESS != GetAdaptersInfo(adapter, &dwSize))
			return szMac;
		
		PIP_ADAPTER_INFO pAdapter = adapter;
		while (pAdapter!= NULL)
		{
			if (pAdapter->Type == MIB_IF_TYPE_ETHERNET && strstr(pAdapter->Description, "VMware") == NULL)
			{
				_stprintf_s(szMac, TEXT("%02X-%02X-%02X-%02X-%02X-%02X"),
					pAdapter->Address[0],
					pAdapter->Address[1],
					pAdapter->Address[2],
					pAdapter->Address[3],
					pAdapter->Address[4],
					pAdapter->Address[5]);
				description = pAdapter->Description;
				break;
			}

			pAdapter = pAdapter->Next;
		}
		return string(szMac);
	}

	string GetServerDeviceInfo()
	{
		CoInitialize(NULL);
		HRESULT hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, 
			RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);

		string sDevInfo;
		IWbemLocator  *pIWbemLocator = NULL;
		IWbemServices *pWbemServices = NULL;
		if (S_OK != CoCreateInstance (CLSID_WbemAdministrativeLocator, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown ,(void**)&pIWbemLocator))
			return sDevInfo;

		if (S_OK != pIWbemLocator->ConnectServer(CComBSTR("root\\cimv2"),  NULL, NULL, NULL, 0, NULL, NULL, &pWbemServices))
		{
			pIWbemLocator->Release();
			return sDevInfo;
		}
		hr = CoSetProxyBlanket(pWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL,
			RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);


		string freespace;
		{
			stringstream stream;
			char buf[] = "X:\\";
			for (char idx='C'; idx<='Z'; idx++)
			{
				buf[0] = idx;
				if (GetDriveType(buf) == DRIVE_FIXED)
				{
					ULARGE_INTEGER ui = {0};
					ULARGE_INTEGER ui2 = {0};
					GetDiskFreeSpaceEx(buf, &ui2, &ui, NULL);
					char sSize[10] = {0};
					char sSize2[10] = {0};
					sprintf(sSize, "%.1f", ui.QuadPart / 1000.0 / 1000 / 1000);
					sprintf(sSize2, "%.1f", ui2.QuadPart / 1000.0 / 1000 / 1000);
					stream << idx << ":" << sSize << "*" << sSize2 << "-";
				}
			}
			freespace = stream.str();
			if (freespace.size())
				freespace.resize(freespace.size()-1);
		}

		//mac,virtualdisk,cpu,memory,disk,freespace,graphics,mainboad,nic,webcam,systemver
		string nic;
		string mac		 = GetMacAddr(nic);
		string cpu		 = GetDevInfoItem(pWbemServices, "Select * from Win32_processor", "Name");
		string memory	 = GetDevInfoItem(pWbemServices, "Select * from Win32_LogicalMemoryConfiguration", "TotalPhysicalMemory");
		string disk		 = GetDevInfoItem(pWbemServices, "Select * from Win32_DiskDrive", "Model");		 
		string graphics	 = GetDevInfoItem(pWbemServices, "Select * from Win32_VideoController", "Name");
		string mainboard = GetDevInfoItem(pWbemServices, "Select * from Win32_BaseBoard", "Product");
		//string nic;
		string webcam	 = "";
		string sysver    = GetDevInfoItem(pWbemServices, "Select * from Win32_OperatingSystem", "Version");

		stringstream out;
		out << handleStr(mac) <<"," << "1" <<"," << handleStr(cpu) <<", " 
			<< _atoi64(memory.c_str()) / 1000 /1000 <<", " 
			<< handleStr(disk) <<", " << handleStr(freespace)  <<", " << handleStr(graphics) <<", " 
			<< handleStr(mainboard) <<", "<< handleStr(nic) <<", " 
			<< handleStr(webcam) <<", " << handleStr(sysver);

		pWbemServices->Release();
		pIWbemLocator->Release();
		sDevInfo = out.str();
		return sDevInfo;
	}

	typedef struct tagNetcard
	{
		char	MAC[64];
		char	Desciption[128];
		DWORD	usage;
		DWORD   index;
		DWORD   preInBytes;
		DWORD   preOutBytes;
	}tagNetcard;

	DWORD Init_NetCard(tagNetcard* pNetcard)
	{
		DWORD dwSize = 0x10000;
		PMIB_IFTABLE pIfTab = (PMIB_IFTABLE)new char[dwSize];

		if (GetIfTable(pIfTab, &dwSize, 0) != NO_ERROR)
		{
			delete []((char*)pIfTab);
			return 0;
		}

		DWORD dwIndex = 0;
		for (DWORD i=0; i<pIfTab->dwNumEntries; i++)
		{
			MIB_IFROW* pMib = &pIfTab->table[i];
			if (pMib->dwPhysAddrLen != 6)
				continue;

			tagNetcard* pNet = &pNetcard[dwIndex];
			_stprintf(pNet->MAC, "%02X:%02X:%02X:%02X:%02X:%02X", 
				pMib->bPhysAddr[0], 
				pMib->bPhysAddr[1],
				pMib->bPhysAddr[2],
				pMib->bPhysAddr[3],
				pMib->bPhysAddr[4],
				pMib->bPhysAddr[5]);
			lstrcpy(pNet->Desciption, (LPCSTR)pMib->bDescr);
			pNet->usage = 0;
			pNet->index = pMib->dwIndex;
			pNet->preInBytes = pMib->dwInOctets;
			pNet->preOutBytes = pMib->dwOutOctets;

			dwIndex++;
		}
		delete []((char*)pIfTab);
		return dwIndex;
	}

	BOOL PopulateNetworkUtilization(tagNetcard* pNetcard, DWORD dwCount)
	{
		DWORD dwSize = 0x10000;
		PMIB_IFTABLE pIfTab = (PMIB_IFTABLE)new char[dwSize];

		if (GetIfTable(pIfTab, &dwSize, 0) != NO_ERROR)
		{
			delete []((char*)pIfTab);
			return FALSE;
		}

		for (DWORD i=0; i<pIfTab->dwNumEntries; i++)
		{
			MIB_IFROW* pMib = &pIfTab->table[i];
			if (pMib->dwPhysAddrLen != 6)
				continue;

			for (DWORD j=0; j<dwCount;j++)
			{
				if (pNetcard[j].index == pMib->dwIndex)
				{
					DWORD inBytesDet = 0;
					DWORD outBytesDet = 0;
					
					tagNetcard* pNet = &pNetcard[j];

					// if it isn't first call
					if (pNet->preInBytes != 0 && pNet->preOutBytes != 0)
					{
						// Deal with the overflow case
						inBytesDet = (pNet->preInBytes >pMib->dwInOctets) 
							? 0xFFFFFFFF - pNet->preInBytes + pMib->dwInOctets
							: pIfTab->table[i].dwInOctets - pNet->preInBytes;
						outBytesDet = (pNet->preOutBytes > pMib->dwOutOctets)
							?0xFFFFFFFF - pNet->preOutBytes + pMib->dwOutOctets
							: pIfTab->table[i].dwOutOctets - pNet->preOutBytes;
					}
					pNet->preInBytes = pMib->dwInOctets;
					pNet->preOutBytes = pMib->dwOutOctets;
					unsigned __int64 totalBytesDet = ((unsigned __int64)inBytesDet + (unsigned __int64)outBytesDet) * 8;
					unsigned int usage = (unsigned int) ((totalBytesDet >pMib->dwSpeed) 
						? 100 : (totalBytesDet * 100 / pMib->dwSpeed));
					pNet->usage = usage;
					
					break;
				}
			}
		}
		delete []((char*)pIfTab);
		return TRUE;
	}

	DWORD i8desk_GetNetCardUsage()
	{
		tagNetcard NetCard[10] = {0};

		DWORD dwSize = Init_NetCard(NetCard);
		if (dwSize<=0)
			return 0;

		PopulateNetworkUtilization(NetCard, dwSize);
		Sleep(1000);

		if (PopulateNetworkUtilization(NetCard, dwSize))
		{
			DWORD dwRet = 0;
			for (DWORD idx=0; idx<dwSize; idx++)
			{
				if (NetCard[idx].usage > dwRet)
				{
					dwRet = NetCard[idx].usage;
				}
			}
			return dwRet;
		}
		return 0;
	}


	void GetFileCrc32(LPCTSTR lpFileName, DWORD& dwCrc32)
	{
		CAutoFile hFile= ::CreateFile(lpFileName,           // open MYFILE.TXT 
			GENERIC_READ,              // open for reading 
			FILE_SHARE_READ,           // share for reading 
			NULL,                      // no security 
			OPEN_EXISTING,             // existing file only 
			FILE_ATTRIBUTE_NORMAL,     // normal file 
			NULL);                     // no attr. template 

		if( !hFile.IsValid() )
			return;

		DWORD dwSize = ::GetFileSize(hFile, NULL);
		if( dwSize == INVALID_FILE_SIZE )
			return;

		CAutoArrayPtr<BYTE>::AutoPtr lpFileBuf(new BYTE[dwSize]);

		if( !ReadFileContent(hFile, lpFileBuf, dwSize) )
			return;

		dwCrc32 = CalBufCRC32(lpFileBuf, dwSize);
	}
}
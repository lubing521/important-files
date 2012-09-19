#include "stdafx.h"
#include "logrpt.h"

#include <WinInet.h>
#include <tchar.h>
#include <Shlwapi.h>
#include <psapi.h>
#include <Iphlpapi.h>
#include <wbemidl.h>
#include <atlenc.h>
#include <algorithm>
#include <functional>

#include <atlbase.h>
#include <atlutil.h>

#include "../../include/Utility/SmartHandle.hpp"

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Version.lib")
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "Wbemuuid")
#pragma comment(lib, "Wininet.lib")

namespace i8desk
{

stdex::tString uri_encode(stdex::tString& source)
{
	std::string res;
	char element[10] = {0};
	unsigned short chr = 0;

	std::string src = CT2A(source.c_str());
	for (size_t i=0; i<src.size(); i++)
	{
		chr = src[i];
		if (chr > 127)		//汉字
		{
			sprintf(element, "%%%02X", chr);
			res += element;
		}
		else if (isalnum(chr) || isdigit(chr))
		{
			res += chr;
		}
		else
		{
			sprintf(element, "%%%02X", chr); 
			res += element;
		}
	}	

	return stdex::tString(CA2T(res.c_str()));
//	return res;

	/*TCHAR buf[1024] = {0};
	DWORD dwLen = 0;
	BOOL suc = AtlEscapeUrl(_T("计"), buf, &dwLen, 1024);
	assert(suc);

	return buf;*/


	//const char DEC2HEX[16 + 1] = "0123456789ABCDEF";
	//const unsigned char * pSrc = (const unsigned char *)sSrc.c_str();
	//const int SRC_LEN = sSrc.length();
	//unsigned char * const pStart = new unsigned char[SRC_LEN * 3];
	//unsigned char * pEnd = pStart;
	//const unsigned char * const SRC_END = pSrc + SRC_LEN;

	//for (; pSrc < SRC_END; ++pSrc)
	//{
	//	if (SAFE[*pSrc]) 
	//		*pEnd++ = *pSrc;
	//	else
	//	{
	//		// escape this char
	//		*pEnd++ = '%';
	//		*pEnd++ = DEC2HEX[*pSrc >> 4];
	//		*pEnd++ = DEC2HEX[*pSrc & 0x0F];
	//	}
	//}

	//std::string sResult((char *)pStart, (char *)pEnd);
	//delete [] pStart;

}

void GetSvrMacAddr(tagUserInfo* pUserInfo)
{
	TCHAR szMac[20] = TEXT("00-00-00-00-00-00");
	IP_ADAPTER_INFO adapter[10] = {0};
	DWORD dwSize = sizeof(adapter);
	if (ERROR_SUCCESS == GetAdaptersInfo(adapter, &dwSize))
	{
		PIP_ADAPTER_INFO pAdapter = adapter;
		while (pAdapter!= NULL)
		{
			if (pAdapter->Type == MIB_IF_TYPE_ETHERNET && strstr(pAdapter->Description, "VMware") == NULL)
			{
				_stprintf_s(szMac, TEXT("%02X-%02X-%02X-%02X-%02X-%02X"),
					pAdapter->Address[0], pAdapter->Address[1], pAdapter->Address[2], 
					pAdapter->Address[3], pAdapter->Address[4], pAdapter->Address[5]);
				
				pUserInfo->szSrvMac = szMac;
				pUserInfo->szNic	= _bstr_t(pAdapter->Description);
				return;
			}
			pAdapter = pAdapter->Next;
		}
	}
	pUserInfo->szSrvMac = szMac;
}

stdex::tString GetFileVersion(IN LPCTSTR pFileName)
{
	stdex::tString Version(TEXT("0.0.0."));
	DWORD dwSize = GetFileVersionInfoSize(pFileName, NULL);
	if(dwSize)
	{
		LPTSTR pblock = new TCHAR[dwSize+1];
		GetFileVersionInfo(pFileName, 0, dwSize, pblock);
		UINT nQuerySize;
		DWORD* pTransTable = NULL;
		VerQueryValue(pblock, TEXT("\\VarFileInfo\\Translation"), (void **)&pTransTable, &nQuerySize);
		LONG m_dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));
		TCHAR SubBlock[MAX_PATH] = {0};
		_stprintf(SubBlock, TEXT("\\StringFileInfo\\%08lx\\FileVersion"), m_dwLangCharset);	
		LPTSTR lpData;
		if (VerQueryValue(pblock, SubBlock, (PVOID*)&lpData, &nQuerySize))
			Version = lpData;
		delete[] pblock;
	}

	// replace ',' to '.'
	std::replace_if(Version.begin(), Version.end(), 
		std::bind2nd(std::equal_to<stdex::tString::value_type>(), TEXT(',')), TEXT('.'));

	// delete [Space]
	stdex::tString::iterator iter = std::remove_if(Version.begin(), Version.end(),
		std::bind2nd(std::equal_to<stdex::tString::value_type>(), TEXT(' ')));

	// remove redundant character
	if( iter != Version.end() )
		Version.erase(iter, Version.end());

	return Version;
}

stdex::tString GetAppPath()
{
	TCHAR path[MAX_PATH] = {0};
	GetModuleFileName(NULL, path, _countof(path));
	PathRemoveFileSpec(path);
	PathAddBackslash(path);
	return stdex::tString(path);
}

void splitString(const stdex::tString &strSource, std::vector<stdex::tString>& plugs, TCHAR ch)
{
	size_t first = 0;
	size_t second = 0;
	do
	{
		second = strSource.find_first_of(ch, first);
		if (second != stdex::tString::npos)
		{
			plugs.push_back(strSource.substr(first, second-first));
			first = second + 1;
		}
		else
		{
			if (first< strSource.size())
			{
				plugs.push_back(strSource.substr(first));
			}
		}
	}while (second != stdex::tString::npos);
}


int GetOemId()
{
	stdex::tString szSysConfig = GetAppPath() + TEXT("oem\\oem.ini");
	stdex::tString szDestFile  = GetAppPath() + TEXT("Data\\oem.ini");

	SetFileAttributes(szDestFile.c_str(), FILE_ATTRIBUTE_NORMAL);
	DeleteFile(szDestFile.c_str());

	utility::CAutoFile hFile = CreateFile(szSysConfig.c_str(), GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, 0, NULL);
	if (hFile.IsValid())
	{
		try
		{
			DWORD dwSize = GetFileSize(hFile, NULL);
			ATL::CAutoVectorPtr<char> pSrc(new char[dwSize]);
			DWORD dwReadBytes = 0;
			if (!ReadFile(hFile, pSrc, dwSize, &dwReadBytes, NULL))
				return -1;

			int dwNewSize = dwSize;
			ATL::CAutoVectorPtr<BYTE> pDst(new BYTE[dwNewSize]);
			ATL::Base64Decode(pSrc, dwSize, pDst, &dwNewSize);


			utility::CAutoFile hFileDest = CreateFile(szDestFile.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, 0, NULL);
			if (hFileDest.IsValid())
			{
				SetFileAttributes(szDestFile.c_str(), FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_SYSTEM);
				WriteFile(hFileDest, pDst, dwNewSize, &dwReadBytes, NULL);

				int Id = GetPrivateProfileInt(TEXT("OEM"), TEXT("oemid"), -1, szDestFile.c_str());
				DeleteFile(szDestFile.c_str());
				return Id;
			}

		}
		catch (...) { }
	}
	return -1;
}

DWORD IsSystemType()
{
	TCHAR buf[10] = {0};
	DWORD dwType = 0;
	DWORD dwSize = sizeof(buf) - 1;
	SHGetValue(HKEY_LOCAL_MACHINE, TEXT("Software\\Goyoo\\i8desk"), TEXT("Entironment"), &dwType, buf, &dwSize);
	return _ttoi(buf) == 1 ? 1 : 0;
}

DWORD CheckProcess(LPCTSTR lpszProcName)
{
	DWORD aProcesses[1024] = {0}, cbNeeded, cProcesses;
	TCHAR path[MAX_PATH] = {0};
	if (!EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ))
		return	0;
	cProcesses = cbNeeded / sizeof(DWORD);
	for (DWORD i = 0; i < cProcesses; i++ )
	{
		utility::CAutoHandle hProcess = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|PROCESS_TERMINATE, FALSE, aProcesses[i]);
		if (hProcess != NULL && GetCurrentProcessId() != aProcesses[i])
		{
			GetModuleBaseName(hProcess, NULL, path, MAX_PATH);
			if (lstrcmpi(path, lpszProcName) == 0)
			{
				return 1;
			}
		}
	}
	return 0;
}

stdex::tString handleStr(stdex::tString &src)
{
	stdex::tString dst = src;
	for (size_t idx=0; idx<dst.size(); idx++)
	{
		if (dst[idx] == TEXT(',') || dst[idx] == TEXT('|'))
		{
			dst[idx] = TEXT(' ');
		}
	}
	return dst;
}

_bstr_t GetDevInfoItem(IWbemServices* pService, LPCTSTR lpszQuery, LPCTSTR lpszField)
{
	CComVariant variant(TEXT(""));
	IEnumWbemClassObject *pEnumObject  = NULL;
	if (S_OK == pService->ExecQuery(CComBSTR(TEXT("WQL")), CComBSTR(lpszQuery), WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumObject))
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

stdex::tString GetServerDeviceInfo(tagUserInfo* pUserInfo)
{
	HRESULT hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, 
		RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);

	stdex::tString sDevInfo;
	IWbemLocator  *pIWbemLocator = NULL;
	IWbemServices *pWbemServices = NULL;
	if (S_OK != CoCreateInstance (CLSID_WbemAdministrativeLocator, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown ,(void**)&pIWbemLocator))
		return sDevInfo;

	if (S_OK != pIWbemLocator->ConnectServer(CComBSTR(TEXT("root\\cimv2")),  NULL, NULL, NULL, 0, NULL, NULL, &pWbemServices))
	{
		pIWbemLocator->Release();
		return sDevInfo;
	}
	hr = CoSetProxyBlanket(pWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL,
		RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

	stdex::tString freespace;
	{
		stdex::tStringstream stream;
		TCHAR buf[] = TEXT("X:\\");
		for (TCHAR idx = TEXT('C'); idx<= TEXT('Z'); idx++)
		{
			buf[0] = idx;
			if (GetDriveType(buf) == DRIVE_FIXED)
			{
				ULARGE_INTEGER ui = {0};
				ULARGE_INTEGER ui2 = {0};
				GetDiskFreeSpaceEx(buf, &ui2, &ui, NULL);
				TCHAR sSize[20] = {0};
				TCHAR sSize2[20] = {0};
				_stprintf(sSize,  TEXT("%.1f"), ui.QuadPart / 1000.0 / 1000 / 1000);
				_stprintf(sSize2, TEXT("%.1f"), ui2.QuadPart / 1000.0 / 1000 / 1000);
				stream << idx << TEXT(":") << sSize << TEXT("*") << sSize2 << TEXT("-");
			}
		}
		freespace = stream.str();
		if (freespace.size())
			freespace.resize(freespace.size()-1);
	}

	try
	{
		stdex::tString cpu		 = GetDevInfoItem(pWbemServices, TEXT("Select * from Win32_processor"),			TEXT("Name"));
		stdex::tString memory	 = GetDevInfoItem(pWbemServices, TEXT("Select * from Win32_LogicalMemoryConfiguration"), TEXT("TotalPhysicalMemory"));
		stdex::tString disk	 = GetDevInfoItem(pWbemServices, TEXT("Select * from Win32_DiskDrive"),			TEXT("Model"));
		stdex::tString graphics = GetDevInfoItem(pWbemServices, TEXT("Select * from Win32_VideoController"),	TEXT("Name"));
		stdex::tString mainboard= GetDevInfoItem(pWbemServices, TEXT("Select * from Win32_BaseBoard"),			TEXT("Product"));
		stdex::tString nic		 = pUserInfo->szNic;
		stdex::tString webcam	 = TEXT("");
		stdex::tString sysver    = GetDevInfoItem(pWbemServices, TEXT("Select * from Win32_OperatingSystem"),	TEXT("Version"));

		stdex::tStringstream out;
		out << handleStr(cpu)		<< TEXT("|*|") 
			<< _ttoi64(memory.c_str()) / 1000 /1000 << TEXT("|*|")
			<< handleStr(disk)		<< TEXT("|*|")
			<< handleStr(freespace) << TEXT("|*|")
			<< handleStr(graphics)	<< TEXT("|*|")
			<< handleStr(mainboard) << TEXT("|*|")
			<< handleStr(nic)		<< TEXT("|*|")
			<< handleStr(webcam)	<< TEXT("|*|")
			<< handleStr(sysver);

		pWbemServices->Release();
		pIWbemLocator->Release();
		sDevInfo = out.str();
	}
	catch(...) { }

	return sDevInfo;
}

stdex::tString GetNodeSvrDeviceInfo(ISvrPlugMgr* pPlugMgr)
{
		ISvrPlug* pRTPlug = pPlugMgr->FindPlug(PLUG_RTDATASVR_NAME);
		IRTDataSvr* pRTDatabase = (pRTPlug == NULL) ? NULL :
			reinterpret_cast<IRTDataSvr*>(pRTPlug->SendMessage(RTDS_CMD_GET_INTERFACE, RTDS_INTERFACE_RTDATASVR, 0));
		if (pRTDatabase == NULL)
			return TEXT("");

		IServerStatusRecordset *pRs = NULL;
		pRTDatabase->GetServerStatusTable()->Select(&pRs, NULL, NULL);
		if (pRs == NULL)
			return TEXT("");

		TCHAR szCmpName[MAX_PATH] = {0};
		DWORD dwSize = _countof(szCmpName) - 1;
		GetComputerName(szCmpName, &dwSize);
		stdex::tString str;
		for (DWORD idx=0; idx<pRs->GetCount(); idx++)
		{
			db::tServerStatus* pST = pRs->GetData(idx);
			if (lstrcmpi(pST->SvrName, szCmpName) != 0)
			{
				stdex::tStringstream out;
				out << _T("|*|")
					<< _T("|*|")
					<< _T("|*|")
					<< _T("|*|")
					<< _T("|*|")
					<< _T("|*|")
					<< _T("|*|")
					<< _T("|*|")
					<< _T("|*|")
					<< _T("|*|");
					
				str += out.str();
				//str += uri_encode(pST->DeviceInfo);
				//str += TEXT("||**||");
			}
		}
		if (str.size())
			str.resize(str.size());
		pRs->Release();
		return str;
}

DWORD i8desk_ftpWriteFtp(LPCTSTR lpszFile, HANDLE hExit)
{
	HINTERNET hSession = NULL, hFtp = NULL, hFile = NULL;

	if (NULL == (hSession = InternetOpen(TEXT("i8desk."), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0)))
		return GetLastError();

	if (NULL == (hFtp = InternetConnect(hSession, TEXT("123.196.115.69"), 22121, TEXT("chengduftp"), TEXT("chengdu@2009"),
		INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, NULL)))
	{
		DWORD dwError = GetLastError();
		InternetCloseHandle(hSession);
		return dwError;
	}

	FtpSetCurrentDirectory(hFtp, TEXT("/"));

	if (NULL == (hFile = FtpOpenFile(hFtp, _tcschr(lpszFile, TEXT('\\'))+1, GENERIC_WRITE, 
		FTP_TRANSFER_TYPE_BINARY, 0)))
	{
		DWORD dwError = GetLastError();
		InternetCloseHandle(hFtp);
		InternetCloseHandle(hSession);
		return dwError;
	}

	HANDLE hZipFile = CreateFile(lpszFile, GENERIC_READ, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	int dwSize = 0;
	if (hZipFile != INVALID_HANDLE_VALUE)
		dwSize = GetFileSize(hZipFile, NULL);

	char buf[0x1000] = {0};
	DWORD dwReadBytes = 0;

	while (dwSize && ReadFile(hZipFile, buf, sizeof(buf), &dwReadBytes, NULL) && dwReadBytes)
	{
		DWORD dwSendBytes = 0;
		if (!InternetWriteFile(hFile, buf, dwReadBytes, &dwSendBytes))
		{
			DWORD dwError = GetLastError();
			CloseHandle(hZipFile);
			InternetCloseHandle(hFile);
			InternetCloseHandle(hFtp);
			InternetCloseHandle(hSession);
			return dwError;
		}
		dwSize -= dwSendBytes;
		if (WAIT_OBJECT_0 == WaitForSingleObject(hExit, 0))
		{
			CloseHandle(hZipFile);
			InternetCloseHandle(hFile);
			InternetCloseHandle(hFtp);
			InternetCloseHandle(hSession);
			return 0;
		}
	}
	CloseHandle(hZipFile);
	InternetCloseHandle(hFile);
	InternetCloseHandle(hFtp);
	InternetCloseHandle(hSession);
	return 0;
}

int AppendLogDir(stdex::tString& dir, stdex::tString filter, HZIP hZip)
{
	int nFiles = 0;
	stdex::tString src = dir;
	if (src.size() && *src.rbegin() != '\\')
		src += TEXT("\\");

	if (hZip == 0)	return nFiles;

	WIN32_FIND_DATA wfd = {0};
	utility::CAutoFindFile hFinder = FindFirstFile((src + filter).c_str(), &wfd);
	if (hFinder.IsValid())
		return nFiles;

	while (FindNextFile(hFinder, &wfd))
	{
		if ((wfd.nFileSizeLow < 2 * 1024 * 1024 && wfd.nFileSizeHigh == 0) &&  //文件大小小于2M的文件才上传
			!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			stdex::tString file = src + wfd.cFileName;
			if (ZR_OK == ZipAdd(hZip, wfd.cFileName, (void*)file.c_str(), file.size(), ZIP_FILENAME))
			{
				DeleteFile(file.c_str());
				nFiles++;
			}
		}
	}

	return nFiles;
}

void UpLoadLog(tagUserInfo* pUserInfo, HANDLE hExited)
{
	//产生日志文件压缩包
	SYSTEMTIME st = {0};
	GetLocalTime(&st);
	TCHAR szZipFile[MAX_PATH] = {0};
	_stprintf(szZipFile, TEXT("%sLog\\Log-%02d%02d%02d%02d%02d%02d-%d.zip"), 
		GetAppPath().c_str(), 
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,
		pUserInfo->dwNibID);
	DeleteFile(szZipFile);
	HZIP hZip = CreateZipZ((void*)szZipFile, 0, ZIP_FILENAME);
	if (hZip == 0)
		return ;
	int nFiles = AppendLogDir(GetAppPath() + TEXT("Log\\"), TEXT("*.log"), hZip);
	nFiles += AppendLogDir(GetAppPath() + TEXT("Dump\\"), TEXT("*.dmp"), hZip);
	CloseZipZ(hZip);

	//上传日志文件
	if (nFiles)
		i8desk_ftpWriteFtp(szZipFile, hExited);
	else
		DeleteFile(szZipFile);
}

void DeleteMoreLogFileImp(stdex::tString& dir, stdex::tString filter, HANDLE m_hExit)
{
	stdex::tString src = dir;
	if (src.size() && *src.rbegin() != TEXT('\\'))
		src += TEXT("\\");

	FILETIME  ftBefore7Day = {0};
	GetSystemTimeAsFileTime(&ftBefore7Day);
	__int64* p = (__int64*)&ftBefore7Day;
	*p -= (7LL * 24 * 60 * 60 * 10000000);

	WIN32_FIND_DATA wfd = {0};
	utility::CAutoFindFile hFinder = FindFirstFile((src + filter).c_str(), &wfd);
	if ( hFinder.IsValid() )
		return ;

	while (FindNextFile(hFinder, &wfd) && WAIT_OBJECT_0 != WaitForSingleObject(m_hExit, 0))
	{
		if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
			CompareFileTime(&wfd.ftLastWriteTime, &ftBefore7Day) == -1)
		{
			stdex::tString file = src + wfd.cFileName;
			SetFileAttributes(file.c_str(), FILE_ATTRIBUTE_NORMAL);
			DeleteFile(file.c_str());
		}
	}
}

void DeleteMoreLogFile(HANDLE m_hExit)
{
	TCHAR szLogDir[MAX_PATH] = {0};
	_stprintf(szLogDir, TEXT("%sLog\\"), GetAppPath().c_str());

	DeleteMoreLogFileImp(stdex::tString(szLogDir), stdex::tString(TEXT("*.log")), m_hExit);
	DeleteMoreLogFileImp(stdex::tString(szLogDir), stdex::tString(TEXT("*.zip")), m_hExit);
}

}
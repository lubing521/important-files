//////////////预处理
#ifndef WINVER                          // 指定要求的最低平台是 Windows Vista。
#define WINVER 0x0600           // 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif

#ifndef _WIN32_WINNT            // 指定要求的最低平台是 Windows Vista。
#define _WIN32_WINNT 0x0600     // 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif

#ifndef _WIN32_WINDOWS          // 指定要求的最低平台是 Windows 98。
#define _WIN32_WINDOWS 0x0410 // 将此值更改为适当的值，以适用于 Windows Me 或更高版本。
#endif

#ifndef _WIN32_IE                       // 指定要求的最低平台是 Internet Explorer 7.0。
#define _WIN32_IE 0x0700        // 将此值更改为相应的值，以适用于 IE 的其他版本。
#endif

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料


#include <windows.h>
#include <WinInet.h>
#include <ShlObj.h>

#include <Shlwapi.h>
#include <process.h>
#include <time.h>
#include <Userenv.h>
#include <winioctl.h>
#include <tchar.h>
#include <fstream>
#include <wbemidl.h>
#include <Userenv.h>
#include <Psapi.h>
#include <comdef.h>
#include <atlbase.h>
#include <atlstr.h>
#include <dshow.h>
#include <comutil.h>
#pragma comment(lib, "strmiids.lib ")
#pragma comment(lib, "comsuppw.lib")
#pragma warning(disable:4200)
#pragma comment(lib, "version.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "Wbemuuid") 
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Userenv.lib")

#include <string>
#include <list>
#include <vector>
#include <algorithm>
#include <map>
#include <sstream>
////////////////////

#include "cltsvrdef.h"
#include "Tool.h"
#include "Hy/hy.h"
#include "system.h"
#include "Extend STL/StringAlgorithm.h"
#include "Win32/System/I8Temperature.h"

const int C_WAIT_WMI_TIME = 5000; //等待WMI 5秒

I8_NAMESPACE_BEGIN

CSystemInfo::CSystemInfo(CHyOperator* pHyOperator)
{
	m_pHyOperator = pHyOperator;
}

CSystemInfo::~CSystemInfo()
{
}

stdex::tString CSystemInfo::GetWmiInfo( LPCTSTR lpszQuery, LPCTSTR lpszField )
{	
	CComPtr<IWbemLocator>  pIWbemLocator;
	CComPtr<IWbemServices> pWbemServices;
	stdex::tString   strValue;

	HRESULT hr = CoInitialize(NULL);
	hr = CoInitializeSecurity( NULL, -1, NULL,	NULL, RPC_C_AUTHN_LEVEL_PKT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, 0);
	if (hr != S_OK && hr != RPC_E_TOO_LATE)
	{
		CoUninitialize();
		return strValue;
	}

	if(CoCreateInstance (CLSID_WbemAdministrativeLocator, NULL,	CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, \
		IID_IUnknown , reinterpret_cast<void**>(&pIWbemLocator.p)) == S_OK)
    {
        if (pIWbemLocator->ConnectServer(BSTR(L"root\\cimv2"),  \
		    NULL, NULL, NULL, 0, NULL, NULL, &pWbemServices.p) == S_OK)
	    {
		    CComPtr<IEnumWbemClassObject> pEnumObject;
            hr = pWbemServices->ExecQuery(_bstr_t("WQL"), _bstr_t(lpszQuery),WBEM_FLAG_RETURN_IMMEDIATELY,NULL,&pEnumObject.p);
            if(FAILED(hr))
            {
                CoUninitialize();
                return strValue;
            }

		    hr = pEnumObject->Reset();
            if(FAILED(hr))
            {
                CoUninitialize();
                return strValue;
            }

            CComPtr<IWbemClassObject> pClassObject;

            ULONG uCount = 1, uReturned = 0;
		    hr = pEnumObject->Next(C_WAIT_WMI_TIME, uCount, &pClassObject.p, &uReturned);
		    if (SUCCEEDED(hr) && pClassObject != NULL)
		    {
			    do
			    {
                    assert(pClassObject != NULL);
				    CComVariant varValue ;
				    _bstr_t bstrName(lpszField);
				    if( pClassObject->Get( bstrName ,0 ,&varValue ,NULL ,0 ) == S_OK )
				    {
					    if(varValue.vt == VT_NULL || varValue.vt == VT_EMPTY || varValue.vt == VT_ERROR)   
						    break;
					    _bstr_t b;
					    if( varValue.vt & VT_BSTR )
					    {
						    b = &varValue;
						    strValue = stdex::tString(b);
					    }
					    else if( varValue.vt & VT_ARRAY )
					    {
						    long iLowBound = 0 , iUpBound = 0 ;
						    SafeArrayGetLBound( varValue.parray , 1 , &iLowBound ) ;
						    SafeArrayGetUBound( varValue.parray , 1 , &iUpBound ) ;
						    for( long j = iLowBound ; j <= iUpBound ; j ++ )
						    {
							    VARIANT *pvar = NULL ;
							    long temp = j ;
							    if( SafeArrayGetElement( varValue.parray , &temp , pvar ) == S_OK &&
								    pvar )
							    {
								    CComVariant varTemp ;
								    if( varTemp.ChangeType( VT_BSTR , pvar ) == S_OK )
								    {
									    if(!strValue.empty())
										    strValue += TEXT(",");
									    b = &varTemp;
									    strValue += stdex::tString(b) ;
								    }                                                                
							    }
						    }
					    }
					    else
					    {
						    if( varValue.ChangeType( VT_BSTR ) == S_OK )
						    {
							    b = &varValue;
							    strValue += stdex::tString(b) ;
						    }					
					    }
				    }
			    } while (FALSE);
		    }
	    }
    }

	CoUninitialize();   
	return strValue;
}

BOOL CSystemInfo::GetComputerName()
{
	DWORD dwSize = MAX_PATH;
	return ::GetComputerName(m_stSysCfgInfo.szName, &dwSize);
}


BOOL CSystemInfo::GetOptSystem()
{
	stdex::tString strValue = GetWmiInfo(TEXT("Select * from Win32_OperatingSystem"), TEXT("Version"));
	if (strValue.empty())
        return FALSE;
    stdex::Trim(strValue);
	lstrcpyn(m_stSysCfgInfo.szOptSystem, strValue.c_str(), MAX_PATH*sizeof(TCHAR));
	return TRUE;
}

BOOL CSystemInfo::GetCPUInfo()
{
	stdex::tString strValue = GetWmiInfo(TEXT("Select * from Win32_processor"), TEXT("Name"));
//     stdex::tString strValue = CI8Temperature::Instance().GetCPUInfo();
    if (strValue.empty())
    {
        return FALSE;
    }
    stdex::Trim(strValue);
	lstrcpyn(m_stCltHardInfo.szCPU, strValue.c_str(), MAX_PATH*sizeof(TCHAR));
	return TRUE;	
}

BOOL CSystemInfo::GetMainboardInfo()
{
	stdex::tString strValue = GetWmiInfo(TEXT("Select * from Win32_BaseBoard"), TEXT("Product"));
	if (strValue.empty())
        return FALSE;
    stdex::Trim(strValue);
	lstrcpyn(m_stCltHardInfo.szMainboard, strValue.c_str(), MAX_PATH*sizeof(TCHAR));
	return TRUE;	
}

BOOL CSystemInfo::GetMemoryInfo()
{
	stdex::tString strValue = GetWmiInfo(TEXT("Select * from Win32_LogicalMemoryConfiguration"), TEXT("TotalPhysicalMemory"));
	if (strValue.empty())
		return FALSE;
	tsprintf(m_stCltHardInfo.szMemory, MAX_PATH, TEXT("%d"), __int64(_ttoi64(strValue.c_str())/1024. + .55));
	return TRUE;
}

INT64 CSystemInfo::GetMemorySize()
{
    stdex::tString strValue = GetWmiInfo(TEXT("Select * from Win32_LogicalMemoryConfiguration"), TEXT("TotalPhysicalMemory"));
    if (strValue.empty())
        return -1;
    return _ttoi64(strValue.c_str());
}

BOOL CSystemInfo::GetHarddiskInfo()
{
	stdex::tString strValue = GetWmiInfo(TEXT("Select * from Win32_DiskDrive"), TEXT("Model"));
	if (strValue.empty())
        return FALSE;
    stdex::Trim(strValue);
	lstrcpyn(m_stCltHardInfo.szDisk, strValue.c_str(), MAX_PATH*sizeof(TCHAR));
	return TRUE;
}

BOOL CSystemInfo::GetSoundCardInfo()
{
	stdex::tString strValue = GetWmiInfo(TEXT("Select * from Win32_SoundDevice"), TEXT("Caption"));
	if (strValue.empty())
        return FALSE;
    stdex::Trim(strValue);
	lstrcpyn(m_stCltHardInfo.szSoundCard, strValue.c_str(), MAX_PATH*sizeof(TCHAR));
	return TRUE;
}

BOOL CSystemInfo::GetGraphicsInfo()
{	
	stdex::tString strValue = GetWmiInfo(TEXT("Select * from Win32_VideoController"), TEXT("Name"));
	if (strValue.empty())
        return FALSE;
    stdex::Trim(strValue);
	lstrcpyn(m_stCltHardInfo.szGraphics, strValue.c_str(), MAX_PATH*sizeof(TCHAR));
	return FALSE;
}

BOOL CSystemInfo::GetCameraInfo()
{
	::CoInitialize(NULL);

	ICreateDevEnum *pSysDevEnum = NULL;
	BOOL            bIsGetSuccess = FALSE;
	HRESULT         hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&pSysDevEnum);
	if (hr == S_FALSE)	return FALSE;	

	IEnumMoniker *pEnumCat = NULL;
	hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumCat, 0);
	if (hr == S_FALSE)	return FALSE;	

	//Enumerate   the   monikers.   
	IMoniker *pMoniker = NULL;
	ULONG   cFetched;
	while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
	{
		IPropertyBag *pPropBag = NULL;
		hr = pMoniker->BindToStorage(0,   0,   IID_IPropertyBag, (void **)&pPropBag);
		if (SUCCEEDED(hr))
		{
			//To retrieve the filter's  friendly   name,   do   the   following:   
			VARIANT   varName;
			VariantInit(&varName);
			hr = pPropBag->Read(L"FriendlyName", &varName, 0);
			if (SUCCEEDED(hr))
			{
				_bstr_t str(varName);
				tstrcpy(m_stCltHardInfo.szWebcam, MAX_PATH, (TCHAR*)str);
				bIsGetSuccess = TRUE;
			}
			VariantClear(&varName);

			//Remember   to   release   pFilter   later.
			pPropBag->Release();
		}
		pMoniker->Release();
		if (bIsGetSuccess) break;
	}
	pEnumCat->Release();
	pSysDevEnum->Release();
	::CoUninitialize();
	return TRUE;
}


STCltHardwareInfo& CSystemInfo::GetHardwareInfo()
{
	GetCPUInfo();
	GetMainboardInfo();
	GetMemoryInfo();
	GetHarddiskInfo();
	GetSoundCardInfo();
	GetGraphicsInfo();
	GetNICInfo();
	GetCameraInfo();

	return m_stCltHardInfo;
}

STSysCfgInfo& CSystemInfo::GetSysCfgInfo(SOCKET hSocket)
{
	GetComputerName();
	GetOptSystem();

	//首先取得连接上服务器的socket的本机ip
	stdex::tString     strIP;
	sockaddr_in addr = {0};
	int         nSize = sizeof(addr);

	if (SOCKET_ERROR == getsockname(hSocket, (PSOCKADDR)&addr, &nSize))
		return m_stSysCfgInfo;
	strIP = (LPCTSTR)_bstr_t(inet_ntoa(addr.sin_addr));


	IP_ADAPTER_INFO  Adapter[10] = {0};
	PIP_ADAPTER_INFO pAdapter = Adapter;

	nSize = sizeof(Adapter);
	if (ERROR_SUCCESS != GetAdaptersInfo(Adapter, (PULONG)&nSize))
		pAdapter = NULL;
	while (strIP.size() && pAdapter != NULL)
	{
		_IP_ADDR_STRING* pIpAddress = &pAdapter->IpAddressList;
		while (pIpAddress != NULL)
		{
			if (std::string(CT2A(strIP.c_str())) == pIpAddress->IpAddress.String)
				goto do_next;
			pIpAddress = pIpAddress->Next;
		}
		pAdapter = pAdapter->Next;
	}

do_next:
	if (pAdapter != NULL)
	{
		tstrcpy(m_stCltHardInfo.szNIC, MAX_PATH, CA2T(pAdapter->Description));
		m_stSysCfgInfo.dwIP = inet_addr(CT2A(strIP.c_str()));
		m_stSysCfgInfo.dwGate = inet_addr(pAdapter->GatewayList.IpAddress.String);
		m_stSysCfgInfo.dwSubMask = inet_addr(pAdapter->IpAddressList.IpMask.String);
		tsprintf(m_stSysCfgInfo.szMacAddr, MAX_PATH, TEXT("%02X-%02X-%02X-%02X-%02X-%02X"), 
			pAdapter->Address[0],
			pAdapter->Address[1],
			pAdapter->Address[2],
			pAdapter->Address[3],
			pAdapter->Address[4],
			pAdapter->Address[5]);
		IP_PER_ADAPTER_INFO perAdapter[10] = {0};
		nSize = sizeof(perAdapter);
		if (ERROR_SUCCESS == GetPerAdapterInfo(pAdapter->Index, perAdapter, (PULONG)&nSize))
		{
			m_stSysCfgInfo.dwDns0 = inet_addr(perAdapter->DnsServerList.IpAddress.String);
			if (perAdapter->DnsServerList.Next != NULL)
			{
				m_stSysCfgInfo.dwDns1 = inet_addr(perAdapter->DnsServerList.Next->IpAddress.String);
			}
		}
	}

	return m_stSysCfgInfo;
}

STI8Version& CSystemInfo::GetI8Version()
{
	stdex::tString strPath;
	strPath = GetRootDir();

	tstrcpy(m_stI8Version.szCltSvrVer, MAX_PATH, GetFileVersion(strPath + TEXT("\\i8deskclisvr.exe")).c_str());
	tstrcpy(m_stI8Version.szCltUIVer, MAX_PATH, GetFileVersion(strPath + TEXT("\\Baronline.exe")).c_str());
#ifdef _DEBUG
    tstrcpy(m_stI8Version.szCltDataVer, MAX_PATH, GetFileVersion(strPath + TEXT("\\I8ClientDataCoreD.dll")).c_str());
#else
    tstrcpy(m_stI8Version.szCltDataVer, MAX_PATH, GetFileVersion(strPath + TEXT("\\I8ClientDataCore.dll")).c_str());
#endif

	strPath = GetSystemDir();
    tstrcpy(m_stI8Version.szProtVer, MAX_PATH, GetFileVersion(strPath + TEXT("\\drivers\\wxProt.sys")).c_str());
    tstrcpy(m_stI8Version.szVirtualDiskVer, MAX_PATH, GetFileVersion(strPath + TEXT("\\drivers\\wxNda.sys")).c_str());

	return m_stI8Version;
}

STHyInfo& CSystemInfo::GetHyInfo()
{
	m_stHyInfo.dwInstall = m_pHyOperator->IsInstallHy() ? 1 : 0;

	int nDriverIdx = 0, nProtIdx = 0;
	for (TCHAR ch = 'C'; ch <= 'Z'; ++ch)
	{
		TCHAR szDriver[MAX_PATH] = {0};
		tsprintf(szDriver, MAX_PATH, TEXT("%c:\\"), ch);
		if (GetDriveType(szDriver) == DRIVE_FIXED)
		{
			m_stHyInfo.szAllDriver[nDriverIdx++] = ch;
		}
	}

    const int HD_MAX_NUM = 10; //假设用户最多有10个硬盘
	for (int i = 0;i < HD_MAX_NUM; ++i)
	{
		tag_HDInfo hdInfo = {0};
		hdInfo.dwHdNumber = i;

		if (0 != m_pHyOperator->GetHDInfo(&hdInfo))
			continue;

		for (DWORD loop = 0; loop < hdInfo.dwPtNums; loop++)
		{
			if (m_stHyInfo.dwInstall == 1 &&
				m_pHyOperator->IsDriverProtected((TCHAR)hdInfo.pt[loop].bDosChar))
			{					
				m_stHyInfo.szProtDriver[nProtIdx++]= hdInfo.pt[loop].bDosChar;
			}
		}
	}
    if (nProtIdx > 0)
    {
        std::sort(&m_stHyInfo.szProtDriver[0], &m_stHyInfo.szProtDriver[nProtIdx]);
    }

	return m_stHyInfo;
}

BOOL CSystemInfo::GetNICInfo()
{
	return TRUE;
}

I8_NAMESPACE_END
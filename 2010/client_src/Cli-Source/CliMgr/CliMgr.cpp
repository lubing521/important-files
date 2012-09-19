// CliMgr.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <sstream>
using namespace std;

#include "CliMgr.h"
#include <comdef.h>
#include <atlbase.h>
#include <atlstr.h>
#include <Tlhelp32.h>
#include <wbemidl.h>
#include <Userenv.h>
#include <Psapi.h>

#pragma comment(lib, "Wbemuuid") 
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Userenv.lib")

#define PLUG_VERSION			0x01000000
#define PLUG_COMPANY			TEXT("Goyoo")
#define PLUG_DESCRIPTION		TEXT("CliMgr Plug")

namespace i8desk
{
	tstring GetRunGameExe()
	{
		char path[MAX_PATH] = {0};
		path[0] = '\"';
		GetModuleFileName(NULL, &path[1], MAX_PATH);
		PathRemoveFileSpec(path);
		PathAddBackslash(path);
		lstrcat(path, "RunGame.exe\" ");
		return tstring(path);
	}
	class CCliMgr : public IPlug
	{
	private:
		ILogger*	m_pLogger;
		IPlugMgr*	m_pPlugMgr;
		CHyOperator m_Hy;
		tstring		m_SysInfo;
	public:
		CCliMgr():m_pLogger(NULL), m_pPlugMgr(NULL)	
		{
		}
		virtual ~CCliMgr()		{ Release_Interface(m_pLogger); }
		virtual void Release()	{ delete this; }
	public:
		virtual LPCTSTR GetPlugName()						{ return CLIMGR_PLUG_NAME;	}
		virtual DWORD  	GetVersion()						{ return PLUG_VERSION;		}
		virtual LPCTSTR GetCompany()						{ return PLUG_COMPANY;		}
		virtual LPCTSTR GetDescription()					{ return PLUG_DESCRIPTION;	}
		virtual bool    NeedUdpNotity()						{ return true;				}
		virtual bool	Initialize(IPlugMgr* pPlugMgr);
		virtual bool	UnInitialize();
		virtual DWORD	SendMessage(DWORD cmd, DWORD param1, DWORD param2)	{ return 0; }
		virtual void	UdpRecvNotify(char* pData, int length, sockaddr_in& from);
	private:
		//调用通信接口来交互一次网络命令
		void InitPackage(_packageheader* header, WORD cmd, int nExtraLen = 0);
		bool ExecCmd(LPSTR pOutData, int nOutLen, LPSTR& pInData, int& nInLen, bool bNeedAck = true);

		void GetSysInfo();		//获取系统信息
		void GetSysProcess();	//获取系统的进程
		void GetSysService();	//获取系统服务

		void KillProcess(LPCTSTR lpszSvrName);	//杀进程
		void StartService(LPCTSTR lpszSvrName);	//启动服务
		void StopService(LPCTSTR lpszSvrName);	//停止服务
		tstring _getWmiInfo(IWbemClassObject *pClassObject,LPCTSTR lpszField);
		tstring GetWmiInfo( LPCTSTR lpszClass,LPCTSTR lpszField );	//通过WMI获取系统信息
		BOOL RunProcess(LPCSTR lpImage,LPCSTR lpPara);	//从服务里执行文件；
		BOOL GetTokenByName(HANDLE &hToken,LPSTR lpName);
	};

	IPlug* WINAPI CreatePlug()
	{
		return new CCliMgr;
	}

	bool CCliMgr::Initialize(IPlugMgr* pPlugMgr)
	{
		if (pPlugMgr == NULL || (m_pLogger = pPlugMgr->CreateLogger()) == NULL)
			return false;
		m_pPlugMgr =  pPlugMgr;
		m_Hy.SetLogger(m_pLogger);
		m_pLogger->SetLogFileName(CLIMGR_PLUG_NAME);
		m_pLogger->SetAddDateBefore(false);
		m_pLogger->WriteLog(LM_INFO, TEXT("========================================================="));
		m_pLogger->SetAddDateBefore(true);
		m_pLogger->WriteLog(LM_INFO, TEXT("Plug Initialize"));
		if (!m_Hy.IsInitialize())
		{
			m_pLogger->WriteLog(LM_INFO, TEXT("HY Initialize Fail."));
			return false;
		}
		return true;
	}

	bool CCliMgr::UnInitialize()
	{
		if (m_pLogger)
		{
			m_pLogger->WriteLog(LM_INFO, TEXT("Plug UnInitialize"));
			m_pLogger->SetAddDateBefore(false);
			m_pLogger->WriteLog(LM_INFO, TEXT("=========================================================\r\n\r\n"));
		}
		return true;
	}

	void CCliMgr::UdpRecvNotify(char* pData, int length, sockaddr_in& from)
	{
		_packageheader* pheader = reinterpret_cast<_packageheader*>(pData);
		CPackageHelper  in(pData);

		m_pLogger->WriteLog(LM_INFO, TEXT("Recv Server UDP Cmd."));

		DWORD dwCRC = in.popDWORD();
		E_CliCtrlCmd cmd = (E_CliCtrlCmd)in.popDWORD();
		switch (cmd)
		{
		case Ctrl_InstallProt:
			{	
				bool bProtArea = (in.popDWORD() == 1);
				m_pLogger->WriteLog(LM_INFO, TEXT("Recv Prot Install, Prot All Area:%s"), 
					bProtArea ?  TEXT("true") : TEXT("false"));
				/*是否安装还原*/
				if (!m_Hy.IsInstallHy())
				{
					if (bProtArea)
					{
						m_Hy.InstallHy("CDEFGHIJKLMNOPQRSTUVWXYZ");
					}
					else
						m_Hy.InstallHy("");
				}
				else
				{
					if (bProtArea)
						m_Hy.ProtArea("CDEFGHIJKLMNOPQRSTUVWXYZ");
				}
				//写注册表，以使还原不自检。
				SHSetValue(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Session Manager", 
					"BootExecute", REG_MULTI_SZ, "", 0);
			}
			break;
		case Ctrl_RemoveProt:
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("Recv Remove Prot."));
				m_Hy.RemoveHy();
			}
			break;
		case Ctrl_ProtArea:
			{
				char ProtArea[30] ={0};
				in.popString(ProtArea);
				m_pLogger->WriteLog(LM_INFO, TEXT("Recv Prot Area Cmd:%s"), ProtArea);				
				m_Hy.ProtArea(ProtArea);
				in.popString(ProtArea);
				m_pLogger->WriteLog(LM_INFO, TEXT("Recv UnProt Area Cmd:%s"), ProtArea);				
				m_Hy.UnProtArea(ProtArea);
			}
			break;
		case Ctrl_ShutDown:
			{
				bool bReboot = (in.popDWORD() == 1);
				m_pLogger->WriteLog(LM_INFO, bReboot ? TEXT("Recv ReBoot Machine Cmd.") : 
					TEXT("Recv ShutDown Machine Cmd."));
				m_Hy.ShutDown(bReboot);
			}
			break;
		case Ctrl_ExecFile:
			{
				char szFile[MAX_PATH] = {0};
				in.popString(szFile);
				m_pLogger->WriteLog(LM_INFO, TEXT("Recv Run Exe File:%s"), szFile);
				
				tstring root = GetRunGameExe();
				root = root +tstring(szFile);
				RunProcess(root.c_str(),root.c_str());
			}
			break;
		case Ctrl_ViewSysInfo:	//得到系统信息
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("Recv View System Info Cmd."));

				GetWmiInfo("Win32_DiskDrive","Manufacturer");
				char *pData = NULL;
				int  nSize = m_SysInfo.size() +1024;
				char* buf= new char[nSize];
				_packageheader* pheader = (_packageheader*)buf;
				InitPackage(pheader, CMD_GAME_CLIENTTOCON);
				
				CPackageHelper out(buf);
				out.pushDWORD(Ctrl_ViewSysInfo);
				out.pushString(m_SysInfo.c_str(), m_SysInfo.size());
				pheader->Length = out.GetOffset();
				if (!ExecCmd(buf, pheader->Length, pData, nSize,false))
					m_pLogger->WriteLog(LM_INFO, "Report Ctrl_ViewSysInfo Fail.");			
				else
				{
					m_pLogger->WriteLog(LM_INFO, "Report Ctrl_ViewSysInfo Success.");
					CoTaskMemFree(pData);
				}
				delete []buf;
				
			}
			break;
		case Ctrl_ViewProcInfo: //系统进程的控制
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("Recv View Process Info Cmd."));
				GetSysProcess();
			}
			break;
		case Ctrl_ViewSviInfo: //服务控制
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("Recv View Service Info Cmd."));
				GetSysService();
			}
			break;
		case Ctrl_KillProcSvr: //杀进程，停服务
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("Recv KillProcSvr Cmd."));
				char Name[MAX_PATH] ={0};
				if (in.popDWORD())
				{
					in.popString(Name);
					KillProcess(Name);
				}
				else
				{
					in.popString(Name);
					StopService(Name);
				}				
			}
			break;
		}
	}

	std::string ProcessImagePath(LPCSTR lpszPath)
	{
		std::string strProcName(lpszPath != NULL ? lpszPath : "");
		transform(strProcName.begin(), strProcName.end(), strProcName.begin(), toupper);
		if (strProcName.substr(0, 4) == "\\??\\")
			strProcName.erase(0, 4);

		char szSysPath[MAX_PATH] = {0};
		GetSystemDirectory(szSysPath, MAX_PATH);
		PathAddBackslash(szSysPath);
		std::string strsys = "\\SYSTEMROOT\\SYSTEM32\\";			
		if (strProcName.substr(0, strsys.size()) == strsys)
		{
			strProcName.erase(0, strsys.size());
			strProcName = string(szSysPath) + strProcName;
		}
		strsys = "SYSTEM32\\";
		if (strProcName.substr(0, strsys.size()) == strsys)
		{
			strProcName.erase(0, strsys.size());
			strProcName = string(szSysPath) + strProcName;
		}
		return strProcName;
	}
	void CCliMgr::GetSysInfo()
	{
		GetWmiInfo("Win32_DiskDrive","Manufacturer");
	}
	
	void CCliMgr::GetSysProcess()
	{
		DWORD aProcesses[1024] = {0}, cbNeeded = 0, cProcesses = 0, MemSize = 0;
		if (!EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ))
		{
			DWORD dwError = GetLastError();
			_com_error Error(dwError);
			m_pLogger->WriteLog(LM_INFO, TEXT("GetSysProcess (EnumProcess) Error:%d:%s"), dwError, Error.ErrorMessage());
			return;
		}
		cProcesses = cbNeeded / sizeof(DWORD);
		m_pLogger->WriteLog(LM_INFO, TEXT("Process Num:%d"), cProcesses);
		CMarkup xml;
		xml.SetDoc("<?xml version=\"1.0\" encoding=\"gb2312\"?>\r\n");
		xml.AddElem("ProcessLst");
		xml.IntoElem();
		for (DWORD idx=0; idx<cProcesses; idx++)
		{
			TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, aProcesses[idx]);
			if (NULL == hProcess)
				continue;
			else
			{
				if (0 == GetModuleFileNameEx(hProcess, NULL, szProcessName, MAX_PATH))
				{
					CloseHandle( hProcess );
					continue;
				}
				PROCESS_MEMORY_COUNTERS pmc = {0};
				pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS);
				if ( ::GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
				{
					MemSize = pmc.WorkingSetSize;
				}
				CloseHandle( hProcess );
			}
			string strProcName = ProcessImagePath(szProcessName);		
 			TCHAR buf[MAX_PATH] = {0};
 			_stprintf(buf, TEXT("%s  %d  (PID: %u)\r\n"), szProcessName,MemSize, aProcesses[idx]);
			xml.AddElem("Process");
			xml.AddAttrib("Name",	strProcName);    
			xml.AddAttrib("Pid",	aProcesses[idx]);    
			xml.AddAttrib("Memory",	MemSize);				
			m_pLogger->WriteLog(LM_INFO, buf);
		}
		xml.OutOfElem();
		string data=xml.GetDoc(); 
		char *pData = NULL;
		int  nSize = data.size() +1024;
		char* buf= new char[nSize];
		_packageheader* pheader = (_packageheader*)buf;
		InitPackage(pheader, CMD_GAME_CLIENTTOCON);
		CPackageHelper out(buf);
		out.pushDWORD(Ctrl_ViewProcInfo);
		out.pushString(data.c_str(), data.size());
		pheader->Length = out.GetOffset();

		if (!ExecCmd(buf, pheader->Length, pData, nSize,false))
			m_pLogger->WriteLog(LM_INFO, "Report Ctrl_ViewProcInfo Fail.");			
		else
		{
			m_pLogger->WriteLog(LM_INFO, "Report Ctrl_ViewProcInfo Success.");
			CoTaskMemFree(pData);
		}
		delete []buf;
	}

	void CCliMgr::GetSysService()
	{
		SC_HANDLE schManager = OpenSCManager(NULL, NULL, SERVICE_QUERY_STATUS);
		if (schManager == NULL)
		{
			DWORD dwError = GetLastError();
			_com_error Error(dwError);
			m_pLogger->WriteLog(LM_INFO, TEXT("GetSysService-OpenSCManager Error:%d:%s"), dwError, Error.ErrorMessage());
			return ;
		}

		DWORD dwSize = 0, dwCount = 0;
		LPENUM_SERVICE_STATUS st = (LPENUM_SERVICE_STATUS)LocalAlloc(LPTR, 64*1024);
		LPQUERY_SERVICE_CONFIG sc = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LPTR, 4 * 1024);
		if (!EnumServicesStatus(schManager, SERVICE_TYPE_ALL, SERVICE_ACTIVE, st, 1024*64, &dwSize, &dwCount, NULL))
		{
			DWORD dwError = GetLastError();
			_com_error Error(dwError);
			m_pLogger->WriteLog(LM_INFO, TEXT("EnumServicesStatus Error:%d:%s"), dwError, Error.ErrorMessage());
		}

		CMarkup xml;
		xml.SetDoc("<?xml version=\"1.0\" encoding=\"gb2312\"?>\r\n");
		xml.AddElem("ServiceLst");
		xml.IntoElem();
		
		for (DWORD idx=0; idx<dwCount; idx++)
		{
			SC_HANDLE hService = ::OpenService(schManager,st[idx].lpServiceName,SERVICE_QUERY_CONFIG);
			if(hService)
			{
				DWORD dwSize = 0;
				::QueryServiceConfig(hService, sc, 4 * 1024, &dwSize);
				CloseServiceHandle(hService);
			}
			if (st[idx].ServiceStatus.dwCurrentState == SERVICE_RUNNING)
			{
				TCHAR buf[MAX_PATH] = {0};
				_stprintf(buf, TEXT("%s  %s  %s)\r\n"), st[idx].lpServiceName, sc->lpBinaryPathName, st[idx].lpDisplayName);

				string strProcName = ProcessImagePath(sc->lpBinaryPathName);
				xml.AddElem("Service");
				xml.AddAttrib("Name",		 st[idx].lpServiceName);
				xml.AddAttrib("Application", strProcName.c_str());
				xml.AddAttrib("DisplayName", st[idx].lpDisplayName);
				m_pLogger->WriteLog(LM_INFO, buf);
			}
		}
		xml.OutOfElem();
		string data=xml.GetDoc();
		char *pData = NULL;
		int  nSize = data.size() + 1024;
		char* buf= new char[nSize];
		_packageheader* pheader = (_packageheader*)buf;
		InitPackage(pheader, CMD_GAME_CLIENTTOCON);
		CPackageHelper out(buf);
		out.pushDWORD(Ctrl_ViewSviInfo);
		out.pushString(data.c_str(), data.size());
		pheader->Length = out.GetOffset();
		if (!ExecCmd(buf, pheader->Length, pData, nSize, false))
			m_pLogger->WriteLog(LM_INFO, "Report Ctrl_ViewSviInfo Fail.");			
		else
		{
			m_pLogger->WriteLog(LM_INFO, "Report Ctrl_ViewSviInfo Success.");
			CoTaskMemFree(pData);
		}
		delete []buf;
		LocalFree(st);
		LocalFree(sc);
		CloseServiceHandle(schManager);
	}

	void CCliMgr::KillProcess(LPCTSTR lpszSvrName)
	{
		DWORD aProcesses[1024] = {0}, cbNeeded = 0, cProcesses = 0, MemSize = 0;
		if (!EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ))
		{
			DWORD dwError = GetLastError();
			_com_error Error(dwError);
			m_pLogger->WriteLog(LM_INFO, TEXT("GetSysProcess (EnumProcess) Error:%d:%s"), dwError, Error.ErrorMessage());
			return;
		}
		cProcesses = cbNeeded / sizeof(DWORD);
		tstring sModName = lpszSvrName;
		for (DWORD idx=0; idx<cProcesses; idx++)
		{
			TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|PROCESS_TERMINATE, FALSE, aProcesses[idx]);
			if (NULL != hProcess)
			{
				if (0 == GetModuleFileNameEx(hProcess, NULL, szProcessName, MAX_PATH))
				{
					CloseHandle( hProcess );
					continue;
				}
				string strProcName = ProcessImagePath(szProcessName);
				if (stricmp(sModName.c_str(), strProcName.c_str()) == 0)
				{
					TerminateProcess(hProcess,0);
					CloseHandle( hProcess );
					return ;
				}
				CloseHandle( hProcess );
			}
		}	
	}

	void CCliMgr::StartService(LPCTSTR lpszSvrName)
	{
		SC_HANDLE   schSCManager;
		SERVICE_STATUS svcStatus;
		BOOL success;
		schSCManager = OpenSCManager(
			NULL,                   // machine (NULL == local)
			NULL,                   // database (NULL == default)
			SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE  // access required
			);
		if (schSCManager == NULL)
			return ;

		//打开服务    
		SC_HANDLE hService = ::OpenService(schSCManager, lpszSvrName, SERVICE_ALL_ACCESS);   
		if (hService != NULL)   
		{   
			success = QueryServiceStatus(hService,&svcStatus);
			if (!success)
			{
				return;
			}
			if (svcStatus.dwCurrentState == SERVICE_STOPPED)
			{
				StartService(lpszSvrName);
			}

			::CloseServiceHandle(hService);   
		}   
		::CloseServiceHandle(schSCManager); 

		return ;
	}

	void CCliMgr::StopService(LPCTSTR lpszSvrName)
	{
		SC_HANDLE   schSCManager;
		SERVICE_STATUS svcStatus;
		BOOL success;
		schSCManager = OpenSCManager(
			NULL,                   // machine (NULL == local)
			NULL,                   // database (NULL == default)
			SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE  // access required
			);
		if (schSCManager == NULL)
			return ;

		//打开服务    
		SC_HANDLE hService = ::OpenService(schSCManager, lpszSvrName, SERVICE_ALL_ACCESS);   
		if (hService != NULL)   
		{   
			success = QueryServiceStatus(hService,&svcStatus);
			if (!success)
			{
				return;
			}
			if (svcStatus.dwCurrentState == SERVICE_RUNNING)
			{
					success = ControlService(
					hService,           // handle to service
					SERVICE_CONTROL_STOP, // control code
					&svcStatus            // pointer to service status structure
					);
					if (!success)
					{
						DWORD dwError = GetLastError();
						_com_error Error(dwError);
						m_pLogger->WriteLog(LM_INFO, TEXT("Stop Service:%s  Erorr:%d:%s"),lpszSvrName, dwError, (LPCTSTR)Error.ErrorMessage());
					}

			}
			::CloseServiceHandle(hService);   
		}   
		::CloseServiceHandle(schSCManager); 

		return ;
	}

	tstring CCliMgr::_getWmiInfo(IWbemClassObject *pClassObject,LPCTSTR lpszField)
	{
		SAFEARRAY *pvNames = NULL;
		tstring lpszText2;
		CComVariant varValue ;
		_bstr_t bstrName(lpszField);
		if( pClassObject->Get( bstrName , 0 , &varValue , NULL , 0 ) == S_OK )
		{
			if(varValue.vt   ==   VT_NULL   ||   varValue.vt   ==   VT_EMPTY   ||   varValue.vt   ==   VT_ERROR)   
				return lpszText2;
			_bstr_t b;
			if( varValue.vt & VT_BSTR )
			{
				b = &varValue;
				lpszText2 = tstring(b);
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
							if( !lpszText2.empty() )
								lpszText2 += _T(",") ;
							b = &varTemp;
							lpszText2 += tstring(b) ;
						}                                                                
					}
				}
			}
			else
			{
				if( varValue.ChangeType( VT_BSTR ) == S_OK )
				{
					b = &varValue;
					lpszText2 += tstring(b) ;
				}					
			}
		}                            
		return lpszText2 ;
	}

	tstring CCliMgr::GetWmiInfo( LPCTSTR lpszClass,LPCTSTR lpszField)
	{
		tstring SysInfo,strtmp;
		HRESULT ret;
		ret = CoInitialize(NULL);
		ret = CoInitializeSecurity( NULL,
			-1,
			NULL,
			NULL,
			RPC_C_AUTHN_LEVEL_PKT,
			RPC_C_IMP_LEVEL_IMPERSONATE,
			NULL,
			EOAC_NONE,
			0
			);
		if (ret == S_OK || ret== RPC_E_TOO_LATE )
		{
			IWbemLocator * pIWbemLocator = NULL;
			IWbemServices * pWbemServices = NULL;
			IEnumWbemClassObject * pEnumObject  = NULL;
			BSTR bstrNamespace = (L"root\\cimv2");// 通过 IWbemLocator 和 IWbemServices 这两个 COM 接口访问 WMI, 获取系统信息
			if(CoCreateInstance (CLSID_WbemAdministrativeLocator, NULL,
				CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, IID_IUnknown ,(void**)&pIWbemLocator) 
				== S_OK)
			{
				if(pIWbemLocator->ConnectServer(
					bstrNamespace,  // Namespace
					NULL,			// Userid
					NULL,           // PW
					NULL,           // Locale
					0,              // flags
					NULL,           // Authority
					NULL,           // Context
					&pWbemServices
					) == S_OK)
				{
					HRESULT hRes;
					_bstr_t strQuery = (L"Select * from Win32_OperatingSystem");
					hRes = pWbemServices->ExecQuery(_bstr_t("WQL"), strQuery,WBEM_FLAG_RETURN_IMMEDIATELY,NULL,&pEnumObject);
					if(hRes == S_OK)
					{
						ULONG uCount = 1, uReturned;
						IWbemClassObject * pClassObject = NULL;
						hRes = pEnumObject->Reset();
						if(hRes == S_OK)
						{
							hRes = pEnumObject->Next(WBEM_INFINITE,uCount, &pClassObject, &uReturned);
							if(hRes == S_OK)
							{
								strtmp = "操作系统的名称: "+_getWmiInfo(pClassObject,"Caption")+"\r\n";
								SysInfo = SysInfo + strtmp;
							}
							if (pClassObject != NULL)
								pClassObject->Release();				
						}
					}
					strQuery = (L"Select * from Win32_DiskDrive");
					hRes = pWbemServices->ExecQuery(_bstr_t("WQL"), strQuery,WBEM_FLAG_RETURN_IMMEDIATELY,NULL,&pEnumObject);
					if(hRes == S_OK)
					{
						ULONG uCount = 1, uReturned;
						IWbemClassObject * pClassObject = NULL;
						hRes = pEnumObject->Reset();
						if(hRes == S_OK)
						{
							hRes = pEnumObject->Next(WBEM_INFINITE,uCount, &pClassObject, &uReturned);
							if(hRes == S_OK)
							{
								strtmp = "硬盘的Model: "+_getWmiInfo(pClassObject,"Model")+"\r\n";
								SysInfo = SysInfo + strtmp;
								m_pLogger->WriteLog(LM_INFO, TEXT("硬盘的Model: %s"),_getWmiInfo(pClassObject,"Model").c_str());
							}
							if (pClassObject != NULL)
								pClassObject->Release();				
						}
					}					
					strQuery = (L"Select * from Win32_LogicalDisk");
					hRes = pWbemServices->ExecQuery(_bstr_t("WQL"), strQuery,WBEM_FLAG_RETURN_IMMEDIATELY,NULL,&pEnumObject);
					if(hRes == S_OK)
					{
						ULONG uCount = 1, uReturned;
						IWbemClassObject * pClassObject = NULL;
						hRes = pEnumObject->Reset();
						if(hRes == S_OK)
						{
							while( pEnumObject->Next( WBEM_INFINITE, 
								uCount, &pClassObject, &uReturned) == S_OK )
							{
								if (atoi(_getWmiInfo(pClassObject,"DriveType").c_str()) != 3)
									continue;
								tstring str;
								str =_getWmiInfo(pClassObject,"Name");
								strtmp = "盘符: " +str+"    ";
								m_pLogger->WriteLog(LM_INFO, TEXT("盘符: %s"),str.c_str());
								tstring drv;
								drv += toupper(str[0]);										
								if(m_Hy.IsDriverProtected((char *)drv.c_str()))
								{
									strtmp = strtmp+ "还原状态: 保护 ";
									m_pLogger->WriteLog(LM_INFO, TEXT("还原状态: 保护 "));
								}
								else
								{
									strtmp = strtmp+ "还原状态: 未保护 ";
									m_pLogger->WriteLog(LM_INFO, TEXT("还原状态: 未保护 "));	
								}
								strtmp = strtmp+ "  剩余空间: "+_getWmiInfo(pClassObject,"FreeSpace")+"\r\n";
								SysInfo = SysInfo + strtmp;
								m_pLogger->WriteLog(LM_INFO, TEXT("剩余空间: %s"),_getWmiInfo(pClassObject,"FreeSpace").c_str());
								
							}
							if (pClassObject != NULL)
								pClassObject->Release();				
						}
					}
					strQuery = (L"Select * from Win32_processor");
					hRes = pWbemServices->ExecQuery(_bstr_t("WQL"), strQuery,WBEM_FLAG_RETURN_IMMEDIATELY,NULL,&pEnumObject);
					if(hRes == S_OK)
					{
						ULONG uCount = 1, uReturned;
						IWbemClassObject * pClassObject = NULL;
						hRes = pEnumObject->Reset();
						if(hRes == S_OK)
						{
							hRes = pEnumObject->Next(WBEM_INFINITE,uCount, &pClassObject, &uReturned);
							if(hRes == S_OK)
							{
								strtmp = "处理器的制造厂商: "+_getWmiInfo(pClassObject,"Manufacturer") + "\r\n";
								strtmp = strtmp + "处理器的名称: "+_getWmiInfo(pClassObject,"Caption") + "\r\n";
								SysInfo = SysInfo + strtmp;

								m_pLogger->WriteLog(LM_INFO, TEXT("处理器的制造厂商: %s"),_getWmiInfo(pClassObject,"Manufacturer").c_str());
								m_pLogger->WriteLog(LM_INFO, TEXT("处理器的名称: %s"),_getWmiInfo(pClassObject,"Caption").c_str());
							}
							if (pClassObject != NULL)
								pClassObject->Release();				
						}
					}
					strQuery = (L"Select * from Win32_LogicalMemoryConfiguration");
					hRes = pWbemServices->ExecQuery(_bstr_t("WQL"), strQuery,WBEM_FLAG_RETURN_IMMEDIATELY,NULL,&pEnumObject);
					if(hRes == S_OK)
					{
						ULONG uCount = 1, uReturned;
						IWbemClassObject * pClassObject = NULL;
						hRes = pEnumObject->Reset();
						if(hRes == S_OK)
						{
							hRes = pEnumObject->Next(WBEM_INFINITE,uCount, &pClassObject, &uReturned);
							if(hRes == S_OK)
							{
								strtmp = "物理内存大小: "+_getWmiInfo(pClassObject,"TotalPhysicalMemory") + "\r\n";
								strtmp = strtmp + "页面文件大小: "+_getWmiInfo(pClassObject,"TotalPageFileSpace") + "\r\n";
								strtmp = strtmp + "虚拟内存大小: "+_getWmiInfo(pClassObject,"TotalVirtualMemory") + "\r\n";
								SysInfo = SysInfo + strtmp;

								m_pLogger->WriteLog(LM_INFO, TEXT("物理内存大小: %s"),_getWmiInfo(pClassObject,"TotalPhysicalMemory").c_str());
								m_pLogger->WriteLog(LM_INFO, TEXT("页面文件大小: %s"),_getWmiInfo(pClassObject,"TotalPageFileSpace").c_str());
								m_pLogger->WriteLog(LM_INFO, TEXT("虚拟内存大小: %s"),_getWmiInfo(pClassObject,"TotalVirtualMemory").c_str());
							}
							if (pClassObject != NULL)
								pClassObject->Release();				
						}
					}
					strQuery = (L"Select * from Win32_VideoController");
					hRes = pWbemServices->ExecQuery(_bstr_t("WQL"), strQuery,WBEM_FLAG_RETURN_IMMEDIATELY,NULL,&pEnumObject);
					if(hRes == S_OK)
					{
						ULONG uCount = 1, uReturned;
						IWbemClassObject * pClassObject = NULL;
						hRes = pEnumObject->Reset();
						if(hRes == S_OK)
						{
							hRes = pEnumObject->Next(WBEM_INFINITE,uCount, &pClassObject, &uReturned);
							if(hRes == S_OK)
							{
								strtmp = "显卡名称: "+_getWmiInfo(pClassObject,"Name") + "\r\n";
								SysInfo = SysInfo + strtmp;
								m_pLogger->WriteLog(LM_INFO, TEXT("显卡名称: %s"),_getWmiInfo(pClassObject,"Name").c_str());
							}
							if (pClassObject != NULL)
								pClassObject->Release();				
						}
					}
					strQuery = (L"Select * from Win32_NetworkAdapter where (Netconnectionstatus = 2)");
					hRes = pWbemServices->ExecQuery(_bstr_t("WQL"), strQuery,WBEM_FLAG_RETURN_IMMEDIATELY,NULL,&pEnumObject);
					if(hRes == S_OK)
					{
						ULONG uCount = 1, uReturned;
						IWbemClassObject * pClassObject = NULL;
						hRes = pEnumObject->Reset();
						if(hRes == S_OK)
						{
							while( pEnumObject->Next( WBEM_INFINITE, 
								uCount, &pClassObject, &uReturned) == S_OK )
							{
								tstring str;
								str =_getWmiInfo(pClassObject,"MACAddress");
								if (!str.empty())
								{
									str =_getWmiInfo(pClassObject,"PNPDeviceID");
									size_t pos =  str.find("PCI",0);
									if (pos != tstring::npos)
									{
										tstring str = _getWmiInfo(pClassObject,"Caption");
										size_t pos = str.find(']');
										if (pos != tstring::npos)
											str = str.substr(pos+1);
										strtmp = "网卡名称: "+str + "\r\n";
										SysInfo = SysInfo + strtmp;
										m_pLogger->WriteLog(LM_INFO,  "网卡名称: %s" , str.c_str()) ;	
									}
								}
							}
							if (pClassObject != NULL)
								pClassObject->Release();				
						}
					}
				}
			}
			if (pIWbemLocator)
				pIWbemLocator->Release();
			if (pWbemServices)
				pWbemServices->Release();
			if (pEnumObject)
				pEnumObject->Release();
		}
		CoUninitialize();
		if (!SysInfo.empty())
		{
			m_SysInfo = SysInfo;
		}
		return SysInfo;
	}

	void CCliMgr::InitPackage(_packageheader* header, WORD cmd, int nExtraLen /* = 0 */)
	{
		ZeroMemory(header, sizeof(_packageheader));
		header->StartFlag = START_FLAG;
		header->Cmd       = cmd;
		header->Length    = sizeof(_packageheader) + nExtraLen;
		header->Version   = PRO_VERSION;
		header->Resv[0]	  = OBJECT_CLIENTSVI;
	}

	bool CCliMgr::ExecCmd(LPSTR pOutData, int nOutLen, LPSTR& pInData, int& nInLen, bool bNeedAck /* = true */)
	{
		ICommunication* pICommunication = m_pPlugMgr->GetCommunication();
		CAutoLock<ICommunication> lock(pICommunication);
		if (!pICommunication->ExecCmd(pOutData, nOutLen, pInData, nInLen, bNeedAck))
			return false;

		if (!bNeedAck)
			return true;

		CPackageHelper in(pInData);
		DWORD dwStatus = in.popDWORD();
		if (dwStatus == CMD_RET_FAIL)
		{
			char szError[4096] = {0};
			in.popString(szError);
			m_pLogger->WriteLog(LM_INFO, "Exec Cmd Error:%s", szError);
			CoTaskMemFree(pInData);
			pInData = NULL;
			nInLen = 0;
			return false;
		}
		return true;
	}

	BOOL CCliMgr::RunProcess(LPCSTR lpImage, LPCSTR lpPara)
	{
		if(!lpImage)
		{
			return FALSE;
		}
		HANDLE hToken;
		if(!GetTokenByName(hToken, "explorer.exe"))
		{
			m_pLogger->WriteLog(LM_INFO, TEXT("OpenProcessToken Faile!"));
			return FALSE;
		}
		STARTUPINFO si = {0};
		PROCESS_INFORMATION pi = {0};
		si.cb= sizeof(STARTUPINFO);
		si.lpDesktop = TEXT("winsta0\\default");
 		LPVOID lpEnv = NULL;
 		CreateEnvironmentBlock(&lpEnv, hToken, FALSE);
		BOOL bResult = CreateProcessAsUser(hToken, NULL, (LPSTR)lpPara, NULL, NULL,
			FALSE, CREATE_UNICODE_ENVIRONMENT, lpEnv, NULL, &si, &pi);
		CloseHandle(hToken);
		m_pLogger->WriteLog(LM_INFO, TEXT("%s."),lpPara);
		if(bResult)
		{
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			m_pLogger->WriteLog(LM_INFO, TEXT("CreateProcessAsUser ok."));
		}
		else
		{
			DWORD ir = GetLastError();
			m_pLogger->WriteLog(LM_INFO, TEXT("CreateProcessAsUser false."));
		}
		if (lpEnv)
			DestroyEnvironmentBlock(lpEnv);
		return bResult;
	}

	BOOL CCliMgr::GetTokenByName(HANDLE &hToken, LPSTR lpName)
	{
		if(!lpName)
		{
			return FALSE;
		}
		HANDLE hProcessSnap = NULL; 
		BOOL bRet = FALSE; 
		PROCESSENTRY32 pe32 = {0}; 
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
		if (hProcessSnap == INVALID_HANDLE_VALUE) 
			return FALSE; 
		pe32.dwSize = sizeof(PROCESSENTRY32); 
		if (Process32First(hProcessSnap, &pe32)) 
		{ 
			do 
			{
				if(!strcmpi(pe32.szExeFile, lpName))
				{
					HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,
						FALSE, pe32.th32ProcessID);
					bRet = OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken);
					DWORD ir = GetLastError();
					CloseHandle(hProcessSnap); 
					return (bRet);
				}
			} 
			while (Process32Next(hProcessSnap, &pe32));
			bRet = TRUE;
		}
		else
			bRet = FALSE;
		CloseHandle (hProcessSnap);
		return (bRet);
	}
}

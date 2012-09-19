#include "stdafx.h"
#include <windows.h>
#include <tchar.h>
#include <Shlwapi.h>
#include <process.h>
#include <WinSock2.h>
#include <WinIoCtl.h>
#include "SafeCenter.h"
#include "LogFile.h"

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "i8NE.lib")

HMODULE g_hI8NEModule = NULL;
IGuardEngine* GuardEngine= NULL;
DWORD IOCTL_ICAFE_GUARD  = CTL_CODE(ICAFE_GUARD_TYPE, 0x900, METHOD_BUFFERED, FILE_ANY_ACCESS);
IISecEngine* ISecEngine = NULL; 

BOOL InstallDriver(LPCTSTR lpFileName, LPCTSTR lpServiceName)
{
	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE | SERVICE_CHANGE_CONFIG);
	if (schSCManager == NULL)
	{
		return FALSE;
	}
	TCHAR szDriver[MAX_PATH] = {0};
	GetSystemDirectory(szDriver, MAX_PATH);
	PathAddBackslash(szDriver);
	lstrcat(szDriver, TEXT("drivers\\"));
	lstrcat(szDriver, lpFileName);
	if (!PathFileExists(szDriver))
	{
		TCHAR szFile[MAX_PATH] = {0};
		GetModuleFileName(NULL, szFile, MAX_PATH);
		PathRemoveFileSpec(szFile);
		PathAddBackslash(szFile);
		lstrcat(szFile, lpFileName);
		if (PathFileExists(szFile))
		{
		}
	}

	//如果没有安装服务，则安装服务
	SC_HANDLE schService = OpenService(schSCManager, lpServiceName, SERVICE_ALL_ACCESS);	
	if (schService == NULL && GetLastError() == ERROR_SERVICE_DOES_NOT_EXIST)
	{		
		schService = CreateService(schSCManager, lpServiceName, lpServiceName, SERVICE_ALL_ACCESS, 
			SERVICE_KERNEL_DRIVER, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
			szDriver, NULL, NULL, NULL, NULL, NULL);
	}
	if (schService == NULL)
	{
		CloseServiceHandle(schSCManager);
		return FALSE;
	}

	//修改服务的配置。
	ChangeServiceConfig(schService, SERVICE_KERNEL_DRIVER, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
		szDriver, NULL, NULL, NULL, NULL, NULL, lpServiceName);

	//保证服务运行起来。
	StartService(schService, 0, NULL);

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
	return TRUE;
}

//FARPROC WINAPI GetArpModProcAddress(int nIndex)
//{
//	if (g_hI8NEModule == NULL)
//	{
//		TCHAR path[MAX_PATH] = {0}, szSafeCltPath[MAX_PATH] = {0}, szCurrentPath[MAX_PATH] = {0};
//		GetModuleFileName(NULL, path, MAX_PATH);
//		PathRemoveFileSpec(path);
//		PathAddBackslash(path);	
//		lstrcat(szSafeCltPath, path);
//		lstrcat(szSafeCltPath, TEXT("SafeClt"));
//		lstrcat(path, TEXT("SafeClt\\i8NE.dll"));
//
//		SetDllDirectory(szSafeCltPath);
//		g_hI8NEModule = LoadLibrary(path);
//	}
//	if (g_hI8NEModule)
//	{
//		return GetProcAddress(g_hI8NEModule, (char*)nIndex);
//	}
//	return NULL;
//}
////ARP防御接口调用
//BOOL __stdcall InitArpMod(ARP_ATTACK_CALLBACK pfnAttackCallback, PIP_MAC LocalIpMac, DWORD Count, PIP_MAC GatewayIpMac)
//{
//	PFNINITARPMOD pfnInitArpMod = (PFNINITARPMOD)GetArpModProcAddress(1);
//	if (pfnInitArpMod != NULL)
//	{
//		return pfnInitArpMod(pfnAttackCallback, LocalIpMac, Count, GatewayIpMac);
//	}
//	return FALSE;
//}
//
//BOOL __stdcall ArpModIsOK()
//{
//	PFNARPMODISOK pfnArpModIsOK = (PFNARPMODISOK)GetArpModProcAddress(9);
//	if (pfnArpModIsOK != NULL)
//	{
//		return pfnArpModIsOK();
//	}
//	return FALSE;
//}
//
//BOOL __stdcall StartProtect()
//{
//	PFNSTARTPROTECT pfnStartProtect = (PFNSTARTPROTECT)GetArpModProcAddress(2);
//	if (pfnStartProtect != NULL)
//	{
//		return pfnStartProtect();
//	}
//	return FALSE;
//}
//
//void __stdcall DisableSign()
//{
//	PFNDISABLESIGN pfnDisableSign = (PFNDISABLESIGN)GetArpModProcAddress(5);
//	if (pfnDisableSign != NULL)
//	{
//		pfnDisableSign();
//	}
//}
//
//void __stdcall EnableSign()
//{
//	PFNENABLESIGN pfnEnableSign = (PFNENABLESIGN)GetArpModProcAddress(6);
//	if (pfnEnableSign != NULL)
//	{
//		pfnEnableSign();
//	}
//}
//
//BOOL __stdcall InstallDriver()
//{
//	PFNINSTALLDRIVER pfnInstallDriver = (PFNINSTALLDRIVER)GetArpModProcAddress(7);
//	if (pfnInstallDriver != NULL)
//	{
//		return pfnInstallDriver();
//	}
//	return FALSE;
//}
//
//BOOL __stdcall RemoveDriver()
//{
//	PFNREMOVEDRIVER pfnRemoveDriver = (PFNREMOVEDRIVER)GetArpModProcAddress(8);
//	if (pfnRemoveDriver != NULL)
//	{
//		return pfnRemoveDriver();
//	}
//	return FALSE;
//}
//
//BOOL __stdcall GetGatewayMac(DWORD GatewayAddr, char MacAddr[])
//{
//	PFNGETGATEWAYMAC pfnGetGatewayMac = (PFNGETGATEWAYMAC)GetArpModProcAddress(10);
//	if (pfnGetGatewayMac != NULL)
//	{
//		return pfnGetGatewayMac(GatewayAddr, MacAddr);
//	}
//	return FALSE;
//}
//
//BOOL __stdcall StopProtect()
//{
//	PFNSTOPPROTECT pfnStopProtect = (PFNSTOPPROTECT)GetArpModProcAddress(11);
//	if (pfnStopProtect != NULL)
//	{
//		return pfnStopProtect();
//	}
//	return FALSE;
//}
//
//void __stdcall UnInitArpMod()
//{
//	if (g_hI8NEModule != NULL)
//	{
//		FreeLibrary(g_hI8NEModule);
//		g_hI8NEModule = NULL;
//	}
//}
//DWORD __stdcall CTL_CODE( WORD DeviceType, WORD Function, WORD Method, WORD Access )
//{
//	return (DeviceType << 16) | (Access << 14) | (Function << 2) | Method;
//}

IGuardEngine* __stdcall GuardEngineInstance()
{
	if(GuardEngine == NULL) GuardEngine = new TGuardEngine;
	return GuardEngine;		
}

DWORD __stdcall GetAuthSeed()
{
	DWORD dwResult = GetCurrentProcessId();
	dwResult = dwResult << 13;
	dwResult = dwResult ^ 0x65438088;
	return dwResult;
}

void FreeTokenInformation(void* Buffer)
{
	if (Buffer != NULL)
	{
		delete Buffer;
		Buffer = NULL;
	}
}

void QueryTokenInformation(HANDLE Token, TOKEN_INFORMATION_CLASS InformationClass, void** Buffer)
{
	DWORD  Length, LastError;
	*Buffer = NULL;
	Length = 0;

	BOOL Ret = GetTokenInformation(Token, InformationClass, *Buffer, Length, &Length);
	if (!Ret && (GetLastError() == ERROR_INSUFFICIENT_BUFFER)) 
	{
		*Buffer = new char[Length];
		Ret = GetTokenInformation(Token, InformationClass, *Buffer, Length, &Length);
		if (!Ret)
		{
			LastError = GetLastError();
			FreeTokenInformation(*Buffer);
			SetLastError(LastError);
		}
	}
}

char* SIDToString(PSID ASID)
{
	PSID_IDENTIFIER_AUTHORITY SidIdAuthority;
	DWORD SubAuthorities, SidRev, SidSize;
	int Counter;

	SidRev = SID_REVISION;
	if(IsValidSid(ASID) == FALSE) return NULL;

	SidIdAuthority = GetSidIdentifierAuthority(ASID);
	SubAuthorities = (DWORD)GetSidSubAuthorityCount(ASID);

	SidSize = (15 + 12 + (12 * SubAuthorities) + 1) * sizeof(char);
	char* pszResult = new char[SidSize + 1];
	ZeroMemory(pszResult, SidSize + 1);

	int nIndex = sprintf(pszResult, "S-%u-", SidRev);
	if (SidIdAuthority->Value[0] != 0 || SidIdAuthority->Value[1] != 0)
	{
		nIndex += sprintf(pszResult + nIndex, "0x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x",
			USHORT(SidIdAuthority->Value[0]),
			USHORT(SidIdAuthority->Value[1]),
			USHORT(SidIdAuthority->Value[2]),
			USHORT(SidIdAuthority->Value[3]),
			USHORT(SidIdAuthority->Value[4]),
			USHORT(SidIdAuthority->Value[5]));
	}
	else
	{
		nIndex += sprintf(pszResult + nIndex, "%u",
			ULONG(SidIdAuthority->Value[5])+
			ULONG(SidIdAuthority->Value[4] << 8)+
			ULONG(SidIdAuthority->Value[3] << 16)+
			ULONG(SidIdAuthority->Value[2] << 24));
	}
	for (Counter = 0; Counter < SubAuthorities - 1; Counter++)
	{
		nIndex += sprintf(pszResult + nIndex, "-%u", GetSidSubAuthority(ASID, Counter));
	}

	return pszResult;
}

string RegKeyToKernelRegKey( HKEY RootKey, LPCTSTR Key)
{
	HANDLE hToken;
	void*  TokenUserInfo;
	PTOKEN_USER UserInfo;
	string strReuslt;

	if (RootKey == HKEY_CURRENT_USER)
	{
		if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
		{
			QueryTokenInformation(hToken, TokenUser, &TokenUserInfo);
			UserInfo = (PTOKEN_USER)TokenUserInfo;
			strReuslt = SIDToString(UserInfo->User.Sid);
			FreeTokenInformation(TokenUserInfo);

			if (strReuslt.empty())
			{
				CloseHandle(hToken);
				return strReuslt;
			}
			strReuslt = string("\\Registry\\User\\") + strReuslt + string("\\");
			strReuslt += Key;
			CloseHandle(hToken);	    
		}
	}
	else if (RootKey == HKEY_LOCAL_MACHINE)
	{
		strReuslt = TEXT("\\Registry\\Machine\\");
		strReuslt += Key;
	}
	return strReuslt;
}

IISecEngine* ISecEngineInstance()
{
	if (ISecEngine == NULL)
	{
		ISecEngine = new TISecEngine;
	}
	return ISecEngine;
}

//销毁窗口
void DisposeWindow(char* Buf)
{
	PProhiWindows    FProhiWindows;
	if (Buf)
	{
		FProhiWindows = (PProhiWindows)Buf;
		delete FProhiWindows;
	}
}

//销毁进程
void DisposeProc(char* Buf)
{
	PProhiProc FProhiProc;
	if (Buf)
	{
		FProhiProc = (PProhiProc)Buf;
		delete FProhiProc;
	}	
}

//销毁进程链表
void DisposeProcList(char* Buf)
{
	PProcessInfo    FProInfo;
	if (Buf)
	{
		FProInfo = (PProcessInfo)Buf;
		delete FProInfo;
	}
}

BOOL CALLBACK EnumWindowsFunc(HWND Handle, LPARAM Param)
{
	TProhiThread* pThread = (TProhiThread*)Param;
	char          Caption[MAX_PATH] = {0};

	if( GetWindowText(Handle, Caption, MAX_PATH) != 0 )
	{
		PWindowsInfo  FWindowsInfo = new TWindowsInfo;
		strcpy(FWindowsInfo->WindowsName, Caption);
		pThread->m_WndList.push_back(FWindowsInfo);
	}
	return TRUE;
}

void DisposeWinInfo(char* Buf)
{
	PWindowsInfo FWindowsInfo;
	if (Buf)
	{
		FWindowsInfo = (PWindowsInfo)Buf;
		delete FWindowsInfo;
	}
}


char AttckType[][MAX_PATH] = 
{
	TEXT("无攻击行为"),
	TEXT("伪造网关查询包攻击"),
	TEXT("伪造网关回应包攻击"),
	TEXT("局域网IP冲突攻击"),
	TEXT("广域网IP冲突攻击"),
	TEXT("错误的协议攻击"),
	TEXT("未知攻击类型")
};
void __stdcall Arp_Attck_Callback(PATTACK_INFO AttackInfo, BOOL IsKnownIP)
{
	//过滤无攻击行为
	//if (AttackInfo->AType == ATTACH_NONE) return;
	//char szNotify[MAX_PATH];
	//in_addr iAddr;
	//ZeroMemory(szNotify, MAX_PATH);	

	//CopyMemory(&iAddr, &(AttackInfo->AttackByIp), 4);
	//sprintf(szNotify, "ARP防御检测到攻击,攻击源IP:[%s] 源MAC:[%0.2x-%0.2x-%0.2x-%0.2x-%0.2x-%0.2x] 攻击类型[%s]\n", 
	//	inet_ntoa(iAddr), AttackInfo->AttackByMac[0], AttackInfo->AttackByMac[1], AttackInfo->AttackByMac[2],
	//	AttackInfo->AttackByMac[3], AttackInfo->AttackByMac[4], AttackInfo->AttackByMac[5], AttckType[(int)AttackInfo->AType]);

	//OutputDebugString(szNotify);
}

bool TGuardEngine::ClientLiveIt()
{
	TParams Params;
	DWORD   BytesReturned;

	if(GEHandle == INVALID_HANDLE_VALUE) return false;
	Params.CtlCode = IOCTL_CHECK_LIVE_IT;
	Params.InputBufferLength = 0;
	Params.InputBuffer = NULL;
	Params.OutputBufferLength = 0;
	Params.OutputBuffer = NULL;
	Params.BytesReturned = NULL;
	return DeviceIoControl(GEHandle,
		IOCTL_ICAFE_GUARD,
		&Params,
		sizeof(Params),
		NULL,
		0,
		&BytesReturned,
		NULL);
}

bool TGuardEngine::Connect(DWORD Seed)
{
	TParams Params;
	DWORD   BytesReturned;

	GEHandle = CreateFile(TEXT("\\.\i8SecR"),
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);
	if(GEHandle == INVALID_HANDLE_VALUE) return false;
	ZeroMemory(&Params, sizeof(Params));
	Params.CtlCode = IOCTL_INIT;
	Params.InputBufferLength = 4;
	Params.InputBuffer = &Seed;
	Params.OutputBufferLength = 0;
	Params.OutputBuffer = NULL;
	Params.BytesReturned = NULL;

	return DeviceIoControl(GEHandle,
		IOCTL_ICAFE_GUARD,
		(void*)&Params,
		sizeof(Params),
		NULL,
		0,
		&BytesReturned,
		NULL);
}

//bool TGuardEngine::DisableKey(tstring Keys[])
//{
//	return true;
//}

bool TGuardEngine::RebootNotify()
{
	TParams Params;
	DWORD   BytesReturned;

	if(GEHandle == INVALID_HANDLE_VALUE) return false;

	ZeroMemory(&Params, sizeof(Params));
	Params.CtlCode = IOCTL_NOTIFY_SHUTDOWN;
	Params.InputBufferLength = 0;
	Params.InputBuffer = NULL;
	Params.OutputBufferLength = 0;
	Params.OutputBuffer = NULL;
	Params.BytesReturned = NULL;

	return DeviceIoControl(GEHandle,
		IOCTL_ICAFE_GUARD,
		(void*)&Params,
		sizeof(Params),
		NULL,
		0,
		&BytesReturned,
		NULL);
}

bool TGuardEngine::ProtectMe()
{
	TParams Params;
	DWORD   BytesReturned;
	TProtSetting ProtSetting;

	if(GEHandle == INVALID_HANDLE_VALUE) return false;
	ZeroMemory(&Params, sizeof(Params));

	ProtSetting.ProtMaskCode = PROT_MASK_PROCESS;

	Params.CtlCode = IOCTL_PROT_ME;
	Params.InputBufferLength = sizeof(TProtSetting);
	Params.InputBuffer = &ProtSetting;
	Params.OutputBufferLength = 0;
	Params.OutputBuffer = NULL;
	Params.BytesReturned = NULL;

	return DeviceIoControl(GEHandle,
		IOCTL_ICAFE_GUARD,
		&Params,
		sizeof(Params),
		NULL,
		0,
		&BytesReturned,
		NULL);
}

bool TGuardEngine::Reboot()
{
	TParams Params;
	DWORD   BytesReturned;

	if(GEHandle == INVALID_HANDLE_VALUE) return false;

	ZeroMemory(&Params, sizeof(Params));
	Params.CtlCode = IOCTL_REBOOT;
	Params.InputBufferLength = 0;
	Params.InputBuffer = NULL;
	Params.OutputBufferLength = 0;
	Params.OutputBuffer = NULL;
	Params.BytesReturned = NULL;

	return DeviceIoControl(GEHandle,
		IOCTL_ICAFE_GUARD,
		(void*)&Params,
		sizeof(Params),
		NULL,
		0,
		&BytesReturned,
		NULL);
}

bool TGuardEngine::RegKeyCanWrite(bool CanWrite, HKEY RootKey, LPCTSTR Key)
{
	TParams Params;
	DWORD   BytesReturned;
	TRegKeyProt RegKeyProt;
	string KernelRegKey;

	if(GEHandle == INVALID_HANDLE_VALUE) return false;

	KernelRegKey = RegKeyToKernelRegKey(RootKey, Key);
	ZeroMemory(&Params, sizeof(Params));
	if (CanWrite)
	{
		RegKeyProt.CanWrite = 1;
	}
	CopyMemory(&(RegKeyProt.RegKey[1]), KernelRegKey.c_str(), KernelRegKey.length() > MAX_PATH - 1 ? MAX_PATH -1 : KernelRegKey.length());

	Params.CtlCode = IOCTL_REGKEY_CAN_WRITE;
	Params.InputBufferLength = sizeof(TRegKeyProt);
	Params.InputBuffer = &RegKeyProt;
	Params.OutputBufferLength = 0;
	Params.OutputBuffer = NULL;
	Params.BytesReturned = NULL;

	return DeviceIoControl(GEHandle,
		IOCTL_ICAFE_GUARD,
		(void*)&Params,
		sizeof(Params),
		NULL,
		0,
		&BytesReturned,
		NULL);
}

DWORD TGuardEngine::Subsist()
{
	TParams Params;
	DWORD   BytesReturned;
	TProtSetting ProtSetting ;
	DWORD  Value;
	bool bRet;

	if(GEHandle == INVALID_HANDLE_VALUE) return false;
	ZeroMemory(&Params, sizeof(Params));
	ProtSetting.ProtMaskCode = PROT_MASK_PROCESS;

	Value  = 0;
	Params.CtlCode  = IOCTL_DRV_SUBSIST;
	Params.InputBufferLength  = 0;
	Params.InputBuffer  = NULL;
	Params.OutputBufferLength  = sizeof(Value);
	Params.OutputBuffer  = &Value;
	Params.BytesReturned = NULL;

	bRet = DeviceIoControl(GEHandle,
		IOCTL_ICAFE_GUARD,
		(void*)&Params,
		sizeof(Params),
		NULL,
		0,
		&BytesReturned,
		NULL);

	if(!bRet) return 0;

	if( Value == ((GetCurrentProcessId() << 2) ^ 0x98768088) )
	{
		return 2;
	}
	else
	{
		return 1;
	}
}

bool TGuardEngine::TerminateProcess(DWORD Pid)
{
	TParams Params;
	DWORD   BytesReturned;
	DWORD   ProcessId;
	if(GEHandle == INVALID_HANDLE_VALUE) return false;

	ProcessId = Pid;
	Params.CtlCode = IOCTL_END_PROCESS;
	Params.InputBufferLength = sizeof(DWORD);
	Params.InputBuffer = &ProcessId;
	Params.OutputBufferLength = 0;
	Params.OutputBuffer = NULL;
	Params.BytesReturned = NULL;

	return DeviceIoControl(GEHandle,
		IOCTL_ICAFE_GUARD,
		(void*)&Params,
		sizeof(Params),
		NULL,
		0,
		&BytesReturned,
		NULL);
}

bool TGuardEngine::GetData(TCHAR Data[], DWORD DataLength)
{
	TParams Params;
	DWORD   BytesReturned;
	if(GEHandle == INVALID_HANDLE_VALUE) return false;
	ZeroMemory(&Params, sizeof(Params));

	Params.CtlCode = IOCTL_GET_DATA;
	Params.InputBufferLength = 0;
	Params.InputBuffer = NULL;
	Params.OutputBufferLength = DataLength;
	Params.OutputBuffer = Data;
	Params.BytesReturned = NULL;

	return DeviceIoControl(GEHandle,
		IOCTL_ICAFE_GUARD,
		(void*)&Params,
		sizeof(Params),
		NULL,
		0,
		&BytesReturned,
		NULL);
}

bool TGuardEngine::SetData(TCHAR Data[], DWORD DataLength)
{
	TParams Params;
	DWORD   BytesReturned;
	if(GEHandle == INVALID_HANDLE_VALUE || Data == NULL || DataLength == 0) return false;
	ZeroMemory(&Params, sizeof(Params));

	Params.CtlCode = IOCTL_SET_DATA;
	Params.InputBufferLength = DataLength;
	Params.InputBuffer = Data;
	Params.OutputBufferLength = 0;
	Params.OutputBuffer = NULL;
	Params.BytesReturned = NULL;

	return DeviceIoControl(GEHandle,
		IOCTL_ICAFE_GUARD,
		(void*)&Params,
		sizeof(Params),
		NULL,
		0,
		&BytesReturned,
		NULL);
}

bool TGuardEngine::DisableLiveCheck()
{
	TParams Params;
	DWORD   BytesReturned;
	if(GEHandle == INVALID_HANDLE_VALUE ) return false;
	ZeroMemory(&Params, sizeof(Params));

	Params.CtlCode = IOCTL_DISABLE_CHECK_LIVE_IT;
	Params.InputBufferLength = 0;
	Params.InputBuffer = NULL;
	Params.OutputBufferLength = 0;
	Params.OutputBuffer = NULL;
	Params.BytesReturned = NULL;

	return DeviceIoControl(GEHandle,
		IOCTL_ICAFE_GUARD,
		(void*)&Params,
		sizeof(Params),
		NULL,
		0,
		&BytesReturned,
		NULL);
}
bool __stdcall TISecEngine::AddRuleFile(const std::string FileName, DWORD ProtectAttributes)
{
	HRESULT         hResult;
	PCommandMessage commandMessage ;
	DWORD           BytesReturned;
	DWORD           NeedSize;
	string          NtName;
	TRuleDetail     Detail;

	hResult = FALSE;
	ZeroMemory(&Detail, sizeof(Detail));
	if (IsFullPath(FileName))
	{
		NtName = FileNameToNtName(FileName);
		Detail.RuleFile.FileNameType = FileNameTypeFull;
	}
	else if (IsRelativePath(FileName))
	{
		NtName = FileName;
		Detail.RuleFile.FileNameType = FileNameTypeRelative;
	}
	else 
	{
		NtName = FileName;
		Detail.RuleFile.FileNameType = FileNameTypeOnly;
	}

	if (NtName.empty() || NtName.length() > MAX_PATH * 2)
	{
		return false;
	}

	NeedSize =  sizeof(TCommandMessage) + sizeof(Detail);
	commandMessage = (PCommandMessage)new char[NeedSize];
	ZeroMemory(commandMessage, NeedSize);

	try
	{
		CopyMemory(&Detail.RuleFile.FileName, NtName.c_str(), NtName.length());
		Detail.RuleFile.Attributes = ProtectAttributes;

		commandMessage->Command = cmdAddRuleFile;
		CopyMemory(&(commandMessage->Data), &Detail, sizeof(Detail));
		hResult = FilterSendMessage(Port,
			commandMessage,
			NeedSize,
			NULL,
			0,
			&BytesReturned);

		if (hResult == S_OK)
		{
			delete []commandMessage;
			return true;
		}
	}
	catch(...)
	{
		delete []commandMessage;
	}
	return true;
}

bool __stdcall TISecEngine::AddRuleFolder(const std::string FolderName, DWORD ProtectAttributes)
{
	HRESULT         hResult;
	PCommandMessage commandMessage ;
	DWORD           BytesReturned;
	DWORD           NeedSize;
	string          NtName;
	TRuleDetail     Detail;

	hResult = FALSE;
	ZeroMemory(&Detail, sizeof(Detail));
	if (IsFullPath(FolderName))
	{
		NtName = FileNameToNtName(FolderName);
		Detail.RuleFolder.FolderNameType = FolderNameTypeFull;
	}
	else if (IsRelativePath(FolderName))
	{
		return false;
	}
	else 
	{
		NtName = FolderName;
		Detail.RuleFolder.FolderNameType = FolderNameTypeOnly;
	}

	if (NtName.empty() || NtName.length() > MAX_PATH * 2)
	{
		return false;
	}

	NeedSize =  sizeof(TCommandMessage) + sizeof(Detail);
	commandMessage = (PCommandMessage)new char[NeedSize];
	ZeroMemory(commandMessage, NeedSize);

	try
	{
		CopyMemory(&Detail.RuleFolder.FolderName, NtName.c_str(), NtName.length());
		Detail.RuleFolder.Attributes = ProtectAttributes;

		commandMessage->Command = cmdAddRuleFolder;
		CopyMemory(&(commandMessage->Data), &Detail, sizeof(Detail));
		hResult = FilterSendMessage(Port,
			commandMessage,
			NeedSize,
			NULL,
			0,
			&BytesReturned);

		if (hResult == S_OK)
		{
			delete []commandMessage;
			return true;
		}
	}
	catch(...)
	{
		delete []commandMessage;
	}
	return true;
}

bool __stdcall TISecEngine::AddRuleProcess(const string ProcessName)
{
	HRESULT         hResult;
	PCommandMessage commandMessage ;
	DWORD           BytesReturned;
	DWORD           NeedSize;
	string          NtName;
	TRuleDetail     Detail;

	hResult = FALSE;
	ZeroMemory(&Detail, sizeof(Detail));
	if (IsFullPath(ProcessName))
	{
		NtName = ProcessName;
		Detail.RuleProcessProt.FileNameType = FileNameTypeFull;
	}
	else if (IsRelativePath(ProcessName))
	{
		return false;
	}
	else 
	{
		NtName = ProcessName;
		Detail.RuleProcessProt.FileNameType = FileNameTypeOnly;
	}

	if (NtName.empty() || NtName.length() > MAX_PATH * 2)
	{
		return false;
	}

	NeedSize =  sizeof(TCommandMessage) + sizeof(Detail);
	commandMessage = (PCommandMessage)new char[NeedSize];
	ZeroMemory(commandMessage, NeedSize);

	try
	{
		CopyMemory(&Detail.RuleProcessProt.ImageName, NtName.c_str(), NtName.length());
		commandMessage->Command = cmdAddRuleProcess;
		CopyMemory(&(commandMessage->Data), &Detail, sizeof(Detail));
		hResult = FilterSendMessage(Port,
			commandMessage,
			NeedSize,
			NULL,
			0,
			&BytesReturned);

		if (hResult == S_OK)
		{
			delete []commandMessage;
			return true;
		}
	}
	catch(...)
	{
		delete []commandMessage;
	}
	return true;
}

bool __stdcall TISecEngine::AddTrustedApp(const string FileName)
{
	HRESULT         hResult;
	PCommandMessage commandMessage ;
	DWORD           BytesReturned;
	DWORD           NeedSize;
	TRuleDetail     Detail;

	hResult = FALSE;
	ZeroMemory(&Detail, sizeof(Detail));

	NeedSize =  sizeof(TCommandMessage) + sizeof(Detail);
	commandMessage = (PCommandMessage)new char[NeedSize];
	ZeroMemory(commandMessage, NeedSize);

	try
	{
		CopyMemory(&Detail.RuleTrustedApp.ImageName, FileName.c_str(), FileName.length());
		commandMessage->Command = cmdAddTrustedApp;
		CopyMemory(&(commandMessage->Data), &Detail, sizeof(Detail));
		hResult = FilterSendMessage(Port,
			commandMessage,
			NeedSize,
			NULL,
			0,
			&BytesReturned);

		if (hResult == S_OK)
		{
			delete []commandMessage;
			return true;
		}
	}
	catch(...)
	{
		delete []commandMessage;
	}
	return true;
}

bool __stdcall TISecEngine::ClearRuleFile()
{
	HRESULT hResult;
	PCommandMessage commandMessage ;
	DWORD           BytesReturned;

	hResult = S_FALSE;
	commandMessage->Command = cmdClearRuleFile;
	hResult = FilterSendMessage(Port,
		&commandMessage,
		sizeof(TCommandMessage),
		NULL,
		0,
		&BytesReturned);
	if(hResult == S_OK) return true;
	else return false;
}

bool __stdcall TISecEngine::ClearRuleFolder()
{
	HRESULT hResult;
	PCommandMessage commandMessage ;
	DWORD           BytesReturned;

	hResult = S_FALSE;
	commandMessage->Command = cmdClearRuleFolder;
	hResult = FilterSendMessage(Port,
		&commandMessage,
		sizeof(TCommandMessage),
		NULL,
		0,
		&BytesReturned);
	if(hResult == S_OK) return true;
	else return false;
}

bool __stdcall TISecEngine::ClearRuleProcess()
{
	HRESULT hResult;
	PCommandMessage commandMessage ;
	DWORD           BytesReturned;

	hResult = S_FALSE;
	commandMessage->Command = cmdClearRuleProcess;
	hResult = FilterSendMessage(Port,
		&commandMessage,
		sizeof(TCommandMessage),
		NULL,
		0,
		&BytesReturned);
	if(hResult == S_OK) return true;
	else return false;
}

bool __stdcall TISecEngine::ClearTrustedApp()
{
	HRESULT hResult;
	PCommandMessage commandMessage ;
	DWORD           BytesReturned;

	hResult = S_FALSE;
	commandMessage->Command = cmdClearTrustedApp;
	hResult = FilterSendMessage(Port,
		&commandMessage,
		sizeof(TCommandMessage),
		NULL,
		0,
		&BytesReturned);
	if(hResult == S_OK) return true;
	else return false;
}


bool __stdcall TISecEngine::Connect(DWORD Seed)
{
	HRESULT hResult;

	if (Port != INVALID_HANDLE_VALUE)
	{
		hResult = S_OK;
		return true;
	}
	hResult = S_FALSE;
	hResult = FilterConnectCommunicationPort((WCHAR*)TEXT("\\i8SecFPort"), 0, &Seed, sizeof(Seed), NULL, &Port);
	if (hResult == S_OK)
	{
		return true;
	}
	return false;
}

bool __stdcall TISecEngine::DisableTimeProtect()
{
	HRESULT hResult ;
	TCommandMessage commandMessage ;
	DWORD BytesReturned;

	hResult = S_FALSE;
	commandMessage.Command = cmdDisableTimeProt;
	hResult = FilterSendMessage(Port,
		&commandMessage,
		sizeof(TCommandMessage),
		NULL,
		0,
		&BytesReturned);
	if(hResult == S_OK) return true;
	else return false;
}

bool __stdcall TISecEngine::EnabledTimeProtect()
{
	HRESULT hResult ;
	TCommandMessage commandMessage ;
	DWORD BytesReturned;

	hResult = S_FALSE;
	commandMessage.Command = cmdEnableTimeProt;
	hResult = FilterSendMessage(Port,
		&commandMessage,
		sizeof(TCommandMessage),
		NULL,
		0,
		&BytesReturned);
	if(hResult == S_OK) return true;
	else return false;
}

string __stdcall TISecEngine::FileNameToNtName(const std::string FileName)
{
	char Drive ;
	string DirPart;
	string FilePart;
	string DrivePart;
	char DosName[512];
	string strResult;

	DrivePart = FileName[0] + string(TEXT(":"));

	ZeroMemory(&DosName, 512);
	QueryDosDevice(DrivePart.c_str(), DosName, 512);

	//需要修改
	strResult = DosName;
	strResult += &(FileName[2]);
	return strResult;
}

DWORD __stdcall TISecEngine::GetStatus()
{
	HRESULT hResult ;
	TCommandMessage commandMessage;
	DWORD BytesReturned;
	DWORD Status;
	DWORD result;

	result = STATUS_NOPROTECT_ACTIVE;
	Status = 0;
	commandMessage.Command = cmdGetStatus;
	hResult = FilterSendMessage(Port,
		&commandMessage,
		sizeof(TCommandMessage),
		&Status,
		0,
		&BytesReturned);
	if(hResult == S_OK) return Status;
	else return 0;
}

bool __stdcall TISecEngine::FileProtResume()
{
	HRESULT hResult ;
	TCommandMessage commandMessage ;
	DWORD BytesReturned;

	hResult = S_FALSE;
	commandMessage.Command = cmdFileProtStart;
	hResult = FilterSendMessage(Port,
		&commandMessage,
		sizeof(TCommandMessage),
		NULL,
		0,
		&BytesReturned);
	if(hResult == S_OK) return true;
	else return false;
}

bool __stdcall TISecEngine::FileProtStop()
{
	HRESULT hResult ;
	TCommandMessage commandMessage ;
	DWORD BytesReturned;

	hResult = S_FALSE;
	commandMessage.Command = cmdFileProtStop;
	hResult = FilterSendMessage(Port,
		&commandMessage,
		sizeof(TCommandMessage),
		NULL,
		0,
		&BytesReturned);
	if(hResult == S_OK) return true;
	else return false;
}

bool __stdcall TISecEngine::FolderProtResume()
{
	HRESULT hResult ;
	TCommandMessage commandMessage ;
	DWORD BytesReturned;

	hResult = S_FALSE;
	commandMessage.Command = cmdFolderProtStart;
	hResult = FilterSendMessage(Port,
		&commandMessage,
		sizeof(TCommandMessage),
		NULL,
		0,
		&BytesReturned);
	if(hResult == S_OK) return true;
	else return false;
}

bool __stdcall TISecEngine::FolderProtStop()
{
	HRESULT hResult ;
	TCommandMessage commandMessage ;
	DWORD BytesReturned;

	hResult = S_FALSE;
	commandMessage.Command = cmdFolderProtStop;
	hResult = FilterSendMessage(Port,
		&commandMessage,
		sizeof(TCommandMessage),
		NULL,
		0,
		&BytesReturned);
	if(hResult == S_OK) return true;
	else return false;
}


bool __stdcall TISecEngine::ProcessProtResume()
{
	HRESULT hResult ;
	TCommandMessage commandMessage ;
	DWORD BytesReturned;

	hResult = S_FALSE;
	commandMessage.Command = cmdProcessProtStart;
	hResult = FilterSendMessage(Port,
		&commandMessage,
		sizeof(TCommandMessage),
		NULL,
		0,
		&BytesReturned);
	if(hResult == S_OK) return true;
	else return false;
}

bool __stdcall TISecEngine::ProcessProtStop()
{
	HRESULT hResult ;
	TCommandMessage commandMessage ;
	DWORD BytesReturned;

	hResult = S_FALSE;
	commandMessage.Command = cmdProcessProtStop;
	hResult = FilterSendMessage(Port,
		&commandMessage,
		sizeof(TCommandMessage),
		NULL,
		0,
		&BytesReturned);
	if(hResult == S_OK) return true;
	else return false;
}


bool __stdcall TISecEngine::IsFullPath(const std::string FileName)
{
	char szDriver[MAX_PATH];
	ZeroMemory(szDriver, MAX_PATH);

	DWORD dwLen = GetLogicalDriveStrings(MAX_PATH, szDriver);
	if(dwLen > 0 && dwLen < MAX_PATH)
	{
		UINT uiDrvType = GetDriveType(szDriver);
		if (uiDrvType == DRIVE_UNKNOWN || uiDrvType == DRIVE_NO_ROOT_DIR)
		{
			return false;
		}
		return true;
	}
	return false;
}

bool __stdcall TISecEngine::IsRelativePath(const string FileName)
{
	char szFileName[MAX_PATH];
	CopyMemory(szFileName, FileName.c_str(), FileName.length());
	if(PathRemoveFileSpec(szFileName)) return true;
	else return false;
}

bool __stdcall TISecEngine::EnumDrivers(PDriverInfoEntry DriversInfo, DWORD DriversInfoLength, DWORD &ReturnLength)
{
	HRESULT hResult ;
	TCommandMessage commandMessage;
	DWORD BytesReturned;
	DWORD Status;

	if (Port == INVALID_HANDLE_VALUE) return false;

	Status = 0;
	BytesReturned = 0;
	commandMessage.Command = cmdGetDrivers;
	hResult = FilterSendMessage(Port,
		&commandMessage,
		sizeof(TCommandMessage),
		DriversInfo,
		DriversInfoLength,
		&BytesReturned);
	ReturnLength = BytesReturned;
	if(BytesReturned > 0) return true;
	return false;
}

bool __stdcall TISecEngine::IsConnected()
{
	if (Port == INVALID_HANDLE_VALUE) return false;
	else return true;
}

bool __stdcall TISecEngine::EnumProcesses(PProcessInfoEntry ProcessInfo, DWORD ProcessInfoLength, DWORD &ReturnLength)
{
	HRESULT hResult ;
	TCommandMessage commandMessage;
	DWORD BytesReturned;
	DWORD Status;

	if (Port == INVALID_HANDLE_VALUE) return false;

	Status = 0;
	BytesReturned = 0;
	commandMessage.Command = cmdGetProcesses;
	hResult = FilterSendMessage(Port,
		&commandMessage,
		sizeof(TCommandMessage),
		ProcessInfo,
		ProcessInfoLength,
		&BytesReturned);
	ReturnLength = BytesReturned;
	if(BytesReturned > 0) return true;
	return false;
}

bool __stdcall TISecEngine::TerminateProcess(DWORD Pid)
{
	HRESULT hResult ;
	PCommandMessage commandMessage ;
	DWORD BytesReturned;
	DWORD NeedSize;

	hResult = S_FALSE;
	NeedSize = sizeof(TCommandMessage) + sizeof(Pid);
	commandMessage = (PCommandMessage)new char[NeedSize];
	ZeroMemory(commandMessage, NeedSize);
	try
	{
		commandMessage->Command = cmdTerminateProcess;
		CopyMemory(&(commandMessage->Data), &Pid, sizeof(Pid));
		hResult = FilterSendMessage(Port,
			commandMessage,
			NeedSize,
			NULL,
			0,
			&BytesReturned);

		if (hResult == S_OK)
		{
			delete []commandMessage;
			return true;
		}
	}
	catch(...)
	{
		delete []commandMessage;
	}
	return true;
}

bool __stdcall EnableProcessPrivilege(const bool Enable, const string Privilege)
{
	HANDLE Token;
	TOKEN_PRIVILEGES TokenPriv;

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &Token))
	{
		TokenPriv.PrivilegeCount = 1; 
		LookupPrivilegeValue(NULL, Privilege.c_str(), &TokenPriv.Privileges[0].Luid);
		TokenPriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 	
		AdjustTokenPrivileges(Token, FALSE, &TokenPriv, 0, (PTOKEN_PRIVILEGES)NULL, 0); 
		CloseHandle(Token);

		return GetLastError() == ERROR_SUCCESS;
	}
	return false;
}

bool __stdcall TISecEngine::SetDateTime(SYSTEMTIME sysDateTime )
{
	HRESULT hResult ;
	PCommandMessage commandMessage ;
	DWORD BytesReturned;
	DWORD NeedSize;
	FILETIME FileTimeSrc;
	FILETIME FileTimeDes;

	EnableProcessPrivilege(true, string("SeSystemtimePrivilege"));

	SystemTimeToFileTime(&sysDateTime, &FileTimeSrc);
	LocalFileTimeToFileTime(&FileTimeSrc, &FileTimeDes);
	NeedSize = sizeof(TCommandMessage) + sizeof(FILETIME);
	commandMessage = (PCommandMessage)new char[NeedSize];
	ZeroMemory(commandMessage, NeedSize);
	try
	{	
		commandMessage->Command = cmdSetTime;
		CopyMemory(&(commandMessage->Data), &FileTimeDes, sizeof(FILETIME));
		hResult = FilterSendMessage(Port,
			commandMessage,
			NeedSize,
			NULL,
			0,
			&BytesReturned);

		if (hResult == S_OK)
		{
			delete []commandMessage;
			return true;
		}
	}
	catch(...)
	{
		delete []commandMessage;
	}
    return false;
}
TProhiThread::TProhiThread()
{
	InitializeCriticalSection(&FCS);
	FEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hThread = INVALID_HANDLE_VALUE;
	FFirst = NULL;
	FLast = NULL;
}

TProhiThread::~TProhiThread()
{
	DeleteCriticalSection(&FCS);
	for(vector<PProhiProc>::iterator it = FProhiProcList.begin(); it != FProhiProcList.end(); it++)
	{
		delete *it;
	}
	for(vector<PProhiWindows>::iterator it = FProhiWindowsList.begin(); it != FProhiWindowsList.end(); it++)
	{
		delete *it;
	}
	FProhiProcList.clear();
	FProhiWindowsList.clear();
	PMsgBuffer pBuffer = FFirst;
	PMsgBuffer pTemp = NULL;
	while(pBuffer)
	{
		pTemp = pBuffer->Next;
		delete pBuffer->Buffer;
		delete pBuffer;
		pBuffer = pTemp;
	}
	CloseHandle(FEvent);
	CloseHandle(m_hThread);
};

void TProhiThread::AddQueue(TOperType OperType, TDataType DataType, char *Data, int Datalen)
{
	PMsgBuffer p_Buffer;
	p_Buffer = new TMsgBuffer;
	p_Buffer->Buffer = new char[Datalen];
	p_Buffer->DataType = DataType;
	p_Buffer->OperType = OperType;
	CopyMemory(p_Buffer->Buffer, Data, Datalen);
	p_Buffer->BufferLen = Datalen;
	p_Buffer->Next = NULL;
	EnterCriticalSection(&FCS);

	if (FFirst)
	{
		FLast->Next = p_Buffer;
	}
	else
	{
		FFirst = p_Buffer;
	}
	FLast = p_Buffer;
	LeaveCriticalSection(&FCS);
}

void TProhiThread::AddData(TDataType DataType, std::string Name, std::string Path)
{
	TProhiWindows FProhiWindows;
	TProhiProc FProhiProc;

	switch(DataType)
	{
	case uProc:
		{
			strcpy(FProhiProc.ProcessName, Name.c_str());
			strcpy(FProhiProc.ProcessPath, Path.c_str());
			AddQueue(uAdd, DataType, (char*)&FProhiProc, sizeof(FProhiProc));
			break;
		}
	case uWindows:
		{
			strcpy(FProhiWindows.WindowsName, Name.c_str());
			AddQueue(uAdd, DataType, (char*)&FProhiWindows, sizeof(FProhiWindows));
			break;
		}
	}
}

void TProhiThread::AddInfo(TDataType DataType, char *Buffer, int Bufferlen)
{
	PProhiWindows FProhiWindows;
	PProhiProc FProhiProc;

	switch(DataType)
	{
	case uProc:
		{
			FProhiProc = new TProhiProc;
			strcpy(FProhiProc->ProcessName, ((PProhiProc)Buffer)->ProcessName);
			strcpy(FProhiProc->ProcessPath, ((PProhiProc)Buffer)->ProcessPath);
			FProhiProcList.push_back(FProhiProc);
			break;
		}
	case uWindows:
		{
			FProhiWindows = new TProhiWindows;
			strcpy(FProhiWindows->WindowsName, ((PProhiWindows)Buffer)->WindowsName);
			FProhiWindowsList.push_back(FProhiWindows);
			break;
		}
	}
}

void TProhiThread::Clear(TDataType DataType)
{
	switch(DataType)
	{
	case uProc:
		{
			vector<PProhiProc>::iterator item = FProhiProcList.begin();
			for(;item != FProhiProcList.end();item++)
			{
				delete *item;
			}
			FProhiProcList.clear();
			break;
		}
	case uWindows:
		{
			vector<PProhiWindows>::iterator item = FProhiWindowsList.begin();
			for(;item != FProhiWindowsList.end();item++)
			{
				delete *item;
			}
			FProhiWindowsList.clear();
			break;
		}
	}
}

void TProhiThread::ClearData(TDataType DataType)
{
	AddQueue(uClear, DataType, NULL, 0);
}

void TProhiThread::DelData(TDataType DataType, string Name, string Path)
{
	TProhiWindows FProhiWindows;
	TProhiProc    FProhiProc;
	switch(DataType)
	{
	case uWindows:
		{
			strcpy(FProhiProc.ProcessName, Name.c_str());
			strcpy(FProhiProc.ProcessPath, Path.c_str());
			AddQueue(uDel, DataType, (char*)&FProhiProc, sizeof(FProhiProc));
			break;
		}
	case uProc:
		{
			strcpy(FProhiWindows.WindowsName, Name.c_str());
			AddQueue(uDel, DataType, (char*)&FProhiWindows, sizeof(FProhiWindows));
			break;
		}
	}
}

//
void TProhiThread::DelInfo(TDataType DataType, char *Buffer, int Bufferlen)
{
	int i;
	PProhiWindows FProhiWindows;
	PProhiProc    FProhiProc;
	switch(DataType)
	{
	case uProc:
		{
			for (vector<PProhiProc>::iterator iter = FProhiProcList.begin(); iter != FProhiProcList.end(); ++iter)
			{
				FProhiProc = *iter;
				if (strcmp(FProhiProc->ProcessName, ((PProhiProc)Buffer)->ProcessName) == 0)
				{	
					FProhiProcList.erase(iter);
					delete FProhiProc;
					FProhiProc = NULL;
				}
			}
		}
	case uWindows:
		{
			for (vector<PProhiWindows>::iterator iter = FProhiWindowsList.begin(); iter != FProhiWindowsList.end(); ++iter)
			{
				FProhiWindows = *iter;
				if (strcmp(FProhiWindows->WindowsName, ((PProhiWindows)Buffer)->WindowsName) == 0)
				{	
					FProhiWindowsList.erase(iter);
					delete FProhiWindows;
					FProhiWindows = NULL;
				}
			}
		}
	}
}

void TProhiThread::PollingTask()
{
	DWORD  FCurrTick = GetTickCount();
	if (FCurrTick - FTick >= POOLINGTIME)
	{
		FTick = FCurrTick;
		CloseProc();
		CloseWindows();
	}
}

void TProhiThread::GetExeNameList(vector<PProcessInfo>& ProcessName)
{
	PProcessInfo TempP = NULL;
	bool ok = false;
	HANDLE ProcessListHandle = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 ProcessStruct;
	int i = 0;
	HANDLE Handle = INVALID_HANDLE_VALUE;
	char Path[MAX_PATH] = {0};
	DWORD n = 0;
	HMODULE hMod = NULL;

	if (ProcessName.size() > 0)
	{
		for (int k = 0; k < ProcessName.size(); ++k)
		{
			delete ProcessName[k];
			ProcessName[k] = NULL;
		}
		ProcessName.clear();
	}
	ProcessListHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	ProcessStruct.dwSize = sizeof(ProcessStruct);
	if (Process32First(ProcessListHandle, &ProcessStruct)) 
	{ 
		do 
		{
			TempP = new TProcessInfo;
			TempP->ProcessName =  ProcessStruct.szExeFile;
			Handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE, FALSE, ProcessStruct.th32ProcessID);
			if (Handle != INVALID_HANDLE_VALUE)
			{
				if (EnumProcessModules(Handle, &hMod, sizeof(hMod), &n))
				{
					ZeroMemory(Path, MAX_PATH);
					GetModuleFileNameEx(Handle, hMod, Path, MAX_PATH);
					TempP->ProcessFillName =  Path;
					TempP->ProcessHandle = Handle;
					//加入链表
					ProcessName.push_back(TempP);
				}
				else
				{
					delete TempP;
				}
			}
			else 
			{
				delete TempP;
			}
		} 
		while (Process32Next(ProcessListHandle, &ProcessStruct)); 
	} 
	CloseHandle(ProcessListHandle);
}

void TProhiThread::CloseWindows()
{
	HWND    Wnd;
	int     i = 0, j = 0;
	PWindowsInfo  FWindowsInfo;
	int     iPos;
	bool    IsClose;
	PProhiWindows FProhiWindows ;
	DWORD   ProcID;
	HANDLE  ProcHandle;
	DWORD   ExitCode;

	m_WndList.clear();
	EnumWindows(EnumWindowsFunc, (LPARAM)(this));

	for (i = 0; i < FProhiWindowsList.size(); ++i)
	{		
	    FProhiWindows = FProhiWindowsList[i];
		char WinName[MAX_PATH] = {0};
		strcpy(WinName, FProhiWindows->WindowsName);
		for (j = 0; j < m_WndList.size(); ++j)
		{
			IsClose = false;
			FWindowsInfo = m_WndList[j];
			char SubWinName[MAX_PATH] = {0};
			char CompareStr[MAX_PATH] = {0};
			strcpy(SubWinName, FWindowsInfo->WindowsName);
			if (WinName[0] == '*' && WinName[strlen(WinName) - 1] != '*')
			{
				CopyMemory(CompareStr, WinName + 1, strlen(WinName) - 1);
				if (strstr(strupr(SubWinName), strupr(CompareStr)))
				{
					IsClose = true;
				}
			}
			else if (WinName[0] != '*' && WinName[strlen(WinName) - 1] == '*')
			{
				CopyMemory(CompareStr, WinName, strlen(WinName) - 1);
				if (strstr(strupr(SubWinName), strupr(CompareStr)))
				{
					IsClose = true;
				}
			}
			else if (WinName[0] == '*' && WinName[strlen(WinName) - 1] == '*')
			{
				CopyMemory(CompareStr, WinName + 1, strlen(WinName) - 2);
				if (strstr(strupr(SubWinName), strupr(CompareStr)))
				{
					IsClose = true;
				}
			}
			else if (stricmp(WinName, SubWinName) == 0)
			{
				IsClose = true;
			}

			if (IsClose)
			{
				ExitCode = 0;
				Wnd = FindWindow(NULL, SubWinName);
				if (Wnd)
				{
					ProcID = 0;
					GetWindowThreadProcessId(Wnd, &ProcID);
					ProcHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE, FALSE, ProcID);
					if (ProcHandle != INVALID_HANDLE_VALUE)
					{
						GetExitCodeProcess(ProcHandle, &ExitCode);
						TerminateProcess(ProcHandle, ExitCode);
						CloseHandle(ProcHandle);
					}
				}
			}			
     	}
	}

	for (int k = 0; k < m_WndList.size(); ++k)
	{
		delete m_WndList[k];
	}
	m_WndList.clear();
}

void TProhiThread::CloseProc()
{
	int i, j;
	PProhiProc   FProhiProc ;
	PProcessInfo FProcessInfo ;
	vector<PProcessInfo> FProcList;
	char FProcPath[MAX_PATH] = {0}, FProcName[MAX_PATH] = {0}, FProcessName[MAX_PATH] = {0};
	DWORD ExitCode;

	//进程快照
	GetExeNameList(FProcList);
	for (i = 0; i < FProhiProcList.size(); ++i)
	{
		//删除的进程
		FProhiProc = FProhiProcList[i];
		char FileName[MAX_PATH] = {0};
		char FileExt[MAX_PATH] = {0};
		_splitpath_s(FProhiProc->ProcessName, NULL, 0, NULL, 0, FileName, MAX_PATH, FileExt, MAX_PATH);
		sprintf(FProcName, "%s%s", FileName, FileExt);

		for (j = 0; j < FProcList.size(); ++j)
		{
			ZeroMemory(FileName, MAX_PATH);
			ZeroMemory(FileExt, MAX_PATH);
			ExitCode = 0;
			//快照进程
			FProcessInfo = FProcList[j];
			_splitpath_s(FProcessInfo->ProcessName.c_str(), NULL, 0, NULL, 0, FileName, MAX_PATH, FileExt, MAX_PATH);
			sprintf(FProcessName, "%s%s", FileName, FileExt);
			if (stricmp(FProcName, FProcessName) == 0)
			{
				GetExitCodeProcess(FProcessInfo->ProcessHandle, &ExitCode);
				if (TerminateProcess(FProcessInfo->ProcessHandle, ExitCode) == FALSE)
				{
					//终止进程失败
				}
			}
		}
	}
	for (i = 0; i < FProcList.size(); ++i)
	{
		delete FProcList[i];
	}
}

void TProhiThread::Execute()
{
	PMsgBuffer FWork = NULL, FNext = NULL;
	while (true)
	{
		EnterCriticalSection(&FCS);
		if (FWork == NULL)
		{
			FWork = FFirst;
			FFirst = NULL;
			FLast = NULL;
		}
		LeaveCriticalSection(&FCS);
		while (FWork)
		{
			FNext = FWork->Next;
			switch(FWork->OperType)
			{
			case uAdd:
				{
					AddInfo(FWork->DataType, FWork->Buffer, FWork->BufferLen);
					break;
				}
			case uDel:
				{
					DelInfo(FWork->DataType, FWork->Buffer, FWork->BufferLen);
					break;
				}
			case uClear:
				{
					Clear(FWork->DataType);
					break;
				}
			}
			if (FWork->Buffer)
			{
				delete FWork->Buffer;
				FWork->Buffer = NULL;
			}
			delete FWork;
			FWork = FNext;
		}
		if (WaitForSingleObject(FEvent, 50) == WAIT_OBJECT_0)
		{
			break;
		}
		PollingTask();
	}
}

void TProhiThread::Stop()
{
	//停止延时等待
	if (m_hThread != NULL)
	{
		SetEvent(FEvent);
		WaitForSingleObject(m_hThread, INFINITE);
		m_hThread = INVALID_HANDLE_VALUE;
	}
}

UINT __stdcall TProhiThread::ExecuteWorkThread(LPVOID lpVoid)
{
	TProhiThread* pThis = (TProhiThread*)lpVoid;
	pThis->Execute();
	return 1;
}

void TProhiThread::Start()
{
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, ExecuteWorkThread, this, 0, NULL);
}
CSafeCenter::CSafeCenter()
{
    m_bIsStart = FALSE;
	m_bIsStartARP = FALSE;
	m_bIsAutoGetGateAddr = FALSE;
}

CSafeCenter::~CSafeCenter()
{
    Stop();
}

//读取配置文件
BOOL CSafeCenter::ReadConfig(LPCTSTR lpFilePath)
{
	//参数校验
	if(!lpFilePath || !PathFileExists(lpFilePath)) return FALSE;
	BOOL    bIsSuccess = FALSE;
	CMarkup xml;
	bIsSuccess = xml.Load(lpFilePath);
	if(bIsSuccess == FALSE) return FALSE;
	//进入根结点
	bIsSuccess = xml.FindElem("Config");
	if(bIsSuccess == FALSE) return FALSE;
	xml.IntoElem();

	//查询ARP配置
	if (xml.FindElem("ArpProtected"))
	{	
		xml.IntoElem();
		xml.FindElem("Status");
		string strIsOpen = xml.GetAttrib("IsOpen");		
		m_bIsStartARP = (atoi(strIsOpen.c_str()) == 1 ? TRUE : FALSE);
		string strAutoGetGateAddr = xml.GetAttrib("IsAutoGetGatewayAddr");
		m_bIsAutoGetGateAddr = (atoi(strAutoGetGateAddr.c_str()) == 1 ? TRUE : FALSE);

		if (m_bIsAutoGetGateAddr)
		{
			xml.OutOfElem();
		}
		else
		{
			m_GateAddr.Ip = inet_addr(xml.GetAttrib("GatewayIp").c_str());
			string strMac = xml.GetAttrib("GatewayMac");
			ParserMacToByte(strMac, m_GateAddr.Mac);
			xml.OutOfElem();
		}
	}
	//查询进程列表
	if(xml.FindElem("ProccessList"))
	{
		if(xml.IntoElem())
		{
			while(xml.FindElem("ProccessInfo"))
			{
				string strProcName = xml.GetAttrib("Name");
				//加入进程查杀列表
				m_KillWndProc.AddData(uProc, strProcName, string(""));
			}
			xml.OutOfElem();
		}
	}
	//查询窗口列表
	if(xml.FindElem("WindowList"))
	{
		if(xml.IntoElem())
		{
			while(xml.FindElem("WindowInfo"))
			{
				string strWndName = xml.GetAttrib("Name");
				//加入进程查杀列表
				m_KillWndProc.AddData(uWindows, strWndName, string(""));
			}
			xml.OutOfElem();
		}
	}
	//退出根结点
	xml.OutOfElem();
	return TRUE;
}

//启动安全中心
BOOL CSafeCenter::Start()
{
	BOOL bIsSuccess = FALSE;
	//启动ARP
	if (m_bIsStartARP)
	{
		bIsSuccess = StartArpProtect();
		if (bIsSuccess == FALSE)
		{
			CLogFile::WriteLog("ARP防火墙启动失败.");
		}
	}
	//启动窗口进程查杀
	CLogFile::WriteLog("启动窗口进程查杀模块.");
	StartWndProcDetect();
    m_bIsStart = TRUE;
	return TRUE;
}

//停止安全中心
BOOL CSafeCenter::Stop()
{
	if(m_bIsStart == FALSE) return TRUE;
	BOOL bIsSuccess = FALSE;
	//停止ARP
	if (m_bIsStartARP)
	{
		bIsSuccess = StopArpProtect();
	}
	//停止窗口进程查杀
	StopWndProcDetect();
	CLogFile::WriteLog("安全中心退出\r\n");
	return bIsSuccess;
}

//启动ARP防御
BOOL CSafeCenter::StartArpProtect()
{
	IP_MAC    LocalAddr[10];
	int       iAdpterCount = 0;
	if (ArpModIsOK() == FALSE)
	{
		char szError[MAX_PATH] = {0};
		sprintf(szError, "ARP模块没有正常工作，可能是由于模块没有安装或其它原因引起的.");
		CLogFile::WriteLog(szError);
		return FALSE;
	}

	//获取本地IP，MAC
	if(GetLocalIpMac(LocalAddr, iAdpterCount) == FALSE) 
	{
		char szError[MAX_PATH] = {0};
		sprintf(szError, "获取本机地址失败");
		CLogFile::WriteLog(szError);
		return FALSE;
	}
	if (m_bIsAutoGetGateAddr)
	{
		GetGatewayAddr();
	}
	if(InitArpMod(Arp_Attck_Callback, LocalAddr, iAdpterCount, &m_GateAddr) == FALSE)
	{
		return FALSE;
	}
	return StartProtect();
}

BOOL CSafeCenter::GetLocalIpMac(IP_MAC LocalAddr[], int &iCount)
{
	IP_ADAPTER_INFO  Adapter[10] = {0};
	PIP_ADAPTER_INFO pAdapter = Adapter;
	iCount = 0;

	DWORD	dwSize = sizeof(Adapter);
	if (ERROR_SUCCESS == GetAdaptersInfo(Adapter, &dwSize))
	{
		while (pAdapter != NULL)
		{
			_IP_ADDR_STRING* pIpAddress = &pAdapter->IpAddressList;
			while (pIpAddress != NULL)
			{
				LocalAddr[iCount].Ip = inet_addr(pIpAddress->IpAddress.String);
				char szMacAddr[MAX_PATH] = {0};
				sprintf(szMacAddr, "%02X-%02X-%02X-%02X-%02X-%02X",
					pAdapter->Address[0],
					pAdapter->Address[1],
					pAdapter->Address[2],
					pAdapter->Address[3],
					pAdapter->Address[4],
					pAdapter->Address[5]);
				ParserMacToByte(szMacAddr, LocalAddr[iCount].Mac);
				pIpAddress = pIpAddress->Next;
				iCount++;
			}
			pAdapter = pAdapter->Next;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CSafeCenter::ParserMacToByte(string strMac, BYTE cbMac[])
{
	char szTemp[MAX_PATH]={0};
	char szByte[3] = {0};
	strcpy(szTemp, strMac.c_str());

	for (int i = 0; i < 6; i++)
	{
		CopyMemory(szByte, szTemp + i*3, 2);
		cbMac[i] = (BYTE)strtol(szByte, NULL, 16);
	}
	return TRUE;
}

BOOL CSafeCenter::StopArpProtect()
{
	if(ArpModIsOK() == FALSE) return FALSE;
	BOOL bOk = StopProtect();
	return bOk;
}

BOOL CSafeCenter::StartWndProcDetect()
{
	m_KillWndProc.Start();
	return TRUE;
}

BOOL CSafeCenter::StopWndProcDetect()
{
	m_KillWndProc.Stop();
	return TRUE;
}

BOOL CSafeCenter::GetGatewayAddr()
{
	IP_ADAPTER_INFO  Adapter[10] = {0};
	PIP_ADAPTER_INFO pAdapter = Adapter;

	DWORD	dwSize = sizeof(Adapter);
	if (ERROR_SUCCESS == GetAdaptersInfo(Adapter, &dwSize))
	{
        m_GateAddr.Ip = inet_addr((char*)(Adapter->GatewayList.IpAddress.String));
		GetGatewayMac(m_GateAddr.Ip, (char*)(m_GateAddr.Mac));
		return TRUE;
	}
	return FALSE;
}
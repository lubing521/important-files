#ifndef _SAFECENTER_H
#define _SAFECENTER_H
#pragma pack(1)
//攻击类型
enum ATTACH_TYPE
{ 
	ATTACH_NONE = 0,			                      // 无攻击行为
	GATEWAY_ARP_QUERY_ATTACH,                         // 伪造网关查询包攻击
	GATEWAY_ARP_REPLY_ATTACH,                         // 伪造网关回应包攻击
	LAN_SAMEIP_ATTACH,		                          // 局域网IP冲突攻击
	WAN_SAMEIP_ATTACH,		                          // 广域网IP冲突攻击
	WRONG_PROTOCOL_ATTACH,	                          // 错误的协议攻击
	UNKNOWN_ATTACH
};

//IP MAC结构
typedef struct  _IP_MAC
{
	DWORD                   Ip;                       //IP地址
	BYTE                    Mac[6];                   //MAC地址，不包含"-"
}IP_MAC, *PIP_MAC;

//攻击信息
typedef struct _ATTACK_INFO
{
    ATTACH_TYPE             AType;                    //攻击类型
	BYTE                    AttackByMac[6];           //攻击源MAC
	BYTE                    AttackByIp[6];            //攻击源IP
}ATTACK_INFO, *PATTACK_INFO;
#pragma pack ()
//初始化ARP的回调函数类型
typedef void (__stdcall *ARP_ATTACK_CALLBACK)(PATTACK_INFO AttackInfo, BOOL IsKnownIP);

//ARP防御导出接口类型定义
typedef BOOL (__stdcall *PFNINITARPMOD)(ARP_ATTACK_CALLBACK pfnAttackCallback, PIP_MAC LocalIpMac, DWORD Count, PIP_MAC GatewayIpMac);
typedef BOOL (__stdcall *PFNARPMODISOK)();
typedef BOOL (__stdcall *PFNSTARTPROTECT)();
typedef BOOL (__stdcall *PFNDISABLESIGN)();
typedef BOOL (__stdcall *PFNENABLESIGN)();
typedef BOOL (__stdcall *PFNINSTALLDRIVER)();
typedef BOOL (__stdcall *PFNREMOVEDRIVER)();
typedef BOOL (__stdcall *PFNGETGATEWAYMAC)(DWORD GatewayAddr, TCHAR MacAddr[]);
typedef BOOL (__stdcall *PFNSTOPPROTECT)();
HMODULE g_hI8NEModule = NULL;
//ARP防御接口调用
BOOL __stdcall InitArpMod(ARP_ATTACK_CALLBACK pfnAttackCallback, PIP_MAC LocalIpMac, DWORD Count, PIP_MAC GatewayIpMac)
{
	if (g_hI8NEModule == NULL)
	{
		TCHAR path[MAX_PATH] = {0};
    	GetModuleFileName(NULL, path, MAX_PATH);
    	PathRemoveFileSpec(path);
    	PathAddBackslash(path);
    	lstrcat(path, TEXT("SafeClt\\i8NE.dll"));
		g_hI8NEModule = LoadLibrary(path);
	}
	if (g_hI8NEModule != NULL)
	{
		PFNINITARPMOD pfnInitArpMod = (PFNINITARPMOD)GetProcAddress(g_hI8NEModule, (char*)1);
		if (pfnInitArpMod != NULL)
		{
			return pfnInitArpMod(pfnAttackCallback, LocalIpMac, Count, GatewayIpMac);
		}
	}
	return FALSE;
}

BOOL __stdcall ArpModIsOK()
{
	if (g_hI8NEModule != NULL)
	{
		PFNARPMODISOK pfnArpModIsOK = (PFNARPMODISOK)GetProcAddress(g_hI8NEModule, (char*)9);
		if (pfnArpModIsOK != NULL)
		{
			return pfnArpModIsOK();
		}
	}
	return FALSE;
}

BOOL __stdcall StartProtect()
{
	if (g_hI8NEModule != NULL)
	{
		PFNSTARTPROTECT pfnStartProtect = (PFNSTARTPROTECT)GetProcAddress(g_hI8NEModule, (char*)2);
		if (pfnStartProtect != NULL)
		{
			return pfnStartProtect();
		}
	}
	return FALSE;
}

void __stdcall DisableSign()
{
	if (g_hI8NEModule != NULL)
	{
		PFNDISABLESIGN pfnDisableSign = (PFNDISABLESIGN)GetProcAddress(g_hI8NEModule, (char*)5);
		if (pfnDisableSign != NULL)
		{
			pfnDisableSign();
		}
	}
}

void __stdcall EnableSign()
{
	if (g_hI8NEModule != NULL)
	{
		PFNENABLESIGN pfnEnableSign = (PFNENABLESIGN)GetProcAddress(g_hI8NEModule, (char*)6);
		if (pfnEnableSign != NULL)
		{
			pfnEnableSign();
		}
	}
}

BOOL __stdcall InstallDriver()
{
	if (g_hI8NEModule != NULL)
	{
		PFNINSTALLDRIVER pfnInstallDriver = (PFNINSTALLDRIVER)GetProcAddress(g_hI8NEModule, (char*)7);
		if (pfnInstallDriver != NULL)
		{
			return pfnInstallDriver();
		}
	}
	return FALSE;
}

BOOL __stdcall RemoveDriver()
{
	if (g_hI8NEModule != NULL)
	{
		PFNREMOVEDRIVER pfnRemoveDriver = (PFNREMOVEDRIVER)GetProcAddress(g_hI8NEModule, (char*)8);
		if (pfnRemoveDriver != NULL)
		{
			return pfnRemoveDriver();
		}
	}
	return FALSE;
}

BOOL __stdcall GetGatewayMac(DWORD GatewayAddr, TCHAR MacAddr[])
{
	if (g_hI8NEModule != NULL)
	{
		PFNGETGATEWAYMAC pfnGetGatewayMac = (PFNGETGATEWAYMAC)GetProcAddress(g_hI8NEModule, (char*)10);
		if (pfnGetGatewayMac != NULL)
		{
			return pfnGetGatewayMac(GatewayAddr, MacAddr);
		}
	}
	return FALSE;
}

BOOL __stdcall StopProtect()
{
	if (g_hI8NEModule != NULL)
	{
		PFNSTOPPROTECT pfnStopProtect = (PFNSTOPPROTECT)GetProcAddress(g_hI8NEModule, (char*)11);
		if (pfnStopProtect != NULL)
		{
			return pfnStopProtect();
		}
	}
	return FALSE;
}

void __stdcall UnInitArpMod()
{
	if (g_hI8NEModule != NULL)
	{
		FreeLibrary(g_hI8NEModule);
		g_hI8NEModule = NULL;
	}
}

#endif
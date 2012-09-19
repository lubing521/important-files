#include "StdAfx.h"
#include "task.h"
#include "DataCenter.h"
#include "PushFile.h"
#include "AutoUpdate.h"
#include "ComponentMgrTask.h"
#include "I8ClientPipe.h"
#include "I8ClientOfflineFileLock.h"
#include "Win32/FileSystem/IniConfig.hpp"
#include "Win32/FileSystem/FileHelper.hpp"
#include "Win32/System/I8Temperature.h"
#include "I8ClientSvrPipe.h"
#include "BootTask.h"
#include "VirtualDiskDll.h"

I8_NAMESPACE_BEGIN

CMenuShareData::CMenuShareData(): m_lpData(NULL), m_hFileMap(NULL), m_pShareData(NULL)
{	
	//生成一个对所有用户可以访问的安全属性.
	m_SecAttr.nLength = sizeof(m_SecAttr);
	m_SecAttr.lpSecurityDescriptor = &m_SecDesc;
	m_SecAttr.bInheritHandle = FALSE;
	m_lpData = BuildRestrictedSD(&m_SecDesc);

	//创建和菜单共享的内存映射.
	if (m_lpData != NULL)
	{
		m_hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, &m_SecAttr, PAGE_READWRITE, 0, MAX_PATH, TEXT("I8MENU_SHARE"));
		if (m_hFileMap != NULL )
		{
			m_pShareData = (DWORD*)MapViewOfFile(m_hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, MAX_PATH );
			if (m_hFileMap != NULL)
				ZeroMemory(m_pShareData, MAX_PATH );
		}
	}

}

CMenuShareData::~CMenuShareData()
{
	if (m_pShareData != NULL)
	{
		UnmapViewOfFile(m_pShareData);
		m_pShareData = NULL;
	}
	if (m_hFileMap != NULL)
	{
		CloseHandle(m_hFileMap);
		m_hFileMap = NULL;
	}

	if (m_lpData)
	{
		HeapFree(GetProcessHeap(), 0, m_lpData);
		m_lpData = NULL;
	}
}

PVOID CMenuShareData::BuildRestrictedSD(PSECURITY_DESCRIPTOR pSD)
{
	DWORD				dwAclLength;
	PSID				psidEveryone = NULL;
	PACL				pDACL   = NULL;
	BOOL				bResult = FALSE;
	PACCESS_ALLOWED_ACE pACE = NULL;
	SID_IDENTIFIER_AUTHORITY siaWorld = SECURITY_WORLD_SID_AUTHORITY  ;
	SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION;

	__try 
	{
		// initialize the security descriptor
		if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION)) 
			__leave;

		// obtain a sid for the Authenticated Users Group
		if (!AllocateAndInitializeSid(&siaWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &psidEveryone)) 
			__leave;

		// The Authenticated Users group includes all user accounts that
		// have been successfully authenticated by the system. If access
		// must be restricted to a specific user or group other than 
		// Authenticated Users, the SID can be constructed using the
		// LookupAccountSid() API based on a user or group name.

		// calculate the DACL length
		dwAclLength = sizeof(ACL)
			// add space for Authenticated Users group ACE
			+ sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD)
			+ GetLengthSid(psidEveryone);

		// allocate memory for the DACL
		if ( (pDACL = (PACL) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwAclLength)) == NULL)
			__leave;

		// initialize the DACL
		if (!InitializeAcl(pDACL, dwAclLength, ACL_REVISION)) 
			__leave;

		// add the Authenticated Users group ACE to the DACL with
		// GENERIC_READ, GENERIC_WRITE, and GENERIC_EXECUTE access
		if (!AddAccessAllowedAce(pDACL, ACL_REVISION, GENERIC_ALL, psidEveryone)) 
			__leave;

		// set the DACL in the security descriptor
		if (!SetSecurityDescriptorDacl(pSD, TRUE, pDACL, FALSE))
			__leave;
		bResult = TRUE;

	} 
	__finally 
	{
		if (psidEveryone) 
			FreeSid(psidEveryone);
	}

	if (bResult == FALSE) 
	{
		if (pDACL) HeapFree(GetProcessHeap(), 0, pDACL);
		pDACL = NULL;
	}

	return (PVOID) pDACL;
}

void CMenuShareData::WriteShareData(DWORD dwOnline, DWORD dwProt)
{
	if (m_pShareData != NULL)
	{
		m_pShareData[0] = dwOnline;	
		m_pShareData[1] = dwProt;
	}
}


CDataCenter::CDataCenter(ICliPlugMgr* pPlugMgr, ILogger* pLog, HANDLE hEvent)
{
	m_pSystemInfo.reset(new CSystemInfo(&m_objHyOperator));
	m_pIoOperator.reset(new CIoOperator(pPlugMgr->GetCommunication(), pLog, &m_objHyOperator));
    m_clPushGame.reset(new CPushGame(&m_objHyOperator, pLog, hEvent, OnUpdateConnect, this));
	m_pLog = pLog;
	m_hExitEvent = hEvent;
	m_uiTimer = 0;
	m_pCliPlugMgr = pPlugMgr;
	m_bIsFirstHeart = TRUE;
    m_bDownVDiskInfo = FALSE;
    if (m_clPipe.CreatePipe(I8_CliSvrPipe_ns::C_PIPE_NAME))
    {
        m_pLog->WriteLog(LM_INFO, TEXT("创建命名管道成功。"));
    }
    else
    {
        m_pLog->WriteLog(LM_INFO, TEXT("创建命名管道失败。"));
    }
    m_clPipe.ListenWithAutoNotify(std::tr1::bind(&CDataCenter::ReceivePipeMessage, this, std::tr1::placeholders::_1));
    m_dwRunBarOnline = I8_ClientDataCore_ns::IsBarOnlineExist() ? GetTickCount() : 0;
    m_clExtendInfoMgr.Init(m_pIoOperator.get(), m_pLog);
    m_bRefreshVirtualDisk = FALSE;
    m_bRefreshVirtualDiskOffonline = FALSE;
}

CDataCenter::~CDataCenter()
{
    ReportClientExit();
    m_clPipe.Close();
	m_clIconFile.Clear();
	m_mapGameInfo.clear();
	m_mapGameIndexInfo.clear();	

	m_vctServerInfo.clear();
    m_mapVDiskInfo.clear();
    m_clPushGame.reset();
}

BOOL CDataCenter::GetData(CComponentMgrTask* ComponentMgr)
{
	m_pComponentMgr = ComponentMgr;
	m_objPipe.WriteShareData(m_pIoOperator->IsConnected()?1:0, m_pComponentMgr->IsStartProtDisk()?1:0);
	if (m_uiTimer != 0)
	{
		m_uiTimer = (m_uiTimer + 1)%61;		
		return TRUE;
	}
    GetServerAddr();
	if (!m_pIoOperator->IsConnected())
	{
		if (m_clIconFile.IsNull())
			ReadGameIconFile();
		if (m_mapGameInfo.empty())
			ReadGameInfoFile();
		if (m_mapSysOption.empty())
			ReadSysOptFile();
		if (m_vctServerInfo.empty())
			ReadSvrCfgFile();
		if (m_mapVDiskInfo.empty())
            ReadVDiskFile();
        if (!m_bRefreshVirtualDiskOffonline && !m_bRefreshVirtualDisk) //刷新虚拟盘
        {
            RefreshVirtualDisk();
            m_bRefreshVirtualDiskOffonline = TRUE;
        }
		return TRUE;
    }

    if (m_mapSysOption.empty())
    {
        ReadSysOptFile();
    }

	if (WaitForSingleObject(m_hExitEvent, 0) == WAIT_OBJECT_0)
		return FALSE;

	STSysCfgInfo      stSysCfgInfo  = m_pSystemInfo->GetSysCfgInfo(m_pCliPlugMgr->GetCommunication()->GetTcpSocket());
	STCltHardwareInfo stCltHardInfo = m_pSystemInfo->GetHardwareInfo();	
	STI8Version       stI8Version   = m_pSystemInfo->GetI8Version();
	STHyInfo          stHyInfo      = m_pSystemInfo->GetHyInfo();
	CCommandParam     cmdParamIn, cmdParamOut;
	DWORD             dwGameDrvFreeSize = 0;
    DWORD             dwCPUTemperature = CI8Temperature::Instance().GetTemperature();
    if (dwCPUTemperature == 0)
    {
        m_pLog->WriteLog(LM_INFO, CI8Temperature::Instance().GetErrInfo());
    }
	
	stdex::tString strGameDriver = GetStringOpt(OPT_M_GAMEDRV);
	if(!strGameDriver.empty()) 
		dwGameDrvFreeSize = GetDriverFreeSize(strGameDriver[0]);
	DWORD dwSafeCenterStatus = 0;


	cmdParamIn.InitPackageHeader(CMD_CLIRPT_CLIENTINFO);
	cmdParamIn<<stSysCfgInfo.szName<<stSysCfgInfo.dwGate<<stSysCfgInfo.dwDns0<<stSysCfgInfo.szMacAddr<<stHyInfo.dwInstall \
		<<stHyInfo.szProtDriver<<stHyInfo.szAllDriver<<stI8Version.szProtVer<<stI8Version.szCltUIVer<<stI8Version.szCltDataVer \
        <<stI8Version.szCltSvrVer<<stI8Version.szVirtualDiskVer<<stSysCfgInfo.dwDns1<<stSysCfgInfo.dwSubMask<<int(dwCPUTemperature) \
		<<dwGameDrvFreeSize<<dwSafeCenterStatus<<((DWORD)m_pComponentMgr->IsBindBho()?1:0)<<((DWORD)m_pComponentMgr->IsStartProtDisk()?1:0);

	if(!m_pIoOperator->ExecCommand(&cmdParamIn, TRUE, TRUE, &cmdParamOut)) 
	{
		m_pLog->WriteLog(LM_INFO, TEXT("发送心跳信息失败。"));
		return FALSE;
	}
    else
    {
        m_pLog->WriteLog(LM_INFO, TEXT("发送心跳信息成功。"));
    }
	stdex::tString strHeartFile = GetRootDir() + TEXT("data\\HeartDataInfo.dat");
	m_pIoOperator->WriteFile(strHeartFile.c_str(), (const char*)cmdParamOut.GetBuffer(), cmdParamOut.GetBufferLength());
	m_uiTimer = (m_uiTimer + 1)%61;	
	ProcessHeartResponse(cmdParamOut);

	if (m_bIsFirstHeart)
	{
		m_bIsFirstHeart = FALSE;
    }
	return TRUE;
}

BOOL CDataCenter::ProcessHeartResponse( CCommandParam& cmdParam )
{
	if (WaitForSingleObject(m_hExitEvent, 0) == WAIT_OBJECT_0)
		return FALSE;

    BOOL bNeedUpdateUI = FALSE;

	DWORD dwResult = 0;
	cmdParam.Pop(dwResult);
	if(dwResult != CMD_RET_SUCCESS)
		return FALSE;

	stdex::tString sClientVersionFile = GetRootDir() + TEXT("GameClient_Version.ini");
	DWORD dwGameVer = 0, dwExInfoVer = 0, dwIconVer = 0, dwVDiskVer = 0, dwSysOptVer = 0, dwClientVer = 0, dwBootTaskVer = 0, dwPlugInVer = 0, dwVDiskCount = 0;
	BOOL bWriteIniFlag = FALSE;
	cmdParam>>dwGameVer>>dwVDiskVer>>dwIconVer>>dwClientVer>>dwSysOptVer>>dwBootTaskVer>>dwPlugInVer>>dwExInfoVer>>dwVDiskCount;

	SetIniConfig(TEXT("VDiskBufVer"),NULL,NULL,sClientVersionFile.c_str());
	for (DWORD i = 0; i < dwVDiskCount; ++i)
	{
        TCHAR atVDiskName[MAX_PATH];
		cmdParam.PopString(atVDiskName, MAX_PATH);
        DWORD dwVDiskBufVer;
		cmdParam>>dwVDiskBufVer;
		SetIniConfig<DWORD>(TEXT("VDiskBufVer"), atVDiskName, dwVDiskBufVer, sClientVersionFile.c_str());	
	}

    if (m_bIsFirstHeart && GetIntOpt(OPT_M_AUTOUPT) != 0) //允许软件自动更新
    {
        if (WaitForSingleObject(m_hExitEvent, 0) == WAIT_OBJECT_0)
            return FALSE;
        DWORD dwOldClientVer = GetIniConfig<DWORD>(TEXT("SystemSet"), TEXT("UptVer"), 0, sClientVersionFile.c_str());
        if (dwClientVer != dwOldClientVer)
        {
            if (dwOldClientVer != 404)
            {
                m_pLog->WriteLog(LM_INFO, TEXT("客户端菜单版本：%u， 服务端菜单版本：%u 需要更新。"), dwOldClientVer, dwClientVer);
                CAutoUpdate clAutoUpdate(m_pLog, m_pIoOperator.get());
                if (clAutoUpdate.Update())
                {
                    SetIniConfig<DWORD>(TEXT("SystemSet"), TEXT("UptVer"), dwClientVer, sClientVersionFile.c_str());
                    bWriteIniFlag = TRUE;
                    m_pLog->WriteLog(LM_INFO, TEXT("菜单下载成功。"));
                }
                else m_pLog->WriteLog(LM_INFO, TEXT("菜单下载失败。"));
            }
            else
            {
                m_pLog->WriteLog(LM_INFO, TEXT("菜单自动升级功能关闭。"));
            }
        }
    }

    if (m_vctServerInfo.empty())
    {
        DownloadSvrCfg();
    }
    if (!m_bDownVDiskInfo)
    {
        DownloadVDiskInfo();
    }

    //刷新虚拟盘
    if (!m_bRefreshVirtualDisk)
    {
        if (RefreshVirtualDisk())
        {
            m_bRefreshVirtualDisk = TRUE;
        }
    }

	//系统选项
	if (WaitForSingleObject(m_hExitEvent, 0) == WAIT_OBJECT_0)
        return FALSE;
	DWORD dwOldSysOptVer = GetIniConfig<DWORD>(TEXT("SystemSet"), TEXT("SysOptVer"), 0, sClientVersionFile.c_str());
	if (dwSysOptVer != dwOldSysOptVer)
	{
        //获取系统选项
        m_pLog->WriteLog(LM_INFO, TEXT("客户端系统选项版本：%u， 服务端系统选项版本：%u 需要更新。"), dwOldSysOptVer, dwSysOptVer);
		BOOL bIsSuccess = DownloadSysOption();
		if(bIsSuccess) 
		{
            bNeedUpdateUI = TRUE;
			SetIniConfig<DWORD>(TEXT("SystemSet"), TEXT("SysOptVer"), dwSysOptVer, sClientVersionFile.c_str());
            bWriteIniFlag = TRUE;
            m_pLog->WriteLog(LM_INFO, TEXT("系统选项下载成功。"));
        }
        else m_pLog->WriteLog(LM_INFO, TEXT("系统选项下载失败。"));
	}    

	if (m_bIsFirstHeart || GetIntOpt(OPT_M_ONLYONE) != 1)
	{	
		if (WaitForSingleObject(m_hExitEvent, 0) == WAIT_OBJECT_0)
            return FALSE;
        if (m_bIsFirstHeart && m_clIconFile.IsNull())
            ReadGameIconFile();
		DWORD dwOldIconVer = GetIniConfig<DWORD>(TEXT("SystemSet"), TEXT("IcoVer"), 0, sClientVersionFile.c_str());
		if (dwIconVer != dwOldIconVer)
		{
			m_pLog->WriteLog(LM_INFO, TEXT("客户端图标版本：%u， 服务端图标版本：%u 需要更新。"), dwOldIconVer, dwIconVer);
			BOOL bIsSuccess = DownloadGameIcon();
			if(bIsSuccess) 
			{
                bNeedUpdateUI = TRUE;
				SetIniConfig<DWORD>(TEXT("SystemSet"), TEXT("IcoVer"), dwIconVer, sClientVersionFile.c_str());
				bWriteIniFlag = TRUE;
				m_pLog->WriteLog(LM_INFO, TEXT("图标下载成功。"));
			}
			else m_pLog->WriteLog(LM_INFO, TEXT("图标下载失败。"));
		}

		if (WaitForSingleObject(m_hExitEvent, 0) == WAIT_OBJECT_0)
            return FALSE;
        if (m_bIsFirstHeart && m_mapGameInfo.empty())
            ReadGameInfoFile();
		DWORD dwOldGameVer = GetIniConfig<DWORD>(TEXT("SystemSet"), TEXT("GameVer"), 0, sClientVersionFile.c_str());
		if (dwGameVer != dwOldGameVer)
		{
			m_pLog->WriteLog(LM_INFO, TEXT("客户端游戏版本：%u， 服务端游戏版本：%u 需要更新。"), dwOldGameVer, dwGameVer);
			BOOL bIsSuccess = DownloadGameInfo();
			if(bIsSuccess)
			{
                bNeedUpdateUI = TRUE;
				SetIniConfig<DWORD>(TEXT("SystemSet"), TEXT("GameVer"), dwGameVer, sClientVersionFile.c_str());
				bWriteIniFlag = TRUE;
				m_pLog->WriteLog(LM_INFO, TEXT("游戏信息下载成功。"));
			}
			else m_pLog->WriteLog(LM_INFO, TEXT("游戏信息下载失败。"));
        }
        if (m_bIsFirstHeart)
        {
            CreateGameShortcut(); //创建游戏快捷方式
        }


        if (WaitForSingleObject(m_hExitEvent, 0) == WAIT_OBJECT_0)
            return FALSE;
        DWORD dwOldBootTaskVer = GetIniConfig<DWORD>(TEXT("SystemSet"), TEXT("BootTaskVer"), 0, sClientVersionFile.c_str());
        if (dwBootTaskVer != dwOldBootTaskVer || dwGameVer != dwOldGameVer || m_clPushGame->IsNeedContinue())
        {
            if (dwBootTaskVer != dwOldBootTaskVer)
            {
                m_pLog->WriteLog(LM_INFO, TEXT("客户端开机任务版本：%u， 服务端开机任务版本：%u 需要推送游戏。"), dwOldBootTaskVer, dwBootTaskVer);
            }
            else
            {
                if (dwGameVer != dwOldGameVer)
                {
                    m_pLog->WriteLog(LM_INFO, TEXT("客户端游戏版本和服务端游戏版本不一致，需要推送游戏。"));
                }
                else
                {
                    m_pLog->WriteLog(LM_INFO, TEXT("上一次推送游戏被中断，重新推送游戏。"));
                }
            }
            BOOL bIsSuccess = StartPushGame();
            if (bIsSuccess)
            {
                m_pLog->WriteLog(LM_INFO, TEXT("推送游戏结束。"));
                if (dwBootTaskVer != dwOldBootTaskVer)
                {
                    SetIniConfig<DWORD>(TEXT("SystemSet"), TEXT("BootTaskVer"), dwBootTaskVer, sClientVersionFile.c_str());
                }
                bWriteIniFlag = TRUE;
            }
            else
            {
                m_pLog->WriteLog(LM_INFO, TEXT("推送游戏失败。"));
                SetIniConfig<DWORD>(TEXT("SystemSet"), TEXT("BootTaskVer"), 0, sClientVersionFile.c_str());
            }
        }
    }

    if (WaitForSingleObject(m_hExitEvent, 0) == WAIT_OBJECT_0)
        return FALSE;
    DWORD dwOldExInfoVer = GetIniConfig<DWORD>(TEXT("SystemSet"), TEXT("ExInfoVer"), 0, sClientVersionFile.c_str());
    if (dwExInfoVer != dwOldExInfoVer)
    {
        m_pLog->WriteLog(LM_INFO, TEXT("客户端扩展信息版本：%u， 服务端扩展信息版本：%u 需要更新。"), dwOldExInfoVer, dwExInfoVer);
        BOOL bIsSuccess = m_clExtendInfoMgr.UpdateExtendInfo();
        if(bIsSuccess)
        {
            bNeedUpdateUI = TRUE;
            SetIniConfig<DWORD>(TEXT("SystemSet"), TEXT("ExInfoVer"), dwExInfoVer, sClientVersionFile.c_str());
            bWriteIniFlag = TRUE;
            m_pLog->WriteLog(LM_INFO, TEXT("扩展信息下载成功。"));
        }
        else m_pLog->WriteLog(LM_INFO, TEXT("扩展信息下载失败。"));
    }

    if (!m_clPlugInInfo.IsHasRun())
    {
        if (WaitForSingleObject(m_hExitEvent, 0) == WAIT_OBJECT_0)
            return FALSE;
        DWORD dwOldPlugInVer = GetIniConfig<DWORD>(TEXT("SystemSet"), TEXT("PlugInVer"), 0, sClientVersionFile.c_str());
        if (dwPlugInVer != dwOldPlugInVer)
        {
            m_pLog->WriteLog(LM_INFO, TEXT("客户端插件信息版本：%u， 服务端插件版本：%u 需要更新。"), dwOldPlugInVer, dwPlugInVer);
            if (m_clPlugInInfo.Download(m_pSystemInfo.get(), m_pIoOperator.get()))
            {
                SetIniConfig<DWORD>(TEXT("SystemSet"), TEXT("PlugInVer"), dwPlugInVer, sClientVersionFile.c_str());
                bWriteIniFlag = TRUE;
                m_pLog->WriteLog(LM_INFO, TEXT("插件信息下载成功。"));
                m_clPlugInInfo.Run(m_pLog, m_hExitEvent);
            }
            else m_pLog->WriteLog(LM_INFO, TEXT("插件信息下载失败。"));
        }
        else
        {
            m_clPlugInInfo.Run(m_pLog, m_hExitEvent);
        }
    }

    if (m_bIsFirstHeart || dwOldSysOptVer != dwSysOptVer)
    {
        DownloadMenuSkin();
        DownloadInternetBarPicture();
        DownloadPostImage(); 
    }

	if(bWriteIniFlag)
		m_pIoOperator->GetHyOperator()->ProtectFile(sClientVersionFile.c_str());

	if (bNeedUpdateUI)
    {
        if (m_bIsFirstHeart) //下载菜单分级信息
        {
            m_clExtendInfoMgr.DownloadClientMenu();
        }
        //新菜单的通讯方式
        if (I8_ClientDataCore_ns::PostPipeMessageToBarOnline(I8_ClientDataCore_ns::E_PIPEMSG_REFRESH_OFFLINE_DATA))
        {
            m_pLog->WriteLog(LM_INFO, TEXT("向客户端数据层发送消息：刷新离线数据。"));
        }
        else
        {
            m_pLog->WriteLog(LM_INFO, TEXT("向客户端数据层发送消息失败：刷新离线数据。"));
        }
        //旧菜单的通讯方式
        int WM_REFRESH_DATA	 = RegisterWindowMessage(TEXT("_i8desk_refresh_data"));
        HWND hFrameWindow = FindWindow(TEXT("DeskXFrame"), NULL);
        if (hFrameWindow != NULL)
            ::SendMessage(hFrameWindow, WM_REFRESH_DATA, (WPARAM)0, (LPARAM)0);
	}
	return TRUE;
}

BOOL CDataCenter::DownloadGameIcon()
{
	if (WaitForSingleObject(m_hExitEvent, 0) == WAIT_OBJECT_0)
		return FALSE;
	if(m_clIconFile.IsNull())
		ReadGameIconFile();

    UINT nOldIconCount = m_clIconFile.GetCount();
	CCommandParam cmdParamIn, cmdParamOut;
	cmdParamIn.InitPackageHeader(CMD_GAME_REQ_ICONS_IDX);
	if(!m_pIoOperator->ExecCommand(&cmdParamIn, TRUE, TRUE, &cmdParamOut)) 
		return FALSE;
	DWORD dwValue = 0;
	cmdParamOut>>dwValue;
	if(dwValue != CMD_RET_SUCCESS) 
		return FALSE;

	map<DWORD, DWORD> mapSvrIconList;
	list<DWORD>       lstIcon;
	DWORD dwIconCount = 0;
	cmdParamOut>>dwIconCount;
	for (DWORD dwLoop = 0; dwLoop < dwIconCount; ++dwLoop)
	{
		DWORD dwGameId  = 0, dwIconCRC = 0;
		cmdParamOut>>dwGameId>>dwIconCRC;
		mapSvrIconList.insert(make_pair(dwGameId, dwIconCRC));

        const std::tr1::shared_ptr<GameIcon_st>* pIcon = m_clIconFile.Find(dwGameId);
		if(pIcon == NULL || pIcon->get()->dwCRC != dwIconCRC) 
			lstIcon.push_back(dwGameId);
	}

	DWORD dwUptIconCount = lstIcon.size();
	DWORD dwTemp = dwUptIconCount;
	while(dwUptIconCount > 0)
	{
		DWORD dwCurUptCount = (dwUptIconCount >= 10 ? 10 : dwUptIconCount);
		dwUptIconCount -= dwCurUptCount;
		cmdParamIn.SetLength(); cmdParamOut.SetLength();
		cmdParamIn.InitPackageHeader(CMD_GAME_REQ_ICONS_DATA);
		cmdParamIn<<dwCurUptCount;
		for (DWORD i = 0; i < dwCurUptCount; ++i)
		{
			cmdParamIn<<lstIcon.front();
			lstIcon.pop_front();			
		}
		if(!m_pIoOperator->ExecCommand(&cmdParamIn, TRUE, TRUE, &cmdParamOut)) 
			return FALSE;
		cmdParamOut>>dwValue;
		if(dwValue != CMD_RET_SUCCESS) 
			return FALSE;
		DWORD dwLastPackage = 0, dwIconCount = 0;
        cmdParamOut>>dwLastPackage>>dwIconCount;
        if (dwIconCount != dwCurUptCount)
        {
            m_pLog->WriteLog(LM_INFO, TEXT("%d个游戏图标没有取到"), dwCurUptCount - dwIconCount);
        }
		while(dwIconCount-- > 0)
		{	
			DWORD dwGID = 0, dwIconSize = 0;
            cmdParamOut>>dwGID>>dwIconSize;

            std::tr1::shared_ptr<GameIcon_st> ptrItem(new GameIcon_st);
            ptrItem->pData = new char[dwIconSize];
            cmdParamOut.PopPointer(ptrItem->pData.Get(), dwIconSize);
            ptrItem->dwSize = dwIconSize;
            ptrItem->dwCRC = GetBufferCRC(ptrItem->pData.Get(), dwIconSize);
            ptrItem->gid = dwGID;
            m_clIconFile.SetData(dwGID, ptrItem);
		}
    }
    if (dwTemp > 0)
    {
        I8_ClientDataCore_ns::CI8ClientOfflineFileLock clLock;
        m_clIconFile.WriteFile();
        clLock.Close();
        stdex::tString strFilePath = GetRootDir() + TEXT("data\\gameicon.dat");
        m_pIoOperator->GetHyOperator()->ProtectFile(strFilePath.c_str());
    }
    m_pLog->WriteLog(LM_INFO, TEXT("下载游戏图标%d个"), m_clIconFile.GetCount() - nOldIconCount);

	if(dwTemp == 0 && mapSvrIconList.size() == m_clIconFile.GetCount()) 
		return TRUE;

    std::vector<DWORD> clIconIndex;
    m_clIconFile.GetAllIndex(clIconIndex);
    lstIcon.clear();
    for (std::vector<DWORD>::const_iterator it = clIconIndex.begin(); it != clIconIndex.end(); ++it)
	{
		if(mapSvrIconList.find(*it) == mapSvrIconList.end()) 
		{
			lstIcon.push_back(*it);
		}
	}
	while(!lstIcon.empty())
	{
		m_clIconFile.DeleteData(lstIcon.front());
		lstIcon.pop_front();
    }

    I8_ClientDataCore_ns::CI8ClientOfflineFileLock clLock;
    m_clIconFile.WriteFile();
    stdex::tString strFilePath = GetRootDir() + TEXT("data\\gameicon.dat");
    m_pIoOperator->GetHyOperator()->ProtectFile(strFilePath.c_str());

	return TRUE;
}

BOOL CDataCenter::DownloadGameInfo()
{
	if (WaitForSingleObject(m_hExitEvent, 0) == WAIT_OBJECT_0)
		return FALSE;

	STSysCfgInfo& stSysCfgInfo = m_pSystemInfo->GetSysCfgInfo(INVALID_SOCKET);
	if(m_mapGameInfo.empty())
		ReadGameInfoFile();

    BOOL bFileExist = PathFileExists((GetRootDir()+ TEXT("data\\GamesInfo.dat")).c_str());
    UINT nOldGameCount = m_mapGameInfo.size();
	CCommandParam cmdParamIn, cmdParamOut;
	cmdParamIn.InitPackageHeader(CMD_GAME_GETIDXS);
	cmdParamIn<<stSysCfgInfo.szName;
	if(!m_pIoOperator->ExecCommand(&cmdParamIn, TRUE, TRUE, &cmdParamOut)) 
		return FALSE;

	DWORD dwValue = 0;
	cmdParamOut>>dwValue;
	if(dwValue != CMD_RET_SUCCESS) 
		return FALSE;

	map<DWORD, std::tr1::shared_ptr<STGameIndexInfo>> mapSvrGameIdx;
	list<DWORD>                  lstGameInfo;
	DWORD dwCount = 0;
	cmdParamOut>>dwCount;
	for (DWORD dwLoop = 0; dwLoop < dwCount; ++dwLoop)
	{
		std::tr1::shared_ptr<STGameIndexInfo> pGameIdxInfo(new STGameIndexInfo);
		cmdParamOut>>pGameIdxInfo->dwGameId>>pGameIdxInfo->dwGameIndex>>pGameIdxInfo->dwRunTypeIndex;
		mapSvrGameIdx.insert(make_pair(pGameIdxInfo->dwGameId, pGameIdxInfo));

		map<DWORD, std::tr1::shared_ptr<STGameIndexInfo>>::iterator it = m_mapGameIndexInfo.find(pGameIdxInfo->dwGameId);
		if(it == m_mapGameIndexInfo.end() || it->second->dwGameIndex != pGameIdxInfo->dwGameIndex \
			|| it->second->dwRunTypeIndex != pGameIdxInfo->dwRunTypeIndex)
		{
			lstGameInfo.push_back(pGameIdxInfo->dwGameId);
		}
	}

	DWORD dwUptGameCount = lstGameInfo.size();
	DWORD dwTemp = dwUptGameCount;
	while(dwUptGameCount > 0)
	{
		DWORD dwCurUptCount = (dwUptGameCount >= 10 ? 10 : dwUptGameCount);
		dwUptGameCount -= dwCurUptCount;
		cmdParamIn.SetLength(); cmdParamOut.SetLength();
		cmdParamIn.InitPackageHeader(CMD_GAME_GETSOME);
		cmdParamIn<<stSysCfgInfo.szName;
		cmdParamIn<<dwCurUptCount;
		for (DWORD i = 0; i < dwCurUptCount; ++i)
		{
			cmdParamIn<<lstGameInfo.front();
			lstGameInfo.pop_front();			
		}
		if(!m_pIoOperator->ExecCommand(&cmdParamIn, TRUE, TRUE, &cmdParamOut)) 
			return FALSE;
		cmdParamOut>>dwValue;
		if(dwValue != CMD_RET_SUCCESS) 
			return FALSE;

		DWORD dwGameCount = 0;
		cmdParamOut>>dwGameCount;
        if (dwGameCount != dwCurUptCount)
        {
            m_pLog->WriteLog(LM_INFO, TEXT("%d个游戏取不到游戏信息"), dwCurUptCount - dwGameCount);
        }
		while(dwGameCount-- > 0)
		{
			std::tr1::shared_ptr<STGameInfo> pGameInfo(new STGameInfo);
			PopGameInfo(cmdParamOut, pGameInfo.get());
            _tcscpy_s(pGameInfo->szArchiveInfo, utility::Base64Decode(pGameInfo->szArchiveInfo).c_str());
			map<DWORD, std::tr1::shared_ptr<STGameInfo>>::iterator it = m_mapGameInfo.find(pGameInfo->dwGameId);
			if(it != m_mapGameInfo.end())
			{
				m_mapGameInfo.erase(it);
			}
			m_mapGameInfo.insert(make_pair(pGameInfo->dwGameId, pGameInfo));
		}
    }
    m_pLog->WriteLog(LM_INFO, TEXT("下载游戏%d个"), m_mapGameInfo.size() - nOldGameCount);

	m_mapGameIndexInfo.clear();
	m_mapGameIndexInfo = mapSvrGameIdx;
	if(dwTemp == 0 && mapSvrGameIdx.size() == m_mapGameInfo.size() && bFileExist) 
		return TRUE;

	lstGameInfo.clear();
	for (map<DWORD, std::tr1::shared_ptr<STGameInfo>>::iterator it = m_mapGameInfo.begin(); it != m_mapGameInfo.end(); ++it)
	{
		if(mapSvrGameIdx.find(it->first) == mapSvrGameIdx.end())
			lstGameInfo.push_back(it->first);
	}
	while(!lstGameInfo.empty())
	{
		map<DWORD, std::tr1::shared_ptr<STGameInfo>>::iterator it = m_mapGameInfo.find(lstGameInfo.front());
		m_mapGameInfo.erase(it);
		lstGameInfo.pop_front();
	}

	WriteGameInfoFile();
	return TRUE;
}

BOOL CDataCenter::ReadGameIconFile()
{
	if (WaitForSingleObject(m_hExitEvent, 0) == WAIT_OBJECT_0)
		return FALSE;

	stdex::tString strIcoFile = GetRootDir() + TEXT("data\\gameicon.dat");
	DWORD  dwFileSize = 0, dwReadBytes = 0;

	CI8ResMgrFile<DWORD>::Result_en enResult = m_clIconFile.SetFile(strIcoFile.c_str());
    switch (enResult)
    {
    case CI8ResMgrFile<DWORD>::E_RST_SUCESS:
        break;
    case CI8ResMgrFile<DWORD>::E_RST_FAIL_ACCESS:
        m_pLog->WriteLog(LM_INFO, TEXT("图标文件无法访问或创建。"));
        return FALSE;
    case CI8ResMgrFile<DWORD>::E_RST_ERR_SIGN:
        m_pLog->WriteLog(LM_INFO, TEXT("错误的图标文件，将重新生成图标文件。"));
        break;
    case CI8ResMgrFile<DWORD>::E_RST_ERR_VERISION:
        m_pLog->WriteLog(LM_INFO, TEXT("图标文件版本过低，将重新生成图标文件。"));
        break;
    case CI8ResMgrFile<DWORD>::E_RST_ERR_UPDATETIME:
        m_pLog->WriteLog(LM_INFO, TEXT("图标文件已损坏，将重新生成图标文件。"));
        break;
    default:
        m_pLog->WriteLog(LM_INFO, TEXT("！图标文件出现未知错误。"));
        return FALSE;
    }
    if (enResult != CI8ResMgrFile<DWORD>::E_RST_SUCESS)
    {
        DeleteFile(strIcoFile.c_str());
        enResult = m_clIconFile.SetFile(strIcoFile.c_str());
        stdex::tString sClientVersionFile = GetRootDir() + TEXT("GameClient_Version.ini");
        SetIniConfig<DWORD>(TEXT("SystemSet"), TEXT("IcoVer"), 0, sClientVersionFile.c_str());
    }
    if (enResult != CI8ResMgrFile<DWORD>::E_RST_SUCESS)
    {
        m_pLog->WriteLog(LM_INFO, TEXT("！图标文件无法访问或创建。"));
        return FALSE;
    }
    
	return TRUE;
}

BOOL CDataCenter::ReadGameInfoFile()
{
	if (WaitForSingleObject(m_hExitEvent, 0) == WAIT_OBJECT_0)
		return FALSE;

	m_mapGameInfo.clear();
	char    *pGameInfoMem = NULL;
	DWORD   dwGameInfoFileSize = 0;	
	stdex::tString strDataFile = GetRootDir()+ TEXT("data\\GamesInfo.dat");

	//打开文件			
	utility::CAutoFile clFile = ::CreateFile(strDataFile.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!clFile.IsValid()) return FALSE;
	DWORD dwFileSize = GetFileSize(clFile, NULL);
	char* pFileMem   = new char[dwFileSize + 1];
	DWORD dwReadBytes = 0;
	if (!ReadFile(clFile, pFileMem, dwFileSize, &dwReadBytes, NULL) || dwReadBytes != dwFileSize || dwFileSize < 30)
	{
		delete[] pFileMem;
		pFileMem = NULL;
		return FALSE;
	}
    clFile.CleanUp();

	try
	{
		CCommandParam cmdParam;
		cmdParam.SetBuffer((BYTE*)pFileMem, RELEASE_MODE_DELETE);
		DWORD dwVersion = 0, dwGameCount = 0;
		cmdParam>>dwVersion;
        if (dwVersion == I8_ClientDataCore_ns::C_CLIENT_OFFLINE_DATA_VERSION_GAME_INFO)
        {
            cmdParam>>dwGameCount;
            for (DWORD dwLoop = 0; dwLoop < dwGameCount; ++dwLoop)
            {
                std::tr1::shared_ptr<STGameInfo> pGameInfo(new STGameInfo);
                PopGameInfo(cmdParam, pGameInfo.get());	
                m_mapGameInfo.insert(make_pair(pGameInfo->dwGameId, pGameInfo));					
            }
        }
        else
        {
            stdex::tString sClientVersionFile = GetRootDir() + TEXT("GameClient_Version.ini");
            SetIniConfig<DWORD>(TEXT("SystemSet"), TEXT("GameVer"), 0, sClientVersionFile.c_str());
        }
	}
	catch(...)
	{
		m_mapGameInfo.clear();
	}

	return TRUE;
}

BOOL CDataCenter::WriteGameInfoFile()
{
	TCHAR szKey[MAX_PATH] = {0}, szData[1024] = {0};   
	SHDeleteKey(HKEY_LOCAL_MACHINE, KEY_I8DESK_GID);

	DWORD dwGameCount = m_mapGameInfo.size();
	DWORD dwTotalSize = sizeof(pkgheader) + 2*sizeof(DWORD) + dwGameCount*sizeof(STGameInfo);

	CCommandParam cmdParam(dwTotalSize);
	cmdParam.InitPackageHeader(CMD_GAME_GETSOME);
    cmdParam<<I8_ClientDataCore_ns::C_CLIENT_OFFLINE_DATA_VERSION_GAME_INFO<<dwGameCount;
	for (map<DWORD, std::tr1::shared_ptr<STGameInfo>>::iterator it = m_mapGameInfo.begin(); it != m_mapGameInfo.end(); ++it)
	{
		//写注册表
		wsprintf(szKey, TEXT("%lu"), it->second->dwGameId);
		wsprintf(szData, TEXT("name=%s;mode=%d;remotepath=%s;localpath=%s"),	it->second->szGameName, it->second->dwRunType, it->second->szServerPath, it->second->szClientPath);
		SHSetValue(HKEY_LOCAL_MACHINE, KEY_I8DESK_GID, szKey, REG_SZ, szData, lstrlen(szData) * sizeof(TCHAR));	
		PushGameInfo(cmdParam, it->second.get());
	}	
	stdex::tString strFilePath = GetRootDir() + TEXT("data\\GamesInfo.dat");
    I8_ClientDataCore_ns::CI8ClientOfflineFileLock clLock;
	m_pIoOperator->WriteFile(strFilePath.c_str(), (const char*)cmdParam.GetBuffer(), cmdParam.GetBufferLength());
    clLock.Close();

	return TRUE;
}

void CDataCenter::CreateGameShortcut()
{
    stdex::tString strMenuPath(GetRootDir() + MODULE_BARONLINE);
    vector<STShortcut> vctShortcut;
    for (map<DWORD, std::tr1::shared_ptr<STGameInfo>>::iterator it = m_mapGameInfo.begin(); it != m_mapGameInfo.end(); ++it)
    {
        if(it->second->dwDesktopLink == 1)
        {
            const std::tr1::shared_ptr<GameIcon_st>* pIcon = m_clIconFile.Find(it->second->dwGameId);
            if (pIcon != NULL)
            {
                STShortcut stShortcut;
                wsprintf(stShortcut.szTargetPath, TEXT("%s"),    strMenuPath.c_str());
                wsprintf(stShortcut.szName,       TEXT("%s"),    it->second->szGameName);
                wsprintf(stShortcut.szParam,      TEXT("/start"));
                wsprintf(stShortcut.szValue,      TEXT("%d"),    it->second->dwGameId);
                wsprintf(stShortcut.szIconPath,   TEXT("%sdata\\icon\\%d.ico"), GetRootDir().c_str(), it->second->dwGameId);
                m_pIoOperator->WriteFile(stShortcut.szIconPath, pIcon->get()->pData.Get(), pIcon->get()->dwSize, FALSE);
                vctShortcut.push_back(stShortcut);
                m_pLog->WriteLog(LM_INFO, TEXT("创建游戏快捷方式:%s。"), stShortcut.szName);
            }
            else
            {
                m_pLog->WriteLog(LM_INFO, TEXT("创建游戏快捷方式失败，找不到图标：%s。"), it->second->szGameName);
            }
        }
    }
    ClearShortcut(strMenuPath.c_str(), TEXT("/start"));
    CreateShortcut(vctShortcut, m_pLog);
}

BOOL CDataCenter::PushGameInfo( CCommandParam &cmdParam, STGameInfo *pGameInfo )
{
	cmdParam<<pGameInfo->dwGameId<<pGameInfo->dwAssociatedId<<pGameInfo->szGameName<<pGameInfo->szClassName<<pGameInfo->szExeName \
		<<pGameInfo->szExeParam<<pGameInfo->dwGameSize<<pGameInfo->dwDesktopLink \
		<<pGameInfo->dwMenuToolBar<<pGameInfo->szServerPath<<pGameInfo->szClientPath<<pGameInfo->dwDeleteFlg \
		<<pGameInfo->szFeatureFile<<pGameInfo->szArchiveInfo<<pGameInfo->dwIDCUpdateTime \
		<<pGameInfo->dwIDCVersion<<pGameInfo->dwServerVersion<<pGameInfo->dwUpdate \
		<<pGameInfo->dwI8Pay<<pGameInfo->dwIDCClickNum<<pGameInfo->dwServerClickNum<<pGameInfo->dwRunType \
		<<pGameInfo->szVDiskID<<pGameInfo->dwStatus<<pGameInfo->szComment<<pGameInfo->dwRepair;

	return TRUE;
}

BOOL CDataCenter::PopGameInfo( CCommandParam &cmdParam, STGameInfo *pGameInfo )
{
	cmdParam>>pGameInfo->dwGameId>>pGameInfo->dwAssociatedId>>pGameInfo->szGameName>>pGameInfo->szClassName>>pGameInfo->szExeName \
		>>pGameInfo->szExeParam>>pGameInfo->dwGameSize>>pGameInfo->dwDesktopLink \
		>>pGameInfo->dwMenuToolBar>>pGameInfo->szServerPath>>pGameInfo->szClientPath>>pGameInfo->dwDeleteFlg \
		>>pGameInfo->szFeatureFile>>pGameInfo->szArchiveInfo>>pGameInfo->dwIDCUpdateTime \
		>>pGameInfo->dwIDCVersion>>pGameInfo->dwServerVersion>>pGameInfo->dwUpdate \
		>>pGameInfo->dwI8Pay>>pGameInfo->dwIDCClickNum>>pGameInfo->dwServerClickNum>>pGameInfo->dwRunType \
		>>pGameInfo->szVDiskID>>pGameInfo->dwStatus>>pGameInfo->szComment>>pGameInfo->dwRepair;

	PathAddBackslash(pGameInfo->szServerPath);
    PathAddBackslash(pGameInfo->szClientPath);

	std::map<stdex::tString, STVirtualDiskInfo>::iterator it = m_mapVDiskInfo.find(pGameInfo->szVDiskID);
	if (it != m_mapVDiskInfo.end())
	{
		tstrcpy(pGameInfo->szVDiskPath, pGameInfo->szServerPath);
		pGameInfo->szVDiskPath[0] = (TCHAR)(it->second.dwClientDrive);
	}
	return TRUE;
}

BOOL CDataCenter::ReadSysOptFile()
{
	if (WaitForSingleObject(m_hExitEvent, 0) == WAIT_OBJECT_0)
		return FALSE;
	stdex::tString strFilePath = GetRootDir() + TEXT("data\\sysopt.dat");
	CCommandParam cmdParam;
	if(m_pIoOperator->ReadFile(strFilePath.c_str(), cmdParam) == 0 )
		return FALSE;

	DWORD dwVersion;
	cmdParam>>dwVersion;	
    if (dwVersion != I8_ClientDataCore_ns::C_CLIENT_OFFLINE_DATA_VERSION_SYSOPT)
		return FALSE;

	m_mapSysOption.clear();

	DWORD dwSize = 0;
	cmdParam>>dwSize;
	TCHAR szName[4096] = {0}, szValue[4096] = {0};

	try
	{
		for (DWORD i = 0; i < dwSize; ++i)
		{
			cmdParam.PopString(szName, 4096);
			char ch = 0;
			cmdParam>>ch;
			cmdParam.PopString(szValue, 4096);	
			m_mapSysOption.insert(make_pair(stdex::tString(szName), stdex::tString(szValue)));
		}
	}
	catch (...)
	{
		m_mapSysOption.clear();
	}

	return TRUE;
}

BOOL CDataCenter::ReadSvrCfgFile()
{
	if (WaitForSingleObject(m_hExitEvent, 0) == WAIT_OBJECT_0)
		return FALSE;
	stdex::tString strFilePath = GetRootDir() + TEXT("data\\ServerCfg.dat"); 
	CCommandParam cmdParamIn, cmdParamOut;
	if(m_pIoOperator->ReadFile(strFilePath.c_str(), cmdParamOut) == 0) 
		return FALSE;
	DWORD dwValue;
	cmdParamOut>>dwValue;	
    if(dwValue != I8_ClientDataCore_ns::C_CLIENT_OFFLINE_DATA_VERSION_SERVER_CFG)
		return FALSE;

	BOOL bAddMainServer = TRUE;
	DWORD dwServerCount = 0;
	cmdParamOut>>dwServerCount;
	for (DWORD dwLoop = 0; dwLoop < dwServerCount; ++dwLoop)
	{
		STServerInfo stSvrCfgItem;
		cmdParamOut>>stSvrCfgItem.dwUpdateIP>>stSvrCfgItem.dwVDiskIPCount;
        cmdParamOut.PopPointer(stSvrCfgItem.adwVDiskIP, min(stSvrCfgItem.dwVDiskIPCount, _countof(stSvrCfgItem.adwVDiskIP)));

		m_vctServerInfo.push_back(stSvrCfgItem);
	}
	if (bAddMainServer)
	{
        STServerInfo stSvrCfgItem;
        stSvrCfgItem.dwUpdateIP = dwValue;
        stSvrCfgItem.dwVDiskIPCount = 1;
		stSvrCfgItem.adwVDiskIP[0] = dwValue;

		m_vctServerInfo.push_back(stSvrCfgItem);
	}
	return TRUE;
}


BOOL CDataCenter::ReadVDiskFile()
{
	if (WaitForSingleObject(m_hExitEvent, 0) == WAIT_OBJECT_0)
		return FALSE;
	stdex::tString strFilePath = GetRootDir() + TEXT("data\\VDisk2.dat"); 
	CCommandParam cmdParam;
	if(m_pIoOperator->ReadFile(strFilePath.c_str(), cmdParam) == 0) 
		return FALSE;
	DWORD dwVersion;
	cmdParam>>dwVersion;	
    if(dwVersion != I8_ClientDataCore_ns::C_CLIENT_OFFLINE_DATA_VERSION_VDISK)
		return FALSE;

	DWORD dwCount = 0;
	cmdParam>>dwCount;
	for (DWORD i = 0; i < dwCount; ++i)
	{
		STVirtualDiskInfo stVDisk;
		cmdParam>>stVDisk.szVDiskId>>stVDisk.dwServerIP>>stVDisk.dwServerPort>>stVDisk.dwServerDrive \
			>>stVDisk.dwClientDrive>>stVDisk.dwDiskType>>stVDisk.dwStartType>>stVDisk.szServerId>>stVDisk.dwSize>>stVDisk.dwSsdDrv;
		m_mapVDiskInfo.insert(make_pair(stVDisk.szVDiskId, stVDisk));
	}
	return TRUE;
}

STCltHardwareInfo& CDataCenter::GetHardwareInfo()
{
	return m_pSystemInfo->GetHardwareInfo();
}

STSysCfgInfo& CDataCenter::GetSysCfgInfo()
{
	return m_pSystemInfo->GetSysCfgInfo(m_pCliPlugMgr->GetCommunication()->GetTcpSocket());
}

STI8Version& CDataCenter::GetI8Version()
{
	return m_pSystemInfo->GetI8Version();
}

STHyInfo& CDataCenter::GetHyInfo()
{
	return m_pSystemInfo->GetHyInfo();
}

const map<stdex::tString, STVirtualDiskInfo>& CDataCenter::GetVDiskInfo()
{
	return m_mapVDiskInfo;
}

const vector<STServerInfo>& CDataCenter::GetSvrCfgInfo()
{
	return m_vctServerInfo;
}

stdex::tString CDataCenter::GetStringOpt(stdex::tString strKey)
{
	stdex::tString strValue;
	map<stdex::tString, stdex::tString>::iterator it = m_mapSysOption.find(strKey);
	if (it != m_mapSysOption.end() && it->second.size()) 
		strValue = it->second;
	return strValue;
}

int CDataCenter::GetIntOpt( stdex::tString strKey )
{
	map<stdex::tString, stdex::tString>::iterator it = m_mapSysOption.find(strKey);
	if (it != m_mapSysOption.end()) 
		return _ttoi(it->second.c_str());
	return -1;
}

BOOL CDataCenter::GetBoolOpt( stdex::tString strKey )
{
	map<stdex::tString, stdex::tString>::iterator it = m_mapSysOption.find(strKey);
	if (it != m_mapSysOption.end()) 
		return _ttoi(it->second.c_str()) != 0;
	return FALSE;
}

BOOL CDataCenter::DownloadMenuSkin()
{
	if (WaitForSingleObject(m_hExitEvent, 0) == WAIT_OBJECT_0)
		return FALSE;
	stdex::tString strSkinInfo = GetStringOpt(OPT_M_CLISKIN);
	if(strSkinInfo.empty() || m_sMenuSkinSvrPath == strSkinInfo) return FALSE;

	size_t iPos = strSkinInfo.find('|');
	if(iPos == stdex::tString::npos)
		return FALSE;
	strSkinInfo = strSkinInfo.substr(iPos+1);
	if (lstrcmpi(strSkinInfo.c_str(), DEFAULT_SKIN_NAME) == 0)
		return TRUE;

	stdex::tString strSkinFile = GetRootDir() + TEXT("Skin\\")+ strSkinInfo;
	stdex::tString strSvrFile  = stdex::tString(TEXT("Skin\\")) + strSkinInfo;
    m_pLog->WriteLog(LM_INFO, TEXT("下载菜单皮肤:%s."), strSkinFile.c_str());
    m_sMenuSkinSvrPath = strSkinInfo;

	DWORD dwSize  = 0; char* pBuffer = NULL;
	return m_pIoOperator->DownloadFile(1, FP_REALPATH, strSvrFile.c_str(), strSkinFile.c_str(), dwSize, &pBuffer, FALSE, TRUE, TRUE);
}

BOOL CDataCenter::DownloadInternetBarPicture()
{
    if (WaitForSingleObject(m_hExitEvent, 0) == WAIT_OBJECT_0)
        return FALSE;
    stdex::tString strInternetBarPicture = GetStringOpt(OPT_M_NETBARJPG);
    if(strInternetBarPicture.empty() || m_sInternetBarPictureSvrPath == strInternetBarPicture) return FALSE;

    TCHAR atPictureName[MAX_PATH];
    lstrcpy(atPictureName, strInternetBarPicture.c_str());
    PathStripPath(atPictureName);
    stdex::tString strCliPath = GetRootDir() + TEXT("Data\\Image\\") + atPictureName;
    m_pLog->WriteLog(LM_INFO, TEXT("下载网吧照片:%s."), strInternetBarPicture.c_str());
    m_sInternetBarPictureSvrPath = strInternetBarPicture;

    DWORD dwSize  = 0; char* pBuffer = NULL;
    return m_pIoOperator->DownloadFile(1, FP_ABSPATH, strInternetBarPicture.c_str(), strCliPath.c_str(), dwSize, &pBuffer, FALSE, TRUE, TRUE);
}

BOOL CDataCenter::DownloadPostImage()
{
    if (WaitForSingleObject(m_hExitEvent, 0) == WAIT_OBJECT_0)
        return FALSE;
    stdex::tString strPostImage = GetStringOpt(OPT_M_GGPICDIR);
    if(strPostImage.empty() || m_sPostImageSvrPath == strPostImage) return FALSE;

    TCHAR atImageName[MAX_PATH];
    lstrcpy(atImageName, strPostImage.c_str());
    PathStripPath(atImageName);
    stdex::tString strCliPath = GetRootDir() + TEXT("Data\\Image\\") + atImageName;
    m_pLog->WriteLog(LM_INFO, TEXT("下载公告图片:%s."), strPostImage.c_str());
    m_sPostImageSvrPath = strPostImage;

    DWORD dwSize  = 0; char* pBuffer = NULL;
    return m_pIoOperator->DownloadFile(1, FP_ABSPATH, strPostImage.c_str(), strCliPath.c_str(), dwSize, &pBuffer, FALSE, TRUE, TRUE);
}

BOOL CDataCenter::DownloadSysOption()
{
	if (WaitForSingleObject(m_hExitEvent, 0) == WAIT_OBJECT_0)
		return FALSE;
	stdex::tString strFilePath = GetRootDir() + TEXT("data\\sysopt.dat");
	CCommandParam cmdParamIn, cmdParamOut;
	cmdParamIn.InitPackageHeader(CMD_SYSOPT_GETLIST);
	if(!m_pIoOperator->ExecCommand(&cmdParamIn, TRUE, TRUE, &cmdParamOut)) 
		return FALSE;

	DWORD dwValue;
	cmdParamOut>>dwValue;
	if (dwValue != CMD_RET_SUCCESS)
		return FALSE;
    m_pLog->WriteLog(LM_INFO, TEXT("下载系统选项:%s."), strFilePath.c_str());
    (*reinterpret_cast<DWORD*>(cmdParamOut.GetBuffer() + sizeof(i8desk::pkgheader))) = I8_ClientDataCore_ns::C_CLIENT_OFFLINE_DATA_VERSION_SYSOPT;
    I8_ClientDataCore_ns::CI8ClientOfflineFileLock clLock;
	m_pIoOperator->WriteFile(strFilePath.c_str(), (const char*)cmdParamOut.GetBuffer(), cmdParamOut.GetBufferLength());
    clLock.Close();
	return ReadSysOptFile();
}

BOOL CDataCenter::DownloadSvrCfg()
{
	if(!m_vctServerInfo.empty())
		return FALSE;
	STSysCfgInfo& stSysCfgInfo = m_pSystemInfo->GetSysCfgInfo(INVALID_SOCKET);

	stdex::tString strFilePath = GetRootDir() + TEXT("data\\ServerCfg.dat"); 
	CCommandParam cmdParamIn, cmdParamOut;

	cmdParamIn.InitPackageHeader(CMD_GET_SERVER_CONFIG);
	cmdParamIn<<stSysCfgInfo.szName;
	if(!m_pIoOperator->ExecCommand(&cmdParamIn, TRUE, TRUE, &cmdParamOut) ) 
	{
		m_pLog->WriteLog(LM_INFO, TEXT("下载服务器配置信息失败。"));
		return FALSE;
	}

	DWORD dwValue;
	cmdParamOut>>dwValue;
	if (dwValue != CMD_RET_SUCCESS)
	{
		return FALSE;
	}

    (*reinterpret_cast<DWORD*>(cmdParamOut.GetBuffer() + sizeof(i8desk::pkgheader))) = I8_ClientDataCore_ns::C_CLIENT_OFFLINE_DATA_VERSION_SERVER_CFG;
    I8_ClientDataCore_ns::CI8ClientOfflineFileLock clLock;
	m_pIoOperator->WriteFile(strFilePath.c_str(), (const char*)cmdParamOut.GetBuffer(), cmdParamOut.GetBufferLength());
    clLock.Close();

	BOOL bAddMainServer = TRUE;
	DWORD dwServerCount = 0;
	cmdParamOut>>dwServerCount;
	for (DWORD dwLoop = 0; dwLoop < dwServerCount; ++dwLoop)
	{
		STServerInfo stSvrCfgItem;
		cmdParamOut>>stSvrCfgItem.dwUpdateIP>>stSvrCfgItem.dwVDiskIPCount;
        cmdParamOut.PopPointer(stSvrCfgItem.adwVDiskIP, min(stSvrCfgItem.dwVDiskIPCount, _countof(stSvrCfgItem.adwVDiskIP)));

		m_vctServerInfo.push_back(stSvrCfgItem);
		DWORD dwValue = inet_addr(CT2A(m_strServerAddr.c_str()));
		if (stSvrCfgItem.dwUpdateIP == dwValue)
		{
			bAddMainServer = FALSE;
		}
	}
	if (bAddMainServer)
	{
		STServerInfo stSvrCfgItem;
		DWORD dwValue = inet_addr(CT2A(m_strServerAddr.c_str()));
        stSvrCfgItem.dwVDiskIPCount = 1;
	    stSvrCfgItem.adwVDiskIP[0] = dwValue;
		stSvrCfgItem.dwUpdateIP =  dwValue;

		m_vctServerInfo.push_back(stSvrCfgItem);
	}
    if (m_vctServerInfo.size() == 0)
    {
        m_pLog->WriteLog(LM_INFO, TEXT("下载服务器配置信息成功。"));
    }
    else
    {
        m_pLog->WriteLog(LM_INFO, TEXT("下载服务器配置信息成功，分配服务器%s。"), ConvertIPToString(m_vctServerInfo[0].dwUpdateIP).c_str());
    }
	return TRUE;
}

BOOL CDataCenter::DownloadVDiskInfo()
{
	if(m_bDownVDiskInfo) 
		return FALSE;

	stdex::tString strFilePath = GetRootDir() + TEXT("data\\VDisk2.dat"); 
	CCommandParam cmdParamIn, cmdParamOut;

	cmdParamIn.InitPackageHeader(CMD_GET_VDISK_CONFIG);
	if(!m_pIoOperator->ExecCommand(&cmdParamIn, TRUE, TRUE, &cmdParamOut)) 
	{
		m_pLog->WriteLog(LM_INFO, TEXT("下载虚拟盘配置信息失败， 失败原因：网络命令执行失败。"));
		return FALSE;
	}

	DWORD dwValue;
	cmdParamOut>>dwValue;
	if (dwValue != CMD_RET_SUCCESS)
	{
		m_pLog->WriteLog(LM_INFO, TEXT("下载虚拟盘配置信息失败， 失败原因：服务器应答失败。"));
		return FALSE;
	}
    (*reinterpret_cast<DWORD*>(cmdParamOut.GetBuffer() + sizeof(i8desk::pkgheader))) = I8_ClientDataCore_ns::C_CLIENT_OFFLINE_DATA_VERSION_VDISK;
    I8_ClientDataCore_ns::CI8ClientOfflineFileLock clLock;
	m_pIoOperator->WriteFile(strFilePath.c_str(), (const char*)cmdParamOut.GetBuffer(), cmdParamOut.GetBufferLength());
    clLock.Close();

	DWORD dwCount = 0;
	cmdParamOut>>dwCount;
	for (DWORD i = 0; i < dwCount; ++i)
	{
		STVirtualDiskInfo stVDisk;
		cmdParamOut>>stVDisk.szVDiskId>>stVDisk.dwServerIP>>stVDisk.dwServerPort
			>>stVDisk.dwServerDrive>>stVDisk.dwClientDrive>>stVDisk.dwDiskType 
			>>stVDisk.dwStartType>>stVDisk.szServerId>>stVDisk.dwSize>>stVDisk.dwSsdDrv;
		m_mapVDiskInfo.insert(make_pair(stVDisk.szVDiskId, stVDisk));
	}
	m_pLog->WriteLog(LM_INFO, TEXT("下载虚拟盘配置信息成功。"));
    m_bDownVDiskInfo = TRUE;
	return TRUE;
}

stdex::tString CDataCenter::GetServerAddr()
{
    if(m_strServerAddr.empty())
	{
		TCHAR szServerIp[MAX_PATH] = {0};
		stdex::tString strCfgFile = GetRootDir() + TEXT("gameclient.ini");
		GetPrivateProfileString(TEXT("SystemSet"), TEXT("ServerAddr"), NULL, szServerIp, MAX_PATH, strCfgFile.c_str());
		m_strServerAddr = szServerIp;
	}
	return m_strServerAddr;
}

BOOL CDataCenter::LoadPushGameList(std::list<PushGameBaseInfo_st>& clPushGameTask)
{
    clPushGameTask.clear();
    CCommandParam cmdParamIn, cmdParamOut;
    cmdParamIn.InitPackageHeader(CMD_BOOT_GETLIST);
    cmdParamIn<<GetSysCfgInfo().szName;

    I8_AUTO_TRY
        if(!m_pIoOperator->ExecCommand(&cmdParamIn, TRUE, TRUE, &cmdParamOut)) 
            return FALSE;
    DWORD dwValue;
    cmdParamOut>>dwValue;
    if (dwValue != CMD_RET_SUCCESS)
        return FALSE;
    DWORD dwTaskCount = 0;
    cmdParamOut>>dwTaskCount;
    TCHAR atSystemDir[MAX_PATH];
    GetSystemDirectory(atSystemDir, MAX_PATH);
    BOOL bHySysDriver = GetIoOperator()->GetHyOperator()->IsDriverProtected(atSystemDir[0]);
    for (DWORD i = 0; i < dwTaskCount; ++i)
    {
        STBootTaskParam stTaskParam;
        if (!CBootTask::ReadBootTask(cmdParamOut, bHySysDriver, stTaskParam, NULL, m_pLog)) //推送游戏时忽略所有限制条件
        {
            continue;
        }
        if (stTaskParam.dwTaskType == brUpdateGame)
        {
            vector<stdex::tString> vctParam;
            splitString(stTaskParam.szAppendParam, vctParam);
            if (vctParam.size() < 3)
            {
                m_pLog->WriteLog(LM_INFO, TEXT("推送游戏的参数不正确(%s)"), stTaskParam.szAppendParam);
                continue;
            }
            PushGameBaseInfo_st stPushGameInfo;
            tstrcpy(stPushGameInfo.szTaskId, stTaskParam.szTaskId);
            stPushGameInfo.dwSpeed = _ttoi(vctParam[0].c_str());
            stPushGameInfo.dwCmpMode = _ttoi(vctParam[1].c_str());
            if (stPushGameInfo.dwCmpMode != FC_COPY_QUICK && stPushGameInfo.dwCmpMode != FC_COPY_FORE && stPushGameInfo.dwCmpMode != FC_COPY_VERSION)
            {
                m_pLog->WriteLog(LM_INFO, TEXT("推送游戏的文件对比方式不正确(%s)"), stTaskParam.szAppendParam);
                continue;
            }
            for (size_t i = 2; i < vctParam.size(); ++i)
            {
                stPushGameInfo.dwGameGID = _ttoi(vctParam[i].c_str());
                clPushGameTask.push_back(stPushGameInfo);
            }
        }
    }
    return TRUE;

    I8_AUTO_CATCH
        return FALSE;
}

BOOL CDataCenter::StartPushGame()
{
    std::list<PushGameBaseInfo_st> clPushGameTask;
    if (!LoadPushGameList(clPushGameTask))
    {
        return FALSE;
    }
    if (!PushGame(clPushGameTask))
    {
        return FALSE;
    }
    return TRUE;
}
BOOL CDataCenter::PushGame(std::list<PushGameBaseInfo_st>& clPushGameTask)
{
    std::vector<tagUpGameinfo> vtPushFile;
    DWORD dwIndex = 0;

    for (std::list<PushGameBaseInfo_st>::const_iterator clId = clPushGameTask.begin(); clId != clPushGameTask.end(); ++clId)
    {
        const PushGameBaseInfo_st& stPushGameInfo = *clId;
        map<DWORD, std::tr1::shared_ptr<STGameInfo>>::iterator it = m_mapGameInfo.find(stPushGameInfo.dwGameGID);
        if(it == m_mapGameInfo.end())
        {
            m_pLog->WriteLog(LM_INFO, TEXT("游戏(%lu)不存在，推送失败"), stPushGameInfo.dwGameGID);
            continue;
        }
        if(it->second->dwRunType != GAME_RT_LOCAL)
        {
            m_pLog->WriteLog(LM_INFO, TEXT("游戏(%lu)不需要下载，无需推送"), stPushGameInfo.dwGameGID);
            continue;
        }

        tagUpGameinfo stGameInfo ;
        stGameInfo.GID = it->second->dwGameId;
        lstrcpy(stGameInfo.CLIPATH,it->second->szClientPath);
        lstrcpy(stGameInfo.SVRPATH,it->second->szServerPath); 	
        stGameInfo.Flag = (FC_COPY_MODE)stPushGameInfo.dwCmpMode;
        stGameInfo.speed = stPushGameInfo.dwSpeed;
        lstrcpy(stGameInfo.szTaskId, stPushGameInfo.szTaskId);
        stGameInfo.Srvip = inet_addr(CT2A(GetServerAddr().c_str()));
        vtPushFile.push_back(stGameInfo);
        m_pLog->WriteLog(LM_INFO, TEXT("推送游戏: gid:%lu  tid:%s  服务端目录:%s  客户端目录:%s"), stGameInfo.GID, stGameInfo.szTaskId, stGameInfo.SVRPATH, stGameInfo.CLIPATH);
        dwIndex++;
    }

    if (vtPushFile.size() > 0)
    {
        m_clPushGame->PushGame(vtPushFile);
    }
    else
    {
        if (clPushGameTask.size() == 0)
        {
            m_pLog->WriteLog(LM_INFO, TEXT("没有设置需要推送的游戏"));
        }
        else
        {
            m_pLog->WriteLog(LM_INFO, TEXT("没有检测到需要推送的游戏"));
        }
        m_clPushGame->EndPushGame();
    }
    return TRUE;
}

void CDataCenter::ReceivePipeMessage(CI8NamedPipeServer& clPipe)
{
    I8_CliSvrPipe_ns::PipeDataHead_st stPipeDataHead;
    clPipe.ReadData(&stPipeDataHead, sizeof(stPipeDataHead));
    if (memcmp(stPipeDataHead.acSign, I8_CliSvrPipe_ns::C_PIPE_DATA_SIGN, sizeof(stPipeDataHead.acSign)) != 0)
    {
        assert(0 && _T("命名管道的数据标记不正确"));
        return;
    }
    if (stPipeDataHead.nVersion != I8_CliSvrPipe_ns::C_PIPE_DATA_VERISION)
    {
        assert(0 && _T("命名管道的数据版本太高，无法处理"));
        return;
    }
    switch (stPipeDataHead.enMessage)
    {
    case I8_CliSvrPipe_ns::E_PIPEMSG_REGISTER_UPDATE:
        {
            assert(stPipeDataHead.iDataSize == sizeof(DWORD));
            DWORD gid;
            clPipe.ReadData(&gid, sizeof(DWORD));
            m_pLog->WriteLog(LM_INFO, _T("收到命名管道消息：更新游戏(%d)"), gid);
            m_clPushGame->SetUpdatingGame(gid);
            DWORD dwResult = 1;
            clPipe.WriteData(&dwResult, sizeof(DWORD));
        }
        break;
    case I8_CliSvrPipe_ns::E_PIPEMSG_UNREGISTER_UPDATE:
        {
            assert(stPipeDataHead.iDataSize == sizeof(DWORD));
            DWORD gid;
            clPipe.ReadData(&gid, sizeof(DWORD));
            m_pLog->WriteLog(LM_INFO, _T("收到命名管道消息：结束更新游戏(%d)"), gid);
            m_clPushGame->SetUpdatingGame(0);
        }
        break;
    case I8_CliSvrPipe_ns::E_PIPEMSG_STARTUP_BARONLINE:
        {
            m_pLog->WriteLog(LM_INFO, _T("收到命名管道消息：客户端界面启动"));
            m_dwRunBarOnline = GetTickCount();
        }
        break;
    default:
        m_pLog->WriteLog(LM_INFO, _T("收到未知命名管道消息"));
        break;
    }
}

void CDataCenter::ReportConnect(ClientConnectSvr enConnectType, DWORD dwIP)
{
    CCommandParam cmdParamIn, cmdParamOut;
    cmdParamIn.InitPackageHeader(CMD_REPORT_SVRIP);
    cmdParamIn<<DWORD(enConnectType)<<GetLocalIP()<<dwIP<<DWORD(0);
    if (m_pIoOperator->ExecCommand(&cmdParamIn, TRUE, TRUE, &cmdParamOut))
    {
        m_pLog->WriteLog(LM_INFO, TEXT("上报连接信息成功(%s) %s"), enConnectType == i8desk::UpdateIP ? _T("更新") : _T("虚拟盘"), ConvertIPToString(dwIP).c_str());
    }
    else
    {
        m_pLog->WriteLog(LM_INFO, TEXT("上报连接信息失败(%s) %s"), enConnectType == i8desk::UpdateIP ? _T("更新") : _T("虚拟盘"), ConvertIPToString(dwIP).c_str());
    }
}

void __stdcall CDataCenter::OnUpdateConnect(void* pCallbackParam, DWORD dwIP)
{
    CDataCenter* pDataCenter = static_cast<CDataCenter*>(pCallbackParam);
    assert(pDataCenter != NULL);
    pDataCenter->ReportConnect(i8desk::UpdateIP, dwIP);
}

void CDataCenter::ReportClientExit()
{
    CCommandParam cmdParamIn, cmdParamOut;
    cmdParamIn.InitPackageHeader(CMD_REPORT_SVRIP);
    cmdParamIn<<DWORD(0)<<GetLocalIP()<<DWORD(0)<<DWORD(1);
    if (m_pIoOperator->ExecCommand(&cmdParamIn, TRUE, TRUE, &cmdParamOut))
    {
        m_pLog->WriteLog(LM_INFO, TEXT("上报退出信息成功。"));
    }
    else
    {
        m_pLog->WriteLog(LM_INFO, TEXT("上报退出信息失败。"));
    }
}

BOOL CDataCenter::RefreshVirtualDisk()
{
    const vector<STServerInfo>& vctServerInfo = GetSvrCfgInfo();
    const map<stdex::tString, STVirtualDiskInfo>& mapVDiskInfo = GetVDiskInfo();

    CVirtualDiskDll clVirtualDisk;
    if (!clVirtualDisk.Load((GetRootDir() + TEXT("Refresh.dll")).c_str()))
    {
        m_pLog->WriteLog(LM_INFO, _T("加载Refresh.dll失败。"));
        return 0;
    }

    stdex::tString strVDiskTmpPath = GetStringOpt(OPT_M_VDDIR);
    if(strVDiskTmpPath.empty())
        strVDiskTmpPath = TEXT("C:\\Temp\\");
    if(*strVDiskTmpPath.rbegin() != TEXT('\\')) 
        strVDiskTmpPath += TEXT('\\');
    utility::MakeSureDirectoryPathExists(strVDiskTmpPath.c_str());

    win32::file::ClearDirectory(strVDiskTmpPath.c_str());

    stdex::tString strRegInfo;
    m_pLog->WriteLog(LM_INFO, TEXT("开始刷新虚拟盘"));
    BOOL bSuccess = FALSE;
    for (UINT i = 0; i < vctServerInfo.size() && !bSuccess; ++i)
    {
        for (map<stdex::tString, STVirtualDiskInfo>::const_iterator it = mapVDiskInfo.begin(); it != mapVDiskInfo.end(); ++it)
        {
            const STVirtualDiskInfo& stVDiskInfo = it->second;
            for (UINT j = 0; j < vctServerInfo[i].dwVDiskIPCount; ++j)
            {
                if (stVDiskInfo.dwServerIP == vctServerInfo[i].adwVDiskIP[j])
                {
                    strRegInfo += (TCHAR)stVDiskInfo.dwClientDrive; strRegInfo += TEXT(':');
                    strRegInfo += (TCHAR)stVDiskInfo.dwServerDrive; strRegInfo += TEXT(';');

                    if (CheckConnect(stVDiskInfo.dwServerIP, WORD(stVDiskInfo.dwServerPort)))
                    {
                        bSuccess = TRUE;
                        if (stVDiskInfo.dwDiskType != VDISK_TYPE_I8DESK)
                        {
                            m_pLog->WriteLog(LM_INFO, TEXT("第三方虚拟盘，无需处理(%c)"), stVDiskInfo.dwClientDrive);
                            continue;
                        }
                        if (stVDiskInfo.dwStartType != VDISK_RT_Boot)
                        {
                            m_pLog->WriteLog(LM_INFO, TEXT("刷盘时机不是开机，无需处理(%c)"), stVDiskInfo.dwClientDrive);
                            continue;
                        }
                        if (IsDriverExist((TCHAR)stVDiskInfo.dwClientDrive))
                        {
                            m_pLog->WriteLog(LM_INFO, TEXT("虚拟盘%c已经存在"), stVDiskInfo.dwClientDrive);
                            continue;
                        }
                        DWORD dwRet = clVirtualDisk.DiskMount(0, stVDiskInfo.dwServerIP, htons((WORD)stVDiskInfo.dwServerPort), (CHAR)stVDiskInfo.dwClientDrive,
                            CT2A(strVDiskTmpPath.c_str()));	
                        m_pLog->WriteLog(LM_INFO, TEXT("刷新虚拟盘%c，刷盘返回值:%lu, 服务器IP:%s, 服务器端口:%u"),
                            stVDiskInfo.dwClientDrive, dwRet, ConvertIPToString(stVDiskInfo.dwServerIP).c_str(), stVDiskInfo.dwServerPort);
                    }
                    else
                    {
                        m_pLog->WriteLog(LM_INFO, TEXT("节点服务器不可用(IP地址:%s 端口:%u 盘符%c)"),
                            ConvertIPToString(stVDiskInfo.dwServerIP).c_str(), stVDiskInfo.dwServerPort, TCHAR(stVDiskInfo.dwClientDrive));
                    }
                    break;
                }
            }
        }
        if (bSuccess)
        {
            ReportConnect(i8desk::VDiskIP, vctServerInfo[i].dwUpdateIP);
        }
        else
        {
            if (vctServerInfo[i].dwVDiskIPCount == 0)
            {
                m_pLog->WriteLog(LM_INFO, TEXT("节点服务器上没有开启虚拟盘服务(%s)"),  ConvertIPToString(vctServerInfo[i].dwUpdateIP).c_str());
            }
            else
            {
                m_pLog->WriteLog(LM_INFO, TEXT("节点服务器上没有虚拟盘(%s)"),  ConvertIPToString(vctServerInfo[i].dwUpdateIP).c_str());
            }
        }
    }

    if (!strRegInfo.empty()) 
        SHSetValue(HKEY_LOCAL_MACHINE, KEY_I8DESK, TEXT("vpartition"), REG_SZ, strRegInfo.c_str(), (strRegInfo.size()-1) * sizeof(TCHAR));
    if (bSuccess)
    {
        m_pLog->WriteLog(LM_INFO, TEXT("刷新虚拟盘完成。\r\n"));
    }
    else
    {
        m_pLog->WriteLog(LM_INFO, TEXT("刷新虚拟盘完成，没有虚拟盘可刷新。\r\n"));
    }
    return TRUE;
}

I8_NAMESPACE_END
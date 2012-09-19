#include "stdafx.h"
#include "I8ExInfoMgrS.h"
#include "LogHelper.h"
#include "XML/Xml.h"
#include "Win32/FileSystem/IniConfig.hpp"

CI8ExInfoMgrS::CI8ExInfoMgrS(i8desk::IRTDataSvr* pRTData, i8desk::ISvrPlug *pGameMgrPlug)
    : m_pRTData(pRTData)
    , m_pGameMgrPlug(pGameMgrPlug)
{
    assert(m_pGameMgrPlug != NULL);
    async::thread::AutoLock<async::thread::AutoCriticalSection> clLock(m_clCriticalSection);
    const stdex::tString sExInfoFile = utility::GetAppPath() + _T("Business\\ExInfo.dat");
    __super::Load(sExInfoFile.c_str());
    m_iGameUpdateTime = i8desk::GetIniConfig<INT64>(_T("GameExInfo"), _T("Version"), 0, GetIniConfigFile().c_str());
}

CI8ExInfoMgrS::~CI8ExInfoMgrS()
{
    i8desk::SetIniConfig<INT64>(_T("GameExInfo"), _T("Version"), m_iGameUpdateTime, GetIniConfigFile().c_str());
}

void CI8ExInfoMgrS::UpdateNewestGameVersion()
{
    __super::UpdateNewestGameVersion();
}

void CI8ExInfoMgrS::UpdateNewestGameVersion(std::map<DWORD, DWORD>& clGameNewestVersion)
{
    std::map<DWORD, DWORD> clGlobalVersion;
    {
        i8desk::GameExInfoVersionArray_st* pGameVersionInfo = reinterpret_cast<i8desk::GameExInfoVersionArray_st*>(m_pGameMgrPlug->SendMessage(i8desk::GAMEEX_VERSION, 0, 0));
        if (pGameVersionInfo == NULL)
        {
            i8desk::Log(i8desk::LM_ERROR, _T("从游戏下载管理插件中取得游戏扩展信息版本失败"));
            return;
        }
#ifdef _SHOWMORELOG
        i8desk::Log(i8desk::LM_INFO, _T("从三层下载组件取得游戏扩展信息版本%d个"), pGameVersionInfo->dwCount);
#endif
        for (DWORD i = 0; i < pGameVersionInfo->dwCount; ++i)
        {
            clGlobalVersion.insert(std::make_pair(pGameVersionInfo->astItem[i].gid, pGameVersionInfo->astItem[i].dwVersion));
        }
        CoTaskMemFree(pGameVersionInfo);
    }
    async::thread::AutoLock<async::thread::AutoCriticalSection> clLock(m_clCriticalSection);
    clGameNewestVersion.clear();
    i8desk::IGameRecordset *records = 0;
    m_pRTData->GetGameTable()->Select(&records, 0, 0);
    for(size_t i = 0; i != records->GetCount(); ++i)
    {
        const i8desk::db::tGame *game = records->GetData(i);
        if( game->Status == i8desk::StatusLocal && game->StopRun == i8desk::DelNone && game->Hide == i8desk::NoHide && game->GID > MIN_IDC_GID )
        {
            std::map<DWORD, DWORD>::const_iterator clId = clGlobalVersion.find(game->GID);
            if (clId != clGlobalVersion.end())
            {
                clGameNewestVersion.insert(std::make_pair(game->GID, clId->second));
            }
        }
    }
    records->Release();
}

void CI8ExInfoMgrS::CheckVersion(const std::tr1::function<void(long, long)>& pfnAddTask, const async::thread::AutoEvent& exit)
{
    async::thread::AutoLock<async::thread::AutoCriticalSection> clLock(m_clCriticalSection);
    __super::CheckVersion(pfnAddTask, exit);
}

BOOL CI8ExInfoMgrS::AddGame(DWORD gid, DWORD dwVersion, const char* pcData, DWORD dwSize)
{
    assert(gid != 0 && pcData != NULL);
    stdex::tString sErr;
    if (!CheckInfoEx(pcData, dwSize, sErr))
    {
        i8desk::Log(i8desk::LM_ERROR, _T("下载的游戏扩展信息不正确(%d)，%s"), gid, sErr.c_str());
        return FALSE;
    }
    async::thread::AutoLock<async::thread::AutoCriticalSection> clLock(m_clCriticalSection);
    __super::AddGame(gid, dwVersion, pcData, dwSize);
    m_iGameUpdateTime = ::time(NULL);
    return TRUE;
}
void* CI8ExInfoMgrS::GetGame(DWORD gid)
{
    async::thread::AutoLock<async::thread::AutoCriticalSection> clLock(m_clCriticalSection);
    const ExInfo_st* pstExInfo = __super::GetGame(gid);
    if (pstExInfo != NULL)
    {
        if (pstExInfo->gid != gid || pstExInfo->clInfo.size() == 0)
        {
            i8desk::Log(i8desk::LM_ERROR, _T("游戏扩展信息(%d)损坏"), gid);
            const stdex::tString sExInfoFile = utility::GetAppPath() + _T("Business\\ExInfo.dat");
            DeleteFile(sExInfoFile.c_str());
            Load(sExInfoFile.c_str());
            return NULL;
        }
        void* pOutData = CoTaskMemAlloc(sizeof(DWORD) + pstExInfo->clInfo.size());
        DWORD* pGameSize = static_cast<DWORD*>(pOutData);
        *pGameSize = pstExInfo->clInfo.size();
        void* pGame = static_cast<void*>(pGameSize + 1);
        memcpy(pGame, &pstExInfo->clInfo[0], pstExInfo->clInfo.size());
        return pOutData;
    }
    else
    {
        return NULL;
    }
}

DWORD CI8ExInfoMgrS::GetNewestGameVersion(DWORD gid)
{
    async::thread::AutoLock<async::thread::AutoCriticalSection> clLock(m_clCriticalSection);
    DWORD dwVersion = 0;
    if (__super::GetNewestGameVersion(gid, dwVersion))
    {
        return dwVersion;
    }
    else
    {
        i8desk::Log(i8desk::LM_ERROR, _T("游戏下载管理插件中缺少指定游戏的扩展信息版本(%d)"), gid);
        return 0;
    }
}

BOOL CI8ExInfoMgrS::SetClientMenuInfo(const char* pcData, DWORD dwSize)
{
    stdex::tString sErr;
    if (!CheckInfoEx(pcData, dwSize, sErr))
    {
        i8desk::Log(i8desk::LM_ERROR, _T("下载的菜单分级信息不正确，%s"), sErr.c_str());
        return FALSE;
    }
    async::thread::AutoLock<async::thread::AutoCriticalSection> clLock(m_clCriticalSection);
    __super::SetClientMenuInfo(pcData, dwSize);
    return TRUE;
}

void* CI8ExInfoMgrS::GetClientMenuInfo()
{
    async::thread::AutoLock<async::thread::AutoCriticalSection> clLock(m_clCriticalSection);
    const ExInfo_st* pstExInfo = __super::GetClientMenuInfo();
    if (pstExInfo != NULL)
    {
        void* pOutData = CoTaskMemAlloc(sizeof(DWORD) + pstExInfo->clInfo.size());
        DWORD* pExInfoSize = static_cast<DWORD*>(pOutData);
        *pExInfoSize = pstExInfo->clInfo.size();
        void* pExInfo = static_cast<void*>(pExInfoSize + 1);
        memcpy(pExInfo, &pstExInfo->clInfo[0], pstExInfo->clInfo.size());
        return pOutData;
    }
    else
    {
        return NULL;
    }
}

void* CI8ExInfoMgrS::GetGameVersion()
{
    async::thread::AutoLock<async::thread::AutoCriticalSection> clLock(m_clCriticalSection);
    void* pOutData = CoTaskMemAlloc(sizeof(DWORD) + GetGameCount() * sizeof(i8desk::GameExInfoVersion_st));
    i8desk::GameExInfoVersionArray_st* pGameVersionInfo = reinterpret_cast<i8desk::GameExInfoVersionArray_st*>(pOutData);
    pGameVersionInfo->dwCount = GetGameCount();
    __super::GetGameVersion(pGameVersionInfo->astItem, pGameVersionInfo->dwCount);
    return pOutData;
}

BOOL CI8ExInfoMgrS::CheckInfoEx(const char* pcData, DWORD dwSize, stdex::tString& sErr)
{
    if (strstr(pcData, "<Flag>000</Flag>") != NULL)
    {
        return TRUE;
    }
    stdex::tString sFlag, sErrInfo;
    GetMessageFromInfoEx(pcData, dwSize, sFlag, sErrInfo);
    sErr = _T("返回值：") + sFlag + _T(" 错误信息：") + sErrInfo;
    return FALSE;
}

void CI8ExInfoMgrS::GetMessageFromInfoEx(const char* pcData, DWORD dwSize, stdex::tString& sFlag, stdex::tString& sErrInfo)
{
    sFlag = _T("");
    sErrInfo = _T("");
    xml::Xml clXML;
    if (!clXML.LoadXml(CA2T(pcData, CP_UTF8)))
    {
        return;
    }
    xml::XmlNodePtr pFlagNode = clXML.GetRoot()->GetChild(_T("Flag"), false);
    if (!pFlagNode->IsNull())
    {
        sFlag = pFlagNode->GetValue<stdex::tString>(_T(""));
    }
    xml::XmlNodePtr pMessageNode = clXML.GetRoot()->GetChild(_T("Message"), false);
    if (!pMessageNode->IsNull())
    {
        sErrInfo = pMessageNode->GetValue<stdex::tString>(_T(""));
    }
}

stdex::tString CI8ExInfoMgrS::GetIniConfigFile() const
{
    return utility::GetAppPath() + _T("Business\\Business.ini");
}

time_t CI8ExInfoMgrS::GetGameUpdateTime() const
{
    return m_iGameUpdateTime;
}

void CI8ExInfoMgrS::WriteFile()
{
    __super::WriteFile();
}

void CI8ExInfoMgrS::ExportErrInfo(LPCTSTR lpszErrInfo)
{
    i8desk::Log(i8desk::LM_ERROR, _T("扩展信息文件不可用，将重新生成(%s)"), lpszErrInfo);
}
#include "stdafx.h"
#include "I8ExInfoMgr.h"

CI8MemoyIStream& operator>>(CI8MemoyIStream& clStream, CI8ExInfoMgr::ExInfo_st& stExInfo)
{
    int iType;
    clStream>>iType>>stExInfo.gid>>stExInfo.dwVersion;
    stExInfo.enType = static_cast<i8desk::ExInfo_Type_en>(iType);
    DWORD dwCount;
    clStream>>dwCount;
    stExInfo.clInfo.resize(dwCount);
    clStream.Read(&stExInfo.clInfo[0], stExInfo.clInfo.size());
    return clStream;
}

CI8MemoyOStream& operator<<(CI8MemoyOStream& clStream, const CI8ExInfoMgr::ExInfo_st& stExInfo)
{
    clStream<<int(stExInfo.enType)<<stExInfo.gid<<stExInfo.dwVersion;
    clStream<<DWORD(stExInfo.clInfo.size());
    clStream.Write(&stExInfo.clInfo[0], stExInfo.clInfo.size());
    return clStream;
}

CI8ExInfoMgr::CI8ExInfoMgr()
{
}

CI8ExInfoMgr::~CI8ExInfoMgr()
{
}

void CI8ExInfoMgr::Load(LPCTSTR lpszFilePath)
{
    CI8ResMgrFile<Index_st>::Result_en enResult = m_clExInfoMgr.SetFile(lpszFilePath);
    if (enResult != CI8ResMgrFile<Index_st>::E_RST_SUCESS)
    {
        stdex::tString sErrInfo;
        switch (enResult)
        {
        case CI8ResMgrFile<Index_st>::E_RST_ERR_SIGN:
            sErrInfo = _T("文件标志不正确：");
            break;
        case CI8ResMgrFile<Index_st>::E_RST_ERR_VERISION:
            sErrInfo = _T("文件版本过低：");
            break;
        case CI8ResMgrFile<Index_st>::E_RST_ERR_UPDATETIME:
            sErrInfo = _T("文件更新时间不正确：");
            break;
        case CI8ResMgrFile<Index_st>::E_RST_FAIL_ACCESS:
            sErrInfo = _T("文件无法访问或创建：");
            break;
        default:
            sErrInfo = _T("文件内容不正确：");
            break;
        }
        sErrInfo += lpszFilePath;
        ExportErrInfo(sErrInfo.c_str());
        m_clExInfoMgr.Clear();
        DeleteFile(lpszFilePath);
        enResult = m_clExInfoMgr.SetFile(lpszFilePath);
        assert(enResult == CI8ResMgrFile<Index_st>::E_RST_SUCESS);
    }
    std::vector<Index_st> clIndex;
    m_clExInfoMgr.GetAllIndex(clIndex);
    m_clGameCurVersion.clear();
    for (UINT i = 0; i < clIndex.size(); ++i)
    {
        if (clIndex[i].enType == i8desk::E_EXINFO_GAME)
        {
            m_clGameCurVersion.insert(std::make_pair(clIndex[i].gid, clIndex[i].dwVersion));
        }
    }
}

void CI8ExInfoMgr::CheckVersion(const std::tr1::function<void(long, long)>& pfnAddTask, HANDLE hEvent)
{
    for (std::map<DWORD, DWORD>::const_iterator clId = m_clGameNewestVersion.begin(); clId != m_clGameNewestVersion.end(); ++clId)
    {
        if (hEvent != NULL && WaitForSingleObject(hEvent, 0) == WAIT_OBJECT_0)
        {
            return;
        }
        std::map<DWORD, DWORD>::iterator clVersonId = m_clGameCurVersion.find(clId->first);
        if (!(clVersonId != m_clGameCurVersion.end() && clVersonId->second == clId->second))
        {
            pfnAddTask(clId->first, 0);
        }
    }
}

void CI8ExInfoMgr::AddGame(DWORD gid, DWORD dwVersion, const char* pcData, DWORD dwSize)
{
    assert(gid != 0 && pcData != NULL);
    std::map<DWORD, DWORD>::iterator clVersonId = m_clGameCurVersion.find(gid);
    if (clVersonId != m_clGameCurVersion.end())
    {
        Index_st stIndex = {i8desk::E_EXINFO_GAME, clVersonId->first, clVersonId->second};
        m_clExInfoMgr.DeleteData(stIndex);
        clVersonId->second = dwVersion;
    }
    else
    {
        m_clGameCurVersion.insert(std::make_pair(gid, dwVersion));
    }
    Index_st stIndex = {i8desk::E_EXINFO_GAME, gid, dwVersion};
    ExInfo_st stExInfo;
    stExInfo.enType = stIndex.enType;
    stExInfo.gid = stIndex.gid;
    stExInfo.dwVersion = stIndex.dwVersion;
    stExInfo.clInfo.resize(dwSize);
    memcpy(&stExInfo.clInfo[0], pcData, dwSize);
    m_clExInfoMgr.SetData(stIndex, stExInfo);
}

const CI8ExInfoMgr::ExInfo_st* CI8ExInfoMgr::GetGame(DWORD gid)
{
    assert(gid != 0);
    std::map<DWORD, DWORD>::const_iterator clVersonId = m_clGameCurVersion.find(gid);
    if (clVersonId != m_clGameCurVersion.end())
    {
        Index_st stIndex = {i8desk::E_EXINFO_GAME, clVersonId->first, clVersonId->second};
        return &m_clExInfoMgr.GetData(stIndex);
    }
    else
    {
        return NULL;
    }
}
BOOL CI8ExInfoMgr::GetNewestGameVersion(DWORD gid, DWORD& dwVersion)
{
    std::map<DWORD, DWORD>::const_iterator clVersonId = m_clGameNewestVersion.find(gid);
    if (clVersonId != m_clGameNewestVersion.end())
    {
        dwVersion = clVersonId->second;
        return TRUE;
    }
    else
    {
        UpdateNewestGameVersion();
        clVersonId = m_clGameNewestVersion.find(gid);
        if (clVersonId != m_clGameNewestVersion.end())
        {
            dwVersion = clVersonId->second;
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
}

void CI8ExInfoMgr::SetClientMenuInfo(const char* pcData, DWORD dwSize)
{
    assert(pcData != NULL && dwSize > 0);
    Index_st stIndex = {i8desk::E_EXINFO_MENU, 0, 0};
    ExInfo_st stExInfo;
    stExInfo.enType = stIndex.enType;
    stExInfo.gid = stIndex.gid;
    stExInfo.dwVersion = stIndex.dwVersion;
    stExInfo.clInfo.resize(dwSize);
    memcpy(&stExInfo.clInfo[0], pcData, dwSize);
    m_clExInfoMgr.SetData(stIndex, stExInfo);
}

const CI8ExInfoMgr::ExInfo_st* CI8ExInfoMgr::GetClientMenuInfo()
{
    Index_st stIndex = {i8desk::E_EXINFO_MENU, 0, 0};
    if (m_clExInfoMgr.IsExist(stIndex))
    {
        return &m_clExInfoMgr.GetData(stIndex);
    }
    else
    {
        return NULL;
    }
}
void CI8ExInfoMgr::GetGameVersion(i8desk::GameExInfoVersion_st* pstVersion, int iMaxCount)
{
    if (iMaxCount < int(m_clGameCurVersion.size()))
    {
        assert(0 && "用于取得游戏扩展信息版本的空间太小");
        return;
    }
    int iIndex = 0;
    for (std::map<DWORD, DWORD>::const_iterator clId = m_clGameCurVersion.begin(); clId != m_clGameCurVersion.end(); ++clId)
    {
        pstVersion[iIndex].gid = clId->first;
        pstVersion[iIndex].dwVersion = clId->second;
        ++iIndex;
    }
}

int CI8ExInfoMgr::GetGameCount()
{
    return m_clGameCurVersion.size();
}

void CI8ExInfoMgr::UpdateNewestGameVersion()
{
    UpdateNewestGameVersion(m_clGameNewestVersion);
}

void CI8ExInfoMgr::WriteFile()
{
    m_clExInfoMgr.WriteFile();
}
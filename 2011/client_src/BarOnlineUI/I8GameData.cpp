#include "StdAfx.h"
#include "I8GameData.h"
#include "Resource.h"

CI8GameData::CI8GameData(void)
{
    m_hDefaultIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
}

CI8GameData::~CI8GameData(void)
{
    if (m_hDefaultIcon != NULL)
    {
        DestroyIcon(m_hDefaultIcon);
    }
}

CI8GameData& CI8GameData::Instance()
{
    static CI8GameData g_clGameData;
    return g_clGameData;
}

void CI8GameData::Refresh()
{
    m_aGameInfo.clear();
    for (std::map<DWORD, HICON>::iterator clId = m_aIconInfo.begin(); clId != m_aIconInfo.end(); ++clId)
    {
        DestroyIcon(clId->second);
    }
    m_aIconInfo.clear();
    CI8ClientDataCoreReturnData<I8CD_GameBaseInfoArray_st, I8CD_GameBaseInfo_st> clGameInfoArray = I8CD_GetAllGameBaseInfo();
    for (int i = 0; i < clGameInfoArray.GetCount(); ++i)
    {
        m_aGameInfo[clGameInfoArray[i].gid] = clGameInfoArray[i];
    }
    CI8ClientDataCoreReturnData<I8CD_IconInfoArray_st, I8CD_IconInfo_st> clIconInfoArray = I8CD_GetAllIconInfo();
    for (int i = 0; i < clIconInfoArray.GetCount(); ++i)
    {
        m_aIconInfo[clIconInfoArray[i].gid] = LoadIconFromBuffer(static_cast<const unsigned char *>(clIconInfoArray[i].pIconData), clIconInfoArray[i].dwSize);
    }
}

I8CD_GameBaseInfo_st* CI8GameData::GetGameInfo(DWORD gid, BOOL bCheckExist)
{
    std::map<DWORD, I8CD_GameBaseInfo_st>::iterator clId = m_aGameInfo.find(gid);
    if (clId == m_aGameInfo.end())
    {
        if (bCheckExist)
        {
            assert(0 && "根据游戏ID无法找到游戏信息");
        }
        return NULL;
    }
    else
    {
        return &clId->second;
    }
    
}

void CI8GameData::GetGameInfo(std::vector<const I8CD_GameBaseInfo_st*>& clDesGameInfo, const CI8ClientDataCoreReturnData<I8CD_GameIDArray_st, DWORD>& clSrcGameInfo)
{
    clDesGameInfo.clear();
    clDesGameInfo.resize(clSrcGameInfo.GetCount());
    for (int i = 0; i < clSrcGameInfo.GetCount(); ++i)
    {
        clDesGameInfo[i] = GetGameInfo(clSrcGameInfo[i]);
    }
}

HICON CI8GameData::FindIconOfGame(DWORD gid)
{
    std::map<DWORD, HICON>::iterator clId = m_aIconInfo.find(gid);
    if (clId == m_aIconInfo.end())
    {
        return m_hDefaultIcon;
    }
    else
    {
        return clId->second;
    }
}

void CI8GameData::GetAllGameInfo(std::vector<const I8CD_GameBaseInfo_st*>& clGameInfo)
{
    clGameInfo.clear();
    clGameInfo.resize(m_aGameInfo.size());
    int iIndex = 0;
    for (std::map<DWORD, I8CD_GameBaseInfo_st>::const_iterator clId = m_aGameInfo.begin(); clId != m_aGameInfo.end(); ++clId)
    {
        clGameInfo[iIndex] = &clId->second;
        ++iIndex;
    }
}

HICON CI8GameData::LoadIconFromBuffer(const unsigned char *buf, DWORD dwSize)
{
    if(dwSize == 0 || buf == NULL)
        return NULL;

    TCHAR path[MAX_PATH]	= {0};
    HANDLE hFile			= INVALID_HANDLE_VALUE;
    DWORD dwWriteBytes		= 0;

    __try
    {
        ::GetSystemDirectory(path, MAX_PATH);
        _tcscat_s(path, _T("\\i8Icon.ico"));
        ::DeleteFile(path);

        hFile = ::CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            __leave;


        ::WriteFile(hFile, buf, dwSize, &dwWriteBytes, NULL);
        if(dwWriteBytes != dwSize)
            __leave;

    }
    __finally
    {
        if(hFile != INVALID_HANDLE_VALUE)
            ::CloseHandle(hFile);
    }

    if(hFile == INVALID_HANDLE_VALUE || dwWriteBytes != dwSize)
        return NULL;

    return (HICON)::LoadImage(NULL, path, IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
}	

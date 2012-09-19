#pragma once

#include "I8ExInfoMgr.h"
#include "MultiThread/Lock.hpp"

class CI8ExInfoMgrS : private CI8ExInfoMgr
{
private:
    i8desk::ISvrPlug* m_pGameMgrPlug;
    i8desk::IRTDataSvr* m_pRTData;
    async::thread::AutoCriticalSection m_clCriticalSection;
    time_t m_iGameUpdateTime;
public:
    CI8ExInfoMgrS(i8desk::IRTDataSvr* pRTData, i8desk::ISvrPlug *pGameMgrPlug);
    virtual ~CI8ExInfoMgrS();
    void UpdateNewestGameVersion();
    virtual void UpdateNewestGameVersion(std::map<DWORD, DWORD>& clGameNewestVersion);
    void CheckVersion(const std::tr1::function<void(long, long)>& pfnAddTask, const async::thread::AutoEvent& exit);
    BOOL AddGame(DWORD gid, DWORD dwVersion, const char* pcData, DWORD dwSize);
    void* GetGame(DWORD gid);
    DWORD GetNewestGameVersion(DWORD gid);
    BOOL SetClientMenuInfo(const char* pcData, DWORD dwSize);
    void* GetClientMenuInfo();
    void* GetGameVersion();
    time_t GetGameUpdateTime() const;
    void WriteFile();
private:
    BOOL CheckInfoEx(const char* pcData, DWORD dwSize, stdex::tString& sErr);
    void GetMessageFromInfoEx(const char* pcData, DWORD dwSize, stdex::tString& sFlag, stdex::tString& sErrInfo);
    stdex::tString GetIniConfigFile() const;
    virtual void ExportErrInfo(LPCTSTR lpszErrInfo);
};

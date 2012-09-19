#pragma once

#include "frame.h"
#include "Utility\utility.h"
using namespace i8desk;
#include "UpdateGame.h"

class CI8UpdateGameDll
{
    typedef HANDLE (__stdcall *UG_StartUpdate_pfn)(DWORD gid, LPCWSTR tid, LPCWSTR src, LPCWSTR dst, DWORD flag, 
        LPCWSTR sip, LPCWSTR cip, DWORD speed, DWORD type);
    typedef void (__stdcall *UG_StopUpdate_pfn)(HANDLE handle);
    typedef bool (__stdcall *UG_GetUpdateStatus_pfn)(HANDLE handle, tagUpdateStatus* pStatus);
    typedef bool (__stdcall *UG_SetConnectCallBack_pfn)(HANDLE handle, OnConnect_pfn pfnCallBack, void* pCallBackParam);
    typedef void (__stdcall *UG_PauseUpdate_pfn)(HANDLE handle);
    typedef void (__stdcall *UG_ResumeUpdate_pfn)(HANDLE handle);
private:
    HMODULE                   m_hModule;
    UG_StartUpdate_pfn        m_pfnStartUpdate;
    UG_StopUpdate_pfn         m_pfnStopUpdate;
    UG_GetUpdateStatus_pfn    m_pfnGetUpdateStatus;
    UG_SetConnectCallBack_pfn m_pfnSetConnectCallBack;
    UG_PauseUpdate_pfn        m_pfnPauseUpdate;
    UG_ResumeUpdate_pfn       m_pfnResumeUpdate;
public:
    CI8UpdateGameDll()
        : m_hModule(NULL)
        , m_pfnStartUpdate(NULL)
        , m_pfnStopUpdate(NULL)
        , m_pfnGetUpdateStatus(NULL)
        , m_pfnSetConnectCallBack(NULL)
        , m_pfnPauseUpdate(NULL)
        , m_pfnResumeUpdate(NULL)
    {
    }
    ~CI8UpdateGameDll()
    {
        if (m_hModule)
        {
            FreeLibrary(m_hModule);
        }
    }
    static CI8UpdateGameDll& Instance()
    {
        static CI8UpdateGameDll s_clUpdateGame;
        return s_clUpdateGame;
    }
    void Load()
    {
        if (m_hModule == NULL)
        {
            stdex::tString strUpdateDll = utility::GetAppPath() + TEXT("UpdateGame.dll");
            m_hModule = LoadLibrary(strUpdateDll.c_str());
            assert(m_hModule != NULL);
            m_pfnStartUpdate = reinterpret_cast<UG_StartUpdate_pfn>(GetProcAddress(m_hModule, reinterpret_cast<LPCSTR>(3)));
            m_pfnStopUpdate = reinterpret_cast<UG_StopUpdate_pfn>(GetProcAddress(m_hModule, reinterpret_cast<LPCSTR>(4)));
            m_pfnGetUpdateStatus = reinterpret_cast<UG_GetUpdateStatus_pfn>(GetProcAddress(m_hModule, reinterpret_cast<LPCSTR>(5)));
            m_pfnSetConnectCallBack = reinterpret_cast<UG_SetConnectCallBack_pfn>(GetProcAddress(m_hModule, reinterpret_cast<LPCSTR>(6)));
            m_pfnPauseUpdate = reinterpret_cast<UG_PauseUpdate_pfn>(GetProcAddress(m_hModule, reinterpret_cast<LPCSTR>(7)));
            m_pfnResumeUpdate = reinterpret_cast<UG_ResumeUpdate_pfn>(GetProcAddress(m_hModule, reinterpret_cast<LPCSTR>(8)));

            if (m_hModule == NULL
                || m_pfnStartUpdate == NULL
                || m_pfnStopUpdate == NULL
                || m_pfnGetUpdateStatus == NULL
                || m_pfnSetConnectCallBack == NULL)
            {
                assert(0);
            }

        }
    }
    HANDLE UG_StartUpdate(DWORD gid, LPCWSTR tid, LPCWSTR src, LPCWSTR dst, DWORD flag, 
        LPCWSTR sip, DWORD type, LPCWSTR cip = NULL, DWORD speed = 0)
    {
        Load();
        return (*m_pfnStartUpdate)(gid, tid, src, dst, flag, sip, cip, speed, type);
    }
    void UG_StopUpdate(HANDLE handle)
    {
        Load();
        (*m_pfnStopUpdate)(handle);
    }
    bool UG_GetUpdateStatus(HANDLE handle, tagUpdateStatus* pStatus)
    {
        Load();
        return (*m_pfnGetUpdateStatus)(handle, pStatus);
    }
    bool UG_SetConnectCallBack(HANDLE handle, OnConnect_pfn pfnCallBack, void* pCallBackParam)
    {
        Load();
        return (*m_pfnSetConnectCallBack)(handle, pfnCallBack, pCallBackParam);
    }
    void UG_PauseUpdate(HANDLE handle)
    {
        Load();
        assert(m_pfnPauseUpdate != NULL);
        (*m_pfnPauseUpdate)(handle);
    }
    void UG_ResumeUpdate(HANDLE handle)
    {
        Load();
        assert(m_pfnResumeUpdate != NULL);
        (*m_pfnResumeUpdate)(handle);
    }
};

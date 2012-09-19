// StartupClient.cpp : CStartupClient 的实现

#include "stdafx.h"
#include "StartupClient.h"
#include <assert.h>
#include <atlstr.h>
#include "../Include/I8ClientPipe.h"
#include "Log.h"

static CLogger s_clLog;

static LPCTSTR C_CLIENT_COMMUNICATION_MUTEX_NAME = _T("__i8_client_communication_mutex_"); //客户端组件通讯互斥体名称

#define KEY_I8DESK TEXT("SOFTWARE\\Goyoo\\i8desk")

template<int iCount>
BOOL GetBarOnlinePath(TCHAR(&atFilePath)[iCount])
{
    DWORD dwSize = iCount, dwType = REG_SZ;
    DWORD dwRet = SHGetValue(HKEY_LOCAL_MACHINE, KEY_I8DESK, TEXT("exec"), &dwType, atFilePath, &dwSize);
    return (dwRet == ERROR_SUCCESS) && PathFileExists(atFilePath);
}


STDMETHODIMP CStartupClient::RunGame(LONG gid, VARIANT_BOOL bRunBackstage, VARIANT_BOOL* bResult)
{
    s_clLog.WriteLog2(bRunBackstage ? _T("隐式运行游戏，超时时间%d") :  _T("运行游戏，超时时间%d"), m_iTimeOut);
    *bResult = VARIANT_FALSE;
    if (gid <= 0)
    {
        return S_OK;
    }
    async::thread::AutoMutex clMutex(NULL, FALSE, C_CLIENT_COMMUNICATION_MUTEX_NAME);
    async::thread::AutoLock<async::thread::AutoMutex> clLock(clMutex);
    if (!I8_ClientDataCore_ns::IsBarOnlineExist())
    {
        TCHAR atBarOnlinePath[MAX_PATH], atParam[MAX_PATH] = {0};
        if (bRunBackstage)
        {
            lstrcpy(atParam, _T("/hide"));
        }
        if (!GetBarOnlinePath(atBarOnlinePath))
        {
            s_clLog.WriteLog(_T("失败：客户端没有正确安装"));
            return S_OK;
        }
        ShellExecute(NULL, _T("open"), atBarOnlinePath, atParam, NULL, SW_SHOW);
        s_clLog.WriteLog(bRunBackstage ? _T("隐式启动客户端") :  _T("启动客户端"));
    }
    I8_ClientDataCore_ns::RunGameParam_st stParam = {gid, bRunBackstage};
    DWORD dwResult;
    I8_ClientDataCore_ns::PipeMsgResult_en enResult = I8_ClientDataCore_ns::SendPipeMessageToBarOnline(I8_ClientDataCore_ns::E_PIPEMSG_RUN_GAME, &stParam, &dwResult, m_iTimeOut);
    if (enResult == I8_ClientDataCore_ns::E_PMR_SUCCESS)
    {
        if (dwResult)
        {
            *bResult = VARIANT_TRUE;
            s_clLog.WriteLog(_T("成功：发送运行游戏命令成功"));
        }
        else
        {
            s_clLog.WriteLog(_T("失败：游戏不存在/客户端很忙"));
        }
    }
    else
    {
        switch (enResult)
        {
        case I8_ClientDataCore_ns::E_PMR_FAIL_TO_WAIT_PIPE:
            s_clLog.WriteLog(_T("失败：连接客户端不成功"));
            break;
        case I8_ClientDataCore_ns::E_PMR_FAIL_TO_NO_RESPONSE:
            s_clLog.WriteLog(_T("失败：客户端没有回应"));
            break;
        default:
            s_clLog.WriteLog(_T("失败：未知原因"));
            break;
        }
    }
    return S_OK;
}

STDMETHODIMP CStartupClient::RunClient(VARIANT_BOOL* bResult)
{
    s_clLog.WriteLog2(_T("运行/显示客户端，超时时间%d"), m_iTimeOut);
    *bResult = VARIANT_FALSE;
    async::thread::AutoMutex clMutex(NULL, FALSE, C_CLIENT_COMMUNICATION_MUTEX_NAME);
    async::thread::AutoLock<async::thread::AutoMutex> clLock(clMutex);
    if (!I8_ClientDataCore_ns::IsBarOnlineExist())
    {
        TCHAR atBarOnlinePath[MAX_PATH];
        if (!GetBarOnlinePath(atBarOnlinePath))
        {
            s_clLog.WriteLog(_T("失败：客户端没有正确安装"));
            return S_FALSE;
        }
        ShellExecute(NULL, _T("open"), atBarOnlinePath, NULL, NULL, SW_SHOW);
        *bResult = VARIANT_TRUE;
        s_clLog.WriteLog(_T("成功：启动客户端"));
    }
    else
    {
        DWORD dwResult;
        I8_ClientDataCore_ns::PipeMsgResult_en enResult = I8_ClientDataCore_ns::SendPipeMessageToBarOnline<DWORD, DWORD>(I8_ClientDataCore_ns::E_PIPEMSG_RUN_CLIENT, NULL, &dwResult, m_iTimeOut);
        if (enResult == I8_ClientDataCore_ns::E_PMR_SUCCESS)
        {
            if (dwResult)
            {
                *bResult = VARIANT_TRUE;
                s_clLog.WriteLog(_T("成功：向客户端发送显示命令"));
            }
            else
            {
                s_clLog.WriteLog(_T("失败：客户端很忙，拒绝显示命令"));
            }
        }
        else
        {
            switch (enResult)
            {
            case I8_ClientDataCore_ns::E_PMR_FAIL_TO_WAIT_PIPE:
                s_clLog.WriteLog(_T("失败：连接客户端不成功"));
                break;
            case I8_ClientDataCore_ns::E_PMR_FAIL_TO_NO_RESPONSE:
                s_clLog.WriteLog(_T("失败：客户端没有回应"));
                break;
            default:
                s_clLog.WriteLog(_T("失败：未知原因"));
                break;
            }
        }
    }
    return S_OK;
}


STDMETHODIMP CStartupClient::SetTimeOut(LONG iTimeOut, VARIANT_BOOL* bResult)
{
    *bResult = VARIANT_FALSE;
    if (iTimeOut <= 0)
    {
        s_clLog.WriteLog2(_T("设置超时时间失败%d"), iTimeOut);
        return S_OK;
    }
    m_iTimeOut = iTimeOut;
    s_clLog.WriteLog2(_T("设置超时时间成功%d"), iTimeOut);
    *bResult = VARIANT_TRUE;
    return S_OK;
}

STDMETHODIMP CStartupClient::IsGameExist(LONG gid, VARIANT_BOOL* bResult)
{
    s_clLog.WriteLog2(_T("检查游戏是否存在，超时时间%d"), m_iTimeOut);
    *bResult = VARIANT_FALSE;
    if (gid <= 0)
    {
        s_clLog.WriteLog(_T("失败：gid为空"));
        return S_OK;
    }
    async::thread::AutoMutex clMutex(NULL, FALSE, C_CLIENT_COMMUNICATION_MUTEX_NAME);
    async::thread::AutoLock<async::thread::AutoMutex> clLock(clMutex);
    if (!I8_ClientDataCore_ns::IsBarOnlineExist())
    {
        TCHAR atBarOnlinePath[MAX_PATH];
        if (!GetBarOnlinePath(atBarOnlinePath))
        {
            s_clLog.WriteLog(_T("失败：客户端没有正确安装"));
            return S_OK;
        }
        ShellExecute(NULL, _T("open"), atBarOnlinePath, _T("/hide"), NULL, SW_SHOW);
        s_clLog.WriteLog(_T("隐式启动客户端"));
    }
    DWORD dwParam = gid;
    DWORD dwResult;
    I8_ClientDataCore_ns::PipeMsgResult_en enResult = I8_ClientDataCore_ns::SendPipeMessageToBarOnline(I8_ClientDataCore_ns::E_PIPEMSG_FIND_GAME, &dwParam, &dwResult, m_iTimeOut);
    if (enResult == I8_ClientDataCore_ns::E_PMR_SUCCESS)
    {
        if (dwResult)
        {
            *bResult = VARIANT_TRUE;
            s_clLog.WriteLog(_T("成功：游戏存在"));
        }
        else
        {
            s_clLog.WriteLog(_T("成功：游戏不存在"));
        }
    }
    else
    {
        switch (enResult)
        {
        case I8_ClientDataCore_ns::E_PMR_FAIL_TO_WAIT_PIPE:
            s_clLog.WriteLog(_T("失败：启动客户端失败/连接客户端不成功"));
            break;
        case I8_ClientDataCore_ns::E_PMR_FAIL_TO_NO_RESPONSE:
            s_clLog.WriteLog(_T("失败：客户端没有回应"));
            break;
        default:
            s_clLog.WriteLog(_T("失败：未知原因"));
            break;
        }
    }
    return S_OK;
}

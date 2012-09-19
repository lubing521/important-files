#pragma once

#include <functional>
#include <memory>
#include "Extend STL/UnicodeStl.h"

class CI8NamedPipe
{
    struct Response_st
    {
        CHAR acSign[4];
        Response_st()
        {
            const char C_RESPONSE_SIGN[] = {'R', 'E', 'S', 'P'};
            memcpy(acSign, C_RESPONSE_SIGN, sizeof(acSign));
        }
        void Clear()
        {
            memset(acSign, 0, sizeof(acSign));
        }
        BOOL IsValid() const
        {
            const char C_RESPONSE_SIGN[] = {'R', 'E', 'S', 'P'};
            return (memcmp(acSign, C_RESPONSE_SIGN, sizeof(C_RESPONSE_SIGN)) == 0);
        }
    };
private:
    HANDLE m_hReadEvent;
protected:
    HANDLE m_hPipe;
public:
    CI8NamedPipe() : m_hPipe(NULL), m_hReadEvent(NULL)
    {
    }
    virtual ~CI8NamedPipe()
    {
        Close();
    }
    virtual void Close()
    {
        if (m_hReadEvent)
        {
            CloseHandle(m_hReadEvent);
            m_hReadEvent = NULL;
        }
        if (m_hPipe)
        {
            CloseHandle(m_hPipe);
            m_hPipe = NULL;
        }
    }
    virtual void CloseConnect() = 0;
    HANDLE GetHandle()
    {
        return m_hPipe;
    }

    void ReadData(void* pData, int iSize)
    {
        assert(m_hPipe != NULL && "命名管道还没有创建");
        assert(pData != NULL && iSize > 0 && "参数不正确");
        int iFinishSize = 0;
        while (iFinishSize < iSize)
        {
            DWORD dwReadSize = 0;
            if(ReadFile(m_hPipe, pData, iSize, &dwReadSize, NULL))
            {
                iFinishSize += dwReadSize;
            }
            else
            {
                assert(0 && "命名管道读数据失败");
            }
        }
    }
    BOOL ReadData(void* pData, int iSize, int iWaitTime)
    {
        assert(m_hPipe != NULL && "命名管道还没有创建");
        assert(pData != NULL && iSize > 0 && "参数不正确");
        assert(iWaitTime >= 0 && "等待时间不能为负");
        if (m_hReadEvent == NULL)
        {
            m_hReadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
            assert(m_hReadEvent != NULL && "创建事件失败");
        }
        else
        {
            ResetEvent(m_hReadEvent);
        }
        OVERLAPPED stOverlapped;
        ZeroMemory(&stOverlapped, sizeof(stOverlapped));
        stOverlapped.hEvent = m_hReadEvent;
        BOOL bSuccess = FALSE;
        DWORD dwReadSize = 0;
        if (ReadFile(m_hPipe, pData, iSize, &dwReadSize, &stOverlapped))
        {
            bSuccess = TRUE;
        }
        else
        {
            DWORD dwResult = WaitForSingleObject(m_hReadEvent, iWaitTime);
            if (dwResult == WAIT_OBJECT_0)
            {
                bSuccess = TRUE;
                dwReadSize = stOverlapped.InternalHigh;
            }
        }
        if (bSuccess)
        {
            if (dwReadSize != iSize)
            {
                assert(0 && "命名管道取得的数据大小和预想不一致");
                CloseConnect();
                return FALSE;
            }
            return TRUE;
        }
        else
        {
            CloseConnect();
            return FALSE;
        }
    }
    BOOL WriteData(const void* pData, int iSize)
    {
        assert(m_hPipe != NULL && "命名管道还没有打开");
        assert(((pData != NULL && iSize > 0) || iSize == 0) && "参数不正确");
        int iFinishSize = 0;
        while (iFinishSize < iSize)
        {
            DWORD dwWriteSize = 0;
            if(WriteFile(m_hPipe, pData, iSize, &dwWriteSize, NULL))
            {
                iFinishSize += dwWriteSize;
            }
            else
            {
                assert(ERROR_NO_DATA == GetLastError() && "命名管道写数据失败");
                return FALSE;
            }
        }
        return TRUE;
    }
    BOOL IsNull()
    {
        return m_hPipe == NULL;
    }
    BOOL ReceiveResponse(int iWaitTime)
    {
        Response_st stResponse;
        stResponse.Clear();
        if (ReadData(&stResponse, sizeof(stResponse), iWaitTime))
        {
            if (stResponse.IsValid())
            {
                return TRUE;
            }
            else
            {
                assert(0 && "错误的消息回应");
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }
    }
    void SendResponse()
    {
        Response_st stResponse;
        WriteData(&stResponse, sizeof(stResponse));
    }
};

class CI8NamedPipeServer : public CI8NamedPipe
{
    static const int C_PIPE_CACHE_SIZE = 1024;
    static const int C_SERVER_LISTEN_WAIT_TIME = 2000;
    HANDLE m_hConnectEvent;
    HANDLE m_hExitEvent;
    HANDLE m_hListenThread;
    OVERLAPPED m_stConnectOverlapped;
    std::tr1::function<void(CI8NamedPipeServer& clPipe)> m_pfnReceiveClientData;
public:
    CI8NamedPipeServer() : m_hConnectEvent(NULL), m_hExitEvent(NULL), m_hListenThread(NULL)
    {
    }
    virtual ~CI8NamedPipeServer()
    {
        Close();
    }
    BOOL CreatePipe(const stdex::tString& sPipeName)
    {
        assert(sPipeName.size() > 0 && "命名管道的名称不能为空");
        assert(m_hPipe == NULL && "命名管道已经创建");
        stdex::tString sCompletePipeName = stdex::tString(_T("\\\\.\\pipe\\")) + sPipeName;
        m_hPipe = CreateNamedPipe(sCompletePipeName.c_str(), PIPE_ACCESS_DUPLEX | FILE_FLAG_FIRST_PIPE_INSTANCE | FILE_FLAG_OVERLAPPED, PIPE_TYPE_MESSAGE |  PIPE_READMODE_MESSAGE, 1, C_PIPE_CACHE_SIZE, C_PIPE_CACHE_SIZE, 0, NULL);
        if (INVALID_HANDLE_VALUE == m_hPipe)
        {
            m_hPipe = NULL;
            return FALSE;
        }
        else
        {
            CreatePipeEvent();
            return TRUE;
        }
    }
    void ListenWithAutoNotify(const std::tr1::function<void(CI8NamedPipeServer& clPipe)>& pfnReceiveClientData)
    {
        assert(m_hPipe != NULL && "命名管道还没有创建");
        m_pfnReceiveClientData = pfnReceiveClientData;
        if (m_hExitEvent == NULL)
        {
            m_hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        }
        else
        {
            ResetEvent(m_hExitEvent);
        }
        WaitConnect();
        m_hListenThread = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, ReceiveDataThread, this, 0, NULL));
    }
    void Listen()
    {
        assert(m_hPipe != NULL && "命名管道还没有创建");
        assert(m_hListenThread == NULL && "命名管道还没有创建");
        m_pfnReceiveClientData = NULL;
        WaitConnect();
        m_hListenThread = NULL;
    }
    HANDLE GetConnectEvent()
    {
        assert(m_hConnectEvent != NULL && "命名管道还没有创建");
        return m_hConnectEvent;
    }
    virtual void CloseConnect()
    {
        if (m_hPipe)
        {
            DisconnectNamedPipe(m_hPipe);
        }
    }
    virtual void Close()
    {
        if (m_hListenThread)
        {
            if (GetCurrentThread() == m_hListenThread)
            {
                assert(0 && "不能在监视线程中关闭监视线程");
            }
            SetEvent(m_hExitEvent);
            WaitForSingleObject(m_hListenThread, INFINITE);
            CloseHandle(m_hListenThread);
            m_hListenThread = NULL;
        }
        if (m_hConnectEvent)
        {
            CloseHandle(m_hConnectEvent);
            m_hConnectEvent = NULL;
        }
        if (m_hExitEvent != NULL)
        {
            CloseHandle(m_hExitEvent);
            m_hExitEvent = NULL;
        }
        m_pfnReceiveClientData = NULL;
        __super::Close();
    }
protected:
    void CreatePipeEvent()
    {
        assert(m_hConnectEvent == NULL && "管道连接事件已经创建");
        m_hConnectEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        assert(m_hConnectEvent != NULL && "创建管道连接事件失败");
    }
    void WaitConnect()
    {
        assert(m_hPipe != NULL && "命名管道还没有创建");
        assert(m_hConnectEvent != NULL && "没有创建管道连接事件");
        ResetEvent(m_hConnectEvent);
        ZeroMemory(&m_stConnectOverlapped, sizeof(m_stConnectOverlapped));
        m_stConnectOverlapped.hEvent = m_hConnectEvent;
        if (!ConnectNamedPipe(m_hPipe, &m_stConnectOverlapped))
        {
            DWORD dwError = GetLastError();
            switch (dwError)
            {
            case ERROR_IO_PENDING:
                break;
            case ERROR_PIPE_LISTENING:
                assert(0 && "????");
                break;
            case ERROR_PIPE_CONNECTED:
                assert(0 && "管道已连接");
                break;
            case ERROR_NO_DATA:
                assert(0 && "客户端已断开");
                break;
            default:
                assert(0 && "连接客户端出现异常");
                break;
            }
        }
        else
        {
            assert(0 && "在这里管道不可能会连接上");
        }
    }
    static UINT __stdcall ReceiveDataThread(void* pParam)
    {
        CI8NamedPipeServer* pPipe = reinterpret_cast<CI8NamedPipeServer*>(pParam);
        assert(pPipe != NULL);
        assert(pPipe->m_hPipe != NULL && "命名管道还没有创建");
        BOOL bNeedExit = FALSE;
        while (!bNeedExit)
        {
            assert(pPipe->m_hExitEvent != NULL);
            HANDLE ahEvent[] = {pPipe->m_hConnectEvent, pPipe->m_hExitEvent};
            DWORD dwResult = WaitForMultipleObjects(_countof(ahEvent), ahEvent, FALSE, INFINITE);
            switch (dwResult)
            {
            case WAIT_OBJECT_0:
                if (pPipe->m_pfnReceiveClientData != NULL)
                {
                    pPipe->m_pfnReceiveClientData(*pPipe);
                }
                if (!DisconnectNamedPipe(pPipe->m_hPipe))
                {
                    assert(0 && "中止客户端连接失败");
                }
                pPipe->WaitConnect();
                break;
            case WAIT_OBJECT_0 + 1:
                bNeedExit = TRUE;
                break;
            case WAIT_TIMEOUT:
                break;
            default:
                assert(0 && "管理监听线程发生异常");
                break;
            }
        }
        return 0;
    }
};

class CI8NamedPipeClient : public CI8NamedPipe
{
public:
    CI8NamedPipeClient()
    {
    }
    virtual ~CI8NamedPipeClient()
    {
    }
    BOOL OpenPipe(const stdex::tString& sPipeName, int iConnectWaitTime = -1)
    {
        assert(sPipeName.size() > 0 && "命名管道的名称不能为空");
        assert(m_hPipe == NULL && "命名管道已经创建");
        stdex::tString sCompletePipeName = stdex::tString(_T("\\\\.\\pipe\\")) + sPipeName;
        DWORD dwBeginTime = GetTickCount();
        m_hPipe = INVALID_HANDLE_VALUE;
        do
        {
            if (WaitNamedPipe(sCompletePipeName.c_str(), 0))
            {
                m_hPipe = CreateFile(sCompletePipeName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            }
            if (m_hPipe == INVALID_HANDLE_VALUE)
            {
                Sleep(100);
            }
        }while (m_hPipe == INVALID_HANDLE_VALUE && (iConnectWaitTime == -1 || (GetTickCount() - dwBeginTime <= DWORD(iConnectWaitTime))));
        if (m_hPipe == INVALID_HANDLE_VALUE)
        {
            m_hPipe = NULL;
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }
    void CloseConnect()
    {
        Close();
    }
};
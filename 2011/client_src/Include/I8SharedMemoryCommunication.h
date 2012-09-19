#pragma once

template <class TYPE>
class CI8SharedMemoryCommunication
{
private:
    static const int C_WAIT_TIME = 2000;
    HANDLE m_hFileMap;
    HANDLE m_hRequestEvent;
    HANDLE m_hReplyEvent;
    TYPE* m_pSharedData;
public:
    CI8SharedMemoryCommunication() : m_hFileMap(NULL), m_hRequestEvent(NULL), m_hReplyEvent(NULL), m_pSharedData(NULL)
    {
    }
    ~CI8SharedMemoryCommunication()
    {
        Close();
    }
    BOOL IsNull() const
    {
        return m_hFileMap == NULL;
    }
    void Close()
    {
        if (m_pSharedData)
        {
            UnmapViewOfFile(m_pSharedData);
            m_pSharedData = NULL;
        }
        if (m_hRequestEvent)
        {
            CloseHandle(m_hRequestEvent);
            m_hRequestEvent = NULL;
        }
        if (m_hReplyEvent)
        {
            CloseHandle(m_hReplyEvent);
            m_hReplyEvent = NULL;
        }
        if (m_hFileMap)
        {
            CloseHandle(m_hFileMap);
            m_hFileMap = NULL;
        }
    }
    BOOL Create(LPCTSTR pName)
    {
        assert(m_hFileMap == NULL);
        assert(pName != NULL && lstrlen(pName) > 0);
        CString sWholeName = CString(_T("Shared_Memory_Communication_")) + pName;
        m_hFileMap = CreateFileMapping(NULL, NULL, PAGE_READWRITE, 0, sizeof(TYPE), sWholeName);
        if (m_hFileMap == NULL)
        {
            return FALSE;
        }
        m_pSharedData = reinterpret_cast<TYPE*>(MapViewOfFile(m_hFileMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, sizeof(TYPE)));
        if (m_pSharedData == NULL)
        {
            assert(0 && "取得共享内存地址失败");
            CloseHandle(m_hFileMap);
            m_hFileMap =NULL;
            return FALSE;
        }
        CreateCommunicationEvent(pName);
        return TRUE;
    }
    BOOL Open(LPCTSTR pName)
    {
        assert(m_hFileMap == NULL);
        assert(pName != NULL && lstrlen(pName) > 0);
        CString sWholeName = CString(_T("Shared_Memory_Communication_")) + pName;
        m_hFileMap = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, sWholeName);
        if (m_hFileMap == NULL)
        {
            return FALSE;
        }
        m_pSharedData = reinterpret_cast<TYPE*>(MapViewOfFile(m_hFileMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, sizeof(TYPE)));
        if (m_pSharedData == NULL)
        {
            assert(0 && "取得请求扩展信息的内存地址失败");
            CloseHandle(m_hFileMap);
            m_hFileMap =NULL;
            return FALSE;
        }
        CreateCommunicationEvent(pName);
        return TRUE;
    }
    BOOL Request(LPCTSTR pName, const TYPE& stData)
    {
        if (m_hFileMap == NULL)
        {
            if (!Open(pName))
            {
                return FALSE;
            }
        }
        *m_pSharedData = stData;
        ResetEvent(m_hReplyEvent);
        SetEvent(m_hRequestEvent);
        if (WaitForSingleObject(m_hReplyEvent, C_WAIT_TIME) == WAIT_OBJECT_0)
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    BOOL Request()
    {
        assert(m_hFileMap != NULL);
        ResetEvent(m_hReplyEvent);
        SetEvent(m_hRequestEvent);
        if (WaitForSingleObject(m_hReplyEvent, C_WAIT_TIME) == WAIT_OBJECT_0)
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    HANDLE GetRequestEvent() const
    {
        assert(m_hFileMap != NULL);
        return m_hRequestEvent;
    }
    TYPE& GetData()
    {
        assert(m_hFileMap != NULL);
        return *m_pSharedData;
    }
    const TYPE& GetData() const
    {
        assert(m_hFileMap != NULL);
        return *m_pSharedData;
    }
    void SetData(const TYPE& Data)
    {
        assert(m_hFileMap != NULL);
        *m_pSharedData = Data;
    }
    void Reply()
    {
        assert(m_hFileMap != NULL);
        ResetEvent(m_hRequestEvent);
        SetEvent(m_hReplyEvent);
    }
private:
    void CreateCommunicationEvent(LPCTSTR pName)
    {
        CString sRequestEventName = CString(_T("Shared_Memory_Communication_RequestEvent_")) + pName;
        assert(m_hRequestEvent == NULL && "共享内存通讯请求事件已经创建");
        assert(m_hReplyEvent == NULL && "共享内存通讯应答事件已经创建");
        m_hRequestEvent = CreateEvent(NULL, TRUE, FALSE, sRequestEventName);
        assert(m_hRequestEvent != NULL && "创建共享内存通讯请求事件失败");
        CString sReplyEventName = CString(_T("Shared_Memory_Communication_ReplyEvent_")) + pName;
        m_hReplyEvent = CreateEvent(NULL, TRUE, FALSE, sReplyEventName);
        assert(m_hReplyEvent != NULL && "创建共享内存通讯应答事件失败");
    }
};

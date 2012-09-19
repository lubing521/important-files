#pragma once

class CVirtualDiskDll
{
    typedef DWORD (WINAPI *PFNDISKMOUNT)(UINT index, ULONG addr,USHORT port,CHAR driver,LPCSTR tempfile);
    typedef DWORD (WINAPI *PFNDISKUMOUNT)(char driver);
    typedef DWORD (WINAPI *PFNUPDATEEXEC)(LPCSTR ChannelID);
private:
    HMODULE       m_hModule;
    PFNDISKMOUNT  m_pfnDiskMount;
    PFNDISKUMOUNT m_pfnDiskUmount;
    PFNUPDATEEXEC m_pfnUpdateExec;
public:
    CVirtualDiskDll()
        : m_hModule(NULL)
        , m_pfnDiskMount(NULL)
        , m_pfnDiskUmount(NULL)
        , m_pfnUpdateExec(NULL)
    {
    }
    ~CVirtualDiskDll()
    {
        if (m_hModule)
        {
            FreeLibrary(m_hModule);
        }
    }
    BOOL Load(LPCTSTR pDllFile)
    {
        assert(m_hModule == NULL);
        assert(pDllFile != NULL);
        m_hModule = LoadLibrary(pDllFile);
        assert(m_hModule != NULL);
        m_pfnDiskMount = reinterpret_cast<PFNDISKMOUNT>(GetProcAddress(m_hModule, "iDiskMount"));
        m_pfnDiskUmount = reinterpret_cast<PFNDISKUMOUNT>(GetProcAddress(m_hModule, "iDiskUmount"));
        m_pfnUpdateExec = reinterpret_cast<PFNUPDATEEXEC>(GetProcAddress(m_hModule, "UpdateExec")); //允许没有该函数
        
        return m_hModule != NULL;
    }
    DWORD DiskMount(UINT index, ULONG addr,USHORT port,CHAR driver,LPCSTR tempfile)
    {
        assert(m_hModule != NULL && "没有加载虚拟盘动态库。");
        assert(m_pfnDiskMount != NULL && "该虚拟盘动态库不支持DiskMount方法。。");
        return (*m_pfnDiskMount)(index, addr, port, driver, tempfile);
    }
    DWORD DiskUmount(char driver)
    {
        assert(m_hModule != NULL && "没有加载虚拟盘动态库。");
        assert(m_pfnDiskUmount != NULL && "该虚拟盘动态库不支持DiskUmount方法。。");
        return (*m_pfnDiskUmount)(driver);
    }
    DWORD UpdateExec(LPCSTR ChannelID)
    {
        assert(m_hModule != NULL&& "没有加载虚拟盘动态库。");
        assert(m_pfnUpdateExec != NULL && "该虚拟盘动态库不支持UpdateExec方法。");
        return (*m_pfnUpdateExec)(ChannelID);
    }
    BOOL IsCanUpdateExec()
    {
        return m_pfnUpdateExec != NULL;
    }
};

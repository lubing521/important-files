#pragma once

#include <set>
#include <map>
#include <vector>
#include <dbghelp.h>
#include <atlbase.h>
#include <algorithm>
#pragma comment(lib, "Dbghelp.lib")
#include "I8SpareMgr.h"
#include "Utility/SmartPtr.hpp"
#include "Utility/utility.h"

template<class INDEX, class SPAREMGR = CI8SpareMgr>
class CI8ResMgrFile
{
public:
    enum Result_en
    {
        E_RST_SUCESS = 0,     //成功
        E_RST_FAIL_ACCESS,    //文件无法访问或创建
        E_RST_ERR_SIGN,       //错误的文件标志
        E_RST_ERR_VERISION ,  //错误的文件版本
        E_RST_ERR_UPDATETIME, //错误的更新时间
        E_RST_ERR_UNKNOWN,    //未知错误
    };
private:
    static const DWORD C_CURRENT_VERSION = 1;
    struct Index_st
    {
        INT64 lAddress;
        DWORD dwSize;
        DWORD dwUsedSize;
        Index_st() : lAddress(0), dwSize(0), dwUsedSize(0)
        {
        }
        Index_st(INT64 iNewAddress, DWORD dwNewSize, DWORD dwNewUsedSize = 0) : lAddress(iNewAddress), dwSize(dwNewSize), dwUsedSize(dwNewUsedSize)
        {
        }
    };
    struct DataIndex_st
    {
        INDEX Index;
        Index_st stIndex;
    };
    struct Head_st
    {
        CHAR     acSign[4];
        DWORD    dwVersion;
        INT64    iIndexAddress;
        DWORD    dwIndexSize;
        DWORD    dwUsedCount;
        DWORD    dwSpareInfoSize;
        FILETIME stUpdateTime;
        DWORD    dwReserve;
    };
private:
    std::map<INDEX, Index_st> m_clIndex;
    SPAREMGR m_SpareMgr;
    Index_st m_stHeadIndex;
    HANDLE m_hFile;
    BOOL m_bIndexChange;
    BOOL m_bHeadChange;
public:
    CI8ResMgrFile(void)
        : m_hFile(NULL)
        , m_bIndexChange(FALSE)
        , m_bHeadChange(FALSE)
    {
    }
    ~CI8ResMgrFile(void)
    {
        Close();
    }
    Result_en Open(LPCTSTR lpszFile)
    {
        if (!IsNull())
        {
            Close();
        }
        MakeSureDirectoryPathExists(CT2A(lpszFile));
        m_hFile = CreateFile(lpszFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
        if (m_hFile == INVALID_HANDLE_VALUE)
        {
            assert(0 && "打开或创建资源文件失败");
            m_hFile = NULL;
            return E_RST_FAIL_ACCESS;
        }
        m_SpareMgr.MarkUsed(0, sizeof(Head_st));
        LARGE_INTEGER lFileSize;
        if (!GetFileSizeEx(m_hFile, &lFileSize))
        {
            assert(0 && "取得资源文件大小失败");
            Close();
            return E_RST_ERR_UNKNOWN;
        }
        if (lFileSize.QuadPart > 0)
        {
            Result_en enResult = ReadIndex();
            if (enResult != E_RST_SUCESS)
            {
                Close();
                return enResult;
            }
        }
        return E_RST_SUCESS;
    }
    void Close()
    {
        if (m_hFile != NULL)
        {
            if (m_bIndexChange)
            {
                WriteIndex();
            }
            if (m_bHeadChange)
            {
                WriteHead();
            }
            CloseHandle(m_hFile);
            m_hFile = NULL;
            m_clIndex.clear();
            m_SpareMgr.Clear();
            m_stHeadIndex = Index_st();
        }
    }
    void WriteIndex()
    {
        assert(m_hFile != NULL);
        DWORD dwIndexSize = sizeof(DataIndex_st) * m_clIndex.size() + m_SpareMgr.GetStorageSize();
        DWORD dwOldIndexSize = m_stHeadIndex.dwSize;
        if (m_stHeadIndex.dwSize != 0 && m_stHeadIndex.dwSize < dwIndexSize)
        {
            FreeIndex(m_stHeadIndex);
            m_stHeadIndex.lAddress = 0;
            m_stHeadIndex.dwUsedSize = 0;
            m_stHeadIndex.dwSize = 0;
        }
        if (m_stHeadIndex.dwSize == 0)
        {
            DWORD dwAllocIndexSize;
            if (dwOldIndexSize == 0)
            {
                dwAllocIndexSize = max(dwIndexSize, 1024);
            }
            else
            {
                dwAllocIndexSize = dwOldIndexSize * 2;
                while (dwAllocIndexSize < dwIndexSize)
                {
                    dwAllocIndexSize *= 2;
                }
            }
            m_stHeadIndex = AllocIndex(dwAllocIndexSize);
        }
        if (m_clIndex.size() > 0)
        {
            utility::CAutoArrayPtr<DataIndex_st>::AutoPtr pIndexData(new DataIndex_st[m_clIndex.size()]);
            DWORD dwIndex = 0;
            for (std::map<INDEX, Index_st>::const_iterator clId = m_clIndex.begin(); clId != m_clIndex.end(); ++clId)
            {
                pIndexData.Get()[dwIndex].Index = clId->first;
                pIndexData.Get()[dwIndex].stIndex = clId->second;
                ++dwIndex;
            }
            Write(m_stHeadIndex.lAddress, pIndexData.Get(), sizeof(DataIndex_st) * m_clIndex.size());
            m_bHeadChange = TRUE;
        }
        if (m_SpareMgr.GetStorageSize() > 0)
        {
            DWORD dwSpareInfoSize = m_SpareMgr.GetStorageSize();
            utility::CAutoArrayPtr<char>::AutoPtr pSpareData(new char[dwSpareInfoSize]);
            m_SpareMgr>>std::make_pair(pSpareData.Get(), dwSpareInfoSize);
            Write(m_stHeadIndex.lAddress + sizeof(DataIndex_st) * m_clIndex.size(), pSpareData.Get(), dwSpareInfoSize);
            m_bHeadChange = TRUE;
        }
        m_bIndexChange = FALSE;
        if (GetFileSize() > m_SpareMgr.GetSize())
        {
            LARGE_INTEGER lFileOffset;
            lFileOffset.QuadPart = m_SpareMgr.GetSize();
            if (SetFilePointerEx(m_hFile, lFileOffset, NULL, FILE_BEGIN))
            {
                if (!SetEndOfFile(m_hFile))
                {
                    assert(0 && "修改文件大小失败");
                }
            }
            else
            {
                assert(0 && "文件定位失败");
            }
        }
        WriteHead();
    }
    void Set(const INDEX& Index, const void* pData, DWORD dwSize, BOOL bSaveIndex = TRUE)
    {
        assert(m_hFile != NULL);
        assert(pData != NULL && dwSize > 0);
        std::map<INDEX, Index_st>::iterator clId = m_clIndex.find(Index);
        if (clId != m_clIndex.end())
        {
            if (clId->second.dwSize < dwSize)
            {
                FreeIndex(clId->second);
                clId->second = AllocIndex(dwSize);
            }
            else
            {
                if (clId->second.dwUsedSize != dwSize)
                {
                    m_bIndexChange = TRUE;
                    clId->second.dwUsedSize = dwSize;
                }
            }
        }
        else
        {
            clId = m_clIndex.insert(std::make_pair(Index, AllocIndex(dwSize))).first;
        }
        Write(clId->second.lAddress, pData, dwSize);
        m_bHeadChange = TRUE;
        if (bSaveIndex && m_bIndexChange)
        {
            WriteIndex();
        }
    }
    template<class DATATYPE>
    void Set(const INDEX& Index, const DATATYPE& Data, BOOL bSaveIndex = TRUE)
    {
        Set(Index, &Data, sizeof(Data), bSaveIndex);
    }
    void Get(const INDEX& Index, void* pData, DWORD dwSize)
    {
        assert(m_hFile != NULL);
        assert(pData != NULL && dwSize > 0);
        std::map<INDEX, Index_st>::const_iterator clId = m_clIndex.find(Index);
        if (clId != m_clIndex.end())
        {
            if (clId->second.dwUsedSize > dwSize)
            {
                assert(0 && "接受的空间不够大");
            }
            else
            {
                Read(clId->second.lAddress, pData, clId->second.dwUsedSize);
            }
        }
        else
        {
            assert(0 && "在资源文件中找不到数据");
        }
    }
    template<class DATATYPE>
    void Get(const INDEX& Index, DATATYPE& Data) const
    {
        Get(Index, &Data, sizeof(Data));
    }
    DWORD GetSize(const INDEX& Index) const
    {
        assert(m_hFile != NULL);
        std::map<INDEX, Index_st>::const_iterator clId = m_clIndex.find(Index);
        if (clId != m_clIndex.end())
        {
            return clId->second.dwUsedSize;
        }
        else
        {
            assert(0 && "在资源文件中找不到数据");
            return 0;
        }
    }
    void Del(const INDEX& Index, BOOL bSaveIndex = TRUE)
    {
        assert(m_hFile != NULL);
        std::map<INDEX, Index_st>::iterator clId = m_clIndex.find(Index);
        if (clId != m_clIndex.end())
        {
            FreeIndex(clId->second);
            m_clIndex.erase(clId);
        }
        else
        {
            assert(0 && "在资源文件中找不到数据");
            return;
        }
        if (bSaveIndex)
        {
            WriteIndex();
        }
        else
        {
            m_bIndexChange = TRUE;
        }
    }
    BOOL IsExist(const INDEX& Index) const
    {
        assert(m_hFile != NULL);
        return m_clIndex.find(Index) != m_clIndex.end();
    }
    DWORD GetCount()
    {
        return m_clIndex.size();
    }
    INT64 GetFileSize()
    {
        LARGE_INTEGER lFileSize;
        if (!GetFileSizeEx(m_hFile, &lFileSize))
        {
            assert(0 && "取得资源文件大小失败");
            return 0;
        }
        return lFileSize.QuadPart;
    }
    BOOL IsNull() const
    {
        return m_hFile == NULL;
    }
    void GetAllIndex(std::vector<INDEX>& clIndex) const
    {
        clIndex.resize(m_clIndex.size());
        DWORD dwCount = 0;
        for (std::map<INDEX, Index_st>::const_iterator clId = m_clIndex.begin(); clId != m_clIndex.end(); ++clId)
        {
            clIndex[dwCount] = clId->first;
            ++dwCount;
        }
    }
    void Defrag()
    {
        assert(m_hFile != NULL);
        m_bIndexChange = TRUE;
        INT64 lBeginAddress = m_SpareMgr.Alloc(1).first;
        m_SpareMgr.Clear();
        m_SpareMgr.MarkUsed(0, lBeginAddress);
        for (std::map<INDEX, Index_st>::iterator clId = m_clIndex.begin(); clId != m_clIndex.end(); ++clId)
        {
            if (clId->second.lAddress > lBeginAddress)
            {
                utility::CAutoArrayPtr<char>::AutoPtr pData(new char[clId->second.dwUsedSize]);
                Read(clId->second.lAddress, pData, clId->second.dwUsedSize);
                clId->second = AllocIndex(clId->second.dwUsedSize);
                Write(clId->second.lAddress, pData, clId->second.dwUsedSize);
                m_bHeadChange = TRUE;
            }
        }
        if (m_stHeadIndex.lAddress > lBeginAddress)
        {
            m_stHeadIndex = AllocIndex(m_stHeadIndex.dwUsedSize);
        }
        if (m_bIndexChange)
        {
            WriteIndex();
        }
    }
    INT64 GetUsedSize() const //如果索引数据在末尾，就有0-1024的误差
    {
        INT64 lUsedSize = sizeof(Head_st) + m_stHeadIndex.dwUsedSize;
        for (std::map<INDEX, Index_st>::const_iterator clId = m_clIndex.begin(); clId != m_clIndex.end(); ++clId)
        {
            lUsedSize += clId->second.dwUsedSize;
        }
        return lUsedSize;
    }
private:
    void FreeIndex(const Index_st& stIndex)
    {
        m_bIndexChange = TRUE;
        m_SpareMgr.Free(stIndex.lAddress, stIndex.dwSize);
    }
    Index_st AllocIndex(DWORD dwSize)
    {
        m_bIndexChange = TRUE;
        if (dwSize == 0)
        {
            return Index_st(0, 0);
        }
        std::pair<INT64, DWORD> clResult = m_SpareMgr.Alloc(dwSize);
        assert(clResult.second > 0);
        return Index_st(clResult.first, clResult.second, dwSize);
    }
    void Write(INT64 lOffset, const void* pData, DWORD dwSize)
    {
        assert(m_hFile != NULL);
        assert(pData != NULL && dwSize >= 0);
        LARGE_INTEGER lFileOffset;
        lFileOffset.QuadPart = lOffset;
        if (!SetFilePointerEx(m_hFile, lFileOffset, NULL, FILE_BEGIN))
        {
            assert(0 && "文件定位失败");
            return;
        }
        if (!utility::WriteFileContent(m_hFile, reinterpret_cast<const BYTE*>(pData), dwSize))
        {
            assert(0 && "写文件失败");
        }
    }
    void Read(INT64 lOffset, void* pData, DWORD dwSize)
    {
        assert(m_hFile != NULL);
        assert(pData != NULL && dwSize >= 0);
        LARGE_INTEGER lFileOffset;
        lFileOffset.QuadPart = lOffset;
        if (!SetFilePointerEx(m_hFile, lFileOffset, NULL, FILE_BEGIN))
        {
            assert(0 && "文件定位失败");
            return;
        }
        if (!utility::ReadFileContent(m_hFile, reinterpret_cast<BYTE*>(pData), dwSize))
        {
            assert(0 && "读文件失败");
        }
    }
    void WriteHead()
    {
        assert(m_hFile != NULL);
        SYSTEMTIME stSystemTime;
        GetSystemTime(&stSystemTime);
        FILETIME stFileTime;
        SystemTimeToFileTime(&stSystemTime, &stFileTime);
        Head_st stHead = {{'R', 'E', 'M', 'R'}, C_CURRENT_VERSION, m_stHeadIndex.lAddress, m_stHeadIndex.dwSize, m_clIndex.size(), m_SpareMgr.GetStorageSize(), stFileTime, 0};
        Write(0, &stHead, sizeof(stHead));
        SetFileTime(m_hFile, NULL, NULL, &stFileTime);
        m_bHeadChange = FALSE;
    }
    Result_en ReadHead(DWORD& dwUsedCount, DWORD& dwSpareInfoSize)
    {
        assert(m_hFile != NULL);
        Head_st stHead;
        Read(0, &stHead, sizeof(stHead));
        const char C_FILE_SIGN[] = {'R', 'E', 'M', 'R'};
        if (memcmp(stHead.acSign, C_FILE_SIGN, sizeof(C_FILE_SIGN)) != 0)
        {
            return E_RST_ERR_SIGN;
        }
        if (stHead.dwVersion != C_CURRENT_VERSION)
        {
            return E_RST_ERR_VERISION;
        }
        FILETIME stUpdateTime;
        if (!GetFileTime(m_hFile, NULL, NULL, &stUpdateTime))
        {
            assert(0 && "取得文件更新时间失败");
            return E_RST_ERR_UNKNOWN;
        }
        if (memcmp(&stHead.stUpdateTime, &stUpdateTime, sizeof(stUpdateTime)) != 0)
        {
            SYSTEMTIME stFileTime, stWriteTime;
            FileTimeToSystemTime(&stUpdateTime, &stFileTime);
            FileTimeToSystemTime(&stHead.stUpdateTime, &stWriteTime);
            return E_RST_ERR_UPDATETIME;
        }
        m_stHeadIndex.lAddress = stHead.iIndexAddress;
        m_stHeadIndex.dwUsedSize = stHead.dwIndexSize;
        m_stHeadIndex.dwSize = stHead.dwIndexSize;
        dwUsedCount = stHead.dwUsedCount;
        dwSpareInfoSize = stHead.dwSpareInfoSize;
        return E_RST_SUCESS;
    }
    Result_en ReadIndex()
    {
        DWORD dwUsedCount, dwSpareInfoSize;
        Result_en enResult = ReadHead(dwUsedCount, dwSpareInfoSize);
        if (enResult != E_RST_SUCESS)
        {
            return enResult;
        }
        if (dwSpareInfoSize > 0)
        {
            utility::CAutoArrayPtr<char>::AutoPtr pSpareData(new char[dwSpareInfoSize]);
            Read(m_stHeadIndex.lAddress + sizeof(DataIndex_st) * dwUsedCount, pSpareData.Get(), dwSpareInfoSize);
            m_SpareMgr<<std::make_pair(pSpareData.Get(), dwSpareInfoSize);
        }
        m_SpareMgr.MarkUsed(m_stHeadIndex.lAddress, m_stHeadIndex.dwSize);
        if (dwUsedCount > 0)
        {
            utility::CAutoArrayPtr<DataIndex_st>::AutoPtr pIndexData(new DataIndex_st[dwUsedCount]);
            Read(m_stHeadIndex.lAddress, pIndexData.Get(), sizeof(DataIndex_st) * dwUsedCount);
            for (DWORD i = 0; i < dwUsedCount; ++i)
            {
                m_clIndex.insert(std::make_pair(pIndexData.Get()[i].Index, pIndexData.Get()[i].stIndex));
                m_SpareMgr.MarkUsed(pIndexData.Get()[i].stIndex.lAddress, pIndexData.Get()[i].stIndex.dwSize);
            }
            if (dwUsedCount != m_clIndex.size())
            {
                assert(0 && "资源管理文件读取数据错误");
                return E_RST_ERR_UNKNOWN;
            }
        }
        return E_RST_SUCESS;
    }
};

#pragma once

#include "I8ResMgrFile.h"
#include "I8MemoyStream.h"
#include <map>
#include <vector>
#include "Utility/SmartPtr.hpp"

template<class INDEX, class DATA, class MEMORYISTREAM = CI8MemoyIStream, class MEMORYOSTREAM = CI8MemoyOStream>
class CI8DynamicResMgr
{
    enum ResOperator_en
    {
        E_RO_SET = 1,
        E_RO_DEL,
    };
    struct ResIndexInfo_st
    {
        INDEX Index;
        UINT64 nAccessSN;
    };
    struct ResInfo_st
    {
        INDEX Index;
        UINT64 nAccessSN;
        DATA* pData;
        DWORD dwSize;
    };
private:
    CI8ResMgrFile<INDEX> m_clFile;
    INT64 m_lLimistSize;
    INT64 m_lDataSize;
    UINT64 m_nLastAccessSN;
    std::map<INDEX, ResInfo_st> m_clRes;
    std::map<UINT64, ResIndexInfo_st> m_clAccessRes;
    std::map<INDEX, ResOperator_en> m_clResOperator;
public:
    CI8DynamicResMgr() : m_lLimistSize(0), m_lDataSize(0), m_nLastAccessSN(0)
    {
    }
    ~CI8DynamicResMgr()
    {
        Clear();
    }
    void Clear()
    {
        if (!m_clFile.IsNull())
        {
            WriteFile();
            m_clFile.Close();
        }
        for (std::map<INDEX, ResInfo_st>::iterator clId = m_clRes.begin(); clId != m_clRes.end(); ++clId)
        {
            delete clId->second.pData;
        }
        m_clRes.clear();
        m_clAccessRes.clear();
        m_lDataSize = 0;
        m_nLastAccessSN = 0;
        assert(m_clResOperator.size() == 0);
    }
    typename CI8ResMgrFile<INDEX>::Result_en SetFile(LPCTSTR lpszFilePath)
    {
        if (!m_clFile.IsNull())
        {
            Clear();
        }
        CI8ResMgrFile<INDEX>::Result_en enResult = m_clFile.Open(lpszFilePath);
        if (enResult != CI8ResMgrFile<INDEX>::E_RST_SUCESS)
        {
            return enResult;
        }
        std::vector<INDEX> clIndex;
        m_clFile.GetAllIndex(clIndex);
        for (DWORD i = 0; i < clIndex.size(); ++i)
        {
            ResInfo_st stResInfo = {clIndex[i], 0, NULL, 0};
            m_clRes.insert(std::make_pair(clIndex[i], stResInfo));
        }
        return CI8ResMgrFile<INDEX>::E_RST_SUCESS;
    }
    void SetLimistSize(INT64 lLimistSize)
    {
        m_lLimistSize = lLimistSize;
        CheckLimistSize();
    }
    void SetData(const INDEX& Index, const DATA& Data)
    {
        assert(!m_clFile.IsNull());
        m_clResOperator[Index] = E_RO_SET;
        std::map<INDEX, ResInfo_st>::iterator clId = m_clRes.find(Index);
        if (clId == m_clRes.end())
        {
            ResInfo_st stResInfo = {Index, 0, NULL, 0};
            clId = m_clRes.insert(std::make_pair(Index, stResInfo)).first;
        }
        if (clId->second.pData == NULL)
        {
            clId->second.pData = new DATA(Data);
        }
        else
        {
            m_lDataSize -= clId->second.dwSize;
            *(clId->second.pData) = Data;
        }
        MEMORYOSTREAM Stream;
        Stream<<Data;
        m_lDataSize += Stream.GetSize();
        clId->second.dwSize = Stream.GetSize();
        AccessData(clId->second);
        CheckLimistSize();
    }
    BOOL IsExist(const INDEX& Index)
    {
        assert(!m_clFile.IsNull());
        return m_clRes.find(Index) != m_clRes.end();
    }
    const DATA& GetData(const INDEX& Index)
    {
        assert(!m_clFile.IsNull());
        std::map<INDEX, ResInfo_st>::iterator clResInfoId = m_clRes.find(Index);
        if (clResInfoId == m_clRes.end())
        {
            throw 0;
        }
        ResInfo_st& stResInfo = clResInfoId->second;
        if (stResInfo.pData == NULL)
        {
            DWORD dwSize = m_clFile.GetSize(Index);
            utility::CAutoArrayPtr<char>::AutoPtr pData(new char[dwSize]);
            m_clFile.Get(Index, pData, dwSize);
            MEMORYISTREAM Stream(pData, dwSize);
            stResInfo.pData = new DATA;
            Stream>>(*stResInfo.pData);
            stResInfo.dwSize = dwSize;
            m_lDataSize += dwSize;
        }
        AccessData(stResInfo);
        CheckLimistSize();
        return *stResInfo.pData;
    }
    const DATA* Find(const INDEX& Index)
    {
        assert(!m_clFile.IsNull());
        std::map<INDEX, ResInfo_st>::iterator clResInfoId = m_clRes.find(Index);
        if (clResInfoId == m_clRes.end())
        {
            return NULL;
        }
        ResInfo_st& stResInfo = clResInfoId->second;
        if (stResInfo.pData == NULL)
        {
            DWORD dwSize = m_clFile.GetSize(Index);
            utility::CAutoArrayPtr<char>::AutoPtr pData(new char[dwSize]);
            m_clFile.Get(Index, pData, dwSize);
            MEMORYISTREAM Stream(pData, dwSize);
            stResInfo.pData = new DATA;
            Stream>>(*stResInfo.pData);
            stResInfo.dwSize = dwSize;
            m_lDataSize += dwSize;
        }
        AccessData(stResInfo);
        CheckLimistSize();
        return stResInfo.pData;
    }
    void DeleteData(const INDEX& Index)
    {
        assert(!m_clFile.IsNull());
        std::map<INDEX, ResInfo_st>::iterator clId = m_clRes.find(Index);
        if (clId != m_clRes.end())
        {
            m_clResOperator[Index] = E_RO_DEL;
            if (clId->second.nAccessSN != 0)
            {
                m_clAccessRes.erase(m_clAccessRes.find(clId->second.nAccessSN));
                m_lDataSize -= clId->second.dwSize;
            }
            m_clRes.erase(clId);
        }
        else
        {
            assert(0 && "删除数据失败");
        }
    }
    BOOL IsNull() const
    {
        return m_clFile.IsNull();
    }
    DWORD GetCount() const
    {
        assert(!m_clFile.IsNull());
        return m_clRes.size();
    }
    INT64 GetActiveDataSize() const
    {
        assert(!m_clFile.IsNull());
        return m_lDataSize;
    }
    void GetAllIndex(std::vector<INDEX>& clIndex) const
    {
        assert(!m_clFile.IsNull());
        clIndex.resize(m_clRes.size());
        DWORD dwCount = 0;
        for (std::map<INDEX, ResInfo_st>::const_iterator clId = m_clRes.begin(); clId != m_clRes.end(); ++clId)
        {
            clIndex[dwCount] = clId->first;
            ++dwCount;
        }
    }
    void WriteFile()
    {
        assert(!m_clFile.IsNull());
        if (m_clResOperator.size() > 0)
        {
            for (std::map<INDEX, ResOperator_en>::iterator clId = m_clResOperator.begin(); clId != m_clResOperator.end(); ++clId)
            {
                switch (clId->second)
                {
                case E_RO_SET:
                    {
                        std::map<INDEX, ResInfo_st>::iterator clResId = m_clRes.find(clId->first);
                        if (clResId != m_clRes.end())
                        {
                            MEMORYOSTREAM Stream;
                            Stream<<(*clResId->second.pData);
                            m_clFile.Set(clId->first, Stream.GetPointer(), Stream.GetSize(), FALSE);
                        }
                        else
                        {
                            assert(0 && "要保存的数据失踪");
                        }
                    }
                    break;
                case E_RO_DEL:
                    if (m_clFile.IsExist(clId->first))
                    {
                        m_clFile.Del(clId->first, FALSE);
                    }
                    break;
                default:
                    assert(0 && "错误数据操作");
                    break;
                }
            }
            m_clResOperator.clear();
            m_clFile.WriteIndex();
            if (m_clFile.GetUsedSize() < m_clFile.GetFileSize() / 2)
            {
                m_clFile.Defrag();
            }
        }
    }
private:
    void AccessData(ResInfo_st& stResInfo)
    {
        if (stResInfo.nAccessSN == 0 || stResInfo.nAccessSN != m_nLastAccessSN)
        {
            if (stResInfo.nAccessSN != 0)
            {
                m_clAccessRes.erase(m_clAccessRes.find(stResInfo.nAccessSN));
            }
            stResInfo.nAccessSN = ++m_nLastAccessSN;
            ResIndexInfo_st stResIndexInfo = {stResInfo.Index, stResInfo.nAccessSN};
            m_clAccessRes.insert(std::make_pair(stResInfo.nAccessSN, stResIndexInfo));
        }
    }
    void CheckLimistSize()
    {
        if (m_clFile.IsNull())
        {
            return;
        }
        while (m_lLimistSize > 0 && m_lDataSize > m_lLimistSize && m_clAccessRes.size() > 1)
        {
            if (m_clResOperator.size() != 0)
            {
                WriteFile();
            }
            std::map<UINT64, ResIndexInfo_st>::iterator clAccessResId = m_clAccessRes.begin();
            std::map<INDEX, ResInfo_st>::iterator clId = m_clRes.find(clAccessResId->second.Index);
            assert(clId->second.pData != NULL);
            m_lDataSize -= clId->second.dwSize;
            delete clId->second.pData;
            clId->second.pData = NULL;
            clId->second.dwSize = 0;
            clId->second.nAccessSN = 0;
            m_clAccessRes.erase(clAccessResId);
        }
    }
};
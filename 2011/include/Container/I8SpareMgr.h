#pragma once

#include <set>
#include <minmax.h>

class CI8SpareMgr
{
    struct Index_st
    {
        INT64 lAddress;
        DWORD dwSize;
        Index_st() : lAddress(0), dwSize(0)
        {
        }
        Index_st(INT64 lNewAddress, DWORD dwNewSize) : lAddress(lNewAddress), dwSize(dwNewSize)
        {
        }
        bool operator<(const Index_st& stIndex) const 
        {
            return dwSize < stIndex.dwSize;
        }
    };
    INT64 m_lMaxSize;
    std::multiset<Index_st> m_clSpareIndex;
    INT64 m_lLastAddress;
public:
    CI8SpareMgr(INT64 lMaxSize = -1)
        : m_lMaxSize(lMaxSize)
        , m_lLastAddress(0)
    {
    }
    INT64 GetSize() const
    {
        return m_lLastAddress;
    }
    DWORD GetStorageSize()
    {
        return m_clSpareIndex.size() * sizeof(Index_st);
    }
    void Clear()
    {
        m_clSpareIndex.clear();
        m_lLastAddress = 0;
    }
    std::pair<INT64, DWORD> Alloc(DWORD dwSize)
    {
        class FindSize_cl
        {
            DWORD m_dwSize;
        public:
            FindSize_cl(DWORD dwSize) : m_dwSize(dwSize){}
            BOOL operator()(const Index_st& stIndex){return stIndex.dwSize >= m_dwSize;}
        };
        if (dwSize == 0)
        {
            return std::make_pair(0, 0);
        }
        for (std::multiset<Index_st>::iterator clId = std::find_if(m_clSpareIndex.begin(), m_clSpareIndex.end(), FindSize_cl(dwSize)); clId != m_clSpareIndex.end(); ++clId)
        {
            Index_st stIndex = *clId;
            m_clSpareIndex.erase(clId);
            return std::make_pair(stIndex.lAddress, stIndex.dwSize);
        }
        if (m_lMaxSize > 0 && m_lLastAddress + dwSize > m_lMaxSize)
        {
            return std::make_pair(0, 0);
        }
        Index_st stIndex(m_lLastAddress, dwSize);
        m_lLastAddress += dwSize;
        return std::make_pair(stIndex.lAddress, stIndex.dwSize);
    }
    void Free(INT64 lAddress, DWORD dwSize)
    {
        class FindAddress_cl
        {
            INT64 m_lAddress;
        public:
            FindAddress_cl(INT64 lAddress) : m_lAddress(lAddress){}
            BOOL operator()(const Index_st& stIndex){return stIndex.lAddress + stIndex.dwSize == m_lAddress;}
        };
        if (lAddress + dwSize == m_lLastAddress)
        {
            m_lLastAddress = lAddress;
            while (TRUE)
            {
                std::multiset<Index_st>::iterator clId = std::find_if(m_clSpareIndex.begin(), m_clSpareIndex.end(), FindAddress_cl(m_lLastAddress));
                if (clId == m_clSpareIndex.end())
                {
                    break;
                }
                m_lLastAddress = clId->lAddress;
                m_clSpareIndex.erase(clId);
            }
        }
        else
        {
            m_clSpareIndex.insert(Index_st(lAddress, dwSize));
        }
    }
    void MarkUsed(INT64 lAddress, INT64 lSize) //没有考虑标记到空闲区中，假设所有使用区都是由Alloc取得的
    {
        m_lLastAddress = max(m_lLastAddress, lAddress + lSize);
    }
    CI8SpareMgr& operator>>(const std::pair<void*, DWORD>& clData)
    {
        assert(clData.first != NULL);
        if (GetStorageSize() != clData.second)
        {
            assert(0 && "存贮空间分配信息的大小不准确");
            return *this;
        }
        Index_st* pIndexData = static_cast<Index_st*>(clData.first);
        DWORD dwIndex = 0;
        for (std::multiset<Index_st>::const_iterator clId = m_clSpareIndex.begin(); clId != m_clSpareIndex.end(); ++clId)
        {
            pIndexData[dwIndex] = *clId;
            ++dwIndex;
        }
        return *this;
    }
    CI8SpareMgr& operator<<(const std::pair<const void*, DWORD>& clData)
    {
        assert(clData.first != NULL);
        if (clData.second % sizeof(Index_st) != 0)
        {
            assert(0 && "空间分配信息的大小不正确，无法计算出信息数量");
            return *this;
        }
        Clear();
        const Index_st* pIndexData = static_cast<const Index_st*>(clData.first);
        DWORD dwSpareCount = clData.second / sizeof(Index_st);
        for (DWORD i = 0; i < dwSpareCount; ++i)
        {
            m_clSpareIndex.insert(pIndexData[i]);
            m_lLastAddress = max(m_lLastAddress, pIndexData[i].lAddress + pIndexData[i].dwSize);
        }
        return *this;
    }
};

#pragma once

#include "Utility/utility.h"

class CI8MemoyOStream
{
    std::vector<char> m_clData;
    UINT m_nPos;
public:
    CI8MemoyOStream(int iLengh = 1024) : m_nPos(0)
    {
        assert(iLengh > 0);
        m_clData.resize(iLengh);
    }
    const char* GetPointer() const
    {
        return &m_clData[0];
    }
    int GetSize() const
    {
        return m_nPos;
    }
    CI8MemoyOStream& operator<<(int iData)
    {
        WriteData(&iData, sizeof(int));
        return *this;
    }
    CI8MemoyOStream& operator<<(DWORD dwData)
    {
        WriteData(&dwData, sizeof(DWORD));
        return *this;
    }
    template<int iCount>
    CI8MemoyOStream& operator<<(const TCHAR(&atData)[iCount])
    {
        DWORD dwCount = lstrlen(atData) + 1;
        WriteData(&dwCount, sizeof(dwCount));
        WriteData(atData, dwCount * sizeof(TCHAR));
        return *this;
    }
    void Write(const WCHAR* pwData, DWORD dwCount)
    {
        WriteData(pwData, dwCount * sizeof(WCHAR));
    }
    void Write(const char* pcData, DWORD dwCount)
    {
        WriteData(pcData, dwCount * sizeof(char));
    }
    void Clear()
    {
        m_clData.resize(0);
        m_nPos = 0;
    }
private:
    void WriteData(const void* pData, UINT iSize)
    {
        if (m_nPos + iSize > m_clData.size())
        {
            m_clData.resize(m_nPos + iSize);
        }
        assert(pData != NULL && iSize > 0);
        memcpy(&m_clData[m_nPos], pData, iSize);
        m_nPos += iSize;
    }
};

class CI8MemoyIStream
{
    const char* m_pData;
    UINT m_nSize;
    UINT m_nPos;
    BOOL m_bAutoFree;
public:
    CI8MemoyIStream() : m_pData(NULL), m_nSize(0), m_nPos(0), m_bAutoFree(FALSE)
    {
    }
    CI8MemoyIStream(const char* pData, UINT nSize) : m_nPos(0), m_bAutoFree(FALSE)
    {
        assert(pData != NULL && nSize > 0);
        m_pData = pData;
        m_nSize = nSize;
    }
    ~CI8MemoyIStream()
    {
        if (m_bAutoFree && m_pData != NULL)
        {
            delete[] m_pData;
            m_pData = NULL;
        }
    }
    BOOL ReadFile(LPCTSTR lpszFileName)
    {
        assert(m_pData == NULL);
        utility::CAutoFile clFile = ::CreateFile(lpszFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (!clFile.IsValid()) return FALSE;

        DWORD nSize = GetFileSize(clFile, NULL);
        char* pData = new char[nSize];
        if (!utility::ReadFileContent(clFile, reinterpret_cast<BYTE*>(pData), nSize))
        {
            delete[] pData;
            return FALSE;
        }
        m_nSize = nSize;
        m_pData = pData;
        m_bAutoFree = TRUE;
        m_nPos = 0;
        return TRUE;
    }
    const char* GetPointer() const
    {
        return m_pData;
    }
    int GetCurPos() const
    {
        return m_nPos;
    }
    int GetSize() const
    {
        return m_nSize;
    }
    CI8MemoyIStream& operator>>(DWORD& dwData)
    {
        ReadData(&dwData, sizeof(DWORD));
        return *this;
    }
    CI8MemoyIStream& operator>>(int& iData)
    {
        ReadData(&iData, sizeof(int));
        return *this;
    }
    template<int iCount>
    CI8MemoyIStream& operator>>(TCHAR(&atData)[iCount])
    {
        DWORD dwCount;
        ReadData(&dwCount, sizeof(dwCount));
        if (dwCount > UINT(iCount))
        {
            assert(0 && "字符数组太小，无法读取数据");
            return *this;
        }
        ReadData(atData, dwCount * sizeof(TCHAR));
        return *this;
    }
    void Read(WCHAR* pwData, DWORD dwCount)
    {
        ReadData(pwData, dwCount * sizeof(WCHAR));
    }
    void Read(char* pcData, DWORD dwCount)
    {
        ReadData(pcData, dwCount * sizeof(char));
    }
private:
    void ReadData(void* pData, UINT iSize)
    {
        assert(m_pData != NULL);
        if (m_nPos + iSize > m_nSize)
        {
            assert(0 && "读数据失败");
            return;
        }
        assert(pData != NULL && iSize > 0);
        memcpy(pData, m_pData + m_nPos, iSize);
        m_nPos += iSize;
    }
};

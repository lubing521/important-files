

template<class ARRAY_TYPE, class TYPE>
class CI8ClientDataCoreReturnData
{
    ARRAY_TYPE* m_pData;
public:
    CI8ClientDataCoreReturnData(ARRAY_TYPE* pData) : m_pData(pData)
    {
    }
    ~CI8ClientDataCoreReturnData()
    {
        if (m_pData != NULL)
        {
            I8CD_ReleaseData(m_pData);
        }
    }
    ARRAY_TYPE& operator->()
    {
        assert(m_pData != NULL);
        return *m_pData;
    }
    int GetCount() const
    {
        return m_pData->iCount;
    }
    TYPE& operator[](int iIndex)
    {
        assert(m_pData != NULL && iIndex >= 0 && iIndex < m_pData->iCount);
        return m_pData->aItem[iIndex];
    }
    const TYPE& operator[](int iIndex) const
    {
        assert(m_pData != NULL && iIndex >= 0 && iIndex < m_pData->iCount);
        return m_pData->aItem[iIndex];
    }
private:
    CI8ClientDataCoreReturnData(const CI8ClientDataCoreReturnData&);
    CI8ClientDataCoreReturnData& operator=(const CI8ClientDataCoreReturnData&);
};

#pragma once

#include "../Data/CustomDataType.h"
#include "../../../../include/Extend STL/StringAlgorithm.h"

namespace i8desk
{
    namespace business
    {
        class CSelectState
        {
            int m_iSelectCount;
            stdex::tString m_sInfoEx;
        public:
            CSelectState()
                : m_iSelectCount(0)
            {
            }
            bool Set(int iSelectCount, const stdex::tString& sInfoEx = stdex::tString())
            {
                assert(iSelectCount >= 0);
                if (iSelectCount != m_iSelectCount || m_sInfoEx != sInfoEx)
                {
                    m_sInfoEx = sInfoEx;
                    m_iSelectCount = iSelectCount;
                    return true;
                }
                else
                {
                    return false;
                }
            }
            void Clear()
            {
                m_iSelectCount = 0;
                m_sInfoEx.clear();
            }
            stdex::tString GetText()
            {
                stdex::tString sInfo;
                if (m_iSelectCount == 0) 
                {
                    sInfo = _T("已选0个");
                }
                else
                {
                    sInfo = _T("已选");
                    sInfo += stdex::ToString<stdex::tString>(m_iSelectCount) + _T("个");
                }
                if (!m_sInfoEx.empty())
                {
                    sInfo += _T(",") + m_sInfoEx;
                }
                return sInfo;
            }
        };
    }
}

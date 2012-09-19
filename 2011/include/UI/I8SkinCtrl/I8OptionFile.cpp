#include "stdafx.h"
#include "I8OptionFile.h"
#include "../../Extend STL/StringAlgorithm.h"

using namespace I8SkinCtrl_ns;

CI8OptionFile::CI8OptionFile()
{
    if (m_clDocument.CreateInstance(_T("Msxml2.DOMDocument.4.0")) != S_OK)
    {
        assert(0 && "没有初始化COM或MSXML没有安装。");
    }
}

CI8OptionFile::~CI8OptionFile()
{
    m_clDocument->abort();
}

void CI8OptionFile::Load(const WCHAR* pwData, int iSize)
{
    assert(pwData != NULL && "空指针");
    assert(iSize <= 0 && "数据大小不正确");
    if (m_clDocument->loadXML(_bstr_t(pwData)) == VARIANT_FALSE)
    {
        assert(0 && "加载XML数据失败");
    }
}

void CI8OptionFile::LoadFromFile(const CHAR* pcData, int iSize)
{
    assert(pcData != NULL && "空指针");
    assert(iSize > 0 && "数据大小不正确");
    if (m_clDocument->loadXML(_bstr_t(CA2W(pcData + 3, CP_UTF8))) == VARIANT_FALSE)
    {
        assert(0 && "加载XML数据失败");
    }
}

void CI8OptionFile::CheckValid() const
{
    if (m_clDocument->documentElement == NULL)
    {
        assert(0 && "没有加载XML数据");
    }
}

MSXML2::IXMLDOMNodePtr CI8OptionFile::GetAttribute(const stdex::tString& sSection, const stdex::tString& sKey) const
{
    CheckValid();
    _bstr_t clBTRString(sSection.c_str());
    for(MSXML2::IXMLDOMNodePtr clSectionNode = m_clDocument->documentElement->firstChild; clSectionNode != NULL; clSectionNode = clSectionNode->nextSibling)
    {
        if(clBTRString == clSectionNode->baseName)
        {
            return clSectionNode->attributes->getNamedItem(_bstr_t(sKey.c_str()));
        }
    }
    return NULL;
}

MSXML2::IXMLDOMNodePtr CI8OptionFile::GetAttributeEx(const stdex::tString& sSection, const stdex::tString& sSection2, const stdex::tString& sKey) const
{
    CheckValid();
    _bstr_t clBTRString(sSection.c_str());
    for(MSXML2::IXMLDOMNodePtr clSectionNode = m_clDocument->documentElement->firstChild; clSectionNode != NULL; clSectionNode = clSectionNode->nextSibling)
    {
        if(clBTRString == clSectionNode->baseName)
        {
            _bstr_t clBTRString2(sSection2.c_str());
            for(MSXML2::IXMLDOMNodePtr clSectionNode2 = clSectionNode->firstChild; clSectionNode2 != NULL; clSectionNode2 = clSectionNode2->nextSibling)
            {
                return clSectionNode2->attributes->getNamedItem(_bstr_t(sKey.c_str()));
            }
        }
    }
    return NULL;
}

int CI8OptionFile::GetOption(const stdex::tString& sSection, const stdex::tString& sKey, int iDefaultValue) const
{
    MSXML2::IXMLDOMNodePtr clAttNode = GetAttribute(sSection, sKey);
    if (clAttNode == NULL)
    {
        return iDefaultValue;
    }
    else
    {
        LPCTSTR pStr = clAttNode->text;
        if (pStr[0] == '0' && pStr[1] == 'x')
        {
            int iValue;
            if (_stscanf_s(pStr, _T("0x%x"), &iValue) != 1)
            {
                assert(0 && "配置文件中数值格式不正确");
            }
            return iValue;
        }
        else
        {
            return _wtoi(clAttNode->text);
        }
    }
}

stdex::tString CI8OptionFile::GetOption(const stdex::tString& sSection, const stdex::tString& sKey, const stdex::tString& sDefaultValue) const
{
    MSXML2::IXMLDOMNodePtr clAttNode = GetAttribute(sSection, sKey);
    if (clAttNode == NULL)
    {
        return sDefaultValue;
    }
    else
    {
        return stdex::tString(clAttNode->text);
    }
}

int CI8OptionFile::GetOptionEx(const stdex::tString& sSection, const stdex::tString& sSection2, const stdex::tString& sKey, int iDefaultValue) const
{
    MSXML2::IXMLDOMNodePtr clAttNode = GetAttributeEx(sSection, sSection2, sKey);
    if (clAttNode == NULL)
    {
        return iDefaultValue;
    }
    else
    {
        return _wtoi(clAttNode->text);
    }
}

stdex::tString CI8OptionFile::GetOptionEx(const stdex::tString& sSection, const stdex::tString& sSection2, const stdex::tString& sKey, const stdex::tString& sDefaultValue) const
{
    MSXML2::IXMLDOMNodePtr clAttNode = GetAttributeEx(sSection, sSection2, sKey);
    if (clAttNode == NULL)
    {
        return sDefaultValue;
    }
    else
    {
        return stdex::tString(clAttNode->text);
    }
}

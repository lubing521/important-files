#pragma once

#include "../../Extend STL/UnicodeStl.h"
#import "msxml3.dll"

namespace I8SkinCtrl_ns
{
    class CI8OptionFile
    {
        MSXML2::IXMLDOMDocumentPtr m_clDocument;
    public:
        CI8OptionFile();
        ~CI8OptionFile();
        void Load(const WCHAR* pwData, int iSize);
        void LoadFromFile(const CHAR* pcData, int iSize);
        int GetOption(const stdex::tString& sSection, const stdex::tString& sKey, int iDefaultValue) const;
        stdex::tString GetOption(const stdex::tString& sSection, const stdex::tString& sKey, const stdex::tString& sDefaultValue) const;
        stdex::tString GetOptionEx(const stdex::tString& sSection, const stdex::tString& sSection2, const stdex::tString& sKey, const stdex::tString& sDefaultValue) const;
        int GetOptionEx(const stdex::tString& sSection, const stdex::tString& sSection2, const stdex::tString& sKey, int iDefaultValue) const;
    private:
        CI8OptionFile(const CI8OptionFile&);
        CI8OptionFile& operator=(const CI8OptionFile&);
        MSXML2::IXMLDOMNodePtr GetAttribute(const stdex::tString& sSection, const stdex::tString& sKey) const;
        MSXML2::IXMLDOMNodePtr GetAttributeEx(const stdex::tString& sSection, const stdex::tString& sSection2, const stdex::tString& sKey) const;
        void CheckValid() const;
    };
}

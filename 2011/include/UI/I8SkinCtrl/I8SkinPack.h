#pragma once

#include "I8ZipPack.h"
#include "I8SkinBitmap.h"
#include "../../Extend STL/UnicodeStl.h"
#include "I8OptionFile.h"
#include <map>

namespace I8SkinCtrl_ns
{
    class CI8SkinPack : public CI8ZipPack
    {
        std::map<stdex::tString, HBITMAP> m_clBitmapLib;
        std::map<stdex::tString, stdex::tString> m_clCtrlDefaultSkin;
        std::map<stdex::tString, CI8OptionFile*> m_clOptionLib;
        std::map<stdex::tString, HFONT> m_clFontLib;
    public:
        CI8SkinPack();
        ~CI8SkinPack();
	    static CI8SkinPack& Instance();
        void LoadSkin(CI8SkinBitmap& clBitmap, const stdex::tString& sFile, BOOL bCheckExist = TRUE);
        void Release();
        void SetDefaultSkin(const stdex::tString& sCtrlName, const stdex::tString& sSkinName);
        const stdex::tString& GetDefaultSkin(const stdex::tString& sCtrlName) const;
        int GetOption(const stdex::tString& sSection, const stdex::tString& sKey, int iDefaultValue, const stdex::tString& sName);
        stdex::tString GetOption(const stdex::tString& sSection, const stdex::tString& sKey, const stdex::tString& sDefaultValue, const stdex::tString& sName);
        HFONT GetFont(const stdex::tString& sSection, const stdex::tString& sKey, const stdex::tString& sName);
        HFONT GetFont(const stdex::tString& sSection, const stdex::tString& sName);
    protected:
        const CI8OptionFile& GetOptionFile(const stdex::tString& sName);
    };
}
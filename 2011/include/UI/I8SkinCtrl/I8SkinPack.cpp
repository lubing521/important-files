#include "stdafx.h"
#include "I8SkinPack.h"

using namespace I8SkinCtrl_ns;

CI8SkinPack::CI8SkinPack()
{
}

I8SkinCtrl_ns::CI8SkinPack::~CI8SkinPack()
{
    Release();
}

CI8SkinPack& CI8SkinPack::Instance()
{
	static CI8SkinPack s_clSkinPack;
	return s_clSkinPack;
}

void CI8SkinPack::LoadSkin(CI8SkinBitmap& clBitmap, const stdex::tString& sFile, BOOL bCheckExist)
{
    HBITMAP& hBitmap = m_clBitmapLib[sFile.c_str()];
    if (hBitmap == NULL)
    {
        hBitmap = LoadBitmapFromZip(sFile.c_str());
        if (hBitmap == NULL && bCheckExist)
        {
            assert(0 && "皮肤图片无法加载。");
        }
    }
    clBitmap.Set(hBitmap, FALSE);
}

void CI8SkinPack::Release()
{
    for (std::map<stdex::tString, HBITMAP>::iterator clId = m_clBitmapLib.begin(); clId != m_clBitmapLib.end(); ++clId)
    {
        DeleteObject(clId->second);
    }
    m_clBitmapLib.clear();
    m_clCtrlDefaultSkin.clear();
    for (std::map<stdex::tString, CI8OptionFile*>::iterator clId = m_clOptionLib.begin(); clId != m_clOptionLib.end(); ++clId)
    {
        delete clId->second;
    }
    m_clOptionLib.clear();
    for (std::map<stdex::tString, HFONT>::iterator clId = m_clFontLib.begin(); clId != m_clFontLib.end(); ++clId)
    {
        DeleteObject(clId->second);
    }
    m_clFontLib.clear();
}

void CI8SkinPack::SetDefaultSkin(const stdex::tString& sCtrlName, const stdex::tString& sSkinName)
{
    m_clCtrlDefaultSkin[sCtrlName] = sSkinName;
}

const stdex::tString& CI8SkinPack::GetDefaultSkin(const stdex::tString& sCtrlName) const
{
    const std::map<stdex::tString, stdex::tString>::const_iterator clId = m_clCtrlDefaultSkin.find(sCtrlName);
    if (clId == m_clCtrlDefaultSkin.end())
    {
        assert(0 && "控件没有设置默认皮肤。");
    }
    return clId->second;
}

int I8SkinCtrl_ns::CI8SkinPack::GetOption(const stdex::tString& sSection, const stdex::tString& sKey, int iDefaultValue, const stdex::tString& sName)
{
    return GetOptionFile(sName).GetOption(sSection, sKey, iDefaultValue);
}

stdex::tString I8SkinCtrl_ns::CI8SkinPack::GetOption(const stdex::tString& sSection, const stdex::tString& sKey, const stdex::tString& sDefaultValue, const stdex::tString& sName)
{
    return GetOptionFile(sName).GetOption(sSection, sKey, sDefaultValue);
}

const CI8OptionFile& CI8SkinPack::GetOptionFile(const stdex::tString& sName)
{
    stdex::tString sFile = sName + _T(".xml");
    const std::map<stdex::tString, CI8OptionFile*>::const_iterator clId = m_clOptionLib.find(sFile);
    if (clId == m_clOptionLib.end())
    {
        CI8OptionFile* pclOptionFile = new CI8OptionFile;
        m_clOptionLib[sFile] = pclOptionFile;
        CI8Trusteeship<CHAR> clData = LoadDataFromZip(sFile.c_str());
        assert(!clData.IsNull() && "找不到配置文件");
        pclOptionFile->LoadFromFile(clData.GetPointer(), clData.GetCount());
        return *pclOptionFile;
    }
    else
    {
        return *clId->second;
    }
}
HFONT CI8SkinPack::GetFont(const stdex::tString& sSection, const stdex::tString& sKey, const stdex::tString& sName)
{
    stdex::tString sFontName = sName + _T("|") + sSection + _T("|") + sKey;
    const std::map<stdex::tString, HFONT>::const_iterator clId = m_clFontLib.find(sFontName);
    if (clId == m_clFontLib.end())
    {
        const CI8OptionFile& clOptionFile = GetOptionFile(sName);
        stdex::tString sFont = clOptionFile.GetOptionEx(sSection, sKey, _T("字体"), _T("Arial"));
        int iFontSize = clOptionFile.GetOptionEx(sSection, sKey, _T("大小"), 10);
        BOOL bBold = clOptionFile.GetOptionEx(sSection, sKey, _T("粗体"), FALSE);
        BOOL bItalic = clOptionFile.GetOptionEx(sSection, sKey, _T("斜体"), FALSE);
        HFONT hFont = ::CreateFont(iFontSize, 0, 0, 0, bBold ? FW_NORMAL : FW_BOLD, bItalic, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, sFont.c_str());
        assert(hFont != NULL && "创建字体失败");
        m_clFontLib[sFontName] = hFont;
        return hFont;
    }
    else
    {
        return clId->second;
    }
}

HFONT CI8SkinPack::GetFont(const stdex::tString& sSection, const stdex::tString& sName)
{
    stdex::tString sFontName = sName + _T("|") + sSection;
    const std::map<stdex::tString, HFONT>::const_iterator clId = m_clFontLib.find(sFontName);
    if (clId == m_clFontLib.end())
    {
        const CI8OptionFile& clOptionFile = GetOptionFile(sName);
        stdex::tString sFont = clOptionFile.GetOption(sSection, _T("字体"), _T("Arial"));
        int iFontSize = clOptionFile.GetOption(sSection, _T("大小"), 10);
        BOOL bBold = clOptionFile.GetOption(sSection, _T("粗体"), FALSE);
        BOOL bItalic = clOptionFile.GetOption(sSection, _T("斜体"), FALSE);
        HFONT hFont = ::CreateFont(iFontSize, 0, 0, 0, bBold ? FW_NORMAL : FW_BOLD, bItalic, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, sFont.c_str());
        assert(hFont != NULL && "创建字体失败");
        m_clFontLib[sFontName] = hFont;
        return hFont;
    }
    else
    {
        return clId->second;
    }
}

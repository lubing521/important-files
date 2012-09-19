#pragma once

#include "../../Zip/Xunzip.h"
#include "I8Trusteeship.h"
#include <memory>

namespace I8SkinCtrl_ns
{
    class CI8ZipPack
    {
        TCHAR szFile[MAX_PATH];
        HZIP  m_hZip;
        char* m_pZipFile;
    public:
	    CI8ZipPack();
	    ~CI8ZipPack();
	    BOOL   LoadSkinInfo(const CString& strSkinFile);
	    BOOL   LoadZip(const CString& strPath);

	    CBitmap* LoadSkinFromHIcon(HICON hIcon);
	    CBitmap* LoadSkinFromFile(LPCTSTR szFile);
	    CBitmap* LoadSkinFromResId(DWORD dwResId, LPCTSTR lpszResType = TEXT("PNG"));
	    CBitmap* LoadSkinFromZip(LPCTSTR szPicture);
	    HICON LoadIconFromZip(LPCTSTR szPicture);
	    BOOL ExtractItem(LPCTSTR szItem, LPCTSTR szFilePath);
    protected:
        CI8Trusteeship<CHAR> LoadDataFromZip(LPCTSTR szFile);
        HBITMAP LoadBitmapFromZip(LPCTSTR szFile);
    };
}
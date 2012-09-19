#include "stdafx.h"
#include "I8ZipPack.h"
#include "../../UI/IconHelper.h"
#include <shlwapi.h>
#include "../../Utility/SmartHandle.hpp"

using namespace I8SkinCtrl_ns;

CI8ZipPack::CI8ZipPack()
{
}
CI8ZipPack::~CI8ZipPack()
{
	if (m_hZip != NULL)
	{
		CloseZip(m_hZip);
		m_hZip = NULL;
	}
	if (m_pZipFile != NULL)
	{
		delete []m_pZipFile;
		m_pZipFile = NULL;
	}
}

BOOL   CI8ZipPack::LoadSkinInfo(const CString& strSkinFile)
{
	if(!LoadZip(strSkinFile))
	{
		if (!LoadZip(TEXT("skin\\skin.zip")))
		{
			return FALSE;
		}
	}
	return TRUE;
}
BOOL   CI8ZipPack::LoadZip(const CString& strPath)
{
	m_hZip = NULL;
	m_pZipFile = NULL;
	utility::CAutoFile clFile = CreateFile(strPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (clFile.IsValid())
	{
		DWORD dwSize = GetFileSize(clFile, NULL);
		m_pZipFile = new char[dwSize + 1];
		DWORD dwReadBytes = 0;
		ReadFile(clFile, m_pZipFile, dwSize, &dwReadBytes, NULL);
		m_pZipFile[0] = 0;
		clFile.CleanUp();
		m_hZip = OpenZip(m_pZipFile, dwSize, ZIP_MEMORY, NULL);
		if (m_hZip != NULL)
		{
			return TRUE;
		}
	}
	return FALSE;
}

CBitmap* CI8ZipPack::LoadSkinFromHIcon(HICON hIcon)
{
	CBitmap* pBitmap = NULL;
	Gdiplus::Bitmap* pImage = Gdiplus::Bitmap::FromHICON(hIcon);
	if (pImage != NULL)
	{
		HBITMAP  hBitmap = NULL;
		if (Gdiplus::Ok == pImage->GetHBITMAP(Gdiplus::Color(0, 0, 0, 0), &hBitmap) && hBitmap != NULL)
		{
			pBitmap = new CBitmap;
			pBitmap->Attach(hBitmap);
		}
		delete pImage;
	}
	return pBitmap;
}

CBitmap* CI8ZipPack::LoadSkinFromFile(LPCTSTR szFile)
{
	CBitmap* pBitmap = NULL;
	Gdiplus::Bitmap* pImage = Gdiplus::Bitmap::FromFile(szFile);
	if (pImage != NULL)
	{
		HBITMAP hBitmap = NULL;
		if (Gdiplus::Ok == pImage->GetHBITMAP(Gdiplus::Color(0, 0, 0, 0), &hBitmap) && hBitmap != NULL)
		{
			pBitmap = new CBitmap;
			pBitmap->Attach(hBitmap);
		}
		delete pImage;
	}
	return pBitmap;
}

CBitmap* CI8ZipPack::LoadSkinFromResId(DWORD dwResId, LPCTSTR lpszResType)
{
	HGLOBAL hGlobal = NULL;
	{
		HMODULE hMod = AfxGetInstanceHandle();
		HRSRC hRes = FindResource(hMod, MAKEINTRESOURCE(dwResId), lpszResType);
		if (hRes != NULL)
		{
			HGLOBAL hResGlobal = LoadResource(hMod, hRes);
			if (hResGlobal != NULL)
			{
				LPVOID  lpData = LockResource(hResGlobal);
				if (lpData != NULL)
				{
					DWORD dwSize = SizeofResource(hMod, hRes);
					hGlobal = GlobalAlloc(GMEM_FIXED, dwSize);
					LPVOID lpVoid = GlobalLock(hGlobal);
					if (lpVoid == NULL)
					{
						GlobalFree(hGlobal);
						hGlobal = NULL;
					}
					else
					{
						memcpy(lpVoid, lpData, dwSize);
						GlobalUnlock(hGlobal);
					}
				}
			}
		}
	}

	CBitmap* pBitmap = NULL;
	if (hGlobal != NULL)
	{
		IStream* pIStream = NULL;
		Gdiplus::Bitmap* pImage = NULL;
		DWORD dwError = CreateStreamOnHGlobal(hGlobal, TRUE, &pIStream);
		if (pIStream != NULL && (pImage = new Gdiplus::Bitmap(pIStream)) != NULL)
		{
			HBITMAP hBitmap = NULL;
			if (Gdiplus::Ok == pImage->GetHBITMAP(Gdiplus::Color(0, 0, 0, 0), &hBitmap) && hBitmap != NULL)
			{
				pBitmap = new CBitmap;
				pBitmap->Attach(hBitmap);					
			}
		}
		delete pImage;
		GlobalFree(hGlobal);
	}
	return pBitmap;
}

CBitmap* CI8ZipPack::LoadSkinFromZip(LPCTSTR szPicture)
{
	if (szPicture == NULL || m_hZip == NULL) return NULL;

#ifdef _UNICODE
	ZIPENTRYW ze = {0};
#else
	ZIPENTRY ze = {0};
#endif
	CBitmap* pBitmap = NULL;
	int idx = 0;
	ZRESULT zr = FindZipItem(m_hZip, szPicture, FALSE, &idx, &ze);
	if (zr == ZR_OK)
	{
		HGLOBAL hGlobal = GlobalAlloc(GMEM_FIXED, ze.unc_size);
		if (hGlobal != NULL)
		{
			void* pVoid = GlobalLock(hGlobal);
			if (pVoid != NULL)
			{
				UnzipItem(m_hZip, ze.index, pVoid, ze.unc_size, ZIP_MEMORY);
				GlobalUnlock(hGlobal);
     
				IStream* pIStream = NULL;
				Gdiplus::Bitmap* pImage = NULL;
				DWORD dwError = CreateStreamOnHGlobal(hGlobal, FALSE, &pIStream);
				if (pIStream != NULL && (pImage = new Gdiplus::Bitmap(pIStream)) != NULL)
				{
					HBITMAP hBitmap = NULL;
					if (Gdiplus::Ok == pImage->GetHBITMAP(Gdiplus::Color(255, 0, 0, 0), &hBitmap) && hBitmap != NULL)
					{
						pBitmap = new CBitmap;
						pBitmap->Attach(hBitmap);					
					}
					delete pImage;
				}
				if (pIStream != NULL)
				{
					pIStream->Release();
				}
			}
			GlobalFree(hGlobal);
		}
	}

	return pBitmap;
}
HICON CI8ZipPack::LoadIconFromZip(LPCTSTR szPicture)
{
	if (szPicture == NULL || m_hZip == NULL) return NULL;

#ifdef _UNICODE
	ZIPENTRYW ze = {0};
#else
	ZIPENTRY ze = {0};
#endif
	HICON hIcon = NULL;
	int idx = 0;
	ZRESULT zr = FindZipItem(m_hZip, szPicture, FALSE, &idx, &ze);
	if (zr == ZR_OK)
	{
		HGLOBAL hGlobal = GlobalAlloc(GMEM_FIXED, ze.unc_size);
		if (hGlobal != NULL)
		{
			void* pVoid = GlobalLock(hGlobal);
			if (pVoid != NULL)
			{
				UnzipItem(m_hZip, ze.index, pVoid, ze.unc_size, ZIP_MEMORY);
				GlobalUnlock(hGlobal);
                hIcon = LoadIconFromBuffer(static_cast<const unsigned char *>(pVoid), ze.unc_size);
			}
			GlobalFree(hGlobal);
		}
	}

	return hIcon;
}

BOOL CI8ZipPack::ExtractItem(LPCTSTR szItem, LPCTSTR szFilePath)
{
	if (szItem == NULL || m_hZip == NULL) return FALSE;

#ifdef _UNICODE
	ZIPENTRYW ze = {0};
#else
	ZIPENTRY ze = {0};
#endif
	HICON hIcon = NULL;
	BOOL  bRet = FALSE;
	int idx = 0;
	ZRESULT zr = FindZipItem(m_hZip, szItem, FALSE, &idx, &ze);
	if (zr == ZR_OK)
	{
		HGLOBAL hGlobal = GlobalAlloc(GMEM_FIXED, ze.unc_size);
		if (hGlobal != NULL)
		{
			void* pVoid = GlobalLock(hGlobal);
			if (pVoid != NULL)
			{
				UnzipItem(m_hZip, ze.index, pVoid, ze.unc_size, ZIP_MEMORY);
				GlobalUnlock(hGlobal);
				utility::CAutoFile clFile = ::CreateFile(szFilePath, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
					NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if (clFile.IsValid()) 
				{
					DWORD dwByteWrite = 0;
					if (WriteFile(clFile, pVoid, ze.unc_size, &dwByteWrite, NULL) && dwByteWrite == ze.unc_size)
					{
						bRet = TRUE;
					}
					SetEndOfFile(clFile);
				}
			}
			GlobalFree(hGlobal);
		}
	}

	return bRet;
}

CI8Trusteeship<CHAR> CI8ZipPack::LoadDataFromZip(LPCTSTR szFile)
{
    assert(szFile != NULL && "空指针");
    assert(m_hZip != NULL && "没有加载Zip文件");
#ifdef _UNICODE
    ZIPENTRYW ze = {0};
#else
    ZIPENTRY ze = {0};
#endif
   CI8Trusteeship<CHAR> clData;
    int idx = 0;
    ZRESULT zr = FindZipItem(m_hZip, szFile, FALSE, &idx, &ze);
    if (zr == ZR_OK)
    {
        clData.ApplyArray(ze.unc_size);
        UnzipItem(m_hZip, ze.index, clData.Pointer(), clData.GetCount(), ZIP_MEMORY);
    }
    return clData;
}

HBITMAP CI8ZipPack::LoadBitmapFromZip(LPCTSTR szFile)
{
    if (szFile == NULL || m_hZip == NULL) return NULL;

#ifdef _UNICODE
    ZIPENTRYW ze = {0};
#else
    ZIPENTRY ze = {0};
#endif
    HBITMAP hBitmap = NULL;
    int idx = 0;
    ZRESULT zr = FindZipItem(m_hZip, szFile, FALSE, &idx, &ze);
    if (zr == ZR_OK)
    {
        HGLOBAL hGlobal = GlobalAlloc(GMEM_FIXED, ze.unc_size);
        if (hGlobal != NULL)
        {
            void* pVoid = GlobalLock(hGlobal);
            if (pVoid != NULL)
            {
                UnzipItem(m_hZip, ze.index, pVoid, ze.unc_size, ZIP_MEMORY);
                GlobalUnlock(hGlobal);

                IStream* pIStream = NULL;
                Gdiplus::Bitmap* pImage = NULL;
                DWORD dwError = CreateStreamOnHGlobal(hGlobal, FALSE, &pIStream);
                if (pIStream != NULL && (pImage = new Gdiplus::Bitmap(pIStream)) != NULL)
                {
                    if (!(Gdiplus::Ok == pImage->GetHBITMAP(Gdiplus::Color(0, 0, 0, 0), &hBitmap) && hBitmap != NULL))
                    {
                        assert(0);
                    }
                    delete pImage;
                }
                if (pIStream != NULL)
                {
                    pIStream->Release();
                }
            }
            GlobalFree(hGlobal);
        }
    }

    return hBitmap;
}

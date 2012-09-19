#include "stdafx.h"
#include "BitmapHelper.hpp"



CBitmapFile* PASCAL CBitmapFile::FromHandle(HBITMAP hBitmap)

{

	return (CBitmapFile*) CGdiObject::FromHandle(hBitmap);

}

CBitmapFile::CBitmapFile()

{

}

BOOL CBitmapFile::LoadBitmap(LPCTSTR lpszFileName)

{

	CFile file;

	if(!file.Open(lpszFileName,CFile::modeRead|CFile::shareDenyWrite))
	{
		return FALSE;

	}

	BITMAPFILEHEADER bfhHeader;

	file.Read(&bfhHeader,sizeof(BITMAPFILEHEADER));

	if(bfhHeader.bfType!=((WORD) ('M'<<8)|'B'))
	{
		return FALSE;

	}

	if(bfhHeader.bfSize!=file.GetLength())
	{
		return FALSE;

	}

	UINT uBmpInfoLen=(UINT) bfhHeader.bfOffBits-sizeof(BITMAPFILEHEADER);

	LPBITMAPINFO lpBitmap=(LPBITMAPINFO) new BYTE[uBmpInfoLen];

	file.Read((LPVOID) lpBitmap,uBmpInfoLen);

	if((* (LPDWORD)(lpBitmap))!=sizeof(BITMAPINFOHEADER))
	{
		return FALSE;

	}

	DWORD dwBitlen=bfhHeader.bfSize - bfhHeader.bfOffBits;

	LPVOID lpBits=new BYTE[dwBitlen];

	file.Read(lpBits,dwBitlen);

	file.Close();

	BOOL bSuccess=CreateBitmapIndirect(lpBitmap, lpBits);

	delete lpBitmap;

	delete lpBits;

	if(!bSuccess)

		return FALSE;

	return TRUE;

}

BOOL CBitmapFile::CreateBitmap(int nWidth, int nHeight, UINT nBitCount,

							   const void* lpSrcBits)

{

	ASSERT(nBitCount==1||nBitCount==4||nBitCount==8

		||nBitCount==16||nBitCount==24||nBitCount==32);

	LPBITMAPINFO lpBitmap;

	lpBitmap=(BITMAPINFO*) new BYTE[sizeof(BITMAPINFOHEADER) +

		GetColorNumber(nBitCount) * sizeof(RGBQUAD)];

	lpBitmap->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);

	lpBitmap->bmiHeader.biWidth=nWidth;

	lpBitmap->bmiHeader.biHeight=nHeight;

	lpBitmap->bmiHeader.biBitCount=nBitCount;

	lpBitmap->bmiHeader.biPlanes=1;

	lpBitmap->bmiHeader.biCompression=BI_RGB;

	lpBitmap->bmiHeader.biSizeImage=0;

	lpBitmap->bmiHeader.biClrUsed=0;

	BOOL bSuccess=CreateBitmapIndirect(lpBitmap, lpSrcBits);

	delete lpBitmap;

	if(!bSuccess)

		return FALSE;

	return TRUE;

}

BOOL CBitmapFile::CreateBitmapIndirect(LPBITMAPINFO lpBitmapInfo, const void* lpSrcBits)

{

	DeleteObject();

	LPVOID lpBits;

	CDC *dc=new CDC;

	dc->CreateCompatibleDC(NULL);

	HBITMAP hBitmap=::CreateDIBSection(dc->m_hDC,lpBitmapInfo,DIB_RGB_COLORS,

		&lpBits,NULL,0);

	ASSERT(hBitmap!=NULL);

	delete dc;

	Attach(hBitmap);

	BITMAP bmp;

	GetBitmap(&bmp);

	DWORD dwCount=(DWORD) bmp.bmWidthBytes * bmp.bmHeight;

	if(SetBitmapBits(dwCount,lpSrcBits)!=dwCount)
	{
		return FALSE;

	}

	return TRUE;

}

CBitmapFile::operator HBITMAP() const

{

	return (HBITMAP)(this == NULL ? NULL : m_hObject);

}

int CBitmapFile::GetBitmap(BITMAP* pBitMap)

{

	ASSERT(m_hObject != NULL);

	return ::GetObject(m_hObject, sizeof(BITMAP), pBitMap);

}

int CBitmapFile::GetColorNumber(WORD wBitCount)

{

	ASSERT(wBitCount==1||wBitCount==4||wBitCount==8

		||wBitCount==16||wBitCount==24||wBitCount==32);

	switch(wBitCount)

	{

	case 1:

		return 2;

	case 4:

		return 16;

	case 8:

		return 256;

	default:

		return 0;

	}

}

DWORD CBitmapFile::SetBitmapBits(DWORD dwCount, const void* lpBits)

{

	if(lpBits!=NULL)

	{

		BITMAP bmp;

		GetBitmap(&bmp);

		memcpy(bmp.bmBits,lpBits,dwCount);

		return dwCount;

	}

	else

		return 0;

}

DWORD CBitmapFile::GetBitmapBits(DWORD dwCount, LPVOID lpBits)

{

	if(lpBits!=NULL)

	{

		BITMAP bmp;

		GetBitmap(&bmp);

		memcpy(lpBits,bmp.bmBits,dwCount);

		return dwCount;

	}

	else

		return 0;

}

CBitmapFile::~CBitmapFile()

{

	CGdiObject::DeleteObject();

}
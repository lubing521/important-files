#include "stdafx.h"
#include "I8Progress.h"
#include "I8SkinCommonFunction.h"

using namespace I8SkinCtrl_ns;

IMPLEMENT_DYNAMIC(CI8Progress, CI8OldButton)

BEGIN_MESSAGE_MAP(CI8Progress, CI8OldButton)
END_MESSAGE_MAP()

CI8Progress::CI8Progress()
{
	m_bIsHorz   = TRUE;
	m_fProgress= 0;
	m_pbkImage	= NULL;
	m_pfgImage	= NULL;
}

CI8Progress::~CI8Progress()
{
	if (m_pbkImage)
	{
		m_pbkImage->DeleteObject();
		delete m_pbkImage;
	}
	if (m_pfgImage)
	{
		m_pfgImage->DeleteObject();
		delete m_pfgImage;
	}
}

void CI8Progress::SetHorzScroll(BOOL bHorz /* = TRUE */)
{
	m_bIsHorz = bHorz;
	if (m_hWnd != NULL)
		Invalidate();
}

void  CI8Progress::SetProgress(float fProgress)
{
	m_fProgress = fProgress;
	if (m_hWnd != NULL)
		Invalidate();
}

void  CI8Progress::SetImage(LPCTSTR lpszBkImage /* = NULL */, LPCTSTR lpszFgImage /* = NULL */)
{
	m_pbkImage = I8_GetSkinImage(lpszBkImage);
	m_pfgImage = I8_GetSkinImage(lpszFgImage);
}

void CI8Progress::DrawItem(LPDRAWITEMSTRUCT lpDis)
{
	CRect rc(lpDis->rcItem);
	CDC* pDC = CDC::FromHandle(lpDis->hDC);

	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);

	if (m_pbkImage != NULL)
	{
		BITMAP bmp = {0};
		m_pbkImage->GetBitmap(&bmp);
		dcMem.SelectObject(m_pbkImage);
		pDC->StretchBlt(0, 0, rc.Width(), rc.Height(), &dcMem, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
	}

	if (m_pfgImage != NULL)
	{
		BITMAP bmp = {0};
		m_pbkImage->GetBitmap(&bmp);
		dcMem.SelectObject(m_pfgImage);
		if (m_bIsHorz)
		{
			int nWidth = int(rc.Width() * m_fProgress / 100 + .5f);
			pDC->StretchBlt(0, 0, nWidth, rc.Height(), &dcMem, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
		}
		else
		{
			int nHeight = int(rc.Height() * m_fProgress / 100 + .5f);
			pDC->StretchBlt(0, 0, rc.Width(), nHeight, &dcMem, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
		}
	}
}

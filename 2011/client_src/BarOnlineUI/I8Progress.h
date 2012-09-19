#pragma once

#include "I8SkinCommonFunction.h"
#include "I8OldButton.h"

namespace I8SkinCtrl_ns
{
    class CI8Progress : public CI8OldButton
    {
    	DECLARE_DYNAMIC(CI8Progress)
    public:
    	CI8Progress();
    	virtual ~CI8Progress();
    public:
    	void  SetHorzScroll(BOOL bHorz = TRUE);
    	BOOL  IsHorzScroll()	{ return m_bIsHorz; }
    	void  SetProgress(float fProgress /* 0.0 - 100.0 */);
    	float GetProgress()	{ return m_fProgress; }
    
    	void  SetImage(LPCTSTR lpszBkImage = NULL, LPCTSTR lpszFgImage = NULL);
    protected:
    	virtual void DrawItem(LPDRAWITEMSTRUCT lpDis);
    	DECLARE_MESSAGE_MAP();
    private:
    	BOOL  m_bIsHorz;
    	float m_fProgress;
    	CBitmap* m_pbkImage, *m_pfgImage;
    };
}

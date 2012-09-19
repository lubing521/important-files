#include "stdafx.h"
#include "I8ComboBox.h"
#include "I8SkinCommonFunction.h"

using namespace I8SkinCtrl_ns;

#define DEFAULT_TIP_TEXT2	TEXT("ÇëÊäÈëÍøÖ·")

IMPLEMENT_DYNAMIC(CI8ComboBox, CComboBox)

CI8ComboBox::CI8ComboBox()
{
	m_pbkImage	= I8_GetSkinImage(TEXT("Skin/BrowseBar/±³¾°_µØÖ·À¸_1px.png"));
	m_pRight	= I8_GetSkinImage(TEXT("Skin/BrowseBar/°´Å¥_µØÖ·À¸_ÏÂÀ­ÁÐ±í_Ä¬ÈÏ×´Ì¬.png"));
	m_Edit.SetDefaultText(DEFAULT_TIP_TEXT2);
}

CI8ComboBox::~CI8ComboBox()
{
	if (m_pRight)
	{
		m_pRight->DeleteObject();
		delete m_pRight;
	}

	if (m_pbkImage)
	{
		m_pbkImage->DeleteObject();
		delete m_pbkImage;
	}
}

BEGIN_MESSAGE_MAP(CI8ComboBox, CComboBox)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CI8ComboBox::OnPaint()
{
	CPaintDC dc(this);

	CRect rcClient;
	GetClientRect(&rcClient);
	rcClient.bottom = rcClient.top + 25;

	CRect rx = rcClient;	
	I8_DrawImage(&dc, m_pbkImage, rcClient);

	rx = rcClient;
	rx.left = rcClient.right - 26;	
	I8_DrawImage(&dc, m_pRight, rx);
}

void CI8ComboBox::OnSize(UINT nType, int cx, int cy)
{
	CComboBox::OnSize(nType, cx, cy);

	if (m_Edit.GetSafeHwnd() != NULL)
	{
		CRect rc;
		m_Edit.GetWindowRect(&rc);
		ScreenToClient(&rc);
		if (rc.top < 5)
		{
			rc.OffsetRect(0, 5);
			m_Edit.MoveWindow(&rc);
		}
	}
}

void CI8ComboBox::OnDestroy()
{
	if (m_Edit.GetSafeHwnd() != NULL)
		m_Edit.UnsubclassWindow();
	CComboBox::OnDestroy();
}

HBRUSH CI8ComboBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (nCtlColor == CTLCOLOR_EDIT)
	{
		if ( m_Edit.GetSafeHwnd() == NULL )
		{
			m_Edit.SubclassWindow(pWnd->GetSafeHwnd());
		}
	}
	return CComboBox::OnCtlColor(pDC, pWnd, nCtlColor);
}

BOOL CI8ComboBox::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CI8ComboBox::OnSetFocus(CWnd* pOldWnd)
{
	m_Edit.SetFocus();
}

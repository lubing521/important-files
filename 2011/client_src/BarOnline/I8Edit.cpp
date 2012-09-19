#include "stdafx.h"
#include "I8Edit.h"

using namespace I8SkinCtrl_ns;

IMPLEMENT_DYNAMIC(CI8Edit, CEdit)

CI8Edit::CI8Edit()
{
}

CI8Edit::~CI8Edit()
{

}

BEGIN_MESSAGE_MAP(CI8Edit, CEdit)
	ON_WM_LBUTTONDOWN()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

void CI8Edit::xProcess()
{
	CString str;
	GetWindowText(str);
	if (str.IsEmpty())
	{
		SetWindowText(m_strDef);
	}
}

BOOL CI8Edit::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd != m_hWnd)
		return CEdit::PreTranslateMessage(pMsg);

	if (pMsg->message == WM_CHAR)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			//return TRUE;
		}
	}
	return CEdit::PreTranslateMessage(pMsg);
}

void CI8Edit::PreSubclassWindow()
{
	CEdit::PreSubclassWindow();
}

void CI8Edit::OnLButtonDown(UINT nFlags, CPoint point)
{
	CEdit::OnLButtonDown(nFlags, point);

	CString str;
	GetWindowText(str);
	if (str.CompareNoCase(m_strDef) == 0)
	{
		SetWindowText(TEXT(""));
	}
}

void CI8Edit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);
	xProcess();
}

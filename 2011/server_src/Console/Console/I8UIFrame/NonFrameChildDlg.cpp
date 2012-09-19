#include "stdafx.h"
#include "NonFrameChildDlg.h"
#include "../ConsoleDlg.h"
#include "../UI/Skin/SkinMgr.h"
#include "../UI/UIHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CNonFrameDlg

IMPLEMENT_DYNAMIC(CNonFrameChildDlg, CDialogEx)


CNonFrameChildDlg::CNonFrameChildDlg(UINT uID, CWnd* pParent /* = NULL */)
: CDialogEx(uID, pParent)
{
	dynamic_cast<i8desk::ui::ChildMgr *>(AfxGetMainWnd())->InsertTopChild(this);
}

CNonFrameChildDlg::~CNonFrameChildDlg()
{
	dynamic_cast<i8desk::ui::ChildMgr *>(AfxGetMainWnd())->EraseTopChild(this);
}

BEGIN_MESSAGE_MAP(CNonFrameChildDlg, CDialogEx)
	ON_WM_NCHITTEST()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


void CNonFrameChildDlg::SetTitle(const CString &text)
{
	titleText_ = text;
	if( ::IsWindow(GetSafeHwnd()) )
		Invalidate();
}

BOOL CNonFrameChildDlg::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN )     
	{     
		if( pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE )     
			return TRUE;       
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

BOOL CNonFrameChildDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//ModifyStyle(WS_BORDER, 0, SWP_DRAWFRAME);
	//ModifyStyle(WS_CAPTION, WS_MINIMIZEBOX, SWP_DRAWFRAME );

	HGDIOBJ hFont = ::GetStockObject(DEFAULT_GUI_FONT); 
	font_.Attach((HFONT)hFont);


	title_.Attach(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Windows_TopBg.png")));
	border_.Attach(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Windows_Border.png")));

	
	static CImage img;
	if( img.IsNull() )
		img.Attach(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Windows_TopClose.png")));


	CRect rcBtn;
	GetClientRect(rcBtn);
	rcBtn.left = rcBtn.right - img.GetWidth() - 10;
	rcBtn.right = rcBtn.left + img.GetWidth();
	rcBtn.top = rcBtn.top + 8;
	rcBtn.bottom = rcBtn.top + img.GetHeight();

	wndBtnClose_.Create(_T(""), WS_CHILD | WS_VISIBLE, rcBtn, this, IDCANCEL);
	wndBtnClose_.SetImages(
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Windows_TopClose.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Windows_TopCloseHover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Windows_TopCloseClick.png")));
	wndBtnClose_.SetThememParent(GetSafeHwnd());

	brush_.CreateSolidBrush(RGB(255, 255, 255));

	CRgn rgn;
	CRect rcClient;
	GetClientRect(rcClient);
	rgn.CreateRoundRectRgn(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, 5, 5);
	SetWindowRgn(rgn, FALSE);

	return TRUE;
}

void CNonFrameChildDlg::OnDestroy()
{
	wndBtnClose_.DestroyWindow();
	title_.Detach();
	border_.Detach();

	return CDialogEx::OnDestroy();
}

void CNonFrameChildDlg::OnPaint()
{
	CPaintDC dc(this);
	
	CRgn rgn;
	CRect rcClient;
	GetClientRect(rcClient);
	rgn.CreateRoundRectRgn(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, 5, 5);
	SetWindowRgn(rgn, FALSE);
}

BOOL CNonFrameChildDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);
	CMemDC memDC(*pDC, rcClient);

	memDC.GetDC().FillRect(rcClient, &brush_);

	// BK
	CRect rcTitle(rcClient.left, rcClient.top, rcClient.right, title_.GetHeight());
	title_.Draw(memDC.GetDC(), rcTitle);

	// Border
	CRect rcBorder(rcClient);
	rcBorder.top += title_.GetHeight();
	CRect rcOutLine(0, 0, border_.GetWidth(), border_.GetHeight());

	CRect rcWorkLeft = rcBorder;
	rcWorkLeft.right = rcWorkLeft.left + rcOutLine.Width();
	border_.Draw(memDC.GetDC(), rcWorkLeft, rcOutLine);

	CRect rcWorkRight = rcBorder;
	rcWorkRight.left = rcWorkRight.right - rcOutLine.Width();
	border_.Draw(memDC.GetDC(), rcWorkRight, rcOutLine);

	CRect rcWorkBottom = rcBorder;
	rcWorkBottom.top = rcWorkBottom.bottom - rcOutLine.Height();
	border_.Draw(memDC.GetDC(), rcWorkBottom, rcOutLine);

	// Title
	CRect rcText(rcTitle.left + 10, rcTitle.top, rcTitle.right, rcTitle.bottom);
	memDC.GetDC().SetBkMode(TRANSPARENT);

	HGDIOBJ fontOld = memDC.GetDC().SelectObject(font_);
	memDC.GetDC().SetTextColor(RGB(255, 255, 255));
	memDC.GetDC().DrawText(titleText_, rcText, DT_VCENTER | DT_LEFT | DT_SINGLELINE);
	memDC.GetDC().SelectObject(fontOld);

	return TRUE;
}

LRESULT CNonFrameChildDlg::OnNcHitTest(CPoint point)
{
	CRect rc;
	GetClientRect(&rc);
	ClientToScreen(&rc);

	return rc.PtInRect(point) ? HTCAPTION : CDialog::OnNcHitTest(point);
}
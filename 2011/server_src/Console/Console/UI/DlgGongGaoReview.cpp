#include "stdafx.h"
#include "../Console.h"
#include "DlgGongGaoReview.h"
#include "../ManagerInstance.h"
#include "../UI/UIHelper.h"
#include "../ui/Skin/SkinMgr.h"
#include "../../../../include/ui/ImageHelpers.h"


// CDlgGongGaoReview 对话框

IMPLEMENT_DYNAMIC(CDlgGongGaoReview, CNonFrameChildDlg)

CDlgGongGaoReview::CDlgGongGaoReview(LPCTSTR gonggao, LPCTSTR front, CWnd* pParent /*=NULL*/)
	: CNonFrameChildDlg(CDlgGongGaoReview::IDD, pParent)
	, textGongGao_(gonggao)
	, strFontString_(front)
{

}

CDlgGongGaoReview::~CDlgGongGaoReview()
{
}

void CDlgGongGaoReview::DoDataExchange(CDataExchange* pDX)
{
	CNonFrameChildDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_GONGGAO_REVIEW, wndEditGongGao_);
	DDX_Text(pDX, IDC_EDIT_GONGGAO_REVIEW, textGongGao_);
	DDX_Control(pDX, IDC_STATIC_GONGGAO_REVIEW, wndLabelGongGao_);
}


BEGIN_MESSAGE_MAP(CDlgGongGaoReview, CNonFrameChildDlg)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()

END_MESSAGE_MAP()


BOOL CDlgGongGaoReview::PreTranslateMessage(MSG* pMsg)
{
	return CDialogEx::PreTranslateMessage(pMsg);
}

// CDlgGongGaoReview 消息处理程序
BOOL CDlgGongGaoReview::OnInitDialog()
{
	CNonFrameChildDlg::OnInitDialog();

	ZeroMemory(&logfont_, sizeof(logfont_));
	logfont_.lfCharSet = DEFAULT_CHARSET;
	logfont_.lfOutPrecision = OUT_DEVICE_PRECIS;
	logfont_.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfont_.lfQuality = DEFAULT_QUALITY;
	logfont_.lfPitchAndFamily = DEFAULT_PITCH;

	//公告字体
	SetAdvFont();

	outboxLine_.Attach(CopyBitmap(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ControlOutBox.png"))));

	return TRUE;
}


void CDlgGongGaoReview::SetAdvFont()
{
	//prase gonggao font string. "字体|大小|颜色|标志(bit0:粗体，bit1:斜线,bit2:下画线,bit3:删除线)"
	{		
		CString szTemp;
		AfxExtractSubString(szTemp, strFontString_, 0, '|');
		if (szTemp.GetLength() == 0) szTemp = _T("宋体");
		_tcscpy(logfont_.lfFaceName, szTemp);

		AfxExtractSubString(szTemp, strFontString_, 1, '|');
		logfont_.lfHeight = -abs(_ttoi(szTemp));
		if (logfont_.lfHeight == 0)	logfont_.lfHeight = -12;		

		AfxExtractSubString(szTemp, strFontString_, 2, '|');
		fontColor_ = _ttoi(szTemp);
		if (fontColor_ == 0) fontColor_ = RGB(0, 0, 0);

		AfxExtractSubString(szTemp, strFontString_, 3, '|');
		DWORD flag = _ttoi(szTemp);
		logfont_.lfWeight	 = (flag & (1<<0)) ? FW_BOLD : FW_NORMAL;
		logfont_.lfItalic	 = (flag & (1<<1)) ? TRUE : FALSE;
		logfont_.lfUnderline = (flag & (1<<2)) ? TRUE : FALSE;
		logfont_.lfStrikeOut = (flag & (1<<3)) ? TRUE : FALSE;	
		font_.DeleteObject();
		BOOL ret = font_.CreateFontIndirect(&logfont_);
		wndEditGongGao_.SetFont(&font_);
		wndEditGongGao_.Invalidate();
	}
}

HBRUSH CDlgGongGaoReview::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_EDIT && pWnd->GetDlgCtrlID() == IDC_EDIT_GONGGAO_REVIEW)
	{
		pDC->SetTextColor(fontColor_);
	}

	return hbr;
}

BOOL CDlgGongGaoReview::OnEraseBkgnd(CDC* pDC)
{
	__super::OnEraseBkgnd(pDC);

	CRect rcMultiText;
	wndEditGongGao_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, *pDC, rcMultiText, &outboxLine_);

	return TRUE;
}
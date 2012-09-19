#include "stdafx.h"
#include "TJDlg.h"
#include "MonitorPage.h"

IMPLEMENT_DYNAMIC(CTJDlg, CDialog)


CTJDlg::CMyListBox::CMyListBox()
{
	LOGFONT lf = {0};
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);
	m_font.CreateFontIndirect(&lf);
	lf.lfWeight = FW_BOLD;
	m_fontbold.CreateFontIndirect(&lf);
}

CTJDlg::CMyListBox::~CMyListBox()
{
	for (size_t idx=0; idx<m_lstData.size(); idx++)
	{
		delete m_lstData[idx];
	}

}
void CTJDlg::CMyListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = 24;
	DWORD dwLine = lpMeasureItemStruct->itemID;
	if (dwLine < (DWORD)m_lstData.size())
	{
		DWORD dwId = m_lstData[dwLine]->Id;
		switch (dwId)
		{
		case 0:
			lpMeasureItemStruct->itemHeight = 33;
			break;
		case 1:
			lpMeasureItemStruct->itemHeight = 26;
			break;
		case 2:
			lpMeasureItemStruct->itemHeight = 24;
		}
	}
}

void CTJDlg::CMyListBox::DrawItem(LPDRAWITEMSTRUCT lpDS)
{
	CRect rc(lpDS->rcItem);
	rc.OffsetRect(-rc.left, -rc.top);

	CDC dc;
	dc.Attach(lpDS->hDC);

	CBitmap bmp;
	CDC dcMem; 
	dcMem.CreateCompatibleDC(NULL);
	bmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());

	dcMem.SelectObject(&bmp);
	dcMem.SetBkMode(1);
	dcMem.FillRect(&rc, &CBrush(RGB(255, 255, 255)));

	size_t nLine = lpDS->itemID;
	if (nLine >= m_lstData.size())
		return ;

	CRect rx = rc;
	tagList_Item *pItem = m_lstData[nLine];
	switch (pItem->Id)
	{
	case 0:
		{
			rx.left += 5;
			rx.right = rx.left + 14;
			rx.top = (rx.Height() - 15)/2; 
			rx.bottom = rx.top + 15;
			AfxDrawImage(&dcMem, pItem->Id, rx);
			rx = rc;
			rx.left = rc.left + 20;
			dcMem.SelectObject(&m_fontbold);
			dcMem.SetTextColor(RGB(0, 0, 0));
			dcMem.DrawText(pItem->Caption, -1, &rx, DT_LEFT|DT_SINGLELINE|DT_VCENTER);
		}
		break;
	case 1:
		{
			AfxDrawImage(&dcMem, IDB_TBL_RED, rc);
			rx.left = rc.left;
			rx.left += 20;
			dcMem.SelectObject(&m_fontbold);
			dcMem.SetTextColor(RGB(40,107,188));
			dcMem.DrawText(pItem->Caption, -1, &rx, DT_LEFT|DT_SINGLELINE|DT_VCENTER);

			if (pItem->StatusText[0] != 0)
			{
				rx.left = rc.left + 360;
				rx.right = rx.left + 14;
				rx.top += (rx.Height() - 14)/2;
				rx.bottom = rx.top + 14;
				AfxDrawImage(&dcMem, pItem->Status ? IDB_GOOD : IDB_WARNING, rx);
			}

			rx = rc;
			rx.left = rc.left + 376;
			rx.right = rc.right;
			dcMem.SetTextColor(RGB(0, 0, 0));
			dcMem.SelectObject(&m_font);
			dcMem.DrawText(pItem->StatusText, -1, &rx, DT_LEFT|DT_SINGLELINE|DT_VCENTER);
		}
		break;
	case 2:
		{
			dcMem.SetTextColor(RGB(0, 0, 0));
			dcMem.SelectObject(&m_font);
			rx.left += 20;
			dcMem.DrawText(pItem->Caption, -1, &rx, DT_LEFT|DT_SINGLELINE|DT_VCENTER);

			rx.left = rc.left + 360;
			rx.right = rx.left + 14;
			rx.top += (rx.Height() - 14)/2;
			rx.bottom = rx.top + 14;
			AfxDrawImage(&dcMem, pItem->Status ? IDB_GOOD : IDB_WARNING, rx);

			rx = rc;
			rx.left = rc.left + 376;
			rx.right = rc.right;
			if (!pItem->Status)
				dcMem.SetTextColor(RGB(122, 122, 0));
			dcMem.DrawText(pItem->StatusText, -1, &rx, DT_LEFT|DT_SINGLELINE|DT_VCENTER);
		}
		break;
	}

	{
		rx = rc;
		CPen pen(PS_SOLID, 1, RGB(232,233,235));
		dcMem.SelectObject(&pen);
		dcMem.MoveTo(rx.left, rx.bottom-1);
		dcMem.LineTo(rx.right, rx.bottom-1);
	}
	rc = CRect(lpDS->rcItem);
	dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &dcMem, 0, 0, SRCCOPY);
	dc.Detach();
}

CTJDlg::CTJDlg(CMonitorPage *pMonitorPage, CWnd* pParent /*=NULL*/)
	: CDialog(CTJDlg::IDD, pParent)
	, m_pMonitorPage(pMonitorPage)
	, m_bIsOnlyView(FALSE)
{
	m_nCheckProgress = 30;
	LOGFONT lf = {0};
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);
	lf.lfHeight = 16;
	lf.lfWeight = FW_BOLD;
	m_fontTitle.CreateFontIndirect(&lf);
	lf.lfHeight = 12;
	m_fontStatus.CreateFontIndirect(&lf);
}

CTJDlg::~CTJDlg()
{
}

void CTJDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_lstBox);
	DDX_Control(pDX, IDC_TITLE, m_staTitle);
	DDX_Control(pDX, IDC_STATUS, m_staStatus);
	DDX_Control(pDX, IDC_BUTTON1, m_btnCancel);
	DDX_Control(pDX, IDC_BUTTON2, m_btnClose);
	DDX_Control(pDX, IDC_PROGRESS, m_btnProgress);
}

BEGIN_MESSAGE_MAP(CTJDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CTJDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CTJDlg::OnBnClickedCancel)
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON1, &CTJDlg::OnBnClickedButton1)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON2, &CTJDlg::OnBnClickedButton2)
END_MESSAGE_MAP()

void CTJDlg::OnBnClickedOk()
{

}

void CTJDlg::OnBnClickedCancel()
{

}

BOOL CTJDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_staTitle.SetFont(&m_fontTitle);
	m_staStatus.SetFont(&m_fontStatus);
	m_btnCancel.SetImage(IDB_TJ_BTN);
	m_btnClose.SetImage(IDB_TJ_BTN);
	m_btnCancel.SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(3, 84, 166), RGB(3, 84, 166), RGB(3, 84, 166),
		RGB(3, 84, 166), CButtonEx::BTEXT_CENTER, FALSE);
	m_btnClose.SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(3, 84, 166), RGB(3, 84, 166), RGB(3, 84, 166),
		RGB(3, 84, 166), CButtonEx::BTEXT_CENTER, FALSE);
	m_btnProgress.SetImage(IDB_TJ_PROGRESS);

	SetCheckProgress(100);
	if (m_bIsOnlyView)
		m_nTimerID = SetTimer(1, 1, NULL);
	else
		m_nTimerID = SetTimer(1, 200, NULL);

// 	tagList_Item Item[] = 
// 	{
// 		{0, TEXT("主服务器"),			 TRUE,  TEXT("")},
// 		{1, TEXT("磁盘空间"),			 TRUE,  TEXT("")},
// 		{2,	TEXT("X:磁盘空间:444.02GB"), TRUE,  TEXT("正常")},
// 		{2,	TEXT("Z:磁盘空间:444.02GB"), FALSE, TEXT("空间不足，推荐进行清理")},
// 		{1,	TEXT("组件状态"),			 TRUE,  TEXT("")},
// 		{2,	TEXT("X:磁盘空间:444.02GB"), FALSE, TEXT("正常")},
// 
// 		{1,	TEXT("磁盘空间"),			 TRUE,  TEXT("")},
// 		{2,	TEXT("X:磁盘空间:444.02GB"), TRUE,  TEXT("正常")},
// 		{2,	TEXT("Z:磁盘空间:444.02GB"), FALSE, TEXT("空间不足，推荐进行清理")},
// 		{2,	TEXT("X:磁盘空间:444.02GB"), TRUE,  TEXT("正常")},
// 		{2,	TEXT("Z:磁盘空间:444.02GB"), FALSE, TEXT("空间不足，推荐进行清理")},
// 
// 		{1,	TEXT("组件状态"),			 TRUE,  TEXT("")},
// 		{2,	TEXT("X:磁盘空间:444.02GB"), TRUE,  TEXT("正常")},
// 		{2,	TEXT("Z:磁盘空间:444.02GB"), FALSE, TEXT("空间不足，推荐进行清理")},
// 		{2,	TEXT("X:磁盘空间:444.02GB"), TRUE,  TEXT("正常")},
// 		{2,	TEXT("Z:磁盘空间:444.02GB"), FALSE, TEXT("空间不足，推荐进行清理")},
// 
// 		{0, TEXT("主服务器"),			 TRUE,  TEXT("")},
// 		{1, TEXT("磁盘空间"),			 TRUE,  TEXT("")},
// 		{2,	TEXT("X:磁盘空间:444.02GB"), TRUE,  TEXT("正常")},
// 		{2,	TEXT("Z:磁盘空间:444.02GB"), FALSE, TEXT("空间不足，推荐进行清理")},
// 		{1,	TEXT("组件状态"),			 TRUE,  TEXT("")},
// 		{2,	TEXT("X:磁盘空间:444.02GB"), FALSE, TEXT("正常")},
// 
// 		{1,	TEXT("磁盘空间"),			 TRUE,  TEXT("")},
// 		{2,	TEXT("X:磁盘空间:444.02GB"), TRUE,  TEXT("正常")},
// 		{2,	TEXT("Z:磁盘空间:444.02GB"), FALSE, TEXT("空间不足，推荐进行清理")},
// 		{2,	TEXT("X:磁盘空间:444.02GB"), TRUE,  TEXT("正常")},
// 		{2,	TEXT("Z:磁盘空间:444.02GB"), FALSE, TEXT("空间不足，推荐进行清理")},
// 
// 		{1,	TEXT("组件状态"),			 TRUE,  TEXT("")},
// 		{2,	TEXT("X:磁盘空间:444.02GB"), TRUE,  TEXT("正常")},
// 		{2,	TEXT("Z:磁盘空间:444.02GB"), FALSE, TEXT("空间不足，推荐进行清理")},
// 		{2,	TEXT("X:磁盘空间:444.02GB"), TRUE,  TEXT("正常")},
// 		{2,	TEXT("Z:磁盘空间:444.02GB"), FALSE, TEXT("空间不足，推荐进行清理")},
// 	};
// 	for (int idx=0; idx<_countof(Item); idx++)
// 	{
// 		m_lstBox.AddLine(&Item[idx]);
// 	}
	MoveWindow(&CRect(0, 0, 539, 567), FALSE);
	CenterWindow();

	return TRUE;
}

void CTJDlg::SetCheckProgress(int nProgress)
{
	if (nProgress > 100)
		nProgress = 100;
	CRect rx;
	rx.left = 26; rx.right = rx.left + int(nProgress * 362 /100.0);
	rx.top  = 61; rx.bottom = rx.top + 19;
	m_btnProgress.MoveWindow(&rx);
}

BOOL CTJDlg::OnEraseBkgnd(CDC* pDC)
{
	//return CDialog::OnEraseBkgnd(pDC);
	CRect rcClient;
	GetClientRect(&rcClient);
	AfxDrawImage(pDC, IDB_TJ_BACKGROUND, rcClient);
	//pDC->FillRect(&rcClient, &CBrush(RGB(255, 255, 255)));

	return TRUE;
}

LRESULT CTJDlg::OnNcHitTest(CPoint point)
{
	UINT nHitTest = CDialog::OnNcHitTest(point);
	if (nHitTest == HTCLIENT)
		return HTCAPTION;
	return nHitTest;
}

HBRUSH CTJDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	static CBrush brush(RGB(237,249,255));
	static CBrush brushWhite(RGB(255, 255, 255));
	if (pWnd->GetSafeHwnd() == m_staTitle.GetSafeHwnd())
	{
		pDC->SetBkMode(1);
		pDC->SetTextColor(RGB(24, 81, 136));
		return brush;
	}
	if (pWnd->GetSafeHwnd() == m_staStatus.GetSafeHwnd())
	{
		pDC->SetBkMode(1);
		pDC->SetTextColor(RGB(0, 0, 0));
		return brush;
	}
	
	if (nCtlColor == CTLCOLOR_LISTBOX)
	{
		pDC->SetBkMode(1);
		return brushWhite;
	}
	return hbr;
}

void CTJDlg::OnBnClickedButton1()
{
	if (m_nTimerID)
	{
		KillTimer(m_nTimerID);
		m_nTimerID = 0;
	}
	OnCancel();
}

void CTJDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (!IsWindow(m_lstBox.GetSafeHwnd()))
		return ;

	CRect rcClient, rx;
	GetClientRect(&rcClient);

	rx.left = 25; rx.top = 12;
	rx.right = rx.left + 100; rx.bottom = rx.top + 16;
	m_staTitle.MoveWindow(&rx);

	rx.left = 25; rx.top = 40;
	rx.right = rx.left + 400;
	rx.bottom = rx.top + 12;
	m_staStatus.MoveWindow(&rx);

	rx.left = rcClient.right - 130;
	rx.right = rx.left + 112;
	rx.top = 58; rx.bottom = rx.top + 23;
	m_btnCancel.MoveWindow(&rx);

	rx = rcClient;
	rx.top = 105; rx.bottom -= 60;
	rx.left += 2; rx.right -= 2;
	m_lstBox.MoveWindow(&rx);

	rx.top = rcClient.bottom - 40;
	rx.bottom = rx.top + 23;
	rx.left = (rcClient.Width() - 112)/2;
	rx.right = rx.left + 112;
	m_btnClose.MoveWindow(&rx);
}

void CTJDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialog::OnTimer(nIDEvent);

	if (m_bIsOnlyView)
		while (!m_pMonitorPage->PerformTJ(this)) {}
	else 
		m_pMonitorPage->PerformTJ(this);
}

void CTJDlg::OnBnClickedButton2()
{
	if (m_nTimerID)
	{
		KillTimer(m_nTimerID);
		m_nTimerID = 0;
	}
	OnCancel();
}

void CTJDlg::TJOver(int nTJ)
{
	CString strMessage;

	if (m_bIsOnlyView)
		strMessage.Format("当前健康指数为:%d分", nTJ);
	else
		strMessage.Format("体检完毕，当前健康指数为:%d分", nTJ);

	SetCheckStatusText(strMessage);
	SetCheckProgress(100);

	if (m_nTimerID)
	{
		KillTimer(m_nTimerID);
		m_nTimerID = 0;
	}
}

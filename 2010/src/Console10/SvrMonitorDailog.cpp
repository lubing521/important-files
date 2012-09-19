// SvrMonitorDailog.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "SvrMonitorDailog.h"

#include "i8logmsg.h"

// CSvrMonitorDailog 对话框

IMPLEMENT_DYNAMIC(CSvrMonitorDailog, CDialog)

CSvrMonitorDailog::CSvrMonitorDailog(CWnd* pParent /*=NULL*/)
	: CDialog(CSvrMonitorDailog::IDD, pParent)
	, m_nMaxLogNum(1024*10)
{
}

CSvrMonitorDailog::~CSvrMonitorDailog()
{
}

void CSvrMonitorDailog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SVR_LOG, m_LogView);
}


BEGIN_MESSAGE_MAP(CSvrMonitorDailog, CDialog)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SVR_LOG, &CSvrMonitorDailog::OnLvnCustomDrawSvrLog)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_SVR_LOG, &CSvrMonitorDailog::OnLvnGetdispinfoSvrLog)
	ON_WM_CLOSE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


void CSvrMonitorDailog::AddLog(i8desk::PluginLogRecord *log)
{
	m_logs.push_back(LogRecord_Ptr(log));
	if (m_logs.size() >= m_nMaxLogNum)
		m_logs.pop_front();

	m_LogView.SetItemCount(m_logs.size());
	m_LogView.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	// 去掉自动弹出
	/*if (log->type >= LM_WARNING)
		this->ShowWindow(SW_SHOW);*/
}

// CSvrMonitorDailog 消息处理程序

BOOL CSvrMonitorDailog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_LogView.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	m_LogView.InsertColumn(0, _T("组件"), LVCFMT_CENTER, 60) ;
	m_LogView.InsertColumn(1, _T("时间"), LVCFMT_CENTER, 80) ;
	m_LogView.InsertColumn(2, _T("内容"), LVCFMT_LEFT, 600) ;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSvrMonitorDailog::OnSize(UINT nType, int cx, int cy)
{
	RECT rc;
	GetClientRect(&rc);
	m_LogView.MoveWindow(&rc);
}

void CSvrMonitorDailog::OnLvnCustomDrawSvrLog( NMHDR *pNmHdr, LRESULT *pResult )
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNmHdr );

	*pResult = CDRF_DODEFAULT;

	if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
	{
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
	else if ( (CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage )
	{
		int nItem = static_cast<int>( pLVCD->nmcd.dwItemSpec );
		ASSERT(nItem < (int)m_logs.size());

		LogRecord_Ptr lr = m_logs[nItem];
		ASSERT(lr);

		COLORREF clrText, clrNewBkColor;

		switch (lr->type) {
		default:	
			break;
		case LM_DEBUG:	 
			clrText = RGB(0, 0, 0);	
			break;
		case LM_INFO:	 
			clrText = RGB(0, 0, 0);	
			break;
		case LM_NOTICE:	 
			clrText = RGB(0, 0, 0);	
			break;
		case LM_WARNING: 
			clrText = RGB(128, 128, 0);	
			break;
		case LM_ERROR:	
			clrText = RGB(255, 0, 0);	
			break;
		}

		//设置背景色
		if ( nItem % 2 == 0 ) {
			clrNewBkColor = RGB( 240, 240, 240 ); //偶数行背景色为灰色
		} else {
			clrNewBkColor = RGB( 255, 255, 255 ); //奇数行背景色为白色
		}

		pLVCD->clrText = clrText;
		pLVCD->clrTextBk = clrNewBkColor;

		*pResult = CDRF_DODEFAULT;
	}
}

void CSvrMonitorDailog::OnLvnGetdispinfoSvrLog(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	
	LV_ITEM* pItem = &pDispInfo->item;
	ASSERT(pItem->iItem < (int)m_logs.size());

	LogRecord_Ptr lr = m_logs[pItem->iItem];
	ASSERT(lr);

	if (pItem->mask & LVIF_TEXT)
	{
		switch (pItem->iSubItem)
		{
		case 0:
			lstrcpyn(pItem->pszText, lr->plugin, pItem->cchTextMax);
			break;
		case 1: {
			time_t t = lr->time;
			if (tm *tm = ::localtime(&t)) {
				::_stprintf_s(pItem->pszText, pItem->cchTextMax,
					_T("%02u:%02u:%02u"),
					tm->tm_hour, tm->tm_min, tm->tm_sec); 
			} else {
				::_stprintf_s(pItem->pszText, pItem->cchTextMax,
					_T("%02u:%02u:%02u"),
					0, 0, 0); 
			}
			break; }
		case 2:
			lstrcpyn(pItem->pszText, lr->data, pItem->cchTextMax);
			break;
		}
	} 
	
	if (pItem->mask & LVIF_IMAGE)
	{
	}
}

void CSvrMonitorDailog::OnClose()
{
	this->ShowWindow(SW_HIDE);
//	CDialog::OnClose();
}

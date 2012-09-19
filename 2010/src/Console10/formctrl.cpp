#include "stdafx.h"
#include "resource.h"
#include "formctrl.h"

#define ID_BASE	0x1000

CFormCtrl::CFormCtrl(UINT nIDTemplate)
: CFormView(nIDTemplate)
{
	LOGFONT lf = {0};
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);
	m_font.CreateFontIndirect(&lf);

	lf.lfWeight = FW_BOLD;
	m_fontbold.CreateFontIndirect(&lf);

	lf.lfWeight = FW_NORMAL;
	lf.lfUnderline = TRUE;
	m_fontunderline.CreateFontIndirect(&lf);

	m_nJKZS = 0;
	m_SvrCount = 0;
	m_nCurSvr = 0;

	ZeroMemory(&m_pServer,  sizeof(m_pServer));
	ZeroMemory(&m_cliInfo,  sizeof(m_cliInfo));
	ZeroMemory(&m_gameInfo, sizeof(m_gameInfo));
}

CFormCtrl::~CFormCtrl()
{
}

BEGIN_MESSAGE_MAP(CFormCtrl, CFormView)
	ON_WM_DESTROY()
	ON_WM_MOUSEACTIVATE()
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_COMMAND_RANGE(ID_BTN_MIN + ID_BASE, ID_BTN_MAX + ID_BASE, OnClickButton)
END_MESSAGE_MAP()

BOOL CFormCtrl::Create(CWnd* parent)
{
	BOOL bCreated = CFormView::Create(NULL, NULL, WS_VISIBLE|WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
		CRect(0, 0, 0, 0), parent, 1, NULL);
	
	m_btnCXTJ.Create(TEXT("重新体验"), WS_VISIBLE|WS_CHILD, CRect(0, 0, 0, 0), this, ID_BASE + ID_JKZS_CXTJ);
	m_btnViewTJ.Create(TEXT("查看检测报告"), WS_VISIBLE|WS_CHILD, CRect(0, 0, 0, 0), this, ID_BASE + ID_JKZS_VIEW);
	m_btnCXTJ.SetImage(IDB_BTN_BK);
	m_btnCXTJ.SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(3, 84, 166), RGB(3, 84, 166), RGB(3, 84, 166), 
		RGB(3, 84, 166), CButtonEx::BTEXT_CENTER, TRUE);
	m_btnViewTJ.SetImage(IDB_BTN_BK);
	m_btnViewTJ.SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(3, 84, 166), RGB(3, 84, 166), RGB(3, 84, 166), 
		RGB(3, 84, 166), CButtonEx::BTEXT_CENTER, TRUE);

	for (int idx=0; idx<_countof(m_btnServer); idx++)
	{
		m_btnServer[idx].Create(TEXT(""), WS_VISIBLE|WS_CHILD, CRect(0, 0, 0, 0), this, idx+ID_BASE + ID_SERVER_SVR1);
		m_btnServer[idx].SetTextStyle(DEFAULT_FONT_NAME, 12,  RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), 
			RGB(0, 0, 0), CButtonEx::BTEXT_CENTER, FALSE);
		m_btnServer[idx].SetImage(idx==0 ? IDB_TAB_WIDTH_UNSEL : IDB_TAB_UNSEL,
			0, 0, idx==0 ? IDB_TAB_WIDTH_SEL : IDB_TAB_SEL);
	}

	for (int idx=0; idx<_countof(m_btnCliStatus); idx++)
	{
		m_btnCliStatus[idx].Create(TEXT(""), WS_VISIBLE|WS_CHILD, CRect(0, 0, 0, 0), this, idx+ID_BASE + ID_CLIENT_CLITOTAL);
		m_btnCliStatus[idx].SetWindowText(TEXT("查看"));
		m_btnCliStatus[idx].SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(94,160,0), RGB(255,156,0), 
			RGB(94,160,0), RGB(94,160,0), CButtonEx::BTEXT_CENTER, FALSE);
	}
	for (int idx=0; idx<_countof(m_btnGameStatus); idx++)
	{
		m_btnGameStatus[idx].Create(TEXT(""), WS_VISIBLE|WS_CHILD, CRect(0, 0, 0, 0), this, idx+ID_BASE + ID_GAME_CENTERNUM);
		m_btnGameStatus[idx].SetWindowText(TEXT("查看"));
		m_btnGameStatus[idx].SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(94,160,0), RGB(255,156,0), 
			RGB(94,160,0), RGB(94,160,0), CButtonEx::BTEXT_CENTER, FALSE);
	}

	SetScrollSizes(MM_TEXT, CSize(800, 520));

	return bCreated;
}

HBRUSH CFormCtrl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);

	static CBrush brush(RGB(255, 255, 255));
	if (nCtlColor == CTLCOLOR_DLG)
	{
		pDC->SetBkMode(1);
		pDC->SetBkColor(RGB(255, 255, 255));
		return brush;
	}
	return hbr;
}

void CFormCtrl::OnDraw(CDC* pDC)
{
	pDC->SetBkMode(1);
	PaitJKZS(pDC);
	PaitServer(pDC);
	PaitClient(pDC);
	PaitGame(pDC);	
}

void CFormCtrl::PostNcDestroy()
{
}

void CFormCtrl::OnDestroy()
{
	CWnd::OnDestroy();
}

int CFormCtrl::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

void CFormCtrl::UpdateJKZS(int nJK, const CString& strLastBackupTime)
{
	m_nJKZS = nJK;
	m_strLastBackupTime = strLastBackupTime;
	CClientDC dc(this);
	OnPrepareDC(&dc);
	UpdateJKZS(&dc);
}

void CFormCtrl::UpdateServer(tagServer* pServer, int nCount)
{
	//如果没有变则，不更新
	{
		if (m_SvrCount == nCount && memcmp(pServer, &m_pServer, nCount * sizeof(tagServer)) == 0)
			return ;
	}

	if (nCount > _countof(m_pServer))
		nCount = _countof(m_pServer);

	CClientDC dc(this);
	ZeroMemory(&m_pServer, sizeof(m_pServer));
	memcpy(&m_pServer, pServer, nCount*sizeof(tagServer));
	m_SvrCount = nCount;
	
	if (m_nCurSvr >= nCount)
		m_nCurSvr = 0;
	OnPrepareDC(&dc);
	UpdateServer(&dc);
}

void CFormCtrl::UpdateClient(tagClientInfo* pInfo)
{
	if (memcmp(pInfo, &m_cliInfo, sizeof(m_cliInfo)) == 0)
		return ;
	
	m_cliInfo = *pInfo;

	CClientDC dc(this);
	OnPrepareDC(&dc);
	UpdateClient(&dc);
}

void CFormCtrl::UpdateGameInfo(tagGameInfo* pInfo)
{
	if (memcmp(pInfo, &m_gameInfo, sizeof(m_gameInfo)) == 0)
		return ;

	m_gameInfo = *pInfo;

	CClientDC dc(this);
	OnPrepareDC(&dc);
	UpdateGame(&dc);
}


void  CFormCtrl::PaitJKZS(CDC* pDC)
{
	CRect rcClient, rx;
	GetClientRect(&rcClient);
	
	//绘背景图
	rx = rcClient; 
	rx.DeflateRect(5, 5);
	rx.bottom = rx.top + 30;
	rx.right = rx.left + 28;
	AfxDrawImage(pDC, IDB_JK_LEFT, rx);

	rx.left = rx.right; rx.right = rcClient.right - 12 - 5;
	AfxDrawImage(pDC, IDB_JK_1P, rx);

	rx.left = rx.right; rx.right = rcClient.right - 5;
	AfxDrawImage(pDC, IDB_JK_RIGHT, rx);

	rx.top += 8; rx.bottom -= 8;
	rx.left = rcClient.right - 240; rx.right = rx.left + 14;
	AfxDrawImage(pDC, IDB_JK_TIME, rx);
	rx.top -= 8; rx.bottom += 8;

	pDC->SelectObject(&m_fontbold);
	rx.left = 35; rx.right = 95;
	pDC->DrawText(TEXT("健康指数: "), -1, &rx, DT_SINGLELINE|DT_VCENTER|DT_LEFT);

	UpdateJKZS(pDC);
}

void  CFormCtrl::PaitServer(CDC* pDC)
{
	int sTop  = 45;
	int sLeft =  5;

	CRect rcClient, rx;
	GetClientRect(&rcClient);

	rx.left = 10; rx.right = rx.left + 16;
	rx.top = sTop + 5; rx.bottom = rx.top + 16;
	AfxDrawImage(pDC, IDB_SVR_STATUS, rx);	

	rx.left = 30; rx.top = 45;
	rx.right = rx.left + 80;
	rx.bottom = rx.top + 27;
	CFont* pOldFont = pDC->SelectObject(&m_fontbold);
	pDC->DrawText(TEXT("服务器状态"), -1, &rx, DT_SINGLELINE|DT_VCENTER|DT_LEFT);

	for (int idx=0; idx<_countof(m_pServer); idx++)
	{
		m_btnServer[idx].ShowWindow(idx < m_SvrCount ? SW_SHOW : SW_HIDE);
		m_btnServer[idx].SetActive(m_nCurSvr == idx);
	}

	//绘表格
	int nTop = rx.bottom;
	rx = CRect(sLeft, nTop, rcClient.right-5, nTop + 165);
	CPen pen(PS_SOLID, 2, RGB(121, 121, 121));
	CPen* pOldPen = pDC->SelectObject(&pen);
	pDC->Draw3dRect(&rx, RGB(108,108,108), RGB(121,121,121));
	{
		CPen pen(PS_SOLID, 2, RGB(108,108,108));
		pDC->SelectObject(&pen);
		pDC->MoveTo(rx.left, rx.top+1);
		pDC->LineTo(rx.right-1, rx.top+1);	
	}

	CPen pen2(PS_SOLID, 1, RGB(229, 229, 229));
	CPoint ptFrom(sLeft+1, nTop+30);
	CPoint ptTo(rcClient.right-6, nTop+30);

	pDC->SelectObject(pen2);
	for (int idx=0; idx<3; idx++)
	{
		pDC->MoveTo(ptFrom); 
		pDC->LineTo(ptTo);

		ptFrom.Offset(0, 30);
		ptTo.Offset(0, 30);
	}
	{
		ptFrom = CPoint(rcClient.Width()/2, nTop + 165 - 3*24-2);
		ptTo = CPoint(rcClient.Width()/2, nTop + 165 - 1);
		pDC->MoveTo(ptFrom);
		pDC->LineTo(ptTo);
	}

	ptFrom = CPoint(sLeft, nTop+2);
	ptTo   = CPoint(sLeft, nTop + 60);
	rx = CRect(ptFrom, CPoint(sLeft, nTop+30));
	CString str[] = {TEXT("主服务器状态"), TEXT("三层更新"), TEXT("虚拟盘"), TEXT("内网更新"), TEXT("商城收银端"), TEXT("CPU占用率"), TEXT("内存使用率")};
	for (int idx=0; idx<_countof(str); idx++)
	{
		int offset = (rcClient.right-5 - sLeft) / _countof(str);
		rx.right = rx.left + offset;

		ptFrom.Offset(offset, 0);
		ptTo.Offset(offset, 0);

		if (idx<_countof(str) - 1)
		{
			pDC->MoveTo(ptFrom);
			pDC->LineTo(ptTo);
		}

		pDC->SetTextColor(RGB(60,60,60));
		pDC->DrawText(str[idx], -1, &rx, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
		rx.OffsetRect(rx.Width(), 0);
	}

	{
		rx = CRect(sLeft+1, sTop+87+1, rcClient.right-6, sTop+117);
		AfxDrawImage(pDC, IDB_TBL_1PIX, rx);

		rx.top = sTop + 87 + 7; rx.bottom = rx.top + 16;
		rx.left+= 5; rx.right = rx.left + 16;
		AfxDrawImage(pDC, IDB_DISK_ICON, rx);

		rx.top -= 7; rx.bottom = rx.top + 30;
		rx.left = sLeft+25; rx.right = rx.left + 200;
		pDC->SetTextColor(RGB(0, 0, 0));
		pDC->DrawText(TEXT("磁盘使用情况"), -1, &rx, DT_SINGLELINE|DT_VCENTER|DT_LEFT);
	}

	UpdateServer(pDC);
}

void  CFormCtrl::PaitClient(CDC* pDC)
{
	int sTop  = (45 + 203);
	int sLeft =  5;

	CRect rcClient, rx;
	GetClientRect(&rcClient);

	CPen pen(PS_SOLID, 1, RGB(122, 122, 122));
	CPen*  pOlePen  = pDC->SelectObject(&pen);

	rx = CRect(sLeft, sTop, rcClient.right-5, sTop + 90);
	pDC->Draw3dRect(&rx, RGB(108,108,108), RGB(121,121,121));
	{
		CPen pen(PS_SOLID, 2, RGB(108,108,108));
		pDC->SelectObject(&pen);
		pDC->MoveTo(rx.left, rx.top);
		pDC->LineTo(rx.right-1, rx.top);	
	}
	
	{
		CPen pen(PS_SOLID, 1, RGB(229, 229, 229));
		pDC->SelectObject(&pen);
		CPoint ptFrom(sLeft+1, sTop+54);
		CPoint ptTo(rcClient.right-6, sTop + 54);

		pDC->MoveTo(ptFrom);
		pDC->LineTo(ptTo);

		rx = CRect(sLeft+1, sTop+1, rcClient.right-6, sTop+30);
		AfxDrawImage(pDC, IDB_TBL2_1PIX, rx);

		rx.top = sTop + 7; rx.bottom = rx.top + 16;
		rx.left+= 5; rx.right = rx.left + 16;
		AfxDrawImage(pDC, IDB_CLI_ICON, rx);

		rx.top -= 7; rx.bottom = rx.top + 30;
		rx.left = 25; rx.right = rx.left + 80;
		pDC->SetTextColor(RGB(72,102,4));
		CFont* pOldFont = pDC->SelectObject(&m_fontbold);
		pDC->DrawText(TEXT("客户机状态"), -1, &rx, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
	}

	pDC->SelectObject(&m_font);
	for (int idx=0; idx<7; idx++)
	{
		int width = (rcClient.right - 5 - sLeft ) / 7;
		{
			pDC->SetTextColor(RGB(0, 0, 0));
			CString str[][7] = 
			{
				{TEXT("客户机总数"), TEXT("在线客户机"), TEXT("还原未启用数"), TEXT("安全中心未启用数"), TEXT("IE保护未开启"), TEXT("防狗驱动未启用"), TEXT("硬件改动数")},
				{TEXT("查看"), TEXT("查看"), TEXT("查看"), TEXT("查看"), TEXT("查看"), TEXT("查看"), TEXT("查看")}
			};

			pDC->SetTextColor(RGB(0, 0, 0));
			CRect rx;
			rx.top = sTop + 30;
			rx.bottom = rx.top + 20;
			rx.left = sLeft + idx * width;
			rx.right = rx.left + width;
			pDC->SelectObject(&m_font);
			pDC->DrawText(str[0][idx], -1, &rx, DT_SINGLELINE|DT_VCENTER|DT_CENTER);

// 			rx.OffsetRect(0, rx.Height()*2);
// 			rx.bottom = rx.top + 16;
// 			pDC->SetTextColor(RGB(94,160,0));
// 			pDC->SelectObject(&m_fontunderline);
// 			pDC->DrawText(str[1][idx], -1, &rx, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
		}

		if (idx>0)
		{
			CPoint ptFrom(sLeft + idx*width, sTop + 30);
			CPoint ptTo(sLeft + idx*width, sTop+90-1);
			pDC->MoveTo(ptFrom);
			pDC->LineTo(ptTo);
		}
	}

	UpdateClient(pDC);
}

void  CFormCtrl::PaitGame(CDC* pDC)
{
	int sTop  = (45 + 203 + 100);
	int sLeft =  5;

	CRect rcClient, rx;
	GetClientRect(&rcClient);

	CFont* pOldFont = pDC->SelectObject(&m_font);
	rx = CRect(sLeft, sTop, rcClient.right-5, sTop + 30 + 6*24);
	pDC->Draw3dRect(&rx, RGB(108,108,108), RGB(121,121,121));
	{
		CPen pen(PS_SOLID, 2, RGB(108,108,108));
		pDC->SelectObject(&pen);
		pDC->MoveTo(rx.left, rx.top);
		pDC->LineTo(rx.right-1, rx.top);	
	}

	{
		rx = CRect(sLeft+1, sTop+1, rcClient.right-6, sTop+30);
		AfxDrawImage(pDC, IDB_TBL2_1PIX, rx);

		rx.top = sTop + 7; rx.bottom = rx.top + 16;
		rx.left = sLeft + 5; rx.right = rx.left + 16;
		AfxDrawImage(pDC, IDB_GAME_ICON, rx);

		rx.top -= 7; rx.bottom = rx.top + 30;
		rx.left = sLeft + 30; rx.right = rx.left + 120;
		pDC->SetTextColor(RGB(72,102,4));
		pDC->SelectObject(&m_fontbold);
		pDC->DrawText(TEXT("游戏状态监控"), -1, &rx, DT_SINGLELINE|DT_VCENTER|DT_LEFT);
	}

	{
		CPen pen(PS_SOLID, 1, RGB(229, 229, 229));
		CPen*  pOlePen  = pDC->SelectObject(&pen);
		pDC->SelectObject(&m_font);
		CString str[6][2] = 
		{
			{TEXT("中心资源数"),					TEXT("匹配到中心的资源")},
			{TEXT("近期中心新增资源(未下载)"),		TEXT("有更新的资源")},
			{TEXT("本地己有资源"),					TEXT("三层更新队列")},
			{TEXT("配置为虚拟盘运行的资源"),		TEXT("三层下载速度")},
			{TEXT("配置为本地更新运行的资源"),		TEXT("内网更新队列")},
			{TEXT("配置为不更新直接运行的资源"),	TEXT("内网更新速度")}
		};
		for (int idx=0; idx<6; idx++)
		{
			CPoint ptFrom(sLeft+1, sTop + 54 + idx*24);
			CPoint ptTo(rcClient.right-6, sTop + 54 + idx*24);

			if (idx<5)
			{
				pDC->MoveTo(ptFrom);
				pDC->LineTo(ptTo);
			}

			{
				int nWidth = rcClient.Width() - 2 * sLeft;

				pDC->SelectObject(&m_font);
				CRect rx(sLeft+5, sTop + 30 + idx*24, sLeft + 5 + (int)(nWidth*0.25), sTop + 54 + idx*24);
				pDC->SetTextColor(RGB(0, 0, 0));
				pDC->DrawText(str[idx][0], -1, &rx, DT_SINGLELINE|DT_VCENTER|DT_LEFT);

				

				pDC->SelectObject(&m_font);
				rx.OffsetRect(nWidth/2, 0);
				pDC->SetTextColor(RGB(0, 0, 0));
				pDC->DrawText(str[idx][1], -1, &rx, DT_SINGLELINE|DT_VCENTER|DT_LEFT);
			}
		}

		{
			CPoint ptFrom(rcClient.Width()/2, sTop + 30);
			CPoint ptTo(rcClient.Width()/2, sTop + 30 + 6*24);
			pDC->MoveTo(ptFrom);
			pDC->LineTo(ptTo);
		}
	}

	UpdateGame(pDC);
}

void CFormCtrl::UpdateJKZS(CDC* pDC)
{
	COLORREF clr = pDC->SetBkColor(RGB(255, 246, 207));

	CRect rx(95, 14, 95+25, 26);
	CString szText;
	szText.Format(TEXT("%d"), m_nJKZS);
	pDC->SelectObject(&m_font);
	pDC->SetTextColor(RGB(255, 0, 0));
	pDC->SelectObject(&m_font);
	pDC->FillRect(&rx, &CBrush(RGB(255, 246, 207)));
	pDC->DrawText(szText, -1, &rx, DT_SINGLELINE|DT_VCENTER|DT_LEFT);

	CRect rcClient;
	GetClientRect(&rcClient);
	rx.left = rcClient.right - 240 + 18; rx.right = rcClient.right - 13;
	pDC->SetTextColor(RGB(0, 0, 0));
	szText.Format(TEXT("上一次体验时间: %s"), (LPCTSTR)m_strLastBackupTime);
	pDC->DrawText(szText, &rx, DT_SINGLELINE|DT_VCENTER|DT_LEFT);

	pDC->SetBkColor(clr);
}

static LPCTSTR GetModuleStatusDesc(int status, DWORD& dwIcon, COLORREF* pclr = NULL)
{
	dwIcon = 0;

	if (pclr != NULL)
		*pclr = RGB(94,160,2);
	if (status == 1) 
	{
		dwIcon = IDB_ICON_NORMAL;
		return  _T("正常");
	}
	else if (status == -1)
	{
		*pclr = RGB(255, 0, 0);
		return _T("异常");
	}
	else if (status == 2) 
		return _T("未安装");
	else if (status == 3) 
		return _T("未启用");

	return _T("未启用");
}

void CFormCtrl::UpdateServer(CDC* pDC)
{
	const int column = 7;
	const int sTop  = 102;
	const int sLeft =  5;
	
	for (int idx=0; idx<_countof(m_pServer); idx++)
	{
		m_btnServer[idx].ShowWindow(idx < m_SvrCount ? SW_SHOW : SW_HIDE);
		m_btnServer[idx].SetActive(m_nCurSvr == idx);
		CString strNew = _bstr_t(m_pServer[idx].HostName);
		if (idx == 0)
		{
			strNew += TEXT("(主服务器)");
		}
		CString strOld;
		m_btnServer[idx].GetWindowText(strOld);
		if (strNew != strOld)
			m_btnServer[idx].SetWindowText(strNew);
		COLORREF clr = idx == m_nCurSvr ? RGB(255, 255, 255) : RGB(0, 0, 0);
		m_btnServer[idx].SetTextStyle(DEFAULT_FONT_NAME, 12, clr, clr, clr, clr, CButtonEx::BTEXT_CENTER, FALSE);
	}

	CRect rx, rcClient;
	GetClientRect(&rcClient);

	//绘服务器各种状态
	{
		struct _pair
		{
			DWORD Icon;
			TCHAR Text[MAX_PATH];
			COLORREF Color;
		};
		_pair element[7] = {0};
		_stprintf(element[0].Text, TEXT("%s"), 
			GetModuleStatusDesc(m_pServer[m_nCurSvr].MainServer, element[0].Icon, &element[0].Color));
		_stprintf(element[1].Text, TEXT("%s"), 
			GetModuleStatusDesc(m_pServer[m_nCurSvr].ThirdLayerUpdate, element[1].Icon, &element[1].Color));
		_stprintf(element[2].Text, TEXT("%s"), 
			GetModuleStatusDesc(m_pServer[m_nCurSvr].VirtualDisk, element[2].Icon, &element[2].Color));
		_stprintf(element[3].Text, TEXT("%s"), 
			GetModuleStatusDesc(m_pServer[m_nCurSvr].InnerUpdate, element[3].Icon, &element[3].Color));
		_stprintf(element[4].Text, TEXT("%s"), 
			GetModuleStatusDesc(m_pServer[m_nCurSvr].ShopCity, element[4].Icon, &element[4].Color));
		_stprintf(element[5].Text, TEXT("%d%%"), m_pServer[m_nCurSvr].cpuusage);
		_stprintf(element[6].Text, TEXT("%d%%"), m_pServer[m_nCurSvr].memoryusage);
		element[5].Color = element[6].Color = RGB(94,160,2);
		
		int colwidth = (rcClient.right-5 - sLeft) / column;
		rx = CRect(sLeft, sTop, sLeft + colwidth, sTop + 30);
		pDC->SelectObject(&m_fontbold);
		for (int idx=0; idx<column; idx++)
		{
			CRect rt = rx;
			rt.DeflateRect(1, 1, 1, 1);
			pDC->FillRect(&rt, &CBrush(RGB(255, 255, 255)));

			rt = rx;
			if (element[idx].Icon != 0)
			{
				rt.left += rx.Width() / 2 - 20;
				rt.right = rt.left + 15;
				rt.top += 7; rt.bottom -= 7;
				AfxDrawImage(pDC, element[idx].Icon, rt);
			
				rt.left += 20;
				rt.right = rt.left + 40;
				pDC->SetTextColor(element[idx].Color);
				pDC->DrawText(element[idx].Text, -1, &rt, DT_SINGLELINE|DT_VCENTER|DT_LEFT);
			}
			else
			{
				rt = rx;
				pDC->SetTextColor(element[idx].Color);
				pDC->DrawText(element[idx].Text, -1, &rt, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
			}
			
			rx.OffsetRect(rx.Width(), 0);
		}
	}
	
	//绘磁盘使用情况
	while (1)
	{
		CRect rx (sLeft+1, sTop + 61, rcClient.right-7, sTop + 61 + 72);
		pDC->FillRect(&rx, &CBrush(RGB(255, 255, 255)));

		tagDisk* pDisk = m_pServer[m_nCurSvr].Disk;
		if (pDisk[0].Driver == 0)
			break;
		CRect rx1 = CRect(sLeft, sTop + 60, sLeft + (rcClient.Width() - 10)/ 2, sTop + 60 + 24);
		UpdateDiskUsage(pDC, rx1, pDisk[0].Driver, pDisk[0].total, pDisk[0].userd);
		
		if (pDisk[1].Driver == 0)
			break;

		int nLeft = rcClient.left + rcClient.Width() / 2 + sLeft;
		CRect rx2 = CRect(nLeft, sTop + 60, rcClient.right - 5, sTop + 60 + 24);
		UpdateDiskUsage(pDC, rx2,pDisk[1].Driver, pDisk[1].total, pDisk[1].userd);

		if (pDisk[2].Driver == 0)
			break;
		rx1.OffsetRect(0, rx1.Height());
		UpdateDiskUsage(pDC, rx1, pDisk[2].Driver, pDisk[2].total, pDisk[2].userd);

		if (pDisk[3].Driver == 0)
			break;
		rx2.OffsetRect(0, rx2.Height());
		UpdateDiskUsage(pDC, rx2, pDisk[3].Driver, pDisk[3].total, pDisk[3].userd);

		if (pDisk[4].Driver == 0)
			break;
		rx1.OffsetRect(0, rx1.Height());
		UpdateDiskUsage(pDC, rx1,pDisk[4].Driver, pDisk[4].total, pDisk[4].userd);

		if (pDisk[5].Driver == 0)
			break;
		rx2.OffsetRect(0, rx2.Height());
		UpdateDiskUsage(pDC, rx2,pDisk[5].Driver, pDisk[5].total, pDisk[5].userd);
		
		break;
	}
}

void CFormCtrl::UpdateDiskUsage(CDC* pDC, const CRect& rx, TCHAR chDrv, float total, float used)
{
	CRect rc;

	//盘符
	CString str;
	str.Format(TEXT("%C:"), chDrv);
	rc = rx; rc.left += 5; rc.right = rc.left + 16;
	pDC->SetTextColor(RGB(0, 0, 0));
	pDC->SelectObject(&m_font);
	pDC->DrawText(str, -1, &rc, DT_SINGLELINE|DT_VCENTER|DT_LEFT);

	//进度条
	rc.top += 5; rc.bottom -= 5;
	rc.left = rc.right; rc.right = rx.right - 290;
	AfxDrawImage(pDC, IDB_PRG_BK, rc);

	rc.top += 2; rc.bottom -= 2;
	int progress = int(100 * used / total);
	rc.left += 2;
	rc.right = rc.left + long((rc.Width()-4)* progress /100.0);

	DWORD dwId = (progress > 80) ? IDB_PRG_RED : 
		(progress > 50 ? IDB_PRG_ORG : IDB_PRG_GREEN);
	AfxDrawImage(pDC, dwId, rc);

	//文本说明
	rc = rx;
	rc.left = rx.right - 280;
	str.Format(TEXT("总容量:%5.2fG,己使用%5.2fG, 空闲:%5.2fG"),
		total, used, total - used);
	pDC->DrawText(str, -1, &rc, DT_SINGLELINE|DT_VCENTER|DT_LEFT);
}

void CFormCtrl::UpdateClient(CDC* pDC)
{
	int sTop  = (45 + 203);
	int sLeft =  5;

	CRect rcClient;
	GetClientRect(&rcClient);

	CString str[7];
	str[0].Format(TEXT("%d台"), m_cliInfo.CliNum);
	str[1].Format(TEXT("%d台"), m_cliInfo.CliOnlineNum);
	str[2].Format(TEXT("%d台"), m_cliInfo.NotProtNum);
	str[3].Format(TEXT("%d台"), m_cliInfo.NotSafeNum);
	str[4].Format(TEXT("%d台"), m_cliInfo.NotIeNum);
	str[5].Format(TEXT("%d台"), m_cliInfo.NotDogNum);
	str[6].Format(TEXT("%d台"), m_cliInfo.HardChgNum);

	pDC->SelectObject(&m_font);
	for (int idx=0; idx<7; idx++)
	{
		int width = (rcClient.right - 5 - sLeft ) / 7;
		{
			CRect rx;
			rx.top = sTop + 54;
			rx.bottom = rx.top + 16;
			rx.left = sLeft + idx * width;
			rx.right = rx.left + width;
			rx.DeflateRect(1, 1, 1, 1);
			pDC->FillRect(&rx, &CBrush(RGB(255, 255, 255)));
			pDC->SetTextColor(RGB(0, 0, 0));
			pDC->DrawText(str[idx], -1, &rx, DT_SINGLELINE|DT_BOTTOM|DT_CENTER);
		}
	}
}

void CFormCtrl::UpdateGame(CDC* pDC)
{
	int sTop  = (45 + 203 + 100);
	int sLeft =  5;
	CRect rcClient;
	GetClientRect(&rcClient);

	pDC->SelectObject(&m_font);
	CString str[12] =
	{
		m_gameInfo.CGameNum, 
		m_gameInfo.CAddGameNum,
		m_gameInfo.LGameNum,
		m_gameInfo.ConVirRunGameNum,
		m_gameInfo.ConLocRunGameNum,
		m_gameInfo.ConRunNotUptGameNum,
		
		m_gameInfo.MatchGameNum,
		m_gameInfo.HaveUptGameNum,
		m_gameInfo.ThirdDwnListNum,
		m_gameInfo.ThirdDwnSpeed,
		m_gameInfo.InnerUptListNum,
		m_gameInfo.InnerUptSpeed
	};
	for (int idx=0; idx<6; idx++)
	{
		int nWidth = rcClient.Width() - 2 * sLeft;
		CRect rx(sLeft + 5 + int(nWidth * 0.25), sTop + 30 + idx*24, sLeft + 5 + int(nWidth * 0.44), sTop + 54 + idx*24);
		{
			CRect rc(rx);
			rc.DeflateRect(1, 1, 1, 1);
			pDC->SetTextColor(RGB(44, 44, 44));
			pDC->FillRect(&rc, &CBrush(RGB(255, 255, 255)));
			pDC->DrawText(str[idx], -1, &rx, DT_SINGLELINE|DT_VCENTER|DT_LEFT);
		}
		
		rx.OffsetRect(nWidth/2, 0);
		{
			CRect rc(rx);
			rc.DeflateRect(1, 1, 1, 1);
			pDC->SetTextColor(RGB(44, 44, 44));
			pDC->FillRect(&rc, &CBrush(RGB(255, 255, 255)));
			pDC->DrawText(str[idx + 6], -1, &rx, DT_SINGLELINE|DT_VCENTER|DT_LEFT);
		}
	}
}

void CFormCtrl::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
	RedrawWindow();

	if (!::IsWindow(m_btnCXTJ.GetSafeHwnd()))
		return ;

	CPoint ptOff = GetScrollPosition();

	CRect rx;
	rx.top = 9; rx.bottom = rx.top + 22;
	rx.left = 125; rx.right = rx.left + 115;
	rx.OffsetRect(-ptOff.x, -ptOff.y);
	m_btnCXTJ.MoveWindow(&rx);

	rx.OffsetRect(rx.Width()+10, 0);
	m_btnViewTJ.MoveWindow(&rx);

	rx.top = 45; rx.bottom = rx.top + 27;
	rx.left = 180+5; rx.right = rx.left + 199;
	rx.OffsetRect(-ptOff.x, -ptOff.y);
	for (int idx=0; idx<_countof(m_btnServer); idx++)
	{
		if (idx>0)
			rx.right = rx.left + 139;
		m_btnServer[idx].MoveWindow(&rx);
		rx.OffsetRect(rx.Width() + 5, 0);
	}


	rx.top = 45 + 203 + 70; rx.bottom = rx.top + 16;
	CRect rcClient;
	GetClientRect(&rcClient);
	int width = (rcClient.right - 10 ) / 7;
	for (int idx=0; idx<_countof(m_btnCliStatus); idx++)
	{
		rx.left = 5 + idx * width + width / 2 - 12;
		rx.right = rx.left + 24;
		{
			CRect rc = rx;
			rc.OffsetRect(-ptOff.x, -ptOff.y);
			m_btnCliStatus[idx].MoveWindow(&rc);
		}
	}

	for (int idx=0; idx<_countof(m_btnGameStatus)/2; idx++)
	{
		int nWidth = rcClient.Width() - 2 * 5;

		CRect rx(nWidth/2 - 30, 382 + idx*24, nWidth/2 - 5, 382 + idx*24 + 16);
		rx.OffsetRect(-ptOff.x, -ptOff.y);
		m_btnGameStatus[idx].MoveWindow(&rx);

		rx.OffsetRect(nWidth/2, 0);
		m_btnGameStatus[idx+6].MoveWindow(&rx);
	}
}

void CFormCtrl::OnClickButton(UINT id)
{
	id -= ID_BASE;
	switch (id)
	{
	case ID_SERVER_SVR1:
	case ID_SERVER_SVR2:
	case ID_SERVER_SVR3:
	case ID_SERVER_SVR4:
		if (m_nCurSvr != id - ID_SERVER_SVR1)
		{
			m_nCurSvr = id - ID_SERVER_SVR1;
			CClientDC dc(this);
			OnPrepareDC(&dc);
			UpdateServer(&dc);
		}
		return ;
	}
	HandleEvent(id, NULL);
}
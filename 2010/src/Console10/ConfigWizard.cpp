// ConfigWizard.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "ConfigWizard.h"
#include "ConsoleDlg.h"

#include "WZWelcomeBoard.h"
#include "WZAreaMgrBorad.h"
#include "WZClassMgrBoard.h"
#include "WZVDiskMgrBoard.h"
#include "WZDownloadBoard.h"
#include "WZMenuBoard.h"
#include "WZAddGameBoard.h"
#include "WZUpdateBoard.h"
#include "WZAccountBoard.h"
#include "WZBootTaskBoard.h"
#include "WZSafeCenterBoard.h"
#include "WZServerBoard.h"
#include "WZSyncTaskBoard.h"


#include "oem.h"

CListBoxExWZ::CListBoxExWZ()
{
	VERIFY(m_bmListBoxExDef.LoadBitmap(IDB_LISTBOXEXDEFWZ));
	VERIFY(m_bmListBoxExSel.LoadBitmap(IDB_LISTBOXEXSELWZ));
	
	BITMAP   bm;   
	m_bmListBoxExSel.GetBitmap(&bm);   
	m_nItemHeight = bm.bmHeight;
}

CListBoxExWZ::~CListBoxExWZ()
{

}

BEGIN_MESSAGE_MAP(CListBoxExWZ, CListBox)
	//{{AFX_MSG_MAP(CListBoxExWZ)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CListBoxExWZ::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = m_nItemHeight;
}

void CListBoxExWZ::DrawItem(LPDRAWITEMSTRUCT lpDS)
{
	CDC dc;
	dc.Attach(lpDS->hDC);

	CRect rcText(lpDS->rcItem);

	if (((lpDS->itemAction | ODA_SELECT) && (lpDS->itemState & ODS_SELECTED)) ||
		((lpDS->itemAction | ODA_FOCUS) && (lpDS->itemState & ODS_FOCUS)) )
	{
		dc.FillRect(&rcText, &CBrush(&m_bmListBoxExSel));
	}
	else
	{
		dc.FillRect(&rcText, &CBrush(&m_bmListBoxExDef));
	}

	rcText.DeflateRect(2, 2, 2, 2);
	rcText.left += 32;
	dc.DrawText(m_lstItemData[lpDS->itemID].szText, -1, &rcText, DT_LEFT|DT_SINGLELINE|DT_VCENTER);

	dc.Detach();
}

void CListBoxExWZ::AddItem(UINT nIconID, LPCTSTR lpszText)
{
	tagItem Item;
	Item.IconID = nIconID;
	Item.szText = lpszText;
	m_lstItemData.push_back(Item);
	AddString("");
}

// CConfigWizard 对话框

IMPLEMENT_DYNAMIC(CConfigWizard, CDialog)

CConfigWizard::CConfigWizard(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigWizard::IDD, pParent)
	, m_iCurrentWizard(1)
	, m_flags(0)
{

}

CConfigWizard::~CConfigWizard()
{
}

void CConfigWizard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_STEP_TIP, m_bnStepDesc);
	DDX_Control(pDX, IDC_BUTTON_BOARD, m_bnBoardPlace);
	DDX_Control(pDX, IDC_LISTBOX, m_lbStep);
}


BEGIN_MESSAGE_MAP(CConfigWizard, CDialog)
	ON_BN_CLICKED(IDC_NEXT_WIZARD, &CConfigWizard::OnBnClickedNextWizard)
	ON_BN_CLICKED(IDC_PREV_WIZARD, &CConfigWizard::OnBnClickedPrevWizard)
	ON_BN_CLICKED(IDOK, &CConfigWizard::OnBnClickedOk)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDCANCEL, &CConfigWizard::OnBnClickedCancel)
	ON_LBN_SELCHANGE(IDC_LISTBOX, &CConfigWizard::OnLbnSelchangeListbox)
END_MESSAGE_MAP()


// CConfigWizard 消息处理程序

BOOL CConfigWizard::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	CConsoleDlg *pMainWnd = static_cast<CConsoleDlg*>(AfxGetMainWnd());


	CString strPath = I8DESK_OEM->GetString(_T("config"), _T(""));
	if( strPath.IsEmpty() || strPath == _T("") )
		m_bnStepDesc.SetImage(IDB_LOGIN1);//->GetIcon(TRUE), NULL);
	else
		m_bnStepDesc.SetImage(strPath);

	//m_bnStepDesc.SetIconImage(pMainWnd->GetIcon(TRUE), NULL);
	COLORREF clr = RGB(255, 255, 255);
	m_bnStepDesc.SetTextStyle(_T("宋体"), 16, clr, clr, clr, clr, 
		CButtonEx::BTEXT_RIGHT, TRUE);
	
//	clr = RGB(241, 247, 250);
//	m_bnStepDesc.SetImage3(clr, clr, clr, clr);

	m_bnBoardPlace.SetImage3(clr, clr, clr, clr);

	RECT rc;
	m_bnBoardPlace.GetWindowRect(&rc);
	m_bnBoardPlace.ScreenToClient(&rc);
	rc.top += 1, rc.left += 1, rc.bottom -= 1, rc.right -= 1;

	AddBoard<CWZWelcomeBoard>(&rc, _T("欢迎使用向导"));
	AddBoard<CWZAreaMgrBoard>(&rc, _T("划分终端区域"));
	AddBoard<CWZClassMgrBoard>(&rc, _T("划分游戏类别"));
	AddBoard<CWZVDiskMgrBoard>(&rc, _T("设置虚拟磁盘"));
	AddBoard<CWZDownloadBoard>(&rc, _T("设置三层下载"));
	AddBoard<CWZMenuBoard>(&rc, _T("设置菜单显示"));
	AddBoard<CWZAddGameBoard>(&rc, _T("搜索添加游戏"));
	AddBoard<CWZUpdateBoard>(&rc, _T("设置内网更新"));
	AddBoard<CWZAccountBoard>(&rc, _T("设置安全密码"));
	AddBoard<CWZBootTaskBoard>(&rc, _T("设置开机任务"));
	AddBoard<CWZSafeCenterBoard>(&rc, _T("设置安全中心"));
	AddBoard<CWZServerBoard>(&rc, _T("部署多服务器"));
	AddBoard<CWZSyncTaskBoard>(&rc, _T("部署同步任务"));

	ShowBoard(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

template<typename T>
void CConfigWizard::AddBoard(LPRECT lpRect, LPCTSTR desc)
{
	CDialog *pDlg = new T;
	pDlg->Create(T::IDD, &m_bnBoardPlace);
	pDlg->MoveWindow(lpRect);
	pDlg->SetWindowText(desc);
	m_wizards.push_back(pDlg);
	m_lbStep.AddItem(0, desc);
}

void CConfigWizard::OnBnClickedNextWizard()
{
	ShowBoard(m_iCurrentWizard + 1);
}

void CConfigWizard::OnBnClickedPrevWizard()
{
	ShowBoard(m_iCurrentWizard - 1);
}

void CConfigWizard::ShowBoard(size_t idx)
{
	if (idx == m_iCurrentWizard)
		return;

	//面板切换
	m_wizards[m_iCurrentWizard]->ShowWindow(SW_HIDE);
	m_wizards[idx]->ShowWindow(SW_SHOW);

	//按钮状态
	if (idx == 0) {
		GetDlgItem(IDC_NEXT_WIZARD)->EnableWindow(TRUE);
		GetDlgItem(IDC_PREV_WIZARD)->EnableWindow(FALSE);
	}
	else if (idx == m_wizards.size() - 1) {
		GetDlgItem(IDC_PREV_WIZARD)->EnableWindow(TRUE);
		GetDlgItem(IDC_NEXT_WIZARD)->EnableWindow(FALSE);
	}
	else {
		GetDlgItem(IDC_PREV_WIZARD)->EnableWindow(TRUE);
		GetDlgItem(IDC_NEXT_WIZARD)->EnableWindow(TRUE);
	}

	//设置面板的描述信息
	m_bnStepDesc.SetWindowText(m_lbStep.GetItemText(idx));

	//步骤状态
	m_lbStep.SetCurSel(idx);

	m_iCurrentWizard = idx;
}

void CConfigWizard::OnBnClickedOk()
{
	if (!GetDbMgr()->IsConnected())
	{
		AfxMessageBox("与服务器连接中断，不能完成配置!");
		return;
	}

	for (size_t i = 0; i < m_wizards.size(); i++) 
	{
		LRESULT ok = m_wizards[i]->SendMessage(i8desk::g_nOptApplyMsg, 0, 0);
		if (!ok)
		{
			ShowBoard(i);
			return;
		}
	}

	OnOK();
}

void CConfigWizard::OnDestroy()
{
	CDialog::OnDestroy();

	for (size_t i = 0; i < m_wizards.size(); i++) {
		m_wizards[i]->DestroyWindow();
		delete m_wizards[i];
	}
}

void CConfigWizard::OnBnClickedCancel()
{
	OnCancel();
}

void CConfigWizard::OnLbnSelchangeListbox()
{
	int nSel = m_lbStep.GetCurSel();
	if (nSel == -1)
		return;

	ShowBoard(nSel);
}

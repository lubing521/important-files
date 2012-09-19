// DlgChkDisk.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgChkDisk.h"
#include "../ui/Skin/SkinMgr.h"
#include "../../../../include/ui/ImageHelpers.h"
#include "../UI/UIHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgChkDisk 对话框

IMPLEMENT_DYNAMIC(CDlgChkDisk, CNonFrameChildDlg)

CDlgChkDisk::CDlgChkDisk(const CString &unChkAreas, const CString &chkAreas,CWnd* pParent /*=NULL*/)
	: CNonFrameChildDlg(CDlgChkDisk::IDD, pParent)
	, unChkAreas_(unChkAreas)
	, chkAreas_(chkAreas)
{

}

CDlgChkDisk::~CDlgChkDisk()
{
}

void CDlgChkDisk::DoDataExchange(CDataExchange* pDX)
{
	CNonFrameChildDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CLIENT_CONTROL_CHECKPROT, wndListBoxChkAreas_);
	DDX_Control(pDX, IDC_LIST_CLIENT_CONTROL_UNCHECKPROT, wndListBoxUnChkAreas_);
	DDX_Control(pDX, IDC_STATIC_CLIENT_CONTROL_UNCHECKPROT, wndLabelUnChkAreas_);
	DDX_Control(pDX, IDC_STATIC_CLIENT_CONTROL_CHECKPROT, wndLabelChkAreas_);

	DDX_Control(pDX, IDC_BUTTON_CLIENT_CONTROL_ADD_PORT, wndBtnAdd_);
	DDX_Control(pDX, IDC_BUTTON_CLIENT_CONTROL_ADDALL_PORT, wndBtnAddAll_);
	DDX_Control(pDX, IDC_BUTTON_CLIENT_CONTROL_REMOVE_PORT, wndBtnRemove_);
	DDX_Control(pDX, IDC_BUTTON_CLIENT_CONTROL_REMOVEALL_PORT, wndBtnRemoveAll_);
	DDX_Control(pDX, IDOK, wndBtnOk_);
	DDX_Control(pDX, IDCANCEL, wndBtnCancel_);

}


BEGIN_MESSAGE_MAP(CDlgChkDisk, CNonFrameChildDlg)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_CONTROL_ADD_PORT, &CDlgChkDisk::OnBnClickedButtonClientControlAddPort)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_CONTROL_ADDALL_PORT, &CDlgChkDisk::OnBnClickedButtonClientControlAddallPort)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_CONTROL_REMOVE_PORT, &CDlgChkDisk::OnBnClickedButtonClientControlRemovePort)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_CONTROL_REMOVEALL_PORT, &CDlgChkDisk::OnBnClickedButtonClientControlRemoveallPort)
	ON_BN_CLICKED(IDOK, &CDlgChkDisk::OnBnClickedOk)
	ON_WM_ERASEBKGND()

END_MESSAGE_MAP()


// CDlgChkDisk 消息处理程序

BOOL CDlgChkDisk::OnInitDialog()
{
	CNonFrameChildDlg::OnInitDialog();
	SetTitle(_T("修复磁盘分区"));

	for(TCHAR chDrv = _T('C'); chDrv <= _T('Z'); chDrv++)
	{
		CString szItem(chDrv);

		if( chkAreas_.Find(szItem) == -1 )
			wndListBoxUnChkAreas_.AddString(szItem);
	}

	for(int nIdx=0; nIdx != chkAreas_.GetLength(); ++nIdx)
	{
		CString szSub = chkAreas_.Mid(nIdx, 1);
		if( -1 == wndListBoxChkAreas_.FindString(-1, szSub) )
			wndListBoxChkAreas_.AddString(szSub);
	}

	if( wndListBoxUnChkAreas_.GetCount() )
		wndListBoxUnChkAreas_.SetCurSel(0);

	if( wndListBoxChkAreas_.GetCount() )
		wndListBoxChkAreas_.SetCurSel(0);

	wndLabelUnChkAreas_.SetThemeParent(GetSafeHwnd());
	wndLabelChkAreas_.SetThemeParent(GetSafeHwnd());

	// 画Button的背景
	HBITMAP btn[] = 
	{

		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn_Hover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn_Press.png"))

	};
	wndBtnAdd_.SetImages(btn[0],btn[1],btn[2]);
	wndBtnAddAll_.SetImages(btn[0],btn[1],btn[2]);
	wndBtnRemove_.SetImages(btn[0],btn[1],btn[2]);
	wndBtnRemoveAll_.SetImages(btn[0],btn[1],btn[2]);
	wndBtnOk_.SetImages(btn[0],btn[1],btn[2]);
	wndBtnCancel_.SetImages(btn[0],btn[1],btn[2]);
	// Out Line

	outboxLine_.Attach(CopyBitmap(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ControlOutBox.png"))));


	return TRUE;  // return TRUE unless you set the focus to a control
}


void CDlgChkDisk::OnBnClickedButtonClientControlAddPort()
{
	for (int idx=0; idx != wndListBoxUnChkAreas_.GetCount(); ++idx)
	{
		if( wndListBoxUnChkAreas_.GetSel(idx) )
		{
			CString Text;
			wndListBoxUnChkAreas_.GetText(idx, Text);
			wndListBoxUnChkAreas_.DeleteString(idx);
			wndListBoxChkAreas_.AddString(Text);
		}
	}

	if (wndListBoxUnChkAreas_.GetCount())
		wndListBoxUnChkAreas_.SetCurSel(0);
}

void CDlgChkDisk::OnBnClickedButtonClientControlAddallPort()
{
	wndListBoxChkAreas_.ResetContent();
	wndListBoxUnChkAreas_.ResetContent();
	for(TCHAR chDrv = _T('C'); chDrv <= _T('Z'); chDrv++)
	{
		CString szItem(chDrv);
		wndListBoxChkAreas_.AddString(szItem);
	}
}

void CDlgChkDisk::OnBnClickedButtonClientControlRemovePort()
{
	for (int idx=0; idx<wndListBoxChkAreas_.GetCount(); idx++)
	{
		if (wndListBoxChkAreas_.GetSel(idx))
		{
			CString Text;
			wndListBoxChkAreas_.GetText(idx, Text);
			wndListBoxChkAreas_.DeleteString(idx);
			wndListBoxUnChkAreas_.AddString(Text);
		}
	}
	if (wndListBoxChkAreas_.GetCount())
		wndListBoxChkAreas_.SetCurSel(0);


}

void CDlgChkDisk::OnBnClickedButtonClientControlRemoveallPort()
{
	wndListBoxUnChkAreas_.ResetContent();
	wndListBoxChkAreas_.ResetContent();
	for(TCHAR chDrv = _T('C'); chDrv <= _T('Z'); chDrv++)
	{
		CString szItem(chDrv);
		wndListBoxUnChkAreas_.AddString(szItem);
	}
}

void CDlgChkDisk::OnBnClickedOk()
{
	for (int idx=0; idx<wndListBoxUnChkAreas_.GetCount(); idx++)
	{
		TCHAR buf[2] = {0};
		wndListBoxUnChkAreas_.GetText(idx, buf);
		unChkAreas_ += buf;
	}

	for (int idx=0; idx<wndListBoxChkAreas_.GetCount(); idx++)
	{
		TCHAR buf[2] = {0};
		wndListBoxChkAreas_.GetText(idx, buf);
		chkAreas_ += buf;
	}

	OnOK();
}

BOOL CDlgChkDisk::OnEraseBkgnd(CDC* pDC)
{
	__super::OnEraseBkgnd(pDC);

	CRect rcMultiText;
	wndListBoxChkAreas_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, *pDC, rcMultiText, &outboxLine_);
	wndListBoxUnChkAreas_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, *pDC, rcMultiText, &outboxLine_);

	return TRUE;
}

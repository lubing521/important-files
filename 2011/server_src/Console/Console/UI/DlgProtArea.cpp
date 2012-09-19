// DlgChkDisk.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgProtArea.h"
#include "../ui/Skin/SkinMgr.h"
#include "../Business/ClientBusiness.h"
#include "../../../../include/Utility/utility.h"

#include "../../../../include/ui/ImageHelpers.h"
#include "../UI/UIHelper.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// CDlgProtArea 对话框

IMPLEMENT_DYNAMIC(CDlgProtArea, CNonFrameChildDlg)

CDlgProtArea::CDlgProtArea(const CString &unProtAreas, const CString &protAreas, CWnd* pParent /*=NULL*/)
: CNonFrameChildDlg(CDlgProtArea::IDD, pParent)
, unProtAreas_(unProtAreas)
, protAreas_(protAreas)
{

}

CDlgProtArea::~CDlgProtArea()
{
}

void CDlgProtArea::DoDataExchange(CDataExchange* pDX)
{
	CNonFrameChildDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CLIENT_CONTROL_CHECKPROT, wndListBoxProtAreas_);
	DDX_Control(pDX, IDC_LIST_CLIENT_CONTROL_UNCHECKPROT, wndListBoxUnProtAreas_);

	DDX_Control(pDX, IDC_STATIC_CLIENT_CONTROL_UNCHECKPROT, wndunProtAreas_);
	DDX_Control(pDX, IDC_STATIC_CLIENT_CONTROL_CHECKPROT, wndprotAreas_);

	DDX_Control(pDX, IDC_BUTTON_CLIENT_CONTROL_ADD_PORT, wndBtnAddProt_);
	DDX_Control(pDX, IDC_BUTTON_CLIENT_CONTROL_ADDALL_PORT, wndBtnAddAllProt_);
	DDX_Control(pDX, IDC_BUTTON_CLIENT_CONTROL_REMOVE_PORT, wndBtnRemoveProt_);
	DDX_Control(pDX, IDC_BUTTON_CLIENT_CONTROL_REMOVEALL_PORT, wndBtnRemoveAll_);
	DDX_Control(pDX, IDOK, wndBtnOk_);
	DDX_Control(pDX, IDCANCEL, wndBtnCancel_);

}


BEGIN_MESSAGE_MAP(CDlgProtArea, CNonFrameChildDlg)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_CONTROL_ADD_PORT, &CDlgProtArea::OnBnClickedButtonClientControlAddPort)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_CONTROL_ADDALL_PORT, &CDlgProtArea::OnBnClickedButtonClientControlAddallPort)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_CONTROL_REMOVE_PORT, &CDlgProtArea::OnBnClickedButtonClientControlRemovePort)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_CONTROL_REMOVEALL_PORT, &CDlgProtArea::OnBnClickedButtonClientControlRemoveallPort)
	ON_BN_CLICKED(IDOK, &CDlgProtArea::OnBnClickedOk)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CDlgProtArea 消息处理程序

BOOL CDlgProtArea::OnInitDialog()
{
	CNonFrameChildDlg::OnInitDialog();
	SetTitle(_T("设置分区保护"));

	for(TCHAR chDrv = _T('C'); chDrv <= _T('Z'); chDrv++)
	{
		CString szItem(chDrv);

		if( protAreas_.Find(szItem) == -1 )
			wndListBoxUnProtAreas_.AddString(szItem);
	}

	for(int nIdx=0; nIdx != protAreas_.GetLength(); ++nIdx)
	{
		CString szSub = protAreas_.Mid(nIdx, 1);
		if( -1 == wndListBoxProtAreas_.FindString(-1, szSub) )
			wndListBoxProtAreas_.AddString(szSub);
	}

	if( wndListBoxUnProtAreas_.GetCount() )
		wndListBoxUnProtAreas_.SetCurSel(0);

	if( wndListBoxProtAreas_.GetCount() )
		wndListBoxProtAreas_.SetCurSel(0);

	// 画Static的背景
	wndunProtAreas_.SetThemeParent(GetSafeHwnd());
	wndprotAreas_.SetThemeParent(GetSafeHwnd());

	// 画Button的背景
	HBITMAP btn[] = 
	{

		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn_Hover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn_Press.png"))
	};
	wndBtnAddProt_.SetImages(btn[0],btn[1],btn[2]);
	wndBtnAddAllProt_.SetImages(btn[0],btn[1],btn[2]);
	wndBtnRemoveProt_.SetImages(btn[0],btn[1],btn[2]);
	wndBtnCancel_.SetImages(btn[0],btn[1],btn[2]);
	wndBtnOk_.SetImages(btn[0],btn[1],btn[2]);
	wndBtnRemoveAll_.SetImages(btn[0],btn[1],btn[2]);

	// Out Line

	outboxLine_.Attach(CopyBitmap(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ControlOutBox.png"))));

	return TRUE;  // return TRUE unless you set the focus to a control
}


void CDlgProtArea::OnBnClickedButtonClientControlAddPort()
{
	for (int idx=0; idx < wndListBoxUnProtAreas_.GetCount(); ++idx)
	{
		if( wndListBoxUnProtAreas_.GetSel(idx) )
		{
			CString Text;
			wndListBoxUnProtAreas_.GetText(idx, Text);
			wndListBoxUnProtAreas_.DeleteString(idx);
			int index = wndListBoxProtAreas_.GetCount();
			wndListBoxProtAreas_.InsertString(index,Text);
		}
	}

	if (wndListBoxUnProtAreas_.GetCount())
		wndListBoxUnProtAreas_.SetCurSel(0);
}

void CDlgProtArea::OnBnClickedButtonClientControlAddallPort()
{
	wndListBoxProtAreas_.ResetContent();
	wndListBoxUnProtAreas_.ResetContent();
	for(TCHAR chDrv = _T('C'); chDrv <= _T('Z'); chDrv++)
	{
		CString szItem(chDrv);
		wndListBoxProtAreas_.AddString(szItem);
	}
}

void CDlgProtArea::OnBnClickedButtonClientControlRemovePort()
{
	for (int idx=0; idx < wndListBoxProtAreas_.GetCount(); idx++)
	{
		if (wndListBoxProtAreas_.GetSel(idx))
		{
			CString Text;
			wndListBoxProtAreas_.GetText(idx, Text);
			wndListBoxProtAreas_.DeleteString(idx);
			int Index = wndListBoxUnProtAreas_.GetCount();
			wndListBoxUnProtAreas_.InsertString(Index,Text);
		}
	}
	if (wndListBoxProtAreas_.GetCount())
		wndListBoxProtAreas_.SetCurSel(0);
}

void CDlgProtArea::OnBnClickedButtonClientControlRemoveallPort()
{
	wndListBoxUnProtAreas_.ResetContent();
	wndListBoxProtAreas_.ResetContent();
	for(TCHAR chDrv = _T('C'); chDrv <= _T('Z'); chDrv++)
	{
		CString szItem(chDrv);
		wndListBoxUnProtAreas_.AddString(szItem);
	}
}

void CDlgProtArea::OnBnClickedOk()
{
	unProtAreas_ = _T("");
	protAreas_ = _T("");
	for (int idx=0; idx<wndListBoxUnProtAreas_.GetCount(); idx++)
	{
		TCHAR buf[2] = {0};
		wndListBoxUnProtAreas_.GetText(idx, buf);
		unProtAreas_ += buf;
	}

	for (int idx=0; idx<wndListBoxProtAreas_.GetCount(); idx++)
	{
		TCHAR buf[2] = {0};
		wndListBoxProtAreas_.GetText(idx, buf);
		protAreas_ += buf;
	}

	OnOK();
}

BOOL CDlgProtArea::OnEraseBkgnd(CDC* pDC)
{
	__super::OnEraseBkgnd(pDC);

	CRect rcMultiText;
	wndListBoxProtAreas_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, *pDC, rcMultiText, &outboxLine_);
	wndListBoxUnProtAreas_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, *pDC, rcMultiText, &outboxLine_);

	return TRUE;
}


// DlgClientSystemInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgClientSystemInfo.h"
#include "../Business/ClientBusiness.h"
#include "../Data/CustomDataType.h"
#include "../ManagerInstance.h"
#include "../../../../include/Utility/utility.h"
#include "../../../../include/ui/ImageHelpers.h"
#include "../ui/Skin/SkinMgr.h"
#include "../UI/UIHelper.h"
#include "../../../../include/I8Type.hpp"
#include "../Network/IOService.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


enum { WM_VIEW_CLIENTINFO = WM_USER + 100 };

IMPLEMENT_DYNAMIC(CDlgClientSysInfo, CNonFrameChildDlg)

CDlgClientSysInfo::CDlgClientSysInfo(i8desk::data_helper::ClientTraits::VectorType clients,
									 const int CtrlType,
									 CWnd* pParent /*=NULL*/)
	: CNonFrameChildDlg(CDlgClientSysInfo::IDD, pParent)
	, curClients_(clients)
	, ctrlType_(CtrlType)
	, textSysInfo_(_T(""))
{

}

CDlgClientSysInfo::~CDlgClientSysInfo()
{

}


void CDlgClientSysInfo::DoDataExchange(CDataExchange* pDX)
{
	CNonFrameChildDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CLIENT_PROCINFOR, listProcInfo_);
	DDX_Control(pDX, IDC_EDIT_CLIENT_SYSTEMINFOR, editSysInfo_);
	DDX_Text(pDX, IDC_EDIT_CLIENT_SYSTEMINFOR, textSysInfo_);

	DDX_Control(pDX, IDC_LIST_CLIENT_SVRINFOR, listSvrInfo_);

}

BEGIN_MESSAGE_MAP(CDlgClientSysInfo, CNonFrameChildDlg)
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_CLIENT_PROCINFOR, &CDlgClientSysInfo::OnLvnGetdispinfoListProcInfo)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_CLIENT_SVRINFOR, &CDlgClientSysInfo::OnLvnGetdispinfoListSvrInfo)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_CLIENT_PROCINFOR, &CDlgClientSysInfo::OnNMRClickList)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_CLIENT_SVRINFOR, &CDlgClientSysInfo::OnNMRClickList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CLIENT_PROCINFOR, &CDlgClientSysInfo::OnNMRClickList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CLIENT_SVRINFOR, &CDlgClientSysInfo::OnNMDblclkList)
	ON_COMMAND(ID_POP_CLIENT_VIEWINFO_REFRESH, &CDlgClientSysInfo::OnKillRefresh)
	ON_COMMAND(ID_POP_CLIENT_VIEWINFO_KILL, &CDlgClientSysInfo::OnKillProcsvr)
	ON_MESSAGE(WM_VIEW_CLIENTINFO, &CDlgClientSysInfo::OnMessage)
END_MESSAGE_MAP()
namespace i8desk
{
	namespace detail
	{
		CCustomColumnTrait *GetTrait()
		{
			CCustomColumnTrait *rowTrait = new CCustomColumnTrait;
			rowTrait->SetBkImg(

				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Table_LineFirst_Bg.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TablePerLine.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TableRowSel.png")));

			return rowTrait;
		}


		enum { MASK_NAME = 1, MASK_PID, MASK_MEMERY };

		template < typename ListCtrlT >
		void InitListProcess(ListCtrlT &listCtrl)
		{
			listCtrl.InsertHiddenLabelColumn();

			listCtrl.InsertColumnTrait(MASK_NAME,	_T("进程名称"),		LVCFMT_LEFT, 180 ,MASK_NAME,detail::GetTrait());
			listCtrl.InsertColumnTrait(MASK_PID,		_T("PID"),		LVCFMT_LEFT, 100 ,MASK_PID,detail::GetTrait());
			listCtrl.InsertColumnTrait(MASK_MEMERY,	_T("内存使用(K)"),	LVCFMT_LEFT, 100 ,MASK_MEMERY,detail::GetTrait());
		}

		enum { MASK_SVRNAME = 1, MASK_SHOWNAME, MASK_PROCESS };

		template < typename ListCtrlT >
		void InitListServer(ListCtrlT &listCtrl)
		{
			listCtrl.InsertHiddenLabelColumn();

			listCtrl.InsertColumnTrait(MASK_SVRNAME,	_T("服务名"),		LVCFMT_LEFT, 100,MASK_SVRNAME, detail::GetTrait());
			listCtrl.InsertColumnTrait(MASK_SHOWNAME,	_T("显示名称"),		LVCFMT_LEFT, 180,MASK_SHOWNAME,detail::GetTrait());
			listCtrl.InsertColumnTrait(MASK_PROCESS,	_T("服务进程名"),	LVCFMT_LEFT, 100,MASK_PROCESS,detail::GetTrait());
		}
	}

}



BOOL CDlgClientSysInfo::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;

	if (pMsg->message == WM_RBUTTONDOWN && pMsg->hwnd == editSysInfo_.m_hWnd)
	{
		LRESULT result;
		OnNMDblclkList(NULL, &result);
		return TRUE;
	}
	return CNonFrameChildDlg::PreTranslateMessage(pMsg);	
}


void CDlgClientSysInfo::OnNMRClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;
	
	LRESULT result;
	OnNMDblclkList(NULL, &result);
}


void CDlgClientSysInfo::OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	CPoint oPoint;
	GetCursorPos( &oPoint );
	CMenu menu;
	menu.LoadMenu(MAKEINTRESOURCE(IDR_MENU_CLIENT_VIEWINFO_KILL));
	menu.ModifyMenu(ID_POP_CLIENT_VIEWINFO_KILL, MF_BYCOMMAND|MF_STRING, ID_POP_CLIENT_VIEWINFO_KILL, 
		ctrlType_ == i8desk::Ctrl_ViewProcInfo ? _T("结束进程") : _T("停止服务"));

	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		oPoint.x, oPoint.y, this);
}

void CDlgClientSysInfo::_RefreshInfo()
{
	struct Op
		: public enable_shared_from_this<Op>
	{
		int type_;
		stdex::tString &msg_;
		i8desk::data_helper::ClientTraits::VectorType &clients_;
		HWND wnd_;

		Op(int type, stdex::tString &msg, i8desk::data_helper::ClientTraits::VectorType &clients, HWND wnd)
			: type_(type) 
			, msg_(msg)
			, clients_(clients)
			, wnd_(wnd)
		{}

		void Callback(size_t, size_t)
		{
			stdex::tString msg;

			switch ( type_ )
			{
			case i8desk::Ctrl_ViewSysInfo:
				i8desk::business::Client::ControlViewSystem(clients_, msg);
				break;
			case i8desk::Ctrl_ViewProcInfo:
				i8desk::business::Client::ControlViewProcess(clients_, msg);
				break;
			case i8desk::Ctrl_ViewSvrInfo:
				i8desk::business::Client::ControlViewService(clients_, msg);
				break;
			default:
				assert(0);
				break;
			}

			if( ::IsWindow(wnd_) )
			{
				msg_ = msg;
				::PostMessage(wnd_, WM_VIEW_CLIENTINFO, 0, 0);
			}
		}
	};

	std::tr1::shared_ptr<Op> op(new Op(ctrlType_, recvText_, curClients_, GetSafeHwnd()));

	using namespace std::tr1::placeholders;
	i8desk::io::GetIODispatcher().Post(std::tr1::bind(&Op::Callback, op, _1, _2));
	
}

void  CDlgClientSysInfo::_RecvParam(const stdex::tString &param)
{
	switch ( ctrlType_ )
	{
	case i8desk::Ctrl_ViewSysInfo:
		textSysInfo_ = param.c_str();
		break;
	case i8desk::Ctrl_ViewProcInfo:
		curListVector_.clear();
		i8desk::business::Client::PraseViewProcess(curListVector_,param);
		break;
	case i8desk::Ctrl_ViewSvrInfo:
		curListVector_.clear();
		i8desk::business::Client::PraseViewService(curListVector_,param);
		break;
	default:
		assert(0);
		break;	
	}
}


void CDlgClientSysInfo::_ShowView()
{
	UpdateData(FALSE);
	size_t cnt = curListVector_.size();

	switch ( ctrlType_ )
	{
	case i8desk::Ctrl_ViewSysInfo:
		SetTitle(_T("计算机系统信息"));
		editSysInfo_.ShowWindow(SW_SHOW);
		listProcInfo_.ShowWindow(SW_HIDE);
		listSvrInfo_.ShowWindow(SW_HIDE);
		break;
	case i8desk::Ctrl_ViewProcInfo:
		SetTitle(_T("计算机进程信息"));
		listProcInfo_.ShowWindow(SW_SHOW);
		editSysInfo_.ShowWindow(SW_HIDE);
		listSvrInfo_.ShowWindow(SW_HIDE);
		listProcInfo_.SetItemCount(cnt);
		break;
	case i8desk::Ctrl_ViewSvrInfo:
		SetTitle(_T("计算机服务信息"));
		listSvrInfo_.ShowWindow(SW_SHOW);
		editSysInfo_.ShowWindow(SW_HIDE);
		listProcInfo_.ShowWindow(SW_HIDE);
		listSvrInfo_.SetItemCount(cnt);
		break;
	default:
		assert(0);
		break;
	}
}

void CDlgClientSysInfo::_SendKillProcSvr(CCustomListCtrl &listCtrl,bool IsProcess)
{
	stdex::tString filename;
	CString file;
	int nSel = -1;

	nSel = listCtrl.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1)
		return ;
	file = listCtrl.GetItemText(nSel, 1);
	if (file.IsEmpty())
		return ;
	filename = file;
	i8desk::business::Client::ControlKillProcess(filename, curClients_[0]->IP,IsProcess);

}

void  CDlgClientSysInfo::_KillProcSvr()
{
	switch( ctrlType_ )
	{
	case i8desk::Ctrl_ViewSysInfo:
		return;
	case i8desk::Ctrl_ViewProcInfo:
		_SendKillProcSvr(listProcInfo_,true);
		break;
	case i8desk::Ctrl_ViewSvrInfo:
		_SendKillProcSvr(listSvrInfo_,false);
		break;
	default:
		assert(0);
		break;
	}

}

void CDlgClientSysInfo::OnKillRefresh()
{
	_RefreshInfo();
}

void CDlgClientSysInfo::OnKillProcsvr()
{
	CWaitCursor wc;
	_KillProcSvr();
}

BOOL CDlgClientSysInfo::OnInitDialog()
{
	CNonFrameChildDlg::OnInitDialog();

	//i8desk::GetRecvDataMgr().RegisterClient(std::tr1::bind(&CDlgClientSysInfo::_RecvParam, this, std::tr1::placeholders::_1));


	// 初始化ListCtrl

	listProcInfo_.SetHeaderImage(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderBg.png")), 
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderLine.png")));
	listSvrInfo_.SetHeaderImage(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderBg.png")), 
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderLine.png")));

	i8desk::detail::InitListProcess(listProcInfo_);
	i8desk::detail::InitListServer(listSvrInfo_);
	
	CRect rc;
	GetClientRect(&rc);
	rc.DeflateRect(5, 35, 5, 5);
	listProcInfo_.MoveWindow(&rc);
	listSvrInfo_.MoveWindow(&rc);
	editSysInfo_.MoveWindow(&rc);

	_ShowView();

	_RefreshInfo();

	// Out Line

	outboxLine_.Attach(CopyBitmap(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ControlOutBox.png"))));


	return TRUE;

}

void CDlgClientSysInfo::OnTimer(UINT_PTR nIDEvent)
{
	CNonFrameChildDlg::OnTimer(nIDEvent);
	_RefreshInfo();
}

LRESULT CDlgClientSysInfo::OnMessage(WPARAM, LPARAM)
{
	if( !::IsWindow(GetSafeHwnd()) )
		return TRUE;

	_RecvParam(recvText_);
	_ShowView();

	return TRUE;
}

void CDlgClientSysInfo::OnDestroy()
{
	//i8desk::GetRecvDataMgr().RegisterClient(0);

	CNonFrameChildDlg::OnDestroy();
}

void CDlgClientSysInfo::OnLvnGetdispinfoListSvrInfo(NMHDR *pNMHDR, LRESULT *pResult)
{

	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	int itemIndex = pItem->iItem;

	if( static_cast<size_t>(itemIndex) >= curListVector_.size() )
		return;

	const std::vector<ListVector>::value_type &val = curListVector_[itemIndex];

	static stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case i8desk::detail::MASK_SVRNAME:
			text = val.col0;
			break;
		case i8desk::detail::MASK_SHOWNAME:
			text = val.col1;
			break;
		case i8desk::detail::MASK_PROCESS:
			text = val.col2;
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}

}

void CDlgClientSysInfo::OnLvnGetdispinfoListProcInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	int itemIndex = pItem->iItem;

	if( static_cast<size_t>(itemIndex) >= curListVector_.size() )
		return;

	const std::vector<ListVector>::value_type &val = curListVector_[itemIndex];

	static stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case i8desk::detail::MASK_NAME:
			text = val.col0;
			break;
		case i8desk::detail::MASK_PID:
			text = val.col1;
			break;
		case i8desk::detail:: MASK_MEMERY:
			text = val.col2;
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}
}

BOOL CDlgClientSysInfo::OnEraseBkgnd(CDC* pDC)
{
	__super::OnEraseBkgnd(pDC);

	CRect rcMultiText;
	listProcInfo_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, *pDC, rcMultiText, &outboxLine_);
	listSvrInfo_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, *pDC, rcMultiText, &outboxLine_);
	editSysInfo_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, *pDC, rcMultiText, &outboxLine_);

	return TRUE;
}
// DlgDownloadMakeIdx.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgDownloadMakeIdx.h"


#include "../../../../include/Utility/utility.h"
#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../ui/Skin/SkinMgr.h"
#include "../UI/UIHelper.h"
#include "../../../../include/ui/ImageHelpers.h"


namespace
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
}


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// CDlgDownloadMakeIdx 对话框

IMPLEMENT_DYNAMIC(CDlgDownloadIdx, CNonFrameChildDlg)

CDlgDownloadIdx::CDlgDownloadIdx(const GameTraits::VectorType &games, bool onlyIDX, CWnd* pParent /*=NULL*/)
	: CNonFrameChildDlg(CDlgDownloadIdx::IDD, pParent)
	, games_(games)
	, onlyMakeIDX_(onlyIDX)
	, nIndex_(0)
{

}

CDlgDownloadIdx::~CDlgDownloadIdx()
{
}

void CDlgDownloadIdx::DoDataExchange(CDataExchange* pDX)
{
	CNonFrameChildDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DOWNLOAD_MAKEIDX, wndListGame_);
	DDX_Control(pDX, IDC_EDIT_DOWNLOAD_MAKEIDX_GAMEINFO, wndResultInfo_);
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_MAKEIDX_PROGRESS_POS, wndProgress_);
	DDX_Control(pDX, IDC_EDIT_DOWNLOAD_MAKEIDX_LOG, wndEditLog_);

	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_MAKEIDX_GAME, wndlableGame_);
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_MAKEIDX_PROGRESS, wndlableProgress_);

	
}


BEGIN_MESSAGE_MAP(CDlgDownloadIdx, CNonFrameChildDlg)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_MAKEIDX, &CDlgDownloadIdx::OnMessage)
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CDlgDownloadMakeIdx 消息处理程序

BOOL CDlgDownloadIdx::OnInitDialog()
{
	CNonFrameChildDlg::OnInitDialog();

	// List
	wndListGame_.SetHeaderImage(

		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderBg.png")), 
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderLine.png")));

	wndListGame_.InsertHiddenLabelColumn();

	wndListGame_.InsertColumnTrait(1, _T("GID"),		LVCFMT_LEFT, 60, 1,GetTrait());
	wndListGame_.InsertColumnTrait(2, _T("游戏名"),		LVCFMT_LEFT, 100,2,GetTrait());
	wndListGame_.InsertColumnTrait(3, _T("路径"),		LVCFMT_LEFT, 160,3,GetTrait());
	wndListGame_.InsertColumnTrait(4, _T("大小(M)"),	LVCFMT_LEFT, 60, 4,GetTrait());
	wndListGame_.InsertColumnTrait(5, _T("状态"),		LVCFMT_LEFT, 100,5,GetTrait());

	stdex::tString msg;
	for(size_t nIdx = 0; nIdx != games_.size(); ++nIdx)
	{
		stdex::ToString(games_[nIdx]->GID, msg);
		wndListGame_.InsertItem(nIdx, _T(""));
		wndListGame_.SetItemText(nIdx, 1, msg.c_str());
		wndListGame_.SetItemText(nIdx, 2, games_[nIdx]->Name);
		wndListGame_.SetItemText(nIdx, 3, games_[nIdx]->SvrPath);
	}

	// Progress
	// 设置Progress皮肤
	HBITMAP bk[] = 
	{

		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/LoadingGray_left.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/LoadingGray_mid.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/LoadingGray_right.png"))
	};
	HBITMAP prog[] = 
	{

		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/LoadingBlue_left.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/LoadingBlue_mid.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/LoadingBlue_right.png"))
	};
	wndProgress_.SetRange(0, 1000);
	wndProgress_.SetImages(bk, prog);
	wndProgress_.SetThemeParent(GetSafeHwnd());

	if( games_.empty() )
		return FALSE;

	stdex::tOstringstream os;
	os << games_[nIndex_]->GID << _T(",") << games_[nIndex_]->Name;
	
	wndResultInfo_.SetWindowText(os.str().c_str());

	makeIdx_.StartMakeIdx(games_[nIndex_]->SvrPath, onlyMakeIDX_, this->GetSafeHwnd());

	SetTitle(onlyMakeIDX_ ? _T("制作游戏索引") : _T("校验游戏索引"));

	// 画Static的背景
	wndlableGame_.SetThemeParent(GetSafeHwnd());
	wndlableProgress_.SetThemeParent(GetSafeHwnd());
	wndResultInfo_.SetThemeParent(GetSafeHwnd());

	// Out Line

	outboxLine_.Attach(CopyBitmap(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ControlOutBox.png"))));


	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgDownloadIdx::OnDestroy()
{
	makeIdx_.StopMakeIdx();

	CNonFrameChildDlg::OnDestroy();
}


LRESULT CDlgDownloadIdx::OnMessage(WPARAM wParam, LPARAM lParam)
{
	static DWORD dwSize = 0;

	switch(wParam)
	{
	case IDX_STATUS_ERROR:
		{
			wndListGame_.SetItemText(nIndex_, 5, _T("发生错误"));

			CString msg;
			wndEditLog_.GetWindowText(msg);
			stdex::tOstringstream os;
			os << games_[nIndex_]->GID << _T(": ") << games_[nIndex_]->Name << _T("\r\n")
				 << makeIdx_.GetErrorString() << _T("\r\n\r\n")<< std::ends;
			msg += os.str().c_str();
			wndEditLog_.SetWindowText(msg);

			nIndex_++;
			if( nIndex_ < (int)games_.size() )	//make next game idx.
			{
				os.clear();
				os << games_[nIndex_]->GID << _T(": ") << games_[nIndex_]->Name;
				wndResultInfo_.SetWindowText(os.str().c_str());
	
				makeIdx_.StartMakeIdx(games_[nIndex_]->SvrPath, onlyMakeIDX_, this->GetSafeHwnd());
			}
		}
		break;
	case IDX_STATUS_SCAN:
		{
			wndListGame_.SetItemText(nIndex_, 5, _T("正在扫描目录"));
		}
		break;
	case IDX_STATUS_GETSIZE:
		{
			wndListGame_.SetItemText(nIndex_, 4, stdex::ToString<stdex::tString>(lParam * 1.0 / 1024, 1).c_str());
			wndListGame_.SetItemText(nIndex_, 5, _T("正在生成索引"));

			dwSize = lParam;
		}
		break;
	case IDX_STATUS_CRC:
		wndProgress_.SetPos(lParam);
		break;
	case IDX_STATUS_CRC_START:
		wndProgress_.SetPos(lParam);
		SetTimer(1, 1000, 0);
		break;
	case IDX_STATUS_CRC_END:
		wndProgress_.SetPos(lParam);
		KillTimer(1);
		break;
	case IDX_STATUS_CHECK:
		wndListGame_.SetItemText(nIndex_, 5, _T("正在检验索引"));
		break;
	case IDX_STATUS_FINISH:
		{
			wndProgress_.SetPos(0);
			wndListGame_.SetItemText(nIndex_, 5, onlyMakeIDX_ ? _T("制作索引完成") : _T("检验索引完成"));

			makeIdx_.StopMakeIdx();
			if( !onlyMakeIDX_ )
			{
				CString msg;
				wndEditLog_.GetWindowText(msg);
				stdex::tOstringstream os;
				os << games_[nIndex_]->GID << _T(": ") << games_[nIndex_]->Name << _T("\r\n")
					<< (makeIdx_.GetCheckLog().length() != 0 ? makeIdx_.GetCheckLog() : _T("索引与数据相符."))
					<< _T("\r\n\r\n");

				msg += os.str().c_str();
				wndEditLog_.SetWindowText(msg);
			}
			else
			{
				const GameTraits::ValueType &game = games_[nIndex_];
				game->Size = dwSize;
				game->SvrVer = _time32(NULL);
				CString msg;
				wndEditLog_.GetWindowText(msg);
				stdex::tOstringstream os;
				os << games_[nIndex_]->GID << _T(": ") << games_[nIndex_]->Name << _T("\r\n")
					<<  _T("制作索引完成.")<< _T("\r\n\r\n");

				msg += os.str().c_str();
				wndEditLog_.SetWindowText(msg);
				
				if( !i8desk::GetDataMgr().ModifyData(i8desk::GetDataMgr().GetGames(), game->GID, game, 
					MASK_TGAME_SIZE | MASK_TGAME_SVRVER) )
					return TRUE;
			}
			
			nIndex_++;

			if( nIndex_ < (int)games_.size() )	//make next game idx.
			{
				stdex::tOstringstream os;
				os << games_[nIndex_]->GID << _T(": ") << games_[nIndex_]->Name;
				wndResultInfo_.SetWindowText(os.str().c_str());

				makeIdx_.StartMakeIdx(games_[nIndex_]->SvrPath, onlyMakeIDX_, this->GetSafeHwnd());
			}
		}
		break;
	default:
		break;
	}
	
	return 0;
}

void CDlgDownloadIdx::OnTimer(UINT_PTR nIDEvent)
{
	wndProgress_.SetPos(makeIdx_.GetProgress());

	CNonFrameChildDlg::OnTimer(nIDEvent);
}

BOOL CDlgDownloadIdx::OnEraseBkgnd(CDC* pDC)
{
	__super::OnEraseBkgnd(pDC);

	CRect rcMultiText;
	wndListGame_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, *pDC, rcMultiText, &outboxLine_);
	wndEditLog_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, *pDC, rcMultiText, &outboxLine_);
	
	return TRUE;
}
// DlgDownloadDelGame.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgDownloadDelGame.h"

#include "../Business/ServerBusiness.h"
#include "../Business/BootTaskBusiness.h"
#include "../Business/GameBusiness.h"
#include "../ManagerInstance.h"
#include "../Misc.h"

#include "../../../../include/Utility/utility.h"
#include "../../../../include/win32/Filesystem/FileOperator.hpp"
#include "../ui/Skin/SkinMgr.h"

#include "../../../../include/I8Type.hpp"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgDownloadDelGame 对话框

IMPLEMENT_DYNAMIC(CDlgDownloadDelGame, CNonFrameChildDlg)

CDlgDownloadDelGame::CDlgDownloadDelGame(const GameTraits::VectorType &games, CWnd* pParent /*=NULL*/)
	: CNonFrameChildDlg(CDlgDownloadDelGame::IDD, pParent)
	, curgames_(games)
{

}

CDlgDownloadDelGame::~CDlgDownloadDelGame()
{
}

void CDlgDownloadDelGame::DoDataExchange(CDataExchange* pDX)
{
	CNonFrameChildDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_DOWNLOAD_DELLOCAL, wndCheckDelLocal_);
	DDX_Control(pDX, IDC_CHECK_DOWNLOAD_DELCLIENT, wndCheckDelClient_);
	DDX_Control(pDX, IDC_CHECK_DOWNLOAD_DELSYNC, wndCheckDelSync_);
	DDX_Control(pDX, IDOK, wndBtnOk_);
	DDX_Control(pDX, IDCANCEL, wndBtnCancel_);

}


BEGIN_MESSAGE_MAP(CDlgDownloadDelGame, CNonFrameChildDlg)
	ON_BN_CLICKED(IDOK, &CDlgDownloadDelGame::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgDownloadDelGame::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgDownloadDelGame 消息处理程序

BOOL CDlgDownloadDelGame::OnInitDialog()
{
	CNonFrameChildDlg::OnInitDialog();
	SetTitle(_T("删除游戏"));
	if( !i8desk::IsRunOnServer() )
		wndCheckDelLocal_.EnableWindow(FALSE);

	// CheckBox
	HBITMAP checkBox[] = 
	{

		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/CheckboxNormal.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/CheckboxNormal.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/CheckBoxChecked.png"))
	};
	wndCheckDelLocal_.SetImages(checkBox[0], checkBox[1], checkBox[2]);
	wndCheckDelClient_.SetImages(checkBox[0], checkBox[1], checkBox[2]);
	wndCheckDelSync_.SetImages(checkBox[0], checkBox[1], checkBox[2]);

	wndCheckDelLocal_.SetThemeParent(this->GetSafeHwnd());
	wndCheckDelClient_.SetThemeParent(this->GetSafeHwnd());
	wndCheckDelSync_.SetThemeParent(this->GetSafeHwnd());

	// 画Button的背景
	HBITMAP btn[] = 
	{

		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn_Hover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn_Press.png"))
	};

	wndBtnOk_.SetImages(btn[0],btn[1],btn[2]);
	wndBtnCancel_.SetImages(btn[0],btn[1],btn[2]);

	return TRUE;  // return TRUE unless you set the focus to a control
}



void CDlgDownloadDelGame::OnBnClickedOk()
{
	bool delLocal	= wndCheckDelLocal_.GetCheck() == BST_CHECKED;
	bool delClient	= wndCheckDelClient_.GetCheck() == BST_CHECKED;
	bool delSync	= wndCheckDelSync_.GetCheck() == BST_CHECKED;
	DWORD flag		= i8desk::DelRecord;

	GameTraits::MapType &games = i8desk::GetDataMgr().GetGames();
	for( size_t i = 0; i < curgames_.size(); i++ )
	{
		const GameTraits::ValueType &game = curgames_[i];
		
		::PathAddBackslash(game->SvrPath);
		::PathAddBackslash(game->CliPath);
		stdex::tString svrPath = game->SvrPath;
		stdex::tString cliPath = game->CliPath;

		utility::Strcpy(game->SvrPath,_T(""));
		utility::Strcpy(game->CliPath,_T(""));

	
		if( !svrPath.empty() && delLocal )
			flag |= i8desk::DelLocal;

		if( delSync )
			flag |= i8desk::DelSync;

		if( delClient && !cliPath.empty() )
			flag |= i8desk::DelClient;


		i8desk::GetTaskMgr().DeleteFileOperate(game->GID, flag);
		i8desk::business::Game::Delete(game);
	}
	

	OnOK();
}

void CDlgDownloadDelGame::OnBnClickedCancel()
{
	OnCancel();
}


void CDlgDownloadDelGame::_PreDelete(GameTraits::MapType &games,const GameTraits::ValueType &game)
{
	if( game->GID < MIN_IDC_GID )
	{
		if( !i8desk::GetDataMgr().DelData(games, game->GID) )
			return;
	}
	else
	{
		utility::ClearString(game->SvrPath);
		utility::ClearString(game->CliPath);
		game->SvrVer	= 0;
		game->SvrClick	= 0;
		game->SvrClick2	= 0;
		game->Status	= 0;

		if( !i8desk::GetDataMgr().ModifyData(games, game->GID, game) )
			return;
	}

	if( !i8desk::GetControlMgr().DelIcon(game->GID) )
		return;
}




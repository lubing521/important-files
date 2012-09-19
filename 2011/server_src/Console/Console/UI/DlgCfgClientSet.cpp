#include "stdafx.h"
#include "../Console.h"

#include "DlgCfgClientSet.h"
#include "DlgGongGaoReview.h"

#include "../ManagerInstance.h"
#include "../../../../include/Extend STL/StringAlgorithm.h"

#include "../../../../include/Zip/XUnzip.h"
#include "../../../../include/Utility/Utility.h"
#include "../../../../include/ui/ImageHelpers.h"
#include "../UI/UIHelper.h"
#include "../ui/Skin/SkinMgr.h"
#include "../MessageBox.h"

#include <vector>

#ifdef min
#undef min
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// CDlgOptClientSet 对话框

IMPLEMENT_DYNAMIC(CDlgCfgClientSet, CDialog)

CDlgCfgClientSet::CDlgCfgClientSet(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCfgClientSet::IDD, pParent)
	, textGongGao_(_T(""))
	, textVDiskDir_(_T(""))
	, textGonggaoPic_(_T(""))
	, textIE_(_T(""))
	, gonggaotype_(0)
{
	outboxLine_.Attach(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ControlOutBox.png")));
}

CDlgCfgClientSet::~CDlgCfgClientSet()
{
	outboxLine_.Detach();
}

void CDlgCfgClientSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CFGCLIENTSET_UPDATEPATH, wndComboDrv_);
	DDX_Text(pDX, IDC_COMBO_CFGCLIENTSET_UPDATEPATH, textComboDrv_);

	DDX_Control(pDX, IDC_COMBO_CFGCLIENTSET_MENUSKIN, wndComboSkin_);
	DDX_Control(pDX, IDC_COMBO_CFGCLIENTSET_MENUMODE, wndComboDisplayMode_);
	DDX_Control(pDX, IDC_COMBO_CFGCLIENTSET_MENUSTART, wndComboOperateMode_);


	DDX_Control(pDX, IDC_STATIC_CFGCLIENTSET_MENU, wndMenu_);
	DDX_Control(pDX, IDC_STATIC_CFGCLIENTSET_MENUSKIN, wndMenuSkin_);
	DDX_Control(pDX, IDC_STATIC_CFGCLIENTSET_MENUMODE, wndMenuMode_);
	DDX_Control(pDX, IDC_STATIC_CFGCLIENTSET_MENUSTART, wndMenuStart_);
	DDX_Control(pDX, IDC_STATIC_CFGCLIENTSET_UPDATEPATH, wndUpdatePath_);
	DDX_Control(pDX, IDC_STATIC_CFGCLIENTSET_VDISKTEMPPATH, wndVdiskTmpPath_);
	DDX_Control(pDX, IDC_STATIC_CFGCLIENTSET_REVIEW, wndReview_);
	DDX_Control(pDX, IDC_STATIC_CFGCLIENTSET_GONGGAOSET, wndGonggaoSet_);
	DDX_Control(pDX, IDC_STATIC_CFGCLIENTSET_IE, wndIE_);

	
	DDX_Control(pDX, IDC_STATIC_CFGCLIENTSET_REVIEWPIC, wndReviewPic_);
	DDX_Control(pDX, IDC_STATIC_PIC_CFGCLIENTSET_GONGGAOPIC, wndGonggaoPic_);

	DDX_Control(pDX, IDC_STATIC_CFGCLIENTSET_GONGGAOPICREVIEW, wndGonggaoReview_);

	DDX_Control(pDX, IDC_STATIC_CFGCLIENTSET_MENUCLIENT, wndMenuClient_);


	DDX_Control(pDX, IDC_BUTTON_CFGCLIENTSET_FONT, wndBtnFont_);

	DDX_Control(pDX, IDC_CHECK_CFGCLIENTSET_BROWSEDIR, wndCheckCliDir_);
	DDX_Control(pDX, IDC_CHECK_CFGCLIENTSET_AUTOSTARTMENU, wndAutoStartMenu_);
	DDX_Control(pDX, IDC_CHECK_CFGCLIENTSET_ONLYREAD, wndOnlyRead_);
	DDX_Control(pDX, IDC_CHECK_CFGCLIENTSET_MENUPWD, wndMenuPwd_);


	DDX_Control(pDX, IDC_RADIO_CFGCLIENTSET_AGREE, wndRadioAgree_);
	DDX_Control(pDX, IDC_RADIO_CFGCLIENTSET_CUSTOM, wndRadioCustom_);
	DDX_Control(pDX, IDC_RADIO_CFGCLIENTSET_GONGGAOPIC, wndRadioNetBarPic_);
	DDX_Control(pDX, IDC_RADIO_CFGCLIENTSET_NETBARGONGGAO, wndRadioGongGao_);

	DDX_Control(pDX, IDC_EDIT_CFGCLIENTSET_VDISKTEMPDIR, wndEditVDiskDir_);
	DDX_Text(pDX, IDC_EDIT_CFGCLIENTSET_VDISKTEMPDIR, textVDiskDir_);

	DDX_Control(pDX, IDC_EDIT_CFGCLIENTSET_GONGGAO, wndEditGongGao_);
	DDX_Text(pDX, IDC_EDIT_CFGCLIENTSET_GONGGAO, textGongGao_);

	DDX_Control(pDX, IDC_EDIT_CFGCLIENTSET_NEWPWD, wndEditNewPwd_);

	DDX_Control(pDX, IDC_STATIC_CFGCLIENTSET_PICPARAM, wndPicParam_);

	
	DDX_Control(pDX, IDC_EDIT_CFGCLIENTSET_GONGGAOPIC, wndEditGonggaoPic_);
	DDX_Text(pDX, IDC_EDIT_CFGCLIENTSET_GONGGAOPIC, textGonggaoPic_);
	DDX_Control(pDX, IDC_BUTTON_CFGCLIENTSET_GONGGAOPIC, wndBtnGonggaoPic_);

	DDX_Control(pDX, IDC_STATIC_CFGCLIENTSET_CPU_TEMPRATRUE, wndLabelTemprature_);
	DDX_Control(pDX, IDC_EDIT_CFGCLIENTSET_TEMPRATURE, wndEditTemprature_);
	DDX_Text(pDX, IDC_EDIT_CFGCLIENTSET_TEMPRATURE, textTemperatrue_);
	DDX_Control(pDX, IDC_EDIT_CFGCLIENTSET_IE, wndEditIE_);
	DDX_Text(pDX, IDC_EDIT_CFGCLIENTSET_IE, textIE_);

}

BEGIN_EASYSIZE_MAP(CDlgCfgClientSet)   
END_EASYSIZE_MAP 

BEGIN_MESSAGE_MAP(CDlgCfgClientSet, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_BN_CLICKED(IDC_BUTTON_CFGCLIENTSET_FONT, &CDlgCfgClientSet::OnBnClickedButtonClientsetFont)
	ON_CBN_SELCHANGE(IDC_COMBO_CFGCLIENTSET_MENUSKIN, &CDlgCfgClientSet::OnCbnSelchangeComboMenuSkin)
	ON_BN_CLICKED(IDC_RADIO_CFGCLIENTSET_CUSTOM, &CDlgCfgClientSet::OnBnClickedCheckCustom)
	ON_BN_CLICKED(IDC_RADIO_CFGCLIENTSET_AGREE, &CDlgCfgClientSet::OnBnClickedCheckAgree)
	ON_BN_CLICKED(IDC_RADIO_CFGCLIENTSET_GONGGAOPIC, &CDlgCfgClientSet::OnBnClickedNetBarPic)
	ON_BN_CLICKED(IDC_RADIO_CFGCLIENTSET_NETBARGONGGAO, &CDlgCfgClientSet::OnBnClickedGongGao)
	ON_BN_CLICKED(IDC_CHECK_CFGCLIENTSET_MENUPWD, &CDlgCfgClientSet::OnBnClickedCheckMenuPwd)
	ON_BN_CLICKED(IDC_BUTTON_CFGCLIENTSET_GONGGAOPIC, &CDlgCfgClientSet::OnBnClickedGonggaoPic)
	ON_MESSAGE(WM_MSG_OK, &CDlgCfgClientSet::OnApplyMessage)
	ON_WM_CTLCOLOR()



END_MESSAGE_MAP()

BOOL CDlgCfgClientSet::OnInitDialog()
{
	CDialog::OnInitDialog();
	INIT_EASYSIZE;

	//设置菜单显示模式
	wndComboDisplayMode_.AddString(_T("全屏"));
	wndComboDisplayMode_.AddString(_T("窗口"));
	//菜单游戏启动后自定义
	wndComboOperateMode_.AddString(_T("最小化"));
	wndComboOperateMode_.AddString(_T("关闭"));

	//客户机存放本地游戏盘符
	stdex::tString szDrv;
	for (TCHAR chDrv='D'; chDrv<='Z'; chDrv++)
	{
		szDrv = chDrv;
		stdex::ToUpper(szDrv);
		wndComboDrv_.AddString(szDrv.c_str());
	}

	ZeroMemory(&logfont_, sizeof(logfont_));
	logfont_.lfCharSet = DEFAULT_CHARSET;
	logfont_.lfOutPrecision = OUT_DEVICE_PRECIS;
	logfont_.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfont_.lfQuality = DEFAULT_QUALITY;
	logfont_.lfPitchAndFamily = DEFAULT_PITCH;


	wndReviewPic_.ShowWindow(SW_HIDE);
	wndGonggaoPic_.ShowWindow(SW_HIDE);

	ReadData();
	return TRUE; 
}

void CDlgCfgClientSet::ReadData()
{
	stdex::tString str;
	//设置菜单皮肤文件
	SetComboBarSkin();
	
	str = i8desk::GetDataMgr().GetOptVal(OPT_M_CLISKIN,str);
	if( str.empty() )
		str = _T("默认|默认.png");
	str = str.substr(0, str.find('|'));
	int nitem = wndComboSkin_.SelectString(-1,str.c_str());
	wndComboSkin_.SetCurSel(nitem);
	
	stdex::tOstringstream filename;
	filename << utility::GetAppPath() << _T("Skin/") << str << _T(".png") << std::ends; 

	DrawPic(filename.str().c_str(), wndReviewPic_);

	//设置菜单显示模式
	wndComboDisplayMode_.SetCurSel(i8desk::GetDataMgr().GetOptVal(OPT_M_WINMODE, 0));
	//设置菜单高级设置密码
	if(wndMenuPwd_.GetCheck())
		wndEditNewPwd_.EnableWindow(TRUE);
	else
		wndEditNewPwd_.EnableWindow(FALSE);

	wndEditNewPwd_.LimitText(32);

	//菜单游戏启动后
	switch (i8desk::GetDataMgr().GetOptVal(OPT_M_MENUACT, 0))
	{
	case 0://默认无动作
		OnBnClickedCheckAgree();
		wndComboOperateMode_.SetCurSel(0);
		break;
	case 1://表示最下化
		OnBnClickedCheckCustom();
		wndComboOperateMode_.SetCurSel(0);
		break;
	case 2://表示关闭
		OnBnClickedCheckCustom();
		wndComboOperateMode_.SetCurSel(1);
		break;
	}
	
	//右键浏览目录
	wndCheckCliDir_.SetCheck(i8desk::GetDataMgr().GetOptVal(OPT_M_BROWSEDIR, 1));


	//公告图片路径();
	str = _T("");
	textGonggaoPic_ = i8desk::GetDataMgr().GetOptVal(OPT_M_GGPICDIR, str).c_str();
	DrawPic((LPCTSTR)textGonggaoPic_, wndGonggaoPic_);

	

	// 显示公告的方式（0：图片公告；1：公告文字 2:公告和文字）
	gonggaotype_ = i8desk::GetDataMgr().GetOptVal(OPT_M_SHOWGONGGAOTYPE, 0);

	//公告内容
	str = _T("");
	textGongGao_ = GetGonggao(i8desk::GetDataMgr().GetOptVal(OPT_M_GGMSG, str)).c_str();
	if(textGongGao_.IsEmpty())
	{
		str = _T("\r\n\r\n\r\n 欢迎光临本网吧，请妥善保管好您的财物，以免丢失。\r\n 如有其它需要，请您联系网管，谢谢！");
		SetGonggao(str);
		textGongGao_ = GetGonggao(i8desk::GetDataMgr().GetOptVal(OPT_M_GGMSG, str)).c_str();
	}	

	//公告字体
	str = _T("Microsoft Sans Serif|30|0|0");
	strFontString_ = i8desk::GetDataMgr().GetOptVal(OPT_M_GGFONT,str ).c_str();
	if (gonggaotype_ == i8desk::Pic)
		OnBnClickedNetBarPic();
	else
		OnBnClickedGongGao();
	
	//开机时启动游戏菜单
	wndAutoStartMenu_.SetCheck(i8desk::GetDataMgr().GetOptVal(OPT_M_RUNMENU, 0));

	//仅开机时读取游戏信息
	wndOnlyRead_.SetCheck(i8desk::GetDataMgr().GetOptVal(OPT_M_ONLYONE, 0));
	//客户机存放本地游戏盘符
	str = _T("E");
	str = i8desk::GetDataMgr().GetOptVal(OPT_M_GAMEDRV, str);
	int item = wndComboDrv_.SelectString(-1,str.c_str());
	wndComboDrv_.SetCurSel(item);


	//设置虚拟盘临时目录
	wndEditVDiskDir_.LimitText(255);
	textVDiskDir_ = i8desk::GetDataMgr().GetOptVal(OPT_M_VDDIR,str).c_str();

	// CPU监控温度
	textTemperatrue_ = i8desk::GetDataMgr().GetOptVal(OPT_M_CPU_TEMPRATURE, _T("60")).c_str();

	// IE首页
	str = _T("");
	textIE_ = i8desk::GetDataMgr().GetOptVal(OPT_M_IEURL , str).c_str();

	UpdateData(FALSE);

}

stdex::tString CDlgCfgClientSet::GetGonggao(const stdex::tString &text)
{
	if(utility::IsEmpty(text.c_str()))
		return text;

	std::string src = CT2A(text.c_str());
	int len = ATL::Base64DecodeGetRequiredLength(src.length());
	std::vector<byte> vec(len + 1);

	ATL::Base64Decode(src.data(), len, &vec[0], &len);

	stdex::tString savePath(CA2T((char *)&(vec[0])));
	return savePath;
}

void CDlgCfgClientSet::SetGonggao( const stdex::tString &text)
{
	std::string src = CT2A(text.c_str());
	int len = ATL::Base64EncodeGetRequiredLength(src.length());
	std::vector<char> vec;
	vec.resize(len + 1);

	ATL::Base64Encode((const byte *)src.c_str(), src.length(), &vec[0], &len);

	stdex::tString val = CA2T(&vec[0]);

	i8desk::GetDataMgr().SetOptVal(OPT_M_GGMSG,  val);
}


void CDlgCfgClientSet::SetAdvFont()
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

void CDlgCfgClientSet::SetComboBarSkin()
{
	wndComboSkin_.ResetContent();
	i8desk::data_helper::BarSkinTraits::VectorType barSkins;
	i8desk::GetDataMgr().GetBarSkin(barSkins); 

	for(i8desk::data_helper::BarSkinTraits::VecIterator it= barSkins.begin();	
		it != barSkins.end(); ++it) 
	{ 
		wndComboSkin_.AddString((*it)->Style);
	}
}


void CDlgCfgClientSet::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	UPDATE_EASYSIZE;

}


BOOL CDlgCfgClientSet::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);

	CMemDC memDC(*pDC, rcClient);
	memDC.GetDC().FillSolidRect(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(),
		RGB(255, 255, 255));

	CRect rcText;
	wndEditNewPwd_.GetWindowRect(rcText);
	i8desk::ui::DrawFrame(this, memDC, rcText, &outboxLine_);

	if ( gonggaotype_ != i8desk::Pic)
	{
		wndEditGongGao_.GetWindowRect(rcText);
		i8desk::ui::DrawFrame(this, memDC, rcText, &outboxLine_);
	}

	return TRUE;
}

void CDlgCfgClientSet::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialog::OnSizing(fwSide, pRect);

	// TODO: 在此处添加消息处理程序代码
}



void CDlgCfgClientSet::OnBnClickedButtonClientsetFont()
{
	CFontDialog dlg(&logfont_);
	dlg.m_cf.rgbColors = fontColor_;
	if (dlg.DoModal() == IDOK)
	{
		dlg.GetCurrentFont(&logfont_);
		DWORD flag = 0;

		if (dlg.IsBold())		flag |= (1<<0);
		if (dlg.IsItalic())		flag |= (1<<1);
		if (dlg.IsUnderline())	flag |= (1<<2);
		if (dlg.IsStrikeOut())	flag |= (1<<3);

		strFontString_.Format(_T("%s|%d|%d|%d"), 
			logfont_.lfFaceName, abs(logfont_.lfHeight), dlg.GetColor(), flag);
		font_.DeleteObject();
		font_.CreateFontIndirect(&logfont_);
		fontColor_ = dlg.GetColor();
	}	

	OnBnClickedGongGao();
}

LRESULT CDlgCfgClientSet::OnApplyMessage(WPARAM wParam, LPARAM lParam)
{//确认键回写数据库
	//客户机皮肤风格
	UpdateData(TRUE);

	int nSel = 0;
	if ((nSel = wndComboSkin_.GetCurSel()) == -1) 
		nSel = 0;
	CString skin;
	wndComboSkin_.GetLBText(nSel, skin);
	skin += _T("|") + skin + _T(".png");
	i8desk::GetDataMgr().SetOptVal(OPT_M_CLISKIN,(LPCTSTR)skin);

	//窗口模式
	i8desk::GetDataMgr().SetOptVal(OPT_M_WINMODE, wndComboDisplayMode_.GetCurSel());
	//客户机高级设置密码
	if (wndMenuPwd_.GetCheck())
	{
		CString textNewPwd;
		GetDlgItemText(IDC_EDIT_CFGCLIENTSET_NEWPWD,textNewPwd);
		if (textNewPwd.IsEmpty())
			textNewPwd = _T("1234567");
		stdex::tString password;
		stdex::ToString(utility::CalBufCRC32((LPCTSTR)textNewPwd,textNewPwd.GetLength()*sizeof(TCHAR)),password);
		i8desk::GetDataMgr().SetOptVal(OPT_M_CLIPWD, password.c_str() );
	}
	//菜单游戏启动后
	int act = 0;
	if(wndRadioCustom_.GetCheck())
	{ 
		 if (0 == wndComboOperateMode_.GetCurSel())
			 act = 1 ;
		 else
			 act = 2;
	}
	i8desk::GetDataMgr().SetOptVal(OPT_M_MENUACT, act);

	//右键浏览目录
	i8desk::GetDataMgr().SetOptVal(OPT_M_BROWSEDIR, wndCheckCliDir_.GetCheck());
	//开机自动启动菜单
	i8desk::GetDataMgr().SetOptVal(OPT_M_RUNMENU, wndAutoStartMenu_.GetCheck());
	//仅适用无盘
	i8desk::GetDataMgr().SetOptVal(OPT_M_ONLYONE, wndOnlyRead_.GetCheck());
	//客户机存放本地游戏盘符
	nSel = 0;
	TCHAR buf[10] = {0};
	nSel = wndComboDrv_.GetCurSel();
	if (nSel == -1) nSel = 1;
	//buf[10] = {0};
	wndComboDrv_.GetLBText(nSel, buf);
	i8desk::GetDataMgr().SetOptVal(OPT_M_GAMEDRV, buf);
	//虚拟盘临时目录
	i8desk::GetDataMgr().SetOptVal(OPT_M_VDDIR, (LPCTSTR)textVDiskDir_);


	// 显示公告的方式（0：图片公告；1：公告文字 2:公告和文字）
	i8desk::GetDataMgr().SetOptVal(OPT_M_SHOWGONGGAOTYPE, gonggaotype_);
	if( gonggaotype_ == i8desk::Pic )
	{
		//公告图片路径
		i8desk::GetDataMgr().SetOptVal(OPT_M_GGPICDIR, (LPCTSTR)textGonggaoPic_);
	}
	else
	{
		//公告内容
		stdex::tString text = textGongGao_;
		SetGonggao(text);
		//公告字体
		i8desk::GetDataMgr().SetOptVal(OPT_M_GGFONT, (LPCTSTR)strFontString_);
	}


	// CPU监控温度
	CString cpuTemprature;
	wndEditTemprature_.GetWindowText(cpuTemprature);
	i8desk::GetDataMgr().SetOptVal(OPT_M_CPU_TEMPRATURE, (LPCTSTR)cpuTemprature);

	// IE首页
	i8desk::GetDataMgr().SetOptVal(OPT_M_IEURL, (LPCTSTR)textIE_);

	return TRUE;

}

void CDlgCfgClientSet::OnBnClickedNetBarPic()
{
	wndRadioGongGao_.SetCheck(FALSE);
	wndRadioNetBarPic_.SetCheck(TRUE); 
	wndEditGonggaoPic_.ShowWindow(SW_SHOW);
	wndPicParam_.ShowWindow(SW_SHOW);
	wndBtnGonggaoPic_.ShowWindow(SW_SHOW);	
	wndGonggaoReview_.ShowWindow(SW_SHOW);	
	wndGonggaoPic_.ShowWindow(SW_SHOW);	
	wndEditGongGao_.ShowWindow(SW_HIDE);
	wndBtnFont_.ShowWindow(SW_HIDE);

	gonggaotype_ = i8desk::Pic;
	Invalidate();

}

void CDlgCfgClientSet::OnBnClickedGongGao()
{

	wndRadioGongGao_.SetCheck(TRUE);
	wndRadioNetBarPic_.SetCheck(FALSE); 

	wndEditGonggaoPic_.ShowWindow(SW_HIDE);
	wndPicParam_.ShowWindow(SW_HIDE);
	wndBtnGonggaoPic_.ShowWindow(SW_HIDE);	
	wndGonggaoReview_.ShowWindow(SW_HIDE);	
	wndGonggaoPic_.ShowWindow(SW_HIDE);	

	wndEditGongGao_.ShowWindow(SW_SHOW);
	wndBtnFont_.ShowWindow(SW_SHOW);

	gonggaotype_ = i8desk::Text;
	SetAdvFont();
	Invalidate();

}


void CDlgCfgClientSet::OnBnClickedCheckCustom()
{
	wndRadioAgree_.SetCheck(FALSE);
	wndRadioCustom_.SetCheck(TRUE); 
	wndComboOperateMode_.EnableWindow(TRUE);

}

void CDlgCfgClientSet::OnBnClickedCheckAgree()
{
	wndRadioAgree_.SetCheck(TRUE);
	wndRadioCustom_.SetCheck(FALSE); 
	wndComboOperateMode_.EnableWindow(FALSE);

}

void CDlgCfgClientSet::OnBnClickedCheckMenuPwd()
{
	if (wndMenuPwd_.GetCheck())
		wndEditNewPwd_.EnableWindow(TRUE);
	else
		wndEditNewPwd_.EnableWindow(FALSE);
}



void CDlgCfgClientSet::OnBnClickedGonggaoPic()
{
	UpdateData(TRUE);

	if( textGonggaoPic_.Right(1) == _T("\\") )
		textGonggaoPic_.Delete(textGonggaoPic_.GetLength() - 1);

	CFileDialog fileDlg(TRUE, _T(""), _T(""), 4|2, _T("所有文件(*.*)|*.*||"));
	CString fileName;
	fileDlg.m_ofn.lpstrInitialDir = textGonggaoPic_;
	if( fileDlg.DoModal() == IDOK )
		fileName = fileDlg.GetPathName();
	else
		return;

	textGonggaoPic_ = fileName;

	if(!textGonggaoPic_.IsEmpty())
		DrawPic((LPCTSTR)textGonggaoPic_, wndGonggaoPic_);

	UpdateData(FALSE);
}

void CDlgCfgClientSet::OnCbnSelchangeComboMenuSkin()
{
	int nSel = 0;
	if ((nSel = wndComboSkin_.GetCurSel()) == -1) 
		nSel = 0;
	TCHAR buf[MAX_PATH] = {0};
	wndComboSkin_.GetLBText(nSel, buf);

	stdex::tOstringstream filename;
	filename << utility::GetAppPath() << _T("Skin/") << buf << _T(".png") << std::ends; 

	if( i8desk::IsRunOnServer() )
	{
		DrawPic(filename.str().c_str(), wndReviewPic_);
	}

}

void CDlgCfgClientSet::DrawPic( LPCTSTR filename, i8desk::ui::SkinImage &wndpic)
{
	std::ifstream file(filename, std::ios_base::in |std::ios_base::binary);
	if( !file )
		return ;

	file.seekg(0, std::ios_base::end);
	std::streamoff len = file.tellg();
	file.seekg(0, std::ios_base::beg);

	assert(len != 0);
	std::vector<char> buf;
	buf.resize(static_cast<size_t>(len));
	
	file.read(&buf[0], len);


	HBITMAP bitmap = AtlLoadGdiplusImage(&buf[0], static_cast<size_t>(len));
	if(bitmap != NULL )
	{
		wndpic.SetImage(bitmap, false);
		wndpic.ShowWindow(SW_SHOW);
	}
}


HBRUSH CDlgCfgClientSet::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_EDIT && pWnd->GetDlgCtrlID() == IDC_EDIT_CFGCLIENTSET_GONGGAO)
	{
		pDC->SetTextColor(fontColor_);
	}

	return hbr;
}



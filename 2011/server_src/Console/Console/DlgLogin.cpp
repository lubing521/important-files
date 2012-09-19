// DlgLogin.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "DlgLogin.h"


#include "Misc.h"
#include "../../../include/ui/ImageHelpers.h"
#include "ui/UIHelper.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// CDlgLogin 对话框

IMPLEMENT_DYNAMIC(CDlgLogin, CDialogEx)

CDlgLogin::CDlgLogin(const LoginCallback &connect, const LoginCallback &password, CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgLogin::IDD, pParent)
	, serverIP_(_T(""))
	, serverPass_(_T(""))
	, connect_(connect)
	, password_(password)
{
	
}

CDlgLogin::~CDlgLogin()
{
}

void CDlgLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LOGIN_IP, serverIP_);
	DDX_Text(pDX, IDC_EDIT_LOGIN_PASSWORD, serverPass_);
	DDX_Control(pDX, IDC_EDIT_LOGIN_IP, wndEditName_);
	DDX_Control(pDX, IDC_EDIT_LOGIN_PASSWORD, wndEditPassword_);
	DDX_Control(pDX, IDC_STATIC_LOGIN_BK, bk_);
	DDX_Control(pDX, IDC_STATIC_LOGIN_TIP, tip_);
	DDX_Control(pDX, IDC_STATIC_LOGIN_SERVER_NAME, serverName_);
	DDX_Control(pDX, IDC_STATIC_LOGIN_PASSWORD, pwd_);
	DDX_Control(pDX, IDOK, ok_);
	DDX_Control(pDX, IDCANCEL, cancel_);
}


BEGIN_MESSAGE_MAP(CDlgLogin, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgLogin::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgLogin::OnBnClickedCancel)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()

END_MESSAGE_MAP()


// CDlgLogin 消息处理程序
BOOL CDlgLogin::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

	serverIP_ = AfxGetApp()->GetProfileString(_T("Option"), _T("SvrIP"), _T("127.0.0.1"));
	wndEditPassword_.SetFocus();
	titleText_ = _T("i8控制台");
	SetWindowText(titleText_);

	HGDIOBJ hFont = ::GetStockObject(DEFAULT_GUI_FONT); 
	font_.Attach((HFONT)hFont);

	title_.Attach(AtlLoadGdiplusImage(IDB_PNG_WNDTOPBG,			_T("PNG")));
	content_.Attach(AtlLoadGdiplusImage(IDB_PNG_WNDCONTENTBG,	_T("PNG")));

	static CImage img;
	if( img.IsNull() )
		img.Attach(AtlLoadGdiplusImage(IDB_PNG_WNDTOPCLOSE,		_T("PNG")));

	CRect rcBtn;
	GetClientRect(rcBtn);
	rcBtn.left = rcBtn.right - img.GetWidth() - 10;
	rcBtn.right = rcBtn.left + img.GetWidth();
	rcBtn.top = rcBtn.top + 8;
	rcBtn.bottom = rcBtn.top + img.GetHeight();

	wndBtnClose_.Create(_T(""), WS_CHILD | WS_VISIBLE, rcBtn, this, IDCANCEL);
	wndBtnClose_.SetImages(
		AtlLoadGdiplusImage(IDB_PNG_WNDTOPCLOSE,		_T("PNG")),
		AtlLoadGdiplusImage(IDB_PNG_WNDTOPCLOSEHOVER,	_T("PNG")),
		AtlLoadGdiplusImage(IDB_PNG_WNDTOPCLOSECLICK,	_T("PNG")));

	bk_.SetImage(AtlLoadGdiplusImage(IDB_PNG_BK, _T("PNG")));

	// Edit
	wndEditName_.SetBkImg(AtlLoadGdiplusImage(IDB_PNG_INPUTBG, _T("PNG")));
	
	HBITMAP edit[] =
	{
		AtlLoadGdiplusImage(IDB_PNG_INPUTLEFT,	_T("PNG")),
		AtlLoadGdiplusImage(IDB_PNG_INPUTMID,	_T("PNG")),
		AtlLoadGdiplusImage(IDB_PNG_INPUTRIGHT, _T("PNG")),
	};
	wndEditName_.SetImages(edit[0], edit[1], edit[2]);

	// Button
	HBITMAP ok[] = 
	{
		AtlLoadGdiplusImage(IDB_PNG_OK_NORMAL, _T("PNG")),
		AtlLoadGdiplusImage(IDB_PNG_OK_HOVER, _T("PNG")),
		AtlLoadGdiplusImage(IDB_PNG_OK_PRESS, _T("PNG"))
	};
	ok_.SetImages(ok[0], ok[1], ok[2]);

	HBITMAP cancel[] = 
	{
		AtlLoadGdiplusImage(IDB_PNG_CANCEL_NORMAL, _T("PNG")),
		AtlLoadGdiplusImage(IDB_PNG_CANCEL_HOVER, _T("PNG")),
		AtlLoadGdiplusImage(IDB_PNG_CANCEL_PRESS, _T("PNG"))
	};
	cancel_.SetImages(cancel[0], cancel[1], cancel[2]);

	// Static
	serverName_.SetThemeParent(GetSafeHwnd());
	pwd_.SetThemeParent(GetSafeHwnd());
	tip_.SetThemeParent(GetSafeHwnd());

	ok_.SetThememParent(GetSafeHwnd());
	cancel_.SetThememParent(GetSafeHwnd());

	outBox_.Attach(AtlLoadGdiplusImage(IDB_PNG_OUTBOX, _T("PNG")));


	CRgn rgn;
	CRect rcClient;
	GetClientRect(rcClient);
	rgn.CreateRoundRectRgn(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, 5, 5);
	SetWindowRgn(rgn, FALSE);

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
}


void CDlgLogin::OnBnClickedOk()
{
	UpdateData();

	if( !i8desk::IsValidIP(serverIP_) )
	{
		MessageBox(_T("IP设置错误!"));
		wndEditName_.SetFocus();
		return;
	}

	stdex::tString ip((LPCTSTR)serverIP_);
	if( !connect_(std::tr1::cref(ip)) )
	{
		MessageBox(_T("不能连接上服务器"));
		wndEditName_.SetFocus();
		return;
	}

	stdex::tString psd = (LPCTSTR)serverPass_;
	if( !password_(std::tr1::cref(psd)) )
	{
		MessageBox(_T("密码错误,请重新输入"));
		wndEditPassword_.SetFocus();
		return;
	}

	AfxGetApp()->WriteProfileString(_T("Option"), _T("SvrIP"), serverIP_);
	OnOK();
}

void CDlgLogin::OnBnClickedCancel()
{
	OnCancel();
}

void CDlgLogin::OnPaint()
{
	CPaintDC dc(this);

	CRgn rgn;
	CRect rcClient;
	GetClientRect(rcClient);
	rgn.CreateRoundRectRgn(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, 5, 5);
	SetWindowRgn(rgn, FALSE);
}

BOOL CDlgLogin::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);
	CMemDC memDC(*pDC, rcClient);

	// BK
	CRect rcTitle(rcClient.left, rcClient.top, rcClient.right, title_.GetHeight());
	title_.Draw(memDC.GetDC(), rcTitle);

	CRect rcContent(rcTitle.left, rcTitle.bottom, rcClient.right, rcClient.bottom);
	content_.Draw(memDC.GetDC(), rcContent);

	// Title
	CRect rcText(rcTitle.left + 10, rcTitle.top, rcTitle.right, rcTitle.bottom);
	memDC.GetDC().SetBkMode(TRANSPARENT);

	HGDIOBJ fontOld = memDC.GetDC().SelectObject(font_);
	memDC.GetDC().DrawText(titleText_, rcText, DT_VCENTER | DT_LEFT | DT_SINGLELINE);
	memDC.GetDC().SelectObject(fontOld);

	CRect rcWindow;
	wndEditPassword_.GetWindowRect(rcWindow);
	i8desk::ui::DrawFrame(this, memDC, rcWindow, &outBox_);


	return TRUE;
}

LRESULT CDlgLogin::OnNcHitTest(CPoint point)
{
	CRect rc;
	GetClientRect(&rc);
	ClientToScreen(&rc);

	return rc.PtInRect(point) ? HTCAPTION : CDialog::OnNcHitTest(point);
}
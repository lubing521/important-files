// WZDownloadBoard.cpp : 实现文件
//

#include "stdafx.h"
#include "WZDownloadBoard.h"
#include "BrowseDirDlg.h"

// CWZDownloadBoard 对话框

IMPLEMENT_DYNAMIC(CWZDownloadBoard, CDialog)

CWZDownloadBoard::CWZDownloadBoard(CWnd* pParent /*=NULL*/)
	: CDialog(CWZDownloadBoard::IDD, pParent)
{

}

CWZDownloadBoard::~CWZDownloadBoard()
{
}

void CWZDownloadBoard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RUNTYPE, m_cbRuntype);
}

static const char *DownSpeeds[] = {
	"80",
	"150",
	"300",
	"550",
	"1100",
	"2000",
	"4000",
	"8200",
	"16500",
	"不限制"
};
static const size_t nDownSpeeds = sizeof(DownSpeeds)/sizeof(char *);

BOOL CWZDownloadBoard::OnInitDialog()
{
	CDialog::OnInitDialog();

	std::string	    ErrInfo;

	SetDlgItemText(IDC_INITDIR, GetDbMgr()->GetOptString(OPT_D_INITDIR, "E:\\").c_str());
	SetDlgItemText(IDC_DOWNTEMPDIR,	  GetDbMgr()->GetOptString(OPT_D_TEMPDIR, "E:\\msctmp\\").c_str());
	
	CComboBox *pTaskNumCtrl = (CComboBox *)GetDlgItem(IDC_TASKNUM);
	pTaskNumCtrl->AddString("1");
	pTaskNumCtrl->AddString("2");
	pTaskNumCtrl->AddString("3");

	int nTaskNum = GetDbMgr()->GetOptInt(OPT_D_TASKNUM);
	if (nTaskNum < 1 || nTaskNum > 3)
	{
		nTaskNum = 3;
		GetDbMgr()->SetOption(OPT_D_TASKNUM, nTaskNum);
	}
	pTaskNumCtrl->SetCurSel(nTaskNum - 1);

	//限速
	CComboBox *dwonspeed = (CComboBox *)GetDlgItem(IDC_DOWNSPEED);
	for (size_t i = 0; i < nDownSpeeds; i++)
		dwonspeed->AddString(DownSpeeds[i]);

	int nValue = GetDbMgr()->GetOptInt(OPT_D_SPEED);
	if (nValue) {
		for (size_t i = 0; i < nDownSpeeds; i++) {
			if (nValue <= atoi(DownSpeeds[i])) {
				dwonspeed->SetCurSel(i);
				break;
			}
		}
	}

	if (dwonspeed->GetCurSel() == -1) {
		dwonspeed->SelectString(0, "不限制");
	}

	return TRUE;
}


BEGIN_MESSAGE_MAP(CWZDownloadBoard, CDialog)
	ON_REGISTERED_MESSAGE(i8desk::g_nOptApplyMsg, &CWZDownloadBoard::OnApplyMessage)
	ON_CBN_SELCHANGE(IDC_TASKNUM, &CWZDownloadBoard::OnCbnSelchangeTasknum)
	ON_CBN_SELCHANGE(IDC_DOWNSPEED, &CWZDownloadBoard::OnCbnSelchangeDownspeed)
	ON_EN_CHANGE(IDC_INITDIR, &CWZDownloadBoard::OnEnChangeInitdir)
	ON_EN_CHANGE(IDC_DOWNTEMPDIR, &CWZDownloadBoard::OnEnChangeDowntempdir)
	ON_CBN_SELCHANGE(IDC_RUNTYPE, &CWZDownloadBoard::OnCbnSelchangeRuntype)
	ON_BN_CLICKED(IDC_SELECT_INITDIR, &CWZDownloadBoard::OnBnClickedSelectInitdir)
	ON_BN_CLICKED(IDC_SELECT_MSCTMPDIR, &CWZDownloadBoard::OnBnClickedSelectMsctmpdir)
END_MESSAGE_MAP()


// CWZDownloadBoard 消息处理程序

LRESULT CWZDownloadBoard::OnApplyMessage(WPARAM wParam, LPARAM lParam)
{
	m_strTempDir.Trim();
	if (!m_strTempDir.IsEmpty())
	{
		if (!i8desk::IsValidDirName((LPCSTR)m_strTempDir))
		{
			AfxMessageBox("下载临时目录路径不是有效的路径名.");
			return FALSE;
		}
		if (m_strTempDir[m_strTempDir.GetLength() - 1] != _T('\\')) {
			m_strTempDir += _T("\\");
		}
		GetDbMgr()->SetOption(OPT_D_TEMPDIR, (LPCSTR)m_strTempDir);
		m_strTempDir = _T("");
	}

	m_strInitDir.Trim();
	if (!m_strInitDir.IsEmpty())
	{
		if (!i8desk::IsValidDirName((LPCSTR)m_strInitDir))
		{
			AfxMessageBox("下载初始目录路径不是有效的路径名.");
			return FALSE;
		}
		if (m_strInitDir[m_strInitDir.GetLength() - 1] != _T('\\')) {
			m_strInitDir += _T("\\");
		}
		GetDbMgr()->SetOption(OPT_D_INITDIR, (LPCSTR)m_strInitDir);
		m_strInitDir = _T("");
	}

	m_strTaskNum.Trim();
	if (!m_strTaskNum.IsEmpty())
	{
		int nValue = ::atoi((LPCTSTR)m_strTaskNum);
		if (nValue > 3 || nValue < 1)
		{
			AfxMessageBox("任务数只能设置为1-3.");
			return FALSE;
		}
		GetDbMgr()->SetOption(OPT_D_TASKNUM, nValue);
		m_strTaskNum = _T("");
	}

	m_strDownSpeed.Trim();
	if (!m_strDownSpeed.IsEmpty())
	{
		int nSpeedValue = ::atoi((LPCTSTR)m_strDownSpeed);
		if (0)//nSpeedValue != 0 && nSpeedValue < 60)
		{
			AfxMessageBox("最高下载速度设置值不能低于60KB/s.");
			return FALSE;
		}	
		GetDbMgr()->SetOption(OPT_D_SPEED, nSpeedValue);
		m_strDownSpeed = _T("");
	}

	return TRUE;
}

void CWZDownloadBoard::OnCbnSelchangeTasknum()
{
	GetDlgItem(IDC_TASKNUM)->GetWindowText(m_strTaskNum);
}

void CWZDownloadBoard::OnCbnSelchangeDownspeed()
{
	GetDlgItem(IDC_DOWNSPEED)->GetWindowText(m_strDownSpeed);
}

void CWZDownloadBoard::OnEnChangeInitdir()
{
	GetDlgItem(IDC_INITDIR)->GetWindowText(m_strInitDir);
}

void CWZDownloadBoard::OnEnChangeDowntempdir()
{
	GetDlgItem(IDC_DOWNTEMPDIR)->GetWindowText(m_strTempDir);
}

void CWZDownloadBoard::OnCbnSelchangeRuntype()
{
}

static char buf[MAX_PATH] = {0};
static int CALLBACK SetSelect_CB(HWND   win,   UINT   msg,   LPARAM   param,   LPARAM   data)   
{   
	if (msg == BFFM_INITIALIZED)
	{
		::SendMessage(win, BFFM_SETSELECTION, TRUE, (LPARAM)buf);
	}
	return 0;
}

void CWZDownloadBoard::OnBnClickedSelectInitdir()
{
	if (GetConsoleDlg()->IsRunOnServer())
	{
		BROWSEINFO info;
		memset(&info, 0, sizeof(info));
		info.hwndOwner = m_hWnd;
		info.lpszTitle = "设置默认下载目录:";
		GetDlgItemText(IDC_INITDIR, buf, MAX_PATH);
		info.lpfn = SetSelect_CB;

		LPITEMIDLIST lst = SHBrowseForFolder(&info);
		if (lst != NULL)
		{
			SHGetPathFromIDList(lst, buf);
			LPMALLOC pMalloc = NULL;
			SHGetMalloc(&pMalloc);
			pMalloc->Free(lst);
			pMalloc->Release();
			SetDlgItemText(IDC_INITDIR, buf);
			m_strInitDir = buf;
		}
	}
	else
	{
		CBrowseDirDlg dlg;
		dlg.m_bBrowseDir = TRUE;
		dlg.m_strDirOrFile = m_strInitDir;
		if (dlg.DoModal() != IDOK)
			return;
		m_strInitDir = dlg.m_strDirOrFile;
	}
}

void CWZDownloadBoard::OnBnClickedSelectMsctmpdir()
{
	if (GetConsoleDlg()->IsRunOnServer())
	{
		BROWSEINFO info;
		memset(&info, 0, sizeof(info));
		info.hwndOwner = m_hWnd;
		info.lpszTitle = "设置下载临时目录:";
		GetDlgItemText(IDC_DOWNTEMPDIR, buf, MAX_PATH);
		info.lpfn = SetSelect_CB;

		LPITEMIDLIST lst = SHBrowseForFolder(&info);
		if (lst != NULL)
		{
			SHGetPathFromIDList(lst, buf);
			LPMALLOC pMalloc = NULL;
			SHGetMalloc(&pMalloc);
			pMalloc->Free(lst);
			pMalloc->Release();
			SetDlgItemText(IDC_DOWNTEMPDIR, buf);
			m_strTempDir = buf;
		}
	}		
	else 
	{
		CBrowseDirDlg dlg;
		dlg.m_bBrowseDir = TRUE;
		dlg.m_strDirOrFile = m_strTempDir;
		if (dlg.DoModal() != IDOK)
			return;
		m_strTempDir = dlg.m_strDirOrFile;
	}

}


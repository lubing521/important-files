// PenetateFile.cpp : 实现文件
//

#include "stdafx.h"
#include "PenetateFile.h"
#include "ConsoleDlg.h"
#include "DirMakeIdxDlg.h"
#include "BrowseDirDlg.h"

// CPenetateFile 对话框

IMPLEMENT_DYNAMIC(CPenetateFile, CDialog)

CPenetateFile::CPenetateFile(CWnd* pParent /*=NULL*/)
	: CDialog(CPenetateFile::IDD, pParent)
{
	m_bMakeIdx = GetConsoleDlg()->IsRunOnServer();
	m_strGameDriver = GetDbMgr()->GetOptString(OPT_M_GAMEDRV, "E");
}

CPenetateFile::~CPenetateFile()
{
}

void CPenetateFile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SRC_DIR, m_strSrcDir);
	DDX_Text(pDX, IDC_DEST_DIR, m_strDestDir);
	DDX_Check(pDX, IDC_CHECK, m_bMakeIdx);
}


BEGIN_MESSAGE_MAP(CPenetateFile, CDialog)
	ON_BN_CLICKED(IDC_SELECT, &CPenetateFile::OnBnClickedSelect)
	ON_BN_CLICKED(IDOK, &CPenetateFile::OnBnClickedOk)
	ON_EN_CHANGE(IDC_SRC_DIR, &CPenetateFile::OnEnChangeSrcDir)
	ON_EN_CHANGE(IDC_DEST_DIR, &CPenetateFile::OnEnChangeDestDir)
END_MESSAGE_MAP()

BOOL CPenetateFile::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_CHECK)->EnableWindow(GetConsoleDlg()->IsRunOnServer());

	return TRUE;
}


static char s_buf[MAX_PATH] = {0};	//保存浏览后的目录.

static int CALLBACK   SetSelect_CB(HWND   win,   UINT   msg,   LPARAM   param,   LPARAM   data)   
{   
	if (msg == BFFM_INITIALIZED)
	{
		::SendMessage(win, BFFM_SETSELECTION, TRUE, (LPARAM)s_buf);
	}
	return 0;
}   

// CPenetateFile 消息处理程序

void CPenetateFile::OnBnClickedSelect()
{
	if (GetConsoleDlg()->IsRunOnServer())
	{
		BROWSEINFO info = {0};
		info.hwndOwner = m_hWnd;
		info.lpszTitle = "选择要穿透的目录:";
		CString xPath;
		GetDlgItemText(IDC_SRC_DIR, xPath);
		if (!xPath.IsEmpty())
			lstrcpy(s_buf, xPath);
		info.lpfn = SetSelect_CB;

		LPITEMIDLIST lst = SHBrowseForFolder(&info);
		if (!lst)
			return;

		SHGetPathFromIDList(lst, s_buf);
		LPMALLOC pMalloc = NULL;
		SHGetMalloc(&pMalloc);
		pMalloc->Free(lst);
		pMalloc->Release();

		m_strDestDir = m_strSrcDir = s_buf;
	}
	else 
	{
		CBrowseDirDlg dlg;
		dlg.m_bBrowseDir = TRUE;
		dlg.m_strDirOrFile = m_strSrcDir;
		if (dlg.DoModal() != IDOK)
			return;
		m_strDestDir = m_strSrcDir = dlg.m_strDirOrFile;
	}

	if (!m_strGameDriver.empty() && !m_strDestDir.IsEmpty())
	{
		m_strDestDir.SetAt(0, m_strGameDriver[0]);
	}

	UpdateData(FALSE);
}

void CPenetateFile::OnBnClickedOk()
{
	UpdateData();

	if (!i8desk::IsValidDirName((LPCTSTR)m_strSrcDir))
	{
		AfxMessageBox("源路径无效!");
		return;
	}

	if (!i8desk::IsValidDirName((LPCTSTR)m_strDestDir))
	{
		AfxMessageBox("目标路径无效!");
		return;
	}
	
	if (m_bMakeIdx)
	{
		CDirMakeIdxDlg dlg(m_strSrcDir);
		dlg.DoModal();
	}

	OnOK();
}



void CPenetateFile::OnEnChangeSrcDir()
{
	UpdateData();
	m_strDestDir = m_strSrcDir;
	if (!m_strGameDriver.empty() && !m_strDestDir.IsEmpty())
	{
		m_strDestDir.SetAt(0, m_strGameDriver[0]);
	}
}

void CPenetateFile::OnEnChangeDestDir()
{
}

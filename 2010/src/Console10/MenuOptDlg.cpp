// MenuOptDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "MenuOptDlg.h"
#include "ConsoleDlg.h"

#include "FindFile.h"
#include "xunzip.h"
// CMenuOptDlg 对话框

IMPLEMENT_DYNAMIC(CMenuOptDlg, CDialog)

CMenuOptDlg::CMenuOptDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMenuOptDlg::IDD, pParent)
{
	m_bModify = FALSE;
}

CMenuOptDlg::~CMenuOptDlg()
{
}

void CMenuOptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_SETPWD, m_btnSetMenuPwd);
}

BEGIN_MESSAGE_MAP(CMenuOptDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CMenuOptDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMenuOptDlg::OnBnClickedCancel)
	ON_REGISTERED_MESSAGE(i8desk::g_nOptApplyMsg, OnApplyMessage)

	ON_BN_CLICKED(IDC_WINMODE, &CMenuOptDlg::OnBnClickedWinmode)
	ON_BN_CLICKED(IDC_BROWSEDIR, &CMenuOptDlg::OnBnClickedWinmode)
	ON_BN_CLICKED(IDC_HOTCOUNT, &CMenuOptDlg::OnBnClickedWinmode)
	ON_CBN_SELCHANGE(IDC_SKIN, &CMenuOptDlg::OnBnClickedWinmode)
	ON_CBN_SELCHANGE(IDC_ACTRUNMENU, &CMenuOptDlg::OnBnClickedWinmode)
	ON_EN_CHANGE(IDC_CLIPASSWORD, &CMenuOptDlg::OnBnClickedWinmode)

	ON_BN_CLICKED(IDC_RUNMENU, &CMenuOptDlg::OnBnClickedWinmode)
	ON_BN_CLICKED(IDC_RUNSHELL, &CMenuOptDlg::OnBnClickedWinmode)
	ON_BN_CLICKED(IDC_ONLYREAD, &CMenuOptDlg::OnBnClickedWinmode)
	ON_BN_CLICKED(IDC_CLIAUTOUPT, &CMenuOptDlg::OnBnClickedWinmode)
	ON_BN_CLICKED(IDC_USE_UDISK, &CMenuOptDlg::OnBnClickedWinmode)
	
	ON_EN_CHANGE(IDC_VDISKTEMP, &CMenuOptDlg::OnBnClickedWinmode)
	ON_EN_CHANGE(IDC_IEURL, &CMenuOptDlg::OnBnClickedWinmode)
	ON_EN_CHANGE(IDC_EDIT_HOTCOUNT, &CMenuOptDlg::OnBnClickedWinmode)
	ON_BN_CLICKED(IDC_CHECK_SETPWD, &CMenuOptDlg::OnBnClickedCheckSetpwd)
	ON_CBN_SELCHANGE(IDC_MENU_MODE, &CMenuOptDlg::OnCbnSelchangeMenuMode)
END_MESSAGE_MAP()

BOOL CMenuOptDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CWaitCursor wc;

	GetDlgItem(IDC_CLIPASSWORD)->EnableWindow(FALSE);
	i8desk::CDbMgr* pDbMgr = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd())->m_pDbMgr;
	
	((CEdit*)GetDlgItem(IDC_EDIT_HOTCOUNT))->LimitText(3);
	((CEdit*)GetDlgItem(IDC_CLIPASSWORD))->LimitText(32);
	((CEdit*)GetDlgItem(IDC_VDISKTEMP))->LimitText(255);
	((CEdit*)GetDlgItem(IDC_IEURL))->LimitText(255);

	((CButton*)GetDlgItem(IDC_WINMODE))->SetCheck(pDbMgr->GetOptInt(OPT_M_WINMODE,   0));

	((CComboBox*)GetDlgItem(IDC_MENU_MODE))->AddString("全屏");
	((CComboBox*)GetDlgItem(IDC_MENU_MODE))->AddString("窗口");
	((CComboBox*)GetDlgItem(IDC_MENU_MODE))->SetCurSel(GetDbMgr()->GetOptInt(OPT_M_WINMODE, 0));
	((CButton*)GetDlgItem(IDC_BROWSEDIR))->SetCheck(pDbMgr->GetOptInt(OPT_M_BROWSEDIR, 1));
	int nHotCount = pDbMgr->GetOptInt(OPT_M_HOTCOUNT, 20);
	((CButton*)GetDlgItem(IDC_HOTCOUNT)) ->SetCheck(nHotCount > 0 ? BST_CHECKED : BST_UNCHECKED);
	SetDlgItemInt(IDC_EDIT_HOTCOUNT, nHotCount);

	std::string ErrInfo;
	if (!pDbMgr->GetAllBarSkin(m_BarSkins, ErrInfo)) {
		::AfxMessageBox(_T("取菜单皮肤列表失败!"));
	}
	

	i8desk::std_string strDefaultSkin = pDbMgr->GetOptString(OPT_M_CLISKIN);
	strDefaultSkin = strDefaultSkin.substr(0, strDefaultSkin.find('|'));
	if( strDefaultSkin == _T("默认") )
		strDefaultSkin = _T("Skin");

	CComboBox *pSkinCtrl = (CComboBox*)GetDlgItem(IDC_SKIN);
	if (m_BarSkins.empty())
	{
		// 检测皮肤文件
		FindFile zipFile;
		i8desk::std_string strSkinPath = i8desk::GetAppPath() + _T("Skin");
		zipFile.find(strSkinPath, "*.zip");
		int nCount = zipFile.count();


		FindFile::typeT files = zipFile.co_file();
		for(FindFile::typeT::iterator it = files.begin(); files.end() != it; ++it)
		{
			// 检测合法性
			HZIP hZip = OpenZip((void *)it->c_str(), 0, ZIP_FILENAME, i8desk::GetAppPath().c_str());
			if( hZip != 0 )
			{
				int nIndex = 0;
				ZIPENTRY ze = {0};
				FindZipItem(hZip, _T("Skin/Dock/logo.png"), FALSE, &nIndex, &ze);
				CloseZip(hZip);
				if( ze.index == -1 )
					continue;
			}
			else
				continue;

			i8desk::std_string strFileName = it->substr(it->find_last_of('\\') + 1);
			i8desk::std_string strFile = strFileName.substr(0, strFileName.find_last_of('.'));


			int nItem = 0;
			std::transform(strFile.begin(), strFile.end(), strFile.begin(), ::toupper);

			if( strFile == _T("SKIN") )
			{
				nItem = pSkinCtrl->AddString(_T("默认"));
				pSkinCtrl->SetCurSel(nItem);
			}
			else
				nItem = pSkinCtrl->AddString(strFile.c_str());

			if( strDefaultSkin == strFile )
				pSkinCtrl->SetCurSel(nItem);
		}

		//pSkinCtrl->SetCurSel(0);
	}
	else 
	{
		for (i8desk::BarSkinListItr it = m_BarSkins.begin();
			it != m_BarSkins.end(); ++it) 
		{
			pSkinCtrl->AddString(it->Syle);
		}

		std::string Value = pDbMgr->GetOptString(OPT_M_CLISKIN);
		CString strSub;
		AfxExtractSubString(strSub, Value.c_str(), 0, '|');
		pSkinCtrl->SelectString(-1, strSub);
	}

	((CComboBox*)GetDlgItem(IDC_ACTRUNMENU))->AddString("无操作");
	((CComboBox*)GetDlgItem(IDC_ACTRUNMENU))->AddString("最小化");
	((CComboBox*)GetDlgItem(IDC_ACTRUNMENU))->AddString("关闭");
	((CComboBox*)GetDlgItem(IDC_ACTRUNMENU))->SetCurSel(pDbMgr->GetOptInt(OPT_M_MENUACT, 0));

	((CButton*)GetDlgItem(IDC_RUNMENU)) ->SetCheck(pDbMgr->GetOptInt(OPT_M_RUNMENU, 0));
	((CButton*)GetDlgItem(IDC_RUNSHELL))->SetCheck(pDbMgr->GetOptInt(OPT_M_RUNSHELL, 0));
	((CButton*)GetDlgItem(IDC_ONLYREAD))->SetCheck(pDbMgr->GetOptInt(OPT_M_ONLYONE, 0));
	((CButton*)GetDlgItem(IDC_CLIAUTOUPT))->SetCheck(pDbMgr->GetOptInt(OPT_M_AUTOUPT, 1));
	((CButton*)GetDlgItem(IDC_USE_UDISK))->SetCheck(pDbMgr->GetOptInt(OPT_M_USEUDISK, 0));

	SetDlgItemText(IDC_VDISKTEMP,	 pDbMgr->GetOptString(OPT_M_VDDIR).c_str());
	SetDlgItemText(IDC_IEURL,		 pDbMgr->GetOptString(OPT_M_IEURL).c_str());

	m_bModify = FALSE;

	return TRUE;
}

void CMenuOptDlg::OnBnClickedOk()
{
}

void CMenuOptDlg::OnBnClickedCancel()
{
}

LRESULT CMenuOptDlg::OnApplyMessage(WPARAM wParam, LPARAM lParam)
{
	CString strVDiskDir;
	GetDlgItem(IDC_VDISKTEMP)->GetWindowText(strVDiskDir);
	if (!i8desk::IsValidDirName((LPCSTR)strVDiskDir))
	{
		AfxMessageBox("虚拟盘临时路径目录非法.");
		return FALSE;
	}
	i8desk::CDbMgr* pDbMgr = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd())->m_pDbMgr;
	if (m_bModify)
	{
		CString strIEURL, strUserDataDir;
		GetDlgItem(IDC_IEURL)->GetWindowText(strIEURL);

		pDbMgr->SetOption(OPT_M_WINMODE, ((CComboBox*)GetDlgItem(IDC_MENU_MODE))->GetCurSel());
		pDbMgr->SetOption(OPT_M_BROWSEDIR, ((CButton*)GetDlgItem(IDC_BROWSEDIR))->GetCheck());

		int nHotCount = GetDlgItemInt(IDC_EDIT_HOTCOUNT);
		if (nHotCount > 0 && nHotCount < 20)
			nHotCount = 20;
		pDbMgr->SetOption(OPT_M_HOTCOUNT, nHotCount);
		pDbMgr->SetOption(OPT_M_MENUACT,  ((CComboBox*)GetDlgItem(IDC_ACTRUNMENU))->GetCurSel());

		pDbMgr->SetOption(OPT_M_RUNMENU,  ((CButton*)GetDlgItem(IDC_RUNMENU))->GetCheck());
		pDbMgr->SetOption(OPT_M_RUNSHELL, ((CButton*)GetDlgItem(IDC_RUNSHELL))->GetCheck());
		pDbMgr->SetOption(OPT_M_ONLYONE,  ((CButton*)GetDlgItem(IDC_ONLYREAD))->GetCheck());
		pDbMgr->SetOption(OPT_M_AUTOUPT,  ((CButton*)GetDlgItem(IDC_CLIAUTOUPT))->GetCheck());

		pDbMgr->SetOption(OPT_M_VDDIR, (LPCSTR)strVDiskDir);
		pDbMgr->SetOption(OPT_M_IEURL, (LPCSTR)strIEURL);
		pDbMgr->SetOption(OPT_M_USERDATADIR, (LPCSTR)strUserDataDir);
		pDbMgr->SetOption(OPT_M_USEUDISK, ((CButton*)GetDlgItem(IDC_USE_UDISK))->GetCheck());

		//菜单皮肤
		CString strStyle;
		GetDlgItem(IDC_SKIN)->GetWindowText(strStyle);
		/*for (i8desk::BarSkinListItr it = m_BarSkins.begin();
			it != m_BarSkins.end(); ++it) 
		{
			if (strStyle == it->Syle) {
				std::string ErrInfo;
				pDbMgr->SetBarSkin(*it, ErrInfo);
				break;
			}
		}*/

		if( strStyle == _T("默认") )
			strStyle = _T("skin");

		std::stringstream ss;
		ss << (LPCTSTR)strStyle
			<< '|' << strStyle + _T(".zip")
			<< std::ends;
		GetConsoleDlg()->m_pDbMgr->SetOption(OPT_M_CLISKIN, ss.str());


		m_bModify = FALSE;
	}
	if (strVDiskDir.IsEmpty())
	{
		strVDiskDir = "C:\\Temp";
		pDbMgr->SetOption(OPT_M_VDDIR, (LPCSTR)strVDiskDir);
	}
	if (m_btnSetMenuPwd.GetCheck())
	{
		char xbuf[MAX_PATH] = {0};
		GetDlgItemText(IDC_CLIPASSWORD, xbuf, MAX_PATH);
		if (lstrlen(xbuf) == 0)
			lstrcpy(xbuf, "1234567");
		pDbMgr->SetOption(OPT_M_CLIPWD,   i8desk::GetBufCrc32((BYTE*)xbuf, lstrlen(xbuf)));
	}

	return TRUE;
}

void CMenuOptDlg::OnBnClickedWinmode()
{
	m_bModify = TRUE;
	BOOL bCheck = ((CButton*)GetDlgItem(IDC_HOTCOUNT))->GetCheck();
	GetDlgItem(IDC_EDIT_HOTCOUNT)->EnableWindow(bCheck);
	if (!bCheck)
		SetDlgItemInt(IDC_EDIT_HOTCOUNT, 0);
}

void CMenuOptDlg::OnBnClickedCheckSetpwd()
{
	GetDlgItem(IDC_CLIPASSWORD)->EnableWindow(m_btnSetMenuPwd.GetCheck());
}

void CMenuOptDlg::OnCbnSelchangeMenuMode()
{
	m_bModify = TRUE;
}

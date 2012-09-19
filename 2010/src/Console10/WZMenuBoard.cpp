// WZMenuBoard.cpp : 实现文件
//

#include "stdafx.h"
#include "WZMenuBoard.h"
#include "FindFile.h"
#include "xunzip.h"

// CWZMenuBoard 对话框

IMPLEMENT_DYNAMIC(CWZMenuBoard, CDialog)

CWZMenuBoard::CWZMenuBoard(CWnd* pParent /*=NULL*/)
	: CDialog(CWZMenuBoard::IDD, pParent)
	, m_strSkin(_T(""))
	, m_nMenuMode(-1)
	, m_nActRunMenu(-1)
	, m_nNotice(-1)
	, m_strNotice(_T(""))
	, m_nAutoRun(-1)
	, m_nBrowserDir(-1)
{

}

CWZMenuBoard::~CWZMenuBoard()
{
}

void CWZMenuBoard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CWZMenuBoard::OnInitDialog()
{
	CDialog::OnInitDialog();

	((CComboBox*)GetDlgItem(IDC_MENU_MODE))->AddString("全屏");
	((CComboBox*)GetDlgItem(IDC_MENU_MODE))->AddString("窗口");
	((CComboBox*)GetDlgItem(IDC_MENU_MODE))->SetCurSel(GetDbMgr()->GetOptInt(OPT_M_WINMODE, 0));

	std::string ErrInfo;
	if (!GetDbMgr()->GetAllBarSkin(m_BarSkins, ErrInfo)) {
		::AfxMessageBox(_T("取菜单皮肤列表失败!"));
	}

	i8desk::CDbMgr* pDbMgr = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd())->m_pDbMgr;
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
	((CComboBox*)GetDlgItem(IDC_ACTRUNMENU))->SetCurSel(GetDbMgr()->GetOptInt(OPT_M_MENUACT, 0));

	((CButton*)GetDlgItem(IDC_DISPLAY_NOTICE))->SetCheck(GetDbMgr()->GetOptInt(OPT_M_SHOWGONGGAO, 0));
	SetDlgItemText(IDC_NOTICE, GetDbMgr()->GetOptString(OPT_M_GGMSG, "").c_str());

	((CButton*)GetDlgItem(IDC_RUNMENU))->SetCheck(GetDbMgr()->GetOptInt(OPT_M_RUNMENU, 0));
	((CButton*)GetDlgItem(IDC_BROWSEDIR))->SetCheck(GetDbMgr()->GetOptInt(OPT_M_BROWSEDIR, 1));

	return TRUE;
}


BEGIN_MESSAGE_MAP(CWZMenuBoard, CDialog)
	ON_REGISTERED_MESSAGE(i8desk::g_nOptApplyMsg, &CWZMenuBoard::OnApplyMessage)
	ON_CBN_SELCHANGE(IDC_SKIN, &CWZMenuBoard::OnCbnSelchangeSkin)
	ON_BN_CLICKED(IDC_PREVIEW, &CWZMenuBoard::OnBnClickedPreview)
	ON_CBN_SELCHANGE(IDC_MENU_MODE, &CWZMenuBoard::OnCbnSelchangeMenuMode)
	ON_CBN_SELCHANGE(IDC_ACTRUNMENU, &CWZMenuBoard::OnCbnSelchangeActrunmenu)
	ON_BN_CLICKED(IDC_DISPLAY_NOTICE, &CWZMenuBoard::OnBnClickedDisplayNotice)
	ON_EN_CHANGE(IDC_NOTICE, &CWZMenuBoard::OnEnChangeNotice)
	ON_BN_CLICKED(IDC_RUNMENU, &CWZMenuBoard::OnBnClickedRunmenu)
	ON_BN_CLICKED(IDC_BROWSEDIR, &CWZMenuBoard::OnBnClickedBrowsedir)
END_MESSAGE_MAP()


// CWZMenuBoard 消息处理程序
LRESULT CWZMenuBoard::OnApplyMessage(WPARAM wParam, LPARAM lParam)
{
	i8desk::CDbMgr* pDbMgr = GetDbMgr();
	
	if (this->m_nMenuMode != -1)
	{
		pDbMgr->SetOption(OPT_M_WINMODE, m_nMenuMode);
		m_nMenuMode = -1;
	}

	if (this->m_nAutoRun != -1)
	{
		pDbMgr->SetOption(OPT_M_RUNMENU, m_nAutoRun);
		m_nAutoRun = -1;
	}

	if (this->m_nActRunMenu != -1)
	{
		pDbMgr->SetOption(OPT_M_MENUACT, m_nActRunMenu);
		m_nActRunMenu = -1;
	}

	if (this->m_nBrowserDir != -1)
	{
		pDbMgr->SetOption(OPT_M_BROWSEDIR, m_nBrowserDir);
		m_nBrowserDir = -1;
	}

	if (!m_strSkin.IsEmpty())
	{
		for (i8desk::BarSkinListItr it = m_BarSkins.begin();
			it != m_BarSkins.end(); ++it) 
		{
			if (m_strSkin == it->Syle) {
				std::string ErrInfo;
				pDbMgr->SetBarSkin(*it, ErrInfo);
				break;
			}
		}
		m_strSkin = _T("");
	}

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
		strStyle = _T("Skin");

	std::stringstream ss;
	ss << (LPCTSTR)strStyle
		<< '|' << strStyle + _T(".zip")
		<< std::ends;
	GetConsoleDlg()->m_pDbMgr->SetOption(OPT_M_CLISKIN, ss.str());


	if (this->m_nNotice != -1)
	{
		pDbMgr->SetOption(OPT_M_SHOWGONGGAO, m_nNotice);
		m_nNotice = -1;
	}

	if (!this->m_strNotice.IsEmpty())
	{
		pDbMgr->SetOption(OPT_M_GGMSG, (LPCSTR)m_strNotice);
		m_strNotice = _T("");
	}

	return TRUE;
}

void CWZMenuBoard::OnCbnSelchangeSkin()
{
	GetDlgItem(IDC_SKIN)->GetWindowText(m_strSkin);
}


void CWZMenuBoard::OnBnClickedPreview()
{

}

void CWZMenuBoard::OnCbnSelchangeMenuMode()
{
	m_nMenuMode = ((CComboBox*)GetDlgItem(IDC_MENU_MODE))->GetCurSel();
}

void CWZMenuBoard::OnCbnSelchangeActrunmenu()
{
	m_nActRunMenu = ((CComboBox*)GetDlgItem(IDC_ACTRUNMENU))->GetCurSel();
}

void CWZMenuBoard::OnBnClickedDisplayNotice()
{
	m_nNotice = ((CButton*)GetDlgItem(IDC_DISPLAY_NOTICE))->GetCheck();
}

void CWZMenuBoard::OnEnChangeNotice()
{
	GetDlgItem(IDC_NOTICE)->GetWindowText(m_strNotice);
}

void CWZMenuBoard::OnBnClickedRunmenu()
{
	m_nAutoRun = ((CButton*)GetDlgItem(IDC_RUNMENU))->GetCheck();
}

void CWZMenuBoard::OnBnClickedBrowsedir()
{
	m_nBrowserDir = ((CButton*)GetDlgItem(IDC_BROWSEDIR))->GetCheck();
}


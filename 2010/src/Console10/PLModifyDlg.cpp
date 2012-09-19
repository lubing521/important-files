// PLModifyDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "PLModifyDlg.h"
#include "ConsoleDlg.h"
#include <algorithm>
// CPLModifyDlg 对话框

IMPLEMENT_DYNAMIC(CPLModifyDlg, CDialog)

CPLModifyDlg::CPLModifyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPLModifyDlg::IDD, pParent)
{

}

CPLModifyDlg::~CPLModifyDlg()
{
	m_GameInfos.clear();
}

void CPLModifyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RUNTYPE, m_lstRunType);
	DDX_Control(pDX, IDC_COMBO_CLASS, m_cboClass);
	DDX_Control(pDX, IDC_COMBO_AUTOUPT, m_cboAutoUpt);
	DDX_Control(pDX, IDC_COMBO_PRIORITY, m_cboPriority);
	DDX_Control(pDX, IDC_COMBO_DOWN_PRIORITY, m_cb3upPriority);
	DDX_Control(pDX, IDC_CHECK_TOOLBAR, m_btnToolbar);
	DDX_Control(pDX, IDC_DESKLNK, m_btnDeskLnk);
	DDX_Control(pDX, IDC_CHECK_SVRPATH,		m_btnSvrPath);
	DDX_Control(pDX, IDC_CHECK_CLIPATH,		m_btnCliPath);
	DDX_Control(pDX, IDC_CHECK_CLASS,		m_btnClass);
	DDX_Control(pDX, IDC_CHECK_3UP,			m_btn3up);
	DDX_Control(pDX, IDC_CHECK_PRIORITY,	m_btnPriority);
	DDX_Control(pDX, IDC_CHECK_DOWN_PRIORITY,	m_btnDownPriority);
	DDX_Control(pDX, IDC_CHECK_MODRUNTYPE,	m_btnEnModRunType);
	DDX_Control(pDX, IDC_CHECK_OTHER,		m_btnEnModOther);
}


BEGIN_MESSAGE_MAP(CPLModifyDlg, CDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &CPLModifyDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_SVRPATH, &CPLModifyDlg::OnBnClickedCheckCtrl)
	ON_BN_CLICKED(IDC_CHECK_CLIPATH, &CPLModifyDlg::OnBnClickedCheckCtrl)
	ON_BN_CLICKED(IDC_CHECK_CLASS, &CPLModifyDlg::OnBnClickedCheckCtrl)
	ON_BN_CLICKED(IDC_CHECK_3UP, &CPLModifyDlg::OnBnClickedCheckCtrl)
	ON_BN_CLICKED(IDC_CHECK_PRIORITY, &CPLModifyDlg::OnBnClickedCheckCtrl)
	ON_BN_CLICKED(IDC_CHECK_DOWN_PRIORITY, &CPLModifyDlg::OnBnClickedCheckCtrl)
	ON_BN_CLICKED(IDC_CHECK_MODRUNTYPE, &CPLModifyDlg::OnBnClickedCheckCtrl)
	ON_BN_CLICKED(IDC_CHECK_OTHER, &CPLModifyDlg::OnBnClickedCheckCtrl)
END_MESSAGE_MAP()

BOOL CPLModifyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	i8desk::CDbMgr* pDbMgr = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd())->m_pDbMgr;	
	std::string ErrInfo;

	if (!pDbMgr->GetAllClass(m_ClassInfos, ErrInfo) ||
		!pDbMgr->GetAllArea(m_AreaInfos, ErrInfo) ||
		!pDbMgr->GetAllVDisk(m_VDiskInfos, ErrInfo))
	{
		AfxMessageBox(ErrInfo.c_str());
		OnCancel();
		return TRUE;
	}

	((CEdit*)GetDlgItem(IDC_FROM))->LimitText(100);
	((CEdit*)GetDlgItem(IDC_TO))->LimitText(100);
	((CEdit*)GetDlgItem(IDC_CLIPATH))->LimitText(100);

	m_cboClass.AddString(CLASS_WL_NAME);
	m_cboClass.AddString(CLASS_DJ_NAME);
	m_cboClass.AddString(CLASS_XX_NAME);
	m_cboClass.AddString(CLASS_WY_NAME);
	m_cboClass.AddString(CLASS_DZ_NAME);
	m_cboClass.AddString(CLASS_QP_NAME);
	m_cboClass.AddString(CLASS_PL_NAME);
	m_cboClass.AddString(CLASS_LT_NAME);
	m_cboClass.AddString(CLASS_CY_NAME);
	m_cboClass.AddString(CLASS_YY_NAME);
	m_cboClass.AddString(CLASS_GP_NAME);
	for (i8desk::DefClassMapItr it = 
		m_ClassInfos.begin(); it != m_ClassInfos.end(); it++)
	{
		if (!i8desk::IsI8DeskClassGUID(it->first))
			m_cboClass.AddString(it->second->Name);
	}
	m_cboClass.SetCurSel(0);

	m_cboAutoUpt.AddString("自动更新");
	m_cboAutoUpt.AddString("手动更新");
	m_cboAutoUpt.SetCurSel(1);

	m_cboPriority.AddString("低:必删");
	m_cboPriority.AddString("中:可删");
	m_cboPriority.AddString("较高:暂不删");
	m_cboPriority.AddString("高:不删");
	m_cboPriority.SetCurSel(1);

	m_cb3upPriority.AddString(_T("最低"));
	m_cb3upPriority.AddString(_T("较低"));
	m_cb3upPriority.AddString(_T("一般"));
	m_cb3upPriority.AddString(_T("较高"));
	m_cb3upPriority.AddString(_T("最高"));
	m_cb3upPriority.SetCurSel(0);

	m_lstRunType.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_lstRunType.InsertColumn(0, "分区", LVCFMT_LEFT, 80);

	//添加组合框的所有运行方式
	CGridColumnTraitCombo* pComboTrait = new CGridColumnTraitCombo;
	pComboTrait->SetStyle( pComboTrait->GetStyle() | CBS_DROPDOWNLIST);
	pComboTrait->AddItem(0, RT_UNKNOWN);
	pComboTrait->AddItem(1, RT_LOCAL);
	int loop = 2;
	for (i8desk::VDiskInfoMapItr it = m_VDiskInfos.begin();
		it != m_VDiskInfos.end(); it ++)
	{
		CString info;
		if (it->second->Type == VDISK_I8DESK)
		{
			info.Format("%s%c (%s:%C)", RT_VDISK, (char)it->second->CliDrv,
				i8desk::MakeIpString(it->second->IP).c_str(), (char)it->second->SvrDrv);
		}
		else
		{
			info.Format("%s%C", RT_VDISK, (char)it->second->CliDrv);
		}
		pComboTrait->AddItem(loop, info);
		loop ++;
	}
	pComboTrait->AddItem(loop, RT_DIRECT);

	m_lstRunType.InsertColumnTrait(1, "运行方式", LVCFMT_LEFT, 200, 1, pComboTrait);

	//添加游戏在每个分区的运行方式
	m_lstRunType.InsertItem(m_lstRunType.GetItemCount(), DEFAULT_AREA_NAME);
	m_lstRunType.SetItemText(0, 1, RT_UNKNOWN);

	i8desk::AreaInfoMapItr it = m_AreaInfos.begin();
	for (; it != m_AreaInfos.end(); it ++)
	{
		if (it->first != DEFAULT_AREA_GUID)
		{
			int nIdx = m_lstRunType.InsertItem(
				m_lstRunType.GetItemCount(), it->second->Name);
			m_lstRunType.SetItemText(nIdx, 1, RT_UNKNOWN);
		}
	}

	OnBnClickedCheckCtrl();

	return TRUE;
}
void CPLModifyDlg::OnDestroy()
{
	m_ClassInfos.clear();
	m_AreaInfos.clear();
	m_VDiskInfos.clear();

	CDialog::OnDestroy();
}

std::string CPLModifyDlg::GetClassGuid()
{
	char buf[MAX_PATH] = {0};
	int nSel = m_cboClass.GetCurSel();
	m_cboClass.GetLBText(nSel, buf);
	for (i8desk::DefClassMapItr it = m_ClassInfos.begin();
		it != m_ClassInfos.end(); it++)
	{
		if (strcmp(it->second->Name, buf) == 0)
		{
			return it->first;
		}
	}
	return CLASS_WL_GUID;
}

std::string CPLModifyDlg::GetAIDByName(std::string& Name)
{
	i8desk::AreaInfoMapItr it = m_AreaInfos.begin();
	for (; it != m_AreaInfos.end(); it ++)
	{
		if (Name == it->second->Name)
		{
			return it->first;
		}
	}
	return "";
}

BOOL CPLModifyDlg::ParaseAreaRTTexT(CString szText, i8desk::tagGameRunType& RtType)
{
	int nLen = lstrlen(RT_VDISK);
	RtType.VID[0] = 0;

	RtType.RunType = 0;
	if (szText == RT_UNKNOWN)
	{
		RtType.RunType = 0;
		return TRUE;
	}
	else if (szText == RT_LOCAL)
	{
		RtType.RunType = 1;
		return TRUE;
	}
	else if (szText == RT_DIRECT)
	{
		RtType.RunType = 3;
		return TRUE;
	}
	else if (szText.Left(nLen) == RT_VDISK && szText.GetLength() > nLen)
	{
		RtType.RunType = 2;
		char CliDrv = szText.GetAt(nLen);
		i8desk::VDiskInfoMapItr it = m_VDiskInfos.begin();
		for (; it != m_VDiskInfos.end(); it++)
		{
			if (it->second->CliDrv == CliDrv)
			{
				strcpy(RtType.VID, it->second->VID);
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CPLModifyDlg::OnBnClickedOk()
{
	CWaitCursor wc;
	char buf[MAX_PATH] = {0};
	std::string src, dst, clipath;
	GetDlgItemText(IDC_FROM, buf, MAX_PATH);
	src = buf;
	i8desk::TrimString(src);
	GetDlgItemText(IDC_TO, buf, MAX_PATH);
	dst = buf;
	i8desk::TrimString(dst);
	GetDlgItemText(IDC_CLIPATH, buf, MAX_PATH);
	clipath = buf;
	i8desk::TrimString(clipath);
	
	if (clipath.size() && *clipath.rbegin() != '\\')
	{
		clipath += "\\";
	}

	std::string ErrInfo;
	i8desk::CDbMgr* pDbMgr = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd())->m_pDbMgr;	
	
	i8desk::AreaInfoMap AreaInfos;
	if (!pDbMgr->GetAllArea(AreaInfos, ErrInfo)) {
		AfxMessageBox(ErrInfo.c_str());
		return;
	}

	i8desk::GameRunTypeList lstRunType;
	if (m_btnEnModRunType.GetCheck())
	{
		for (int nIdx=0;nIdx<m_lstRunType.GetItemCount(); nIdx++)
		{
			i8desk::tagGameRunType RtType;
			RtType.AreaName = m_lstRunType.GetItemText(nIdx, 0);
			strcpy(RtType.AID, GetAIDByName(RtType.AreaName).c_str());
			if (ParaseAreaRTTexT(m_lstRunType.GetItemText(nIdx, 1), RtType))
				lstRunType.push_back(RtType);
		}
	}

	std::string guid = GetClassGuid();

	for (i8desk::GameInfoVectorItr it=m_GameInfos.begin(); 
		it != m_GameInfos.end(); it++)
	{
		i8desk::uint64 mask = 0;
		i8desk::GameInfo GameInfo = **it;
		if (m_btnSvrPath.GetCheck() && !src.empty())
		{
			int pos = 0;
			std::string path = GameInfo.SvrPath;
			std::transform(path.begin(), path.end(), path.begin(), tolower);
			std::transform(src.begin(), src.end(), src.begin(), tolower);
			if ( (pos = path.find(src, pos)) != std::string::npos)
			{
				path.replace(pos, src.size(), dst);
				pos += dst.size();
				i8desk::SAFE_STRCPY(GameInfo.SvrPath, path.c_str());
				mask |= MASK_TGAME_SVRPATH;
			}
		}

		if (m_btnCliPath.GetCheck() && !clipath.empty())
		{
			i8desk::SAFE_STRCPY(GameInfo.CliPath, (clipath + GameInfo.Name + "\\").c_str());
			mask |= MASK_TGAME_CLIPATH;
		}
		
		if (m_btnClass.GetCheck() && GameInfo.GID < MIN_IDC_GID) {
			i8desk::SAFE_STRCPY(GameInfo.DefClass, guid.c_str());
			mask |= MASK_TGAME_DEFCLASS;
		}

		if (m_btn3up.GetCheck() && GameInfo.GID >= MIN_IDC_GID) {
			GameInfo.AutoUpt = m_cboAutoUpt.GetCurSel() == 0;
			mask |= MASK_TGAME_AUTOUPT;
		}

		if (m_btnPriority.GetCheck()) {
			GameInfo.Priority = m_cboPriority.GetCurSel() + 1;
			mask |= MASK_TGAME_PRIORITY;
		}
		
		if (m_btnDownPriority.GetCheck()) {
			GameInfo.DownPriority = m_cb3upPriority.GetCurSel() + 1;
			mask |= MASK_TGAME_DOWNPRIORITY;
		}
		
		if (m_btnEnModOther.GetCheck())
		{
			GameInfo.Toolbar = m_btnToolbar.GetCheck();
			GameInfo.DeskLnk = m_btnDeskLnk.GetCheck();
			mask |= MASK_TGAME_TOOLBAR | MASK_TGAME_DESKLNK;
		}

		if (!pDbMgr->ModifyGame(GameInfo, ErrInfo, mask))
		{
			AfxMessageBox(ErrInfo.c_str());
			return ;
		}		

		//修改成功，将新值写回
		**it = GameInfo;

		if (m_btnEnModRunType.GetCheck())
		{
			for (i8desk::GameRunTypeListItr Iter = lstRunType.begin();
				Iter != lstRunType.end(); Iter++)
			{
				Iter->GID = GameInfo.GID;
			}

			if (!pDbMgr->SetGameRunType(GameInfo.GID, lstRunType, ErrInfo)
				|| !pDbMgr->GetGameRunType((*it)->GID, (*it)->RunTypes, ErrInfo, AreaInfos))
			{
				AfxMessageBox(ErrInfo.c_str());
				return ;
			}
		}
	}

	OnOK();
}

void CPLModifyDlg::OnBnClickedCheckCtrl()
{
	m_btnDeskLnk.EnableWindow( m_btnEnModOther.GetCheck() );
	m_btnToolbar.EnableWindow( m_btnEnModOther.GetCheck() );
	m_lstRunType.EnableWindow( m_btnEnModRunType.GetCheck() );

	this->GetDlgItem(IDC_FROM)->EnableWindow( m_btnSvrPath.GetCheck() );
	this->GetDlgItem(IDC_TO)->EnableWindow( m_btnSvrPath.GetCheck() );

	this->GetDlgItem(IDC_CLIPATH)->EnableWindow( m_btnCliPath.GetCheck() );

	m_cboClass.EnableWindow( m_btnClass.GetCheck() );
	m_cboAutoUpt.EnableWindow( m_btn3up.GetCheck() );
	m_cboPriority.EnableWindow(	m_btnPriority.GetCheck() );
	m_cb3upPriority.EnableWindow(	m_btnDownPriority.GetCheck() );

	this->GetDlgItem(IDOK)->EnableWindow(
		m_btnEnModOther.GetCheck() ||
		m_btnEnModRunType.GetCheck() ||
		m_btnSvrPath.GetCheck() ||
		m_btnCliPath.GetCheck() ||
		m_btnClass.GetCheck() ||
		m_btn3up.GetCheck() ||
		m_btnPriority.GetCheck() ||
		m_btnDownPriority.GetCheck()
		);
}

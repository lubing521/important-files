// MakeIdxDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "MakeIdxDlg.h"
#include "ConsoleDlg.h"

IMPLEMENT_DYNAMIC(CMakeIdxDlg, CDialog)

CMakeIdxDlg::CMakeIdxDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMakeIdxDlg::IDD, pParent)
	, m_bOnlyMakeIdx (true)
	, m_bIsBackgroudWork(false)
{
}

CMakeIdxDlg::~CMakeIdxDlg()
{
}

void CMakeIdxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_lstCtrl);
	DDX_Control(pDX, IDC_PROGRESS1, m_btnProgress);
	DDX_Control(pDX, IDC_EDIT_LOG, m_edtLog);
}

BEGIN_MESSAGE_MAP(CMakeIdxDlg, CDialog)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_DESTROY()	
	ON_REGISTERED_MESSAGE(i8desk::g_nIdxMessage, OnMessage)
END_MESSAGE_MAP()

BOOL CMakeIdxDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_bFinish = false;
	
	m_lstCtrl.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_lstCtrl.InsertColumn(0, "GID",	LVCFMT_LEFT, 60);
	m_lstCtrl.InsertColumn(1, "游戏名", LVCFMT_LEFT, 100);
	m_lstCtrl.InsertColumn(2, "路径",	LVCFMT_LEFT, 160);
	m_lstCtrl.InsertColumn(3, "大小(M)",LVCFMT_LEFT, 60);
	m_lstCtrl.InsertColumn(4, "状态",	LVCFMT_LEFT, 100);
	m_btnProgress.SetRange(0, 1000);

	for (size_t nIdx=0; nIdx<m_GameInfos.size(); nIdx++)
	{
		CString msg;
		msg.Format("%05d", m_GameInfos[nIdx]->GID);
		int nItem = m_lstCtrl.InsertItem(m_lstCtrl.GetItemCount(), msg);
		m_lstCtrl.SetItemText(nItem, 1, m_GameInfos[nIdx]->Name);
		m_lstCtrl.SetItemText(nItem, 2, m_GameInfos[nIdx]->SvrPath);
	}
	CString msg;
	m_nIndex = 0;
	msg.Format("%05d,%s", m_GameInfos[m_nIndex]->GID, m_GameInfos[m_nIndex]->Name);
	SetDlgItemText(IDC_TIP, msg);
	MakeIdx.StartMakeIdx(CA2W(m_GameInfos[m_nIndex]->SvrPath), m_bOnlyMakeIdx, m_hWnd);

	SetWindowText(m_bOnlyMakeIdx ? _T("制作游戏索引") : _T("校验游戏索引"));
	
	return TRUE;
}

void CMakeIdxDlg::OnClose()
{
	if (!m_bFinish && MessageBox("正在制作索引,的确要退出吗?", "提示", MB_OKCANCEL|MB_ICONINFORMATION) != IDOK)
		return ;
	
	CDialog::OnClose();
}

void CMakeIdxDlg::OnDestroy()
{
	CWaitCursor wc;
	MakeIdx.StopMakeIdx();
	wc.Restore();
	
	CDialog::OnDestroy();
}

void CMakeIdxDlg::OnTimer(UINT_PTR nIDEvent)
{
	m_btnProgress.SetPos(MakeIdx.GetProgress());
}

LRESULT CMakeIdxDlg::OnMessage(WPARAM wParam, LPARAM lParam)
{
	static DWORD dwSize = 0;
	CString msg;
	if (wParam == IDX_STATUS_ERROR)
	{
		m_lstCtrl.SetItemText(m_nIndex, 4, "发生错误");
		CString str, append;
		m_edtLog.GetWindowText(str);
		append.Format("%05d:%s\r\n%s", m_GameInfos[m_nIndex]->GID, 
			m_GameInfos[m_nIndex]->Name,
			MakeIdx.GetErrorString().c_str());
		str += append + "\r\n\r\n";
		m_edtLog.SetWindowText(str);

		m_nIndex++;
		if (m_nIndex < (int)m_GameInfos.size())	//make next game idx.
		{
			msg.Format("%05d:%s", m_GameInfos[m_nIndex]->GID, m_GameInfos[m_nIndex]->Name);
			SetDlgItemText(IDC_TIP, msg);
			MakeIdx.StartMakeIdx(CA2W(m_GameInfos[m_nIndex]->SvrPath), m_bOnlyMakeIdx, m_hWnd);
		}
		else
		{
			m_bFinish = true;
		}
	}
	else if (wParam == IDX_STATUS_SCAN)
	{
		m_lstCtrl.SetItemText(m_nIndex, 4, "正在扫描目录");
	}
	else if (wParam == IDX_STATUS_GETSIZE)
	{
		msg.Format("%0.f", lParam*1.0/1024);
		m_lstCtrl.SetItemText(m_nIndex, 3, msg);
		dwSize = lParam;
		m_lstCtrl.SetItemText(m_nIndex, 4, "正在生成索引");
	}	
	else if (wParam == IDX_STATUS_CRC)
	{
		m_btnProgress.SetPos(lParam);
	}
	else if (wParam == IDX_STATUS_CRC_START)
	{
		m_btnProgress.SetPos(lParam);
		this->SetTimer(1, 1000, 0);
	}
	else if (wParam == IDX_STATUS_CRC_END)
	{
		m_btnProgress.SetPos(lParam);
		KillTimer(1);
	}
	else if (wParam == IDX_STATUS_CHECK)
	{
		m_lstCtrl.SetItemText(m_nIndex, 4, "正在检验索引");
	}
	else if (wParam == IDX_STATUS_FINISH)
	{
		m_btnProgress.SetPos(0);
		MakeIdx.StopMakeIdx();
		m_lstCtrl.SetItemText(m_nIndex, 4, m_bOnlyMakeIdx ? "制作索引完成" : "检验索引完成");
		if (!m_bOnlyMakeIdx)
		{
			CString str, append;
			m_edtLog.GetWindowText(str);
			append.Format("%05d:%s\r\n", m_GameInfos[m_nIndex]->GID, m_GameInfos[m_nIndex]->Name);
			str += append;
			append = MakeIdx.GetCheckLog().c_str();
			if (append.GetLength() == 0)
				append = "索引与数据相符.";
			str += append;
			str += "\r\n\r\n";
			m_edtLog.SetWindowText(str);
		}
		else
		{
			i8desk::GameInfo_Ptr pGame = m_GameInfos[m_nIndex];
			pGame->Size = dwSize;
			pGame->SvrVer = _time32(NULL);
			CConsoleDlg* pFrame = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());
			std::string ErrInfo;
			pFrame->m_pDbMgr->UpdateGameSizeAndVer(pGame->GID, pGame->Size, pGame->SvrVer, ErrInfo);
		}
		m_nIndex++;
		if (m_nIndex < (int)m_GameInfos.size())	//make next game idx.
		{
			msg.Format("%05d:%s", m_GameInfos[m_nIndex]->GID, m_GameInfos[m_nIndex]->Name);
			SetDlgItemText(IDC_TIP, msg);
			MakeIdx.StartMakeIdx(CA2W(m_GameInfos[m_nIndex]->SvrPath), m_bOnlyMakeIdx, m_hWnd);
		}
		else
		{
			m_bFinish = true;
		}

		//后台工作时成功完成后自动关闭窗口
		if (m_bIsBackgroudWork) 
		{
			PostMessage(WM_CLOSE);
		}
	}
	return 0;
}
// MakeIdxDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "DirMakeIdxDlg.h"
#include "ConsoleDlg.h"

IMPLEMENT_DYNAMIC(CDirMakeIdxDlg, CDialog)

CDirMakeIdxDlg::CDirMakeIdxDlg(LPCTSTR dir, CWnd* pParent /*=NULL*/)
	: CDialog(CDirMakeIdxDlg::IDD, pParent)
	, m_strDir(dir)
	, m_nIndex(0)
{
}

CDirMakeIdxDlg::~CDirMakeIdxDlg()
{
}

void CDirMakeIdxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_lstCtrl);
	DDX_Control(pDX, IDC_PROGRESS1, m_btnProgress);
	DDX_Control(pDX, IDC_EDIT_LOG, m_edtLog);
}

BEGIN_MESSAGE_MAP(CDirMakeIdxDlg, CDialog)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_DESTROY()	
	ON_REGISTERED_MESSAGE(i8desk::g_nIdxMessage, OnMessage)
END_MESSAGE_MAP()

BOOL CDirMakeIdxDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_bFinish = false;
	
	m_lstCtrl.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_lstCtrl.InsertColumn(0, "GID",	LVCFMT_LEFT, 0);
	m_lstCtrl.InsertColumn(1, "游戏名", LVCFMT_LEFT, 0);
	m_lstCtrl.InsertColumn(2, "路径",	LVCFMT_LEFT, 160);
	m_lstCtrl.InsertColumn(3, "大小(M)",LVCFMT_LEFT, 60);
	m_lstCtrl.InsertColumn(4, "状态",	LVCFMT_LEFT, 100);
	m_btnProgress.SetRange(0, 1000);

	m_lstCtrl.InsertItem(0, "");
	m_lstCtrl.SetItemText(0, 2, m_strDir);

	CString msg;
	SetDlgItemText(IDC_TIP, m_strDir);
	MakeIdx.StartMakeIdx(CT2W(m_strDir), true, m_hWnd);

	SetWindowText(_T("制作索引"));
	
	return TRUE;
}

void CDirMakeIdxDlg::OnClose()
{
	if (!m_bFinish && MessageBox("正在制作索引,的确要退出吗?", "提示", MB_OKCANCEL|MB_ICONINFORMATION) != IDOK)
		return ;
	
	CDialog::OnClose();
}

void CDirMakeIdxDlg::OnDestroy()
{
	CWaitCursor wc;
	MakeIdx.StopMakeIdx();
	wc.Restore();
	
	CDialog::OnDestroy();
}

void CDirMakeIdxDlg::OnTimer(UINT_PTR nIDEvent)
{
	m_btnProgress.SetPos(MakeIdx.GetProgress());
}

LRESULT CDirMakeIdxDlg::OnMessage(WPARAM wParam, LPARAM lParam)
{
	static DWORD dwSize = 0;
	CString msg;
	if (wParam == IDX_STATUS_ERROR)
	{
		m_lstCtrl.SetItemText(m_nIndex, 4, "发生错误");
		CString str, append;
		m_edtLog.GetWindowText(str);
		append.Format("%s", MakeIdx.GetErrorString().c_str());
		str += append + "\r\n\r\n";
		m_edtLog.SetWindowText(str);

		m_bFinish = true;
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
		m_lstCtrl.SetItemText(m_nIndex, 4, "制作索引完成");
		m_bFinish = true;
		PostMessage(WM_CLOSE);
	}
	return 0;
}
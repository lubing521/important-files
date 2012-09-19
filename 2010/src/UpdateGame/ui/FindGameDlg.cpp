#include "stdafx.h"
#include "ServerUI.h"
#include "FindGameDlg.h"
#include "MainFrm.h"
#include "SyncGameInfoView.h"
#include "MyChiToLetter.h"

static void GetPY(std::string& sChinese, std::string& result)
{
	result = GetPyLetter(sChinese);
	return;

	for (size_t idx=0; idx<sChinese.size(); idx++)
	{
		if (IsDBCSLeadByte(sChinese[idx]))
		{
			wchar_t wchr = 0;
			wchr = (sChinese[idx] & 0xff) << 8;
			wchr |= (sChinese[++idx] & 0xff);			
			result += convert(wchr);
		}
		else
		{
			result += tolower(sChinese[idx]);
		}
	}
}

IMPLEMENT_DYNAMIC(CFindGameDlg, CDialog)

CFindGameDlg::CFindGameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindGameDlg::IDD, pParent)
	, m_bRaido1(FALSE)
	, m_bRaido2(FALSE)
{
		m_nItem = 0;
		m_InputChar ='\0';
}

CFindGameDlg::~CFindGameDlg()
{
}

void CFindGameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO1, m_Radio);
	DDX_Control(pDX, IDC_RADIO2, m_Radio2);
	DDX_Radio(pDX, IDC_RADIO1, m_bRaido1);
	DDX_Radio(pDX, IDC_RADIO2, m_bRaido2);
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
}


BEGIN_MESSAGE_MAP(CFindGameDlg, CDialog)
	ON_BN_CLICKED(BTN_FINDGAME, &CFindGameDlg::OnBnClickedFindgame)
	ON_BN_CLICKED(BTN_EXIT, &CFindGameDlg::OnBnClickedExit)
//	ON_BN_CLICKED(BTN_SELECTALL, &CFindGameDlg::OnBnClickedSelectall)
	ON_BN_CLICKED(IDC_RADIO1, &CFindGameDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CFindGameDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDOK, &CFindGameDlg::OnBnClickedOk)
	ON_EN_SETFOCUS(IDC_EDIT1, &CFindGameDlg::OnEnSetfocusEdit1)
END_MESSAGE_MAP()

void CFindGameDlg::OnBnClickedFindgame()
{
	CString  str;
	GetDlgItemText(IDC_EDIT1,str);

	FindGame(str,++m_nItem);
}

void CFindGameDlg::OnBnClickedExit()
{

	OnCancel();
}

void CFindGameDlg::OnBnClickedSelectall()
{
	
}

bool CFindGameDlg::FindGame(CString& str,int nItem)
{
	if(str.IsEmpty())
		return false;
	CSyncGameInfoView* pSyncView = NULL;
	std::map<std::string,CWnd*>::iterator it  = ((CMainFrame*)AfxGetMainWnd())->m_mapView.find(STR_SYNCGAMEINFO);
	if(it !=  ((CMainFrame*)AfxGetMainWnd())->m_mapView.end())
	{
		pSyncView = (CSyncGameInfoView*)it->second;
	}
	if(pSyncView)
	{
			DWORD ncount = pSyncView->GetListCtrl().GetItemCount();
			for(DWORD i = nItem; i<ncount;i++)
			{
				CString strname = pSyncView->GetListCtrl().GetItemText(i,1);
				std::string GameName( strname);
				transform(GameName.begin(), GameName.end(), GameName.begin(), tolower);
				std::string result;		
				GetPY(GameName, result);
				if (GameName.find(str) !=  std::string::npos|| result.find(str)   != std::string::npos)
				{
					m_nItem = i;
					pSyncView->GetListCtrl().SetFocus();
					//pSyncView->GetListCtrl().SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED|LVIF_STATE , LVIS_SELECTED|LVIS_FOCUSED|LVIF_STATE );
 					pSyncView->GetListCtrl().SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED , LVIS_SELECTED|LVIS_FOCUSED );
					pSyncView->GetListCtrl().EnsureVisible(i, TRUE);
					return true;
				}
			}
	}
	return false;
}
void CFindGameDlg::OnBnClickedRadio1()
{
	m_Radio2.SetCheck(0);
	m_Radio.SetCheck(1);
}

void CFindGameDlg::OnBnClickedRadio2()
{
	m_Radio2.SetCheck(1);
	m_Radio.SetCheck(0);
}

BOOL CFindGameDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if(m_InputChar !='\0')
	{
		m_Edit.SetWindowText(CString(m_InputChar));
	}

	OnBnClickedRadio1();
	return TRUE;
}

void CFindGameDlg::OnBnClickedOk()
{
	CString  str;
	GetDlgItemText(IDC_EDIT1,str);

	FindGame(str,++m_nItem);
}

void CFindGameDlg::OnEnSetfocusEdit1()
{
	CString str;
	m_Edit.GetWindowText(str);
	m_Edit.SetSel(str.GetLength(),str.GetLength()+1);
}

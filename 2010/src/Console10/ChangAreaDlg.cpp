// ChangAreaDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "ChangAreaDlg.h"
#include "ConsoleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif // _DEBUG

IMPLEMENT_DYNAMIC(CChangAreaDlg, CDialog)

CChangAreaDlg::CChangAreaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChangAreaDlg::IDD, pParent)
	, m_bMulti(FALSE)
{

}

CChangAreaDlg::~CChangAreaDlg()
{
}

void CChangAreaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_AREA, m_cboArea);
	DDX_Control(pDX, IDC_IPFROM, m_ipFrom);
	DDX_Control(pDX, IDC_NETGATE, m_ipNetGate);
	DDX_Control(pDX, IDC_MARK, m_ipMark);
	DDX_Control(pDX, IDC_DNS, m_ipDNS);
	DDX_Control(pDX, IDC_DNS2, m_ipDNS2);
	DDX_Control(pDX, IDC_COMBO_AREA, m_cboArea);
}


BEGIN_MESSAGE_MAP(CChangAreaDlg, CDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &CChangAreaDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CChangAreaDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

BOOL CChangAreaDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	i8desk::CDbMgr* pDbMgr = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd())->m_pDbMgr;
	i8desk::AreaInfoMap AreaInfos;
	std::string ErrInfo;
	if (!pDbMgr->GetAllArea(AreaInfos, ErrInfo))
	{
		AfxMessageBox(ErrInfo.c_str());
		OnCancel();
		return TRUE;
	}
	i8desk::AreaInfoMapItr it = AreaInfos.begin();
	m_cboArea.AddString(DEFAULT_AREA_NAME);
	char* pGuid = new char[lstrlen(DEFAULT_AREA_GUID) + 1];
	lstrcpy(pGuid, DEFAULT_AREA_GUID);
	m_cboArea.SetItemData(0, reinterpret_cast<DWORD>(pGuid));

	for (; it != AreaInfos.end(); it++)
	{
		if (it->first != DEFAULT_AREA_GUID)
		{
			int nItem = m_cboArea.AddString(it->second->Name);
			pGuid = new char[it->first.size()+1];
			lstrcpy(pGuid, it->first.c_str());
			m_cboArea.SetItemData(nItem, reinterpret_cast<DWORD>(pGuid));
			if (it->first == m_Machine.AID)
			{
				m_cboArea.SetCurSel(nItem);
			}
		}
	}
	AreaInfos.clear();

	if (m_bMulti) {
		m_ipFrom.EnableWindow(FALSE);
		m_ipNetGate.EnableWindow(FALSE);
		m_ipMark.EnableWindow(FALSE);
		m_ipDNS.EnableWindow(FALSE);
		m_ipDNS2.EnableWindow(FALSE);
	} else {
		m_ipFrom.SetAddress(ntohl(m_Machine.IP));
		m_ipNetGate.SetAddress(ntohl(inet_addr(m_Machine.NetGate)));
		m_ipMark.SetAddress(ntohl(inet_addr(m_Machine.Mark)));
		m_ipDNS.SetAddress(ntohl(inet_addr(m_Machine.DNS)));
		m_ipDNS2.SetAddress(ntohl(inet_addr(m_Machine.DNS2)));
	}

	if (m_cboArea.GetCurSel() == -1)
		m_cboArea.SetCurSel(0);
	
	return TRUE;
}

void CChangAreaDlg::OnDestroy()
{
	for (int idx=0; idx<m_cboArea.GetCount(); idx++)
	{
		char* pGuid = reinterpret_cast<char*>(m_cboArea.GetItemData(idx));
		delete []pGuid;
	}
	CDialog::OnDestroy();
}

void CChangAreaDlg::OnBnClickedOk()
{
	int nSel = m_cboArea.GetCurSel();
	if (nSel == -1)
		return ;

	if (!m_bMulti) {
		DWORD nValue;
		m_ipFrom.GetAddress(nValue);
		m_Machine.IP = htonl(nValue);

		m_ipNetGate.GetAddress(nValue);
		i8desk::SAFE_STRCPY(m_Machine.NetGate, i8desk::MakeIpString(htonl(nValue)).c_str());

		m_ipMark.GetAddress(nValue);
		i8desk::SAFE_STRCPY(m_Machine.Mark, i8desk::MakeIpString(htonl(nValue)).c_str());

		m_ipDNS.GetAddress(nValue);
		i8desk::SAFE_STRCPY(m_Machine.DNS, i8desk::MakeIpString(htonl(nValue)).c_str());

		m_ipDNS2.GetAddress(nValue);
		i8desk::SAFE_STRCPY(m_Machine.DNS2, i8desk::MakeIpString(htonl(nValue)).c_str());
	}

	i8desk::SAFE_STRCPY(m_Machine.AID, reinterpret_cast<const char*>(m_cboArea.GetItemData(nSel)));
	OnOK();
}

void CChangAreaDlg::OnBnClickedCancel()
{
	OnCancel();
}

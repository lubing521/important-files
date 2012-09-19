// SCArpProtectDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SCArpProtectDlg.h"

#include <regex>
#include <sstream>

// CSCArpProtectDlg 对话框

IMPLEMENT_DYNAMIC(CSCArpProtectDlg, CDialog)

CSCArpProtectDlg::CSCArpProtectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSCArpProtectDlg::IDD, pParent)
	, m_strGatewayMac(_T(""))
	, m_enable(FALSE)
	, m_bAutoGetAddr(FALSE)
{

}

CSCArpProtectDlg::~CSCArpProtectDlg()
{
}

void CSCArpProtectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MACADDRESS, m_strGatewayMac);
	DDX_Control(pDX, IDC_IPADDRESS, m_IPCtrl);
	DDX_Check(pDX, IDC_ENABLE, m_enable);
	DDX_Check(pDX, IDC_AUTO_GET_GATEWAY, m_bAutoGetAddr);
}


BEGIN_MESSAGE_MAP(CSCArpProtectDlg, CDialog)
	ON_REGISTERED_MESSAGE(i8desk::g_nOptApplyMsg, &CSCArpProtectDlg::OnApplyMessage)
	ON_BN_CLICKED(IDC_ENABLE, &CSCArpProtectDlg::OnBnClickedEnable)
	ON_BN_CLICKED(IDC_AUTO_GET_GATEWAY, &CSCArpProtectDlg::OnBnClickedAutoGetGateway)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS, &CSCArpProtectDlg::OnIpnFieldchangedIpaddress)
	ON_EN_CHANGE(IDC_MACADDRESS, &CSCArpProtectDlg::OnEnChangeMacaddress)
END_MESSAGE_MAP()

void CSCArpProtectDlg::SetConfig(BOOL bIsOpen,
								 BOOL bIsAutoGetGatewayAddr,
								 LPCTSTR lpIP, 
								 LPCTSTR lpMAC)
{
	m_enable = bIsOpen;
	m_bAutoGetAddr = bIsAutoGetGatewayAddr;
	m_IPCtrl.SetWindowText(lpIP);
	m_strGatewayMac = lpMAC;

	UpdateData(FALSE);

	OnBnClickedEnable();
	OnBnClickedAutoGetGateway();
}

// CSCArpProtectDlg 消息处理程序


void CSCArpProtectDlg::OnBnClickedEnable()
{
	UpdateData();

	GetDlgItem(IDC_AUTO_GET_GATEWAY)->EnableWindow(m_enable);
	GetDlgItem(IDC_IPADDRESS)->EnableWindow(m_enable);
	GetDlgItem(IDC_MACADDRESS)->EnableWindow(m_enable);
}

void CSCArpProtectDlg::OnBnClickedAutoGetGateway()
{
	UpdateData();
	GetDlgItem(IDC_IPADDRESS)->EnableWindow(!m_bAutoGetAddr);
	GetDlgItem(IDC_MACADDRESS)->EnableWindow(!m_bAutoGetAddr);
}

void CSCArpProtectDlg::OnIpnFieldchangedIpaddress(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

void CSCArpProtectDlg::OnEnChangeMacaddress()
{
}

LRESULT CSCArpProtectDlg::OnApplyMessage(WPARAM wParam, LPARAM lParam)
{
	UpdateData();

	CString IP;
	m_IPCtrl.GetWindowText(IP);

	if (m_enable && !m_bAutoGetAddr)
	{
		if (IP.IsEmpty() || IP == "0.0.0.0")
		{
			AfxMessageBox(_T("网关IP地址无效"));
			return FALSE;
		}

		std::tr1::regex regex("^([0-9a-fA-F]{2})((-([0-9a-fA-F]{2})){5})$");
		if (!std::tr1::regex_match((LPCTSTR)m_strGatewayMac, regex))
		{
			AfxMessageBox(_T("网关MAC地址格式不正确\n应形如XX-XX-XX-XX-XX-XX\n其中X只能为数字或a~f或A~F"));
			return FALSE;
		}
	}

	MSXML2::IXMLDOMDocument *pXmlDoc = reinterpret_cast<MSXML2::IXMLDOMDocument *>(wParam);
	ASSERT(pXmlDoc);

	MSXML2::IXMLDOMElement *pRootElem = reinterpret_cast<MSXML2::IXMLDOMElement *>(lParam);
	ASSERT(pRootElem);

	MSXML2::IXMLDOMElementPtr ArpProtected = pXmlDoc->createElement(_T("ArpProtected"));
	pRootElem->appendChild(ArpProtected);

	MSXML2::IXMLDOMElementPtr Status = pXmlDoc->createElement(_T("Status"));
	Status->setAttribute(_T("IsOpen"), m_enable ? 1L : 0L);
	Status->setAttribute(_T("IsAutoGetGatewayAddr"), m_bAutoGetAddr ? 1L : 0L);
	Status->setAttribute(_T("GatewayIp"), (LPCTSTR)IP);
	Status->setAttribute(_T("GatewayMac"), (LPCTSTR)m_strGatewayMac);
	ArpProtected->appendChild(Status);

	return TRUE;
}
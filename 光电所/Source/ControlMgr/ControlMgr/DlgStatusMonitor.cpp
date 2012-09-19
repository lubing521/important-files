// DlgStatusMonitor.cpp : 实现文件
//

#include "stdafx.h"
#include "ControlMgr.h"
#include "DlgStatusMonitor.h"

#include <sstream>

#include "ControlManager.hpp"
#include "../../../include/UI/ImageHelpers.h"

// CDlgStatusMonitor 对话框

IMPLEMENT_DYNAMIC(CDlgStatusMonitor, CDialog)

CDlgStatusMonitor::CDlgStatusMonitor(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgStatusMonitor::IDD, pParent)
{

}

CDlgStatusMonitor::~CDlgStatusMonitor()
{
}

void CDlgStatusMonitor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_UDPPROXY_IP, wndLabelUDPProxyIP_);
	DDX_Text(pDX, IDC_STATIC_UDPPROXY_PORT, wndLabelUDPProxyPort_);
	DDX_Text(pDX, IDC_STATIC_UDPPROXY_STATUS, wndLabelUDPProxyStatus_);
	DDX_Text(pDX, IDC_STATIC_SENDSVR_IP, wndLabelSendSvrIP_);
	DDX_Text(pDX, IDC_STATIC_SENDSVR_PORT, wndLabelSendSvrPort_);
	DDX_Text(pDX, IDC_STATIC_SENDSVR_STATUS, wndLabelSendSvrStatus_);
	DDX_Text(pDX, IDC_STATIC_RECVSVR_IP, wndLabelRecvSvrIP_);
	DDX_Text(pDX, IDC_STATIC_RECVSVR_PORT, wndLabelRecvSvrPort_);
	DDX_Text(pDX, IDC_STATIC_RECVSVR_STATUS, wndLabelRecvSvrStatus_);
	DDX_Control(pDX, IDC_LIST_SENDSVR_CLIENT, wndListClients_);
	DDX_Control(pDX, IDC_BUTTON_UDPPROXY_START, wndBtnProxyStart_);
	DDX_Control(pDX, IDC_BUTTON_UDPPROXY_STOP, wndBtnProxySuspend_);
	DDX_Control(pDX, IDC_BUTTON_SENDSVR_START, wndBtnSendSvrStart_);
	DDX_Control(pDX, IDC_BUTTON_SENDSVR_STOP, wndBtnSendSvrSuspend_);
	DDX_Control(pDX, IDC_BUTTON_RECVSVR_START, wndBtnRecvSvrStart_);
	DDX_Control(pDX, IDC_BUTTON_RECVSVR_STOP, wndBtnRecvSvrSuspend_);
}

enum { WM_REFRESH = WM_USER  + 0x100 };

BEGIN_MESSAGE_MAP(CDlgStatusMonitor, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_UDPPROXY_START, &CDlgStatusMonitor::OnBnClickedButtonUdpproxyStart)
	ON_BN_CLICKED(IDC_BUTTON_UDPPROXY_STOP, &CDlgStatusMonitor::OnBnClickedButtonUdpproxyStop)
	ON_BN_CLICKED(IDC_BUTTON_SENDSVR_START, &CDlgStatusMonitor::OnBnClickedButtonSendsvrStart)
	ON_BN_CLICKED(IDC_BUTTON_SENDSVR_STOP, &CDlgStatusMonitor::OnBnClickedButtonSendsvrStop)
	ON_BN_CLICKED(IDC_BUTTON_RECVSVR_START, &CDlgStatusMonitor::OnBnClickedButtonRecvsvrStart)
	ON_BN_CLICKED(IDC_BUTTON_RECVSVR_STOP, &CDlgStatusMonitor::OnBnClickedButtonRecvsvrStop)

	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_REFRESH, &CDlgStatusMonitor::OnRefresh)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
END_MESSAGE_MAP()


BOOL CDlgStatusMonitor::OnInitDialog()
{
	CDialog::OnInitDialog();

	wndListClients_.InsertColumn(0, _T(""), LVCFMT_LEFT, 20);
	wndListClients_.InsertColumn(1, _T("IP"), LVCFMT_LEFT, 80);

	return TRUE;  // return TRUE unless you set the focus to a control
}



// CDlgStatusMonitor 消息处理程序

void CDlgStatusMonitor::OnBnClickedButtonUdpproxyStart()
{
	ctrl::ControlMgrInstance()->ChangeUDPProxyState(false);
	PostMessage(WM_REFRESH);

}

void CDlgStatusMonitor::OnBnClickedButtonUdpproxyStop()
{
	ctrl::ControlMgrInstance()->ChangeUDPProxyState(true);
	PostMessage(WM_REFRESH);
}

void CDlgStatusMonitor::OnBnClickedButtonSendsvrStart()
{
	ctrl::ControlMgrInstance()->ChangeSendSvrState(false);
	PostMessage(WM_REFRESH);
}

void CDlgStatusMonitor::OnBnClickedButtonSendsvrStop()
{
	ctrl::ControlMgrInstance()->ChangeSendSvrState(true);
	PostMessage(WM_REFRESH);
}

void CDlgStatusMonitor::OnBnClickedButtonRecvsvrStart()
{
	ctrl::ControlMgrInstance()->ChangeRecvSvrState(false);
	PostMessage(WM_REFRESH);
}

void CDlgStatusMonitor::OnBnClickedButtonRecvsvrStop()
{
	ctrl::ControlMgrInstance()->ChangeRecvSvrState(true);
	PostMessage(WM_REFRESH);
}

LRESULT CDlgStatusMonitor::OnRefresh(WPARAM, LPARAM)
{
	ctrl::StatusInfo proxyStatus = ctrl::ControlMgrInstance()->GetUDPProxyStatus();
	ctrl::StatusInfo sendSvrStatus = ctrl::ControlMgrInstance()->GetSendSvrStatus();
	ctrl::StatusInfo recvSvrStatus = ctrl::ControlMgrInstance()->GetRecvSvrStatus();

	std::wostringstream os;

	wndLabelUDPProxyIP_ = std::tr1::get<ctrl::STATUS_IP>(proxyStatus).c_str();
	os << std::tr1::get<ctrl::STATUS_PORT>(proxyStatus);
	wndLabelUDPProxyPort_ = os.str().c_str();
	if( std::tr1::get<ctrl::STATUS_PAUSED>(proxyStatus) )
	{
		wndLabelUDPProxyStatus_ = _T("暂停");
		wndBtnProxyStart_.EnableWindow(TRUE);
		wndBtnProxySuspend_.EnableWindow(FALSE);
	}
	else
	{
		if( std::tr1::get<ctrl::STATUS_OK>(proxyStatus) )
		{
			wndLabelUDPProxyStatus_ = _T("运行");
			wndBtnProxyStart_.EnableWindow(FALSE);
			wndBtnProxySuspend_.EnableWindow(TRUE);
		}
		else
		{
			wndLabelUDPProxyStatus_ = _T("无数据");
			wndBtnProxyStart_.EnableWindow(FALSE);
			wndBtnProxySuspend_.EnableWindow(FALSE);
		}
	}

	os.str(L"");
	wndLabelSendSvrIP_ = std::tr1::get<ctrl::STATUS_IP>(sendSvrStatus).c_str();
	os << std::tr1::get<ctrl::STATUS_PORT>(sendSvrStatus);
	wndLabelSendSvrPort_ = os.str().c_str();

	if( std::tr1::get<ctrl::STATUS_PAUSED>(sendSvrStatus) )
	{
		wndLabelSendSvrStatus_ = _T("暂停");
		wndBtnSendSvrStart_.EnableWindow(TRUE);
		wndBtnSendSvrSuspend_.EnableWindow(FALSE);
	}
	else
	{
		if( std::tr1::get<ctrl::STATUS_OK>(sendSvrStatus) )
		{
			wndLabelSendSvrStatus_ = _T("运行");
			wndBtnSendSvrStart_.EnableWindow(FALSE);
			wndBtnSendSvrSuspend_.EnableWindow(TRUE);
		}
		else
		{
			wndLabelSendSvrStatus_ = _T("无数据");
			wndBtnSendSvrStart_.EnableWindow(FALSE);
			wndBtnSendSvrSuspend_.EnableWindow(FALSE);
		}
	}


	os.str(L"");
	wndLabelRecvSvrIP_ = std::tr1::get<ctrl::STATUS_IP>(recvSvrStatus).c_str();
	os << std::tr1::get<ctrl::STATUS_PORT>(recvSvrStatus);
	wndLabelRecvSvrPort_ = os.str().c_str();

	if( std::tr1::get<ctrl::STATUS_PAUSED>(recvSvrStatus) )
	{
		wndLabelRecvSvrStatus_ = _T("暂停");
		wndBtnRecvSvrStart_.EnableWindow(TRUE);
		wndBtnRecvSvrSuspend_.EnableWindow(FALSE);
	}
	else
	{
		if( std::tr1::get<ctrl::STATUS_OK>(recvSvrStatus) )
		{
			wndLabelRecvSvrStatus_ = _T("运行");
			wndBtnRecvSvrStart_.EnableWindow(FALSE);
			wndBtnRecvSvrSuspend_.EnableWindow(TRUE);
		}
		else
		{
			wndLabelRecvSvrStatus_ = _T("无数据");
			wndBtnRecvSvrStart_.EnableWindow(FALSE);
			wndBtnRecvSvrSuspend_.EnableWindow(FALSE);
		}
	}


	common::RemoteClients remoteClients = ctrl::ControlMgrInstance()->GetRecvSvrClients();
	wndListClients_.DeleteAllItems();

	int index = 0;
	for(common::RemoteClients::const_iterator iter = remoteClients.begin();
		iter != remoteClients.end(); ++iter)
	{
		std::wstringstream os;
		os << index;
		index = wndListClients_.InsertItem(index, os.str().c_str());

		wndListClients_.SetItemText(index++, 1, (*iter).c_str());
	}

	UpdateData(FALSE);

	Invalidate();
	return TRUE;
}

void CDlgStatusMonitor::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CMemDC memDC(dc, this);

	
	HBRUSH tmp = ui::draw::AtlGetBackgroundBrush(GetSafeHwnd(), GetParent()->GetSafeHwnd());
	CBrush brush;
	brush.Attach(tmp);
	CRect rcClient;
	GetClientRect(rcClient);
	memDC.GetDC().FillRect(rcClient, &brush);
}

BOOL CDlgStatusMonitor::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

HBRUSH CDlgStatusMonitor::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor) 
{ 
	HBRUSH   hbr = CDialog::OnCtlColor(pDC,pWnd,nCtlColor); 
	if   (nCtlColor==CTLCOLOR_STATIC ) 
	{ 
		pDC-> SetBkMode(TRANSPARENT); 
		return   (HBRUSH)::GetStockObject(NULL_BRUSH); 
	} 
	return   hbr; 
} 
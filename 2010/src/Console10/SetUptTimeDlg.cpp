// SetUptTimeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "SetUptTimeDlg.h"

IMPLEMENT_DYNAMIC(CSetUptTimeDlg, CDialog)

CSetUptTimeDlg::CSetUptTimeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetUptTimeDlg::IDD, pParent)
{

}

CSetUptTimeDlg::~CSetUptTimeDlg()
{
}

void CSetUptTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_dtStart);
	DDX_Control(pDX, IDC_DATETIMEPICKER2, m_dtEnd);
}

BEGIN_MESSAGE_MAP(CSetUptTimeDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSetUptTimeDlg::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CSetUptTimeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_dtStart.SetTime(&CTime(2009, 1, 1, 21, 0, 0));
	m_dtEnd.SetTime(&CTime(2009, 1, 1, 23, 59, 0));
	return TRUE;
}
void CSetUptTimeDlg::OnBnClickedOk()
{
	m_dtStart.GetTime(tmStart);
	m_dtEnd.GetTime(tmEnd);

	tmStart = CTime(2009, 1, 1, tmStart.GetHour(), tmStart.GetMinute(), 0);
	tmEnd   = CTime(2009, 1, 1, tmEnd.GetHour(),   tmEnd.GetMinute(),   0);
	if (tmStart >= tmEnd)
	{
		AfxMessageBox("请保证结束时间大于开始时间.");
		return ;
	}

	OnOK();
}

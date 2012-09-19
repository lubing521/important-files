// GongGaoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "GongGaoDlg.h"
#include "ConsoleDlg.h"

// CGongGaoDlg 对话框

IMPLEMENT_DYNAMIC(CGongGaoDlg, CDialog)

CGongGaoDlg::CGongGaoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGongGaoDlg::IDD, pParent)
{
	ZeroMemory(&m_lf, sizeof(m_lf));
}

CGongGaoDlg::~CGongGaoDlg()
{
}

void CGongGaoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_SHOW, m_btnShowGongGao);
}


BEGIN_MESSAGE_MAP(CGongGaoDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CGongGaoDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CGongGaoDlg::OnBnClickedCancel)
	ON_REGISTERED_MESSAGE(i8desk::g_nOptApplyMsg, &CGongGaoDlg::OnApplyMessage)
	ON_BN_CLICKED(IDC_FONT1, &CGongGaoDlg::OnBnClickedFont1)
	ON_EN_CHANGE(IDC_EDIT1, &CGongGaoDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_CHECK_SHOW, &CGongGaoDlg::OnEnChangeEdit1)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL CGongGaoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	i8desk::CDbMgr* pDbMgr = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd())->m_pDbMgr;

	SetDlgItemText(IDC_EDIT1, pDbMgr->GetOptString(OPT_M_GGMSG, "").c_str());
	m_btnShowGongGao.SetCheck(pDbMgr->GetOptInt(OPT_M_SHOWGONGGAO, 0));

	m_szFontString = pDbMgr->GetOptString(OPT_M_GGFONT, "宋体|12|0|0").c_str();
	//prase gonggao font string. "字体|大小|颜色|标志"
	{		
		CString szTemp;
		AfxExtractSubString(szTemp, m_szFontString, 0, '|');
		if (szTemp.GetLength() == 0) szTemp = "宋体";
		strcpy(m_lf.lfFaceName, szTemp);

		AfxExtractSubString(szTemp, m_szFontString, 1, '|');
		m_lf.lfHeight = -abs(atoi(szTemp));
		if (m_lf.lfHeight == 0)	m_lf.lfHeight = -12;		

		AfxExtractSubString(szTemp, m_szFontString, 2, '|');
		m_fontColor = atoi(szTemp);
		//if (m_fontColor == 0) m_fontColor = RGB(0, 0, 0);

		AfxExtractSubString(szTemp, m_szFontString, 3, '|');
		DWORD flag = atoi(szTemp);
		m_lf.lfWeight	 = (flag & (1<<0)) ? FW_BOLD : FW_NORMAL;
		m_lf.lfItalic	 = (flag & (1<<1)) ? TRUE : FALSE;
		m_lf.lfUnderline = (flag & (1<<2)) ? TRUE : FALSE;
		m_lf.lfStrikeOut = (flag & (1<<3)) ? TRUE : FALSE;	
		m_font.CreateFontIndirect(&m_lf);
		GetDlgItem(IDC_EDIT1)->SetFont(&m_font);
	}

	m_bModify = FALSE;
	return TRUE;
}

void CGongGaoDlg::OnBnClickedOk()
{
	
}

void CGongGaoDlg::OnBnClickedCancel()
{

}

LRESULT CGongGaoDlg::OnApplyMessage(WPARAM wParam, LPARAM lParam)
{
	if (m_bModify)
	{
		CString strGongGao, strScrInfo;
		GetDlgItemText(IDC_EDIT1, strGongGao);
		GetDlgItemText(IDC_EDIT2, strScrInfo);

		i8desk::CDbMgr* pDbMgr = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd())->m_pDbMgr;
		pDbMgr->SetOption(OPT_M_GGMSG, (LPCSTR)strGongGao);
		pDbMgr->SetOption(OPT_M_SHOWGONGGAO, m_btnShowGongGao.GetCheck() ? 1 : 0);
		pDbMgr->SetOption(OPT_M_GGFONT, (LPCSTR)m_szFontString);
		m_bModify = FALSE;
	}
	return TRUE;
}

void CGongGaoDlg::OnBnClickedFont1()
{
	CFontDialog dlg(&m_lf);
	dlg.m_cf.rgbColors = m_fontColor;
	if (dlg.DoModal() == IDOK)
	{
		dlg.GetCurrentFont(&m_lf);
		DWORD flag = 0;

		if (dlg.IsBold())		flag |= (1<<0);
		if (dlg.IsItalic())		flag |= (1<<1);
		if (dlg.IsUnderline())	flag |= (1<<2);
		if (dlg.IsStrikeOut())	flag |= (1<<3);

		m_szFontString.Format("%s|%d|%d|%d", 
			m_lf.lfFaceName, abs(m_lf.lfHeight), dlg.GetColor(), flag);
		m_font.DeleteObject();
		m_font.CreateFontIndirect(&m_lf);
		m_fontColor = dlg.GetColor();
		GetDlgItem(IDC_EDIT1)->SetFont(&m_font);
		m_bModify = TRUE;
	}	
}

void CGongGaoDlg::OnEnChangeEdit1()
{
	m_bModify = TRUE;
}

HBRUSH CGongGaoDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if (nCtlColor == CTLCOLOR_EDIT && pWnd->GetDlgCtrlID() == IDC_EDIT1)
	{
		pDC->SetTextColor(m_fontColor);
	}

	return hbr;
}
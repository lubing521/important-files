// WidgetBlacklist.cpp : 实现文件
//

#include "stdafx.h"
#include "WidgetBlacklist.h"

#include <regex>

// CWidgetBlacklist 对话框

IMPLEMENT_DYNAMIC(CWidgetBlacklist, CDialog)

CWidgetBlacklist::CWidgetBlacklist(CWnd* pParent /*=NULL*/)
	: CDialog(CWidgetBlacklist::IDD, pParent)
	, m_strWidget(_T(""))
	, m_strDescription(_T(""))
{

}

CWidgetBlacklist::~CWidgetBlacklist()
{
}

void CWidgetBlacklist::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_WIDGET, m_strWidget);
	DDX_Text(pDX, IDC_DESCRIPTION, m_strDescription);
}


BEGIN_MESSAGE_MAP(CWidgetBlacklist, CDialog)
	ON_BN_CLICKED(IDOK, &CWidgetBlacklist::OnBnClickedOk)
	ON_EN_CHANGE(IDC_WIDGET, &CWidgetBlacklist::OnEnChangeWidget)
END_MESSAGE_MAP()


// CWidgetBlacklist 消息处理程序

void CWidgetBlacklist::OnBnClickedOk()
{
	UpdateData();

	if (m_strWidget.IsEmpty()) 
	{
		AfxMessageBox(_T("必须输入有效的窗口标题"));
		return;
	}
	else 
	{
		std::tr1::regex regex("^\\**$");
		if (std::tr1::regex_match((LPCTSTR)m_strWidget, regex))
		{
			AfxMessageBox(_T("窗口标题特征串不能全为“*”字符"));
			return;
		}
	}

	OnOK();
}

void CWidgetBlacklist::OnEnChangeWidget()
{
	CString old = m_strWidget;
	UpdateData();
	{
		std::tr1::regex regex("^.*\\*{2}.*$");
		if (std::tr1::regex_match((LPCTSTR)m_strWidget, regex))
		{
			AfxMessageBox(_T("不要连续输入“*”字符"));
			m_strWidget = old;
			UpdateData(FALSE);
			return;
		}
	}
	{
		std::tr1::regex regex("^.+\\*.+");
		if (std::tr1::regex_match((LPCTSTR)m_strWidget, regex))
		{
			AfxMessageBox(_T("“*”字符不能置于特征串的中间"));
			m_strWidget = old;
			UpdateData(FALSE);
			return;
		}
	}
}

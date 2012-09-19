// SCWidgetBlacklistDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SCWidgetBlacklistDlg.h"
#include "WidgetBlacklist.h"

// CSCWidgetBlacklistDlg 对话框

IMPLEMENT_DYNAMIC(CSCWidgetBlacklistDlg, CDialog)

CSCWidgetBlacklistDlg::CSCWidgetBlacklistDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSCWidgetBlacklistDlg::IDD, pParent)
{

}

CSCWidgetBlacklistDlg::~CSCWidgetBlacklistDlg()
{
}

void CSCWidgetBlacklistDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_list);
}


BEGIN_MESSAGE_MAP(CSCWidgetBlacklistDlg, CDialog)
	ON_REGISTERED_MESSAGE(i8desk::g_nOptApplyMsg, &CSCWidgetBlacklistDlg::OnApplyMessage)
	ON_BN_CLICKED(IDC_ADD, &CSCWidgetBlacklistDlg::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_REMOVE, &CSCWidgetBlacklistDlg::OnBnClickedRemove)
	ON_NOTIFY(NM_CLICK, IDC_LIST, &CSCWidgetBlacklistDlg::OnNMClickList)
	ON_NOTIFY(LVN_DELETEITEM, IDC_LIST, &CSCWidgetBlacklistDlg::OnLvnDeleteitemList)
END_MESSAGE_MAP()


// CSCWidgetBlacklistDlg 消息处理程序

BOOL CSCWidgetBlacklistDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_list.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_list.InsertColumn(0, _T("添加时间"), LVCFMT_LEFT, 120);
	m_list.InsertColumn(1, _T("窗口"), LVCFMT_LEFT, 120);
	m_list.InsertColumn(2, _T("说明"), LVCFMT_LEFT, 200);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSCWidgetBlacklistDlg::InsertItem(LPCTSTR lpAddTime, LPCTSTR lpName, LPCTSTR lpDesc)
{
	int nItem = m_list.GetItemCount();
	m_list.InsertItem(nItem, lpAddTime);
	m_list.SetItemText(nItem, 1, lpName);
	m_list.SetItemText(nItem, 2, lpDesc);
}

void CSCWidgetBlacklistDlg::OnBnClickedAdd()
{
	CWidgetBlacklist dlg;
	if (dlg.DoModal() == IDOK) 
	{
		int nItem = m_list.GetItemCount();
		CTime tAdd = CTime::GetCurrentTime();
		m_list.InsertItem(nItem, tAdd.Format(_T("%Y/%m/%d %H:%M:%S")));
		m_list.SetItemText(nItem, 1, dlg.m_strWidget);
		m_list.SetItemText(nItem, 2, dlg.m_strDescription);
	}
}

void CSCWidgetBlacklistDlg::OnBnClickedRemove()
{
	int nSel = -1;
	while ((nSel = m_list.GetNextItem(nSel, LVIS_SELECTED)) != -1 )
	{
		m_list.DeleteItem(nSel);
		nSel = -1;
	}
}

void CSCWidgetBlacklistDlg::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	POSITION pos = m_list.GetFirstSelectedItemPosition();
	bool b = pos != NULL;

	this->GetDlgItem(IDC_REMOVE)->EnableWindow(b);

	*pResult = 0;
}

LRESULT CSCWidgetBlacklistDlg::OnApplyMessage(WPARAM wParam, LPARAM lParam)
{
	MSXML2::IXMLDOMDocument *pXmlDoc = reinterpret_cast<MSXML2::IXMLDOMDocument *>(wParam);
	ASSERT(pXmlDoc);

	MSXML2::IXMLDOMElement *pRootElem = reinterpret_cast<MSXML2::IXMLDOMElement *>(lParam);
	ASSERT(pRootElem);

	MSXML2::IXMLDOMElementPtr WindowList = pXmlDoc->createElement(_T("WindowList"));
	pRootElem->appendChild(WindowList);

	for (int i = 0; i < m_list.GetItemCount(); i++)
	{
		MSXML2::IXMLDOMElementPtr WindowInfo = pXmlDoc->createElement(_T("WindowInfo"));
		WindowList->appendChild(WindowInfo);
		WindowInfo->setAttribute(_T("Name"), (LPCTSTR)m_list.GetItemText(i, 1));
		WindowInfo->setAttribute(_T("AddTime"), (LPCTSTR)m_list.GetItemText(i, 0));
		WindowInfo->setAttribute(_T("Description"), (LPCTSTR)m_list.GetItemText(i, 2));
	}

	return TRUE;
}

void CSCWidgetBlacklistDlg::OnLvnDeleteitemList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	POSITION pos = m_list.GetFirstSelectedItemPosition();
	bool b = pos != NULL;

	this->GetDlgItem(IDC_REMOVE)->EnableWindow(b);

	*pResult = 0;
}

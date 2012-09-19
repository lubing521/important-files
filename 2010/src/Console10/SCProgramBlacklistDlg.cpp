// SCProgramBlacklistDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SCProgramBlacklistDlg.h"
#include "ProgramBlacklist.h"


// CSCProgramBlacklistDlg 对话框

IMPLEMENT_DYNAMIC(CSCProgramBlacklistDlg, CDialog)

CSCProgramBlacklistDlg::CSCProgramBlacklistDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSCProgramBlacklistDlg::IDD, pParent)
{

}

CSCProgramBlacklistDlg::~CSCProgramBlacklistDlg()
{
}

void CSCProgramBlacklistDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_list);
}


BEGIN_MESSAGE_MAP(CSCProgramBlacklistDlg, CDialog)
	ON_REGISTERED_MESSAGE(i8desk::g_nOptApplyMsg, &CSCProgramBlacklistDlg::OnApplyMessage)
	ON_BN_CLICKED(IDC_ADD, &CSCProgramBlacklistDlg::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_REMOVE, &CSCProgramBlacklistDlg::OnBnClickedRemove)
	ON_NOTIFY(NM_CLICK, IDC_LIST, &CSCProgramBlacklistDlg::OnNMClickList)
	ON_NOTIFY(LVN_DELETEITEM, IDC_LIST, &CSCProgramBlacklistDlg::OnLvnDeleteitemList)
END_MESSAGE_MAP()


// CSCProgramBlacklistDlg 消息处理程序

BOOL CSCProgramBlacklistDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_list.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_list.InsertColumn(0, _T("添加时间"), LVCFMT_LEFT, 120);
	m_list.InsertColumn(1, _T("程序"), LVCFMT_LEFT, 120);
	m_list.InsertColumn(2, _T("说明"), LVCFMT_LEFT, 200);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSCProgramBlacklistDlg::InsertItem(LPCTSTR lpAddTime, LPCTSTR lpName, LPCTSTR lpDesc)
{
	int nItem = m_list.GetItemCount();
	m_list.InsertItem(nItem, lpAddTime);
	m_list.SetItemText(nItem, 1, lpName);
	m_list.SetItemText(nItem, 2, lpDesc);
}

void CSCProgramBlacklistDlg::OnBnClickedAdd()
{
	CProgramBlacklist dlg;
	if (dlg.DoModal() == IDOK && !dlg.m_strProgram.IsEmpty()) {
		int nItem = m_list.GetItemCount();
		CTime tAdd = CTime::GetCurrentTime();
		m_list.InsertItem(nItem, tAdd.Format(_T("%Y/%m/%d %H:%M:%S")));
		m_list.SetItemText(nItem, 1, dlg.m_strProgram);
		m_list.SetItemText(nItem, 2, dlg.m_strDescription);
	}
}

void CSCProgramBlacklistDlg::OnBnClickedRemove()
{
	int nSel = -1;
	while ((nSel = m_list.GetNextItem(nSel, LVIS_SELECTED)) != -1 )
	{
		m_list.DeleteItem(nSel);
		nSel = -1;
	}
}

void CSCProgramBlacklistDlg::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	POSITION pos = m_list.GetFirstSelectedItemPosition();
	bool b = pos != NULL;

	this->GetDlgItem(IDC_REMOVE)->EnableWindow(b);

	*pResult = 0;
}

LRESULT CSCProgramBlacklistDlg::OnApplyMessage(WPARAM wParam, LPARAM lParam)
{
	MSXML2::IXMLDOMDocument *pXmlDoc = reinterpret_cast<MSXML2::IXMLDOMDocument *>(wParam);
	ASSERT(pXmlDoc);

	MSXML2::IXMLDOMElement *pRootElem = reinterpret_cast<MSXML2::IXMLDOMElement *>(lParam);
	ASSERT(pRootElem);

	MSXML2::IXMLDOMElementPtr ProccessList = pXmlDoc->createElement(_T("ProccessList"));
	pRootElem->appendChild(ProccessList);

	for (int i = 0; i < m_list.GetItemCount(); i++)
	{
		MSXML2::IXMLDOMElementPtr ProccessInfo = pXmlDoc->createElement(_T("ProccessInfo"));
		ProccessList->appendChild(ProccessInfo);
		ProccessInfo->setAttribute(_T("Name"), (LPCTSTR)m_list.GetItemText(i, 1));
		ProccessInfo->setAttribute(_T("AddTime"), (LPCTSTR)m_list.GetItemText(i, 0));
		ProccessInfo->setAttribute(_T("Description"), (LPCTSTR)m_list.GetItemText(i, 2));
	}

	return TRUE;
}

void CSCProgramBlacklistDlg::OnLvnDeleteitemList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	POSITION pos = m_list.GetFirstSelectedItemPosition();
	bool b = pos != NULL;

	this->GetDlgItem(IDC_REMOVE)->EnableWindow(b);

	*pResult = 0;
}

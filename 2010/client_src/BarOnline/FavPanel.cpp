// FavPanel.cpp : 实现文件
//

#include "stdafx.h"
#include "BarOnline.h"
#include "FavPanel.h"
#include "MainFrame.h"
#include <intshcut.h>
#include <map>
using namespace std;

IMPLEMENT_DYNAMIC(CFavPanel, CDialog)

CFavPanel::CFavPanel(CWnd* pParent /*=NULL*/)
	: CDialog(CFavPanel::IDD, pParent)
{
	m_pImgTab = AfxGetSkinImage(TEXT("Skin/Wnd/fav/背景_收藏夹标签_1px.png"));
	m_hCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND));
}

CFavPanel::~CFavPanel()
{
	if (m_pImgTab)
	{
		m_pImgTab->DeleteObject();
		delete m_pImgTab;
	}
	if (m_treFont.GetSafeHandle())
	{
		m_treFont.DeleteObject();
	}
}

void CFavPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_FAV,   m_btnFav);
	DDX_Control(pDX, IDC_BTN_LINE,   m_btnLine);
	DDX_Control(pDX, IDC_BTN_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_BTN_MANAGE, m_btnManage);
	DDX_Control(pDX, IDC_TREE1,		m_treCtrl);
}

BEGIN_MESSAGE_MAP(CFavPanel, CDialog)
	ON_BN_CLICKED(IDOK, &CFavPanel::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CFavPanel::OnBnClickedCancel)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_SETCURSOR()
	ON_BN_CLICKED(IDC_BTN_CLOSE, &CFavPanel::OnBnClickedBtnClose)
	ON_BN_CLICKED(IDC_BTN_MANAGE, &CFavPanel::OnBnClickedBtnManage)
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CFavPanel::OnTvnSelchangedTree1)
//	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, &CFavPanel::OnNMDblclkTree1)
	ON_WM_DESTROY()
	ON_WM_PAINT()
//	ON_NOTIFY(NM_CLICK, IDC_TREE1, &CFavPanel::OnNMClickTree1)
END_MESSAGE_MAP()

BOOL CFavPanel::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_btnFav.SetImage(TEXT("Skin/Wnd/fav/按钮_收藏夹_选中状态.png"));
	m_btnLine.SetImage(TEXT("Skin/Wnd/fav/按钮_收藏夹_未选中状态.png"));
	m_btnFav.SetTextStyle(DEFAULT_FONT_NAME, 14, RGB(0, 0, 0), RGB(0, 0, 0), 
		RGB(0, 0, 0), RGB(0, 0, 0), CButtonEx::BTEXT_CENTER, TRUE);
	m_btnClose.SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(0, 64, 128), RGB(0, 64, 128), 
		RGB(0, 64, 128), RGB(0, 64, 128));
	m_btnManage.SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(128, 128, 0), RGB(128, 128, 0), 
		RGB(128, 128, 0), RGB(128, 128, 0));

	HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
	m_btnFav.SetCursor(hCursor);
	m_btnClose.SetCursor(hCursor);
	m_btnManage.SetCursor(hCursor);

	m_itreCtrl.Create(16, 16, ILC_COLOR32|ILC_MASK, 1, 1);
	m_itreCtrl.SetImageCount(2);
	CBitmap* pIco01 = AfxGetSkinImage(TEXT("Skin/Wnd/fav/ico01.png"));
	CBitmap* pIco02 = AfxGetSkinImage(TEXT("Skin/Wnd/fav/ico02.png"));
	m_itreCtrl.Replace(0, pIco01, NULL);
	m_itreCtrl.Replace(1, pIco02, NULL);
	m_treCtrl.SetImageList(&m_itreCtrl, LVSIL_NORMAL);
	if (pIco01 != NULL)
	{
		pIco01->DeleteObject();
		delete pIco01;
		pIco01 = NULL;
	}
	if (pIco02 != NULL)
	{
		pIco02->DeleteObject();
		delete pIco02;
		pIco02 = NULL;
	}

	LOGFONT lf;
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);
	lstrcpy(lf.lfFaceName, TEXT("宋体"));
	lf.lfHeight = 12;
	m_treFont.CreateFontIndirect(&lf);
	m_treCtrl.SetFont(&m_treFont);

	Refresh();

	return TRUE;
}

void CFavPanel::Refresh()
{
	FreeData(m_treCtrl.GetRootItem());
	m_treCtrl.DeleteAllItems();

	TCHAR szFavFolder[MAX_PATH] = {0};
	if (S_OK != SHGetFolderPath(m_hWnd, CSIDL_FAVORITES|CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, szFavFolder))
		return ;
	PathAddBackslash(szFavFolder);
	GetAllFavUrl(szFavFolder, TVI_ROOT);
}

void CFavPanel::GetAllFavUrl(LPCTSTR szDir, HTREEITEM hParentItem /* = NULL */)
{
	TCHAR szFinderDir[MAX_PATH] = {0};
	_stprintf(szFinderDir, TEXT("%s*.*"), szDir);
	
	WIN32_FIND_DATA wfd = {0};
	HANDLE hFinder = FindFirstFile(szFinderDir, &wfd);
	if (hFinder == INVALID_HANDLE_VALUE)
		return ;

	BOOL bRes = TRUE;
	map<CString, CString> lstFiles;
	while (bRes)
	{
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (wfd.cFileName[0] != TEXT('.'))
			{
				//HTREEITEM hItem = m_treCtrl.InsertItem(wfd.cFileName, TVI_ROOT);
				HTREEITEM hItem = m_treCtrl.InsertItem(wfd.cFileName, 0, 0, TVI_ROOT);
				TCHAR szNewDir[MAX_PATH] = {0};
				//先插入目录
				_stprintf(szNewDir, TEXT("%s%s\\"), szDir, wfd.cFileName);
				m_treCtrl.SetItemData(hItem, 1);
				GetAllFavUrl(szNewDir, hItem);
			}
		}
		else
		{
			CString str(szDir);
			str += wfd.cFileName;
			lstFiles.insert(make_pair(str, CString(wfd.cFileName)));
		}
		bRes = FindNextFile(hFinder, &wfd);
	}

	//后插入文件
	CoInitialize(NULL);
	for (map<CString, CString>::iterator it=lstFiles.begin(); it != lstFiles.end(); it++)
	{
		IUniformResourceLocator *pUrl = NULL;
		HRESULT hRes = CoCreateInstance (CLSID_InternetShortcut, NULL, CLSCTX_INPROC_SERVER, 
			IID_IUniformResourceLocator, (LPVOID*)&pUrl);
		if (SUCCEEDED(hRes))
		{
			IPersistFile *pPF = NULL;
			hRes = pUrl->QueryInterface(IID_IPersistFile, (void **)&pPF);
			if (SUCCEEDED(hRes))
			{
				LPTSTR pszUrl = NULL;
				if (S_OK == pPF->Load(it->first, STGM_READWRITE) && S_OK == pUrl->GetURL(&pszUrl) && pszUrl != NULL)
				{
					CString str = it->second;
					if (str.Right(4).CompareNoCase(TEXT(".url")) == 0)
					{
						str.Delete(str.GetLength() - 4, 4);
					}
					//HTREEITEM hItem = m_treCtrl.InsertItem(str, hParentItem);
					HTREEITEM hItem = m_treCtrl.InsertItem(str, 1, 1, hParentItem);
					m_treCtrl.SetItemData(hItem, (DWORD)pszUrl);
					//m_treCtrl.EnsureVisible(hItem);
				}
				pPF->Release();
			}
			pUrl->Release();
		}
	}
	CoUninitialize();
	FindClose(hFinder);
}

void CFavPanel::OnBnClickedOk()
{
}

void CFavPanel::OnBnClickedCancel()
{
	OnCancel();
}

HBRUSH CFavPanel::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	static CBrush brush(RGB(255, 255, 255));
	if (nCtlColor == CTLCOLOR_DLG)
		return brush;

	return hbr;
}

void CFavPanel::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (m_btnFav.GetSafeHwnd() == NULL)
		return ;

	CRect rc;
	GetClientRect(&rc);

	CRect rx = CRect(5, 5, 5+109, 5+29);
	m_btnFav.MoveWindow(&rx);

	rx.OffsetRect(rx.Width(), 0);
	m_btnLine.MoveWindow(&rx);

	rx.left = rc.right - 35;
	rx.right = rc.right-5;
	m_btnClose.MoveWindow(&rx);

	rx.right = rx.left - 5;
	rx.left  = rx.right - 60;
	m_btnManage.MoveWindow(&rx);
	
	rc.left = 2;
	rc.top = rx.bottom + 5;
	rc.DeflateRect(1, 1, 1, 1);
	m_treCtrl.MoveWindow(&rc);
}

void CFavPanel::OnBnClickedBtnClose()
{
	CMainFrame* pFrame = reinterpret_cast<CMainFrame*>(AfxGetMainWnd());
	if (pFrame != NULL && pFrame->m_hWnd != NULL)
	{
		pFrame->HideUrlFavorite();
	}
}

void CFavPanel::OnBnClickedBtnManage()
{

}

void CFavPanel::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	HTREEITEM hCurSel = m_treCtrl.GetSelectedItem();
	if (hCurSel == NULL)
		return ;

	CMainFrame* pFrame = reinterpret_cast<CMainFrame*>(AfxGetMainWnd());
	if (pFrame != NULL && pFrame->m_hWnd != NULL)
	{
		DWORD dwData = m_treCtrl.GetItemData(hCurSel);
		if (dwData != 1)
		{
			CString str((LPCTSTR)dwData);
			pFrame->ClickUrlFavorite(str);
		}
	}
}

//void CFavPanel::OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	*pResult = 0;
//}

void CFavPanel::FreeData(HTREEITEM hItem)
{
	if (hItem != NULL)
	{
//for debug view.
// 		CString str = m_treCtrl.GetItemText(hItem);
// 		OutputDebugString(str);
// 		OutputDebugString(TEXT("\r\n"));

		DWORD dwData = m_treCtrl.GetItemData(hItem);
		if (dwData != 1)
		{
			LPCTSTR lpStr = (LPCTSTR)dwData;
			CoTaskMemFree((LPVOID)lpStr);
		}

		FreeData(m_treCtrl.GetChildItem(hItem));
		FreeData(m_treCtrl.GetNextSiblingItem(hItem));
	}
}

void CFavPanel::OnDestroy()
{
	FreeData(m_treCtrl.GetRootItem());

	CDialog::OnDestroy();
}

void CFavPanel::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
}

BOOL CFavPanel::OnEraseBkgnd( CDC* pDC )
{
    CRect rtClient, rtTab;
	GetClientRect(&rtClient);
	rtTab.top  = 1;
	rtTab.bottom = 34;
	pDC->FillRect(&rtClient, &CBrush(RGB(255, 255, 255)));

	CPen linePen(PS_SOLID, 1, RGB(201, 201, 201));
	CPen* pOldPen = pDC->SelectObject(&linePen);
	pDC->MoveTo(0, 0);
	pDC->LineTo(1, rtClient.bottom);
	pDC->MoveTo(rtClient.right - 1, 0);
	pDC->LineTo(rtClient.right - 1, rtClient.bottom);
    pDC->SelectObject(pOldPen);
	
	for (int i = 0; i <= rtClient.Width()/50; ++i)
	{
		rtTab.left = i*50;
		rtTab.right = rtTab.left + 50;
		AfxDrawImage(pDC, m_pImgTab, rtTab);
	}
	return TRUE;
}

BOOL CFavPanel::OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message )
{
	if (m_hCursor != NULL && m_treCtrl.m_hWnd
		&& pWnd->m_hWnd == m_treCtrl.m_hWnd)
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}
//void CFavPanel::OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	// TODO: 在此添加控件通知处理程序代码
//	*pResult = 0;
//}

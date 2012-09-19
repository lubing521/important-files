// GameListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "GameListCtrl.h"
#include "ConsoleDlg.h"

// CGameListCtrl

IMPLEMENT_DYNAMIC(CGameListCtrl, CListCtrl)

CGameListCtrl::CGameListCtrl()
	: m_pDragImage(0)
{
	m_hOkDropCursor = ::AfxGetApp()->LoadCursor(IDC_POINTER_COPY);
	m_hNoDropCursor =::AfxGetApp()->LoadCursor(IDC_NODROP);
}

CGameListCtrl::~CGameListCtrl()
{
	::DestroyCursor(m_hOkDropCursor);
	::DestroyCursor(m_hNoDropCursor);
}


BEGIN_MESSAGE_MAP(CGameListCtrl, CListCtrl)
	ON_WM_DROPFILES()
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, &CGameListCtrl::OnLvnBegindragList)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CGameListCtrl 消息处理程序
void CGameListCtrl::OnDropFiles(HDROP hDropInfo)
{
	this->GetParent()->SendMessage(WM_DROPFILES, (WPARAM)hDropInfo);
}

void CGameListCtrl::OnLvnBegindragList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
//*
    POINT pt;   
	int nOffset = 5; //offset in pixels for drag image (up and    
                         //to the left)   
    pt.x = nOffset;   
    pt.y = nOffset;   

	m_pDragImage = CreateDragImage(pNMLV->iItem, &pt);
    m_pDragImage->BeginDrag(0, CPoint(nOffset, nOffset));   
    m_pDragImage->DragEnter(GetDesktopWindow(), pNMLV->ptAction);

	this->SetCapture();
}


void CGameListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_pDragImage)
	{
		CPoint pt(point);
		ClientToScreen(&pt);

		m_pDragImage->DragShowNolock(FALSE);
		m_pDragImage->DragMove(pt);

		WPARAM wparam = (pt.x << 16) | pt.y;
		if (WindowFromPoint(pt) == this 
			|| GetParent()->SendMessage(WM_DRAGMOVE, wparam))
		{
			SetCursor(m_hOkDropCursor);
		}
		else 
		{
			SetCursor(m_hNoDropCursor);
		}
	}

	CListCtrl::OnMouseMove(nFlags, point);
}

void CGameListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_pDragImage)
	{
		m_pDragImage->EndDrag();
		delete m_pDragImage;
		m_pDragImage = 0;
		ReleaseCapture();

		CPoint pt(point);
		ClientToScreen(&pt);

		//CWnd *pWnd = WindowFromPoint(pt);
		//if (pWnd->IsKindOf(RUNTIME_CLASS(CTreeCtrl)))
		{
			CWnd *pParent = this->GetParent();
			//if (pWnd->GetParent() == pParent)
			{
				WPARAM wparam = (pt.x << 16) | pt.y;
				pParent->SendMessage(WM_DRAGEND, wparam);
			}
		}
	}

	CListCtrl::OnMouseMove(nFlags, point);
}

#include "stdafx.h"
#include "CustomListCtrl.h"

#include "CustomButton.h"
#include "CustomImage.h"
#include "../UI/Skin/SkinCheckBox.h"
#include "../../../../include/UI/ImageHelpers.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const size_t Padding = 10;



CCustomColumnBase::CCustomColumnBase()
: bk_(new ATL::CImage)
, selBk_(new ATL::CImage)
, firstBK_(new ATL::CImage)
{}

CCustomColumnBase::~CCustomColumnBase()
{}


LRESULT CCustomColumnBase::SetBkImg(HBITMAP firstBK, HBITMAP bk, HBITMAP selBK)
{
	assert(bk != 0);
	assert(selBK != 0);
	assert(firstBK != 0);

	HBITMAP firstBKTmp = CopyBitmap(firstBK);
	HBITMAP bkTmp = CopyBitmap(bk);
	HBITMAP selBKTmp = CopyBitmap(selBK);

	bk_->Attach(bkTmp);
	selBk_->Attach(selBKTmp);
	firstBK_->Attach(firstBKTmp);

	return TRUE;
}

void CCustomColumnBase::DrawBK(CGridListCtrlEx& owner, CDC *pDC, const CRect &rcItem, int nRow, int nCol)
{
	if( nRow == 0 )
	{
		CRect rcBk(0, 0, firstBK_->GetWidth(), firstBK_->GetHeight());
		firstBK_->Draw(pDC->GetSafeHdc(), rcItem, rcBk);
	}

	if( nRow % 2 && !owner.IsRowSelected(nRow) )
	{
		CRect rcBk(0, 0, bk_->GetWidth(), bk_->GetHeight());
		bk_->Draw(pDC->GetSafeHdc(), rcItem, rcBk);
	}
	else if( owner.IsRowSelected(nRow) )
	{
		CRect rcImg(0, 0, selBk_->GetWidth(), selBk_->GetHeight());
		selBk_->Draw(pDC->GetSafeHdc(), rcItem, rcImg);
	}
}


CCustomColumnTraitIcon::CCustomColumnTraitIcon(CCustomListCtrl &owner)
: owner_(owner)
{}
CCustomColumnTraitIcon::~CCustomColumnTraitIcon()
{
}

size_t CCustomColumnTraitIcon::AddIcon(HBITMAP bmp)
{
	assert(bmp != 0);

	HBITMAP bmpTmp = CopyBitmap(bmp);
	ImagePtr img(new ATL::CImage);
	img->Attach(bmpTmp);
	icons_.push_back(img);

	return icons_.size() - 1;
}

bool CCustomColumnTraitIcon::Replace(size_t index, HBITMAP bmp)
{
	assert(index < icons_.size());

	HBITMAP tmp = CopyBitmap(bmp);
	if( index < icons_.size() )
	{
		icons_[index]->Attach(tmp);
		return true;
	}
	else
	{
		return false;
	}
}

void CCustomColumnTraitIcon::SetIconState(size_t row, size_t state)
{
	assert(state < icons_.size());
	if( iconInfos_.size() <= row )
		iconInfos_.push_back(state);
	else
		iconInfos_[row] = state;
}

void CCustomColumnTraitIcon::InitIcons(size_t size)
{
	iconInfos_.reserve(size);
}

void CCustomColumnTraitIcon::OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult)
{
	size_t nRow = (size_t)pLVCD->nmcd.dwItemSpec;
	size_t nSubItem = pLVCD->iSubItem;

	if( iconInfos_.size() <= nRow )
	{
		iconInfos_.reserve(nRow + 1);
		while(iconInfos_.size() < nRow + 1)
			iconInfos_.push_back(0);
	}

	if( pLVCD->nmcd.dwDrawStage == (CDDS_SUBITEM|CDDS_ITEMPREPAINT) )   
	{
		CRect rcItem;
		owner.GetSubItemRect(nRow, nSubItem, LVIR_BOUNDS, rcItem);
		CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
		pDC->SetBkMode(TRANSPARENT);
		
		// Back ground
		DrawBK(owner, pDC, rcItem, nRow, nSubItem);

		// Icon
		if( !icons_[iconInfos_[nRow]]->IsNull() )
			icons_[iconInfos_[nRow]]->Draw(pDC->GetSafeHdc(), rcItem.left, rcItem.top);

		// Text
		CString text = owner.GetItemText(nRow, nSubItem);
		CRect rcText = rcItem;
		rcText.left += icons_[iconInfos_[nRow]]->GetWidth() + 5;
		pDC->DrawText(text, rcText, DT_VCENTER | DT_SINGLELINE);
		
		*pResult |= CDRF_NEWFONT;
		*pResult |= CDRF_SKIPDEFAULT;
	}

}


CCustomColumnTraitGameIcon::CCustomColumnTraitGameIcon(CCustomListCtrl &owner, GamesType &curGames)
: owner_(owner)
, curGames_(curGames)
, padding_(0)
{}

CCustomColumnTraitGameIcon::~CCustomColumnTraitGameIcon()
{
}

void CCustomColumnTraitGameIcon::SetDefaultBmp(HICON icon)
{
	assert(icon != 0);

	defIcon_ = icon;

	ICONINFO iconInfo = {0};
	defIcon_.GetIconInfo(&iconInfo);

	padding_ = iconInfo.xHotspot * 2;
}

void CCustomColumnTraitGameIcon::OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult)
{
	size_t nRow = (size_t)pLVCD->nmcd.dwItemSpec;
	size_t nSubItem = pLVCD->iSubItem;

	assert(nRow < curGames_.size());

	if( pLVCD->nmcd.dwDrawStage == (CDDS_SUBITEM|CDDS_ITEMPREPAINT) )   
	{
		CRect rcItem;
		owner.GetSubItemRect(nRow, nSubItem, LVIR_BOUNDS, rcItem);
		CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
		pDC->SetBkMode(TRANSPARENT);

		// Back ground
		DrawBK(owner, pDC, rcItem, nRow, nSubItem);

		// Icon
		CRect rcIcon = rcItem;
		rcIcon.left = rcItem.left + 4;
		rcIcon.top	= rcItem.top + 4;
		if( curGames_[nRow]->Icon != 0 )
		{
			WTL::CIconHandle icon(curGames_[nRow]->Icon);
			icon.DrawIconEx(pDC->GetSafeHdc(), rcIcon.left, rcIcon.top, 16, 16);
		}
		else
			defIcon_.DrawIconEx(pDC->GetSafeHdc(), rcIcon.left, rcIcon.top, 16, 16);
		
		// Text
		CRect rcText = rcItem;
		rcText.left += padding_ + 5;
		CString text = owner.GetItemText(nRow, nSubItem);
		pDC->DrawText(text, rcText, DT_VCENTER | DT_SINGLELINE);

		*pResult |= CDRF_NEWFONT;
		*pResult |= CDRF_SKIPDEFAULT;
	}
}


CCustomColumnTraitPlugStatus::CCustomColumnTraitPlugStatus(PlugType &curPlugs)
: curPlugs_(curPlugs)
, isStatus_(true)
{}

CCustomColumnTraitPlugStatus::~CCustomColumnTraitPlugStatus()
{
}


void CCustomColumnTraitPlugStatus::SetProgressImg(HBITMAP normal, HBITMAP gray)
{
	HBITMAP normalTmp = CopyBitmap(normal);
	HBITMAP grayTmp = CopyBitmap(gray);

	progress_[0].reset(new ATL::CImage);
	progress_[1].reset(new ATL::CImage);

	progress_[0]->Attach(normalTmp);
	progress_[1]->Attach(grayTmp);
}

void CCustomColumnTraitPlugStatus::SetProgress(size_t row, size_t pos, bool isProgress)
{
	if( row >= progresses_.size() )
		progresses_.push_back(RowInfo(isProgress, row, pos));
	else
	{
		progresses_[row].pos_			= pos;
		progresses_[row].isProgress_	= isProgress;
	}
}

void CCustomColumnTraitPlugStatus::OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult)
{
	size_t nRow = (size_t)pLVCD->nmcd.dwItemSpec;
	size_t nSubItem = pLVCD->iSubItem;

	while( nRow >= progresses_.size() )
		progresses_.push_back(RowInfo());

	if( pLVCD->nmcd.dwDrawStage == (CDDS_SUBITEM|CDDS_ITEMPREPAINT) )   
	{
		CRect rcItem;
		owner.GetSubItemRect(nRow, nSubItem, LVIR_BOUNDS, rcItem);
		CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
		pDC->SetBkMode(TRANSPARENT);

		// Back ground
		DrawBK(owner, pDC, rcItem, nRow, nSubItem);

		// Icon
		CRect rcText = rcItem;


		if( !progresses_[nRow].isProgress_ )
		{
			// Status
			static ATL::CImage img;
			if( !img.IsNull() )
				img.Detach();

			size_t download = curPlugs_[nRow]->DownloadStatus;

			HBITMAP bmpTmp = 0;
			if( download == 0 ) // 未下载
			{
				bmpTmp = curPlugs_[nRow]->StatusBmp[1];
			}
			else				// 已下载
			{
				size_t status = curPlugs_[nRow]->Status;
				switch(status)
				{
				case 0:	// 未启用
					bmpTmp = curPlugs_[nRow]->StatusBmp[3];
					break;
				case 1: // 已启用
					bmpTmp = curPlugs_[nRow]->StatusBmp[2];
					break;
				default:
					bmpTmp = curPlugs_[nRow]->StatusBmp[3];
					break;
				}
			}


			img.Attach(bmpTmp);
			CPoint pt((rcItem.Width() - img.GetWidth()) / 2, (rcItem.Height() - img.GetHeight()) / 2);
			img.Draw(pDC->GetSafeHdc(), rcItem.left + pt.x, rcItem.top + pt.y, img.GetWidth(), img.GetHeight());

		}
		else
		{
			// Progress

			CRect rcProgress(0, 0, progress_[0]->GetWidth(), progress_[0]->GetHeight());

			CRect rcProgDest(CPoint(rcItem.left + 10, rcItem.top + (rcItem.Height() - rcProgress.Height()) / 2),
				CSize(rcProgress.Width(), rcProgress.Height()));

			progress_[0]->Draw(pDC->GetSafeHdc(), rcProgDest, rcProgress);

			rcProgDest.right = rcProgDest.left + progress_[1]->GetWidth() * progresses_[nRow].pos_ / 100;
			CRect rcNum(0, 0, progress_[1]->GetWidth() * progresses_[nRow].pos_ / 100, progress_[1]->GetHeight());
			if( rcNum.right == 0 )
				rcNum.right = 1;
			if( rcProgDest.Width() == 0 )
				rcProgDest.DeflateRect(0, 0, -1, 0);
			progress_[1]->Draw(pDC->GetSafeHdc(), rcProgDest, rcNum);

		}

		*pResult |= CDRF_NEWFONT;
		*pResult |= CDRF_SKIPDEFAULT;
	}
}



CCustomColumnTraitPlugRate::CCustomColumnTraitPlugRate(PlugType &curPlugs)
: curPlugs_(curPlugs)
{}

CCustomColumnTraitPlugRate::~CCustomColumnTraitPlugRate()
{
}


void CCustomColumnTraitPlugRate::OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult)
{
	size_t nRow = (size_t)pLVCD->nmcd.dwItemSpec;
	size_t nSubItem = pLVCD->iSubItem;

	assert(nRow < curPlugs_.size());

	if( pLVCD->nmcd.dwDrawStage == (CDDS_SUBITEM|CDDS_ITEMPREPAINT) )   
	{
		CRect rcItem;
		owner.GetSubItemRect(nRow, nSubItem, LVIR_BOUNDS, rcItem);
		CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
		pDC->SetBkMode(TRANSPARENT);

		// Back ground
		DrawBK(owner, pDC, rcItem, nRow, nSubItem);

		// Icon
		size_t rate = curPlugs_[nRow]->IdcClick;

		static ATL::CImage img;
		if( !img.IsNull() )
			img.Detach();

		img.Attach(curPlugs_[nRow]->rateBmp);

		CPoint pt((rcItem.Width() - rate * img.GetWidth()) / 2, (rcItem.Height() - img.GetHeight()) / 2);
		for(size_t i = 0; i != rate; ++i)
		{
			img.Draw(pDC->GetSafeHdc(), rcItem.left + pt.x, rcItem.top + pt.y, img.GetWidth(), img.GetHeight());

			pt.x += img.GetWidth();
		}
		

		*pResult |= CDRF_NEWFONT;
		*pResult |= CDRF_SKIPDEFAULT;
	}
}



CCustomColumnTraitPlugName::CCustomColumnTraitPlugName(PlugType &curPlugs)
: curPlugs_(curPlugs)
, iconBk_(new ATL::CImage)
{}

CCustomColumnTraitPlugName::~CCustomColumnTraitPlugName()
{
}

LRESULT CCustomColumnTraitPlugName::SetIconBK(HBITMAP iconBK)
{
	assert(iconBK != 0);

	HBITMAP iconBKTmp = CopyBitmap(iconBK);

	iconBk_->Attach(iconBKTmp);

	return TRUE;
}

void CCustomColumnTraitPlugName::OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult)
{
	size_t nRow = (size_t)pLVCD->nmcd.dwItemSpec;
	size_t nSubItem = pLVCD->iSubItem;

	assert(nRow < curPlugs_.size());

	if( pLVCD->nmcd.dwDrawStage == (CDDS_SUBITEM|CDDS_ITEMPREPAINT) )   
	{
		CRect rcItem;
		owner.GetSubItemRect(nRow, nSubItem, LVIR_BOUNDS, rcItem);
		CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
		pDC->SetBkMode(TRANSPARENT);

		// Back ground
		DrawBK(owner, pDC, rcItem, nRow, nSubItem);

		// Icon
		CRect rcIconBK(rcItem);
		rcIconBK.DeflateRect(10, 10, 0, 0);
		iconBk_->Draw(pDC->GetSafeHdc(), rcIconBK.left, rcIconBK.top, iconBk_->GetWidth(), iconBk_->GetHeight());

	
		CRect rcIcon(rcIconBK);
		rcIcon.DeflateRect(5, 5, 0, 0);
		pDC->DrawIcon(rcIcon.left, rcIcon.top, curPlugs_[nRow]->Icon);
		
		// Text
		CRect rcTitle(rcIconBK.left + 32 + 30, rcIconBK.top - 2, rcItem.right - 30, rcIconBK.top - 2 + 20);
		pDC->SetTextColor(RGB(0, 0, 255));

		CString title = curPlugs_[nRow]->Name;
		pDC->DrawText(title, rcTitle, DT_TOP | DT_LEFT | DT_SINGLELINE);


		pDC->SetTextColor(RGB(0, 0, 0));
		CRect rcContent(rcTitle);
		rcContent.top = rcTitle.bottom;
		rcContent.bottom = rcItem.bottom;

		CString content = curPlugs_[nRow]->Comment;
		pDC->DrawText(content, rcContent, DT_LEFT | DT_WORDBREAK);
		

		*pResult |= CDRF_NEWFONT;
		*pResult |= CDRF_SKIPDEFAULT;
	}
}



CCustomColumnTraitIconStyle::CCustomColumnTraitIconStyle(CCustomListCtrl &owner, GamesType &curGames)
: owner_(owner)
, curGames_(curGames)
, rowSize_(0)
, bk_(new ATL::CImage)
{

}

CCustomColumnTraitIconStyle::~CCustomColumnTraitIconStyle()
{
}


void CCustomColumnTraitIconStyle::SetDefaultBmp(HICON icon)
{
	defIcon_ = icon;

	imgList_.Create(80, 80, ILC_COLOR32 | ILC_MASK, 0, 0);
	owner_.SetImageList(&imgList_, LVSIL_SMALL);
}

void CCustomColumnTraitIconStyle::SetBkImg(HBITMAP bk)
{
	assert(bk != 0);

	HBITMAP bkTmp = CopyBitmap(bk);
	bk_->Attach(bkTmp);
}


void CCustomColumnTraitIconStyle::SetRowSize(size_t size)
{
	rowSize_ = size;
}

void CCustomColumnTraitIconStyle::OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult)
{
	size_t nRow = (size_t)pLVCD->nmcd.dwItemSpec;
	size_t nSubItem = pLVCD->iSubItem;

	size_t index = nRow * rowSize_ + nSubItem - 1;
	

	if( pLVCD->nmcd.dwDrawStage == (CDDS_SUBITEM|CDDS_ITEMPREPAINT) )   
	{
		CRect rcItem;
		owner.GetSubItemRect(nRow, nSubItem, LVIR_BOUNDS, rcItem);
		CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
		pDC->SetBkMode(TRANSPARENT);
	
		// Background
		if( nRow == 0 )
		{
			CRect rcBK(0, 0, bk_->GetWidth(), bk_->GetHeight());
			bk_->Draw(pDC->GetSafeHdc(), rcItem, rcBK);
		}

		if( index >= curGames_.size() )
		{
			*pResult |= CDRF_SKIPDEFAULT;
			return;
		}

		
		// 是否选中
		int selIndex = (int)::SendMessage(owner.GetSafeHwnd(), LVM_GETNEXTITEM, (WPARAM)-1, MAKELPARAM(LVNI_ALL | LVNI_SELECTED, 0));
		bool bIsSelected = (selIndex == (int) pLVCD->nmcd.dwItemSpec);

		if( bIsSelected ) 
		{
			pDC->FillSolidRect(rcItem, RGB(192, 255, 0));
		}

		if( pLVCD->nmcd.uItemState & CDIS_SELECTED 
			&& owner.GetFocusCell() == nSubItem 
			&& owner.GetFocusRow() == nRow )
		{
			pDC->FillSolidRect(rcItem, RGB(192, 255, 0));
		}

		// Icon
		CRect rcText = rcItem;
		CRect rcIcon = rcItem;
		
		rcIcon.left = rcItem.CenterPoint().x - 16;
		rcIcon.top  = rcItem.top + Padding;

		if( curGames_[index]->Icon!= 0 )
		{
			WTL::CIconHandle img(curGames_[index]->Icon);
			img.DrawIconEx(pDC->GetSafeHdc(), rcIcon.left, rcIcon.top, 32, 32);
			
			rcText.top += 32;
		}
		else
		{
			defIcon_.DrawIconEx(pDC->GetSafeHdc(), rcIcon.left, rcIcon.top, 32, 32);

			rcText.top += 32;
		}

		// Text
		rcText.top = rcText.bottom - 35;
		pDC->DrawText(curGames_[index]->Name, rcText, DT_CENTER | DT_WORDBREAK );

		*pResult |= CDRF_NEWFONT;
		*pResult |= CDRF_SKIPDEFAULT;
	}
}



CCustomProgressColumnTrait::CCustomProgressColumnTrait(CCustomListCtrl &owner)
: owner_(owner)
{
	progress_[0].reset(new ATL::CImage);
	progress_[1].reset(new ATL::CImage);
}
CCustomProgressColumnTrait::~CCustomProgressColumnTrait()
{}

void CCustomProgressColumnTrait::SetProgressImg(HBITMAP normal, HBITMAP gray)
{
	HBITMAP normalTmp = CopyBitmap(normal);
	HBITMAP grayTmp = CopyBitmap(gray);

	progress_[0]->Attach(normalTmp);
	progress_[1]->Attach(grayTmp);
}

void CCustomProgressColumnTrait::SetProgress(size_t row, size_t pos)
{
	if( row >= progresses_.size() )
		progresses_.push_back(std::make_pair(row, pos));
	else
		progresses_[row].second = pos;

	owner_.UpdateWindow();
}


void CCustomProgressColumnTrait::OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult)
{
	size_t nRow = (size_t)pLVCD->nmcd.dwItemSpec;
	size_t nSubItem = pLVCD->iSubItem;

	while( nRow >= progresses_.size() )
		progresses_.push_back(std::make_pair(nRow, 0));

	if( pLVCD->nmcd.dwDrawStage == (CDDS_SUBITEM|CDDS_ITEMPREPAINT) )   
	{
		CRect rcItem;
		owner.GetSubItemRect(nRow, nSubItem, LVIR_BOUNDS, rcItem);
		CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
		pDC->SetBkMode(TRANSPARENT);


		// Back ground
		DrawBK(owner, pDC, rcItem, nRow, nSubItem);

	
		// Progress
		CRect rcProgress(0, 0, progress_[0]->GetWidth(), progress_[0]->GetHeight());

		CRect rcProgDest(CPoint(rcItem.left + 10, rcItem.top + (rcItem.Height() - rcProgress.Height()) / 2),
			CSize(rcProgress.Width(), rcProgress.Height()));
		
		progress_[0]->Draw(pDC->GetSafeHdc(), rcProgDest, rcProgress);

		rcProgDest.right = rcProgDest.left + progress_[1]->GetWidth() * progresses_[nRow].second / 100;
		CRect rcNum(0, 0, progress_[1]->GetWidth() * progresses_[nRow].second / 100, progress_[1]->GetHeight());
		if( rcNum.right == 0 )
			rcNum.right = 1;
		if( rcProgDest.Width() == 0 )
			rcProgDest.DeflateRect(0, 0, -1, 0);
		progress_[1]->Draw(pDC->GetSafeHdc(), rcProgDest, rcNum);
		
		// Text
		pDC->SetTextColor(RGB(0, 0, 0));
		CString text = owner.GetItemText(nRow, nSubItem);
		CRect rcText = rcItem;
		pDC->DrawText(text, rcText, DT_VCENTER | DT_CENTER | DT_SINGLELINE);

		*pResult |= CDRF_NEWFONT;
		*pResult |= CDRF_SKIPDEFAULT;
	}
}


CCustomColumnTrait::CCustomColumnTrait()
{}


void CCustomColumnTrait::SetTextColor(size_t row, size_t col, COLORREF clr)
{
	textClr_[std::make_pair(row, col)] = clr;
}

void CCustomColumnTrait::OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult)
{
	switch (pLVCD->nmcd.dwDrawStage)
	{
		// Before painting a row
	case CDDS_SUBITEM|CDDS_ITEMPREPAINT:
		{
			int nRow = (int)pLVCD->nmcd.dwItemSpec;
			int nSubItem = pLVCD->iSubItem;

			
			CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
			pDC->SetBkMode(TRANSPARENT);
	
			CRect subItem;
			VERIFY(owner.GetSubItemRect(nRow, nSubItem, LVIR_BOUNDS, subItem));

			// BK
			DrawBK(owner, pDC, subItem, nRow, nSubItem);

			// Text
			COLORREF clr = textClr_[std::make_pair(nRow, nSubItem)];
			if( clr != 0 )
				pDC->SetTextColor(clr);
			else
				pDC->SetTextColor(RGB(0, 0, 0));

			CString text = owner.GetItemText(nRow, nSubItem);
			CRect rcText = subItem;
			rcText.DeflateRect(10, 0, 0, 0);
			pDC->DrawText(text, rcText, DT_VCENTER | DT_SINGLELINE);

			*pResult |= CDRF_NEWFONT;
			*pResult |= CDRF_SKIPDEFAULT;
		}
	}
}


class CCutomTraitButton
	: public CCustomButton
{
	typedef CCustomColumnTraitPlugButton::ButtonPos ButtonPos;
	typedef CCustomColumnTraitPlugButton::Callback Callback;

private:
	Callback callback_;
	size_t row_;
	ButtonPos pos_;

public:
	CCutomTraitButton(size_t row, ButtonPos pos, const Callback &callback)
		: row_(row)
		, pos_(pos)
		, callback_(callback)
	{}

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

BEGIN_MESSAGE_MAP(CCutomTraitButton, CCustomButton)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void CCutomTraitButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags, point);
	callback_(row_, pos_);
}


CCustomColumnTraitPlugButton::CCustomColumnTraitPlugButton()
{}

void CCustomColumnTraitPlugButton::SetButton(HBITMAP normal, HBITMAP hover, HBITMAP down, HBITMAP disbale)
{
	btnBmps_[0] = normal;
	btnBmps_[1] = hover;
	btnBmps_[2] = down;
	btnBmps_[3] = disbale;
}

void CCustomColumnTraitPlugButton::SetButtonCallback(const Callback &callback, const OnInitCallback &initCallback)
{
	callback_ = callback;
	initCallback_ = initCallback;
}

void CCustomColumnTraitPlugButton::SetButtonText(size_t row, ButtonPos pos, LPCTSTR text)
{
	for(Buttons::const_iterator iter = btns_.begin(); iter != btns_.end(); ++iter)
	{
		if( iter->first.first == row )
		{
			iter->second[pos]->SetWindowText(text);
			return;
		}
	}

	assert(0);
}

void CCustomColumnTraitPlugButton::EnableButton(size_t row, ButtonPos pos, bool enable)
{
	for(Buttons::const_iterator iter = btns_.begin(); iter != btns_.end(); ++iter)
	{
		if( iter->first.first == row )
		{
			iter->second[pos]->EnableWindow(enable ? TRUE : FALSE);
			return;
		}
	}

	assert(0);
}

void CCustomColumnTraitPlugButton::_CreateButton(CGridListCtrlEx& owner, int nRow, int nCol)
{
	CRect rcItem = GetCellEditRect(owner, nRow, nCol);

	size_t width = rcItem.Width() / 4 - 10;
	size_t height = rcItem.Height() - 40;

	size_t left = rcItem.left + 10;
	size_t top	= rcItem.top + 20;

	std::vector<ButtonPtr> btns;
	CRect rcConfig(CPoint(left, top), CSize(width, height));
	ButtonPtr pBtn1(_CreateButton(owner, _T("配置"), nRow, nCol, CONFIG, rcConfig));
	btns.push_back(pBtn1);

	CRect rcStart(CPoint(rcConfig.right + 10, top), CSize(width, height));
	ButtonPtr pBtn2(_CreateButton(owner, _T("启用"), nRow, nCol, START, rcStart));
	btns.push_back(pBtn2);

	CRect rcStop(CPoint(rcStart.right + 10, top), CSize(width, height));
	ButtonPtr pBtn3(_CreateButton(owner, _T("禁用"), nRow, nCol, STOP, rcStop));
	btns.push_back(pBtn3);

	CRect rcDownload(CPoint(rcStop.right + 10, top), CSize(width, height));
	ButtonPtr pBtn4(_CreateButton(owner, _T("下载"), nRow, nCol, DOWNLOAD, rcDownload));
	btns.push_back(pBtn4);

	btns_.insert(std::make_pair(std::make_pair(nRow, nCol), btns));
}


CCutomTraitButton* CCustomColumnTraitPlugButton::_CreateButton(CGridListCtrlEx& owner, LPCTSTR caption, int nRow, int nCol, ButtonPos pos, const CRect& rect)
{
	CCutomTraitButton* pBtn = new CCutomTraitButton(nRow, pos, callback_);
	VERIFY( pBtn->Create(caption, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rect, &owner, 0) );
	pBtn->SetImages(btnBmps_[0], btnBmps_[1], btnBmps_[2], btnBmps_[3]);

	// Configure font
	pBtn->SetFont(owner.GetCellFont());
	return pBtn;
}

void CCustomColumnTraitPlugButton::OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult)
{
	int nRow = (int)pLVCD->nmcd.dwItemSpec;
	int nSubItem = pLVCD->iSubItem;

	Buttons::const_iterator iter = btns_.find(std::make_pair(nRow, nSubItem));
	if( iter == btns_.end() )
	{
		_CreateButton(owner, nRow, nSubItem);
		initCallback_(nRow);
	}

	

	switch (pLVCD->nmcd.dwDrawStage)
	{
		// Before painting a row
	case CDDS_SUBITEM|CDDS_ITEMPREPAINT:
		{
			CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
			pDC->SetBkMode(TRANSPARENT);

			CRect subItem;
			VERIFY(owner.GetSubItemRect(nRow, nSubItem, LVIR_BOUNDS, subItem));
		
			// BK
			DrawBK(owner, pDC, subItem, nRow, nSubItem);

			*pResult |= CDRF_NEWFONT;
			*pResult |= CDRF_SKIPDEFAULT;
		}
	}
}



CCustomColumnTraitDownloadStatus::CCustomColumnTraitDownloadStatus()
: downloaded_(false)
{
}

void CCustomColumnTraitDownloadStatus::SetStatus(bool status)
{
	downloaded_ = status;
}

void CCustomColumnTraitDownloadStatus::SetStatusImage(HBITMAP download, HBITMAP unDownload)
{
	assert(download != 0);
	assert(unDownload != 0);

	status_[0] = download;
	status_[1] = unDownload;
}

void CCustomColumnTraitDownloadStatus::OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult)
{
	size_t nRow = (size_t)pLVCD->nmcd.dwItemSpec;
	size_t nSubItem = pLVCD->iSubItem;


	if( pLVCD->nmcd.dwDrawStage == (CDDS_SUBITEM|CDDS_ITEMPREPAINT) )   
	{
		CRect rcItem;
		owner.GetSubItemRect(nRow, nSubItem, LVIR_BOUNDS, rcItem);
		CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
		pDC->SetBkMode(TRANSPARENT);


		// Back ground
		DrawBK(owner, pDC, rcItem, nRow, nSubItem);

		CRect rcDest(rcItem);
		rcDest.bottom = rcDest.top + 2 * rcItem.Height();
		rcDest.top += 8;
		rcDest.left += 5;
		
		if( nRow == 0 )
		{
			if( customImg_.get() == 0 )
			{
				customImg_.reset(new CCustomImage);
				customImg_->Create(_T(""), WS_VISIBLE, rcDest, &owner);
			}

			customImg_->SetThemeParent(owner.GetSafeHwnd());
			customImg_->SetImage(status_[downloaded_ ? 0 : 1]);
		}


		*pResult |= CDRF_NEWFONT;
		*pResult |= CDRF_SKIPDEFAULT;
	}
}


CCustomColumnTraitServerStatus::CCustomColumnTraitServerStatus()
: online_(false)
{
	status_[0].reset(new ATL::CImage);
	status_[1].reset(new ATL::CImage);
}

void CCustomColumnTraitServerStatus::SetStatus(bool status)
{
	online_ = status;
}

void CCustomColumnTraitServerStatus::SetStatusImage(HBITMAP online, HBITMAP offline)
{
	assert(online != 0);
	assert(offline != 0);

	HBITMAP onlineTmp = CopyBitmap(online);
	HBITMAP offlineTmp = CopyBitmap(offline);

	status_[0]->Attach(onlineTmp);
	status_[1]->Attach(offlineTmp);
}

void CCustomColumnTraitServerStatus::OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult)
{
	size_t nRow = (size_t)pLVCD->nmcd.dwItemSpec;
	size_t nSubItem = pLVCD->iSubItem;

	
	if( pLVCD->nmcd.dwDrawStage == (CDDS_SUBITEM|CDDS_ITEMPREPAINT) )   
	{
		CRect rcItem;
		owner.GetSubItemRect(nRow, nSubItem, LVIR_BOUNDS, rcItem);
		CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
		pDC->SetBkMode(TRANSPARENT);


		// Back ground
		DrawBK(owner, pDC, rcItem, nRow, nSubItem);


		// Status
		CRect rcImg(rcItem);
		rcImg.left += 5;
		rcImg.right = rcItem.left + status_[0]->GetWidth();

		if( online_ )
		{
			status_[0]->Draw(pDC->GetSafeHdc(), rcImg);
		}
		else
		{
			status_[1]->Draw(pDC->GetSafeHdc(), rcImg);
		}

		static struct
		{
			stdex::tString text;
			COLORREF clr;
		}textInfo[] = 
		{
			{ _T("正常"), RGB(0, 255, 0) },
			{ _T("离线"), RGB(255, 0, 0) }
		};

		// Text
		COLORREF clr = textInfo[online_ ? 0 : 1].clr;
		CString text = textInfo[online_ ? 0 : 1].text.c_str();

		pDC->SetTextColor(clr);
		CRect rcText = rcItem;
		rcText.left = rcImg.right;
		rcText.DeflateRect(10, 0, 0, 0);
		pDC->DrawText(text, rcText, DT_VCENTER | DT_SINGLELINE);


		*pResult |= CDRF_NEWFONT;
		*pResult |= CDRF_SKIPDEFAULT;
	}
}


CCustomColumnTraitServerInfoStatus::CCustomColumnTraitServerInfoStatus(size_t defRows)
{
	for(size_t i = 0; i != defRows; ++i)
		clrs_[i] = RGB(0, 0, 0);
}

void CCustomColumnTraitServerInfoStatus::SetTextColor(size_t row, COLORREF clr)
{
	clrs_[row] = clr;
}

void CCustomColumnTraitServerInfoStatus::OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult)
{
	size_t nRow = (size_t)pLVCD->nmcd.dwItemSpec;
	size_t nSubItem = pLVCD->iSubItem;


	if( pLVCD->nmcd.dwDrawStage == (CDDS_SUBITEM|CDDS_ITEMPREPAINT) )   
	{
		CRect rcItem;
		owner.GetSubItemRect(nRow, nSubItem, LVIR_BOUNDS, rcItem);
		CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
		pDC->SetBkMode(TRANSPARENT);


		// Back ground
		DrawBK(owner, pDC, rcItem, nRow, nSubItem);

		if( nSubItem == 2 )
			pDC->SetTextColor(clrs_[nRow]);

		CString text = owner.GetItemText(nRow, nSubItem);
		rcItem.left += 5;
		pDC->DrawText(text, rcItem, DT_VCENTER | DT_SINGLELINE);
		

		*pResult |= CDRF_NEWFONT;
		*pResult |= CDRF_SKIPDEFAULT;
	}
}




CCustomColumnTraitDownloadRes::CCustomColumnTraitDownloadRes()
{
	operate_.reset(new ATL::CImage);
	operateBK_.reset(new ATL::CImage);
}


void CCustomColumnTraitDownloadRes::SetOperateImage(HBITMAP operate, HBITMAP operateBK)
{
	assert(operate != 0);
	assert(operateBK != 0);

	HBITMAP operateTmp = CopyBitmap(operate);
	HBITMAP operateBKTmp = CopyBitmap(operateBK);

	operate_->Attach(operateTmp);
	operateBK_->Attach(operateBKTmp);
}

void CCustomColumnTraitDownloadRes::OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult)
{
	size_t nRow = (size_t)pLVCD->nmcd.dwItemSpec;
	size_t nSubItem = pLVCD->iSubItem;


	if( pLVCD->nmcd.dwDrawStage == (CDDS_SUBITEM|CDDS_ITEMPREPAINT) )   
	{
		CRect rcItem;
		owner.GetSubItemRect(nRow, nSubItem, LVIR_BOUNDS, rcItem);
		CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
		pDC->SetBkMode(TRANSPARENT);


		// Back ground
		DrawBK(owner, pDC, rcItem, nRow, nSubItem);

		// Operate
		if( nRow == 0 )
		{
			CRect rcText = rcItem;
			CString text = owner.GetItemText(nRow, nSubItem);
			pDC->DrawText(text, rcText, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
		}
		else if( nRow == 1 )
		{
			static CString text[] = 
			{
				_T(""), _T(""), _T("查看资源"), _T(""), _T("查看资源"), 
				_T(""), _T("清理文件"), _T("设置限速"), _T("")
			};
			
			// bk
			operateBK_->Draw(pDC->GetSafeHdc(), rcItem);


			// icon
			CRect rcIcon(rcItem);
			rcIcon.right = rcIcon.left + operate_->GetWidth();
			if( !text[nSubItem].IsEmpty() )
				operate_->Draw(pDC->GetSafeHdc(), rcIcon);

			// text
			CRect rcText(rcIcon.right + 5, rcIcon.top, rcItem.right, rcItem.bottom);
			pDC->DrawText(text[nSubItem], rcText, DT_VCENTER | DT_SINGLELINE);
		}

		*pResult |= CDRF_NEWFONT;
		*pResult |= CDRF_SKIPDEFAULT;
	}
}


class CCutomTraitCheckBox
	: public i8desk::ui::SkinCheckBox
{
private:
	size_t row_;
	std::tr1::function<void(size_t, bool)> callback_;

public:
	CCutomTraitCheckBox(size_t row, const std::tr1::function<void(size_t, bool)> &callback)
		: row_(row)
		, callback_(callback)
	{}

	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point)
	{
		__super::OnLButtonDown(nFlags, point);
		callback_(row_, GetCheck() != BST_CHECKED);
	}
};

BEGIN_MESSAGE_MAP(CCutomTraitCheckBox, i8desk::ui::SkinCheckBox)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

CCustomColumnSmartCleanCheckTrait::CCustomColumnSmartCleanCheckTrait(CCustomListCtrl &list)
: list_(list)
{}


void CCustomColumnSmartCleanCheckTrait::RegisterCallback(const OnInitCallback &initCallback)
{
	initCallback_ = initCallback;
}

void CCustomColumnSmartCleanCheckTrait::SelectAll(bool flag)
{
	size_t i = 0;
	using std::tr1::placeholders::_1;

	for(CheckBoxs::iterator iter = btns_.begin(); iter != btns_.end(); ++iter)
	{
		iter->second.first->SetCheck(flag ? BST_CHECKED : BST_UNCHECKED);
		iter->second.second = flag;
	}
}

void CCustomColumnSmartCleanCheckTrait::OnCheck(size_t row, bool flag)
{
	btns_[row].second = flag;
	initCallback_(row);
}

void CCustomColumnSmartCleanCheckTrait::SetCheck(size_t row, bool flag)
{
	btns_[row].first->SetCheck(flag ? BST_CHECKED : BST_UNCHECKED);
	btns_[row].second = flag;
}

bool CCustomColumnSmartCleanCheckTrait::GetCheck(size_t row)
{
	return btns_[row].second;
}

void CCustomColumnSmartCleanCheckTrait::Resize(size_t cnt)
{
	btns_.clear();
	for(size_t i = 0; i != cnt; ++i)
	{
		CheckBoxs::const_iterator iter = btns_.find(i);
		if( iter == btns_.end() )
		{
			_CreateCheckBox(list_, i, 0);
		}
	}
}

void CCustomColumnSmartCleanCheckTrait::_CreateCheckBox(CGridListCtrlEx& owner, int nRow, int nCol)
{
	CRect rcItem = GetCellEditRect(owner, nRow, nCol);

	size_t left = rcItem.left + 5;
	size_t top	= rcItem.top + 5;
	CRect rect(CPoint(left, top), CSize(16, 16));
	CheckBoxPtr pBtn(new CCutomTraitCheckBox(nRow, std::tr1::bind(&CCustomColumnSmartCleanCheckTrait::OnCheck, this,
		std::tr1::placeholders::_1, std::tr1::placeholders::_2)));
	VERIFY( pBtn->Create(_T(""), WS_CHILD | WS_VISIBLE | BS_CHECKBOX | BS_AUTOCHECKBOX, rect, &owner, 0) );
	pBtn->ShowWindow(SW_SHOW);
	
	btns_.insert(std::make_pair(nRow, std::make_pair(pBtn, false)));
}


void CCustomColumnSmartCleanCheckTrait::OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult)
{
	int nRow = (int)pLVCD->nmcd.dwItemSpec;
	int nSubItem = pLVCD->iSubItem;

	CheckBoxs::const_iterator iter = btns_.find(nRow);
	if( iter == btns_.end() && nSubItem == 1 )
	{
		_CreateCheckBox(owner, nRow, nSubItem);
	}
	else if( nSubItem == 1 )
	{
		CRect rcItem = GetCellEditRect(owner, nRow, nSubItem);

		size_t left = rcItem.left + 5;
		size_t top	= rcItem.top + 5;
		CRect rect(CPoint(left, top), CSize(16, 16));

		iter->second.first->MoveWindow(rect, TRUE);
		iter->second.first->Invalidate(TRUE);
	}


	switch (pLVCD->nmcd.dwDrawStage)
	{
		// Before painting a row
	case CDDS_SUBITEM|CDDS_ITEMPREPAINT:
		{
			CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
			pDC->SetBkMode(TRANSPARENT);

			CRect subItem;
			VERIFY(owner.GetSubItemRect(nRow, nSubItem, LVIR_BOUNDS, subItem));

			// BK
			DrawBK(owner, pDC, subItem, nRow, nSubItem);

			*pResult |= CDRF_NEWFONT;
			*pResult |= CDRF_SKIPDEFAULT;
		}
	}
}



CCustomListCtrl::CCustomListCtrl(bool hasHeader)
: hasHeader_(hasHeader)
, height_(0)
{}

CCustomListCtrl::~CCustomListCtrl()
{

}

BEGIN_MESSAGE_MAP(CCustomListCtrl, CGridListCtrlEx)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_DESTROY()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
END_MESSAGE_MAP()


void CCustomListCtrl::SetHeaderImage(HBITMAP bg, HBITMAP line)
{
	header_.SetImage(bg, line);
}

void CCustomListCtrl::SetScrollImage(HBITMAP img[6])
{
	vScroll_.SetImages(img);
}

void CCustomListCtrl::SetItemHeight(size_t height)
{
	height_ = height;

	CRect rcWin;
	GetWindowRect(&rcWin);
	WINDOWPOS wp;
	wp.hwnd = m_hWnd;
	wp.cx = rcWin.Width();
	wp.cy = rcWin.Height();
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;

	SendMessage(WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp);


	// --
	imageList_.reset(new CImageList);
	imageList_->Create(height, height, ILC_COLOR32 | ILC_MASK, 0, 0);
	SetImageList(imageList_.get(), LVSIL_SMALL);
}


void CCustomListCtrl::AutoAdjustWidth()
{	
	CHeaderCtrl *header = GetHeaderCtrl();
	if( header == 0 )
		return;


	int nColumnCount = header->GetItemCount();
	if( nColumnCount <= 1 )
		return;

	CRect rcWindow;
	GetWindowRect(rcWindow);


	size_t width = 0;
	for(int i = 0; i < nColumnCount; ++i)
	{
		int nColumnWidth = GetColumnWidth(i);
		if( i < nColumnCount - 1 )
			width += nColumnWidth;
	}
	

	int realWidth = rcWindow.Width() - width;
	int lastWidth = GetColumnWidth(nColumnCount - 1);
	if( realWidth > lastWidth ) // 16--Scroll Bar Width
	{
		// 去掉滚动条占用的宽度
		SCROLLBARINFO sbi = {0};
		size_t scrollWidth = 0;
		if( GetScrollBarInfo(OBJID_VSCROLL, &sbi) )
			scrollWidth = sbi.rcScrollBar.right - sbi.rcScrollBar.left;
		
		SetColumnWidth(nColumnCount - 1, realWidth - scrollWidth - 16);
	}
}


void CCustomListCtrl::RedrawCurPageItems()
{
	RedrawItems(GetTopIndex(), GetTopIndex() + GetCountPerPage());
}

void CCustomListCtrl::PreSubclassWindow()
{
	__super::PreSubclassWindow();


	CRect windowRect;
	GetWindowRect(windowRect);

	CRect rcHeader;
	if( GetHeaderCtrl() && hasHeader_ )
	{
		header_.SubclassWindow(GetHeaderCtrl()->m_hWnd);
		header_.GetWindowRect(rcHeader);
	}

	ModifyStyle(0, WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	InitializeFlatSB(GetSafeHwnd());

	// ScrollBar
	//InitializeFlatSB(m_hWnd);
	//FlatSB_EnableScrollBar(m_hWnd, SB_BOTH, ESB_DISABLE_BOTH);

	

	//vScroll_.Create(this, WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE|WS_GROUP,
	//	CRect(CPoint(windowRect.right - 16, rcHeader.bottom), CSize(16, windowRect.Height() - rcHeader.Height())), this);

}


void CCustomListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{

	__super::OnCustomDraw(pNMHDR, pResult);
}

BOOL CCustomListCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_CHAR )
	{
		TCHAR chr = static_cast<TCHAR>(pMsg->wParam);
		switch ( chr )
		{
		case 0x01:	// 0x01 is the key code for ctrl-a and also for ctrl-A
			{
				SetItemState(-1, LVIS_SELECTED, LVIS_SELECTED);
				return TRUE;
			}
		default:
			break;
		}	
	}

	return FALSE;
}

void CCustomListCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rect;
	GetClientRect(&rect);
	CMemDC memDC(dc, rect);

	CRect headerRect;
	GetDlgItem(0)->GetWindowRect(&headerRect);
	ScreenToClient(&headerRect);
	dc.ExcludeClipRect(&headerRect);


	CRect clip;
	memDC.GetDC().GetClipBox(&clip);

	//vScroll_.UpdateThumbPosition();

	DefWindowProc(WM_PAINT, (WPARAM)memDC.GetDC().GetSafeHdc(), (LPARAM)0);   
}


BOOL CCustomListCtrl::OnEraseBkgnd(CDC* pDC)
{
	//vScroll_.UpdateThumbPosition();
	return TRUE;//__super::OnEraseBkgnd(pDC);
}

void CCustomListCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = height_;
}

void CCustomListCtrl::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	//ShowScrollBar(SB_BOTH, FALSE);

	if( ::IsWindow(header_.GetSafeHwnd()) )
		header_.Invalidate();

	
	AutoAdjustWidth();


	if( !::IsWindow(vScroll_.GetSafeHwnd()) )
		return;

	CRect windowRect;
	GetWindowRect(windowRect);

	CRect rcHeader;
	header_.GetWindowRect(rcHeader);
	ScreenToClient(rcHeader);

	ScreenToClient(windowRect);


	
	//vScroll_.MoveWindow(CRect(CPoint(windowRect.right - 16, windowRect.top + rcHeader.Height()), CSize(16, windowRect.Height() - rcHeader.Height())));

	
}

void CCustomListCtrl::OnMove(int x, int y)
{
	AutoAdjustWidth();
	__super::OnMove(x, y);
}

void CCustomListCtrl::OnDestroy()
{
	__super::OnDestroy();
}


BOOL CCustomListCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	//vScroll_.UpdateThumbPosition();
	return __super::OnMouseWheel(nFlags, zDelta, pt);
}

void CCustomListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	//vScroll_.UpdateThumbPosition();
	return __super::OnKeyDown(nFlags, nRepCnt, nFlags);
}

void CCustomListCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	//vScroll_.UpdateThumbPosition();
	return __super::OnKeyUp(nFlags, nRepCnt, nFlags);
}

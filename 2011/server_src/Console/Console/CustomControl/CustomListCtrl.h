#pragma once

#include "../GridListCtrlEx/CGridListCtrlEx.h"
#include "../GridListCtrlEx/CGridColumnTrait.h"
#include "../GridListCtrlEx/CGridRowTrait.h"
#include "../GridListCtrlEx/CGridColumnTraitText.h"

#include "CustomScrollBar.h"
#include "CustomHeaderCtrl.h"
#include "../Data/DataHelper.h"

#include <vector>

class CCustomListCtrl;

class CCustomColumnBase
	: public CGridColumnTraitText
{
	std::auto_ptr<ATL::CImage> bk_;
	std::auto_ptr<ATL::CImage> selBk_;
	std::auto_ptr<ATL::CImage> firstBK_;

public:
	CCustomColumnBase();
	~CCustomColumnBase();

public:
	LRESULT SetBkImg(HBITMAP firstBK, HBITMAP bk, HBITMAP selBK);
	void DrawBK(CGridListCtrlEx& owner, CDC *pDC, const CRect &rcItem, int row, int col);
};

class CCustomColumnTraitIcon
	: public CCustomColumnBase
{
private:
	typedef std::tr1::shared_ptr<ATL::CImage> ImagePtr;
	typedef std::vector<ImagePtr> Images;
	Images icons_;

	std::vector<size_t> iconInfos_;
	CCustomListCtrl &owner_;

public:
	explicit CCustomColumnTraitIcon(CCustomListCtrl &owner);
	~CCustomColumnTraitIcon();

public:
	void InitIcons(size_t size);
	size_t AddIcon(HBITMAP bmp);
	bool Replace(size_t index, HBITMAP bmp);
	void SetIconState(size_t row, size_t state);

public:
	virtual void OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult);
};


class CCustomColumnTraitIconStyle
	: public CGridColumnTrait
{
	typedef i8desk::data_helper::GameTraits::VectorType GamesType;

private:
	size_t rowSize_;
	GamesType &curGames_;
	CCustomListCtrl &owner_;
	CImageList imgList_;

	std::auto_ptr<ATL::CImage> bk_;
	WTL::CIconHandle defIcon_;

	
public:
	CCustomColumnTraitIconStyle(CCustomListCtrl &owner, GamesType &curGames);
	~CCustomColumnTraitIconStyle();

public:
	void SetBkImg(HBITMAP bk);
	void SetDefaultBmp(HICON icon);
	void SetRowSize(size_t size);
	
public:
	virtual void OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult);
};


class CCustomColumnTraitGameIcon
	: public CCustomColumnBase
{
	typedef i8desk::data_helper::GameTraits::VectorType GamesType;

private:
	GamesType &curGames_;
	WTL::CIconHandle defIcon_;
	size_t padding_;

	CCustomListCtrl &owner_;

public:
	CCustomColumnTraitGameIcon(CCustomListCtrl &owner, GamesType &curGames);
	~CCustomColumnTraitGameIcon();

public:
	void SetDefaultBmp(HICON icon);

public:
	virtual void OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult);
};


class CCustomColumnTraitPlugStatus
	: public CCustomColumnBase
{
	typedef i8desk::data_helper::PlugToolTraits::VectorType PlugType;

private:
	PlugType &curPlugs_;
	std::auto_ptr<ATL::CImage> progress_[2];

	struct RowInfo
	{
		bool isProgress_;
		size_t row_;
		size_t pos_;

		RowInfo(bool isProgress = false, size_t row = 0, size_t pos = 0)
			: isProgress_(false)
			, row_(0)
			, pos_(0)
		{}
	};
	std::vector<RowInfo> progresses_;

	bool isStatus_;

public:
	CCustomColumnTraitPlugStatus(PlugType &curPlugs);
	~CCustomColumnTraitPlugStatus();

public:
	void SetProgressImg(HBITMAP normal, HBITMAP gray);
	void SetProgress(size_t row, size_t pos, bool isProgress);

public:
	virtual void OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult);
};

class CCustomColumnTraitPlugRate
	: public CCustomColumnBase
{
	typedef i8desk::data_helper::PlugToolTraits::VectorType PlugType;

private:
	PlugType &curPlugs_;


public:
	CCustomColumnTraitPlugRate(PlugType &curPlugs);
	~CCustomColumnTraitPlugRate();

public:
	virtual void OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult);
};


class CCustomColumnTraitPlugName
	: public CCustomColumnBase
{
	typedef i8desk::data_helper::PlugToolTraits::VectorType PlugType;

private:
	PlugType &curPlugs_;

	std::auto_ptr<ATL::CImage> iconBk_;

public:
	CCustomColumnTraitPlugName(PlugType &curPlugs);
	~CCustomColumnTraitPlugName();

public:
	LRESULT SetIconBK(HBITMAP iconBk);

public:
	virtual void OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult);
};




class CCustomProgressColumnTrait
	: public CCustomColumnBase
{
private:
	std::auto_ptr<ATL::CImage> progress_[2];
	std::vector<std::pair<size_t, size_t>> progresses_;

	CCustomListCtrl &owner_;

public:
	CCustomProgressColumnTrait(CCustomListCtrl &owner);
	~CCustomProgressColumnTrait();

public:
	void SetProgressImg(HBITMAP normal, HBITMAP gray);
	void SetProgress(size_t row, size_t pos);

public:
	virtual void OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult);
};



class CCustomColumnTrait
	: public CCustomColumnBase
{
private:
	std::map<std::pair<size_t, size_t>, COLORREF> textClr_;

public:
	CCustomColumnTrait();

public:
	void SetTextColor(size_t row, size_t col, COLORREF clr);

public:
	virtual void OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult);
};


class CCutomTraitButton;

class CCustomColumnTraitPlugButton
	: public CCustomColumnBase
{
public:
	enum ButtonPos { CONFIG, START, STOP, DOWNLOAD };

	typedef std::tr1::function<void(size_t Row, ButtonPos pos)> Callback;
	typedef std::tr1::function<void(size_t row)>				OnInitCallback;
	typedef std::tr1::shared_ptr<CCutomTraitButton> ButtonPtr;
	typedef std::map<std::pair<size_t, size_t>, std::vector<ButtonPtr>> Buttons;

private:
	std::tr1::array<HBITMAP, 4> btnBmps_;
	Buttons btns_;
	Callback callback_;
	OnInitCallback initCallback_;

public:
	CCustomColumnTraitPlugButton();

public:
	void SetButton(HBITMAP normal, HBITMAP hover, HBITMAP down, HBITMAP disbale);
	void SetButtonCallback(const Callback &callback, const OnInitCallback &initCallback);
	void SetButtonText(size_t row, ButtonPos pos, LPCTSTR text);
	void EnableButton(size_t row, ButtonPos, bool enable);

public:
	virtual void OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult);

private:
	void _CreateButton(CGridListCtrlEx& owner, int nRow, int nCol);
	CCutomTraitButton* _CreateButton(CGridListCtrlEx& owner, LPCTSTR caption, int nRow, int nCol, ButtonPos pos, const CRect& rect);
};


class CCustomColumnTraitServerStatus
	: public CCustomColumnBase
{
private:
	std::auto_ptr<ATL::CImage> status_[2];
	bool online_;

public:
	CCustomColumnTraitServerStatus();

public:
	void SetStatus(bool status);
	void SetStatusImage(HBITMAP online, HBITMAP offline);

public:
	virtual void OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult);
};


class CCustomColumnTraitServerInfoStatus
	: public CCustomColumnBase
{
	typedef std::map<size_t, COLORREF> RowColor;
	RowColor clrs_;

public:
	explicit CCustomColumnTraitServerInfoStatus(size_t defRows);

public:
	void SetTextColor(size_t row, COLORREF clr);

public:
	virtual void OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult);
};


class CCustomImage;

class CCustomColumnTraitDownloadStatus
	: public CCustomColumnBase
{
private:
	HBITMAP status_[2];
	bool downloaded_;

	std::auto_ptr<CCustomImage> customImg_;

public:
	CCustomColumnTraitDownloadStatus();

public:
	void SetStatus(bool status);
	void SetStatusImage(HBITMAP online, HBITMAP offline);

public:
	virtual void OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult);
};

class CCustomColumnTraitDownloadRes
	: public CCustomColumnBase
{
private:
	std::auto_ptr<ATL::CImage> operate_;
	std::auto_ptr<ATL::CImage> operateBK_;

public:
	CCustomColumnTraitDownloadRes();

public:
	void SetOperateImage(HBITMAP operate, HBITMAP operateBK);

public:
	virtual void OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult);
};


class CCutomTraitCheckBox;

class CCustomColumnSmartCleanCheckTrait
	: public CCustomColumnTrait
{
public:
	typedef std::tr1::function<void(size_t row)>		OnInitCallback;
	typedef std::tr1::shared_ptr<CCutomTraitCheckBox>	CheckBoxPtr;
	typedef std::map<size_t, std::pair<CheckBoxPtr, bool>>				CheckBoxs;

private:
	CheckBoxs btns_;
	OnInitCallback initCallback_;
	CCustomListCtrl &list_;

public:
	CCustomColumnSmartCleanCheckTrait(CCustomListCtrl &list);

public:
	void RegisterCallback(const OnInitCallback &initCallback);
	void SelectAll(bool flag);
	void OnCheck(size_t row, bool flag);
	void SetCheck(size_t row, bool flag);
	bool GetCheck(size_t row);
	void Resize(size_t cnt);

public:
	virtual void OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult);

private:
	void _CreateCheckBox(CGridListCtrlEx& owner, int nRow, int nCol);
};


class CCustomListCtrl
	: public CGridListCtrlEx
{
private:
	bool hasHeader_;
	size_t height_;
	CCustomHeaderCtrl header_;
	CCustomVScrollbar vScroll_;

	std::auto_ptr<CImageList> imageList_;

public:
	explicit CCustomListCtrl(bool hasHeader = true);
	~CCustomListCtrl();

public:
	void SetHeaderImage(HBITMAP bg, HBITMAP line);
	void SetScrollImage(HBITMAP img[6]);
	void SetItemHeight(size_t height);
	void AutoAdjustWidth();

	void RedrawCurPageItems();

protected:
	virtual void PreSubclassWindow();
	virtual void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnDestroy();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
};
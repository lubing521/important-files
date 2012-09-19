#pragma once


#include <memory>
#include "../../../../include/ui/ImageHelpers.h"

/////////////////////////////////////////////////////////////////////////////
// CSkinHeaderCtrl window

class CCustomHeaderCtrl : public CHeaderCtrl
{

public:
	CCustomHeaderCtrl();
	virtual ~CCustomHeaderCtrl();

private:
	std::auto_ptr<ATL::CImage> imageBk_; 
	std::auto_ptr<ATL::CImage> imageLine_;

protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	
public:
	void SetImage(LPCTSTR bgImg, LPCTSTR lineImg);
	void SetImage(HBITMAP bgImg, HBITMAP lineImg);
	
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnLayout(WPARAM, LPARAM);
};

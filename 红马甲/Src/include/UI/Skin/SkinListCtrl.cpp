#include "stdafx.h"
#include "SkinListCtrl.h"
#include "SkinMgr.h"


namespace i8desk
{
	namespace ui
	{
		SkinListCtrl::SkinListCtrl(bool hasHeader)
		{
			SetHeaderImage(SkinMgrInstance().GetSkin(_T("Common/ListHeaderBg.png")), 
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderLine.png")));
		}

		void SkinListCtrl::PreSubclassWindow()
		{
			__super::PreSubclassWindow();
		}
	}
}
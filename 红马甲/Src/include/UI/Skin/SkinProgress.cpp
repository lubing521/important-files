#include "stdafx.h"
#include "SkinProgress.h"

#include "SkinMgr.h"


namespace i8desk
{
	namespace ui
	{
		void SkinProgress::PreSubclassWindow()
		{
			__super::PreSubclassWindow();

			HBITMAP bks[] = 
			{
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/LoadingGray_left.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/LoadingGray_mid.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/LoadingGray_right.png"))
			};
			HBITMAP imgs[] = 
			{
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/LoadingBlue_left.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/LoadingBlue_mid.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/LoadingBlue_right.png"))
			};

			SetImages(bks, imgs);
			SetThemeParent(GetParent()->GetSafeHwnd());
		}
	}
}
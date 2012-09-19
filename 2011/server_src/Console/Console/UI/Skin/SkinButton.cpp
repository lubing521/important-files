#include "stdafx.h"
#include "SkinButton.h"
#include "SkinMgr.h"


namespace i8desk
{
	namespace ui
	{
		void SkinButton::PreSubclassWindow()
		{
			__super::PreSubclassWindow();

			HBITMAP imgs[] = 
			{
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn_Hover.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn_Press.png"))
			};
			SetImages(imgs[0], imgs[1], imgs[2]);
			//SetThememParent(GetParent()->GetSafeHwnd());
		}
	}
}
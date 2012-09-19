#include "stdafx.h"
#include "SkinRadioBox.h"

#include "SkinMgr.h"


namespace i8desk
{
	namespace ui
	{
		void SkinRadioBox::PreSubclassWindow()
		{
			__super::PreSubclassWindow();

			HBITMAP imgs[] = 
			{
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/RadioNormal.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/RadioNormal.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/RadioSelected.png"))
			};
			
			SetThemeParent(GetParent()->GetSafeHwnd());
			SetImages(imgs[0], imgs[1], imgs[2]);
		}
	}
}
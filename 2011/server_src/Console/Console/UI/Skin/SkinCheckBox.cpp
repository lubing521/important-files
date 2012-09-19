#include "stdafx.h"
#include "SkinCheckBox.h"
#include "SkinMgr.h"


namespace i8desk
{
	namespace ui
	{
		void SkinCheckBox::PreSubclassWindow()
		{
			__super::PreSubclassWindow();

			HBITMAP imgs[] = 
			{
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/CheckboxNormal.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/CheckboxNormal.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/CheckBoxChecked.png"))
			};
			SetImages(imgs[0], imgs[1], imgs[2]);
			SetThemeParent(GetParent()->GetSafeHwnd());
		}
	}
}
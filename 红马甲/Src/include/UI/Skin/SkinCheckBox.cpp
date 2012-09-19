#include "stdafx.h"
#include "SkinCheckBox.h"


namespace ui
{
	namespace skin
	{
		void CheckBox::PreSubclassWindow()
		{
			__super::PreSubclassWindow();

			
			SetThemeParent(GetParent()->GetSafeHwnd());
		}
	}
}
#include "stdafx.h"
#include "SkinRadioBox.h"


namespace ui
{
	namespace skin
	{
		void RadioBox::PreSubclassWindow()
		{
			__super::PreSubclassWindow();
			SetThemeParent(GetParent()->GetSafeHwnd());
		}
	}
}
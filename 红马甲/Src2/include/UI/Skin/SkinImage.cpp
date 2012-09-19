#include "stdafx.h"
#include "SkinImage.h"


namespace ui
{
	namespace skin
	{
		void Image::PreSubclassWindow()
		{
			__super::PreSubclassWindow();

			SetThemeParent(GetParent()->GetSafeHwnd());
		}
	}
}
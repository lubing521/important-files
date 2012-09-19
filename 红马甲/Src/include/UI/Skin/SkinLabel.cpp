#include "stdafx.h"
#include "SkinLabel.h"

namespace ui
{
	namespace skin
	{
		void Label::PreSubclassWindow()
		{
			__super::PreSubclassWindow();

			SetThemeParent(GetParent()->GetSafeHwnd());
		}
	}
}
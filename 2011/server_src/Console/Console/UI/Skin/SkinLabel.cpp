#include "stdafx.h"
#include "SkinLabel.h"
#include "SkinMgr.h"


namespace i8desk
{
	namespace ui
	{
		void SkinLabel::PreSubclassWindow()
		{
			__super::PreSubclassWindow();

			SetThemeParent(GetParent()->GetSafeHwnd());
		}
	}
}
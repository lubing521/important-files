#include "stdafx.h"
#include "SkinImage.h"

#include "SkinMgr.h"


namespace i8desk
{
	namespace ui
	{
		void SkinImage::PreSubclassWindow()
		{
			__super::PreSubclassWindow();

			SetThemeParent(GetParent()->GetSafeHwnd());
		}
	}
}
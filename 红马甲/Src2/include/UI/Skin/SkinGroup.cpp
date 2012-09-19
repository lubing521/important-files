#include "stdafx.h"
#include "SkinGroup.h"
#include "SkinMgr.h"


namespace i8desk
{
	namespace ui
	{
		void SkinGroup::PreSubclassWindow()
		{
			__super::PreSubclassWindow();

			SetThemeParent(GetParent()->GetSafeHwnd());
			SetLine(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/GroupBK.png")));
		}
	}
}
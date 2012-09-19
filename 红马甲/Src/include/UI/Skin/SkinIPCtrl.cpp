#include "stdafx.h"
#include "SkinIPCtrl.h"

#include "SkinMgr.h"


namespace i8desk
{
	namespace ui
	{
		void SkinIPCtrl::PreSubclassWindow()
		{
			__super::PreSubclassWindow();

			SetImage(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputBg.png")));
		}
	}
}
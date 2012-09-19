#include "stdafx.h"
#include "SkinEdit.h"

#include "SkinMgr.h"


namespace i8desk
{
	namespace ui
	{
		void SkinEdit::PreSubclassWindow()
		{
			__super::PreSubclassWindow();

			HBITMAP imgs[] = 
			{
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputLeft.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputMid.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputRight.png"))
			};
			SetImages(imgs[0], imgs[1], imgs[2]);
			SetBkImg(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputBg.png")));
		}
	}
}
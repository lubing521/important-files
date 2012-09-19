#include "stdafx.h"
#include "SkinComboBox.h"

#include "SkinMgr.h"


namespace i8desk
{
	namespace ui
	{

		SkinComboBox::SkinComboBox()
			: CCustomComboBox(CSize(24, 24))
		{
			HBITMAP bk = i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputBg.png"));
			HBITMAP editBk = i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ComboEditBK.png"));
			SetBk(bk, editBk);
		}

		void SkinComboBox::PreSubclassWindow()
		{
			__super::PreSubclassWindow();

			HBITMAP arrow[] = 
			{
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ComboBoxArrow.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ComboBoxArrow.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ComboBoxArrow.png"))
			};
			SetArrow(arrow[0], arrow[1], arrow[2]);
		}
	}
}
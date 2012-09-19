#ifndef __SKIN_CHECKBOX_HPP
#define __SKIN_CHECKBOX_HPP


#include "../../CustomControl/CustomCheckBox.h"


namespace i8desk
{
	namespace ui
	{
		class SkinCheckBox
			: public CCustomCheckBox
		{
		protected:
			virtual void PreSubclassWindow();
		};
	}
}

#endif
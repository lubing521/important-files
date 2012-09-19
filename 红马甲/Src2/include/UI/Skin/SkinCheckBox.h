#ifndef __SKIN_CHECKBOX_HPP
#define __SKIN_CHECKBOX_HPP


#include "CustomControl/CustomCheckBox.h"


namespace ui
{
	namespace skin
	{
		class CheckBox
			: public CCustomCheckBox
		{
		protected:
			virtual void PreSubclassWindow();
		};
	}
}

#endif
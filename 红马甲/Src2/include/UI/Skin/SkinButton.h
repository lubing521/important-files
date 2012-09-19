#ifndef __SKIN_BUTTON_HPP
#define __SKIN_BUTTON_HPP

#include "CustomControl/CustomButton.h"


namespace ui
{
	namespace skin
	{
		class Button
			: public CCustomButton
		{
		protected:
			virtual void PreSubclassWindow();
		};

	}
}

#endif
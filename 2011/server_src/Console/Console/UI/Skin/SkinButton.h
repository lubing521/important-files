#ifndef __SKIN_BUTTON_HPP
#define __SKIN_BUTTON_HPP

#include "../../CustomControl/CustomButton.h"


namespace i8desk
{
	namespace ui
	{
		class SkinButton
			: public CCustomButton
		{
		protected:
			virtual void PreSubclassWindow();
		};

	}
}

#endif
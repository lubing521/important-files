#ifndef __SKIN_RADIOBOX_HPP
#define __SKIN_RADIOBOX_HPP


#include "CustomControl/CustomRadio.h"


namespace ui
{
	namespace skin
	{
		class RadioBox
			: public CCustomRadio
		{
		protected:
			virtual void PreSubclassWindow();
		};

	}
}


#endif
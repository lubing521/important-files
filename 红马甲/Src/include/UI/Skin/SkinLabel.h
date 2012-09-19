#ifndef __SKIN_LABEL_HPP
#define __SKIN_LABEL_HPP


#include "CustomControl/CustomLabel.h"


namespace ui
{
	namespace skin
	{
		class Label
			: public CCustomLabel
		{
		protected:
			virtual void PreSubclassWindow();
		};

	}
}


#endif
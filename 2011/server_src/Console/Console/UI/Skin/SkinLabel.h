#ifndef __SKIN_LABEL_HPP
#define __SKIN_LABEL_HPP


#include "../../CustomControl/CustomLabel.h"


namespace i8desk
{
	namespace ui
	{
		class SkinLabel
			: public CCustomLabel
		{
		protected:
			virtual void PreSubclassWindow();
		};

	}
}


#endif
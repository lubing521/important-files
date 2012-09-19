#ifndef __SKIN_PROGRESS_HPP
#define __SKIN_PROGRESS_HPP

#include "../../CustomControl/CustomProgress.h"

namespace i8desk
{
	namespace ui
	{
		class SkinProgress
			: public CCustomProgress
		{
		protected:
			virtual void PreSubclassWindow();
		};

	}
}


#endif
#ifndef __SKIN_GROUP_BOX_HPP
#define __SKIN_GROUP_BOX_HPP


#include "../../CustomControl/CustomGroup.h"


namespace i8desk
{
	namespace ui
	{
		class SkinGroup
			: public CCustomGroup
		{
		protected:
			virtual void PreSubclassWindow();
		};

	}
}


#endif
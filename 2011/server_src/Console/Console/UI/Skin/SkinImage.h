#ifndef __SKIN_IMAGE_HPP
#define __SKIN_IMAGE_HPP


#include "../../CustomControl/CustomImage.h"


namespace i8desk
{
	namespace ui
	{
		class SkinImage
			: public CCustomImage
		{
		protected:
			virtual void PreSubclassWindow();
		};

	}
}

#endif
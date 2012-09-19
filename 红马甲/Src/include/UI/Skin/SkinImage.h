#ifndef __SKIN_IMAGE_HPP
#define __SKIN_IMAGE_HPP


#include "CustomControl/CustomImage.h"


namespace ui
{
	namespace skin
	{
		class Image
			: public CCustomImage
		{
		protected:
			virtual void PreSubclassWindow();
		};

	}
}

#endif
#ifndef __SKIN_IP_CONTROL_HPP
#define __SKIN_IP_CONTROL_HPP


#include "../../CustomControl/CustomIPAddress.h"


namespace i8desk
{
	namespace ui
	{
		class SkinIPCtrl
			: public CCustomIPAddress
		{
		protected:
			virtual void PreSubclassWindow();
		};

	}
}

#endif
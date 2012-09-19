#ifndef __SKIN_EDIT_HPP
#define __SKIN_EDIT_HPP

#include "../../CustomControl/CustomEdit.h"


namespace async
{
	namespace ui
	{
		class SkinEdit
			: public CCustomEdit
		{
		protected:
			virtual void PreSubclassWindow();
		};

	}
}

#endif
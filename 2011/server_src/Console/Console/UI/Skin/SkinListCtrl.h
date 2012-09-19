#ifndef __SKIN_LIST_CTRL_HPP
#define __SKIN_LIST_CTRL_HPP

#include "../../CustomControl/CustomListCtrl.h"


namespace i8desk
{
	namespace ui
	{
		class SkinListCtrl
			: public CCustomListCtrl
		{
		public:
			SkinListCtrl(bool hasHeader = true);

		protected:
			virtual void PreSubclassWindow();
		};
	}
}


#endif
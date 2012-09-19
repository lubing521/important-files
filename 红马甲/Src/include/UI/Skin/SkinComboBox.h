#ifndef __SKIN_COMBOBOX_HPP
#define __SKIN_COMBOBOX_HPP

#include "../../CustomControl/CustomComboBox.h"


namespace i8desk
{
	namespace ui
	{
		class SkinComboBox
			: public CCustomComboBox
		{
		public:
			SkinComboBox();

		protected:
			virtual void PreSubclassWindow();
		};
	}
}



#endif
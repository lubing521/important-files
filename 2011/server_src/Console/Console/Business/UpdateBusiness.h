#ifndef __UPDATE_GAME_BUSINESS_HPP
#define __UPDATE_GAME_BUSINESS_HPP

#include "../Data/DataHelper.h"

namespace i8desk
{
	namespace business
	{
		namespace update
		{
			stdex::tString GetState(const data_helper::UpdateGameStatusTraits::ValueType &val);
		}
	}
}



#endif
#include "stdafx.h"
#include "UpdateBusiness.h"


namespace i8desk
{
	namespace business
	{
		namespace update
		{

			stdex::tString GetState(const data_helper::UpdateGameStatusTraits::ValueType &val)
			{
				stdex::tString text;

				switch(val->UpdateState) 
				{
				case 1:		text = _T("正在更新");	break;
				default:	text = _T("");			break;
				}

				return text;
			}
		}
	}
}
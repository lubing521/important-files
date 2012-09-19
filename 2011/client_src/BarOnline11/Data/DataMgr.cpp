#include "stdafx.h"
#include "DataMgr.h"

#include "detail/SysOptData.h"


namespace i8desk
{
	namespace data
	{
		struct DataMgr::DataMgrImpl
		{
			detail::SysOptImpl sysOpt_;

			DataMgrImpl()
			{

			}

			template < typename T >
			const T &GetSysOpt(const stdex::tString &key, const T &val)
			{
				return sysOpt_->GetValue(key, val);
			}
		};




		DataMgr::DataMgr()
			: impl_(new DataMgrImpl)
		{

		}
	}
}
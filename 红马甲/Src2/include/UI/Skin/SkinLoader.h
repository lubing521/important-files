#ifndef __SKIN_LOADER_HPP
#define __SKIN_LOADER_HPP

#include "../../../../../include/Utility/SmartHandle.hpp"


namespace i8desk
{
	namespace ui
	{
		// Interface
		struct SkinLoaderInterface
		{
			virtual ~SkinLoaderInterface() = 0 {}

			virtual utility::BITMAPPtr Load(const stdex::tString &path) = 0;
			virtual utility::ICONPtr Load(long gid) = 0;
		};

		typedef std::tr1::shared_ptr<SkinLoaderInterface> SkinLoaderPtr;

		// 获取接口
		SkinLoaderPtr GetSkinLoader();
	}
}


#endif
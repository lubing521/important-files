#ifndef __SKIN_MANAGER_HPP
#define __SKIN_MANAGER_HPP

#include <memory>
#include <map>
#include "SkinInfo.h"
#include "SkinLoader.h"


namespace i8desk
{
	namespace ui
	{
		class SkinMgr
		{
		private:
			stdex::tString skinsPath_;								// 皮肤路径
			stdex::tString defaultSkinPath_;						// 默认皮肤路径
			std::auto_ptr<SkinInfo> currentSkin_;					// 当前皮肤
			SkinLoaderPtr loader_;									// 皮肤加载器
			typedef std::map<stdex::tString, utility::BITMAPPtr> Skins;
			Skins skins_;											// 所有皮肤

			typedef std::map<long, utility::ICONPtr> Icons;
			Icons icons_;											// 所有图标

		public:
			SkinMgr();
			~SkinMgr();

			// Ban
		private:
			SkinMgr(const SkinMgr&);
			SkinMgr& operator=(const SkinMgr&);


		public:
			const SkinInfo *GetSkinInfo() const;						// 获取当前皮肤信息	
			void SetSkinInfo(const stdex::tString &skinFile);			// 设置当前皮肤信息

		public:
			utility::BITMAPPtr GetSkin(const stdex::tString &name);		// 获取皮肤
			utility::ICONPtr GetIcon(long gid);							// 获取图标
		};


		SkinMgr &SkinMgrInstance();
	}

}




#endif
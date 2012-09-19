#include "stdafx.h"
#include "SkinMgr.h"

#include "SkinInfo.h"
#include "../../../../../include/ui/ImageHelpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace i8desk
{
	namespace ui
	{
		SkinMgr::SkinMgr()
			: currentSkin_(new SkinInfo)
		{
			
			currentSkin_->Load(skinsPath_);
		}
		SkinMgr::~SkinMgr()
		{}

	
		const SkinInfo *SkinMgr::GetSkinInfo() const
		{
			return currentSkin_.get();
		}

		void SkinMgr::SetSkinInfo(const stdex::tString &skinFile)
		{
			currentSkin_->Load(skinFile);
		}

		utility::BITMAPPtr SkinMgr::GetSkin(const stdex::tString &name)
		{
			if( !loader_ )
			{
				loader_ = GetSkinLoader();
			}

			Skins::const_iterator iter = skins_.find(name);
			if( iter == skins_.end() )
			{	
				utility::BITMAPPtr bmp = loader_->Load(skinsPath_ + name);
				assert(bmp != 0);

				skins_.insert(std::make_pair(name, bmp));

				return bmp;
			}
			else
				return iter->second;
		}

		utility::ICONPtr SkinMgr::GetIcon(long gid)
		{
			Icons::const_iterator iter = icons_.find(gid);
			if( iter == icons_.end() )
			{
				utility::ICONPtr icon = loader_->Load(gid);
				assert(icon != 0);

				icons_.insert(std::make_pair(gid, icon));
				return icon;
			}
			else
				return iter->second;
		}


		SkinMgr &SkinMgrInstance()
		{
			static SkinMgr skinMgr;
			return skinMgr;
		}
	}
}
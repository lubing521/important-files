#include "stdafx.h"
#include "PlugToolBusiness.h"

#include "../../../../include/Utility/utility.h"
#include "../../../../include/ui/ImageHelpers.h"
#include "../../../../include/UI/IconHelper.h"

#include "../ui/Skin/SkinMgr.h"
#include "../ManagerInstance.h"


namespace i8desk
{
	namespace business
	{
		namespace plug
		{
			const utility::ICONPtr &GetBitmap(const data_helper::PlugToolTraits::ValueType &plug)
			{
				if( plug->Icon == 0 )
				{
					unsigned char data[1024 * 10] = {0};
					size_t size = 0;
					utility::BITMAPPtr &tmp = i8desk::ui::SkinMgrInstance().GetSkin(_T("PlugTool/Default.png"));
					if( !i8desk::GetControlMgr().GetPlugIcon(plug->PID, data, size) )
					{
						plug->Icon = Bitmap2Icon(tmp);
					}
					else
					{
						plug->Icon = LoadIconFromBuffer(data, size);		
					}
				}

				return plug->Icon;
			}


			void SetNameBmp(data_helper::PlugToolTraits::VectorType &plugs)
			{
				for(size_t i = 0; i != plugs.size(); ++i)
				{
					plugs[i]->Icon = GetBitmap(plugs[i]);
				}
			}

			void SetStatusBmp(data_helper::PlugToolTraits::VectorType &plugs)
			{
				for(size_t i = 0; i != plugs.size(); ++i)
				{
					plugs[i]->StatusBmp[0] = i8desk::ui::SkinMgrInstance().GetSkin(_T("PlugTool/Download.png"));
					plugs[i]->StatusBmp[1] = i8desk::ui::SkinMgrInstance().GetSkin(_T("PlugTool/UnDownload.png"));
					plugs[i]->StatusBmp[2] = i8desk::ui::SkinMgrInstance().GetSkin(_T("PlugTool/Enable.png"));
					plugs[i]->StatusBmp[3] = i8desk::ui::SkinMgrInstance().GetSkin(_T("PlugTool/Disable.png"));
				}
			}

			void SetRateBmp(data_helper::PlugToolTraits::VectorType &plugs)
			{
				for(size_t i = 0; i != plugs.size(); ++i)
				{
					plugs[i]->rateBmp = i8desk::ui::SkinMgrInstance().GetSkin(_T("PlugTool/Rate.png"));
				}
			}


			void GetProgress(std::map<long, std::pair<size_t, size_t>> &progress)
			{
				using namespace i8desk::data_helper;
				PlugToolStatusTraits::VectorType pluStatus = i8desk::GetRealDataMgr().GetPlugToolStatus();
				const PlugToolTraits::VectorType &plugTools = i8desk::GetDataMgr().GetPlugTools();

				struct FindPlug
				{
					long PID_;
					FindPlug(long PID)
						: PID_(PID)
					{}

					bool operator()(const PlugToolTraits::ValueType &rhs) const
					{ return rhs->PID == PID_; }
				};

				for(size_t i = 0; i != pluStatus.size(); ++i)
				{
					PlugToolTraits::VecConstIterator iter = std::find_if(plugTools.begin(), plugTools.end(), FindPlug(pluStatus[i]->PID));
					size_t row = std::distance(plugTools.begin(), iter);
					
					progress[(*iter)->PID] = (std::make_pair(row, (size_t)(pluStatus[i]->ReadSize * 100.0 / pluStatus[i]->Size)));
				}
			}
		}
	}
}
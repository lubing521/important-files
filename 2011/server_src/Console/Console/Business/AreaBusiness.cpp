#include "stdafx.h"
#include "AreaBusiness.h"

#include <functional>

#include "VDiskBusiness.h"

#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../../../../include/Utility/utility.h"
#include "../ManagerInstance.h"
#include "../../../../include/I8Type.hpp"

namespace i8desk
{
	namespace business
	{

		namespace Area
		{
			const stdex::tString &Area::GetAIDByName(const stdex::tString &name)
			{
				using data_helper::AreaTraits;
				const AreaTraits::MapType &areas = GetDataMgr().GetAreas();

				// bug????
				/*AreaTraits::MapConstIterator iter = std::find_if(areas.begin(), areas.end(), 
				bind(&AreaTraits::MapType::value_type::first, std::tr1::placeholders::_1) == name);*/

				struct FindIF
				{
					const stdex::tString &name_;
					FindIF(const stdex::tString &name)
						: name_(name)
					{}

					bool operator()(const AreaTraits::MapType::value_type &val) const
					{ return val.second->Name == name_; }
				};
				AreaTraits::MapConstIterator iter = std::find_if(areas.begin(), areas.end(), FindIF(name));

				if( iter != areas.end() )
					return iter->first;
				else
				{
					static const stdex::tString &tmp = _T("");
					return tmp;
				}
			}

			bool IsDefaultArea(const data_helper::AreaTraits::ValueType &val)
			{
				if( utility::Strcmp(val->AID, DEFAULT_AREA_GUID) == 0 )
					return true;
				else
					return false;
			}

			void EnsureDefaultArea()
			{
				using i8desk::data_helper::AreaTraits;
				AreaTraits::MapType &areas = i8desk::GetDataMgr().GetAreas();
				if( areas.find(DEFAULT_AREA_GUID) == areas.end() )
				{
					AreaTraits::ValueType defaultArea(new AreaTraits::ElementType);
					utility::Strcpy(defaultArea->AID, DEFAULT_AREA_GUID);
					utility::Strcpy(defaultArea->Name, DEFAULT_AREA_NAME);

					i8desk::GetDataMgr().AddData(areas, defaultArea->AID, defaultArea);
				}
			}


			bool Area::ParseArea(const stdex::tString &area, data_helper::RunTypeTraits::ValueType &runType)
			{
				using namespace data_helper;
				runType->Type = 0;

				if( area == RT_UNKNOWN )
				{
					runType->Type = i8desk::ERT_NONE;
					return true;
				}
				else if( area == RT_LOCAL ) 
				{
					runType->Type = i8desk::ERT_LOCAL;
					return true;
				}
				else if( area == RT_DIRECT ) 
				{
					runType->Type = i8desk::ERT_DIRECT;
					return true;
				}
				else if( area.substr(0, RT_VDISK.length()) == RT_VDISK 
					&& area.length() > RT_VDISK.length() ) 
				{
					runType->Type = i8desk::ERT_VDISK;
					TCHAR cliDrv = area[RT_VDISK.length()];

					const VDiskTraits::MapType &areas = GetDataMgr().GetVDisks();

					for(VDiskTraits::MapConstIterator iter = areas.begin(); 
						iter != areas.end(); ++iter) 
					{
						if( iter->second->CliDrv == cliDrv ) 
						{
							utility::Strcpy(runType->VID, iter->second->VID);
							return true;
						}
					}
				}

				return false;
			}


			stdex::tString ParseRunType(const stdex::tString &AID, const stdex::tString &areaRunType)
			{
				using namespace i8desk::data_helper;

				std::vector<stdex::tString> areaTypes;
				stdex::Split(areaTypes, areaRunType, _T('|'));

				for(size_t i = 0; i != areaTypes.size(); ++i)
				{
					const stdex::tString &dest = areaTypes[i];
					if( dest.empty() )
						break;

					stdex::tString szAid = stdex::Split(dest, _T(','), 0);
					stdex::tString szRunType = stdex::Split(dest, _T(','), 1);

					if( szAid != AID )
						continue;

					if( szRunType == _T("0") )
						return RT_UNKNOWN;
					else if ( szRunType == _T("1") )
						return RT_LOCAL;
					else if ( szRunType == _T("3") )
						return RT_DIRECT;
					else
					{
						const VDiskTraits::MapType &vDisks = GetDataMgr().GetVDisks();
						VDiskTraits::MapConstIterator vit = vDisks.find(szRunType);
						if( vit != vDisks.end() ) 
						{
							stdex::tString desc;
							VDisk::RunTypeDesc(vit->second, desc);
							return desc;
						}
						else
							return RT_UNKNOWN;
					}
				}

				return data_helper::RT_UNKNOWN;
			}

			stdex::tString ParseRunType(const stdex::tString &AID, const data_helper::RunTypeTraits::VectorType &runTypes)
			{
				using namespace data_helper;

				for(RunTypeTraits::VecConstIterator iter = runTypes.begin();
					iter != runTypes.end(); ++iter)
				{
					if( (*iter)->AID != AID )
						continue;

					switch((*iter)->Type)
					{
					case i8desk::ERT_NONE:
						return RT_UNKNOWN;
						break;
					case i8desk::ERT_LOCAL:
						return RT_LOCAL;
						break;
					case i8desk::ERT_VDISK:
						{
							for(VDiskTraits::MapConstIterator vIter = GetDataMgr().GetVDisks().begin();
								vIter != GetDataMgr().GetVDisks().end(); ++vIter)
							{
								if( utility::Strcmp((*iter)->VID, vIter->second->VID) == 0 )
								{
									stdex::tString msg;
									VDisk::RunTypeDesc(vIter->second, msg);

									return msg;
								}
							}
						}
						break;
					case i8desk::ERT_DIRECT:
						return RT_DIRECT;
						break;
					default:
						assert(0);
						break;
					}
				}

				return RT_UNKNOWN;
			}

			bool HasSameName(const data_helper::AreaTraits::ValueType &val)
			{
				using i8desk::data_helper::AreaTraits;
				const AreaTraits::MapType &areas = i8desk::GetDataMgr().GetAreas();

				for(AreaTraits::MapConstIterator iter = areas.begin(); iter != areas.end(); ++iter)
				{
					if( utility::Strcmp(iter->second->AID, val->AID) == 0 )
						continue;
					if( utility::Strcmp(iter->second->Name, val->Name) == 0 )
						return true;
				}

				return false;
			}

		}

	}
}
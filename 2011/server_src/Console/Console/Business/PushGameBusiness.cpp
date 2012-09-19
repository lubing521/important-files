#include "stdafx.h"
#include "PushGameBusiness.h"


#include "../../../../include/Utility/utility.h"
#include "../../../../include/Extend STL/StringAlgorithm.h"
#include <set>


namespace i8desk
{
	namespace business
	{
		namespace PushGame
		{
			namespace detail
			{
				struct Filter
				{
					typedef std::tr1::function<bool (const data_helper::GameTraits::ElementType &)> Callback;

					data_helper::GameTraits::VectorType &games_;
					Callback callback_;
					Filter(data_helper::GameTraits::VectorType &games, const Callback &callback)
						: games_(games)
						, callback_(callback)
					{}
					void operator()(const data_helper::GameTraits::MapType::value_type &val) const
					{ 
						if( callback_(*(val.second)) )
							games_.push_back(val.second);
					}
				};

				struct Func
				{
					Func(const stdex::tString &TID, data_helper::BootTaskAreaTraits::VectorType &taskareas)
						: taskareas_(taskareas)
						, TID_(TID)
					{}
					void operator ()(const data_helper::BootTaskAreaTraits::MapType::value_type &val ) const
					{
						if( TID_ == val.second->TID )
							taskareas_.push_back(val.second);
					}
					const stdex::tString &TID_;
					data_helper::BootTaskAreaTraits::VectorType &taskareas_;
				};
			}

			void LocalAllFilter(data_helper::GameTraits::VectorType &curGames)
			{
				// if( val.second->Status  == 1 )
				using namespace std::tr1::placeholders;
				using i8desk::data_helper::RunTypeTraits;
				RunTypeTraits::MapType &runtypes = i8desk::GetDataMgr().GetRunTypes();
				RunTypeTraits::VectorType curRunTypes;
				for(RunTypeTraits::MapConstIterator iter = runtypes.begin(); iter != runtypes.end(); ++iter)
				{
					if( iter->second->Type == i8desk::ERT_LOCAL )
						curRunTypes.push_back(iter->second);
				}

				
				using i8desk::data_helper::GameTraits;
				GameTraits::MapType &games = i8desk::GetDataMgr().GetGames();
				std::set<uint32> SetGame;

				for(RunTypeTraits::VecIterator veciter = curRunTypes.begin(); veciter != curRunTypes.end(); ++veciter)
				{
					GameTraits::MapIterator mapiter = games.find((*veciter)->GID);
					if( mapiter == games.end() )
						continue;

					if( mapiter->second->Status == i8desk::StatusLocal )
						SetGame.insert(mapiter->second->GID);
				}

				for(std::set<uint32>::const_iterator setiter = SetGame.begin(); setiter != SetGame.end(); ++setiter  )
				{
					GameTraits::MapIterator mapiter = games.find(*setiter);

					if( mapiter == games.end() )
						continue;

					curGames.push_back(mapiter->second);

				}

			}

			void ClassFilter(data_helper::GameTraits::VectorType &curGames, data_helper::ClassTraits::MapType &curClass)
			{
				using namespace std::tr1::placeholders;
				using i8desk::data_helper::ClassTraits;
				ClassTraits::MapType &classes = i8desk::GetDataMgr().GetClasses();
				for(data_helper::GameTraits::VecIterator giter = curGames.begin(); giter != curGames.end(); ++giter)
				{
					for(ClassTraits::MapIterator iter = classes.begin(); iter != classes.end(); ++iter)
					{
						if( utility::Strcmp((*giter)->CID,  iter->second->CID ) == 0)
							curClass.insert(*iter);
					}
				}
			}

			void TaskAreaFilter(stdex::tString TID, data_helper::BootTaskAreaTraits::VectorType &curTaskareas)
			{
				data_helper::BootTaskAreaTraits::MapType &taskareas = i8desk::GetDataMgr().GetBootTaskAreas();

				std::for_each(taskareas.begin(), taskareas.end(), detail::Func(TID, curTaskareas));
			}


			bool HasSameName(const data_helper::PushGameTraits::ValueType &val)
			{
				using i8desk::data_helper::PushGameTraits;
				const PushGameTraits::MapType &pushgames = i8desk::GetDataMgr().GetPushGames();

				for(PushGameTraits::MapConstIterator iter = pushgames.begin(); iter != pushgames.end(); ++iter)
				{
					if( utility::Strcmp(iter->second->TID,val->TID) == 0)
						continue;
					if( utility::Strcmp(iter->second->Name, val->Name) == 0 )
						return true;
				}

				return false;
			}

			stdex::tString GetAreaDesc(const data_helper::PushGameTraits::ValueType &val)
			{
				const data_helper::AreaTraits::MapType &areas = GetDataMgr().GetAreas();
				const data_helper::BootTaskAreaTraits::MapType &bootAreas = GetDataMgr().GetBootTaskAreas();

				stdex::tString areaDesc;
				for(data_helper::BootTaskAreaTraits::MapConstIterator iter = bootAreas.begin();
					iter != bootAreas.end(); ++iter)
				{
					if( utility::Strcmp(val->TID, iter->second->TID) != 0 )
						continue;

					data_helper::AreaTraits::MapConstIterator areaIter = areas.find(iter->second->AID);
					if( areaIter != areas.end() )
					{
						areaDesc += areaIter->second->Name;
						areaDesc += _T("|");
					}
				}

				if( !areaDesc.empty() )
					areaDesc.erase(areaDesc.begin() + areaDesc.find_last_of('|'), areaDesc.end());

				return areaDesc;
			}


			static const struct
			{
				int checkType_;
				const stdex::tString checkTypeDesc_;
			} CheckType[] = 
			{
				{ FC_COPY_COMPARE,		_T("对比压缩") },
				{ FC_COPY_QUICK,		_T("对比文件")},
				{ FC_COPY_FORE,			_T("不对比 全部更新")},
				{ FC_COPY_VERSION,		_T("对比索引版本号")	}
			};

			stdex::tString GetCheckDesc(const size_t checktype )
			{
				for(size_t i = 0; i != _countof(CheckType); ++i)
				{
					if( CheckType[i].checkType_ == checktype )
						return CheckType[i].checkTypeDesc_;
				}

				//assert(0);
				return _T("");
			}

			stdex::tString GetParameterDesc(const data_helper::PushGameTraits::ValueType &val, int type )
			{
				stdex::tString ParameteDesc = _T("");
				stdex::tString desc = val->Parameter;
				
				switch( type )
				{
				case fst_maxspeed:
					ParameteDesc = stdex::Split(desc, _T('|'), 0);
					break;
				case scd_cheecktype:
					{
						size_t checktype = 0;
						stdex::ToNumber(checktype, stdex::Split(desc, _T('|'), 1)) ;
						ParameteDesc = GetCheckDesc(checktype);
					}
					break;
				case trd_gid:
					ParameteDesc = stdex::Split(desc, _T('|'), 2);
					break;
				default:
					assert(0);
					break;
				}

				return ParameteDesc;

			}


			
		}
	}
}
#include "stdafx.h"
#include "GameBusiness.h"

#include <list>
#include <set>
#include <numeric>
#include <regex>
#include <atlpath.h>

#include "VDiskBusiness.h"
#include "ClassBusiness.h"

#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../../../../include/Extend STL/StdEx.h"
#include "../../../../include/Utility/utility.h"
#include "../../../../include/ui/ImageHelpers.h"
#include "../../../../include/UI/IconHelper.h"
#include "../../../../include/I8Type.hpp"
#include "../../../../include/Utility/ChineseToLetter.h"

#include "../ManagerInstance.h"
#include "../ui/Skin/SkinMgr.h"


namespace i8desk
{
	namespace business
	{

		namespace Game
		{
			namespace detail
			{
				struct FindRunType
				{
					const data_helper::RunTypeTraits::ValueType &runType_;
					FindRunType(const data_helper::RunTypeTraits::ValueType &runType)
						: runType_(runType)
					{}

					bool operator()(const data_helper::RunTypeTraits::ValueType &val) const
					{
						return utility::Strcmp(val->AID, runType_->AID) == 0;
					}
				};
			}


			void HandlePath(data_helper::GameTraits::ValueType &val)
			{
				// 如果SvrPath为空
				if( utility::IsEmpty(val->SvrPath) )
				{
					using data_helper::ClassTraits;
					const ClassTraits::MapType &classes = GetDataMgr().GetClasses();

					// 获取类别路径
					for(ClassTraits::MapConstIterator iter = classes.begin(); 
						iter != classes.end(); ++iter)
					{
						if( iter->first != val->CID )
							continue;

						// 类别路径为空
						stdex::tString path;
						::PathAddBackslash(iter->second->SvrPath);
						if( utility::IsEmpty(iter->second->SvrPath) )
						{
							path = GetDataMgr().GetOptVal(OPT_D_INITDIR, _T(""));
							if( !path.empty() )
							{
								ATL::CPath tmp(path.c_str());
								tmp.AddBackslash();
								path = tmp;
								path = path + iter->second->Name + _T("\\") + val->Name + _T("\\");
							}
						}
						else	// 非空
						{
							path = iter->second->SvrPath;
							path = path + val->Name + _T("\\");
						}

						utility::Strcpy(val->SvrPath, path);
						break;
					}
				}

				// 如果客户端路径为空
				if( utility::IsEmpty(val->CliPath) )
				{
					utility::Strcpy(val->CliPath, val->SvrPath);

					stdex::tString drv = GetDataMgr().GetOptVal(OPT_M_GAMEDRV, _T("E"));
					if( !drv.empty() )
						val->CliPath[0] = drv[0];
				}
			}


			void HandleRunTypeDesc(const data_helper::RunTypeTraits::VectorType &runTpes, stdex::tString &desc)
			{
				typedef std::set<stdex::tString> Description;
				Description tmp;
				using namespace i8desk::data_helper;

				for(RunTypeTraits::VecConstIterator iter = runTpes.begin(); 
					iter != runTpes.end(); ++iter)
				{
					if( (*iter)->Type == i8desk::ERT_LOCAL ) 
						tmp.insert(RT_LOCAL);
					else if( (*iter)->Type == i8desk::ERT_DIRECT )
						tmp.insert(RT_DIRECT);
					else if( (*iter)->Type == i8desk::ERT_NONE )
						tmp.insert(RT_UNKNOWN);
					else 
					{
						const VDiskTraits::MapType &vDisks = GetDataMgr().GetVDisks();
						VDiskTraits::MapConstIterator vit = vDisks.find((*iter)->VID);
						if( vit != vDisks.end() ) 
						{
							stdex::tOstringstream os;
							os << RT_VDISK << (TCHAR)vit->second->CliDrv;
							tmp.insert(os.str());
						}
					}
				}

				if( tmp.empty() )
					tmp.insert(RT_UNKNOWN);

				desc.clear();
				for(Description::const_iterator it = tmp.begin();
					it != tmp.end(); ++it)
				{
					desc += it->c_str();
					desc += _T("/");
				}
				desc.erase(desc.begin() + desc.find_last_of('/'), desc.end());
			}


			bool CouldDownload(const data_helper::GameTraits::ValueType &val)
			{
				// 不需要下载或更新
				if( val->GID < MIN_IDC_GID )
					return false;
				
				if( val->Status == StatusLocal && val->SvrVer == val->IdcVer )
					return false;

				return true;
			}

			void Delete(const data_helper::GameTraits::ValueType &val)
			{
				// 如果为手动添加游戏则删除,否则设置为不可见
				if( val->GID < MIN_IDC_GID )
					GetDataMgr().DelData(GetDataMgr().GetGames(), val->GID);
				else
				{
					val->Status = 0;
					val->SvrVer = 0;
					val->StatusDesc.clear();
					//val->Icon.Type().reset();
				}
			}
			
			bool SetRunType(const data_helper::GameTraits::ValueType &game, const data_helper::RunTypeTraits::VectorType &runType)
			{
				if( runType.empty() )
					return true;

				using data_helper::RunTypeTraits;
				RunTypeTraits::MapType &runTypes = i8desk::GetDataMgr().GetRunTypes();

				game->Runtypes = runType;
				for(RunTypeTraits::VecConstIterator iter = game->Runtypes.begin();
					iter != game->Runtypes.end(); ++iter)
				{
					const RunTypeTraits::ValueType &val = *iter;
					val->GID = game->GID;

					
					if( !GetDataMgr().EnsureData(runTypes, RunTypeTraits::KeyType(val->AID, val->GID),
						val, MASK_TRUNTYPE_TYPE | MASK_TRUNTYPE_VID) )
						return false;
				}

				business::Game::HandleRunTypeDesc(game->Runtypes, game->RuntypeDesc);
				return true;
			}


			void AddRunType(const stdex::tString &AID)
			{
				data_helper::GameTraits::MapType &games = GetDataMgr().GetGames();
				data_helper::RunTypeTraits::ValueType runType(new data_helper::RunTypeTraits::ElementType);
				utility::Strcpy(runType->AID, AID);

				for(data_helper::GameTraits::MapConstIterator iter = games.begin();
					iter != games.end(); ++iter)
				{
					data_helper::RunTypeTraits::VectorType &runTypes = iter->second->Runtypes;

					if( std::find_if(runTypes.begin(), runTypes.end(), detail::FindRunType(runType)) != runTypes.end() )
					{
						//runTypes
					}
					else
						runTypes.push_back(runType);

					SetRunType(iter->second, runTypes);
				}

			}

			void EraseRunType(const stdex::tString &AID)
			{
				data_helper::GameTraits::MapType &games = GetDataMgr().GetGames();
				data_helper::RunTypeTraits::ValueType runType(new data_helper::RunTypeTraits::ElementType);
				utility::Strcpy(runType->AID, AID);

				for(data_helper::GameTraits::MapConstIterator iter = games.begin();
					iter != games.end(); ++iter)
				{
					data_helper::RunTypeTraits::VectorType &runTypes = iter->second->Runtypes;

					data_helper::RunTypeTraits::VecIterator RunIter = std::remove_if(runTypes.begin(), runTypes.end(), detail::FindRunType(runType));
					if( RunIter != runTypes.end() )
						runTypes.erase(RunIter);

					SetRunType(iter->second, runTypes);
				}
			}

			bool SetIcon(const data_helper::GameTraits::ValueType &game, const utility::ICONPtr &icon)
			{
				assert(game->GID != 0);
				unsigned char iconData[1024 * 10] = {0};
				size_t size = 0;
				SaveIconToBuffer(icon, iconData, size);

				if( !i8desk::GetControlMgr().AddIcon(game->GID, iconData, size) )
					return false;

				game->Icon = icon;

				return true;
			}

			
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
						if( callback_(std::tr1::cref(*(val.second))) )
							games_.push_back(val.second);
					}
				};

				struct CenterFilter
				{
					typedef std::tr1::function<bool (const data_helper::GameTraits::ElementType &)> Callback;

					data_helper::GameTraits::MapType &games_;
					Callback callback_;
					CenterFilter(data_helper::GameTraits::MapType &games, const Callback &callback)
						: games_(games)
						, callback_(callback)
					{}
					void operator()(const data_helper::GameTraits::MapType::value_type &val) const
					{ 
						if( callback_(*(val.second)) )
							games_.insert(val) ;
					}
				};
			}
			
			void CenterFilter(const data_helper::GameTraits::MapType &games, data_helper::GameTraits::VectorType &curGames)
			{
				// Gid > 10000 
				using namespace std::tr1::placeholders;
				std::for_each(games.begin(), games.end(), [&curGames](const data_helper::GameTraits::MapType::value_type &val)
				{
					if( val.second->GID > MIN_IDC_GID )
						curGames.push_back(val.second);
				});
			}

			void CenterI8PlayFilter(const data_helper::GameTraits::MapType &games, data_helper::GameTraits::VectorType &curGames)
			{
				// Gid > 10000 && I8Play >= 1
				for(data_helper::GameTraits::MapConstIterator iter = games.begin();
					iter != games.end(); ++iter)
				{
					if( iter->second->GID > MIN_IDC_GID &&
						iter->second->I8Play >= 1 )
						curGames.push_back(iter->second);
				}

				/*using namespace std::tr1::placeholders;
				std::for_each(games.begin(), games.end(), detail::Filter(curGames, 
					std::tr1::bind(std::greater_equal<int>(),
						std::tr1::bind(&data_helper::GameTraits::ElementType::I8Play, _1), 1)));*/
			}

			void NoDownloadFilter(const data_helper::GameTraits::MapType &games, data_helper::GameTraits::VectorType &curGames)
			{
				// Gid > 10000 && !val.second->Status
				/*using namespace std::tr1::placeholders;
				std::for_each(games.begin(), games.end(), detail::Filter(curGames, 
					stdex::Compose2(std::logical_and<bool>(), 
					std::tr1::bind(std::not_equal_to<int>(),std::tr1::bind(&data_helper::GameTraits::ElementType::Status, _1), StatusLocal),
					std::tr1::bind(std::greater<uint32>(),std::tr1::bind(&data_helper::GameTraits::ElementType::GID, _1), MIN_IDC_GID))));*/

				for(data_helper::GameTraits::MapConstIterator iter = games.begin();
					iter != games.end(); ++iter)
				{
					if( iter->second->GID > MIN_IDC_GID &&
						iter->second->Status != StatusLocal )
						curGames.push_back(iter->second);
				}
			}

			void RecentFilter(const data_helper::GameTraits::MapType &games, data_helper::GameTraits::VectorType &curGames)
			{
				// 30天算近期
				CTime now = CTime::GetCurrentTime();
				CTime start(now.GetYear(), now.GetMonth(), now.GetDay(), 0, 0, 0);
				start -= CTimeSpan(30,0,0,0);
				ulong startTime = static_cast<DWORD>(start.GetTime());

				// id > 10000 &&  val.second->IdcAddDate > startTime_
				for(data_helper::GameTraits::MapConstIterator iter = games.begin();
					iter != games.end(); ++iter)
				{
					if( iter->second->GID > MIN_IDC_GID &&
						iter->second->IdcAddDate > startTime )
						curGames.push_back(iter->second);
				}

				/*using namespace std::tr1::placeholders;
				std::for_each(games.begin(), games.end(), detail::Filter(curGames, 
					std::tr1::bind(std::greater<ulong>(),
					std::tr1::bind(&data_helper::GameTraits::ElementType::IdcAddDate, _1), startTime)));*/
			}

			void DeleteFilter( data_helper::GameTraits::VectorType &curGames )
			{
				data_helper::GameTraits::MapType &games = i8desk::GetDataMgr().GetGames();
			
				// StopRun >= DelLock
				using namespace std::tr1::placeholders;
				std::for_each(games.begin(), games.end(), [&curGames](const data_helper::GameTraits::MapType::value_type &val)
				{
					if( val.second->StopRun >= DelLock )
						curGames.push_back(val.second);
				});
			}

			void UpdateFilter(const data_helper::GameTraits::MapType &games, data_helper::GameTraits::VectorType &curGames)
			{
				// 手动更新方式游戏，不显示在“有更新”列表中
				// if( id > 10000 && 
				//		val.second->Status == 1 && 
				//		val.second->SvrVer != val.second->IdcVer &&
				//		val.second->AutoUpt == 1 )

				for(data_helper::GameTraits::MapConstIterator iter = games.begin(); iter != games.end(); ++iter)
				{
					if( iter->second->Status == StatusLocal && 
						iter->second->SvrVer != iter->second->IdcVer &&
						iter->second->AutoUpt == 1 )
					{
						curGames.push_back(iter->second);
					}
				}

				using namespace std::tr1::placeholders;

				// MSVC tr1 bind bug??, so Compose2

				/*std::for_each(games.begin(), games.end(), detail::Filter(curGames, 
					stdex::Compose2(std::logical_and<bool>(),
						stdex::Compose2(std::logical_and<bool>(), 
							std::tr1::bind(std::equal_to<int>(), std::tr1::bind(&data_helper::GameTraits::ElementType::Status, _1), StatusLocal), 
							std::tr1::bind(std::equal_to<int>(), std::tr1::bind(&data_helper::GameTraits::ElementType::AutoUpt, _1), 1)),
							std::tr1::bind(std::not_equal_to<ulong>(), 
								std::tr1::bind(&data_helper::GameTraits::ElementType::SvrVer, _1), 
								std::tr1::bind(&data_helper::GameTraits::ElementType::IdcVer, _1)))));*/
			}

			void LocalAllFilter(const data_helper::GameTraits::MapType &games, data_helper::GameTraits::VectorType &curGames)
			{
				// if( val.second->Status  == 1 )
				using namespace std::tr1::placeholders;
				std::for_each(games.begin(), games.end(), [&curGames](const data_helper::GameTraits::MapType::value_type &val)
				{
					if( val.second->Status == StatusLocal )
						curGames.push_back(val.second);
				});
			}


			void LocalFilter(const data_helper::GameTraits::MapType &games, data_helper::GameTraits::VectorType &curGames, const stdex::tString &CID)
			{
				// if( val.second->Status && CID_ == val.second->CID )
				using namespace std::tr1::placeholders;
				std::for_each(games.begin(), games.end(), [&curGames, &CID](const data_helper::GameTraits::MapType::value_type &val)
				{
					if( val.second->Status == StatusLocal &&
						CID == val.second->CID )
						curGames.push_back(val.second);
				});
				
			}


			void ClassFilter(const data_helper::GameTraits::VectorType &games, data_helper::GameTraits::VectorType &filterGames, const stdex::tString &CID)
			{
				struct ClassFilterFunc
				{
					data_helper::GameTraits::VectorType &games_;
					const stdex::tString &CID_;
					ClassFilterFunc(data_helper::GameTraits::VectorType &games, const stdex::tString &CID)
						: games_(games)
						, CID_(CID)
					{}
					void operator()(const data_helper::GameTraits::MapType::value_type &val) const
					{ 
						if( val.second->CID == CID_ )
							games_.push_back(val.second);
					}
					void operator()(const data_helper::GameTraits::VectorType::value_type &val) const
					{ 
						if( val->CID == CID_ )
							games_.push_back(val);
					}
				};
				std::for_each(games.begin(), games.end(), ClassFilterFunc(filterGames, CID));
			}

			unsigned long GetCustomGID()
			{
				// 保证GID不重复
				const i8desk::data_helper::GameTraits::MapType &games = i8desk::GetDataMgr().GetGames();

				static std::set<unsigned long> customGID;

				for(size_t i = 1; i != MIN_IDC_GID; ++i)
				{
					if( customGID.find(i) == customGID.end() &&
						games.find(i) == games.end() )
					{
						customGID.insert(i);
						return i;
					}
				}

				return 0;
			}

			LPCTSTR GePriorityDesc(DWORD priority)
			{
				using namespace i8desk::data_helper;

				switch(priority) 
				{
				case i8desk::Lowest:
					return _T("最低");
				case i8desk::Lower:
					return _T("较低");
				case i8desk::Normal:
					return _T("一般");
				case i8desk::Higher:
					return _T("较高");
				case i8desk::Highest:
					return _T("最高");
				default:
					return _T("无效");
				}
			}

			LPCTSTR GetStatusDesc(const data_helper::GameTraits::ValueType &game)
			{
				if( game->Status == StatusLocal )
				{
					if( game->GID < MIN_IDC_GID )
						game->StatusDesc = _T("己最新");
					else if( game->SvrVer == game->IdcVer )
						game->StatusDesc = _T("己最新");
					else
						game->StatusDesc = _T("有更新");
				}
				else
				{
					if( game->StatusDesc.empty() )
						return _T("未下载");
				}

				return game->StatusDesc.c_str();
			}

			void GetUpdateDesc(const std::tr1::function<int(LPCTSTR)> &callback)
			{
				LPCTSTR updateDesc[] = 
				{
					_T("自动更新"),
					_T("手动更新")
				};

				std::for_each(updateDesc, updateDesc + _countof(updateDesc), 
					std::tr1::bind(callback, std::tr1::placeholders::_1));
			}

			void SetUpdate(const data_helper::GameTraits::ValueType &game, const stdex::tString &updateDesc)
			{
				LPCTSTR desc[] = 
				{
					_T("自动更新"),
					_T("手动更新")
				};

				if( updateDesc == desc[0] )
					game->AutoUpt = 1;
				else
					game->AutoUpt = 0;

				i8desk::GetDataMgr().EnsureData(i8desk::GetDataMgr().GetGames(), game->GID, game, MASK_TGAME_AUTOUPT);
			}

			void GetPriorityDesc(const std::tr1::function<int(LPCTSTR)> &callback)
			{
				LPCTSTR priorityDesc[] = 
				{
					_T("最低"),
					_T("较低"),
					_T("一般"),
					_T("较高"),
					_T("最高")	
				};

				std::for_each(priorityDesc, priorityDesc + _countof(priorityDesc), 
					std::tr1::bind(callback, std::tr1::placeholders::_1));
			}

			void SetStatusDesc(const data_helper::GameTraits::ValueType &game, const stdex::tString &desc)
			{
				game->StatusDesc = desc;
			}

			const stdex::tString &GetRunTypeDesc(const data_helper::GameTraits::ValueType &game)
			{
				if( game->RuntypeDesc.empty() )
				{
					using i8desk::data_helper::RunTypeTraits;

					const RunTypeTraits::MapType &runTypes = GetDataMgr().GetRunTypes();
					RunTypeTraits::VectorType curRunTypes;
					for(RunTypeTraits::MapConstIterator iter = runTypes.begin(); iter != runTypes.end(); ++iter)
					{
						if( iter->second->GID == game->GID )
							curRunTypes.push_back(iter->second);
					}

					game->Runtypes = curRunTypes;
					HandleRunTypeDesc(game->Runtypes, game->RuntypeDesc);
				}
				
				return game->RuntypeDesc;
			}

			const utility::ICONPtr &GetIcon(const data_helper::GameTraits::ValueType &game)
			{

				if( game->Icon == 0 )
					game->Icon = i8desk::ui::SkinMgrInstance().GetIcon(game->GID);

				return game->Icon;
			}


			bool CheckByName(const stdex::tString &name, i8desk::data_helper::GameTraits::ValueType &findGame)
			{
				using i8desk::data_helper::GameTraits;
				GameTraits::MapType &games = i8desk::GetDataMgr().GetGames();

				for(GameTraits::MapConstIterator iter = games.begin(); iter != games.end(); ++iter)
				{
					if( name == iter->second->Name )
					{
						findGame = iter->second;
						return true;
					}
				}

				return false;
			}


			void FindGame(LPCTSTR destText, const data_helper::GameTraits::VectorType &games, data_helper::GameTraits::VectorType &values)
			{
				stdex::tString text(destText);
				stdex::ToLower(text);
				
				typedef std::set<data_helper::GameTraits::ValueType> GamesTmp;
				GamesTmp gamesTmp;

				
				// 试图匹配名称
				stdex::tString gameName;
				stdex::tString PYRes;

				for(size_t i = 0; i != games.size(); ++i)
				{
					gameName = games[i]->Name;
					stdex::ToLower(gameName);

					//i8desk::GetPY(gameName, PYRes);
					PYRes = i8desk::GetPyLetter(gameName);

					if( gameName.find(text) != stdex::tString::npos || 
						PYRes.find(text) != stdex::tString::npos )
					{
						gamesTmp.insert(games[i]);
					}
				}
                                                                                                                                                                                                                       
				// 试图匹配GID
				for(size_t i = 0; i != games.size(); ++i)
				{
					if( games[i]->GIDDesc.empty() )
						stdex::ToString(games[i]->GID, games[i]->GIDDesc);

					if( games[i]->GIDDesc.find(text) != stdex::tString::npos )
					{
						gamesTmp.insert(games[i]);
					}
				}
				
				for(GamesTmp::const_iterator iter = gamesTmp.begin(); iter != gamesTmp.end(); ++iter)
					values.push_back(*iter);
			}


			bool TaskOperate(long gid, TaskOperation op)
			{
				data_helper::GameTraits::MapType &games = GetDataMgr().GetGames();
				data_helper::GameTraits::MapType::const_iterator iter = games.find(gid);
				if( iter == games.end() )
					return false;

				i8desk::business::Game::SetStatusDesc(iter->second, _T(""));
				return i8desk::GetTaskMgr().TaskOperate(gid, op);
			}


			bool LocalTaskOperate(const data_helper::GameTraits::ValueType &game, TaskOperation op, bool isForce)
			{
				// 本地游戏不需要三层更新
				if( game->GID < MIN_IDC_GID )
					return false;
				
				return i8desk::GetTaskMgr().TaskOperate(game->GID, op, game->Priority, game->IdcVer, isForce, game->IdcClick);
			}

			void GetLocalDrv(std::set<TCHAR> &drvs)
			{
				using data_helper::GameTraits;
				const GameTraits::MapType &games = GetDataMgr().GetGames();
				for(GameTraits::MapConstIterator iter = games.begin(); iter != games.end(); ++iter)
				{
					if( iter->second->Status == data_helper::StatusLocal )
						drvs.insert(iter->second->SvrPath[0]);
				}

				// 如果为空则添加默认下载路径
				if( drvs.empty() )
				{
					stdex::tString dir = GetDataMgr().GetOptVal(OPT_D_INITDIR, _T("E:\\msctmp\\"));
					drvs.insert(dir[0]);
				}
			}

			void GetCleanGames(data_helper::GameTraits::VectorType &games, TCHAR drv, ulonglong size, int iKeepDay, bool greateHundred)
			{
				using data_helper::GameTraits;
				const GameTraits::MapType &allGames = GetDataMgr().GetGames();

				// 1. 保留天数
				// 2. 100MB
				// 3. 本地点击数低
				// 4. 中心点击数低
				// 5. 大小高
				struct SmartCleanPolicy
				{
					bool operator()(const GameTraits::ValueType &lhs, const GameTraits::ValueType &rhs) const
					{
                        return lhs->SvrClick < rhs->SvrClick ||
                            (lhs->SvrClick == rhs->SvrClick && lhs->IdcClick < rhs->IdcClick);
                    }
				};
				

				for(GameTraits::MapConstIterator iter = allGames.begin(); iter != allGames.end(); ++iter)
				{
					if( iter->second->Status != StatusLocal || iter->second->Force )
						continue;

					if( iKeepDay > 0 )
					{
						CTime start(iter->second->SvrVer);
						CTime end = CTime::GetCurrentTime();
						CTimeSpan span = end - start;

						if( span.GetDays() > iKeepDay && 
							iter->second->SvrPath[0] == drv )
						{
							if( greateHundred )
							{
								if( iter->second->Size >= 100 * 1024 )
									games.push_back(iter->second);
							}
							else
								games.push_back(iter->second);
						}
					}
					else
					{
						if( greateHundred )
						{
							if( iter->second->Size >= 100 * 1024 )
								games.push_back(iter->second);
						}
						else
							games.push_back(iter->second);
					}
				}

				std::sort(games.begin(), games.end(), SmartCleanPolicy());

				// 只需要列出符合大小的游戏
				const ulonglong matchSize = size;// + 10LL * 1024LL * 1024LL;

				ulonglong totalSize = 0;
				size_t i = 0;
				for(; i != games.size(); ++i)
				{
					if( totalSize < matchSize )
					{
						totalSize += games[i]->Size;	
					}
					else
						break;
				}
				games.resize(i);
			}


			bool IsEnoughSpace(const stdex::tString &path, const ulonglong totalSize)
			{
				bool needClean = false;
				GetControlMgr().NeedSmartClean(path, totalSize, needClean);
				return needClean;
			}

			ulonglong GetSize(data_helper::GameTraits::VectorType &games)
			{
				typedef data_helper::GameTraits::ElementType GameType;
				using namespace std::tr1::placeholders;

				ulonglong totalSize = 0;
				for(size_t i = 0; i != games.size(); ++i)
					totalSize += games[i]->Size;

				return totalSize;
				/*std::accumulate(games.begin(), games.end(), 
					std::tr1::bind(std::plus<ulonglong>(), 
					std::tr1::bind(&GameType::Size, _1), std::tr1::bind(&GameType::Size, _2)));*/
			}

			stdex::tString GetSavePath(const data_helper::GameTraits::ValueType &val)
			{
				if( utility::IsEmpty(val->SaveFilter) )
					return val->SaveFilter;

				std::string src = CT2A(val->SaveFilter);
				int len = ATL::Base64DecodeGetRequiredLength(src.length());
				std::vector<byte> vec(len + 1);

				ATL::Base64Decode(src.data(), len, &vec[0], &len);

				stdex::tString savePath(CA2T((char *)&(vec[0])));
				return savePath;
			}


			void SetSavePath(data_helper::GameTraits::ValueType &val, const stdex::tString &path)
			{
				std::string src = CT2A(path.c_str());
				int len = ATL::Base64EncodeGetRequiredLength(src.length());
				std::vector<char> vec;
				vec.resize(len + 1);

				ATL::Base64Encode((const byte *)src.c_str(), src.length(), &vec[0], &len);

				utility::Strcpy(val->SaveFilter, CA2T(&vec[0]));
			}

			void CummulateCenterGame(std::map<int, size_t> &centerGames)
			{
				using data_helper::GameTraits;
			
				const GameTraits::MapType &games = GetDataMgr().GetGames();
				for(GameTraits::MapConstIterator iter = games.begin(); 
					iter != games.end(); ++iter)
				{
					if( iter->second->GID <= MIN_IDC_GID )
						continue;

					// I8Play >= 1
					if( iter->second->I8Play >= 1 )
						centerGames[1]++;

					// !val.second->Status && GID >10000
					if( iter->second->Status == i8desk::StatusNone )
						centerGames[2]++;

					{
						// 30天算近期
						CTime now = CTime::GetCurrentTime();
						CTime start(now.GetYear(), now.GetMonth(), now.GetDay(), 0, 0, 0);
						start -= CTimeSpan(30,0,0,0);
						unsigned long startTime = static_cast<DWORD>(start.GetTime());

						// val.second->IdcAddDate > startTime_
						if( iter->second->IdcAddDate > startTime )
							centerGames[3]++;
					}

					// StopRun >= DelLock
					if( iter->second->StopRun == i8desk::DelLock )
						centerGames[4]++;


					// 手动更新方式游戏 
					if( iter->second->Status == i8desk::StatusLocal && 	
						iter->second->SvrVer != iter->second->IdcVer &&	
						iter->second->AutoUpt == 1 )
						centerGames[5]++;
				}
			}


			void CummulateLocalGame(std::map<stdex::tString, size_t> &centerGames)
			{
				using data_helper::GameTraits;
				const GameTraits::MapType &games = i8desk::GetDataMgr().GetGames();
				for(GameTraits::MapConstIterator iter = games.begin(); iter != games.end(); ++iter)
				{
					if( iter->second->Status != StatusLocal )
						continue;

					centerGames[iter->second->CID]++;
				}
			}

			void KeepDayDesc( const KeepDayDescCallback &descCallback, const KeepDayDataCallback &dataCallback )
            {
                enum KeepDay_en
                {
                    dcAll = 0,
                    dcIgnoreHalfMonth = 15,
                    dcIgnoreMonth = 30,
                    dcIgnoreHalfYear = 365 / 2,
                };
                static const struct
                {
                    int keepDay_;
                    const stdex::tString keepDayDesc_;
                } KeepDayType[] = 
                {
                    { dcAll,		     _T("无限制") },
                    { dcIgnoreHalfMonth, _T("15日内更新游戏不删除")},
                    { dcIgnoreMonth,	 _T("一月内更新游戏不删除")},
                    { dcIgnoreHalfYear,	 _T("半年内更新游戏不删除")},
                };
				for(size_t i = 0; i != _countof(KeepDayType); ++i)
				{
					descCallback(KeepDayType[i].keepDayDesc_.c_str());
					dataCallback(i, KeepDayType[i].keepDay_);
				}

			}

			stdex::tString  GetGameName(const long gid)
			{
				using data_helper::GameTraits;
				const GameTraits::MapType &games = i8desk::GetDataMgr().GetGames();

				GameTraits::MapConstIterator iter = games.find(gid);
				if(iter == games.end())
					return _T("未知的游戏");
				
				stdex::tString gameName = iter->second->Name;
				return gameName;

			}

			stdex::tString  GetSvrPath(const long gid)
			{
				using data_helper::GameTraits;
				const GameTraits::MapType &games = i8desk::GetDataMgr().GetGames();

				GameTraits::MapConstIterator iter = games.find(gid);
				if(iter == games.end())
					return _T("未知的游戏");

				stdex::tString svrPath = iter->second->SvrPath;
				return svrPath;
			}

			stdex::tString  GetClass(const long gid)
			{
				using data_helper::GameTraits;
				using data_helper::ClassTraits;

				const GameTraits::MapType &games = i8desk::GetDataMgr().GetGames();
				const ClassTraits::MapType &classes = i8desk::GetDataMgr().GetClasses();

				GameTraits::MapConstIterator iter = games.find(gid);
				if(iter == games.end())
					return _T("未知的游戏");

				ClassTraits::MapConstIterator citer = classes.find(iter->second->CID);
				if(citer == classes.end())
					return _T("未知的游戏类别");

				stdex::tString className = citer->second->Name;
				return className;
            }

            db::TCustomGamePtr GetGameInfo(const long gid)
            {
                using data_helper::GameTraits;

                const GameTraits::MapType &games = i8desk::GetDataMgr().GetGames();

                GameTraits::MapConstIterator iter = games.find(gid);
                if(iter == games.end())
                    return db::TCustomGamePtr();

                return iter->second;
            }

			ulonglong  GetGameSize(const long gid)
			{
				using data_helper::GameTraits;
				const GameTraits::MapType &games = i8desk::GetDataMgr().GetGames();

				GameTraits::MapConstIterator iter = games.find(gid);
				if(iter == games.end())
					return 0;

				return iter->second->Size;
			}

            ulonglong GetGameSize(const std::vector<ulong>& gid)
            {
                using data_helper::GameTraits;
                using data_helper::ClassTraits;
                ULONGLONG totalSize = 0;
                const GameTraits::MapType &games = i8desk::GetDataMgr().GetGames();
                for (UINT i = 0; i < gid.size(); ++i)
                {
                    GameTraits::MapConstIterator iter = games.find(gid[i]);
                    if(iter != games.end())
                    {
                        totalSize += iter->second->Size * 1024LL;
                    }
                }
                return totalSize;
            }
		}
	}
}
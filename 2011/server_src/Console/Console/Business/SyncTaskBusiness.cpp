#include "stdafx.h"
#include "SyncTaskBusiness.h"
#include "GameBusiness.h"
#include "ServerBusiness.h"
#include "../ManagerInstance.h"
#include "../../../../include/Utility/utility.h"
#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../../../../include/Utility/ChineseToLetter.h"

#include <numeric>
#include <set>


namespace i8desk
{
	namespace business
	{
		namespace SyncTask
		{
			bool AddSyncTask(const data_helper::SyncTaskTraits::ValueType &syncTask, const data_helper::GameTraits::VectorType &games)
			{
				using data_helper::SyncTaskTraits;

				SyncTaskTraits::MapType &syncTasks = GetDataMgr().GetSyncTasks();

				if( !GetDataMgr().AddData(syncTasks, syncTask->SID, syncTask) )
					return false;

				using data_helper::SyncGameTraits;
				SyncGameTraits::MapType &syncGames = GetDataMgr().GetSyncGames();

				for(data_helper::GameTraits::VecConstIterator iter = games.begin(); iter != games.end(); ++iter)
				{
					SyncGameTraits::ValueType val(new SyncGameTraits::ElementType);

					utility::Strcpy(val->SID, syncTask->SID);
					val->GID =  (*iter)->GID;

					if( !GetDataMgr().AddData(syncGames, std::make_pair(val->SID, val->GID), val) )
						return false;
				}

				return true;
			}

			bool ModifySyncTask(const data_helper::SyncTaskTraits::ValueType &syncTask)
			{
				using data_helper::SyncTaskTraits;

				SyncTaskTraits::MapType &syncTasks = GetDataMgr().GetSyncTasks();

				if( !GetDataMgr().ModifyData(syncTasks, syncTask->SID, syncTask) )
					return false;

				return true;
			}

			bool DelSyncTask(const stdex::tString &SID)
			{

				using data_helper::SyncGameTraits;
				SyncGameTraits::MapType &syncGames = GetDataMgr().GetSyncGames();

				SyncGameTraits::VectorType GIDs;
				std::for_each(syncGames.begin(), syncGames.end(), [SID, &GIDs](const SyncGameTraits::MapType::value_type &val)
				{
					if( SID == val.second->SID )
						GIDs.push_back(val.second);
				});

				for(SyncGameTraits::VecConstIterator iter = GIDs.begin(); iter != GIDs.end(); ++iter)
				{
					if (utility::Strcmp(SID.c_str(), (*iter)->SID) != 0)
						continue;

					if( !GetDataMgr().DelData(syncGames, std::make_pair((*iter)->SID, (*iter)->GID)) )
						return false;
				}

				using data_helper::SyncTaskTraits;

				SyncTaskTraits::MapType &syncTasks = GetDataMgr().GetSyncTasks();

				if( !GetDataMgr().DelData(syncTasks, SID) )
					return false;

				return true;
			}


			bool DelSyncGame(const data_helper::SyncTaskStatusTraits::VectorType &syncGamestatus)
			{
				using data_helper::SyncGameTraits;
				using data_helper::SyncTaskStatusTraits;
				SyncGameTraits::MapType &syncGames = GetDataMgr().GetSyncGames();

				for(SyncTaskStatusTraits::VecConstIterator iter = syncGamestatus.begin(); iter != syncGamestatus.end(); ++iter)
				{
					SyncGameTraits::ValueType val(new SyncGameTraits::ElementType);

					utility::Strcpy(val->SID, (*iter)->SID);
					val->GID =  (*iter)->GID;

					if( !GetDataMgr().DelData(syncGames, std::make_pair(val->SID, val->GID)) )
						return false;
				}

				return true;
			}

			bool ModifySyncGame( const data_helper::SyncTaskTraits::ValueType &syncTask, data_helper::SyncTaskStatusTraits::VectorType &syncGamestatus)
			{
				using data_helper::SyncTaskStatusTraits;

				for(SyncTaskStatusTraits::VecIterator iter = syncGamestatus.begin(); iter != syncGamestatus.end(); ++iter)
				{
					if(utility::Strcmp(syncTask->SID, (*iter)->SID) != 0)
						continue;

					(*iter)->NodeDir[0] = syncTask->NodeDir[0];
				}

				return true;
			}

			bool SaveSyncGame(data_helper::SyncTaskStatusTraits::VectorType &syncGamestatus)
			{
				using data_helper::SyncGameTraits;
				using data_helper::SyncTaskStatusTraits;
				SyncGameTraits::MapType &syncGames = GetDataMgr().GetSyncGames();

				for(SyncTaskStatusTraits::VecConstIterator iter = syncGamestatus.begin(); iter != syncGamestatus.end(); ++iter)
				{
					SyncGameTraits::ValueType val(new SyncGameTraits::ElementType);

					utility::Strcpy(val->SID, (*iter)->SID);
					val->GID =  (*iter)->GID;

					SyncGameTraits::MapConstIterator mapiter = syncGames.find(std::make_pair(val->SID, val->GID));
					if( mapiter != syncGames.end() ) 
						continue;

					if( !GetDataMgr().AddData(syncGames, std::make_pair(val->SID, val->GID),val) )
						return false;
				}

				return true;
			}




			void GetAllGamesPath(std::set<stdex::tString> &pathes)
			{
				using i8desk::data_helper::GameTraits;
				GameTraits::MapType &games = i8desk::GetDataMgr().GetGames();
				i8desk::GetDataMgr().GetAllData(games);

				struct PathOp
				{
					std::set<stdex::tString> &pathes_;
					PathOp( std::set<stdex::tString> &pathes )
						: pathes_(pathes)
					{}

					void operator()(const GameTraits::MapType::value_type &val) const
					{
						// 本地游戏
						if( utility::IsEmpty(val.second->SvrPath) ||
							val.second->Status != StatusLocal )
							return;

						// 目录应该大于两级
						size_t cnt = std::count_if(val.second->SvrPath, val.second->SvrPath + std::char_traits<TCHAR>::length(val.second->SvrPath), 
							std::tr1::bind(std::equal_to<TCHAR>(), _T('\\'), std::tr1::placeholders::_1));
						if( cnt < 2 )
							return;

						TCHAR SvrPath[MAX_PATH] = {0};
						utility::Strcpy(SvrPath, val.second->SvrPath);

						::PathRemoveBackslash(SvrPath);
						static stdex::tString path;
						path = SvrPath;

						size_t pos = path.find(val.second->Name);
						if( pos == stdex::tString::npos )
							pos = path.find_last_of(_T('\\'));
						path = path.substr(0, pos);
						utility::Strcpy(SvrPath, path);

						::PathRemoveBackslash(SvrPath);
						SvrPath[0] = ::toupper(SvrPath[0]);

						path = SvrPath;
						pathes_.insert(path);
					}
				};

				std::for_each(games.begin(), games.end(), PathOp(pathes));
			}

			void ServerNodeDir( std::set<stdex::tString> &nodes)
			{
				std::set<stdex::tString> pathes;
				GetAllGamesPath( pathes);

				struct NodeOp
				{
					std::set<stdex::tString> &nodes_;
					NodeOp( std::set<stdex::tString> &nodes )
						: nodes_(nodes)
					{}
					void operator()(const  std::set<stdex::tString>::value_type &val) const
					{ 
						stdex::tString path = val;
						if( path.empty() )
							return;

						size_t pos = 0;
						pos = path.find(_T(":"));
						path = path.substr(0, pos);
						nodes_.insert(path);
					}
				};

				std::for_each(pathes.begin(), pathes.end(), NodeOp(nodes));
			}


			bool HasSameIP(const data_helper::SyncTaskTraits::ValueType &syncTask)
			{
				using data_helper::ServerTraits;
				const ServerTraits::MapType &servers = GetDataMgr().GetServers();

				for(ServerTraits::MapConstIterator iter = servers.begin();
					iter != servers.end(); ++iter)
				{
					if( iter->second->SvrType == OtherServer && iter->second->SvrIP == syncTask->SoucIp )
					{
						return true;
					}
				}

				return false;
			}

			bool HasSameTaskName(const data_helper::SyncTaskTraits::ValueType &syncTask)
			{
				using data_helper::SyncTaskTraits;
				const SyncTaskTraits::MapType &tasks = GetDataMgr().GetSyncTasks();

				for(SyncTaskTraits::MapConstIterator iter = tasks.begin();
					iter != tasks.end(); ++iter)
				{
					if( utility::Strcmp(iter->second->SID, syncTask->SID) == 0 )
						continue;
					if( utility::Strcmp(iter->second->Name, syncTask->Name) == 0 )
						return false;
				}

				return true;
			}

			bool HasSameNodeDir(const data_helper::SyncTaskTraits::ValueType &syncTask)
			{
				using data_helper::SyncTaskTraits;
				const SyncTaskTraits::MapType &tasks = GetDataMgr().GetSyncTasks();

				using i8desk::data_helper::ServerTraits;
				const ServerTraits::MapType &servers = GetDataMgr().GetServers();

				ServerTraits::MapConstIterator mapiter = servers.find(syncTask->SvrID);
				if( mapiter == servers.cend() )
					return false;

				if(i8desk::business::Server::IsMainServer(mapiter->second))
					return false;

				if(syncTask->SoucIp != syncTask->DestIp)
					return false;

				for(SyncTaskTraits::MapConstIterator iter = tasks.begin();
					iter != tasks.end(); ++iter)
				{
					if( utility::Strcmp(iter->second->SID, syncTask->SID) == 0 )
						continue;
					if( utility::Strcmp(iter->second->NodeDir, syncTask->NodeDir) == 0 )
						return true;
				}

				return false;
			}

			bool HasSamePath(const data_helper::SyncTaskTraits::ValueType &syncTask, const data_helper::GameTraits::VectorType &games)
			{
				using i8desk::data_helper::ServerTraits;
				const ServerTraits::MapType &servers = GetDataMgr().GetServers();

				ServerTraits::MapConstIterator mapiter = servers.find(syncTask->SvrID);
				if( mapiter == servers.cend() )
					return false;

				if(i8desk::business::Server::IsMainServer(mapiter->second))
				{
					for(auto iter = games.cbegin();	iter != games.cend(); ++iter)
					{
						if((*iter)->SvrPath[0] == syncTask->NodeDir[0])
							return true;
					}
				}

				return false;
			}

			void MatchSyncTask(const data_helper::SyncTaskTraits::VectorType &syncTasks, data_helper::GameTraits::VectorType &syncGames)
			{
				using data_helper::GameTraits;
				using data_helper::SyncGameTraits;

				std::set<GameTraits::ValueType> setUnComplateGames;

				// 获取当前游戏
				struct MatchSvrPath
				{
					std::set<GameTraits::ValueType> &setUnComplateGames_;

					MatchSvrPath(std::set<GameTraits::ValueType> &setUnComplateGames)
						: setUnComplateGames_(setUnComplateGames)
					{}

					void operator()(const data_helper::SyncTaskTraits::ValueType &val) const
					{
						const GameTraits::MapType &games = i8desk::GetDataMgr().GetGames();
						for(GameTraits::MapConstIterator iter = games.begin();
							iter != games.end(); ++iter)
						{
							if( iter->second->Status != data_helper::StatusLocal )
								continue;

							const TCHAR *const src = iter->second->SvrPath;
							size_t srcLen = _tcslen(src);

							const TCHAR *const dst = val->SvrDir;
							size_t dstLen = _tcslen(dst);

							using namespace std::tr1::placeholders;
							const TCHAR *const match = std::search(src, src + srcLen, dst, dst + dstLen, [](int lhs, int rhs)->bool
							{
								return ::tolower(lhs) == ::tolower(rhs);
							});

							if( match != src + srcLen )
							{
								setUnComplateGames_.insert(iter->second);
							}
						}
					}
				};

				std::for_each(syncTasks.begin(), syncTasks.end(), MatchSvrPath(setUnComplateGames));
				syncGames.reserve(setUnComplateGames.size());

				std::copy(setUnComplateGames.begin(), setUnComplateGames.end(), std::back_inserter(syncGames));
			}


			void CumulateSize(const data_helper::GameTraits::VectorType &curGames, uint64 &gamesSize)
			{
				using namespace std::tr1::placeholders;

				// 计算同步游戏大小
				/*gamesSize = std::accumulate(curGames.begin(), curGames.end(), 0LL,
				std::tr1::bind(std::plus<uint64>(), 
				std::tr1::bind(&data_helper::GameTraits::ElementType::Size, _1), 
				std::tr1::bind(&data_helper::GameTraits::ElementType::Size, _2)));*/
				for(data_helper::GameTraits::VecConstIterator iter = curGames.begin(); iter != curGames.end();
					++iter)
				{
					gamesSize += (*iter)->Size;
				}
			}


			uint64 GetSyncTaskSpeed()
			{
				//using i8desk::data_helper::SyncGameStatusTraits;
				//SyncGameStatusTraits::MapType syncStatus = i8desk::GetRealDataMgr().GetSyncGameStatus();

				uint64 speed = 0;
				/*for(SyncGameStatusTraits::MapConstIterator iter = syncStatus.begin();
				iter != syncStatus.end(); ++iter)
				{
				if( iter->second->Type == 1 )
				speed += iter->second->TransferRate;
				}*/

				return speed;
			}

			void GetTaskName( std::set<std::pair<stdex::tString, stdex::tString>> &tasknames)
			{
				using i8desk::data_helper::SyncTaskTraits;
				SyncTaskTraits::MapType &synctask = i8desk::GetDataMgr().GetSyncTasks();
				tasknames.clear();
				struct TaskNameOp
				{
					std::set<std::pair<stdex::tString, stdex::tString>> &synctask_;
					TaskNameOp( std::set<std::pair<stdex::tString, stdex::tString>> &synctask )
						: synctask_(synctask)
					{}
					void operator()(const SyncTaskTraits::MapType::value_type &val) const
					{ 
						synctask_.insert(std::make_pair(val.second->NameID,val.second->Name));
					}
				};

				std::for_each(synctask.begin(), synctask.end(), TaskNameOp(tasknames));
			}

			void SetSyncTaskStatus( const stdex::tString &SID, i8desk::SyncTaskState state)
			{
				using i8desk::data_helper::SyncTaskTraits;
				SyncTaskTraits::MapType &allSyncTasks = i8desk::GetDataMgr().GetSyncTasks();

				auto iter = allSyncTasks.find(SID.c_str());
				if( iter == allSyncTasks.end())
					return;

				iter->second->Status = state;

				i8desk::GetDataMgr().ModifyData(allSyncTasks, SID, iter->second, MASK_TSYNCTASK_STATUS);
		
			}

			void FilterByNameID(const data_helper::SyncTaskTraits::MapType &syncTasks, data_helper::SyncTaskTraits::MapType &diffSyncTasks)
			{
				using i8desk::data_helper::SyncTaskTraits;

				struct DiffNameID
				{
					bool operator()(const SyncTaskTraits::ValueType &lhs, const SyncTaskTraits::ValueType &rhs) const
					{
						return utility::Strcmp(lhs->SID, rhs->SID) < 0;
					}
				};
				typedef std::set<SyncTaskTraits::ValueType, DiffNameID> DiffNameSyncTasks;
				DiffNameSyncTasks diffNameSyncTasks;

				for(SyncTaskTraits::MapType::const_iterator iter = syncTasks.begin(); iter != syncTasks.end(); ++iter )
				{
					diffNameSyncTasks.insert(iter->second);
				}

				for(DiffNameSyncTasks::const_iterator iter = diffNameSyncTasks.begin(); iter != diffNameSyncTasks.end(); ++iter)
					diffSyncTasks.insert(std::make_pair((*iter)->Name, *iter));
			}

			void GetSameNameIDSrcPaths(const stdex::tString &nameID, std::vector<stdex::tString> &srcPaths)
			{
				using i8desk::data_helper::SyncTaskTraits;
				const SyncTaskTraits::MapType &allSyncTasks = i8desk::GetDataMgr().GetSyncTasks();

				for(SyncTaskTraits::MapType::const_iterator iter = allSyncTasks.begin(); iter != allSyncTasks.end(); ++iter )
				{
					if( nameID == iter->second->NameID )
					{
						srcPaths.push_back(iter->second->SvrDir);
					}
				}
			}

			void GetSameNameIDDstPaths(const stdex::tString &nameID, std::vector<stdex::tString> &dstPaths)
			{
				using i8desk::data_helper::SyncTaskTraits;
				const SyncTaskTraits::MapType &allSyncTasks = i8desk::GetDataMgr().GetSyncTasks();

				for(SyncTaskTraits::MapType::const_iterator iter = allSyncTasks.begin(); iter != allSyncTasks.end(); ++iter)
				{
					if( nameID == iter->second->NameID )
					{
						dstPaths.push_back(iter->second->NodeDir);
					}
				}
			}

			bool DeleteSyncGame(data_helper::SyncTaskStatusTraits::VectorType &taskgame, const stdex::tString SID)
			{
				using i8desk::data_helper::SyncGameTraits;
				using i8desk::data_helper::SyncTaskStatusTraits;

				for( SyncTaskStatusTraits::VecIterator iter = taskgame.begin(); iter != taskgame.end(); ++iter )
				{
					if(utility::Strcmp(SID.c_str(), (*iter)->SID) == 0)
						taskgame.erase(iter);
				}

				SyncGameTraits::MapType &syncGames = GetDataMgr().GetSyncGames();
				for( SyncGameTraits::MapConstIterator iter = syncGames.begin(); iter != syncGames.end(); ++iter )
				{
					if(utility::Strcmp(SID.c_str(), iter->second->SID) == 0)
					{
						if( !i8desk::GetDataMgr().DelData(syncGames, std::make_pair(SID.c_str(), iter->second->GID) ))
							return false;
					}
				}
				return true;
			}

			bool CanNotifyGame(data_helper::GameTraits::VectorType &games, const std::tr1::function<void(const stdex::tString &)> &callback)
			{
				using namespace data_helper;
				const SyncTaskTraits::MapType &syncTasks = GetDataMgr().GetSyncTasks();

				bool suc = false;
				for(size_t i = 0; i != games.size(); ++i)
				{
					stdex::tString gamePath = games[i]->SvrPath;
					for(SyncTaskTraits::MapConstIterator iter = syncTasks.begin();
						iter != syncTasks.end(); ++iter)
					{
						if( gamePath.find(iter->second->SvrDir) != stdex::tString::npos )
						{
							suc = true;
							i8desk::business::Game::LocalTaskOperate(games[i], i8desk::toSync, false);
						}
					}
				}

				if( !suc )
					callback(_T("没有按类别目录同步的同步任务,请添加目录同步任务"));

				return suc;

			}

			bool AddSyncGame(const stdex::tString &SID, long gid)
			{
				data_helper::SyncGameTraits::ValueType syncGame(new data_helper::SyncGameTraits::ElementType);
				utility::Strcpy(syncGame->SID, SID);
				syncGame->GID = gid;

				std::pair<stdex::tString, i8desk::uint32> key = std::make_pair(SID, gid);

				return GetDataMgr().EnsureData(i8desk::GetDataMgr().GetSyncGames(), key, syncGame);
			}

			void AddSyncTaskStatus(const data_helper::SyncTaskTraits::VectorType &syncTasks, const data_helper::GameTraits::MapType &games)
			{
				using data_helper::GameTraits;
				using data_helper::SyncGameTraits;

				struct Add
				{
					bool operator()(long gid, LPCTSTR sid, LPCTSTR nameID)
					{
						using data_helper::SyncTaskStatusTraits;

						std::pair<stdex::tString, long> key = std::make_pair(sid, gid);

						SyncTaskStatusTraits::ValueType status(new SyncTaskStatusTraits::ElementType);
						status->GID = gid;
						utility::Strcpy(status->SID, sid);
						utility::Strcpy(status->NameID, nameID);
						status->UpdateState = GameUnComplate;

						return GetDataMgr().EnsureData(key, status);
					}
				};
				// 获取当前游戏
				struct MatchSvrPath
				{
					const data_helper::GameTraits::MapType &games_;
					MatchSvrPath(const data_helper::GameTraits::MapType &games)
						: games_(games)
					{}

					void operator()(const data_helper::SyncTaskTraits::ValueType &val) const
					{
						for(GameTraits::MapConstIterator iter = games_.begin(); iter != games_.end(); ++iter)
						{
							if( iter->second->Status != data_helper::StatusLocal )
								continue;

							const TCHAR *const src = iter->second->SvrPath;
							size_t srcLen = _tcslen(src);

							const TCHAR *const dst = val->SvrDir;
							size_t dstLen = _tcslen(dst);

							using namespace std::tr1::placeholders;
							const TCHAR *const match = std::search(src, src + srcLen, dst, dst + dstLen, [](int lhs, int rhs)
							{
								return ::tolower(lhs) == ::tolower(rhs);
							});

							if( match != src + srcLen )
							{
								Add()(iter->second->GID, val->SID, val->NameID);
							}

						}
					}
				};

				std::for_each(syncTasks.begin(), syncTasks.end(), MatchSvrPath(games));
			}


			void AddGameToSyncTaskStatus(const data_helper::SyncTaskTraits::VectorType &syncTasks, 
				const data_helper::GameTraits::ValueType &game)
			{
				using data_helper::GameTraits;
				using data_helper::SyncTaskTraits;

				struct Add
				{
					bool SyncTaskStatus(long gid, LPCTSTR sid, LPCTSTR nameID)
					{
						using i8desk::data_helper::SyncTaskStatusTraits;
						SyncTaskStatusTraits::ValueType status(new SyncTaskStatusTraits::ElementType);
						status->GID = gid;
						utility::Strcpy(status->SID, sid);
						utility::Strcpy(status->NameID, nameID);
						status->UpdateState = GameUnComplate;

						return GetDataMgr().EnsureData(std::make_pair(sid, gid), status);
					}
				};


				if( syncTasks.empty() )
					return;

				struct Compare
				{
					bool operator()(int lhs, int rhs)
					{
						return ::tolower(lhs) == ::tolower(rhs);
					}
				};
				SyncTaskTraits::ValueType syncTask;
				for(SyncTaskTraits::VecConstIterator iter = syncTasks.begin();
					iter != syncTasks.end(); ++iter)
				{
					const TCHAR * const src = game->SvrPath;
					size_t srcLen = _tcslen(src);

					const TCHAR * const dst = (*iter)->SvrDir;
					size_t dstLen = _tcslen(dst);

					using namespace std::tr1::placeholders;
					const TCHAR *match = std::search(src, src + srcLen, dst, dst + dstLen, Compare());

					if( match != src + srcLen )
					{
						syncTask = *iter;
						break;
					}
				}

				Add().SyncTaskStatus(game->GID, syncTask->SID, syncTask->NameID);
			}


			void GetSyncTasks(data_helper::SyncTaskTraits::VectorType &newSyncTasks)
			{
				using data_helper::SyncTaskTraits;
				const SyncTaskTraits::MapType &syncTasks = GetDataMgr().GetSyncTasks();

				for(SyncTaskTraits::MapConstIterator iter = syncTasks.begin();
					iter != syncTasks.end(); ++iter)
				{
					newSyncTasks.push_back(iter->second);
				}
			}

			void GetSyncTaskStatus(data_helper::SyncTaskStatusTraits::VectorType &newsyncstatus)
			{
				using data_helper::SyncGameTraits;
				using data_helper::SyncTaskTraits;
				using data_helper::GameTraits;
				using data_helper::SyncTaskStatusTraits;

				const SyncGameTraits::MapType &syncGame = GetDataMgr().GetSyncGames();
				const SyncTaskTraits::MapType &syncTask = GetDataMgr().GetSyncTasks();
				const GameTraits::MapType	  &game = GetDataMgr().GetGames();

				for(SyncGameTraits::MapConstIterator iter = syncGame.begin(); iter != syncGame.end(); ++iter)
				{
					SyncTaskStatusTraits::ValueType val(new SyncTaskStatusTraits::ElementType);
					utility::Strcpy(val->SID, iter->second->SID);
					val->GID = iter->second->GID;

					SyncTaskTraits::MapConstIterator mapiter = syncTask.find(val->SID);
					if( mapiter != syncTask.end() )
						utility::Strcpy(val->GameName, mapiter->second->Name);

					GameTraits::MapConstIterator gameiter = game.find(val->GID);
					if( gameiter != game.end() )
					{
						utility::Strcpy(val->SvrDir, gameiter->second->SvrPath);
						TCHAR NodeDir[MAX_PATH] = {0};
						utility::Strcpy(NodeDir, gameiter->second->SvrPath);
						if( mapiter != syncTask.end() )
						{
							NodeDir[0] = mapiter->second->NodeDir[0];
							utility::Strcpy(val->NodeDir, NodeDir);
						}
					}

					newsyncstatus.push_back(val);
				}
			}


			stdex::tString GetStateDesc(const data_helper::SyncTaskStatusTraits::ValueType &val)
			{
				static struct  
				{
					int state_;
					stdex::tString desc_;
				}StateDesc[] = 
				{
					{ GameComplate, _T("已完成")	},
					{ GameUnComplate, _T("未完成")},
					{ GamePaused, _T("已暂停")	},
					{ GameError, _T("出现错误")},
					{ GameRunning, _T("更新中")	}

				};

				return StateDesc[val->UpdateState].desc_;
			}


			stdex::tString GetTaskDesc(const data_helper::SyncTaskStatusTraits::ValueType &val)
			{
				using data_helper::SyncTaskTraits;
				const SyncTaskTraits::MapType &syncTasks = GetDataMgr().GetSyncTasks();

				for(SyncTaskTraits::MapConstIterator iter = syncTasks.begin();
					iter != syncTasks.end(); ++iter)
				{
					if(utility::Strcmp(iter->second->SID, val->SID) == 0)
						return iter->second->Name;
				}

				return _T("未知的任务");
			}

			stdex::tString GetTaskNameBySID(const stdex::tString &SID)
			{
				using data_helper::SyncTaskTraits;
				const SyncTaskTraits::MapType &syncTasks = GetDataMgr().GetSyncTasks();

				for(SyncTaskTraits::MapConstIterator iter = syncTasks.begin();
					iter != syncTasks.end(); ++iter)
				{
					if(utility::Strcmp(iter->second->SID, SID.c_str()) == 0)
						return iter->second->Name;
				}

				return _T("未知的任务");
			}


			void TaskFilter(data_helper::SyncTaskStatusTraits::VectorType &taskstatus, const stdex::tString &SID )
			{
				using data_helper::SyncGameTraits;
				using data_helper::SyncTaskTraits;
				using data_helper::GameTraits;
				using data_helper::SyncTaskStatusTraits;

				const SyncGameTraits::MapType &syncgame = GetDataMgr().GetSyncGames();
				const SyncTaskTraits::MapType &syncTask = GetDataMgr().GetSyncTasks();
				const GameTraits::MapType	  &game = GetDataMgr().GetGames();

				for(SyncGameTraits::MapConstIterator iter = syncgame.begin(); iter != syncgame.end(); ++iter )
				{
					if( SID != iter->second->SID )
						continue;

					SyncTaskStatusTraits::ValueType val(new SyncTaskStatusTraits::ElementType);
					utility::Strcpy(val->SID, iter->second->SID);
					val->GID = iter->second->GID;


					GameTraits::MapConstIterator gameiter = game.find(val->GID);
					if( gameiter != game.end() )
					{
						utility::Strcpy(val->GameName, gameiter->second->Name);
						utility::Strcpy(val->SvrDir, gameiter->second->SvrPath);
						TCHAR NodeDir[MAX_PATH] = {0};
						utility::Strcpy(NodeDir, gameiter->second->SvrPath);
						val->UpdateState = GameUnComplate;

						SyncTaskTraits::MapConstIterator mapiter = syncTask.find(val->SID);
						if( mapiter != syncTask.end() )
						{
							NodeDir[0] = mapiter->second->NodeDir[0];
							utility::Strcpy(val->NodeDir, NodeDir);
						}
					}

					taskstatus.push_back(val);
				}
			}

			void FindSync(LPCTSTR destText, const data_helper::SyncTaskStatusTraits::VectorType &tasks, data_helper::SyncTaskStatusTraits::VectorType &values)
			{
				stdex::tString text(destText);
				stdex::ToLower(text);

				typedef std::set<data_helper::SyncTaskStatusTraits::ValueType> TasksTmp;
				TasksTmp tasksTmp;


				// 试图匹配名称
				stdex::tString gameName;
				stdex::tString PYRes;

				for(size_t i = 0; i != tasks.size(); ++i)
				{
					gameName = tasks[i]->GameName;
					stdex::ToLower(gameName);

					//i8desk::GetPY(gameName, PYRes);
					PYRes = i8desk::GetPyLetter(gameName);

					if( gameName.find(text) != stdex::tString::npos || 
						PYRes.find(text) != stdex::tString::npos )
					{
						tasksTmp.insert(tasks[i]);
					}
				}

				// 试图匹配GID
				for(size_t i = 0; i != tasks.size(); ++i)
				{
					DWORD gid = stdex::ToNumber<DWORD>(text);

					if( tasks[i]->GID == gid )
					{
						tasksTmp.insert(tasks[i]);
					}
				}

				for(TasksTmp::const_iterator iter = tasksTmp.begin(); iter != tasksTmp.end(); ++iter)
					values.push_back(*iter);
			}

			void FindSync(LPCTSTR destText, const data_helper::SyncGameTraits::VectorType &tasks, data_helper::SyncGameTraits::VectorType &values)
			{
				stdex::tString text(destText);
				stdex::ToLower(text);

				typedef std::set<data_helper::SyncGameTraits::ValueType> TasksTmp;
				TasksTmp tasksTmp;


				// 试图匹配名称
				stdex::tString gameName;
				stdex::tString PYRes;

				for(size_t i = 0; i != tasks.size(); ++i)
				{
					gameName = Game::GetGameName(tasks[i]->GID);
					stdex::ToLower(gameName);

					//i8desk::GetPY(gameName, PYRes);
					PYRes = i8desk::GetPyLetter(gameName);

					if( gameName.find(text) != stdex::tString::npos || 
						PYRes.find(text) != stdex::tString::npos )
					{
						tasksTmp.insert(tasks[i]);
					}
				}

				// 试图匹配GID
				for(size_t i = 0; i != tasks.size(); ++i)
				{
					DWORD gid = stdex::ToNumber<DWORD>(text);

					if( tasks[i]->GID == gid )
					{
						tasksTmp.insert(tasks[i]);
					}
				}

				for(TasksTmp::const_iterator iter = tasksTmp.begin(); iter != tasksTmp.end(); ++iter)
					values.push_back(*iter);
			}

			void SetSyncStatus(data_helper::SyncTaskStatusTraits::VectorType &taskstatus, const stdex::tString &SID)
			{
				using i8desk::data_helper::SyncTaskStatusTraits;
				const SyncTaskStatusTraits::MapType &tasks = i8desk::GetRealDataMgr().GetSyncTaskStatus(SID);

				if( taskstatus.empty() ||
					tasks.empty() )
					return;

				for(SyncTaskStatusTraits::VecIterator iter = taskstatus.begin(); iter != taskstatus.end(); ++iter)
				{
					SyncTaskStatusTraits::MapConstIterator mapiter = tasks.find(std::make_pair((*iter)->SID, (*iter)->GID));
					if( mapiter != tasks.cend() )
					{
						(*iter)->BytesTotal			= mapiter->second->BytesTotal ;
						(*iter)->BytesTransferred	= mapiter->second->BytesTransferred ;
						(*iter)->TransferRate		= mapiter->second->TransferRate ;
						(*iter)->UpdateSize			= mapiter->second->UpdateSize;
						(*iter)->UpdateState		= mapiter->second->UpdateState;	
						utility::Strcpy((*iter)->Error, mapiter->second->Error);	
					};
				}
			}


			void GetSyncGamesBySID(const stdex::tString &SID,  data_helper::GameTraits::VectorType &games)
			{
				using i8desk::data_helper::SyncGameTraits;
				const SyncGameTraits::MapType &syncGames = i8desk::GetDataMgr().GetSyncGames();

				std::set<long> GIDs;

				for(SyncGameTraits::MapConstIterator iter = syncGames.begin(); iter != syncGames.end(); ++iter)
				{
					if(SID == iter->second->SID)
						GIDs.insert(iter->second->GID);
				}

				using i8desk::data_helper::GameTraits;
				const GameTraits::MapType &Games = i8desk::GetDataMgr().GetGames();

				for(std::set<long>::const_iterator siter = GIDs.begin(); siter != GIDs.end(); ++siter)
				{
					GameTraits::MapConstIterator mapiter = Games.find(*siter);
					if(mapiter != Games.end())
					{
						games.push_back(mapiter->second);
					}
				}
			}

			bool IsPaused(const stdex::tString &SID)
			{
				const data_helper::SyncTaskTraits::MapType &syncTasks = GetDataMgr().GetSyncTasks();
				auto iter = syncTasks.find(SID);
				if( iter != syncTasks.end() )
					return iter->second->Status == TaskPaused;
				else
					return false;
			}

			bool IsRunning(const stdex::tString &SID)
			{
				const data_helper::SyncTaskTraits::MapType &syncTasks = GetDataMgr().GetSyncTasks();
				auto iter = syncTasks.find(SID);
				if( iter != syncTasks.end() )
					return iter->second->Status == TaskStarted || iter->second->Status == TaskResumed;
				else
					return false;
			}

		}
	}
}
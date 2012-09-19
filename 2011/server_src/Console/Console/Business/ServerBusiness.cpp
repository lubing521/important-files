#include "stdafx.h"
#include "ServerBusiness.h"


#include "../ManagerInstance.h"
#include "../Misc.h"
#include "../../../../include/Utility/utility.h"
#include "../../../../include/Extend STL/StringAlgorithm.h"

#include "../MessageBox.h"
#include "MonitorBusiness.h"

namespace i8desk
{
	namespace business
	{
		namespace Server
		{

			bool IsRunOnMainServer()
			{
				return IsRunOnServer();
			}

			bool IsMainServer(const data_helper::ServerTraits::ValueType &svr)
			{
				return svr->SvrType == i8desk::MainServer;
			}

			void GetAllService(const std::tr1::function<void(LPCTSTR, LPCTSTR)> &callback)
			{
				using i8desk::data_helper::ServerTraits;
				const ServerTraits::MapType &servers = i8desk::GetDataMgr().GetServers();

				for(ServerTraits::MapConstIterator iter = servers.begin(); iter != servers.end(); ++iter)
				{
					callback(iter->second->SvrName, iter->second->SvrID);
				}
			}

			void VDiskFilter(data_helper::VDiskClientTraits::VectorType &diskClientStatuses, const stdex::tString &svrID, const stdex::tString &vID)
			{
				//static data_helper::VDiskClientTraits::MapType oldVDiskStatus;
				data_helper::VDiskClientTraits::MapType vDiskClientStatus = i8desk::GetRealDataMgr().GetVDiskClientStatus();
				const data_helper::ServerTraits::MapType &servers = GetDataMgr().GetServers();
				const data_helper::ServerTraits::MapConstIterator svrIter = servers.find(svrID);
				if( svrIter == servers.end() )
				{
					assert(0);
					return;
				}

				for(data_helper::VDiskClientTraits::MapConstIterator iter = vDiskClientStatus.begin();
					iter != vDiskClientStatus.end(); ++iter)
				{
					// 超过30秒则视为断开链接
					/*data_helper::VDiskClientTraits::MapConstIterator oldIter = oldVDiskStatus.find(iter->first);
					if( oldIter != oldVDiskStatus.end() )
					{
						const u_long time = iter->second->OnlineTime - oldIter->second->OnlineTime;
						if( !oldVDiskStatus.empty() && ( time > 30) )
							continue;
					}*/

					if( vID == iter->second->VID )
					{
						diskClientStatuses.push_back(iter->second);
					}
				}

				//oldVDiskStatus = vDiskClientStatus;
			}

			const data_helper::ServerTraits::ValueType &GetServiceBySID(const stdex::tString &sid)
			{
				i8desk::data_helper::ServerTraits::MapConstIterator iter = i8desk::GetDataMgr().GetServers().find(sid);
				assert(iter != i8desk::GetDataMgr().GetServers().end());

				if( iter == i8desk::GetDataMgr().GetServers().end() )
				{
					i8desk::data_helper::ServerTraits::ValueType svr(new i8desk::data_helper::ServerTraits::ElementType);
					utility::Strcpy(svr->SvrID, sid);

					i8desk::GetDataMgr().GetServers()[sid] = svr;
					return i8desk::GetDataMgr().GetServers()[sid];
				}

				return iter->second;
			}

			const data_helper::ServerTraits::ValueType &GetMainServer()
			{
				using i8desk::data_helper::ServerTraits;
				const ServerTraits::MapType &servers = i8desk::GetDataMgr().GetServers();
				
				for(ServerTraits::MapConstIterator iter = servers.begin(); iter != servers.end(); ++iter)
				{
					if( iter->second->SvrType == MainServer )
						return iter->second;
				}

				assert(0);
				return servers.begin()->second;
			}

			void ServerFilter(data_helper::ServerStatusTraits::VectorType &servers, const stdex::tString &svrID)
			{
				data_helper::ServerStatusTraits::MapType serverStatus = i8desk::GetRealDataMgr().GetServerStatus();
				if(serverStatus.size() == 0)
					return;

				struct ServerStatusCount
				{
					const stdex::tString &svrID_;
					data_helper::ServerStatusTraits::VectorType &servers_;

					ServerStatusCount(const stdex::tString &svrID, data_helper::ServerStatusTraits::VectorType &servers)
						: svrID_(svrID)
						, servers_(servers)
					{}

					void operator()(const data_helper::ServerStatusTraits::MapType::value_type &rhs) const
					{
						if( svrID_ == rhs.second->SvrID )
							servers_.push_back(rhs.second);
					}
				};
				std::for_each(serverStatus.begin(), serverStatus.end(), ServerStatusCount(svrID, servers));
			}


			bool HasSameName(const stdex::tString &svrID, const stdex::tString &svrName)
			{
				using i8desk::data_helper::ServerTraits;
				const ServerTraits::MapType &servers = i8desk::GetDataMgr().GetServers();

				stdex::tString tmpSvrName = stdex::ToUpper(svrName);

				for(ServerTraits::MapConstIterator iter = servers.begin(); iter != servers.end(); ++iter)
				{
					if( iter->second->SvrID == svrID )
						continue;
					if( iter->second->SvrName == tmpSvrName )
						return true;
				}

				return false;
			}

		

			bool ModifyAreaPriority(bool isNew, const data_helper::AreaTraits::ElementType *area, const stdex::tString &svrID)
			{
				using i8desk::data_helper::AreaTraits;
				AreaTraits::MapType &areas = GetDataMgr().GetAreas();

				using i8desk::data_helper::ServerTraits;
				const ServerTraits::MapType &servers = i8desk::GetDataMgr().GetServers();

				if( isNew )
				{
					if( !utility::IsEmpty(area->SvrID) )
					{
						if( area->SvrID != svrID && servers.find(area->SvrID) != servers.end())
						{
							// 检测该区域是否已经分配到其他服务器
							stdex::tOstringstream os;
							os << _T("[") << area->Name << _T("]") << _T("区域已经分配到服务器")
								<< _T("[") << servers.find(area->SvrID)->second->SvrName << _T("]")
								<< _T(", 确认需要重新分配到此服务器吗?");

							CMessageBox msgDlg(_T("区域优先"),os.str().c_str() );
							if( msgDlg.DoModal() == IDCANCEL)
								return true;
						}
					}

					AreaTraits::ValueType &newArea = areas[area->AID];
					// 选中的区域被设置为本服务器优先
					utility::Strcpy(newArea->SvrID, svrID);

					if( !i8desk::GetDataMgr().ModifyData(areas, newArea->AID, newArea, MASK_TAREA_SVRID) )
						return false;
				}
				else
				{
					if( area->SvrID == svrID )
					{
						// 没选中的如果原先的设置为本服务器，则清除
						AreaTraits::ValueType &newArea = areas[area->AID];
						utility::Strcpy(newArea->SvrID, _T(""));

						if( !i8desk::GetDataMgr().ModifyData(areas, newArea->AID, newArea, MASK_TAREA_SVRID) )
							return false;
					}
				}

				return true;
			}


			bool IsMainServerIP(const ulong svrip)
			{
				if(svrip == i8desk::String2IP(_T("127.0.0.1")))
				 	return true;

				using i8desk::data_helper::ServerTraits;
				const ServerTraits::MapType &servers = i8desk::GetDataMgr().GetServers();

				for( i8desk::data_helper::ServerTraits::MapConstIterator iter = servers.begin();
					 iter != servers.end(); ++iter )
				{
				/*	if( iter->second->SvrType == i8desk::MainServer )
						if( iter->second->SvrIP == svrip )
							return true;*/
				}

				return false;
			}


			bool IsServerExsit(const stdex::tString &SID)
			{
				const i8desk::data_helper::ServerTraits::MapType &servers = i8desk::GetDataMgr().GetServers();
				return servers.find(SID) != servers.end();
			}

			stdex::tString GetServerIDBySvrIP(ulong ip)
			{
				using i8desk::data_helper::ServerTraits;
				const ServerTraits::MapType &servers = i8desk::GetDataMgr().GetServers();
				for(ServerTraits::MapConstIterator iter = servers.begin(); iter != servers.end(); ++iter )
				{
					if( iter->second->SvrIP == ip || iter->second->Ip1 == ip ||
						iter->second->Ip2 == ip || iter->second->Ip3 == ip ||iter->second->Ip4 == ip)
						return iter->second->SvrID;
					
				}

				return _T("");
			}


			bool Delete(const stdex::tString &SID, const std::tr1::function<void(LPCTSTR, LPCTSTR)> &error)
			{
				using namespace i8desk::data_helper;

				// 删除服务器
				ServerTraits::MapType &servers = GetDataMgr().GetServers();
				if( IsMainServer(servers[SID]) )
				{
					error(_T("提示"), _T("不能删除主服务器"));
					return false;
				}
				
				if( !GetDataMgr().DelData(servers, SID) )
				{
					error(_T("提示"), _T("删除服务器出错"));
					return false;
				}

				// 删除同步任务
				std::vector<stdex::tString> syncTasksSID;
				SyncTaskTraits::MapType &syncTasks = GetDataMgr().GetSyncTasks();
				for(SyncTaskTraits::MapConstIterator iter = syncTasks.begin(); 
					iter != syncTasks.end(); ++iter)
				{
					if( iter->second->SvrID == SID )
					{
						syncTasksSID.push_back(iter->second->SID);
					}
				}

				for(size_t i = 0; i != syncTasksSID.size(); ++i)
				{
					if( !GetDataMgr().DelData(syncTasks, syncTasksSID[i]) )
					{
						error(_T("提示"), _T("删除服务器所属同步任务出错"));
						break;
					}
				}

				// 删除虚拟盘
				std::vector<stdex::tString> vDisksSID;
				VDiskTraits::MapType &vDisks = GetDataMgr().GetVDisks();
				for(VDiskTraits::MapConstIterator iter = vDisks.begin();
					iter != vDisks.end(); ++iter)
				{
					if( iter->second->SvrID == SID )
					{
						vDisksSID.push_back(iter->second->VID);
					}
				}
				for(size_t i = 0; i != vDisksSID.size(); ++i)
				{
					if( !GetDataMgr().DelData(vDisks, vDisksSID[i]) )
					{
						error(_T("提示"), _T("删除服务器所属虚拟盘出错"));
						break;
					}
				}

				return true;
			}

			void FormatIP(const stdex::tString &svrID, stdex::tString &text)
			{
				const data_helper::ServerTraits::ValueType &val = GetServiceBySID(svrID);
				stdex::tOstringstream os;
				os << _T("默认IP:") << i8desk::IP2String(val->SvrIP) << _T("/ ");

				if( val->Ip1 != 0 )
					os << _T("IP1:") << i8desk::IP2String(val->Ip1) << _T("/ ");
				if( val->Ip2 != 0 )
					os << _T("IP2:") << i8desk::IP2String(val->Ip2) << _T("/ ");
				if( val->Ip3 != 0 )
					os << _T("IP3:") << i8desk::IP2String(val->Ip3) << _T("/ ");
				if( val->Ip4 != 0 )
					os << _T("IP4:") << i8desk::IP2String(val->Ip4) << _T("/ ");

				text = os.str();
			}

			void FormatDisk(const stdex::tString &svrID, stdex::tString &text)
			{
				stdex::tOstringstream os;
				data_helper::DiskStatusTraits::MapType diskStatus = GetRealDataMgr().GetDiskStatus();
				for(data_helper::DiskStatusTraits::MapConstIterator iter = diskStatus.begin();
					iter != diskStatus.end(); ++iter)
				{
					if( iter->second->Type == DRIVE_FIXED &&
						iter->second->SvrID == svrID )
					{
						stdex::tString total, free;
						i8desk::FormatSize(iter->second->Capacity, total);
						i8desk::FormatSize(iter->second->FreeSize, free);

						os << (TCHAR)iter->second->Partition << _T(": ")
							<< free << _T("/") << total << _T("  ");
					}
				}

				text = os.str();
			}

			namespace detail
			{
				struct
				{
					int type_;
					LPCTSTR desc_;
				}balanceType[] = 
				{
					{ BALANCE_UNKNOWN,		_T("未设置") },
					{ BALANCE_POLL,			_T("轮询分配") },
					{ BALANCE_AREAPRIORITY, _T("区域优先") }
				};
			}
			void GetBalanceTypes(const std::tr1::function<void(int, LPCTSTR)> &callback)
			{
				for(size_t i = 0; i != _countof(detail::balanceType); ++i)
					callback(detail::balanceType[i].type_, detail::balanceType[i].desc_);
			}

			LPCTSTR GetBalanceTypeDescByType(int type)
			{
				for(size_t i = 0; i != _countof(detail::balanceType); ++i)
				{
					if( detail::balanceType[i].type_ == type )
						return detail::balanceType[i].desc_;
				}

				return _T("");
			}

			namespace detail
			{
				struct
				{
					int type_;
					LPCTSTR desc_;
				}SvrType[] = 
				{
					{ i8desk::OtherServer,		_T("从服务") },
					{ i8desk::MainServer,		_T("主服务") },
				};
			}
			void GetSvrTypes(const std::tr1::function<void(int, LPCTSTR)> &callback)
			{
				for(size_t i = 0; i != _countof(detail::SvrType); ++i)
					callback(detail::SvrType[i].type_, detail::SvrType[i].desc_);
			}

			LPCTSTR GetSvrTypeDescByType(int type)
			{
				for(size_t i = 0; i != _countof(detail::SvrType); ++i)
				{
					if( detail::SvrType[i].type_ == type )
						return detail::SvrType[i].desc_;
				}

				return _T("");
			}

			

			void GetSvrAreaPriotyDescBySID(const stdex::tString &svrID, stdex::tString &text)
			{
				using i8desk::data_helper::AreaTraits;
				AreaTraits::MapType &areas = GetDataMgr().GetAreas();

				stdex::tOstringstream os;

				for(AreaTraits::MapConstIterator iter = areas.begin(); iter != areas.end(); ++iter)
				{
					if(utility::Strcmp(iter->second->SvrID, svrID.c_str()) == 0)
						os << iter->second->Name << _T("   ");
				}

				text = os.str();
			}

			stdex::tString GetNameBySID(const stdex::tString &sid)
			{
				using i8desk::data_helper::ServerTraits;
				ServerTraits::MapType &servers = GetDataMgr().GetServers();

				stdex::tString Name;
				ServerTraits::MapConstIterator iter = servers.find(sid.c_str());
				if(iter != servers.end())
					Name = iter->second->SvrName;

				return Name;

			}

		}
	}
}
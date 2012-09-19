#ifndef __DATA_MANAGER_HELPER_HPP
#define __DATA_MANAGER_HELPER_HPP


#include "../../../../include/frame.h"
#include "../../../../include/tablestruct.h"
#include "CustomDataType.h"
#include "../NetworkMgr.h"
#include "../Misc.h"

#include <vector>
#include <map>


namespace i8desk
{
	namespace data_helper
	{

		namespace detail
		{
			template< typename KeyT, typename ValueT, size_t Mask, typename LessT = std::less<KeyT> >
			struct DeclareType
			{
				enum { MASK = Mask };

				typedef typename ValueT::element_type		ElementType;
				typedef KeyT								KeyType;
				typedef ValueT								ValueType;

				typedef std::vector<ValueType>				VectorType;
				typedef typename VectorType::iterator		VecIterator;
				typedef typename VectorType::const_iterator	VecConstIterator;

				typedef std::map<KeyType, ValueType, LessT>	MapType;
				typedef typename MapType::iterator			MapIterator;
				typedef typename MapType::const_iterator	MapConstIterator;
			};
		}


		typedef detail::DeclareType<stdex::tString, db::TVDiskPtr,			FLAG_VDISK_BIT>			VDiskTraits;
		typedef detail::DeclareType<std::pair<stdex::tString, uint32>, db::TCustomRunTypePtr,		FLAG_RUNTYPE_BIT>		RunTypeTraits;
		typedef detail::DeclareType<uint32,			db::TCustomGamePtr,		FLAG_GAME_BIT>			GameTraits;
		typedef detail::DeclareType<stdex::tString, db::TAreaPtr,			FLAG_AREA_BIT, db::AreaLess> AreaTraits;
		typedef detail::DeclareType<stdex::tString, db::TCustomClassPtr,	FLAG_CLASS_BIT>			ClassTraits;
		typedef detail::DeclareType<stdex::tString, db::TClientPtr,			FLAG_CLIENT_BIT>		ClientTraits;
		typedef detail::DeclareType<std::pair<stdex::tString, stdex::tString>, db::TBootTaskAreaPtr,	FLAG_BOOTTASKAREA_BIT>	BootTaskAreaTraits;
		typedef detail::DeclareType<stdex::tString, db::TCmpBootTaskPtr,	FLAG_CMPBOOTTASK_BIT>	BootTaskTraits;
		typedef detail::DeclareType<stdex::tString, db::TPushGamePtr,		FLAG_CMPBOOTTASK_BIT>	PushGameTraits;
		typedef detail::DeclareType<stdex::tString, db::TFavoritePtr,		FLAG_FAVORITE_BIT>		FavoriteTraits;
		typedef detail::DeclareType<stdex::tString, db::TSysOptPtr,			FLAG_SYSOPT_BIT>		SysOptTraits;
		typedef detail::DeclareType<stdex::tString, db::TBarSkinPtr, 0>								BarSkinTraits;
		typedef detail::DeclareType<stdex::tString, db::TSyncTaskPtr,		FLAG_SYNCTASK_BIT>		SyncTaskTraits;
		typedef detail::DeclareType<stdex::tString, db::TServerPtr,			FLAG_SERVER_BIT>		ServerTraits;
		typedef detail::DeclareType<ulong,			db::TCustomPlugToolPtr,	FLAG_PLUGTOOL_BIT>		PlugToolTraits;
		typedef detail::DeclareType<std::pair<stdex::tString, uint32>, db::TSyncGamePtr,	FLAG_SYNCGAME_BIT>		SyncGameTraits;


		typedef detail::DeclareType<stdex::tString,	db::TClientStatusPtr,	FLAG_CLIENT_BIT>	ClientStatusTraits;
		typedef detail::DeclareType<stdex::tString, db::TServerStatusPtr,	FLAG_SERVERSTATUS_BIT>	ServerStatusTraits;
		typedef detail::DeclareType<std::pair<stdex::tString, uint32>, db::TDiskStatusPtr,		FLAG_DISKSTATUS_BIT>	DiskStatusTraits;
		typedef detail::DeclareType<ulong,			db::TVDiskClientPtr,	FLAG_VDISKCLIENT_BIT>	VDiskClientTraits;
		typedef detail::DeclareType<ulong,			db::TTaskStatusPtr,		FLAG_TASKSTATUS_BIT>	TaskStatusTraits;
		typedef detail::DeclareType<ulong,			db::TPlugToolStatusPtr,	FLAG_PLUGTOOLSTATUS_BIT> PlugToolStatusTraits;
		typedef detail::DeclareType<ulong,			db::TUpdateGameStatusPtr,FLAG_UPDATEGAMESTATUS_BIT> UpdateGameStatusTraits;
		typedef detail::DeclareType<std::pair<stdex::tString, uint32>,	db::TSyncTaskStatusPtr,	FLAG_SYNCTASKSTATUS_BIT>	SyncTaskStatusTraits;
		typedef detail::DeclareType<ulong,			db::TUpdateGameStatusPtr,FLAG_PUSHGAMESTATUS_BIT> PushGameStatusTraits;

		typedef detail::DeclareType<stdex::tString, db::TViewGamePtr,		0>						ViewGameTraits;
		typedef detail::DeclareType<stdex::tString, db::TViewClientPtr,		0>						ViewClientTraits;

		typedef detail::DeclareType<int,			db::TCustomMonitorPtr,	0>						MonitorStatusTraits;
		typedef detail::DeclareType<int,			db::TCustomStaticsPtr,	0>						StaticsStatusTraits;
		

		// --------------------------------
		// 根据Value信息得到Traits

		template < typename ValueT >
		struct Value2Traits
		{
			~Value2Traits()
			{
				char staticAssert[-1];
				(static_cast<char[sizeof(staticAssert)]>(staticAssert));
			}
			typedef void				TraitsType;	// error
		};

		template <>
		struct Value2Traits<db::TVDiskPtr>
		{
			typedef VDiskTraits		TraitsType;
		};

		template <>
		struct Value2Traits<db::TCustomRunTypePtr>
		{
			typedef RunTypeTraits	TraitsType;
		};

		template <>
		struct Value2Traits<db::TCustomGamePtr>
		{
			typedef GameTraits		TraitsType;
		};

		template <>
		struct Value2Traits<db::TAreaPtr>
		{
			typedef AreaTraits		TraitsType;
		};

		template <>
		struct Value2Traits<db::TCustomClassPtr>
		{
			typedef ClassTraits		TraitsType;
		};

		template <>
		struct Value2Traits<db::TClientPtr>
		{
			typedef ClientTraits	TraitsType;
		};

		template <>
		struct Value2Traits<db::TBootTaskAreaPtr>
		{
			typedef BootTaskAreaTraits	TraitsType;
		};

		template <>
		struct Value2Traits<db::TCmpBootTaskPtr>
		{
			typedef BootTaskTraits	TraitsType;
		};

		template <>
		struct Value2Traits<db::TPushGamePtr>
		{
			typedef PushGameTraits	TraitsType;
		};


		template <>
		struct Value2Traits<db::TFavoritePtr>
		{
			typedef FavoriteTraits	TraitsType;
		};

		template <>
		struct Value2Traits<db::TBarSkinPtr>
		{
			typedef BarSkinTraits	TraitsType;
		};

		template <>
		struct Value2Traits<db::TViewGamePtr>
		{
			typedef ViewGameTraits	TraitsType;
		};

		template <>
		struct Value2Traits<db::TViewClientPtr>
		{
			typedef ViewClientTraits	TraitsType;
		};

		template <>
		struct Value2Traits<db::TSyncTaskPtr>
		{
			typedef SyncTaskTraits	TraitsType;
		};

		template <>
		struct Value2Traits<db::TSysOptPtr>
		{
			typedef SysOptTraits	TraitsType;
		};

		template <>
		struct Value2Traits<db::TServerPtr>
		{
			typedef ServerTraits	TraitsType;
		};


		template <>
		struct Value2Traits<db::TCustomPlugToolPtr>
		{
			typedef PlugToolTraits	TraitsType;
		};

		template <>
		struct Value2Traits<db::TClientStatusPtr>
		{
			typedef ClientStatusTraits	TraitsType;
		};

		template <>
		struct Value2Traits<db::TServerStatusPtr>
		{
			typedef ServerStatusTraits	TraitsType;
		};

		template <>
		struct Value2Traits<db::TDiskStatusPtr>
		{
			typedef DiskStatusTraits	TraitsType;
		};

		template <>
		struct Value2Traits<db::TVDiskClientPtr>
		{
			typedef VDiskClientTraits	TraitsType;
		};

		
		template <>
		struct Value2Traits<db::TSyncGamePtr>
		{
			typedef SyncGameTraits		TraitsType;
		};

		template <>
		struct Value2Traits<db::TSyncTaskStatusPtr>
		{
			typedef SyncTaskStatusTraits		TraitsType;
		};

		// ------------------------------------
		// GetAllData Traits
		template < typename TraitsT >
		struct GetAllDataTraits
		{
			template < typename NetworkT >
			static Buffer GetAllData(NetworkT *network)
			{
				return ExecuteSQL(network, TraitsT::MASK);
			}
		};

		template <>
		struct GetAllDataTraits<PushGameTraits>
		{
			template < typename NetworkT >
			static Buffer GetAllData(NetworkT *network)
			{
				return ExecuteSQL(network, PushGameTraits::MASK, SQL_COND_MODE_EQUAL, MASK_TCMPBOOTTASK_TYPE, brUpdateGame);
			}
		};

		template <>
		struct GetAllDataTraits<BootTaskTraits>
		{
			template < typename NetworkT >
			static Buffer GetAllData(NetworkT *network)
			{
				return ExecuteSQL(network, BootTaskTraits::MASK, SQL_COND_MODE_NEQUAL, MASK_TCMPBOOTTASK_TYPE, brUpdateGame);
			}
		};


		template <>
		struct GetAllDataTraits<ClientStatusTraits>
		{
			template < typename NetworkT >
			static Buffer GetAllData(NetworkT *network)
			{
				return ExecuteSQL(network, ClientStatusTraits::MASK, SQL_COND_MODE_EQUAL, MASK_TCLIENT_ONLINE, Online);
			}
		};

		template <>
		struct GetAllDataTraits<SyncTaskStatusTraits>
		{
			template < typename NetworkT >
			static Buffer GetAllData(NetworkT *network, const stdex::tString &SID)
			{
				return ExecuteSQL(network, SyncTaskStatusTraits::MASK, SQL_COND_MODE_EQUAL, MASK_TSYNCTASKSTATUS_NAMEID, 0, 0, SID);
			}
		};


		// -------------------------------------
		// InsertData Traits

		namespace detail
		{
			template < typename MapT, typename KeyT, typename ValueT >
			void PutValue(MapT &values, const KeyT &key, const ValueT &val)
			{
				typedef typename MapT::value_type value_type;
				values.insert(value_type(key, val));
			}

			template < typename VectorT, typename ValueT >
			void PutValue(VectorT &values, const ValueT &val)
			{
				values.push_back(val);
			}
		}

		template < typename Traits >
		struct GetAllTraits
		{};

		template <>
		struct GetAllTraits<VDiskTraits>
		{
			template <typename InT, typename ValuesT, typename T>
			static void GetAllData(InT &in, ValuesT &values, const T &val)
			{
				in >> *val;
				detail::PutValue(values, val->VID, val);
			}
		};

		template <>
		struct GetAllTraits<AreaTraits>
		{
			template <typename InT, typename ValuesT, typename T>
			static void GetAllData(InT &in, ValuesT &values, const T &val)
			{
				in >> *val;
				detail::PutValue(values, val->AID, val);
			}
		};

		template <>
		struct GetAllTraits<ClientTraits>
		{
			template <typename InT, typename ValuesT, typename T>
			static void GetAllData(InT &in, ValuesT &values, const T &val)
			{
				in >> *val;
				detail::PutValue(values, val->Name, val);
			}
		};

		template <>
		struct GetAllTraits<ClassTraits>
		{
			template <typename InT, typename ValuesT, typename T>
			static void GetAllData(InT &in, ValuesT &values, const T &val)
			{
				in >> *(db::tClass *)&*val;
				detail::PutValue(values, val->CID, val);
			}
		};

		template <>
		struct GetAllTraits<SyncTaskTraits>
		{
			template <typename InT, typename ValuesT, typename T>
			static void GetAllData(InT &in, ValuesT &values, const T &val)
			{
				in >> *val;
				detail::PutValue(values, val->SID, val);
			}
		};

		template <>
		struct GetAllTraits<FavoriteTraits>
		{
			template <typename InT, typename ValuesT, typename T>
			static void GetAllData(InT &in, ValuesT &values, const T &val)
			{
				in >> *val;
				detail::PutValue(values, val->FID, val);
			}
		};

		template <>
		struct GetAllTraits<BootTaskAreaTraits>
		{
			template <typename InT, typename ValuesT, typename T>
			static void GetAllData(InT &in, ValuesT &values, const T &val)
			{
				in >> *val;
				detail::PutValue(values, std::make_pair(val->TID, val->AID), val);
			}
		};

		template <>
		struct GetAllTraits<BootTaskTraits>
		{
			template <typename InT, typename ValuesT, typename T>
			static void GetAllData(InT &in, ValuesT &values, const T &val)
			{
				in >> *val;
				detail::PutValue(values, val->TID, val);
			}
		};

		template <>
		struct GetAllTraits<PushGameTraits>
		{
			template <typename InT, typename ValuesT, typename T>
			static void GetAllData(InT &in, ValuesT &values, const T &val)
			{
				in >> *val;
				detail::PutValue(values, val->TID, val);
			}
		};

		template <>
		struct GetAllTraits<SysOptTraits>
		{
			template <typename InT, typename ValuesT, typename T>
			static void GetAllData(InT &in, ValuesT &values, const T &val)
			{
				in >> *val;
				detail::PutValue(values, val->key, val);
			}
		};

		template <>
		struct GetAllTraits<ServerTraits>
		{
			template <typename InT, typename ValuesT, typename T>
			static void GetAllData(InT &in, ValuesT &values, const T &val)
			{
				in >> *val;
				detail::PutValue(values, val->SvrID, val);
			}
		};

		template <>
		struct GetAllTraits<RunTypeTraits>
		{
			template <typename InT, typename ValuesT, typename T>
			static void GetAllData(InT &in, ValuesT &values, const T &val)
			{
				in >> *(db::tRunType *)&*val;
				detail::PutValue(values, std::make_pair(val->AID, val->GID), val);
			}
		};

		template <>
		struct GetAllTraits<GameTraits>
		{
			template <typename InT, typename ValuesT, typename T>
			static void GetAllData(InT &in, ValuesT &values, const T &val)
			{
				in  >> val->GID			>> val->PID			>> val->CID			>> val->Name		
					>> val->Exe			>> val->Param		>> val->Size		>> val->SvrPath		
					>> val->CliPath		>> val->TraitFile	>> val->SaveFilter	>> val->DeskLink 
					>> val->Toolbar		>> val->Memo		>> val->GameSource	>> val->IdcAddDate
					>> val->EnDel		>> val->IdcVer		>> val->SvrVer		>> val->IdcClick
					>> val->SvrClick	>> val->SvrClick2	>> val->I8Play		>> val->AutoUpt		
					>> val->Priority	>> val->Force		>> val->ForceDir	>> val->Status		
					>> val->StopRun;

				detail::PutValue(values, val->GID, val);
			}
		};

		template <>
		struct GetAllTraits<PlugToolTraits>
		{
			template <typename InT, typename ValuesT, typename T>
			static void GetAllData(InT &in, ValuesT &values, const T &val)
			{
				in >> *(db::tPlugTool *)&*val;
				detail::PutValue(values, val);
			}
		};


		template <>
		struct GetAllTraits<ClientStatusTraits>
		{
			template <typename InT, typename ValuesT, typename T>
			static void GetAllData(InT &in, ValuesT &values, const T &val)
			{
				in >> *val;
				detail::PutValue(values, val->Name, val);
			}
		};

		template <>
		struct GetAllTraits<ServerStatusTraits>
		{
			template <typename InT, typename ValuesT, typename T>
			static void GetAllData(InT &in, ValuesT &values, const T &val)
			{
				in >> *val;
				detail::PutValue(values, val->SvrID, val);
			}
		};

		template <>
		struct GetAllTraits<DiskStatusTraits>
		{
			template <typename InT, typename ValuesT, typename T>
			static void GetAllData(InT &in, ValuesT &values, const T &val)
			{
				in >> *val;
				detail::PutValue(values, std::make_pair(val->SvrID, val->Partition), val);
			}
		};

		template <>
		struct GetAllTraits<VDiskClientTraits>
		{
			template <typename InT, typename ValuesT, typename T>
			static void GetAllData(InT &in, ValuesT &values, const T &val)
			{
				in >> *val;
				detail::PutValue(values, val->ClientIP, val);
			}
		};

		template <>
		struct GetAllTraits<TaskStatusTraits>
		{
			template <typename InT, typename ValuesT, typename T>
			static void GetAllData(InT &in, ValuesT &values, const T &val)
			{
				in >> *val;
				detail::PutValue(values, val);
			}
		};

		template <>
		struct GetAllTraits<UpdateGameStatusTraits>
		{
			template <typename InT, typename ValuesT, typename T>
			static void GetAllData(InT &in, ValuesT &values, const T &val)
			{
				in >> *val;
				detail::PutValue(values, val->ClientIP, val);
			}
		};

		template <>
		struct GetAllTraits<PushGameStatusTraits>
		{
			template <typename InT, typename ValuesT, typename T>
			static void GetAllData(InT &in, ValuesT &values, const T &val)
			{
				in >> *val;
				detail::PutValue(values, val->ClientIP, val);
			}
		};

		template <>
		struct GetAllTraits<PlugToolStatusTraits>
		{
			template <typename InT, typename ValuesT, typename T>
			static void GetAllData(InT &in, ValuesT &values, const T &val)
			{
				in >> *val;
				detail::PutValue(values, val);
			}
		};

		template <>
		struct GetAllTraits<SyncGameTraits>
		{
			template <typename InT, typename ValuesT, typename T>
			static void GetAllData(InT &in, ValuesT &values, const T &val)
			{
				in >> *(db::tSyncGame *)&*val;
				detail::PutValue(values, std::make_pair(val->SID, val->GID), val);
			}
		};

		template <>
		struct GetAllTraits<SyncTaskStatusTraits>
		{
			template <typename InT, typename ValuesT, typename T>
			static void GetAllData(InT &in, ValuesT &values, const T &val)
			{
				in >> *(db::tSyncTaskStatus *)&*val;
				detail::PutValue(values, std::make_pair(val->SID, val->GID), val);
			}
		};


		// ---------------------------------
		// GetDataTraits

		template < typename TraitsT >
		struct GetDataTraits
		{
			typedef TraitsT								TraistType;
			typedef typename TraistType::ValueType		ValueType;

			template < typename NetworkT >
			static Buffer GetData(NetworkT *network, const ValueType &val)
			{
				char staticAssert[-1];
				(static_cast<char[sizeof(staticAssert)]>(staticAssert));
			}
		};

		template <>
		struct GetDataTraits<ClientTraits>
		{
			typedef ClientTraits						TraistType;
			typedef TraistType::ValueType				ValueType;

			template < typename NetworkT >
			static Buffer GetData(NetworkT *network, const ValueType &val)
			{
				return ExecuteSQL(network, TraistType::MASK, SQL_COND_MODE_KEY, 0, 0, 0, val->Name);
			}
		};

		template <>
		struct GetDataTraits<SysOptTraits>
		{
			typedef SysOptTraits						TraistType;
			typedef TraistType::ValueType				ValueType;

			template < typename NetworkT >
			static Buffer GetData(NetworkT *network, const ValueType &val)
			{
				return ExecuteSQL(network, TraistType::MASK, SQL_COND_MODE_KEY, 0, 0, 0, val->key);
			}
		};


		template <>
		struct GetDataTraits<GameTraits>
		{
			typedef GameTraits							TraistType;
			typedef TraistType::ValueType				ValueType;

			template < typename NetworkT >
			static Buffer GetData(NetworkT *network, const ValueType &val)
			{
				return ExecuteSQL(network, TraistType::MASK, SQL_COND_MODE_KEY, 0, val->GID);
			}
		};

		template <>
		struct GetDataTraits<BootTaskAreaTraits>
		{
			typedef BootTaskAreaTraits					TraistType;
			typedef TraistType::ValueType				ValueType;

			template < typename NetworkT >
			static Buffer GetData(NetworkT *network, const ValueType &val)
			{
				return ExecuteSQL(network, TraistType::MASK, SQL_COND_MODE_KEY, 0, 0, 0, val->TID, val->AID);
			}
		};


		template <>
		struct GetDataTraits<MonitorStatusTraits>
		{
			typedef MonitorStatusTraits						TraistType;
			typedef TraistType::ValueType				ValueType;

			template < typename NetworkT >
			static Buffer GetData(NetworkT *network, const ValueType &val)
			{
				char buf[512] = {0};
				CPkgHelper out(buf, CMD_CON_MONITOR_STATUS, OBJECT_CONSOLE);
				return ExecuteMsg(network, out.GetBuffer(), out.GetLength());
			}
		};

		template <>
		struct GetDataTraits<StaticsStatusTraits>
		{
			typedef StaticsStatusTraits						TraistType;
			typedef TraistType::ValueType				ValueType;

			template < typename NetworkT >
			static Buffer GetData(NetworkT *network, const ValueType &val)
			{
				char buf[512] = {0};
				CPkgHelper out(buf, CMD_CON_STATISTIC_STATUS, OBJECT_CONSOLE);
				return ExecuteMsg(network, out.GetBuffer(), out.GetLength());
			}
		};


		// ----------------------------------
		// Get Traits

		template < typename TraitsT >
		struct GetTraits
		{
			typedef TraitsT								TraistType;
			typedef typename TraistType::ValueType		ValueType;

			template < typename InT >
			static void GetData(InT &in, ValueType &val)
			{
				in >> *val;
			}
		};

		template <>
		struct GetTraits<GameTraits>
		{
			typedef GameTraits					TraistType;
			typedef TraistType::ValueType		ValueType;

			template < typename InT >
			static void GetData(InT &in, ValueType &val)
			{
				in >> *(db::tGame *)&*val;
			}
		};

		template <>
		struct GetTraits<MonitorStatusTraits>
		{
			typedef MonitorStatusTraits				TraistType;
			typedef TraistType::ValueType		ValueType;

			template < typename InT >
			static void GetData(InT &in, ValueType &val)
			{
				size_t cnt = 0;
				in >> cnt;

				val->ServersInfo.clear();
				for(size_t i = 0; i != cnt; ++i)
				{
					typedef typename MonitorStatusTraits::ElementType::tServerPtr tServerPtr;
					tServerPtr serverInfo(new tServerPtr::element_type);
					in >> serverInfo->SvrName >> serverInfo->SvrType >> serverInfo->Status >> serverInfo->ClientConnects >>serverInfo->Connects >> 
						serverInfo->TotalSpeed >> serverInfo->TotalReadData;

					val->ServersInfo.push_back(serverInfo);
				}

				in >> val->DownloadResInfo.Status >> val->DownloadResInfo.DownlaodingCount >>
					val->DownloadResInfo.TotalSpeed >> val->DownloadResInfo.QueueCount >>
					val->DownloadResInfo.TotalConnect >> val->DownloadResInfo.TmpSize >>
				val->DownloadResInfo.LimitSpeed >> val->DownloadResInfo.CPUUsage;
				in >> val->UpdateInfo.TotalClients >> val->UpdateInfo.TotalSpeed;
				in >> val->SyncTaskInfo.TotalClients >> val->SyncTaskInfo.TotalSpeed;
			}
		};

		template <>
		struct GetTraits<StaticsStatusTraits>
		{
			typedef StaticsStatusTraits			TraistType;
			typedef TraistType::ValueType		ValueType;

			template < typename InT >
			static void GetData(InT &in, ValueType &val)
			{
				in >> val->IdcCount >> val->IdcSize >> val->LocalCount >> val->LocalSize 
					>> val->OnlineClients >> val->TotalClients >> val->LogState;
			}
		};


		// ----------------------------------
		// Ensure Traits

		template < typename TraitsT >
		struct EnsureTraits
		{
			typedef TraitsT								TraistType;
			typedef typename TraistType::ElementType	ElementType;
			typedef typename TraistType::ValueType		ValueType;
			typedef typename TraistType::KeyType		KeyType;

			template < typename NetworkT >
			static void EnsureData(NetworkT *network, const stdex::tString &key, const ValueType &val, uint64 mask)
			{
				data_helper::ExecuteSQL(network, SQL_OP_ENSURE, TraistType::MASK, 
					val.get(), SQL_COND_MODE_KEY, 0, 0, 0, key, _T(""), mask);
			}

			template < typename NetworkT >
			static void EnsureData(NetworkT *network, const DWORD &key, const ValueType &val, uint64 mask)
			{
				data_helper::ExecuteSQL(network, SQL_OP_ENSURE, TraistType::MASK, 
					val.get(), SQL_COND_MODE_KEY, 0, key, 0, _T(""), _T(""), mask);
			}
		};

		template <>
		struct EnsureTraits<RunTypeTraits>
		{
			typedef RunTypeTraits				TraistType;
			typedef TraistType::ElementType		ElementType;
			typedef TraistType::ValueType		ValueType;
			typedef TraistType::KeyType			KeyType;

			template < typename NetworkT >
			static void EnsureData(NetworkT *network, const KeyType &key, const ValueType &val, uint64 mask)
			{
				data_helper::ExecuteSQL(network, SQL_OP_ENSURE, TraistType::MASK, 
					val.get(), SQL_COND_MODE_KEY, 0, key.second, 0, key.first, _T(""), mask);
			}
		};

		template <>
		struct EnsureTraits<SyncGameTraits>
		{
			typedef SyncGameTraits				TraistType;
			typedef TraistType::ElementType		ElementType;
			typedef TraistType::ValueType		ValueType;
			typedef TraistType::KeyType			KeyType;

			template < typename NetworkT >
			static void EnsureData(NetworkT *network, const KeyType &key, const ValueType &val, uint64 mask)
			{
				data_helper::ExecuteSQL(network, SQL_OP_ENSURE, TraistType::MASK, 
					val.get(), SQL_COND_MODE_KEY, 0, key.second, 0, key.first, _T(""), mask);
			}
		};

		template <>
		struct EnsureTraits<SyncTaskStatusTraits>
		{
			typedef SyncTaskStatusTraits		TraistType;
			typedef TraistType::ElementType		ElementType;
			typedef TraistType::ValueType		ValueType;
			typedef TraistType::KeyType			KeyType;

			template < typename NetworkT >
			static void EnsureData(NetworkT *network, const KeyType &key, const ValueType &val, uint64 mask)
			{
				data_helper::ExecuteSQL(network, SQL_OP_ENSURE, TraistType::MASK, 
					val.get(), SQL_COND_MODE_KEY, 0, key.second, 0, key.first, _T(""), mask);
			}
		};
		
		// ----------------------------------
		// Del Traits

		template < typename TraitsT >
		struct DelTraits
		{
			typedef TraitsT								TraistType;
			typedef typename TraistType::ElementType	ElementType;
			typedef typename TraistType::KeyType		KeyType;

			template < typename NetworkT >
			static void DelData(NetworkT *network, const stdex::tString &key)
			{
				data_helper::ExecuteSQL(network, SQL_OP_DELETE, TraistType::MASK, 
					reinterpret_cast<ElementType *>(0), SQL_COND_MODE_KEY, 0, 0, 0, key);
			}

			template < typename NetworkT >
			static void DelData(NetworkT *network, const DWORD &key)
			{
				ExecuteSQL(network, SQL_OP_DELETE, TraistType::MASK,
					reinterpret_cast<ElementType *>(0), SQL_COND_MODE_KEY, 0, key);
			}
		};

		template <>
		struct DelTraits<BootTaskAreaTraits>
		{
			typedef BootTaskAreaTraits			TraistType;
			typedef TraistType::ElementType		ElementType;
			typedef TraistType::KeyType			KeyType;

			template < typename NetworkT >
			static void DelData(NetworkT *network, const KeyType &key)
			{
				data_helper::ExecuteSQL(network, SQL_OP_DELETE, TraistType::MASK, 
					reinterpret_cast<ElementType *>(0), SQL_COND_MODE_KEY, 0, 0, 0, key.first, key.second);
			}
		};

		template <>
		struct DelTraits<SyncGameTraits>
		{
			typedef SyncGameTraits				TraistType;
			typedef TraistType::ElementType		ElementType;
			typedef TraistType::KeyType			KeyType;

			template < typename NetworkT >
			static void DelData(NetworkT *network, const KeyType &key)
			{
				data_helper::ExecuteSQL(network, SQL_OP_DELETE, TraistType::MASK, 
					reinterpret_cast<ElementType *>(0), SQL_COND_MODE_KEY, 0, key.second, 0, key.first);
			}
		};


		template < typename TraitsT >
		struct AddTraits
		{
			typedef TraitsT								TraistType;
			typedef typename TraistType::ValueType		ValueType;

			template < typename NetworkT >
			static void AddData(NetworkT *network, const ValueType &val)
			{
				data_helper::ExecuteSQL(network, SQL_OP_INSERT, TraistType::MASK, val.get());
			}
		};


		template < typename TraitsT >
		struct ModifyTraits
		{
			typedef TraitsT								TraistType;
			typedef typename TraistType::ElementType	ElementType;
			typedef typename TraistType::ValueType		ValueType;
			typedef typename TraistType::KeyType		KeyType;

			template < typename NetworkT >
			static void ModifyData(NetworkT *network, const stdex::tString &key, const ValueType &val, uint64 mask)
			{
				data_helper::ExecuteSQL(network, SQL_OP_UPDATE, TraistType::MASK, 
					val.get(), SQL_COND_MODE_KEY, 0, 0, 0, key, _T(""), mask);
			}

			template < typename NetworkT >
			static void ModifyData(NetworkT *network, const DWORD &key, const ValueType &val, uint64 mask)
			{
				data_helper::ExecuteSQL(network, SQL_OP_UPDATE, TraistType::MASK, 
					val.get(), SQL_COND_MODE_KEY, 0, key, 0, _T(""), _T(""), mask);
			}
		};


		namespace detail
		{
			// 获取完整表信息
			template < typename TraitsT, typename NetworkT, typename ValuesT >
			void GetAllData(NetworkT *network, ValuesT &values)
			{
				Buffer buf = GetAllDataTraits<TraitsT>::GetAllData(network);

				CPkgHelper in(buf.first.get(), buf.second);
				DWORD ret = 0, count = 0;
				in >> ret >> count;

				assert(ret != 0);
				values.clear();

				// 读取数据
				typedef typename TraitsT::ElementType	ElementType;
				typedef typename TraitsT::ValueType		ValueType;
				
				for(size_t i = 0; i != count; ++i)
				{
					ValueType val(new ElementType);
					
					GetAllTraits<TraitsT>::GetAllData(in, values, val);
				}
			}

			template < typename TraitsT, typename NetworkT, typename ValuesT, typename FilterT >
			void GetAllData(NetworkT *network, ValuesT &values, const FilterT &filter)
			{
				Buffer buf = GetAllDataTraits<TraitsT>::GetAllData(network, filter);

				CPkgHelper in(buf.first.get(), buf.second);
				DWORD ret = 0, count = 0;
				in >> ret >> count;

				assert(ret != 0);
				values.clear();

				// 读取数据
				typedef typename TraitsT::ElementType	ElementType;
				typedef typename TraitsT::ValueType		ValueType;

				for(size_t i = 0; i != count; ++i)
				{
					ValueType val(new ElementType);

					GetAllTraits<TraitsT>::GetAllData(in, values, val);
				}
			}



			template < typename NetworkT, typename TraitsT >
			void GetData(NetworkT *network, typename TraitsT::ValueType &val)
			{
				Buffer buf = GetDataTraits<TraitsT>::GetData(network, val);

				CPkgHelper in(buf.first.get(), buf.second);
				DWORD ret = 0, count = 0;
				in >> ret >> count;

				assert(ret != 0);
				if( count != 1 )
					throw exception::Businuss(_T("服务器没有该数据"));

				GetTraits<TraitsT>::GetData(in, val);
			}
		}



		// -----------------------------------------------
		// DataHandleTraits

		template < typename TraitsT >
		struct DataHandleTraits
		{
			typedef TraitsT								TraistType;
			typedef typename TraistType::ElementType	ElementType;
			typedef typename TraistType::KeyType		KeyType;
			typedef typename TraistType::ValueType		ValueType;


			template < typename NetworkT, typename ValuesT >
			static void GetAllData(NetworkT *network, ValuesT &values)
			{
				detail::GetAllData<TraistType>(network, values);
			}

			template < typename NetworkT, typename ValuesT, typename FilterT >
			static void GetAllData(NetworkT *network, ValuesT &values, const FilterT &filter)
			{
				detail::GetAllData<TraistType>(network, values, filter);
			}

			template < typename NetworkT >
			static void GetData(NetworkT *network, ValueType &val)
			{
				detail::GetData<NetworkT, TraistType>(network, val);
			}

			template < typename NetworkT >
			static void AddData(NetworkT *network, const ValueType &val)
			{
				AddTraits<TraistType>::AddData(network, val);
			}

			template < typename NetworkT >
			static void DelData(NetworkT *network, const KeyType &key)
			{
				DelTraits<TraistType>::DelData(network, key);
			}

			template < typename NetworkT >
			static void ModifyData(NetworkT *network, const KeyType &key, const ValueType &val, uint64 mask)
			{
				ModifyTraits<TraistType>::ModifyData(network, key, val, mask);
			}

			template < typename NetworkT >
			static void EnsureData(NetworkT *network, const KeyType &key, const ValueType &val, uint64 mask)
			{
				EnsureTraits<TraistType>::EnsureData(network, key, val, mask);
			}
		};

	}


	
}

#endif
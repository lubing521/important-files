#ifndef __CONSOLE_DATA_MGR_HPP
#define __CONSOLE_DATA_MGR_HPP


#include "DataHelper.h"

#include <type_traits>


namespace i8desk
{

	// forward declare
	class NetworkDataMgr;

	namespace manager
	{	

		// --------------------------------
		// class DataMgr

		// 数据管理
		class DataMgr
		{
			typedef std::tr1::function<void(const stdex::tString &)> ErrorCallback;
			typedef std::tr1::function<void(DWORD, DWORD, DWORD)>	UDPCallback;

			typedef NetworkDataMgr::AutoLock						AutoLock;

		private:
			NetworkDataMgr						*network_;

			data_helper::VDiskTraits::MapType	vDisk_;
			data_helper::ClientTraits::MapType	clients_;
			data_helper::AreaTraits::MapType	areas_;
			data_helper::GameTraits::MapType	games_;
			data_helper::ClassTraits::MapType	classes_;
			data_helper::RunTypeTraits::MapType runType_;
			data_helper::ServerTraits::MapType	servers_;
			data_helper::SyncTaskTraits::MapType syncTasks_;
			data_helper::BootTaskAreaTraits::MapType bootAreas_;
			data_helper::BootTaskTraits::MapType bootTasks_;
			data_helper::PushGameTraits::MapType pushGames_;
			data_helper::PlugToolTraits::VectorType plugTools_;
			data_helper::SyncGameTraits::MapType syncGames_;

			ErrorCallback						errorCallback_;

		public:
			explicit DataMgr(NetworkDataMgr *network);
			~DataMgr();

		private:
			DataMgr(const DataMgr &);
			DataMgr &operator=(const DataMgr &);

		public:
			void RegisterError(const ErrorCallback &errorCallback);


		public:
			data_helper::ClientTraits::MapType &GetClients();
			data_helper::AreaTraits::MapType &GetAreas();
			data_helper::GameTraits::MapType &GetGames();
			data_helper::ServerTraits::MapType &GetServers();
			data_helper::SyncTaskTraits::MapType &GetSyncTasks();
			data_helper::VDiskTraits::MapType &GetVDisks();
			data_helper::RunTypeTraits::MapType &GetRunTypes();
			data_helper::ClassTraits::MapType &GetClasses();
			data_helper::BootTaskTraits::MapType &GetBootTasks();
			data_helper::PushGameTraits::MapType &GetPushGames();
			data_helper::BootTaskAreaTraits::MapType &GetBootTaskAreas();
			data_helper::PlugToolTraits::VectorType &GetPlugTools();
			data_helper::SyncGameTraits::MapType &GetSyncGames();

		public:
			stdex::tString GetOptVal(const stdex::tString &key, const stdex::tString &defVal);
			int GetOptVal(const stdex::tString &key, int defVal);
			bool SetOptVal(const stdex::tString &key, const stdex::tString &val);
			bool SetOptVal(const stdex::tString &key, int val);

			bool GetBarSkin(data_helper::BarSkinTraits::VectorType &barSkins);
			bool SetBarSkin(const data_helper::BarSkinTraits::ValueType &barSkin);

		public:
			template < typename KeyT, typename ValueT, typename LessT >
			bool GetAllData(std::map<KeyT, ValueT, LessT> &values)
			{
				typedef ValueT value_type;
				typedef typename data_helper::Value2Traits<value_type>::TraitsType TraitsType;

				if( values.empty() )
				{
					try
					{
						AutoLock lock(network_->MutexData());
						data_helper::DataHandleTraits<TraitsType>::GetAllData(network_, values);
					}
					catch(exception::Base &e)
					{
						exception::detail::Translate(errorCallback_, e);
						return false;
					}
				}

				return true;
			}

			template < typename ValueT >
			bool GetAllData(std::vector<ValueT> &values)
			{
				typedef ValueT value_type;
				typedef typename data_helper::Value2Traits<value_type>::TraitsType TraitsType;

				if( values.empty() )
				{
					try
					{
						AutoLock lock(network_->MutexData());
						data_helper::DataHandleTraits<TraitsType>::GetAllData(network_, values);
					}
					catch(exception::Base &e)
					{
						exception::detail::Translate(errorCallback_, e);
						return false;
					}
				}

				return true;
			}

			template < typename KeyT, typename ValueT, typename LessT, typename FilterT >
			bool GetAllData(std::map<KeyT, ValueT, LessT> &values, const FilterT &filter)
			{
				typedef ValueT value_type;
				typedef typename data_helper::Value2Traits<value_type>::TraitsType TraitsType;

				if( values.empty() )
				{
					try
					{
						AutoLock lock(network_->MutexData());
						data_helper::DataHandleTraits<TraitsType>::GetAllData(network_, values, filter);
					}
					catch(exception::Base &e)
					{
						exception::detail::Translate(errorCallback_, e);
						return false;
					}
				}

				return true;
			}

			template < typename ValueT, typename FilterT >
			bool GetAllData(std::vector<ValueT> &values, const FilterT &filter)
			{
				typedef ValueT value_type;
				typedef typename data_helper::Value2Traits<value_type>::TraitsType TraitsType;

				if( values.empty() )
				{
					try
					{
						AutoLock lock(network_->MutexData());
						data_helper::DataHandleTraits<TraitsType>::GetAllData(network_, values, filter);
					}
					catch(exception::Base &e)
					{
						exception::detail::Translate(errorCallback_, e);
						return false;
					}
				}

				return true;
			}


			// --------------------------------------------
			template < typename ValueT >
			bool GetData(ValueT &val)
			{
				typedef typename data_helper::Value2Traits<ValueT>::TraitsType TraitsType;

				try
				{
					AutoLock lock(network_->MutexData());
					data_helper::DataHandleTraits<TraitsType>::GetData(network_, val);
				}
				catch(exception::Base &e)
				{
					exception::detail::Translate(errorCallback_, e);
					return false;
				}

				return true;
			}


			// ----------------------------------------
			template < typename KeyT, typename ValueT >
			bool EnsureData(std::map<KeyT, ValueT> &values, const KeyT &key, const ValueT &val, uint64 mask = ~0LL)
			{
				typedef ValueT value_type;
				typedef typename data_helper::Value2Traits<value_type>::TraitsType TraitsType;

				try
				{
					AutoLock lock(network_->MutexData());
					data_helper::DataHandleTraits<TraitsType>::EnsureData(network_, key, val, mask);

					values[key] = val;
				}
				catch(exception::Base &e)
				{
					exception::detail::Translate(errorCallback_, e);
					return false;
				}

				return true;
			}

			template < typename ValueT, typename KeyT >
			bool EnsureData(std::vector<ValueT> &values, const KeyT &key, const ValueT &val, uint64 mask = ~0LL)
			{
				typedef ValueT value_type;
				typedef typename data_helper::Value2Traits<value_type>::TraitsType TraitsType;

				try
				{
					AutoLock lock(network_->MutexData());
					data_helper::DataHandleTraits<TraitsType>::EnsureData(network_, key, val, mask);

					std::vector<ValueT>::iterator iter = values.find(key);
					if( iter != values.end() )
						*iter = val;
					else
						values.push_back(val);
				}
				catch(exception::Base &e)
				{
					exception::detail::Translate(errorCallback_, e);
					return false;
				}

				return true;
			}

			template < typename KeyT, typename ValueT >
			bool EnsureData(const KeyT &key, const ValueT &val)
			{
				typedef ValueT value_type;
				typedef typename data_helper::Value2Traits<value_type>::TraitsType TraitsType;

				return _EnsureData(key, val, TraitsType());
			}
			// for SysOpt
			template < typename KeyT, typename ValueT >
			bool _EnsureData(const KeyT &key, const ValueT &val, data_helper::SysOptTraits)
			{
				typedef ValueT value_type;
				typedef data_helper::SysOptTraits TraitsType;

				try
				{
					AutoLock lock(network_->MutexData());
					data_helper::DataHandleTraits<TraitsType>::EnsureData(network_, key, val, MASK_TSYSOPT_VALUE);
				}
				catch(exception::Base &e)
				{
					exception::detail::Translate(errorCallback_, e);
					return false;
				}

				return true;
			}

			template < typename KeyT, typename ValueT, typename TraistT >
			bool _EnsureData(const KeyT &key, const ValueT &val, TraistT)
			{
				typedef ValueT value_type;
				typedef typename data_helper::Value2Traits<value_type>::TraitsType TraitsType;

				try
				{
					AutoLock lock(network_->MutexData());
					data_helper::DataHandleTraits<TraitsType>::EnsureData(network_, key, val, ~0);
				}
				catch(exception::Base &e)
				{
					exception::detail::Translate(errorCallback_, e);
					return false;
				}

				return true;
			}

			// ----------------------------------------
			template < typename ValueT >
			bool AddData( const ValueT &val)
			{
				typedef ValueT	ValueType;
				typedef typename data_helper::Value2Traits<ValueType>::TraitsType TraitsType;

				try
				{
					AutoLock lock(network_->MutexData());
					data_helper::DataHandleTraits<TraitsType>::AddData(network_, val);
				}
				catch(exception::Base &e)
				{
					exception::detail::Translate(errorCallback_, e);
					return false;
				}

				return true;
			}

			template < typename KeyT, typename ValueT, typename LessT, typename KeyType >
			bool AddData(std::map<KeyT, ValueT, LessT> &container, const KeyType &key, const ValueT &val)
			{
				typedef ValueT	ValueType;
				typedef typename data_helper::Value2Traits<ValueType>::TraitsType TraitsType;

				try
				{
					AutoLock lock(network_->MutexData());
					data_helper::DataHandleTraits<TraitsType>::AddData(network_, val);

					container.insert(std::make_pair(key, val));
				}
				catch(exception::Base &e)
				{
					exception::detail::Translate(errorCallback_, e);
					return false;
				}

				return true;
			}

			template < typename ValueT, typename KeyT >
			bool AddData(std::vector<ValueT> &container, const KeyT &key, const ValueT &val)
			{
				typedef ValueT	ValueType;
				typedef typename data_helper::Value2Traits<ValueType>::TraitsType TraitsType;

				try
				{
					AutoLock lock(network_->MutexData());
					data_helper::DataHandleTraits<TraitsType>::AddData(network_, val);

					container.push_back(val);
				}
				catch(exception::Base &e)
				{
					exception::detail::Translate(errorCallback_, e);
					return false;
				}

				return true;
			}


			// ---------------------------------------------
			template < typename KeyT, typename ValueT, typename LessT, typename KeyType >
			bool DelData(std::map<KeyT, ValueT, LessT> &container, const KeyType &key)
			{
				typedef ValueT value_type;
				typedef typename data_helper::Value2Traits<value_type>::TraitsType TraitsType;

				try
				{
					AutoLock lock(network_->MutexData());
					data_helper::DataHandleTraits<TraitsType>::DelData(network_, key);

					container.erase(key);
				}
				catch(exception::Base &e)
				{
					exception::detail::Translate(errorCallback_, e);
					return false;
				}

				return true;
			}

			// --------------------------------------------
			template < typename ValueT, typename KeyT >
			bool DelData(std::vector<ValueT> &container, const KeyT &key)
			{
				typedef ValueT value_type;
				typedef typename data_helper::Value2Traits<value_type>::TraitsType TraitsType;

				try
				{
					AutoLock lock(network_->MutexData());
					data_helper::DataHandleTraits<TraitsType>::DelData(network_, key);

					container.erase(key);
				}
				catch(exception::Base &e)
				{
					exception::detail::Translate(errorCallback_, e);
					return false;
				}

				return true;
			}


			// ----------------------------------------------------
			template < typename KeyT, typename ValueT, typename LessT, typename KeyType >
			bool ModifyData(std::map<KeyT, ValueT, LessT> &container, const KeyType &key, const ValueT &val, uint64 mask = ~0LL)
			{
				typedef ValueT	value_type;
				typedef typename data_helper::Value2Traits<value_type>::TraitsType TraitsType;

				try
				{
					AutoLock lock(network_->MutexData());
					data_helper::DataHandleTraits<TraitsType>::ModifyData(network_, key, val, mask);

					std::map<KeyT, ValueT, LessT>::iterator iter = container.find(key);
					assert(iter != container.end());
					if( iter != container.end() ) 
						iter->second = val;
				}
				catch(exception::Base &e)
				{
					exception::detail::Translate(errorCallback_, e);
					return false;
				}

				return true;
			}

			template < typename KeyT, typename ValueT >
			bool ModifyData(std::vector<ValueT> &container, const KeyT &key, const ValueT &val, uint64 mask = ~0LL)
			{
				typedef ValueT	value_type;
				typedef typename data_helper::Value2Traits<value_type>::TraitsType TraitsType;

				try
				{
					AutoLock lock(network_->MutexData());
					data_helper::DataHandleTraits<TraitsType>::ModifyData(network_, key, val, mask);

					std::vector<ValueT>::iterator iter = container.find_if(std::equal_to<KeyT>(key));
					assert(iter != container.end());
					if( iter != container.end() ) 
						*iter = val;
				}
				catch(exception::Base &e)
				{
					exception::detail::Translate(errorCallback_, e);
					return false;
				}

				return true;
			}

		};
	}
}


#endif
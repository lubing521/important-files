#ifndef __CLIENT_DATA_MANAGER_HPP
#define __CLIENT_DATA_MANAGER_HPP


namespace i8desk
{
	namespace data
	{

		// ------------------------------
		// class DataMgr

		class DataMgr
		{		
			struct DataMgrImpl;
			std::auto_ptr<DataMgrImpl> impl_;
			
		public:
			DataMgr();

		private:
			DataMgr(const DataMgr &);
			DataMgr &operator=(const DataMgr &);

		public:
			template < typename T >
			const T &GetSysOpt(const stdex::tString &key, const T &val)
			{
				return impl_->GetSysOpt(key, val);
			}
		};
	}
}






#endif
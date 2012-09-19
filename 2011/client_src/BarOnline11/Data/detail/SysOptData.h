#ifndef __DATA_DETAIL_SYSOPT_HPP
#define __DATA_DETAIL_SYSOPT_HPP

#include <type_traits>
#include <map>

namespace i8desk
{
	namespace data
	{
		namespace detail
		{
			// ---------------------------------- 
			// SysOptImpl

			class SysOptImpl
			{
				typedef std::map<stdex::tString, stdex::tString> SysOpt;
				SysOpt sysOpt_;

			public:
				SysOptImpl();

			private:
				SysOptImpl(const SysOptImpl &);
				SysOptImpl &operator=(const SysOptImpl &);

			public:
				void Load();

				template < typename T >
				const T &GetValue(const stdex::tString &key, const T &defVal = 0)
				{
					static_assert<std::tr1::is_integral<T>::value>;

					SysOpt::const_iterator iter = sysOpt_.find(key);
					if( iter == sysOpt_.end() )
						return defVal;

					
					return stdex::ToNumber<T>(iter->second);
				}

				const stdex::tString &GetValue(const stdex::tString &key, const stdex::tString &defVal = _T(""))
				{
					SysOpt::const_iterator iter = sysOpt_.find(key);
					if( iter == sysOpt_.end() )
						return defVal;

					return iter->second;
				}

			
			};
		}
	}
}

#endif
#ifndef __STRING_ALGORITHM_H
#define __STRING_ALGORITHM_H

#include <iomanip>
#include <algorithm>
#include <functional>
#include <sstream>
#include <string>
#include <assert.h>
#include <vector>
#include <atlenc.h>

namespace stdex
{
	template<typename StringT>
	inline void ToUpper(StringT &str)
	{
		std::transform(str.begin(), str.end(), str.begin(), ::toupper);
	}

	template<typename CharT, size_t N>
	inline void ToUpper(CharT (&str)[N])
	{
		std::transform(str, str + N, str, ::toupper);
	}

	template<typename StringT>
	inline StringT ToUpper(const StringT &str)
	{
		StringT tmp;
		tmp.resize(str.length());
		std::transform(str.begin(), str.end(), tmp.begin(), ::toupper);

		return tmp;
	}

	template<typename StringT>
	inline void ToLower(StringT &str)
	{
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	}
	template<typename CharT, size_t N>
	inline void ToLower(CharT (&str)[N])
	{
		std::transform(str, str + N, str, ::tolower);
	}


	template<typename StringT>
	inline StringT ToLower(const StringT &str)
	{
		StringT tmp;
		tmp.reserve(str.length());

		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		return tmp;
	}

	template<typename StringT>
	inline void TrimLeft(StringT &str)
	{
		str.erase(0, str.find_first_not_of(' '));
	}

	template<typename StringT>
	inline void TrimRight(StringT &str)
	{
		str.erase(str.find_last_not_of(' ') + 1);
	}

	template<typename StringT>
	inline void Trim(StringT &str)
	{
		str.erase(0, str.find_first_not_of(' '));
		str.erase(str.find_last_not_of(' ') + 1);
	}

	template<typename StringT>
	inline void Erase(StringT &str, const typename StringT::value_type &charactor)
	{
		str.erase(remove_if(str.begin(), str.end(), 
			bind2nd(std::equal_to<typename StringT::value_type>(), charactor)), str.end());
	}

	template<typename StringT>
	inline void Replace(StringT &str, const StringT &strObj, const StringT &strDest)
	{
		while(true)
		{
			typename StringT::size_type pos = str.find(strObj);

			if( pos != StringT::npos )
				str.replace(pos, strObj.size(), strDest);
			else
				break;
		}
		
	}

	template<typename StringT>
	inline bool IsStartWith(StringT &str, const StringT &strObj)
	{
		return str.compare(0, strObj.size(), strObj) == 0;
	}

	template<typename StringT>
	inline bool IsEndWith(StringT &str, const StringT &strObj)
	{
		return str.compare(str.size() - strObj.size(), strObj.size(), strObj) == 0;
	}

	template < typename CharT >
	inline std::basic_stringstream<CharT> &operator>>(std::basic_stringstream<CharT> &os, std::basic_string<CharT> &str)
	{
		str = os.str();
		return os;
	}

	template<typename T, typename CharT>
	inline std::vector<T>& Split(std::vector<T> &seq, const std::basic_string<CharT> &str, CharT separator)
	{
		if( str.empty() )
			return seq;

		std::basic_stringstream<CharT> iss(str);
		for(std::basic_string<CharT> s; std::getline(iss, s, separator); )
		{
			typename T val;
			std::basic_stringstream<CharT> isss(s);

			isss >> val;
			seq.push_back(val);
		}

		return seq;    
	}

	template < typename CharT >
	inline std::basic_string<CharT> Split(const std::basic_string<CharT> &str, CharT separator, size_t index)
	{
		if( str.empty() )
			return std::basic_string<CharT>();

		std::basic_stringstream<CharT> iss(str);
		std::vector<std::basic_string<CharT>> tmp;

		for(std::basic_string<CharT> s; std::getline(iss, s, separator); )
		{
			std::basic_string<CharT> val;
			std::basic_stringstream<CharT> isss(s);

			isss >> val;
			tmp.push_back(val);
		}

		assert(index < tmp.size());
		return tmp[index];
	}


	// ---------------------
	template<typename T, typename CharT>
	inline T ToNumber(const std::basic_string<CharT> &str)
	{
		T num(0);
		std::basic_istringstream<CharT>(str) >> num;

		return num;
	}

	template<typename T, typename CharT>
	inline void ToNumber(T &val, const std::basic_string<CharT> &str)
	{
		std::basic_istringstream<CharT>(str) >> val;
	}
	
	template<typename CharT>
	inline void ToNumber(bool &val, const std::basic_string<CharT> &str)
	{
		std::basic_istringstream<CharT>(str) >> std::boolalpha >> val;
	}
	template<typename T, typename CharT, size_t N>
	inline void ToNumber(T &val, const CharT (&str)[N])
	{
		std::basic_istringstream<CharT>(str) >> val;
	}
	
	template<typename T, typename CharT>
	inline void ToString(const T &val, std::basic_string<CharT> &str)
	{
		std::basic_ostringstream<CharT> temp;
		temp << val;

		str = temp.str();
	}

	template<typename T, typename StringT>
	inline void ToString(const T &val, StringT &str, int prec)
	{
		std::basic_ostringstream<typename StringT::value_type> temp;
		temp << std::setiosflags(std::ios::fixed) << std::setprecision(prec) << val;

		str = temp.str();
	}

	template<typename StringT>
	inline void ToString(bool val, StringT &str)
	{
		std::basic_ostringstream<typename StringT::value_type> temp;
		temp << std::boolalpha << val;

		str = temp.str();
	}

	template<typename StringT, typename T>
	inline StringT ToString(const T &val)
	{
		std::basic_ostringstream<typename StringT::value_type> temp;
		temp << val;

		return temp.str();
	}

	template<typename StringT, typename T>
	inline StringT ToString(const T &val, int prec)
	{
		std::basic_ostringstream<typename StringT::value_type> temp;
		temp << std::setiosflags(std::ios::fixed) << std::setprecision(prec) << val;

		return temp.str();
	}

}


#endif
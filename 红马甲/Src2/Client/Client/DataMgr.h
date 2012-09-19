#ifndef __DATA_MANAGER_HPP
#define __DATA_MANAGER_HPP

#include <string>

namespace data
{
	class DataMgr
	{
	private:
		std::wstring userName_;
		std::wstring psw_;

		enum { NORMAL = 1, GOLDEN, Diamond };
		size_t permission_;

	public:
		DataMgr();
		~DataMgr();

	private:
		DataMgr(const DataMgr &);
		DataMgr &operator=(const DataMgr &);

	public:
		void SetUserInfo(const std::wstring &userName, const std::wstring &psw);
		std::pair<const std::wstring, const std::wstring> GetUserInfo() const;

		std::wstring GetUrl(size_t index) const;

		void SetPermission(size_t permission);
		bool HasPermission(size_t index) const;
	};


	// µ¥Àý
	DataMgr &DataMgrInstance();
}




#endif
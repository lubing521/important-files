#include "stdafx.h"
#include "IconHelper.h"

#include "../../../include/CustomFileDLL.h"

#pragma comment(lib, "CustomFileDLL")


namespace i8desk
{

	IconHelper::IconHelper()
		: handle_(0)
	{
		handle_ = ::CreateCustomFile((utility::GetAppPath() + _T("plugin_tools/") + _T("icons.dat")).c_str());
		assert(handle_ != 0);
	}

	IconHelper::~IconHelper()
	{
		assert(handle_ != 0);
		::DestroyCustomFile(handle_);
	}


	bool IconHelper::EnsureIcon(long gid, size_t size, const void *data)
	{
		void *tmpdata = 0;
		size_t outsize = 0;
		if( !::GetFileData(handle_, gid, outsize, tmpdata) )
			return ::AddCustomFile2(handle_, gid, size, data);
		else
			return ::ModifyCustomFile2(handle_, gid, size, data);
	}

	bool IconHelper::GetIcon(long gid, size_t &size, void *&data)
	{
		return ::GetFileData(handle_, gid, size, data);
	}

}
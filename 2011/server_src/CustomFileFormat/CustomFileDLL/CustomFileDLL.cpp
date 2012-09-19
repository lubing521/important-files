// CustomFileDLL.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "../../../include/CustomFileDLL.h"
#include "../../../include/frame.h"

#include "../../../include/MultiThread/Lock.hpp"
#include "../../../include/Utility/utility.h"

#include <cassert>
#include "../CustonFileFormat/CustomFileImpl.h"


using namespace i8desk;

struct Log
{
	typedef async::thread::AutoCriticalSection	Mutex;
	typedef async::thread::AutoLock<Mutex>		AutoLock;

	Mutex mutex_;
	ILogger *log_;

	Log()
		: log_(0)
	{
		char buf[] = {0};
	
		typedef BOOL (WINAPI *PFNCREATELOGGER)(ILogger** ppILogger);
		stdex::tString appPath = utility::GetAppPath();
		appPath += _T("Frame.dll");

		HMODULE hMod = ::LoadLibrary(appPath.c_str());
		PFNCREATELOGGER pfnCreateLogger = NULL;
		if (hMod != NULL && (pfnCreateLogger = (PFNCREATELOGGER)::GetProcAddress(hMod, MAKEINTRESOURCEA(5))))
		{
			pfnCreateLogger(&log_);
			if (log_ != NULL)
			{
				log_->SetLogFileName(TEXT("CustomFile"));
				log_->SetAddDateBefore(false);
				log_->WriteLog(LM_INFO, TEXT("==============================================="));
				log_->SetAddDateBefore(true);
			}
		}
	}

	~Log()
	{
		if( log_ != 0 )
			log_->Release();
	}

	void Write(LPCTSTR formator, ...)
	{
		assert(log_ != 0);
		if( log_ == 0 )
			return;

		TCHAR msg[1024] = {0};

		va_list marker;
		va_start(marker, formator);
		int len = ::_vstprintf(msg, formator, marker);
		va_end(marker);

		log_->WriteLog(LM_ERROR, formator);
	}
};

std::auto_ptr<Log> gLog;

void *WINAPI CreateCustomFile(LPCTSTR lpszSavePath)
{
	assert(lpszSavePath);
	
	custom_file::CustomFileImpl *custom = 0;
	try
    {
        gLog.reset(new Log);
		custom = new custom_file::CustomFileImpl(lpszSavePath);

		custom->LoadFromFile();
	}
	catch(std::exception &e)
	{
		gLog->Write(CA2T(e.what()));
		::OutputDebugStringA(e.what());
	}

	return custom;
}

bool WINAPI DestroyCustomFile(void *handle)
{
	assert(handle);
	custom_file::CustomFileImpl *custom = static_cast<custom_file::CustomFileImpl *>(handle);

	try
	{
		custom->FlushFile();
		delete custom;
		custom = 0;

		gLog.reset();
	}
	catch(std::exception &e)
	{
		gLog->Write(CA2T(e.what()));
		::OutputDebugStringA(e.what());
		return false;
	}

	return true;
}


bool WINAPI FlushCustomFile(void *handle)
{
	assert(handle);
	custom_file::CustomFileImpl *custom = static_cast<custom_file::CustomFileImpl *>(handle);

	try
	{
		custom->FlushFile();
	}
	catch(std::exception &e)
	{
		::OutputDebugStringA(e.what());
		gLog->Write(CA2T(e.what()));
		return false;
	}

	return true;
}

bool WINAPI AddCustomFile(void *handle, long gid, LPCTSTR filePath)
{
	assert(handle);
	custom_file::CustomFileImpl *custom = static_cast<custom_file::CustomFileImpl *>(handle);

	try
	{
		custom->AddFile(gid, filePath);
	}
	catch(std::exception &e)
	{
		::OutputDebugStringA(e.what());
		gLog->Write(CA2T(e.what()));
		return false;
	}

	return true;
}


bool WINAPI AddCustomFile2(void *handle, IN long gid, IN size_t size, IN const void *data)
{
	assert(handle);
	custom_file::CustomFileImpl *custom = static_cast<custom_file::CustomFileImpl *>(handle);

	try
	{
		custom->AddFile(gid, 0, size, data);
	}
	catch(std::exception &e)
	{
		::OutputDebugStringA(e.what());
		gLog->Write(CA2T(e.what()));
		return false;
	}

	return true;
}

bool WINAPI DeleteCustomFile(void *handle, long gid)
{
	assert(handle);
	custom_file::CustomFileImpl *custom = static_cast<custom_file::CustomFileImpl *>(handle);

	try
	{
		custom->DeleteFile(gid);
	}
	catch(std::exception &e)
	{
		::OutputDebugStringA(e.what());
		gLog->Write(CA2T(e.what()));
		return false;
	}

	return true;
}


bool WINAPI ModifyCustomFile(void *handle, long gid, LPCTSTR newFilePath)
{
	assert(handle);
	custom_file::CustomFileImpl *custom = static_cast<custom_file::CustomFileImpl *>(handle);

	try
	{
		custom->ModifyFile(gid, newFilePath);
	}
	catch(std::exception &e)
	{
		::OutputDebugStringA(e.what());
		gLog->Write(CA2T(e.what()));
		return false;
	}

	return true;
}

bool WINAPI ModifyCustomFile2(void *handle, IN long gid, IN size_t size, IN const void *data)
{
	assert(handle);
	custom_file::CustomFileImpl *custom = static_cast<custom_file::CustomFileImpl *>(handle);

	try
	{
		custom->ModifyFile(gid, 0, size, data);
	}
	catch(std::exception &e)
	{
		::OutputDebugStringA(e.what());
		gLog->Write(CA2T(e.what()));
		return false;
	}

	return true;
}

bool WINAPI GetFileData(void *handle, IN long gid, OUT size_t &size, OUT void *&data)
{
	assert(handle);
	custom_file::CustomFileImpl *custom = static_cast<custom_file::CustomFileImpl *>(handle);

	try
	{
		const custom_file::FileInfoPtr &fileInfo = custom->GetData(gid);
		
		size = fileInfo->curSize_;
		data = fileInfo->data_.get();
	}
	catch(std::exception &e)
	{
		::OutputDebugStringA(e.what());
		gLog->Write(CA2T(e.what()));
		return false;
	}

	return true;
}


size_t WINAPI GetAllFileData(void *handle, IN OUT size_t &size, IN OUT CustomFileInfoPtr &files)
{
	assert(handle);
	custom_file::CustomFileImpl *custom = static_cast<custom_file::CustomFileImpl *>(handle);

	size_t  totalSize = 0;
	try
	{
		totalSize = custom->GetAllData(size, files);
	}
	catch(std::exception &e)
	{
		::OutputDebugStringA(e.what());
		gLog->Write(CA2T(e.what()));
		return 0;
	}

	return totalSize;
}
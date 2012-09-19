#include "stdafx.h"
#include "MonitorMgr.hpp"

#include "../../../include/Win32/FileSystem/FindFile.hpp"
#include "../../../include/Zip/ZipHelper.hpp"

namespace recvSvr
{

	void MonitorMgr::Start()
	{
		exit_.Create(0, TRUE);

		io_.Post(std::tr1::bind(&MonitorMgr::_Handle, this));
	}

	void MonitorMgr::Stop()
	{
		exit_.SetEvent();
	}


	void MonitorMgr::_Handle()
	{
		path_ += _T("*.flag");

		while(1)
		{
			if( exit_.WaitForEvent(20) )
				break;
			
			if( IsPaused() )
				continue;

			::filesystem::CFindFile findFile;
			if( !findFile.FindFile(path_.c_str()) )
				continue;
			
			std::wstring fileFlag = findFile.GetFilePath();
			std::wstring filePath(fileFlag, 0, fileFlag.length() - 4);
			filePath += L"bmp";

			std::ifstream in(filePath.c_str(), std::ios::binary | std::ios::in);
			if( !in )
			{
				::DeleteFile(fileFlag.c_str());
				::InterlockedExchange(&isOK_, 0);
				continue;
			}

			in.seekg(0, std::ios::end);
			size_t len = in.tellg();
			in.seekg(0, std::ios::beg);

			common::Buffer zipBuffer = common::MakeBMPBuffer(len);
			in.read(zipBuffer.first.get(), zipBuffer.second);
			in.close();

			common::Buffer fileBuffer;
			try
			{
				fileBuffer = zip::UnZip(zipBuffer, &common::MakeBMPBuffer);
			}
			catch(std::exception &e)
			{
				::InterlockedExchange(&isOK_, 0);
				::OutputDebugStringA(e.what());
			}

			::DeleteFile(filePath.c_str());
			::DeleteFile(fileFlag.c_str());


			if( fileBuffer.second != 0 )
			{
				std::wstring fileName(filePath, filePath.rfind(L'\\') + 1);
				callback_(std::tr1::cref(std::make_pair(fileBuffer, fileName)));
			}

			::InterlockedExchange(&isOK_, 1);
		}
	}	
}
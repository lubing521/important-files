#include "stdafx.h"
#include "HistoryMonitor.hpp"

#include "../../../include/Win32/FileSystem/FindFile.hpp"
#include "../../../include/Zip/ZipHelper.hpp"

namespace recvSvr
{

	void HistoryMonitor::Start()
	{
		exit_.Create(0, TRUE);

		io_.Post(std::tr1::bind(&HistoryMonitor::_Handle, this));
	}

	void HistoryMonitor::Stop()
	{
		exit_.SetEvent();
	}


	void HistoryMonitor::_Handle()
	{
		//path_ += _T("*.flag");

		while(1)
		{
			if( exit_.WaitForEvent(60/* * 1000*/) )
				break;

			::filesystem::CFindFile findFileFalg;
			BOOL suc = findFileFalg.FindFile((path_ + _T("*.flag")).c_str());
			if( !suc )
				continue;
			::DeleteFile(findFileFalg.GetFilePath().c_str());

			std::wstring file_path = path_ + findFileFalg.GetFileTitle() + _T(".*");
			::filesystem::CFindFile findFile;
			suc = findFile.FindFile(file_path.c_str());
			
			while(suc)
			{
				const std::wstring &filePath = findFile.GetFilePath();
			
				std::ifstream in(filePath.c_str(), std::ios::binary | std::ios::in);
				if( !in )
				{
					::DeleteFile(filePath.c_str());
					continue;
				}

				in.seekg(0, std::ios::end);
				size_t len = in.tellg();
				in.seekg(0, std::ios::beg);

				common::Buffer zipBuffer(common::MakeNormalBuffer(len));
				in.read(zipBuffer.first.get(), zipBuffer.second);
				in.close();

				::DeleteFile(filePath.c_str());

				if( zipBuffer.second != 0 )
				{
					std::wstring fileName(filePath, filePath.rfind(L'.') + 1);
					callback_(std::tr1::cref(std::make_pair(zipBuffer, fileName)));
				}
			}

			
		}
	}	
}
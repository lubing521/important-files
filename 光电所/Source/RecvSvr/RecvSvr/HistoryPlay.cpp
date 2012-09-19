#include "stdafx.h"
#include "HistoryPlay.hpp"

#include "../../../include/Win32/FileSystem/FindFile.hpp"


namespace recvSvr
{
	volatile long HistoryPlay::ONLYONE = 0;
	
	void HistoryPlay::Start()
	{
		if( ::InterlockedExchangeAdd(&ONLYONE, 0) == 1 )
			return;

		::InterlockedExchange(&ONLYONE, 1);

		exit_.Create(0, TRUE);

		io_.Post(std::tr1::bind(&HistoryPlay::_Handle, shared_from_this(),
			i_path_, i_freq_, i_callback_));
		io_.Post(std::tr1::bind(&HistoryPlay::_Handle, shared_from_this(),
			j_path_, j_freq_, j_callback_));
	}

	void HistoryPlay::Stop()
	{
		::InterlockedExchange(&ONLYONE, 0);
	}


	void HistoryPlay::_Handle(std::wstring path, size_t freq, const Callback &callback)
	{
		path += _T("/*.bmp");

		::filesystem::CFindFile findFile;
		if( !findFile.FindFile(path.c_str()) )
			return;

		while(1)
		{
			if( exit_.WaitForEvent(freq) )
				break;
			if( ::InterlockedExchangeAdd(&ONLYONE, 0) == 0 )
				break;

			std::wstring filePath = findFile.GetFilePath();

			try
			{
				std::ifstream in(filePath.c_str(), std::ios::binary | std::ios::in);
				if( !in )
					continue;

				in.seekg(0, std::ios::end);
				size_t len = in.tellg();
				in.seekg(0, std::ios::beg);

				common::Buffer buffer = common::MakeBMPBuffer(len);
				in.read(buffer.first.get(), buffer.second);
				in.close();

				std::wstring fileName(filePath, filePath.rfind(L'\\') + 1);
				callback(std::tr1::cref(std::make_pair(buffer, fileName)));
			}
			catch(std::exception &e)
			{
				::OutputDebugStringA(e.what());
			}

			if( !findFile.FindNextFile() )
				break;
		}

		::InterlockedExchange(&ONLYONE, 0);
	}	
}
// NetSendTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>

#include <atlbase.h>
#include <atlconv.h>

#include "../../../include/Win32/System/except.h"
#include "../../../include/Win32/System/DynamicFunc.hpp"
#include "../../../include/MultiThread/Thread.hpp"
#include "../../Common/Utility.hpp"


typedef std::vector<std::pair<std::vector<char>, std::string>> BmpBuffers;
BmpBuffers buffers;


void LoadBuffers()
{
	for(size_t i = 0; i != 17; ++i)
	{
		std::wostringstream os;
		os << common::GetAppPath() << L"test" << i << L".bmp";

		std::ifstream in(os.str().c_str(), std::ios::binary | std::ios::in);
		in.seekg(0, std::ios::end);
		size_t len = in.tellg();
		in.seekg(0, std::ios::beg);

		BITMAPFILEHEADER bmp_file_header = {0};
		in.read(reinterpret_cast<char *>(&bmp_file_header), sizeof(bmp_file_header));

		size_t offset = sizeof(bmp_file_header) + bmp_file_header.bfOffBits;
		const size_t data_len = len - bmp_file_header.bfOffBits;
		in.seekg(offset, std::ios::beg);
		std::vector<char> buf(data_len);
		in.read(&buf[0], data_len);

		std::ostringstream fileName;
		fileName << "test" << i << ".bmp";
		buffers.push_back(std::make_pair(buf, fileName.str()));
	}

}

template < typename FuncT >
DWORD Send(const FuncT &func, size_t freq, size_t width, size_t height)
{
	size_t task_id = common::GetIniConfig<size_t>(L"NetSend.ini", L"CONFIG", L"TASK_ID");

	if( func.IsValid() )
	{

		for(size_t loop = 0; loop != 1000; ++loop)
		{
			for(size_t i = 0; i != buffers.size(); ++i)
			{
				(*func)(&(buffers[i].first[0]), width * height, task_id, freq, width, height, buffers[i].second.c_str());
				::Sleep(freq);
			}
		}

	}

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	except::MiniDump dump;

	LoadBuffers();

	{
		typedef void (__stdcall *SendFunc)(const char *, size_t, size_t, size_t, size_t, size_t, const char *);
		win32::system::DynamicFunc<SendFunc> sendI(_T("NetSend.dll"), "Send_I");
		win32::system::DynamicFunc<SendFunc> sendJ(_T("NetSend.dll"), "Send_J");

		async::thread::ThreadImplEx threadI;
		async::thread::ThreadImplEx threadJ;

		int freq_i = common::GetIniConfig<int>(L"NetSend.ini", L"CONFIG", L"FREQ_I");
		int freq_j = common::GetIniConfig<int>(L"NetSend.ini", L"CONFIG", L"FREQ_J");

		size_t width = common::GetIniConfig<size_t>(L"NetSend.ini", L"CONFIG", L"WIDTH");
		size_t height = common::GetIniConfig<size_t>(L"NetSend.ini", L"CONFIG", L"HEIGHT");

		threadI.RegisterFunc(std::tr1::bind(&Send<win32::system::DynamicFunc<SendFunc>>, std::tr1::ref(sendI), freq_i, width, height));
		threadJ.RegisterFunc(std::tr1::bind(&Send<win32::system::DynamicFunc<SendFunc>>, std::tr1::ref(sendJ), freq_j, width, height));

		threadI.Start();
		threadJ.Start();

		system("pause");

		threadI.Stop(2000);
		threadJ.Stop(2000);
	}

	
	return 0;
}


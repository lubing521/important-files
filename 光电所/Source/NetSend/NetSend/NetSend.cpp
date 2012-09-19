// NetSend.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "NetSend.h"

#include <sstream>
#include <iomanip>
#include "BufferSend.hpp"


static oc::BufferSend bufferSendI(L"PORT_I");
static oc::BufferSend bufferSendJ(L"PORT_J");

void __stdcall Send_I(const char *buf, size_t len, size_t task_id, size_t freq, size_t width, size_t height, const char *file_name)
{
	std::ostringstream os;
	SYSTEMTIME localTime = {0};
	::GetLocalTime(&localTime);
	os << ::time(0) << std::setfill('0') << std::setw(4) << localTime.wMilliseconds << "_" << task_id 
		<< "_" << 'I' << "_" << freq << "_" 
		<< width << "_" << height << "_" << file_name;

	bufferSendI.SetBuffer(buf, len, os.str().c_str());
	bufferSendI.Run();
}


void __stdcall Send_J(const char *buf, size_t len, size_t task_id, size_t freq, size_t width, size_t height, const char *file_name)
{
	std::ostringstream os;
	SYSTEMTIME localTime = {0};
	::GetLocalTime(&localTime);
	os << ::time(0) << std::setfill('0') << std::setw(4) << localTime.wMilliseconds << "_" << task_id 
		<< "_" << 'J' << "_" << freq << "_" 
		<< width << "_" << height << "_" << file_name;

	bufferSendJ.SetBuffer(buf, len, os.str().c_str());
	bufferSendJ.Run();
}

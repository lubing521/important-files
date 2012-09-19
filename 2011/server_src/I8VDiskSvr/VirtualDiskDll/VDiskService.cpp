#include "stdafx.h"
#include "VDiskService.hpp"

#include <ctime>

#include "VDiskAcceptor.hpp"
#include "Misc.hpp"
#include "NetworkFlow.hpp"


namespace i8desk
{
	
	

	using namespace async::network;


	VDiskService::VDiskService()
		: io_(GetFitThreadNum())
		, refreshSock_(INVALID_SOCKET)
		, version_(0)
		, time_(io_)
	{
		acceptor_.reset(new VDiskAcceptor(*this, io_));
	}


	DWORD VDiskService::Start(LPCTSTR ip, WORD port, LPCTSTR filePath)
	{
		refreshSock_	= INVALID_SOCKET;
		filePath_		= filePath;

		acceptor_->Start(ip, port);
		time_.AsyncWait(std::tr1::bind(&VDiskService::_Total, this), 2000);

		return ERROR_SUCCESS;
	}

	void VDiskService::Stop()
	{
		// 撤销刷盘socket
		refreshSock_ = INVALID_SOCKET;

		time_.Cancel();
		acceptor_->Stop();
	}


	void VDiskService::GetVDiskInfo(PDWORD pdwConnect, PDWORD pdwVersion)
	{
		//*pdwConnect = networkFlow_.Size();
		*pdwVersion = version_;
	}
	
	void VDiskService::RefreshBufferEx()
	{
		// 清空文件缓存
		//cache_.diskCache_.clear();

		// 刷新版本号
		version_ = _time32(NULL);
		refreshSock_ = INVALID_SOCKET;
	}

	void VDiskService::_Total()
	{
		std::cout << "Cache Read: " << g_flow.cacheRead_ << "\t" << "Disk Read: " << g_flow.diskRead_ << "\t" << "System Read: " << g_flow.systemRead_ << std::endl
			<< "Cache Size: " << g_flow.cacheSize_ / 1024.0 << "\t" << "Disk Size: " << g_flow.diskSize_ / 1024.0 << "\t" << "System Size: " << g_flow.systemSize_ / 1024.0 << std::endl
			<< "Hint: " << (g_flow.systemRead_ == 0 ? 0 : (g_flow.cacheRead_ * 100.0 / g_flow.systemRead_)) << std::endl << std::endl << std::endl;

	}
}
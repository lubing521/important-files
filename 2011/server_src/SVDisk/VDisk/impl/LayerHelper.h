#ifndef __LAYER_HELPER_HPP
#define __LAYER_HELPER_HPP



#include "../impl/NetworkLayer/NetworkService.h"
#include "../impl/LogicLayer/UserMgr.h"

namespace i8desk
{

	// 获取硬盘大小
	template<typename SizeT>
	bool GetDiskSize(LPCTSTR filePath, SizeT &sz)
	{
		HANDLE hFile = ::CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING,
			NULL);

		// 获取分区大小
		PARTITION_INFORMATION partionInfo = {0};
		DWORD nSize = sizeof(partionInfo);

		OVERLAPPED over = {0};
		over.hEvent = ::CreateEvent(NULL, TRUE, FALSE, _T("Global/SVDiskEvent"));
		::DeviceIoControl(hFile, IOCTL_DISK_GET_PARTITION_INFO, NULL, 0, &partionInfo, 
			sizeof(partionInfo), &nSize, &over);
		::WaitForSingleObject(over.hEvent, INFINITE);

		sz.QuadPart = partionInfo.PartitionLength.QuadPart;

		::CloseHandle(over.hEvent);
		::CloseHandle(hFile);

		return true;
	}


	template<typename SocketT, typename BufferT, typename AddrT>
	void AddNewUser(const SocketT &socket, const BufferT &buffer, const AddrT &addr)
	{
		logic::UserMgr::GetInstance().AddUser(socket, buffer, addr);
	}

	template<typename SocketT, typename BufferT>
	void SendDiskSize(const SocketT &socket, const BufferT &buffer, LPCTSTR filePath)
	{
		// 返回硬盘大小
		LARGE_INTEGER sz = {0};
		if( !i8desk::GetDiskSize(filePath, sz) )
		{
			// 出错处理
			std::cerr << "获取硬盘大小出错" << std::endl;
			return;
		}

		networkimpl::NetworkService::GetInstance().SendDiskSize(socket, buffer, sz);
	}

	template<typename SocketT, typename BufferT, typename OffsetT, typename LengthT>
	void SendFileData(const SocketT &socket, const BufferT &buffer, OffsetT offset, LengthT len)
	{

	}
}




#endif
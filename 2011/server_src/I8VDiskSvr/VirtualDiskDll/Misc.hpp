#ifndef __VDISK_MISC_HPP
#define __VDISK_MISC_HPP

#include <winioctl.h>
#include "../../../include/Utility/SmartHandle.hpp"


namespace utility
{
	// 获取硬盘大小
	inline LARGE_INTEGER GetDiskSize(LPCTSTR lpszFile)
	{
		utility::CAutoFile hFile = ::CreateFile(lpszFile, GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,  FILE_ATTRIBUTE_NORMAL, NULL);

		//BOOL suc = ::GetFileSizeEx(hFile, &fileSize); // 不支持对卷操作
		//assert(suc);

		// get disk file size.
		PARTITION_INFORMATION PartInfo = {0};
		ULONG IoLength = sizeof(PARTITION_INFORMATION);
		OVERLAPPED ov = {0};
		ov.hEvent = ::CreateEvent(NULL, TRUE, FALSE, TEXT("Global\\VDISK_DSCSISERVERINIT"));
		if( ov.hEvent == NULL )
			throw async::iocp::Win32Exception("CreateEvent");

		BOOL suc = ::DeviceIoControl(hFile, IOCTL_DISK_GET_PARTITION_INFO, NULL, 0,
			&PartInfo, sizeof(PARTITION_INFORMATION), &IoLength, &ov);
		assert(suc);
		if( !suc )
			throw async::iocp::Win32Exception("DeviceIoControl");

		::WaitForSingleObject(ov.hEvent, INFINITE);
		::CloseHandle(ov.hEvent);

		return PartInfo.PartitionLength;
	}


	// 圆整为扇区大小倍数
	template<typename U>
	inline U Round(const U &m)
	{
		return (m + 512 - 1) & ~(512 - 1);
	}

}




#endif
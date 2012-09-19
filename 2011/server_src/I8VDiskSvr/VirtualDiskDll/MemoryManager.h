#pragma once

#ifndef __VDISK_MEMORY_MANAGER_HPP__
#define __VDISK_MEMORY_MAMAGER_HPP__


#include "../../../include/MemoryPool/FixedMemoryPool.hpp"
#include "../../../include/MemoryPool/SGIMemoryPool.hpp"
#include "../../../include/Allocator/ContainerAllocator.hpp"

#include "../../../include/Cache/Cache.hpp"

#include "NetLayerHdr.h"


namespace i8desk
{	

	// 内存池管理器
	struct VDiskMemoryMgr
	{
		static const size_t MaxDiskDataSize	= 0x10000;
		static const size_t PackageBufSize	= PACKAGE_BUFFER_SIZE;

		// DiskData使用
		typedef async::memory::SGIMemoryPool<true, MaxDiskDataSize>					DiskDataPoolType;

		// 专用内存池，提供给Package使用
		typedef async::memory::FixedMemoryPool<false, PACKAGE_BUFFER_SIZE>			PackagePoolType;

		// Package专用分配器
		typedef async::allocator::ContainerAllocator<char, PackagePoolType>			PackageAllocType;

		// Disk数据分配器
		typedef async::allocator::ContainerAllocator<char, DiskDataPoolType>		DiskDataAllocType;
	};

}



#endif
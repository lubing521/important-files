#ifndef __NET_LAYER_HEADER_HPP
#define __NET_LAYER_HEADER_HPP



namespace i8desk
{
	
	#pragma pack(push,8)
	#pragma warning(disable:4200)

	enum 
	{
		DRP_MJ_REFRESH	= 0x01,			//刷新虚拟为盘，上行只有一个命令，应答的数据空。只是成功。
		DRP_MJ_READ		= 0x02,			//读数据，上行，包括读数据的位置（扇区的偏移量），以及长度。应答表示成功或者失败，成功时要应答传回的数据的长度（与请求长度一样大）　
		DRP_MJ_WRITE	= 0x03,			//没有用。
		DRP_MJ_QUERY	= 0x04			//查询分区大小，应答是devinfo.
	};

	typedef struct tagDRP
	{
		BYTE			 MajorFunction;//1: Refresh, 2: Read, 3: Write, 4:Query.
		union
		{
			struct 
			{
				LARGE_INTEGER	Offset;
				ULONG			Length;
			}Read;
			struct 
			{
				LARGE_INTEGER   Offset;
				ULONG			Length;
			}Write;
			struct 
			{
				ULONG	DiskSize;
			}Query;
		};
		char buffer[0];
	}DRP,*LPDRP;

	typedef struct tagDSTATUS
	{
		WORD	Status;				// 返回状态
		DWORD	Information;		// 暂不用
		DWORD	Length;				// 返回文件长度
	}DSTATUS,*LPDSTATUS;


	enum 
	{
		STATUS_SUCCESS		 = 0x0000,
		STATUS_INVALID_CALL	 = 0x2001,
		STATUS_READ_ERROR	 = 0x2002,
		STATUS_WRITE_ERROR   = 0x2003
	};


	typedef struct tagDEVINFO
	{
		UCHAR			Type;	//0:protect 1:readonly 2:read write
		LARGE_INTEGER	Size;
	}DEVINFO, *LPDEVINFO;

	#pragma pack(pop)



	// 数据包大小
	const size_t PACKAGE_BUFFER_SIZE = 0x10000 + sizeof(DSTATUS);


	// 组包
	inline LPDSTATUS ConstructDSTATUS(char *buf, WORD status, DWORD len)
	{
		LPDSTATUS lpStatus	= reinterpret_cast<LPDSTATUS>(buf);

		lpStatus->Length		= len;
		lpStatus->Information	= len;
		lpStatus->Status		= status;

		return lpStatus;
	}

}








#endif
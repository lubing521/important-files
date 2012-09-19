#ifndef _virtual_disk_inc_
#define _virtual_disk_inc_

#include "../../../include/frame.h"

namespace i8desk
{
	//虚拟盘插件sendmessage的几个命令
	//(0x01--0x04来代替以前几个输出的api)

	struct tagVDInfo
	{
		TCHAR  ID[MAX_GUID_LEN];
		TCHAR  svrMode[MAX_NAME_LEN];

		DWORD dwIp;
		DWORD nPort;
		char szSvrDrv;
		char szCliDrv;
		DWORD dwConnect;
		DWORD dwVersion;//(time_t);
		
		ulong cacheSize;
		int ssdDrv;
		TCHAR szTempDir[MAX_PATH];
	};
	typedef std::tr1::shared_ptr<tagVDInfo> VDInfoPtr;


	struct VDiskClientStatus
	{
		tagVDInfo	*vDiskInfo;					// 虚拟盘信息

		TCHAR		VID[MAX_GUID_LEN];			// 虚拟盘ID
		ulong		VIP;						// 虚拟盘IP
		ulong		ClientIP;					//客户机IP
		ulonglong	ReadCount;					//读取数据量				
		ulong		ReadSpeed;					//读取速度
		ulong		ReadMax;					//读取峰值
		ulong		ConnectTime;				//连接时长
		ulong		CacheShooting;				//缓存命中率
	};
	typedef std::tr1::shared_ptr<VDiskClientStatus> VDiskClientStatusPtr;


	#define VDISK_CMD_START		0x01 
	/*参数说明
		[in]param1:tagVDInfo* pVDisk.
		param2:保留。填0
		return value:返回一个,表示虚拟盘实例对象的句柄(标识)。
	*/

	#define VDISK_CMD_STOP		0x02
	/*参数说明
		[in]param1:DWORD handle.要停止虚拟盘对象的句柄.
		param2:保留。(0);
		return value:0.
	*/

	#define VDISK_CMD_REFRESH	0x03
	/*参数说明:
		[in]param1:DWORD handle.要刷新虚拟盘对象的句柄.
		param2:保留。(0);
		return value:0.
	*/

	#define VDISK_CMD_GETINFO	0x04
	/*参数说明:
		[in]param1:DWORD handle.要操作的虚拟盘对象句柄.
		[in, out]param2:tagVDInfo* pVDInfo.虚拟盘的状态
		return value:0
	*/

	ISvrPlug* WINAPI CreateSvrPlug(DWORD);

// 	DWORD  WINAPI StartServer  (PHANDLE handle, LPCSTR lpszSvrIP, char szDriver, WORD wPort);
// 	void   WINAPI GetVdiskInfo (HANDLE handle, PDWORD pdwConnect, PDWORD pdwVersion);
// 	void   WINAPI StopServer   (HANDLE handle);
// 	void   WINAPI RefreshBuffer(HANDLE handle);

	#define VDISK_CMD_GETALLINFO 0x05
	/*参数说明:
		[in,out]param1:tagVDInfo* pVDInfo.
		[in]param2:DWORD dwCount.表示参数1可以放入的虚拟盘个数
		return value:写入状态的虚拟盘个数.

		eg.
		DWORD dwCount = 10;//需要取状态的虚拟盘个数。
		tagVDInfo VDInfo[dwCount] = {0};
		DWORD dwVDCount = SendMessage(VDISK_CMD_GETSTATUS2, (DWORD)VDInfo, dwConnt);
	*/

	#define VDISK_CMD_GETINFOEX 0x06
}

#endif

#ifndef _virtual_disk_inc_
#define _virtual_disk_inc_

#include "../../../include/plug.h"

namespace i8desk
{
	//虚拟盘插件sendmessage的几个命令
	//(0x01--0x04来代替以前几个输出的api)

	typedef struct tagVDInfo
	{
		DWORD dwIp;
		WORD  nPort;
		TCHAR szSvrDrv;
		TCHAR szCliDrv;
		DWORD dwConnect;
		DWORD dwVersion;//(time_t);
		TCHAR szTempDir[MAX_PATH];
	}tagVDInfo;

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

	IPlug* WINAPI CreatePlug   (void);

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
}

#endif

#ifndef _DEFINE_H_
#define _DEFINE_H_


#include "string"
#include "NetLayer.h"
using namespace i8desk;
using namespace std;

#define PORT  17791

#define NET_CMD_POSTBLOCKFILE   0x0100
#define NET_CMD_REGISTCLIENT	0x0101
#define NET_CMD_LOGSERVER		0x0102	
#define NET_CMD_GETFILE			0x0103
#define NET_CMD_LOG				0x0104
#define NET_CMD_GETUDISKINFO	0x0105
#define NET_CMD_GETCOMMENTRES	0x0106

#define MET_CMD_GETBLCOKFILE	0x0107
#define NET_CMD_DELETEFILE		0x0108
#define NET_CMD_GETCONFIGINFO	0x0109
#define NET_CMD_RESETPASSWORD	0x010A

#define NET_CMD_SAVEGAMERECORD	0x0110
#define NET_CMD_GETSAVEGAMELST	0x0111

#define NET_CMD_DELETEGAMERECORD 0x0112

#define NET_CMD_GETGAMERECORD	0x0113

#define NET_CMD_FAIL			0x0600
#define NET_CMD_SUCESS			0x0601




#define BLOCK			64*1024
#define USERNAMELEN		64
#define FILENAMELEN		64

struct FileHeader
{
	
	char filename[FILENAMELEN];//文件名
	char FilePath[USERNAMELEN];//存放的文件路径
	DWORD Num;//分块序号
	DWORD DataSize;//数据大小
	DWORD64 FileSize;//文件大小
	bool bEnd;//是否是文件的最后一快，是的话需要关闭文件。
	bool IsPersion;
};

struct FileInfo
{
	std::string filename;
	DWORD filesize;
	DWORD dwTime;
};

typedef struct tagSrvConfigInfo
{
	string			sztitle;		//显示标题
	string			szSavePath;		//个人存档路径
	DWORD			dwAllocSize;	//个人分配空间大小
	bool			bShowPersion;	//是否显示个人存储区
	string			szCustomInfo;	//用户自定义说明。
	bool			bAutoStart;		//是否随系统启动
	bool			bRegist;		//是否可以注册
	string			szStopRigTitle;	//停止注册显示信息
	DWORD			dwDay;			// 删除几天不登陆用户
	DWORD			dwSpace;			//可用空间小于好多停止注册
	string			szCommResPath;	//公共资源路劲
	bool			bShowCommres;  //是否显示公共资源。
	tagSrvConfigInfo():sztitle("测试网吧"),dwAllocSize(100),bShowPersion(true)
					,szCustomInfo("用户自定义"),bAutoStart(true),bRegist(true),
					szStopRigTitle("磁盘空间不够"),dwDay(50),dwSpace(500),bShowCommres(true)
	{
		
	}
}SrvConfigInfo;

#endif
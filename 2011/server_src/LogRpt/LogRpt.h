#ifndef __i8desk_logrpt_inc__
#define __i8desk_logrpt_inc__

#include "../../include/frame.h"
#include "../../include/Zip/XZip.h"

#include <vector>

namespace i8desk
{
	class tagUserInfo
	{
	public:
		//in param.
		DWORD	dwCpuID;				//硬件ID
		stdex::tString szUser;			//用户名
		stdex::tString szPassword;		//密码
		stdex::tString oau1;			//OAU1
		stdex::tString oau2;			//OAU2
		stdex::tString szSrvMac;		//服务器MAC地址
		stdex::tString szSrvVersion;	//服务器程序版本

		//out param.
		DWORD   dwResCode;				//表示用户验证返回的flag值
		DWORD   dwNibID;				//网吧nid		
		DWORD	dwSID;					//网吧服务器sid
		DWORD	dwRID;					//用户区域rid
		DWORD	dwYear;					//用户过期日期年
		DWORD	dwMonth;				//用户过期日期月
		DWORD	dwDay;					//用户过期日期
		bool    dwUploadLog;			//是否上传日志
		stdex::tString szNbName;		//网吧名称:显示到菜单网吧地址的第一行
		stdex::tString szNbProvince;	//网吧所在省
		stdex::tString szNbCity;		//网吧所在市
		stdex::tString szNbRealityUrl;	//网吧实名信息修改网址
		stdex::tString szNbAddress;		//网吧地址：真实的网吧地址
		stdex::tString szNbPhone;		//网吧电话（座机）
		stdex::tString szNbMobile;		//网吧移动电话（手机）
		stdex::tString szNbTitle;		//显示到菜单网吧名称的第二行。
		stdex::tString szNbSubTitle;	//显示到菜单网吧名称的第三行
		stdex::tString szNbTitleUrl;	//网吧首页：点点击菜单网吧地址后，弹出这个url.如果为空，则点周网吧名称后显示公告
		DWORD	dwConShowDialog;		//中心下发给控制台的信息:是否弹出对话框(1|0)
		DWORD   dwConShowCancel;		//中心下发给控制台的信息:是否显示取消按钮
		stdex::tString szConMessage;	//中心下发给控制台的信息:信息内容
		stdex::tString szConOkUrl;		//中心下发给控制台的信息:点确定后弹出的网址
		stdex::tString szOemID;			//渠道id.
		stdex::tString szOemName;		//渠道名称
		stdex::tString szUserType;		//用户类型
		stdex::tString szMessage;		//中心给用户提示信息
		stdex::tString szPlugParam;		//注册插件的参数信息
		stdex::tString szTaskInfo;		//记录开机任务的信息
		stdex::tString szError;			//返回的错误信息(当flag不等于1时)
		stdex::tString szMenuGGao;		//菜单公告弹出控制.p1=-1|0|1(表示按网吧设置|强制关|强制开),p2=n保留
		stdex::tString szMenuPop;		//菜单右下角广告弹出控制.p1=0|1(表示关|开),p2=n保留
		stdex::tString szBotLayer;		//菜单底部广告
		stdex::tString szMenuPage;		//菜单默认页
		stdex::tString szUrlNav;		//网址导航列表
		stdex::tString szVdMus;			//视频音乐列表
		stdex::tString szDotCard;		//点卡商城地址
		stdex::tString szGameRoomUrl;	//游戏大厅后置网址
		stdex::tString szPersonUrl;		//菜单个人用户中心网址

		stdex::tString szNic;			// 网卡名称
		int uAdslSpeed;					// ADSL Speed 0是不上传，-1是不限制，其它是限制速度
	
		int createDay;					// 网吧创建的天数
		int diskType;					// 磁盘类型 1:无盘 2:有盘 3:混合
		stdex::tString diskName;		// 无盘产品名称
		stdex::tString feeName;			// 收费产品名称
		stdex::tString culturalName;	// 安全监控软件名称
		int broadBandType;				// 网络状况1:其它 2:ADSL 3:光纤
		int reality;					// 是否已通过实名 1:是 0:否


		tagUserInfo()
		{
			dwCpuID = 0;
			dwResCode = 0;
			dwNibID	= 0;
			dwSID	= 0;
			dwRID	= 0;
			dwYear  = 0;
			dwMonth	= 0;
			dwDay	= 0;
			dwUploadLog = false;
			dwConShowDialog = 0;
			dwConShowCancel = 0;
			uAdslSpeed = 0;
			createDay = 0;
			diskType = 0;
			broadBandType = 0;
			reality = 0;
		}
	};

	stdex::tString uri_encode(stdex::tString& source);
	inline  stdex::tString uri_encode(LPCTSTR source) { return uri_encode(stdex::tString(source)); }
	void	GetSvrMacAddr(tagUserInfo* pUserInfo);
	stdex::tString GetFileVersion(IN LPCTSTR pFileName);
	stdex::tString GetAppPath();
	void	splitString(const stdex::tString &strSource, std::vector<stdex::tString>& plugs, TCHAR ch);
	int		GetOemId();
	DWORD	IsSystemType();
	DWORD	CheckProcess(LPCTSTR lpszProcName);
	stdex::tString handleStr(stdex::tString &src);
	inline  stdex::tString handleStr(LPCTSTR src) { return handleStr(stdex::tString(src)); }
	stdex::tString GetServerDeviceInfo(tagUserInfo* pUserInfo);
	stdex::tString GetNodeSvrDeviceInfo(ISvrPlugMgr* pPlugMgr);
	void	UpLoadLog(tagUserInfo* pUserInfo, HANDLE hExited);
	void	DeleteMoreLogFile(HANDLE m_hExit);
};

#endif
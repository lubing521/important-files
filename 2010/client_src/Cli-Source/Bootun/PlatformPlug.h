#pragma once

#include "UpdateGameCli.h"
#include <string>
#include <vector>
using namespace std;
namespace i8desk
{

#define       PLATFORMPLUG_CONFIG_FILENAME               TEXT("plugin_config.xml") //插件配置文件名


	//插件信息
	typedef struct tagSTPlugInfo
	{
		DWORD                    dwGameId;                   //插件ID
		DWORD                    dwRunType;                  //插件运行方式
		BOOL                     bEnable;                    //是否启用
		char                     szServerDir[MAX_PATH];      //服务端目录
		char                     szClientDir[MAX_PATH];      //客户端目录
		char                     szExeName[MAX_PATH];        //客户端执行文件
		char                     szExeParam[MAX_PATH];       //执行参数

		tagSTPlugInfo()
		{
			ZeroMemory(this, sizeof(tagSTPlugInfo));			
		}
	}STPlugInfo;

	//平台插件类
	class CPlatformPlug
	{
		//构造析构
	public:
		CPlatformPlug(void);
		virtual ~CPlatformPlug(void);

	public:
		//开始工作
		BOOL                     Start(ILogger* pLogger, IPlugMgr* pPlugMgr, HANDLE hExit, CHyOperator* pRestore);

		//工作线程
	public:
		UINT                     WorkThread();
		static UINT __stdcall    _WorkThread(PVOID lpVoid);

	private:
		//下载配置
		BOOL                     DownloadConfig();
		//解析配置
		BOOL                     ParserConfig();
		//运行客户端
		BOOL                     ExecClient(STPlugInfo stPlugInfo);
		//计算文件是否需要更新
		BOOL                     FileIsNeedUpdate(DWORD dwProgramType, DWORD dwPathType, LPSTR lpRemoteFileName, LPSTR lpLocalFilePath = NULL);
		//下载文件
		BOOL                     DownloadFile( DWORD dwProgramType, DWORD dwPathType, string strRemoteFileName, char** pRecvBuffer, DWORD &dwRecvLen);
		//保存离线信息（包括游戏数据，虚拟盘，系统设置,收藏夹等服务端数据）
		void                     WriteDataFile(string FileName, char* data,bool bInprotect, DWORD datalength = 0);	
		//计算文件CRC
		DWORD	                 GetFileCRC32(const std::string&  szFile);
		//删除目录（穿透还原）
		BOOL                     DeleteDirectory(char *pszDir);
		//运行进程
		BOOL                     RunProcess(LPCSTR lpImage,LPCSTR lpPara);
		BOOL                     GetTokenByName(HANDLE &hToken,LPSTR lpName);

		//内网更新函数指针
	private:
		FASTCOPY_START           FastCopy_Start;
		FASTCOPY_END             FastCopy_End;
		FASTCOPY_GETSPEEDEX      FastCopy_GetSpeedEx;

	private:
		//当前目录
		string                   m_strCurrentDir;
		//主服务IP
		string                   m_strMajorSvrIp;
		//从服务IP
		vector<string>           m_strSubSvrIp;
		//日志插件
		ILogger                 *m_pLogger;
		//插件管理器
		IPlugMgr                *m_pPlugMgr;
		//插件
		vector<tagSTPlugInfo>    m_vtPlugInfo;
		//还原接口
		CHyOperator             *m_pRestore;
		//退出事件
		HANDLE                   m_hExit;
		//线程
		HANDLE                   m_hWorkThread;
		//更新模块
		HMODULE                  m_hUpdateModule;
	};


}









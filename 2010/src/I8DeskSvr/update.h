#ifndef __update_inc__
#define __update_inc__

#include <list>

#include "../../include/plug.h"
#include "plugmgr.h"

namespace i8desk
{
	//////////////////////////////////////////////////////////////////////////
	//中心XML文件列表的配置.当http请求时,返回的是xml文件经过base64编码后的字符串
	/*
		<?xml version="1.0" encoding="gb2312" ?>
		<filelist>
			<!-- file.name表示需要更新的文件，支持%root%,%system$,%program%三个格式 -->
			<!-- file.url表示这个文件的http下载地址 -->
			<!-- file.crc表示这个文件的crc32(16进制的值，前面没有0x) -->
			<!-- file.modifytime表示这个文件的修改时间(utc) -->
			<file name="%root%\a.exe" url="http://www.i8desk.com/a.exe" crc="123456" modifytime = "112233" />
			<file name="%root%\BarOnlineUpdate\wxnda.sys" url="http://www.i8desk.com/wxnda.sys" crc="" modifytime = "112233" />
		</filelist>
	*/
	//////////////////////////////////////////////////////////////////////////
	class CAutoUpate
	{
		//每个文件的信息
		typedef struct tagFileInfo
		{
			std::string		szFileName;			//更新的文件名					(file.name)
			std::string		szParseFile;		//解析后的文件名
			std::string		szTmpName;			//下载时的临时文件名		 
			std::string		szUrl;				//http的地址					(file.url)
			DWORD			dwCrc;				//文件ＣＲＣ.					(file.crc)
			DWORD			dwModifyTime;		//文件修改时间					(file.modifytime)
			tagFileInfo()
			{
			}
		}tagFileInfo;
	public:
		CAutoUpate(ILogger* pLogger, CPlugMgr* pPlugMgr);
		~CAutoUpate();
	public:
		//启动与停止软件自动更新
		bool StartUpdate();
		void StopUpdate();
	private:
		//自动更新的几个步骤
		//和中心通信，取得更新的所有文件列表
		void GetFileList();

		//依次下载所有需要更新的文件到一个临时目录
		bool DownAllFile();

		//把临时文件的文件更新到真正的目录.然后启动自己停止的EXE或者服务.
		void ExecUpdate();

		//更新的文件列表.
		std::list<tagFileInfo> m_lstFileList;

		static UINT __stdcall WorkThread(LPVOID lpVoid);
	private:
		DWORD		GetFileCRC32(const std::string&  szFile);
		void		ProccessFilePath(tagFileInfo* pFileInfo);
		bool		DownLoadFile(tagFileInfo* pFileInfo);
		tstring		GetOpt(LPCTSTR szSection, LPCTSTR szKeyName, LPCTSTR szDefValue);
		void		EnsureFileDirExist(const std::string& file, int offset = 0);
		DWORD		StringToInt(LPCSTR pString, int K = 16);
		tstring		GetFileVersion();
		std::string	uri_encode(LPCTSTR lpszSouce);
	private:
		ILogger*	m_pLogger;
		CPlugMgr*	m_pPlugMgr;
		HANDLE		m_hThread;
		HANDLE		m_hExited;
		bool		m_bNeedReStartService;
	};
}

#endif
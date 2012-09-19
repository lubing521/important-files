#ifndef __update_inc__
#define __update_inc__

#pragma once

#include <list>
#include "plugmgr.h"

namespace i8desk
{
	//////////////////////////////////////////////////////////////////////////
	//中心XML文件列表的配置.当http请求时,返回的是xml文件经过base64编码后的字符串
	/*	url:http://update.i8desk.com/update2/u2.do
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
			stdex::tString		szFileName;			//(file.name)
			stdex::tString		szParseFile;		//(dst file name)
			stdex::tString		szTmpName;			//(download tmpe file name)
			stdex::tString		szUrl;				//(file.url)
			DWORD		dwCrc;				//(file.crc)
			DWORD		dwModifyTime;		//(file.modifytime:time_t)
			tagFileInfo()
			{
			}
		}tagFileInfo;
	public:
		CAutoUpate(CPlugMgr* pPlugMgr, ILogger* pLogger);
		~CAutoUpate();
	public:
		bool StartUpdate();
		void StopUpdate();
	private:
		void GetFileList();
		bool DownAllFile();
		void ExecUpdate();
		std::list<tagFileInfo> m_lstFileList;
		static UINT __stdcall WorkThread(LPVOID lpVoid);
	private:
		DWORD		GetFileCRC32(const stdex::tString&  szFile);
		void		ProccessFilePath(tagFileInfo* pFileInfo);
		bool		DownLoadFile(tagFileInfo* pFileInfo);
		void		EnsureFileDirExist(const stdex::tString& file, int offset = 0);
		DWORD		StringToInt(LPCTSTR pString, int K = 16);
		stdex::tString CAutoUpate::uri_encode(LPCTSTR lpszSouce);
	private:
		CPlugMgr*	m_pPlugMgr;
		ILogger*	m_pLogger;
		HANDLE		m_hThread;
		HANDLE		m_hExited;
		bool		m_bNeedReStartService;
	};
}

#endif
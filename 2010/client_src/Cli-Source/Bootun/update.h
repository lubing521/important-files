#ifndef __update_inc__
#define __update_inc__

#include <list>

#include "..\Include\plug.h"

namespace i8desk
{
	typedef struct tagFileItem//文件信息
	{
		DWORD dwFileCrc32;//文件CRC32
		char FileName[MAX_PATH];//文件名称
	}tagFileItem;

	typedef struct tagIdxFile//更新文件列表结构
	{
		DWORD dwCrc32;//文件列表的CRC
		DWORD dwVersion;//更新版本号
		DWORD dwFileCount;//更新文件数
		tagFileItem FileList[1];//文件列表
	}tagIdxFile;

	class CAutoUpdate
	{
		//对比文件的方式
		typedef enum 
		{
			CMP_FILE_CRC32			= 0,		//按文件的CRC32来比较
			CMP_FILE_SIZE_MODTIME	= 1,		//按文件大小修改时间来对比
			CMP_FILE_VERSION		= 2			//按文件版本来比较		
		}E_CMP_TYPE;

		//对比文件的条件
		typedef enum
		{
			CON_EQUAL				= 0,		//等于
			CON_NOT_EQUAL			= 1,		//不等于
			CON_LESS				= 2,		//小于
			CON_LESS_EQUAL			= 3,		//小于等于
			CON_GREATER				= 4,		//大于
			CON_GREATER_EQUAL		= 5			//大于等于
		}E_CMP_CONDIV;

		//每个文件的信息
		typedef struct tagFileInfo
		{
			bool			NeedUpdate;			//该文件是否需要更新
			std::string		szFileName;			//更新的文件名
			std::string		szTmpName;			//下载时的临时文件名
			std::string		szRelPath;			//http的地址
			E_CMP_TYPE		dwCmpType;			//对比方式
 			//E_CMP_CONDIV	dwCmpCondiv;		//对比条件
			std::string		szParam1;			//参数1.	文件CRC,大小，版本
			std::string		szParam2;			//参数2.	文件的修改时间
			tagFileInfo()
			{
				NeedUpdate	= false;
				dwCmpType	= CMP_FILE_CRC32;
				//dwCmpCondiv = CON_EQUAL;
			}
		}tagFileInfo;
	public:
		CAutoUpdate(ILogger* pLogger, IPlugMgr* pPlugMgr,CHyOperator* pHy);
		~CAutoUpdate();
	public:
		//启动与停止软件自动更新
		bool StartUpdate();
		void StopUpdate();
	private:
		//自动更新的几个步骤
		//和中心通信，取得更新的所有文件列表
		bool GetFileList();

		//对比后，得到需要更新的文件列表
		void GetNeedUpdateList();

		//依次下载所有需要更新的文件到一个临时目录
		bool DownAllFile();

		//保存已下载的文件;
		void WriteDataFile(tstring FileName, char* data, DWORD datalength);

		//把临时文件的文件更新到真正的目录.然后启动自己停止的EXE或者服务.
		bool ExecUpdate();

		//是否穿透还原更新
		bool NeedInProtUpdate(tstring path);

		//更新的文件列表.
		std::list<tagFileInfo> m_lstFileList;

		static UINT __stdcall WorkThread(LPVOID lpVoid);
	private:
		std::string GetFileVersion(const std::string& szFile);
		DWORD		GetFileSizeModifyTime(const std::string& szFile, DWORD& dwModTime);
		DWORD		GetFileCRC32(const std::string&  szFile);
		void		ProccessFilePath(std::string& src,std::string& relPath);
		bool		DownLoadFile(tagFileInfo* pFileInfo);
		tstring		GetOpt(LPCTSTR szSection, LPCTSTR szKeyName, LPCTSTR szDefValue);
		void		EnsureFileDirExist(const std::string& file, int offset = 0);
		void		DelUpdateTmpFile(std::string& FilePath);
		BOOL        DownloadFile( DWORD dwProgramType, DWORD dwPathType, string strRemoteFileName, char** pRecvBuffer, DWORD &dwRecvLen);

	private:
		ILogger*	m_pLogger;
		IPlugMgr*	m_pPlugMgr;
		CHyOperator* m_pHy;
		HANDLE		m_hThread;
		HANDLE		m_hExited;

	};
}

#endif
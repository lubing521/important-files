#include "stdafx.h"
#include "GameFileDeletor.h"

#include <deque>

#include "PLInfo.h"
#include "LogHelper.h"
#include "ThreadPoolHelper.h"

#include "../../../include/Http/HttpHelper.h"
#include "../../../include/Extend STL/StringAlgorithm.h"
#include "../../../include/Utility/utility.h"
#include "../../../include/Win32/FileSystem/FileHelper.hpp"
#include "../../../include/Utility/SmartPtr.hpp"
#include "../../../include/I8Type.hpp"
#include "../../../include/UpdateGame.h"

#include "DownloadEngine.h"
#include "DelClientGame.h"

namespace i8desk
{


	namespace 
	{
		template< typename EventT >
		inline bool IsStop(EventT &event)
		{
			if( event.WaitForEvent(0) )
			{
				return true;
			}

			return false;
		}
	}

	GameFileDeletor::GameFileDeletor(IRTDataSvr *rtDataSvr, ISvrPlugMgr *plugMgr, ISysOpt *sysOpt, DownloadEngine *downEngine, DelClientGame *delClient)
		: rtDataSvr_(rtDataSvr)
		, plugMgr_(plugMgr)
		, sysOpt_(sysOpt)
		, downEngine_(downEngine)
		, delClient_(delClient)
	{
		assert(rtDataSvr_);
		assert(plugMgr_);
		assert(downEngine_);
	}
	GameFileDeletor::~GameFileDeletor()
	{

	}


	void GameFileDeletor::Start()
	{
		_Init();
	}

	void GameFileDeletor::Stop()
	{
		exit_.SetEvent();

		thread_.Stop();
		Log(LM_DEBUG, _T("GameFileDeletor 退出成功"));
	}

	void GameFileDeletor::Run()
	{
		Log(LM_DEBUG, _T("设置删除游戏事件..."));
		waitHandle_.SetEvent();
	}

	void GameFileDeletor::DeleteTmpFiles()
	{
		delTmpFiles_.SetEvent();
	}

	void GameFileDeletor::_Init()
	{
		stdex::tString strTmpPath = utility::GetAppPath() + _T("tmp\\");

		if( !::PathFileExists(strTmpPath.c_str()) ) 
		{
			if( !::CreateDirectory(strTmpPath.c_str(), NULL) ) 
				throw std::runtime_error("创建tmp目录失败");
		}

		if( !exit_.Create() )
			throw std::runtime_error("创建事件失败");

		if( !waitHandle_.Create() )
			throw std::runtime_error("创建事件失败");

		if( !delTmpFiles_.Create() )
			throw std::runtime_error("创建事件失败");

		thread_.RegisterFunc(std::tr1::bind(&GameFileDeletor::_Thread, this));
		thread_.Start();
	}


	void GameFileDeletor::_DeleteGameImpl(const std::tr1::tuple<long, stdex::tString, DWORD> &info)
	{
		if( IsStop(exit_) )
			return;

		long gid = std::tr1::get<0>(info);
		stdex::tString path = std::tr1::get<1>(info);
		DWORD delFlag = std::tr1::get<2>(info);

		if(delFlag == DelNone)
			return;


		Log(LM_DEBUG, _T("正在删除游戏... GID: %ld, delFlag: %d"), gid, delFlag);
		
		if( delFlag & DelTmp )
		{
			// 得到三层资源临时目录
			TCHAR tmpDir[MAX_PATH] = {0};
			sysOpt_->GetOpt(OPT_D_TEMPDIR, tmpDir);
			::PathAddBackslash(tmpDir);

			stdex::tOstringstream os;
			os << tmpDir << gid << _T("\\");
			stdex::tString path = os.str();

			Log(LM_DEBUG, _T("删除临时目录 Path = %s\n"), path.c_str());

			win32::file::DeleteDirectory(path.c_str());
		}
		
		if( delFlag & DelLocal  )
		{
			Log(LM_DEBUG, _T("删除服务器上信息和文件 SvrPath = %s\n"), path.c_str());

			// 删除服务器上信息和文件
			if( ::PathIsDirectory(path.c_str()) )
			{
				win32::file::DeleteDirectory(path.c_str());
			}
			
			Log(LM_DEBUG, _T("删除图标数据\n"));

			// 删除图标数据
			ISvrPlug *pRTDataSvr = plugMgr_->FindPlug(RTDATASVR_PLUG_NAME);
			assert(pRTDataSvr);
			if( pRTDataSvr == 0 )
			{
				Log(LM_ERROR, _T("没有实时数据服务插件\n"));
				return ;
			}

			if( !pRTDataSvr->SendMessage(RTDS_CMD_ICON_OPERATE, RTDS_CMD_ICON_OPDELETE, gid) )
			{
				Log(LM_ERROR, _T("删除图标出错 %d"), gid);
			}
	
		}

		if( delFlag & DelSync  )
		{	
			Log(LM_DEBUG, _T("通知同步插件删除从服务器上的游戏记录\n"));

			// 通知同步插件删除从服务器上的游戏记录
			if( ISvrPlug *pSyncPlug = plugMgr_->FindPlug(PLUG_UPTSVR_NAME) )
			{
				pSyncPlug->SendMessage(CMD_UPT_SM_NOTIFYDELETE, gid, 0);
				Log(LM_DEBUG, _T("已通知同步插件删除游戏: %d"), gid);
			}
		}

		db::tGame game;
		rtDataSvr_->GetGameTable()->GetData(gid, &game);


		if( delFlag & DelClient  )
		{
			Log(LM_DEBUG, _T("添加客户机删除文件\n"));
			IClientRecordset *record = 0;
			rtDataSvr_->GetClientTable()->Select(&record, 0, 0);
			for(size_t i = 0; i != record->GetCount(); ++i)
			{
				const db::tClient *client = record->GetData(i);
				delClient_->Add(gid, client->Name, game.CliPath);
			}
			record->Release();
		}

		if( delFlag & DelIdc )
		{
			 if(rtDataSvr_->GetGameTable()->Delete(gid))
				 Log(LM_ERROR, _T("删除数据库中游戏记录失败 gid = %d\n"), gid);
			 else
				 Log(LM_DEBUG, _T("删除数据库记录成功 GID = %ld\n"), gid);
		}

		if( delFlag & DelRecord )
		{
			// 删除数据记录
			downEngine_->DeleteTask(gid);

			// 删除游戏记录
			utility::Strcpy(game.CliPath, _T(""));
			utility::Strcpy(game.SvrPath,  _T(""));
			game.Status		= 0;
			game.SvrVer		= 0;
			if( rtDataSvr_->GetGameTable()->Update(gid, &game, MASK_TGAME_CLIPATH | MASK_TGAME_SVRPATH | MASK_TGAME_STATUS | MASK_TGAME_SVRVER ) )
				Log(LM_ERROR, _T("删除数据库中游戏记录失败 gid = %d\n"), gid);
			else
				Log(LM_DEBUG, _T("删除数据库记录成功 GID = %ld\n"), gid);
		}


	}


	void GameFileDeletor::_DeleteTmpFiles()
	{
		// 得到三层资源临时目录
		TCHAR tmpDir[MAX_PATH] = {0};
		sysOpt_->GetOpt(OPT_D_TEMPDIR, tmpDir);
		::PathAddBackslash(tmpDir);
		
		// 得到三层正在下载文件
		TasksInfo tasks;
		downEngine_->GetTasksInfo(tasks);

		// 循环删除临时文件
		std::vector<stdex::tString> filesPath;
		filesystem::CFindFile file;
		stdex::tString tmpPath = tmpDir;
		tmpPath += _T("*");
		BOOL suc = file.FindFile(tmpPath.c_str());
		while(suc)
		{
			suc = file.FindNextFile();

			if( file.IsDots() )
				continue;

			struct FindIF
			{
				long gid_;
				FindIF(LPCTSTR fileName)
					: gid_(_ttoi(fileName))
				{}

				bool operator()(const TaskInfo &task) const
				{
					return task.gid_ == gid_;
				}
			};

			TasksInfo::const_iterator iter = std::find_if(tasks.begin(), tasks.end(), FindIF(file.GetFileName().c_str()));
			if( iter == tasks.end() && file.IsDirectory() )
				filesPath.push_back(file.GetFilePath());
		}

		// 删除目标临时文件
		win32::file::detail::CheckFile checker;
		win32::file::detail::DeleteFileT deletor;

		typedef win32::file::FileDepthSearch<win32::file::detail::CheckFile, win32::file::detail::DeleteFileT> FilesDeletor;
		FilesDeletor filesOperator(checker, deletor, true);

		std::for_each(filesPath.begin(), filesPath.end(), 
			std::tr1::bind(&FilesDeletor::Run, &filesOperator, std::tr1::placeholders::_1));

		const std::vector<stdex::tString> &delPaths = checker.GetDirectorys();
		for( std::vector<stdex::tString>::const_iterator iter = delPaths.begin(); iter != delPaths.end(); ++iter )
			::RemoveDirectory(iter->c_str());

		for( std::vector<stdex::tString>::const_iterator iter = filesPath.begin(); iter != filesPath.end(); ++iter )
			::RemoveDirectory(iter->c_str());
	}

	void GameFileDeletor::_DeleteGame()
	{
		using namespace std::tr1::placeholders;

		DeleteInfo::GetInstance().ForEach(std::tr1::bind(&GameFileDeletor::_DeleteGameImpl, this, _1));
	}

	DWORD GameFileDeletor::_Thread()
	{
		HANDLE handle[] = { waitHandle_, delTmpFiles_, exit_ };

		while( !thread_.IsAborted() )
		{
			try
			{
				DWORD ret = ::WaitForMultipleObjects(_countof(handle), handle, FALSE, DELETE_FILES_TIMEOUT);
				if( ret == WAIT_OBJECT_0  || ret == WAIT_TIMEOUT )
				{
					_DeleteGame();
					continue;
				}
				else if( ret == WAIT_OBJECT_0 + 1 )
				{
					Log(LM_INFO, _T("删除三层临时文件..."));

					_DeleteTmpFiles();

					Log(LM_INFO, _T("删除三层临时文件完成"));
					continue;
				}
				else if( ret == WAIT_OBJECT_0 + 2 )
				{
					break;
				}
				else
				{
					assert(0);
				}
			}
			catch(std::exception &e)
			{
				Log(LM_ERROR, CA2T(e.what()));
			}
			catch(...)
			{
				Log(LM_ERROR, _T("删除中心指定游戏发生未知错误!"));
			}
		}

		return 0;
	}
}
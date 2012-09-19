#include "stdafx.h"
#include "GameMgr.h"

#include "../../../include/UpdateGame.h"
#include "../../../include/BusinessInfo.h"
#include "../../../include/Serialize/Serialize.hpp"

#include "DataSvrHelper.h"
#include "LogHelper.h"

#include "GameDiskMonitor.h"
#include "GameFileDeletor.h"
#include "MscRateMonitor.h"
#include "KHRateMonitor.h"
#include "DownloadEngine.h"
#include "PLControl.h"
#include "TaskProductor.h"
#include "CenterDel.hpp"
#include "PLInfo.h"
#include "ForceGameZip.h"
#include "GameComplateHelper.h"
#include "GameTableCache.h"
#include "DelClientGame.h"


namespace i8desk
{


	namespace game_mgr
	{
		stdex::tString PLUG_NAME		= _T("GameMgr");
		stdex::tString PLUG_COMPANY		= _T("GoYoo");
		stdex::tString PLUG_DESCRIPTION	= _T("Game Manage Plug");
		u_long PLUG_VERSION				= 0x01000000;
	}


	
	GameMgr::GameMgr()
		: plugMgr_(0)
		, rtDataSvr_(0)
		, sysOpt_(0)
	{
	}

	GameMgr::~GameMgr()
	{

	}


	//释放插件
	void GameMgr::Release()
	{
		delete this;
	}

	//得到插件的名字
	LPCTSTR GameMgr::GetPlugName()
	{
		return game_mgr::PLUG_NAME.c_str();
	}
	//得到插件的版本0xaabbccdd.
	DWORD  	GameMgr::GetVersion() 
	{
		return game_mgr::PLUG_VERSION;
	}
	//得到插件开发的公司名
	LPCTSTR GameMgr::GetCompany()
	{
		return game_mgr::PLUG_COMPANY.c_str();
	}
	//得到插件的一些描述
	LPCTSTR GameMgr::GetDescription() 
	{
		return game_mgr::PLUG_DESCRIPTION.c_str();
	}

	//如果插件需要框架监听的网络端口，如果不需要监听，则返回0.
	//当框架收到该端口事件时，会回调给该接口的INetLayerEvent对应的虚函数
	DWORD   GameMgr::GetListPort()
	{
		return 0;
	}

	//插件内部的初始化工作,当ISvrPlugMgr加载一个插件后会调用该函数
	bool GameMgr::Initialize(ISvrPlugMgr* pPlugMgr)
	{
		//__asm { int 3 }

		return _InitPlug(pPlugMgr) && _InitOther();
	}


	bool GameMgr::_InitPlug(ISvrPlugMgr *plugMgr)
	{
		assert(plugMgr);
		if( plugMgr == 0 )
			return false;

		plugMgr_ = plugMgr;
		sysOpt_  = plugMgr_->GetSysOpt();
		assert(sysOpt_);
		
		// 初始化日志组件
		log::LogHelper::GetInstance(plugMgr_);

		Log(LM_INFO, _T("GameMgr 正在启动...\n"));

		ISvrPlug *pRTDataSvr = plugMgr_->FindPlug(RTDATASVR_PLUG_NAME);
		assert(pRTDataSvr);
		if( pRTDataSvr == 0 )
		{
			Log(LM_ERROR, _T("没有实时数据服务插件\n"));
			return false;
		}
		
		rtDataSvr_ = reinterpret_cast<IRTDataSvr *>(
			pRTDataSvr->SendMessage(RTDS_CMD_GET_INTERFACE, RTDS_INTERFACE_RTDATASVR, 0));
		assert(rtDataSvr_);

		Log(LM_INFO, _T("GameMgr 启动完成\n"));
		return true;
	}

	bool GameMgr::_InitOther()
	{
		using namespace std::tr1::placeholders;

		try
		{
			// 删除游戏
			DeleteInfo::GetInstance(rtDataSvr_);

			// 启动游戏磁盘监测
			gameDiskMonitor_.reset(new GameDiskMonitor(rtDataSvr_, sysOpt_));
			gameDiskMonitor_->Start();

			// 启动三层下载限速
			//mscRateMonitor_.reset(new MscRateMonitor(rtDataSvr_->GetSysOptTable()));
			//mscRateMonitor_->Start();

			// 启动KH下载限速
			khRateMonitor_.reset(new KHRateMonitor(rtDataSvr_->GetSysOptTable()));
			khRateMonitor_->Start();


			// 删除客户机游戏文件
			delClientGame_.reset(new DelClientGame);
			delClientGame_->Start();

			// 启动强推ZIP
			unZipGame_.reset(new UnZipGame(rtDataSvr_));
			unZipGame_->Start();
			unZipGame_->RegisterCallback(std::tr1::bind(&GameComplateHelper, unZipGame_.get(), plugMgr_, sysOpt_, delClientGame_.get(), _1));

			// 启动PL下载解析
			plControl_.reset(new PLControl(sysOpt_, rtDataSvr_));
			plControl_->Start(std::tr1::bind(&UnZipGame::Push, unZipGame_.get(), _1, _2, _3));

			// 启动下载引擎
			downloadEgine_.reset(new DownloadEngine(plugMgr_, rtDataSvr_, sysOpt_));
			downloadEgine_->Start();
			downloadEgine_->Register(std::tr1::bind(&GameComplateHelper, unZipGame_.get(), plugMgr_, sysOpt_, delClientGame_.get(), _1));

			// 启动下载中心指定XML
			ISvrPlug *logRpt = plugMgr_->FindPlug(PLUG_LOGRPT_NAME);
			centerDel_.reset(new CenterDel(sysOpt_, logRpt));
			centerDel_->Start();



			// 启动冗余游戏文件删除
			gameFileDeletor_.reset(new GameFileDeletor(rtDataSvr_,  plugMgr_, sysOpt_, downloadEgine_.get(), delClientGame_.get()));
			gameFileDeletor_->Start();

			// 启动任务产生器
			taskProductor_.reset(new TaskProductor(rtDataSvr_->GetGameTable(), sysOpt_, unZipGame_.get(),
				std::tr1::bind(&DownloadEngine::AddTask, downloadEgine_.get(), _1, _2, _3, _4, _5, _6, _7, _8, _9)));
			taskProductor_->Start();
			
			// 注册回调
			plControl_->RegisterCallback(std::tr1::bind(&GameFileDeletor::Run, gameFileDeletor_.get()));
			plControl_->RegisterCallback(std::tr1::bind(&TaskProductor::ForceCheck, taskProductor_.get()));
			plControl_->RegisterCallback(std::tr1::bind(&GameMgr::_RefreshPL, this));

			// 启动业务交互
			cmdOperate_.reset(new cmd_operate::CmdOperate(plugMgr_, 
				std::tr1::bind(&GameDiskMonitor::Update, gameDiskMonitor_.get()),
				std::tr1::bind(&PLControl::SetForceRefresh, plControl_.get()),
				std::tr1::bind(&GameFileDeletor::Run, gameFileDeletor_.get()),
				downloadEgine_.get()));
		}
		catch(std::exception &e)
		{
			Log(LM_ERROR, CA2T(e.what()));
			return false;
		}

		return true;
	}

	void GameMgr::_RefreshPL()
	{
		plugMgr_->FindPlug(RTDATASVR_PLUG_NAME)->SendMessage(RTDS_CMD_REFRESH_PL, 0, 0);
	}


	bool GameMgr::UnInitialize() 
	{
		Log(LM_INFO, _T("GameMgr 正在退出..."));

		cmdOperate_.reset();
		
		plControl_->Stop();
		plControl_.reset();

		taskProductor_->Stop();
		taskProductor_.reset();

		centerDel_->Stop();
		centerDel_.reset();

		gameFileDeletor_->Stop();
		gameFileDeletor_.reset();

		downloadEgine_->Stop();
		downloadEgine_.reset();

		unZipGame_->Stop();
		unZipGame_.reset();

		delClientGame_->Stop();
		delClientGame_.reset();

		/*mscRateMonitor_->Stop();
		mscRateMonitor_.reset();*/

		khRateMonitor_->Stop();
		khRateMonitor_.reset();
		
		gameDiskMonitor_->Stop();
		gameDiskMonitor_.reset();

		Log(LM_INFO, _T("GameMgr 退出成功"));
		return true;
	}

	//主框架或者其它插件可以通过该函数来相互交互。类似于Windows的消息机制,
	//根据cmd的不同，param1,parma2实际的意义与不同.
	//如果需要在插件内部分配内存返回给调用者。则插件内部通过CoTaskMemFree来分配内存，
	//调用者通过CoTaskMemFree来释放插件分配的内存，因此插件接口不再提供其它内存分配释放内存.
	DWORD GameMgr::SendMessage(DWORD cmd, DWORD param1, DWORD param2)
	{
		if( cmdOperate_->RecvCmd(cmd, (LPCSTR)param1, param2) )
			return 1;

		switch(cmd)
		{
			// 内网发送强制修复命令
		case CMD_UPT_SM_NOFIGY3UPFORCE:
			{
				uint32 gid = static_cast<uint32>(param1);

				// 如果正在三层中则不进行添加
				TasksInfo tasksInfo;
				downloadEgine_->GetTasksInfo(tasksInfo);
				for(TasksInfo::const_iterator iter = tasksInfo.begin();
					iter != tasksInfo.end(); ++iter)
				{
					if( iter->gid_ == gid )
						return 0;
				}

				// 由内网更新组件发送强制修复操作命令
				
				Log(LM_DEBUG, _T("内网更新组件请求强制修复游戏, GID=%u\n"), gid);

				if( gid < MIN_IDC_GID ) 
				{
					Log(LM_ERROR, _T("非中心资源无法强制修复, GID=%u\n"), gid);
					return (DWORD)-1;
				}


				db::tGame g;
				if ( rtDataSvr_->GetGameTable()->GetData(gid, &g) == 0 )
					return taskProductor_->ForceUpdateGame(gid, param2, g);
				else
				{
					Log(LM_ERROR, _T("内网更新组件请求强制修复游戏时没有找到该游戏, GID=%u\n"), gid);
					return 0;
				}
			}
			break;

		case UP_SPEED:
			{
				// 总的上传速度
				std::pair<long, long> speed = downloadEgine_->GetMscSpeed();
				return speed.first;
			}
			break;

		case DOWN_SPEED:
			{
				// 总的下载速度
				std::pair<long, long> speed = downloadEgine_->GetMscSpeed();
				return speed.second;
			}
			break;

			// 获取三层下载信息
		case DOWNLOAD_STATUS:
			{
				TasksInfo tasksInfo;
				downloadEgine_->GetTasksInfo(tasksInfo);

				// 多留一个作为结束标志
				const size_t count = tasksInfo.size() + 1;
				TaskInfo *tmp = reinterpret_cast<TaskInfo *>(::CoTaskMemAlloc(sizeof(TaskInfo) * count));
				std::uninitialized_fill_n(tmp, count, TaskInfo());
				std::copy(tasksInfo.begin(), tasksInfo.end(), tmp);

				return (DWORD)tmp;
			}
			break;

			// 游戏盘更新
		case UPDATE_DISK:
			{
				gameDiskMonitor_->UpdateStatus();
			}
			break;

			// PL版本号
		case PL_VERSION:
			{
				const wchar_t *pl = data_helper::GetPLVersion(PLInfo::GetInstance()).c_str();
				return (DWORD)pl;
			}
			break;
			// 删除临时目录文件
		case DELETE_FILE:
			{
				gameFileDeletor_->DeleteTmpFiles();
			}
			break;

			// 智能删除
		case SMART_CLEAN:
			{
				size_t cnt = param1;
				std::vector<long> gids(cnt);

				long *allGids = (long *)param2;
				std::copy(allGids, allGids + cnt, &gids[0]);

				for(size_t i = 0; i != cnt; ++i)
				{
					Log(LM_DEBUG, _T("添加删除游戏: %ld"), gids[i]);
					DeleteInfo::GetInstance().PushGame(gids[i], DelLocal | DelRecord);
				}

				gameFileDeletor_->Run();
			}
			break;

		case GAMEEX_VERSION:
			{
				DWORD cnt = GameTableCache::Instance().size();
				size_t len = cnt * sizeof(GameExInfoVersion_st) + sizeof(DWORD);
				void *buf = ::CoTaskMemAlloc(len);

				std::copy(&cnt, &cnt + 1, (DWORD *)buf);

				DWORD index = 0;
				for(GameTableCache::GamesTable::const_iterator iter = GameTableCache::Instance().begin();
					iter != GameTableCache::Instance().end(); ++iter)
				{
					GameExInfoVersion_st version = { iter->first, iter->second.GameExVersion };
					char *tmp = (char *)buf + sizeof(DWORD) + sizeof(GameExInfoVersion_st) * index;
					std::copy(&version, &version + 1, (GameExInfoVersion_st *)tmp);

					++index;
					if( index >= cnt )
						break;
				}
				return (DWORD)buf;

			}
			break;

		case MSC_TMP_SIZE:
			{
				static ulonglong tmpSize = 0;
				tmpSize = gameDiskMonitor_->MscTmpSize();

				return (DWORD)&tmpSize;
			}

		case CLEINT_DELETE_GAME:
			{

				int cmdtype = param1;
				

				DWORD gid = 0;
				stdex:: tString name,path;
				
				switch(cmdtype)
				{
				case i8desk::Add:
					assert(0);
					break;
				case Del:
					{
						std::vector<long> Gids;
						char *buf = reinterpret_cast<char *>(param2);
						size_t cnt = 0;
						memcpy(&cnt, buf, sizeof(size_t));

						utility::serialize::Serialize os(buf + sizeof(size_t), cnt);

						os >> name >> Gids;

						for(std::vector<long>::const_iterator iter = Gids.begin(); iter != Gids.end(); ++iter)
						{
							delClientGame_->Delete(*iter, name);
						}

					}
					break;
				case Find:
					{
						TCHAR *client = reinterpret_cast<TCHAR*>(param2);
						name = client;


						typedef std::vector<std::pair<long, stdex::tString>> DelClientGame;
						DelClientGame DelGame = delClientGame_->Find(name);

						const size_t cnt = DelGame.size() * (sizeof(long) + sizeof(size_t) + MAX_NAME_LEN) + sizeof(size_t) * 2;
						char *buf = (char *)::CoTaskMemAlloc(cnt);

						utility::serialize::Serialize os(buf, cnt);
						os << cnt << DelGame;

						return (DWORD)buf;
					}
				}
	
			}
			break;
		case CON_SAVE_DATA:
				downloadEgine_->Save();
			break;
		default:
			assert(0);
			break;
		}
		

		return 0;
	}


	void GameMgr::OnAccept(i8desk::INetLayer*pNetLayer, SOCKET sck, DWORD param)
	{}

	void GameMgr::OnSend(i8desk::INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param)
	{}

	void GameMgr::OnRecv(i8desk::INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param)
	{}

	void GameMgr::OnClose(i8desk::INetLayer*pNetLayer, SOCKET sck, DWORD param)
	{}
}
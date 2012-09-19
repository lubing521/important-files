#include "stdafx.h"
#include "GameComplateHelper.h"

#include "resource.h"

#include "../../../include/UpdateGame.h"
#include "../../../include/BusinessInfo.h"

#include "DataSvrHelper.h"
#include "ForceGameZip.h"
#include "CmdOperate.h"
#include "LogHelper.h"
#include "GameTableCache.h"
#include "PLInfo.h"
#include "DelClientGame.h"


namespace i8desk
{

	ConsoleInfo &ConsoleInstance()
	{
		static ConsoleInfo console;
		return console;
	}

	void GetIcon(IGameTable *gameTable, ISvrPlug *rtDataSvr, long gid)
	{
		assert(gameTable);
		assert(rtDataSvr);

		stdex::tString svrPath;
		stdex::tString strGid;
		stdex::ToString(gid, strGid);

		if( !data_helper::GetSvrPathFromGid(gameTable, gid, svrPath) )
		{
			std::ostringstream os;
			os << "获取图标路径失败: gid = " << gid;
			throw std::runtime_error(os.str());
		}

		// 检测路径
		if( svrPath[svrPath.size() - 1] != '\\' )
			svrPath += _T("\\");

		stdex::tString exeName;
		if( !data_helper::GetExeNameFromGid(gameTable, gid, exeName) )
		{
			std::ostringstream os;
			os << "获取图标路径失败: gid = " << gid;
			throw std::runtime_error(os.str());
		}

		svrPath += exeName;


		stdex::tString iconPath = svrPath.c_str();
		utility::CAutoIcon hIcon = LoadIconFromFile(iconPath.c_str());

		if( !hIcon.IsValid() ) 
		{
			hIcon = ::LoadIcon(::GetModuleHandle(PLUG_GAMEMGR_MODULE), MAKEINTRESOURCE(IDI_ICON_DEFAULT));
		}

		db::tIcon icon;
		icon.gid = gid;
		SaveIconToBuffer(hIcon, static_cast<BYTE *>(icon.data), (size_t&)icon.size);

		// 更新临时图标表及缓存图标列表
		if( !rtDataSvr->SendMessage(RTDS_CMD_ICON_OPERATE, RTDS_CMD_ICON_OPENSURE, reinterpret_cast<DWORD>(&icon)) )
			assert(0);

	}

	void ChangeData(IRTDataSvr *rtDataSvr, ISysOpt *sysOpt, long gid, ulong version, bool isPush, const stdex::tString &svrPath, TaskType taskType)
	{
		db::tGame g;

		if( rtDataSvr->GetGameTable()->GetData(gid, &g) != 0 ) 
			return;

		// 更改为本地状态及SvrVer
		g.Status	= 1;
		g.SvrVer	= version;
		uint64 mask = MASK_TGAME_SVRVER | MASK_TGAME_STATUS;

		// 三层强推的任务要设置为自动更新，且要设置为需要同步任务游戏
		if( isPush ) 
		{
			g.AutoUpt = 1;
			mask |= MASK_TGAME_AUTOUPT;
		}

		// 下载完成后强制更新Exe及Param字段
		mask |= MASK_TGAME_EXE;
		utility::Strcpy(g.Exe, GameTableCache::Instance(g.GID).Exe);

		mask |= MASK_TGAME_PARAM;
		utility::Strcpy(g.Param, GameTableCache::Instance(g.GID).Param);

		// 如果数据库的服务端路径与实际下载的路径不一致，则更新
		if( utility::IsEmpty(g.SvrPath) && svrPath != g.SvrPath ) 
		{
			utility::Strcpy(g.SvrPath, svrPath.c_str());
			mask |= MASK_TGAME_SVRPATH;
		}

		// 如果数据库的客户端路径为空，则设置缺省的
		// 缺省路径为内网更新盘符+类别名+游戏名
		stdex::tString gameName;
		data_helper::GetGameNameFromGid(rtDataSvr->GetGameTable(), gid, gameName);

		stdex::tString className;
		stdex::tString svrClassPath;
		data_helper::GetClassPathFromGid(rtDataSvr->GetGameTable(), rtDataSvr->GetClassTable(), gid, svrClassPath, className);

		stdex::tString clientPath;
		TCHAR disk[32] = {0};
		sysOpt->GetOpt(OPT_M_GAMEDRV, disk, _T("E"));

		clientPath += disk;
		clientPath += _T(":\\") + className + _T("\\") + gameName;
		if( utility::IsEmpty(g.CliPath) ) 
		{
			utility::Strcpy(g.CliPath, clientPath.c_str());
			mask |= MASK_TGAME_CLIPATH;
		}

		//运行方式
		stdex::tString szAreaRunType; 
		TCHAR defaultRunType[1024] = {0};
		sysOpt->GetOpt(OPT_D_AREADEFRUNTYPE, defaultRunType);
		szAreaRunType = defaultRunType;

		for(;;)
		{
			size_t pos = szAreaRunType.find_first_of('|');
			if (pos == stdex::tString::npos)
				pos = szAreaRunType.size();

			stdex::tString Item = szAreaRunType.substr(0, pos);
			if (Item.empty())
				break;

			size_t pos_aid = Item.find_first_of(',');
			stdex::tString szAid = Item.substr(0, pos_aid);
			stdex::tString szRunType = Item.substr(pos_aid + 1, Item.size());

			GameRunType runType;
			if( szRunType == _T("0") )
				runType = GAME_RT_NONE;
			else if( szRunType == _T("1") )
				runType = GAME_RT_LOCAL;
			else if( szRunType == _T("3") )
				runType = GAME_RT_DIRECT;
			else
				runType = GAME_RT_VDISK;

			// 是否已经设置
			db::tRunType ga;
			bool bExsits = rtDataSvr->GetRunTypeTable()->GetData(gid, szAid.c_str(), &ga) == 0;

			// 对该游戏没有设置运行方式
			if( !bExsits )
			{
				// 未设置时添加
				if( taskType == tForceTask || taskType == tSmartForceTask) 
				{
					// 如果网吧没设置运行方式，则更改为按网吧默认设置更新数据库
					ga.Type = runType;
				}
				else
				{
					ga.Type = GAME_RT_NONE;
				}

				utility::Strcpy(ga.AID, szAid.c_str());
				ga.GID = gid;
				utility::Strcpy(ga.VID, runType == GAME_RT_VDISK ? szRunType.c_str() : _T(""));
				rtDataSvr->GetRunTypeTable()->Insert(&ga);
			}
			else if( (taskType == tForceTask || taskType == tSmartForceTask)
				&& (ga.Type == GAME_RT_NONE || ga.Type == GAME_RT_DIRECT) ) 
			{
				// 如果网吧没设置运行方式，则更改为按网吧默认设置更新数据库
				ga.Type  = runType;
				rtDataSvr->GetRunTypeTable()->Update(gid, szAid.c_str(), &ga, MASK_TRUNTYPE_TYPE);
			}

			if( pos == szAreaRunType.size() )
			{
				break;
			}
			szAreaRunType = szAreaRunType.substr(pos + 1, szAreaRunType.size());
		}


		if( rtDataSvr->GetGameTable()->Update(gid, &g, mask) == 0 )
		{
			Log(LM_INFO, _T("下载完成.GID = %d\tIdcVer = %d\tSvrVer = %d\n"), 
				gid, version, g.SvrVer);
		}
	}


	void GameComplateHelper(const UnZipGame *unZipGame, ISvrPlugMgr *plugMgr, ISysOpt *sysOpt, DelClientGame *delClientGame, const CallbackType &type)
	{
		ISvrPlug *rtPlug = plugMgr->FindPlug(RTDATASVR_PLUG_NAME);
		ISvrPlug *syncPlug = plugMgr->FindPlug(PLUG_UPTSVR_NAME);
		ISvrPlug *businessPlug = plugMgr->FindPlug(PLUG_BUSINESS_NAME);

		IRTDataSvr *rtDataSvr = reinterpret_cast<IRTDataSvr *>(
			rtPlug->SendMessage(RTDS_CMD_GET_INTERFACE, RTDS_INTERFACE_RTDATASVR, 0));
		assert(rtDataSvr);

		try
		{
			if( type.taskNotify_ == i8desk::TASK_COMPLATE )
			{
				// 更改数据库信息
				if( !unZipGame->IsForceZip(type.gid_) )
				{
					// 修改数据库信息，及通知相关模块
					ChangeData(rtDataSvr, sysOpt, type.gid_, type.version_, type.isPush_, type.svrPath_, type.taskType_);

					// 提取图标
					GetIcon(rtDataSvr->GetGameTable(), rtPlug, type.gid_);
				}
				else
				{
					db::tGame g;

					if( rtDataSvr->GetGameTable()->GetData(type.gid_, &g) != 0 ) 
						return;

					// 更改为本地状态
					g.Status	= 1;
					rtDataSvr->GetGameTable()->Ensure(&g, MASK_TGAME_STATUS);
				}

				// 通知同步插件
				if( syncPlug != 0 )
					syncPlug->SendMessage(CMD_UPT_SM_NOTIFYSYNC, type.gid_, type.forceRepair_);
			}

			// 通知控制台
			GamePlugNotify notify= {GamePlug, type.taskNotify_, type.gid_, type.err_};
			rtPlug->SendMessage(RTDS_CMD_NOTIFYCONSOEL, (DWORD)&notify, 0);

			
			// 恢复客户机游戏文件不被删除
			IClientRecordset *record = 0;
			rtDataSvr->GetClientTable()->Select(&record, 0, 0);
			for(size_t i = 0; i != record->GetCount(); ++i)
			{
				const db::tClient *client = record->GetData(i);
				if(delClientGame->Find(type.gid_, client->Name))
					delClientGame->Delete(type.gid_, client->Name);
			}
			record->Release();

			// 添加或修改游戏推送统计时间
			db::tGame d;
			if( rtDataSvr->GetGameTable()->GetData(type.gid_, &d) != 0 ) 
				return;
			db::tPushGameStatic pushgame;
			if(rtDataSvr->GetPushGameStaticTable()->GetData(type.gid_, &pushgame) != 0)
			{
				if(d.Force != 0)
				{
					pushgame.GID = type.gid_;
					pushgame.Size = d.Size;
					pushgame.BeginDate = _time32(NULL);
					rtDataSvr->GetPushGameStaticTable()->Insert(&pushgame);			
				}
			}
			else
			{
				if( d.Force == 0)
				{
					pushgame.EndDate = _time32(NULL);
					rtDataSvr->GetPushGameStaticTable()->Update(pushgame.GID,  &pushgame, MASK_TPUSHGAMESTATIC_ENDDATE);
				}
			}
		}
		catch(std::exception &e)
		{
			Log(LM_ERROR, CA2T(e.what()));
		}

	}
}
#include "stdafx.h"
#include "PlugToolHelper.h"

#include "LogHelper.h"
#include "../../../include/Utility/utility.h"

namespace i8desk
{

	const stdex::tString PlugToolPath = utility::GetAppPath() + _T("plugin_tools/");

	PlugToolHelper::PlugToolHelper(IRTDataSvr *rtData)
		: rtData_(rtData)
		, pool_(std::tr1::bind(&PlugToolHelper::_Thread, this))
	{}

	void PlugToolHelper::Start()
	{
		exit_.Create(0, TRUE);
		pool_.Call();
	}

	void PlugToolHelper::Stop()
	{
		exit_.SetEvent();
		Log(LM_DEBUG, _T("退出PlugToolHelper成功"));
	}

	bool PlugToolHelper::Enable(long gid)
	{
		db::tPlugTool plug;
		int ret = rtData_->GetPlugToolTable()->GetData(gid, &plug);
		if( ret < 0 )
		{
			Log(LM_ERROR, _T("获取插件 %d 数据出错"), plug.PID);
			return false;
		}

		bool suc = false;
		if( !utility::IsEmpty(plug.SvrExe) )
		{
			const stdex::tString path = PlugToolPath + 
				stdex::ToString<stdex::tString>(plug.PID) + _T("/Server/") + 
				plug.SvrExe;

			TCHAR svrParam[MAX_PARAM_LEN] = {0};
			utility::Strcpy(svrParam, plug.SvrParam);

			if( !utility::StartProcess(path.c_str(), svrParam) )
			{
				plug.Status = 0;
				Log(LM_ERROR, _T("启动插件: %s 失败"), plug.Name);
				suc = false;
			}
			else
			{
				plug.Status = 1;

				Log(LM_DEBUG, _T("启动插件: %s 成功"), plug.Name);
				suc = true;
			}
		}
		else
		{
			plug.Status = 1;

			Log(LM_DEBUG, _T("启动插件: %s 设置客户端启动"), plug.Name);
			suc = true;
		}

		rtData_->GetPlugToolTable()->Update(gid, &plug, MASK_TPLUGTOOL_STATUS);

		return suc;
		
	}

	bool PlugToolHelper::Disable(long gid)
	{
		db::tPlugTool plug;
		int ret = rtData_->GetPlugToolTable()->GetData(gid, &plug);
		if( ret < 0 )
		{
			Log(LM_ERROR, _T("获取插件 %d 数据出错"), plug.PID);
			return false;
		}

		bool suc = false;
		if( !utility::IsEmpty(plug.SvrExe) )
		{
			if( utility::KillService(plug.SvrExe) )
			{
				plug.Status = 0;
				
				Log(LM_DEBUG, _T("关闭插件 %s 程序成功"), plug.SvrExe);

				suc = true;
			}
			else
			{
				Log(LM_ERROR, _T("关闭插件 %s 程序出错"), plug.SvrExe);
				suc = false;
			}
		}
		else
		{
			plug.Status = 0;

			Log(LM_DEBUG, _T("关闭插件 %s 程序成功"), plug.SvrExe);

			suc = true;
		}

		rtData_->GetPlugToolTable()->Update(gid, &plug, MASK_TPLUGTOOL_STATUS);
		return suc;
	}

	bool PlugToolHelper::Config(long gid)
	{
		db::tPlugTool plug;
		int ret = rtData_->GetPlugToolTable()->GetData(gid, &plug);
		if( ret < 0 )
		{
			Log(LM_ERROR, _T("获取插件 %d 数据出错"), plug.PID);
			return false;
		}

		const stdex::tString path = PlugToolPath + stdex::ToString<stdex::tString>(plug.PID) + _T("/") + plug.ConfigExe + _T(" ") + plug.ConfigParam;

		TCHAR svrParam[MAX_PARAM_LEN] = {0};
		utility::Strcpy(svrParam, path.c_str());

		if( utility::StartProcess(NULL, svrParam) )
		{
			Log(LM_DEBUG, _T("关闭插件 %s 配置程序成功"), plug.SvrExe);

			return true;
		}
		else
		{
			Log(LM_ERROR, _T("打开插件 %s 配置程序出错"), plug.ConfigExe);
			return false;
		}
	}

	void PlugToolHelper::DownloadComplate(long gid)
	{
		// 更新数据库
		db::tPlugTool plug;
		int ret = rtData_->GetPlugToolTable()->GetData(gid, &plug);
		if( ret < 0 )
		{
			Log(LM_ERROR, _T("获取插件 %d 数据出错"), plug.PID);
			return;
		}

		plug.DownloadStatus = 1;
		rtData_->GetPlugToolTable()->Update(gid, &plug, MASK_TPLUGTOOL_DOWNLOADSTATUS);
	}

	void PlugToolHelper::_Thread()
	{
		IPlugToolRecordset *record = 0;
		rtData_->GetPlugToolTable()->Select(&record, 0, 0);
		const uint32 count = record->GetCount();

		for(size_t i = 0; i != count; ++i)
		{
			if( exit_.WaitForEvent(0) )
				break;
	
			// 如果设置了启用, 则启动该进程
			db::tPlugTool &plug = *((*record).GetData(i));
			if( plug.Status )
				Enable(plug.PID);
		}

		record->Release();
	}
}
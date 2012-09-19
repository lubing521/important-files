#include "stdafx.h"
#include "CenterDel.hpp"
#include "LogHelper.h"

#include "PLInfo.h"
#include "../../../include/Http/HttpHelper.h"
#include "../../../include/Xml/Xml.h"
#include "../../../include/I8Type.hpp"
#include "../../../include/LogReport.h"

namespace i8desk
{
	namespace detail
	{
		template < typename EventT >
		bool IsStop(EventT &event)
		{
			return event.WaitForEvent(0) == TRUE;
		}
	}

	CenterDel::CenterDel(ISysOpt *opt, ISvrPlug *logRpt)
		: sysOpt_(opt)
		, logRpt_(logRpt)
	{

	}


	void CenterDel::Start()
	{
		if( !exitEvent_.Create() )
			throw std::runtime_error("创建删除中心指定文件事件失败");

		thread_.RegisterFunc(std::tr1::bind(&CenterDel::_Thread, this));
		if( !thread_.Start() )
			throw std::runtime_error("创建删除中心指定线程失败");
	}

	void CenterDel::Stop()
	{
		exitEvent_.SetEvent();
		thread_.Stop();
	}

	void CenterDel::_OperateXML()
	{
		stdex::tString url;
		if( logRpt_ != 0 )
			url = (LPCTSTR)logRpt_->SendMessage(LOG_RPT_GET_JOB_URL, 0, 0);

		using namespace xml;
		// 下载
		Log(LM_DEBUG, _T("开始下载中心指定删除游戏XML"));

		http::Buffer buf = http::DownloadFile2(url, std::tr1::bind(&detail::IsStop<async::thread::AutoEvent>, std::tr1::ref(exitEvent_)));

		// 解析
		Log(LM_DEBUG, _T("开始解析中心指定删除游戏XML"));

		stdex::tString content(CA2T(buf.first.get()));
		Xml xml;
		if( !xml.LoadXml(content.c_str()) )
			throw std::runtime_error("加载XML失败");

		XmlNode root = xml.GetRoot();
		XmlNodePtr job = root.GetChild(_T("Job"));
		/*uint64 version_ = 0;
		job->GetAttribute(_T("version"), version_);*/
		XmlNodePtr deleteGame = job->GetChild(_T("DeleteGame"));
		XmlNodes nodes = deleteGame->GetChildren();

		for(size_t i = 0; i != nodes.GetCount(); ++i)
		{
			// Item
			XmlNodePtr item = nodes[i]->GetChild(_T("Item"));
			XmlNodePtr GID		= item->GetChild(_T("GID"));
			XmlNodePtr svrParam = item->GetChild(_T("ServerParameter"));
			XmlNodePtr cliParam = item->GetChild(_T("ClientParameter"));

			long gid = 0;
			gid = GID->GetValue(gid);

			int svrVal = 0, cliVal = 0;
			DWORD delFlag = 0;
			svrVal = svrParam->GetValue(svrVal);
			if( svrVal == DELETE_SVR_RECORD ) delFlag = DelRecord;
			if( svrVal == DELETE_SVR_ALL ) delFlag = DelRecord + DelLocal;

			cliVal = cliParam->GetValue(cliVal);
			if( cliVal == DELETE_CLI_ALL ) delFlag += DelClient;

			DeleteInfo::GetInstance().PushGame(gid, delFlag);
		}
		
	}

	DWORD CenterDel::_Thread()
	{
		while( !thread_.IsAborted() )
		{
			BOOL ret = exitEvent_.WaitForEvent(TIME_OUT);
			if( ret  )
				break;
			else
			{
				try
				{
					_OperateXML();
				}
				catch(std::exception &e)
				{
					Log(LM_ERROR, CA2T(e.what()));
				}
			}
			
		}


		return 0;
	}

}
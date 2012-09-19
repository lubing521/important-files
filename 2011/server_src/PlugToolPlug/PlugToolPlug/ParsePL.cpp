#include "stdafx.h"
#include "ParsePL.h"

#include "../../../include/Xml/Xml.h"
#include "LogHelper.h"
#include "IconHelper.h"


namespace i8desk
{

	ParsePL::ParsePL(IRTDataSvr *rtData, IconHelper *iconHelper)
		: rtData_(rtData)
		, version_(0)
		, iconHelper_(iconHelper)
		, isExit_(0)
	{
	}

	void ParsePL::Parse(LPCTSTR data)
	{
		Log(LM_INFO, _T("开始解析插件列表..."));

		_ParseImpl(data);

		Log(LM_INFO, _T("解析插件列表完成"));
	}

	void ParsePL::Stop()
	{
		::InterlockedExchange(&isExit_, 1);
	}

	void ParsePL::_ParseImpl(LPCTSTR data)
	{
		using namespace xml;
		try
		{
			Xml plXML;
			if( !plXML.LoadXml(data) )
				throw std::runtime_error("加载XML出错!");

			XmlNode root = plXML.GetRoot();
			ulonglong tmpVer = 0;
			if( !root.IsNull() )
				tmpVer = root.GetAttribute(_T("ver"), tmpVer);
			else
				throw std::runtime_error("解析XML出错: 缺少Root信息");

			// 如果版本号相同，则不解析
			if( version_ == tmpVer )
				return;
			else
				version_ = tmpVer;

			
			// 获取Products信息
			XmlNodePtr productNode = root.GetChild(_T("products"));
			XmlNodes nodes = productNode->GetChildren();
			for(size_t i = 0; i != nodes.GetCount(); ++i)
			{
				if( ::InterlockedExchangeAdd(&isExit_, 0) == 1 )
				{
					return;
				}

				// 入库
				db::tPlugTool plugTool;
				
				// Product
				plugTool.PID	= nodes[i]->GetAttribute(_T("gid"), plugTool.PID);
				utility::Strcpy(plugTool.CID, nodes[i]->GetAttribute(_T("type"), plugTool.CID));
				plugTool.IdcVer = nodes[i]->GetAttribute(_T("idcver"), plugTool.IdcVer);

				// Base
				XmlNodePtr baseNode			= nodes[i]->GetChild(_T("base"));
				utility::Strcpy(plugTool.Name,		baseNode->GetChild(_T("name"))->GetValue(plugTool.Name));
				utility::Strcpy(plugTool.Comment,	baseNode->GetChild(_T("description"))->GetValue(plugTool.Comment));
				utility::Strcpy(plugTool.ToolSource,baseNode->GetChild(_T("provider"))->GetValue(plugTool.ToolSource));
				plugTool.Size				= baseNode->GetChild(_T("size"))->GetValue(plugTool.Size);
				plugTool.IdcClick			= baseNode->GetChild(_T("rating"))->GetValue(plugTool.IdcClick);
				plugTool.DownloadType		= baseNode->GetChild(_T("download"))->GetValue(plugTool.DownloadType);
				plugTool.DownloadStatus		= baseNode->GetChild(_T("status"))->GetValue(plugTool.DownloadStatus);
				
				// 图标
				assert(iconHelper_ != 0);
				std::string iconData;
				baseNode->GetChild(_T("icon"))->GetValue(iconData);
				if( !iconData.empty() )
				{
					int len = ATL::Base64DecodeGetRequiredLength(iconData.length());
					std::vector<unsigned char> tmp(len);
					ATL::Base64Decode(iconData.data(), iconData.length(), &tmp[0], &len);
					
					iconHelper_->EnsureIcon(plugTool.PID, len, &tmp[0]);
				}

				// Server
				XmlNodePtr serverNode = nodes[i]->GetChild(_T("server"));
				utility::Strcpy(plugTool.SvrName, serverNode->GetChild(_T("name"))->GetValue(plugTool.SvrName));
				utility::Strcpy(plugTool.SvrExe, serverNode->GetChild(_T("exe"))->GetValue(plugTool.SvrExe));
				utility::Strcpy(plugTool.SvrParam, serverNode->GetChild(_T("param"))->GetValue(plugTool.SvrParam));
				plugTool.SvrRunType = serverNode->GetChild(_T("runtype"))->GetValue(plugTool.SvrRunType);
				utility::Strcpy(plugTool.SvrPath, serverNode->GetChild(_T("path"))->GetValue(plugTool.SvrPath));
				

				// Config
				XmlNodePtr configNode = nodes[i]->GetChild(_T("config"));
				utility::Strcpy(plugTool.ConfigExe, configNode->GetChild(_T("exe"))->GetValue(plugTool.ConfigExe));
				utility::Strcpy(plugTool.ConfigParam, configNode->GetChild(_T("param"))->GetValue(plugTool.ConfigParam));
				plugTool.ConfigRunType = configNode->GetChild(_T("runtype"))->GetValue(plugTool.ConfigRunType);
				utility::Strcpy(plugTool.ConfigPath, configNode->GetChild(_T("path"))->GetValue(plugTool.ConfigPath));

				// Client
				XmlNodePtr cliNode = nodes[i]->GetChild(_T("client"));
				utility::Strcpy(plugTool.CliName, cliNode->GetChild(_T("name"))->GetValue(plugTool.CliName));
				utility::Strcpy(plugTool.CliExe, cliNode->GetChild(_T("exe"))->GetValue(plugTool.CliExe));
				utility::Strcpy(plugTool.CliParam, cliNode->GetChild(_T("param"))->GetValue(plugTool.CliParam));
				plugTool.CliRunType = cliNode->GetChild(_T("runtype"))->GetValue(plugTool.CliRunType);
				stdex::tOstringstream os;
				os << utility::GetAppPath() << _T("plugin_tools\\") << plugTool.PID  << _T("\\") << cliNode->GetChild(_T("path"))->GetValue(plugTool.CliPath);
				utility::Strcpy(plugTool.CliPath, os.str());

				rtData_->GetPlugToolTable()->Ensure(&plugTool, 
					0xFFFFFFF & ~MASK_TPLUGTOOL_DOWNLOADSTATUS & ~MASK_TPLUGTOOL_STATUS);
			}
		}
		catch(std::exception &e)
		{
			stdex::tString err(CA2T(e.what()));
			Log(LM_ERROR, err);
		}
	}
}
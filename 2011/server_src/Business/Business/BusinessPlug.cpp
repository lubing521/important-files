#include "stdafx.h"
#include "BusinessPlug.hpp"

#include "LogHelper.h"

#include "../../../include/Win32/System/except.h"
#include "../../../include/BusinessInfo.h"

#include "DownloadEngine/DownloadEngine.hpp"
#include "LocalGameChecker.hpp"


namespace i8desk
{
	namespace plugMgr
	{
		stdex::tString PLUG_NAME		= PLUG_BUSINESS_NAME;
		stdex::tString PLUG_COMPANY		= _T("GoYoo");
		stdex::tString PLUG_DESCRIPTION	= _T("Business Plug");
		u_long PLUG_VERSION				= 0x01000000;
	}


	// Global var
	except::MiniDump miniDump;

	BusinessPlug::BusinessPlug()
		: plugMgr_(0)
		, log_(0)
		, rtDataSvr_(0)
	{}

	BusinessPlug::~BusinessPlug()
	{

	}


	//释放插件
	void BusinessPlug::Release()
	{
		delete this;
	}

	//得到插件的名字
	LPCTSTR BusinessPlug::GetPlugName()
	{
		return plugMgr::PLUG_NAME.c_str();
	}
	//得到插件的版本0xaabbccdd.
	DWORD BusinessPlug::GetVersion() 
	{
		return plugMgr::PLUG_VERSION;
	}
	//得到插件开发的公司名
	LPCTSTR BusinessPlug::GetCompany()
	{
		return plugMgr::PLUG_COMPANY.c_str();
	}
	//得到插件的一些描述
	LPCTSTR BusinessPlug::GetDescription() 
	{
		return plugMgr::PLUG_DESCRIPTION.c_str();
	}

	//如果插件需要框架监听的网络端口，如果不需要监听，则返回0.
	//当框架收到该端口事件时，会回调给该接口的INetLayerEvent对应的虚函数
	DWORD BusinessPlug::GetListPort()
	{
		return 0;
	}

	bool BusinessPlug::Initialize(ISvrPlugMgr* pPlugMgr)
	{
		assert(pPlugMgr);
		if( pPlugMgr == 0 )
			return false;

		plugMgr_ = pPlugMgr;
		// 初始化日志组件
		log::LogHelper::GetInstance(plugMgr_);

		Log(LM_INFO, _T("Business 正在启动..."));

		ISvrPlug *pRTDataSvr = plugMgr_->FindPlug(RTDATASVR_PLUG_NAME);
		assert(pRTDataSvr);
		if( pRTDataSvr == 0 )
		{
			Log(LM_ERROR, _T("没有实时数据服务插件"));
			return false;
		}

        ISvrPlug *pGameMgrPlug = plugMgr_->FindPlug(PLUG_GAMEMGR_NAME);
        assert(pGameMgrPlug);
        if( pGameMgrPlug == 0 )
        {
            Log(LM_ERROR, _T("没有三层上报插件"));
            return false;
        }

		rtDataSvr_ = reinterpret_cast<IRTDataSvr *>(
			pRTDataSvr->SendMessage(RTDS_CMD_GET_INTERFACE, RTDS_INTERFACE_RTDATASVR, 0));
		assert(rtDataSvr_);


		try
		{
			using namespace std::tr1::placeholders;

            clExInfoMgr_.reset(new CI8ExInfoMgrS(rtDataSvr_, pGameMgrPlug));
			engine_.reset(new engine::DownloadEngine(clExInfoMgr_.get()));
			checker_.reset(new TaskProductor(rtDataSvr_, clExInfoMgr_.get(),
				std::tr1::bind(&BusinessPlug::_AddTask, this, GameXML, _1, _2),
				std::tr1::bind(&BusinessPlug::_AddTask, this, ClientXML, 0, 3))); // 最高优先级

			engine_->Start();
			checker_->Start();
		}
		catch(std::exception &e)
		{
			stdex::tString err(CA2T(e.what()));
			Log(LM_ERROR, err);
			return false;
		}

		Log(LM_INFO, _T("Business 启动完成"));
		return true;
	}

	bool BusinessPlug::UnInitialize()
	{
		Log(LM_INFO, _T("Exit Business..."));

		checker_->Stop();
		engine_->Stop();

		checker_.reset();
        engine_.reset();
        clExInfoMgr_.reset();
		

		Log(LM_INFO, _T("Exit Business Successed!"));
		return true;
	}

	void BusinessPlug::_AddTask(long type, long gid, long priority)
	{
		engine_->Add(type, gid, priority, plugMgr_);
	}

	DWORD BusinessPlug::SendMessage(DWORD cmd, DWORD param1, DWORD param2)
	{
		switch(cmd)
		{
		case E_BSC_GET_EX_INFO:// param1 类型,param2 GID
            switch (param1)
            {
            case GameXML:
                {
                    void* pInfo = clExInfoMgr_->GetGame(param2);
                    if (pInfo == NULL)
                    {
                        _AddTask(param1, param2, 3);	// 最高优先级
                    }
                    else
                    {
                        return reinterpret_cast<DWORD>(pInfo);
                    }
                }
                break;
            case ClientXML:
                {
                    void* pInfo = clExInfoMgr_->GetClientMenuInfo();
                    if (pInfo == NULL)
                    {
                        _AddTask(param1, param2, 3);	// 最高优先级
                    }
                    else
                    {
                        return reinterpret_cast<DWORD>(pInfo);
                    }
                }
                break;
            }
			break;
        case E_BSC_UPDATE_EX_INFO:
            switch (param1)
            {
            case GameXML:
                _AddTask(param1, param2, 3);	// 最高优先级
                break;
            case ClientXML:
                _AddTask(param1, param2, 3);	// 最高优先级
                break;
            }
            break;
        case E_BSC_GET_GAMEEX_VERSION:
            return reinterpret_cast<DWORD>(clExInfoMgr_->GetGameVersion());
        case E_BSC_GET_GAMEEX_GLOBAL_VERSION:
            return DWORD(clExInfoMgr_->GetGameUpdateTime());
		default:
			assert(0);
		}
		return 0;
	}

	void BusinessPlug::OnAccept(i8desk::INetLayer*pNetLayer, SOCKET sck, DWORD param)
	{
	}

	void BusinessPlug::OnSend(i8desk::INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param)
	{
	}

	void BusinessPlug::OnRecv(i8desk::INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param)
	{
	}

	void BusinessPlug::OnClose(i8desk::INetLayer*pNetLayer, SOCKET sck, DWORD param)
	{
	}
}
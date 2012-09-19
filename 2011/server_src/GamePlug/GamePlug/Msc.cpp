#include "stdafx.h"
#include "Msc.h"
#include "LogHelper.h"

#pragma comment(lib, "massctl.lib")


namespace i8desk
{
	namespace msc
	{

		MscInit::MscInit(IRTDataSvr *rtDatasvr)
		{
			assert(rtDatasvr);

			db::tServerStatus d;
			IServerRecordset *pServerRst = 0;

			rtDatasvr->GetServerTable()->Select(&pServerRst,0,0);
			for( uint32 i = 0; i < pServerRst->GetCount(); ++i )
			{
				db::tServer *server = pServerRst->GetData(i);
				if( server->SvrType == MainServer )
				{
					utility::Strcpy( d.SvrID, server->SvrID );
					break;
				}
			}
			pServerRst->Release();

			// 初始化三层下载组件
			int iRet = ::msc_init(L"Downloader", L"Downloader");
			
			// 写三层模块状态到服务模块状态表中
			d.DNAService = iRet != 0 ? -1 : 1;
			rtDatasvr->GetServerStatusTable()->Ensure(&d, MASK_TSERVERSTATUS_DNASERVICE);

			if( iRet != 0 ) 
			{
				std::ostringstream os;
				os << "msc_init失败, ownergid = " << ::msc_getownerid() << " err: " << iRet;

				Log(LM_ERROR, CA2T(os.str().c_str()));
			}
		}

		MscInit::~MscInit()
		{
			//Log(LM_DEBUG, _T("三层组件退出"));
		}


		void MscSpeed::Update()
		{
			unsigned int downRate = 0, upRate = 0;
			::msc_GetTransferRate(&downRate, &upRate);

			::InterlockedExchange(&upRate_, upRate);
			::InterlockedExchange(&downRate_, downRate);
		}
	}
}
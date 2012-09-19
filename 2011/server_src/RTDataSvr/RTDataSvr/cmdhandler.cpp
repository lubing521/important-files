#include "StdAfx.h"

#include "rtdatasvr.h"
#include "datacache.h"
#include "filemonitor.h"
#include "dbwriter.h"
#include "Icontable.h"
#include "SeverBallance.hpp"

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include <algorithm>
#include <array>
#include "..\..\..\include\tablestruct.h"
#include "..\..\..\include\Utility\utility.h"
#include "..\..\..\include\Win32\FileSystem\FileHelper.hpp"
#include "..\..\..\include\Win32\System\cpu.h"
#include "..\..\..\include\PluginInfo.h"
#include "..\..\..\include\Extend STL\StringAlgorithm.h"
#include "..\..\..\include\I8Type.hpp"
#include "..\..\..\include\win32\FileSystem\FileOperator.hpp"
#include "..\..\..\include\MultiThread\Lock.hpp"
#include "..\..\..\include\UpdateGame.h"
#include "..\..\..\include\AsyncIO\Network\IPAddress.hpp"
#include "..\..\..\include\BusinessInfo.h"
#include "..\..\..\include\LogReport.h"
#include "..\..\..\include\BusinessInfo.h"
#include "..\..\..\include\Win32\System\SystemHelper.hpp"
#include "..\..\..\include\Serialize\Serialize.hpp"


#include <set>
#pragma comment(lib, "ws2_32.lib")


namespace i8desk
{
	static bool IsValidChar(const TCHAR *buf) 
	{
		size_t userlen = _tcslen(buf);
		TCHAR ErrorChar[] = _T("\\/:*?\"<>|'");

		for (size_t i = 0; i < sizeof(ErrorChar); i++) {
			for (size_t j = 0; j < userlen; j++) {
				if (buf[j] == ErrorChar[i]) {
					return false;
				}
			}
		}

		return true;
	}


	void CRTDataSvr::SendNextFileBlock(NetClient_Ptr pClient, INetLayer*pNetLayer, SOCKET sck)
	{
//		//TEST_THIS_SECTION(__FUNCTION__);

		if (pClient->HasRemainBlock()) {
			LPSTR lpSendPackage = pNetLayer->GetIMemPool()->Alloc(BLOCK_SIZE);
			BOOL ok = pClient->ReadFileData((BYTE*)lpSendPackage, BLOCK_SIZE);
			if (ok) {
				pClient->IncreaseIndex();
				//如果此包发送完成，则立即清除记录，以能接收下一个文件传送请求
				if (!pClient->HasRemainBlock() && pClient->GetRemainSize() == 0) {
					pClient->CloseFile();
					pClient->ClearBlockCount();
					pClient->ClearCurrentCmd();
				}
				pNetLayer->SendPackage(sck, (LPSTR)lpSendPackage,BLOCK_SIZE);
			}
			else {
				pClient->CloseFile();
				pClient->ClearCurrentCmd();
				pNetLayer->GetIMemPool()->Free(lpSendPackage);
			}
		}
		else if (DWORD dwRemainSize = pClient->GetRemainSize()) {
			LPSTR lpSendPackage = pNetLayer->GetIMemPool()->Alloc(BLOCK_SIZE);
			BOOL ok = pClient->ReadFileData((BYTE*)lpSendPackage, dwRemainSize);

			//此包发送完成，立即清除记录，以能接收下一个文件传送请求
			pClient->ClearBlockCount();
			pClient->CloseFile();
			pClient->ClearCurrentCmd();

			if (ok) {
				pNetLayer->SendPackage(sck, lpSendPackage, dwRemainSize);
			}
			else {
				pNetLayer->GetIMemPool()->Free(lpSendPackage);
			}
		}
		else {
			pClient->CloseFile();
			pClient->ClearBlockCount();
			pClient->ClearCurrentCmd();
		}
	}

	void CRTDataSvr::OnSend(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param)
	{
		NetClient_Ptr pClient;
		if (m_ClientList.peek(sck, pClient)) {
			switch (pClient->GetCurrentCmd()) {
			case CMD_USER_DOWNLOAD: 
			case CMD_CLIUPT_DOWNFILE: {
				//如果不够一块则pClient->m_dwBlockCount==0,所以只会发送一次
				if (pClient->GetBlockCount() > 0) {
					SendNextFileBlock(pClient, pNetLayer, sck);
				}
				else {
					pClient->CloseFile();
					pClient->ClearCurrentCmd();
				}
				break; }
			case CMD_GAME_GETICONLIST: {
				//if (uint32 NextGid = pClient->GetNextIconGid()) {
				//	SendNextIconPackage(pClient, NextGid, pNetLayer, sck);
				//}
				//else {
				//	pClient->ClearCurrentCmd();
				//}
				break; }
			default:
				break;
			} 
		}
	}

	void CRTDataSvr::Cmd_Unknown(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
	{
		LPSTR lpSendPackage = pNetLayer->GetIMemPool()->Alloc(1024);
		CPkgHelper outpackage(lpSendPackage, 1024);
		outpackage << 0L <<_T( "未知命令");
		pNetLayer->SendPackage(sck, (LPSTR)outpackage.GetBuffer(), outpackage.GetLength());
	}

	void CRTDataSvr::Cmd_Game_Req_Icons_Idx(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
	{

		size_t nCount = 0;
		CustomFileInfoPtr pIcons ;
		size_t filesize = 0;
		filesize =  m_pIconTable->GetAllFileData(nCount,pIcons);

		size_t bufsize =  nCount * sizeof(DWORD) * 2 + 128;
		LPSTR pbuf = pNetLayer->GetIMemPool()->Alloc(bufsize);
		CPkgHelper out(pbuf, bufsize, header.Cmd, OBJECT_CONSOLE);

		if ( filesize != 0 )
		{
			out << 1L << nCount;
			CustomFileInfo* custominfo = pIcons.get();

			for( size_t i = 0; i!= nCount; ++i )
			{
				out << custominfo->gid << custominfo->crc ;
				custominfo += 1;
			}
		}
		else
		{
			out << 1L << 0;
		}

		pNetLayer->SendPackage(sck, (LPSTR)out.GetBuffer(), out.GetLength());
	}

	void CRTDataSvr::Cmd_Game_Req_Icons_data(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
	{

		size_t nCount = 0;
		CustomFileInfoPtr pIcons ;
		size_t filesize = 0;
		filesize =  m_pIconTable->GetAllFileData(nCount,pIcons);

		size_t bufsize =  MAX_COUNT_ICON_PER_PACKEG * 1024 * 10 + 128;
		LPSTR pbuf = pNetLayer->GetIMemPool()->Alloc(bufsize);
		CPkgHelper pkg(pbuf, bufsize, header.Cmd, OBJECT_CONSOLE);
		
		pkg << 1L << 1L << 0L;
		DWORD *pdwCount = (DWORD*)(pbuf + sizeof(pkgheader) + 8);

		try {
			TCHAR szGid[64] = {0};
			long gid = 0;
			size_t size = 0;
			void *data;

			DWORD nIcons = 0;
			inpackage.Pop(nIcons);
			nIcons = std::min(nIcons, MAX_COUNT_ICON_PER_PACKEG);
			for (DWORD n = 0; n < nIcons; n++) {
				inpackage.Pop(gid);

				pkg.Push(gid);
				
				if( !m_pIconTable->GetFileData(gid,size,data) )
					assert(0);
				pkg.Push(size);
				pkg.PushPointer((BYTE*)data, size);
				(*pdwCount)++;
			}
		}
		catch (...) {
			Log(LM_ERROR, _T("发送图标时打包图标数据过程异常!\n"));
		}
		pNetLayer->SendPackage(sck, (LPSTR)pkg.GetBuffer(), pkg.GetLength());
	}


	void CRTDataSvr::Cmd_Game_GetIdxs(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
	{
//		//TEST_THIS_SECTION(__FUNCTION__);

		db::tClient client;
		try 
		{
			inpackage >> client.Name;
		} catch (...)  
		{
			NAK_RETURN(_T("错误的命令格式"));
		} 

		if (m_pTableMgr->GetClientTable()->GetData(client.Name, &client) != 0)
			utility::Strcpy(client.AID, DEFAULT_AREA_GUID);

		const size_t bufsiz = 1*1024*1024;
		LPSTR pbuf = pNetLayer->GetIMemPool()->Alloc(bufsiz);
		CPkgHelper out(pbuf, bufsiz, header.Cmd,OBJECT_UNKOWN);
		out << 1L << 0L;
		DWORD *pCount = (DWORD *)(pbuf + sizeof(pkgheader) + 4);


		struct FGetGameIdxs 
			: std::unary_function<GameCache_Ptr, void> 
		{
			FGetGameIdxs(CPkgHelper& out, DWORD *pCount, const TCHAR *AID)
				: m_out(out)
				, m_pCount(pCount)
				, m_AID(AID) 
			{}

			void operator()(const argument_type& _Arg) 
			{
				assert(_Arg);

				if (_Arg->IsValid()) 
				{
					if( _Arg->GetRunTypeType(m_AID) != GAME_RT_NONE )
					{
						m_out << _Arg->GetGid() 
							<< _Arg->GetVersion() 
							<< _Arg->GetRunTypeVersion(m_AID); 
						(*m_pCount)++;
					}
				}
			}
		private:
			CPkgHelper& m_out;
			DWORD *m_pCount;
			const TCHAR *m_AID;
		};

		m_pDataCache->GetGames().foreach_value(FGetGameIdxs(out, pCount, client.AID));

		header.Length = out.GetLength();
		out.pushHeader(header);
		pNetLayer->SendPackage(sck, (LPSTR)pbuf, out.GetLength());
	}

	void CRTDataSvr::Cmd_Game_GetSome(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage, pkgheader &header)
	{
//		//TEST_THIS_SECTION(__FUNCTION__);

		db::tClient client;
		DWORD dwCount;
		uint32 gid;
		try 
		{
			inpackage >> client.Name >> dwCount;
		} 
		catch (...) 
		{
			NAK_RETURN(_T("错误的命令格式"));
		} 

		if(m_pTableMgr->GetClientTable()->GetData(client.Name, &client) != 0)
			utility::Strcpy(client.AID, DEFAULT_AREA_GUID);

		if (dwCount > MAX_COUNT_GAME_PER_PACKEG) 
			dwCount = MAX_COUNT_GAME_PER_PACKEG;

		const size_t bufsiz = sizeof(pkgheader) + 4 + sizeof(db::tGame) * dwCount + 1024;
		LPSTR pbuf = pNetLayer->GetIMemPool()->Alloc(bufsiz);
		CPkgHelper out(pbuf, bufsiz, header.Cmd,OBJECT_UNKOWN);

		out << 1L << 0L;
		DWORD *pCount = (DWORD *)(pbuf + sizeof(pkgheader) + 4);

		db::tGame game;
		db::tClass clss;
		ZGameCache::RunType rt;


		switch(header.Version)
		{
		case 0x0200:
			try 
			{
				for(DWORD i = 0; i < dwCount; i++) 
				{
					inpackage >> gid;
					GameCache_Ptr g;
					if (m_pDataCache->GetGames().peek(gid, g)
						&& g->GetGame(game, clss) 
						&& g->GetRunType(client.AID, rt)) 
					{
						if( rt.type == GAME_RT_NONE )
							continue;

						out << game.GID 
							<< game.PID 
							<< game.Name 
							<< clss.Name 
							<< game.Exe 
							<< game.Param 
							<< game.Size
							<< game.DeskLink 
							<< game.Toolbar 
							<< game.SvrPath
							<< game.CliPath
							<< 1L
							<< 1L
							<< game.TraitFile
							<< game.SaveFilter
							<< game.IdcAddDate
							<< game.SvrVer
							<< game.IdcVer
							<< game.SvrVer 
							<< game.AutoUpt 
							<< game.I8Play 
							<< game.IdcClick 
							<< game.SvrClick 
							<< game.IdcAddDate
							<< _T("")
							<< rt.type 
							<< 1L
							<< 1L
							<< rt.vid
							<< game.Status 
							<< game.Memo;
							
						(*pCount)++;
					}
				}
			} 
			catch (...) 
			{
				Log(LM_WARNING, _T("%s:10版错误的命令格式2\n"), __FUNCTION__);
			} 
			break;
		case PRO_VERSION:
			try 
			{
				for(DWORD i = 0; i < dwCount; i++) 
				{
					inpackage >> gid;
					GameCache_Ptr g;
					if (m_pDataCache->GetGames().peek(gid, g)
						&& g->GetGame(game, clss) 
						&& g->GetRunType(client.AID, rt)) 
					{
						if( rt.type == GAME_RT_NONE )
							continue;

						if ( game.Hide == i8desk::Hide)
							continue;

						out << game.GID 
							<< game.PID 
							<< game.Name 
							<< clss.Name 
							<< game.Exe 
							<< game.Param 
							<< game.Size
							<< game.DeskLink 
							<< game.Toolbar 
							<< game.SvrPath
							<< game.CliPath
							<< game.EnDel
							<< game.TraitFile
							<< game.SaveFilter
							<< game.IdcAddDate
							<< game.IdcVer
							<< game.SvrVer 
							<< game.AutoUpt 
							<< game.I8Play 
							<< game.IdcClick 
							<< game.SvrClick 
							<< rt.type 
							<< rt.vid
							<< game.Status 
							<< game.Memo
							<< game.Repair;
						(*pCount)++;
					}
				}
			} 
			catch (...) 
			{
				Log(LM_WARNING, _T("%s:11版错误的命令格式2\n"), __FUNCTION__);
			} 
			break;
			
		}
	
		header.Length = out.GetLength();
		out.pushHeader(header);
		pNetLayer->SendPackage(sck,(LPSTR)pbuf, out.GetLength());
	}
///////////////////////

	void CRTDataSvr::Cmd_PlugTool_GetIdxs(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
	{
		const size_t bufsiz = 1*1024*1024;
		LPSTR pbuf = pNetLayer->GetIMemPool()->Alloc(bufsiz);
		CPkgHelper out(pbuf, bufsiz, header.Cmd,OBJECT_UNKOWN);
		out << 1L << 0L;
		DWORD *pCount = (DWORD *)(pbuf + sizeof(pkgheader) + 4);


		struct FGetPlugToolIdxs 
			: std::unary_function<PlugToolCache_Ptr, void> 
		{
			FGetPlugToolIdxs(CPkgHelper& out, DWORD *pCount)
				: m_out(out)
				, m_pCount(pCount)
			{}

			void operator()(const argument_type& _Arg) 
			{
				assert(_Arg);

				if (_Arg->IsValid()) 
				{
					m_out << _Arg->GetPid() << _Arg->GetVersion(); 
					(*m_pCount)++;
				}
			}
		private:
			CPkgHelper& m_out;
			DWORD *m_pCount;
		};

		m_pDataCache->GetPlugTools().foreach_value(FGetPlugToolIdxs(out, pCount));

		header.Length = out.GetLength();
		out.pushHeader(header);
		pNetLayer->SendPackage(sck, (LPSTR)pbuf, out.GetLength());
	}

	void CRTDataSvr::Cmd_Report_ConnectSvrIP(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
	{
		ulong clientIP = 0, ip = 0;
		DWORD flag = 0;
		DWORD optype = 0;
		try
		{
			inpackage >> flag >> clientIP >> ip >> optype;
			if(optype == 0)
				balance_->UpdateConnect(flag, ip, clientIP);
			else if(optype == 1)
				balance_->UpdateDisconnect(clientIP);
		}
		catch(...)
		{
			NAK_RETURN(_T("错误的命令格式"));
		}

		ACK();
	}

	void CRTDataSvr::Cmd_PlugTool_GetSome(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage, pkgheader &header)
	{
		DWORD dwCount;
		uint32 pid;
		try 
		{
			inpackage >> dwCount;
		} 
		catch (...) 
		{
			NAK_RETURN(_T("错误的命令格式"));
		} 


		if (dwCount > MAX_COUNT_GAME_PER_PACKEG) 
			dwCount = MAX_COUNT_GAME_PER_PACKEG;

		const size_t bufsiz = sizeof(pkgheader) + 4 + sizeof(db::tPlugTool) * dwCount + 1024;
		LPSTR pbuf = pNetLayer->GetIMemPool()->Alloc(bufsiz);
		CPkgHelper out(pbuf, bufsiz, header.Cmd,OBJECT_UNKOWN);

		out << 1L << 0L;
		DWORD *pCount = (DWORD *)(pbuf + sizeof(pkgheader) + 4);
		
		try 
		{
			for(DWORD i = 0; i < dwCount; i++) 
			{
				inpackage >> pid;
				PlugToolCache_Ptr pt;
				if (!m_pDataCache->GetPlugTools().peek(pid, pt))
					return;
				db::tPlugTool plugtool;
				pt->GetPlugTool(plugtool);
				out << plugtool.PID 
					<< plugtool.CID
					<< plugtool.IdcVer 
					<< plugtool.SvrVer 
					<< plugtool.Name
					<< plugtool.Size 
					<< plugtool.IdcClick 
					<< plugtool.DownloadType
					<< plugtool.DownloadStatus
					<< plugtool.Status
					<< plugtool.CliName
					<< plugtool.CliExe
					<< plugtool.CliParam
					<< plugtool.CliPath
					<< plugtool.CliRunType;
					(*pCount)++;
			}
		} 
		catch (...) 
		{
			Log(LM_WARNING, _T("%s:11版错误的命令格式2\n"), __FUNCTION__);
		} 

		header.Length = out.GetLength();
		out.pushHeader(header);
		pNetLayer->SendPackage(sck,(LPSTR)pbuf, out.GetLength());
	}


///////////////////////////
	void CRTDataSvr::Cmd_Game_RepClick(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
	{
		//TEST_THIS_SECTION(__FUNCTION__);

		try {
			db::tGame game;
			DWORD dwGid;
			DWORD dwClickCount;

			DWORD dwCount = 0;
			inpackage.Pop(dwCount);
			for (DWORD i = 0; i < dwCount; i++) {
 				inpackage >> dwGid >> dwClickCount;
				game.SvrClick = game.SvrClick2 = dwClickCount;
				m_pTableMgr->GetGameTable()->Cumulate(dwGid, &game, 
					MASK_TGAME_SVRCLICK | MASK_TGAME_SVRCLICK2);
			}

			DWORD dwCount1;
			db::tModuleUsage mu ;
			inpackage >> mu.MAC >> dwCount1;
			for (DWORD i = 0; i < dwCount1; i++) {
				inpackage >> mu.MID >> mu.ClickCount >> mu.UsedTime;
				if (m_pTableMgr->GetModuleUsageTable()->Cumulate(mu.MAC, mu.MID, &mu,
					MASK_TMODULEUSAGE_CLICKCOUNT | MASK_TMODULEUSAGE_USEDTIME) != 0)
				{
					m_pTableMgr->GetModuleUsageTable()->Insert(&mu);
				}
			}

			// 二级菜单
			DWORD dwCount2 = 0;
			db::tModule2Usage mu2;
			inpackage >> dwCount2;
			utility::Strcpy(mu2.MAC, mu.MAC);
			for(DWORD i = 0; i != dwCount2; ++i)
			{
				inpackage >> mu2.MID >> mu2.Name >> mu2.ClickCount >> mu2.UsedTime;

				if (m_pTableMgr->GetModule2UsageTable()->Cumulate(mu2.MAC, &mu2,
					MASK_TMODULE2USAGE_CLICKCOUNT | MASK_TMODULE2USAGE_USEDTIME) != 0)
				{
					m_pTableMgr->GetModule2UsageTable()->Insert(&mu2);
				}
			}

			LPSTR pbuf = pNetLayer->GetIMemPool()->Alloc(1024);
			CPkgHelper out(pbuf, 1024, header.Cmd,OBJECT_UNKOWN);
			out << 1L;
			header.Length = out.GetLength();
			out.pushHeader(header);
			pNetLayer->SendPackage(sck, pbuf, out.GetLength());
		} catch (...) {
			NAK_RETURN(_T("错误的命令格式"));
		} 
	}

	void CRTDataSvr::Cmd_Game_ConExeSql(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
	{
		//TEST_THIS_SECTION(__FUNCTION__);

		stdex::tString strErr;

		DWORD optype = SQL_OP_NONE;
		try 
		{
			inpackage.Pop(optype);
			switch (optype) 
			{
			case SQL_OP_SELECT:
				ResponseRecordset(pNetLayer, sck,lpPackage, inpackage, header);
				break;
			case SQL_OP_INSERT:
				InsertRecord(pNetLayer, sck, inpackage, header);
				break;
			case SQL_OP_UPDATE:
				UpdateRecord(pNetLayer, sck, inpackage, header);
				break;
			case SQL_OP_DELETE:
				DeleteRecord(pNetLayer, sck, inpackage, header);
				break;
			case SQL_OP_ENSURE:
				EnsureRecord(pNetLayer, sck, inpackage, header);
				break;
			case SQL_OP_COMULATE:
				ComulateRecord(pNetLayer, sck, inpackage, header);
				break;
			default:
				break;
			}
		} catch (std::exception &e) {
			strErr = (LPCTSTR)CA2T(e.what());
			Log(LM_WARNING, _T("执行SQL时异常:%s\n"), strErr);			
		} catch (...) {
			strErr = _T("执行SQL时异常");
			Log(LM_ERROR, _T("执行SQL时未知异常\n"));
			assert(0);
		} 

		if (!strErr.empty()) {
			NAK(strErr.c_str());
		}
	}

	void CRTDataSvr::Cmd_VDisk_GetList(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
	{
//		//TEST_THIS_SECTION(__FUNCTION__);

		const size_t bufsiz = 1*1024*1024;
		LPSTR lpSendPackage = pNetLayer->GetIMemPool()->Alloc(bufsiz);
		CPkgHelper outpackage(lpSendPackage, bufsiz, header.Cmd,OBJECT_UNKOWN);

		struct FGetVDisk : public std::unary_function<VDiskCache_Ptr, void> {
			FGetVDisk(CPkgHelper& pkg, DWORD *pCount) 
				: outpackage(pkg)
				, m_pCount(pCount) 
			{
			}
			void operator()(VDiskCache_Ptr vdisk)
			{
				i8desk::db::tVDisk vd;
					if (vdisk->GetVDisk(vd)) {
						outpackage << vd.VID
							<< vd.SoucIP
							<< vd.Port
							<< vd.SvrDrv
							<< vd.CliDrv
							<< vd.Type
							<< vd.LoadType;
						(*m_pCount)++;
				}
			}

			CPkgHelper& outpackage;
			DWORD *m_pCount;
		};

		outpackage << 1L << 0L;
		DWORD *pCount = (DWORD *)(lpSendPackage + sizeof(pkgheader) + 4);

		m_pDataCache->GetVDisks().foreach_value(FGetVDisk(outpackage, pCount));

		header.Length=outpackage.GetLength();
		outpackage.pushHeader(header);
		pNetLayer->SendPackage(sck, lpSendPackage, outpackage.GetLength());
	}

	void CRTDataSvr::Cmd_VDisk_GetUpdateFileList(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
	{
		// VDisk Update File List
		static const stdex::tString fileList[] =
		{
			MODULE_I8VDISKSVR,
			PLUG_GAMEUPDATE_MODULE,
			PLUG_VDISK_MODULE,
			PLUG_FRAME_MODULE
		};

		const size_t bufSize = 1024;
		LPSTR pbuf = pNetLayer->GetIMemPool()->Alloc(bufSize);
		CPkgHelper out(pbuf, bufSize);

		out << 1L
			<< sizeof(fileList) / sizeof(fileList[0])
			<< fileList[0]
			<< fileList[1]
			<< fileList[2]
			<< fileList[3];

		pNetLayer->SendPackage(sck, (LPSTR)out.GetBuffer(), out.GetLength());

	}

	void CRTDataSvr::Cmd_VDisk_GetUpdateFileInfo(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
	{
		TCHAR fileName[MAX_PATH] = {0};
		size_t len = 0;

		try 
		{
			inpackage >> len >> fileName;
		} 
		catch (...)
		{
			NAK_RETURN(_T("错误的命令格式"));
		}

		NetClient_Ptr pClient;
		if (!m_ClientList.peek(sck, pClient)) 
		{
			return;
		}

		const size_t bufSize = 1024;
		LPSTR pbuf = pNetLayer->GetIMemPool()->Alloc(bufSize);
		CPkgHelper out(pbuf, bufSize,header.Cmd,OBJECT_UNKOWN);

		stdex::tString path = utility::GetAppPath() + fileName;
		DWORD size = pClient->GetFileSize(path.c_str()) ;
		DWORD crc = pClient->GetFileCrc(path.c_str());
		out << 1L
			<< size
			<< crc;

		header.Length = out.GetLength();
		out.pushHeader(header);
		pNetLayer->SendPackage(sck, pbuf, out.GetLength());
	}

	void CRTDataSvr::Cmd_SysOpt_GetList(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
	{
//		//TEST_THIS_SECTION(__FUNCTION__);

		const size_t bufsiz = 5*1024*1024;
		LPSTR lpSendPackage = pNetLayer->GetIMemPool()->Alloc(bufsiz);
		CPkgHelper outpackage(lpSendPackage, bufsiz, header.Cmd,OBJECT_UNKOWN);

		outpackage << 1L << 0L;
		DWORD *pCount = (DWORD *)(lpSendPackage + sizeof(pkgheader) + 4);

		struct FGetSysoptData : public std::unary_function<std::pair<stdex::tString, stdex::tString>, void> {
			FGetSysoptData(CPkgHelper& pkg, DWORD *pCount) : outpackage(pkg), m_pCount(pCount) {}
			void operator()(const std::pair<stdex::tString, stdex::tString>& opt)
			{
				outpackage << opt.first << '=' << opt.second;
				(*m_pCount)++;
			}
		private:
			DWORD *m_pCount;
			CPkgHelper& outpackage;
		};

		m_pDataCache->GetSysopts().foreach(FGetSysoptData(outpackage, pCount));

		header.Length=outpackage.GetLength();
		outpackage.pushHeader(header);
		pNetLayer->SendPackage(sck, lpSendPackage,outpackage.GetLength());
	}

	void CRTDataSvr::Cmd_Favorite_GetList(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
	{
//		//TEST_THIS_SECTION(__FUNCTION__);

		const size_t bufsiz = 2*1024*1024;
		LPSTR lpSendPackage = pNetLayer->GetIMemPool()->Alloc(bufsiz);
		CPkgHelper outpackage(lpSendPackage, bufsiz, header.Cmd,OBJECT_UNKOWN);
		outpackage << 1L << 0L;
		DWORD *pCount = (DWORD *)(lpSendPackage + sizeof(pkgheader) + 4);

		struct FGetFavorites : public std::unary_function<FavoriteCache_Ptr, void> {
			FGetFavorites(std::multimap<int, FavoriteCache_Ptr>& favorites)
				: m_favorites(favorites) {}
			void operator()(FavoriteCache_Ptr f)
			{
				m_favorites.insert(std::make_pair<int, FavoriteCache_Ptr>(f->GetSerial(), f));
			}
		private:
			std::multimap<int, FavoriteCache_Ptr>& m_favorites;
		};

		std::multimap<int, FavoriteCache_Ptr> favorites;
		m_pDataCache->GetFavorites().foreach_value(FGetFavorites(favorites));

		std::multimap<int, FavoriteCache_Ptr>::iterator it;
		for (it = favorites.begin(); it != favorites.end(); ++it) {
			i8desk::db::tFavorite f ;
			if (it->second->GetFavorite(f)) {
				outpackage << f.Type << f.Name << f.URL;
				(*pCount)++;
			}
		}

		header.Length=outpackage.GetLength();
		outpackage.pushHeader(header);
		pNetLayer->SendPackage(sck, lpSendPackage, outpackage.GetLength());
	}

	void CRTDataSvr::Cmd_Boot_GetList(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
	{
//		//TEST_THIS_SECTION(__FUNCTION__);

		db::tClient client;
		try {
			inpackage.PopString(client.Name);
		} catch (...) {
			Cmd_Nak(pNetLayer, sck, header);
			return;
		} 

		const size_t bufsiz = 2*1024*1024;
		LPSTR lpSendPackage = pNetLayer->GetIMemPool()->Alloc(bufsiz);
		CPkgHelper outpackage(lpSendPackage, bufsiz, header.Cmd,OBJECT_UNKOWN);
		outpackage << 1L << 0L;
		DWORD *pCount = (DWORD *)(lpSendPackage + sizeof(pkgheader) + 4);
		
		if (m_pTableMgr->GetClientTable()->GetData(client.Name, &client, 0) != 0) 
			utility::Strcpy(client.AID, DEFAULT_AREA_GUID);

		IBootTaskAreaRecordset *pRecordset = 0;
		ICmpBootTaskRecordset	*pReset = 0;

		struct Filter : public IBootTaskAreaFilter {
					Filter(TCHAR* AID) : m_AID(AID) {}
					bool bypass(const db::tBootTaskArea *const d) {
						return _tcscmp(d->AID , m_AID) != 0;
					}
				private:
					TCHAR *m_AID;
				} filter(client.AID);
		m_pTableMgr->GetBootTaskAreaTable()->Select(&pRecordset,&filter,0);
		
		for (uint32 i=0;i<pRecordset->GetCount();i++)
		{
			db::tBootTaskArea *taskArea = pRecordset->GetData(i);
			db::tCmpBootTask task;
			if(!m_pTableMgr->GetCmpBootTaskTable()->GetData(taskArea->TID,&task))
			{
				outpackage << task.TID
					<< task.Type
					<< task.Flag
					<< task.RunDate
					<< task.StartTime
					<< task.DelayTime
					<< task.EndTime
					<< task.EndDate
					<< task.RunType
					<< task.Status
					<< task.Parameter;
				(*pCount)++;
			}
		}

		pRecordset->Release();
		header.Length=outpackage.GetLength();
		outpackage.pushHeader(header);
		pNetLayer->SendPackage(sck, lpSendPackage, header.Length);
	}
	
	void CRTDataSvr::Cmd_Svrrpt_ServerInfo_Ex(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
	{
//		//TEST_THIS_SECTION(__FUNCTION__);

		sockaddr_in addr = {0};
		int len = sizeof(addr);
		::getpeername(sck, (sockaddr *)&addr, &len);

		DWORD ip = addr.sin_addr.s_addr;

		try 
		{
			db::tServerStatus d ;

			TCHAR HostName[ MAX_NAME_LEN ] = {0};
			std::tr1::array<DWORD, 4> IPs;
			inpackage >> HostName >> IPs[0] >> IPs[1] >> IPs[2] >> IPs[3];

			//for(size_t i = 0; i != IPs.size(); ++i)
			//	IPs[i] = ::htonl(IPs[i]);

			m_SvrList.set(ip, HostName);

			TCHAR szHostName[ MAX_NAME_LEN ] = {0};			
			DWORD nSize = _countof(szHostName);
			::GetComputerName(szHostName, &nSize);

			IServerRecordset	*pServerReset = 0;

			m_pTableMgr->GetServerTable()->Select(&pServerReset, 0, 0);
			
			// 找到从服务
			bool isMainServer = false;
			for (uint32 i=0;i<pServerReset->GetCount();i++)
			{
				const db::tServer *server = pServerReset->GetData(i);

				bool match = false;
				for(size_t i = 0; i != IPs.size(); ++i)
				{
					if( IPs[i] == server->SvrIP )
					{
						match = true;
						break;
					}
				}

				if( match || utility::Stricmp(server->SvrName, HostName) == 0 )
				{
					utility::Strcpy(d.SvrID, server->SvrID);
					utility::Strcpy(d.SvrName, server->SvrName);
					d.SvrType = server->SvrType;
					break;
				}
			}

			// 没找到从服务
			if( utility::IsEmpty(d.SvrID) )
			{
				db::tServer newServer;
				utility::Strcpy(newServer.SvrID, utility::CreateGuidString());
				utility::Strcpy(newServer.SvrName, HostName);
				newServer.SvrType = OtherServer;
				newServer.SvrIP	= IPs[0];
				newServer.Ip1	= IPs[0];
				newServer.Ip2	= IPs[1];
				newServer.Ip3	= IPs[2];
				newServer.Ip4	= IPs[3];
				newServer.Speed = 0;
				newServer.BalanceType = BALANCE_POLL;

				m_pTableMgr->GetServerTable()->Insert(&newServer);

				// 关联虚拟盘
				IVDiskRecordset	*pVDiskReset = 0;
				m_pTableMgr->GetVDiskTable()->Select(&pVDiskReset, 0, 0);
				for(uint32 i = 0; i != pVDiskReset->GetCount(); ++i)
				{
					db::tVDisk *vDisk = pVDiskReset->GetData(i);
					if(vDisk->SoucIP == newServer.SvrIP )
					{
						utility::Strcpy(vDisk->SvrID, newServer.SvrID);
						m_pTableMgr->GetVDiskTable()->Update(vDisk->VID, vDisk, MASK_TVDISK_SVRID);
					}
				}
				pVDiskReset->Release();

				utility::Strcpy(d.SvrID, newServer.SvrID);
				utility::Strcpy(d.SvrName, newServer.SvrName);

				{
					SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
					if (s == INVALID_SOCKET)
					{
						Log(LM_WARNING, _T("UDP套接字打开失败!\n"));
						return;
					}

					// 通知控制台
					try
					{
						sockaddr_in address		= {0};
						int len					= sizeof(address);
						address.sin_family		= AF_INET;
						address.sin_addr.s_addr	= GetConsoleIP();
						address.sin_port		= ::htons(CONTROL_UDP_PORT);

						char buf[512] = {0};
						i8desk::CPkgHelper out(buf, CMD_ADD_NEW_SERVER, i8desk::OBJECT_UNKOWN);
						out << 1L;

						sendto(s, out.GetBuffer(), out.GetLength(), 0, (sockaddr*)&address, len );
					}
					catch(std::exception &e)
					{
						e.what();
						Log(i8desk::LM_ERROR, _T("发送UDP数据失败[TO:IP=%d, err=%d]!"), ip, ::GetLastError());
					}
					::closesocket(s);
				}
			}
			pServerReset->Release();

			
			//模块状态
			{
				inpackage >> d.I8DeskSvr;
				inpackage >> d.I8VDiskSvr;
				inpackage >> d.I8UpdateSvr;
				inpackage >> d.DNAService;
				inpackage >> d.I8MallCashier;
				inpackage >> d.LastBackupDBTime;
				inpackage >> d.CPUUtilization;
				inpackage >> d.MemoryUsage;
				inpackage >> d.RemainMemory;
				inpackage >> d.NetworkSendRate;
				inpackage >> d.NetworkRecvRate;
				inpackage >> d.DeviceInfo;

				if( d.SvrType == MainServer )
					d.I8DeskSvr = 1;

				uint64 mask = 0
					| MASK_TSERVERSTATUS_SVRID
					| MASK_TSERVERSTATUS_SVRNAME
					| MASK_TSERVERSTATUS_SVRTYPE
					| MASK_TSERVERSTATUS_I8DESKSVR	
					| MASK_TSERVERSTATUS_I8VDISKSVR
					| MASK_TSERVERSTATUS_I8UPDATESVR
					| MASK_TSERVERSTATUS_I8MALLCASHIER
					| MASK_TSERVERSTATUS_LASTBACKUPDBTIME
					| MASK_TSERVERSTATUS_CPUUTILIZATION
					| MASK_TSERVERSTATUS_MEMORYUSAGE
					| MASK_TSERVERSTATUS_DEADCOUNTER
					| MASK_TSERVERSTATUS_REMAINMEMORY
					| MASK_TSERVERSTATUS_NETWORKSENDRATE
					| MASK_TSERVERSTATUS_NETWORKRECVRATE
					| MASK_TSERVERSTATUS_DEVICEINFO;
				m_pTableMgr->GetServerStatusTable()->Ensure(&d, mask);
			}

			//磁盘状态
			{
				db::tDiskStatus disk ;
				utility::Strcpy(disk.SvrID, d.SvrID);
				utility::Strcpy(disk.SvrName, d.SvrName);
				int  count;
				inpackage >> count;
				for (int i = 0; i < count; i++) {
					inpackage 
						>> disk.Partition
						>> disk.Type
						>> disk.Capacity
						>> disk.UsedSize
						>> disk.FreeSize
						>> disk.ReadDataRate;
					m_pTableMgr->GetDiskStatusTable()->Ensure(&disk,
						MASK_TDISKSTATUS_SVRNAME
						| MASK_TDISKSTATUS_TYPE
						| MASK_TDISKSTATUS_CAPACITY
						| MASK_TDISKSTATUS_USEDSIZE
						| MASK_TDISKSTATUS_FREESIZE
						| MASK_TDISKSTATUS_DEADCOUNTER
						);
				}
			}

			ACK();
		} catch (...) {
			NAK_RETURN(_T("错误的消息"));
		} 
	}


	void CRTDataSvr::Cmd_Clirpt_ClientInfo(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
	{
//		//TEST_THIS_SECTION(__FUNCTION__);

		sockaddr_in address;
		int len = sizeof(sockaddr_in);
		getpeername(sck,(sockaddr*)&address,&len);

		db::tClient client ;
		client.IP = address.sin_addr.s_addr;

	
			try 
			{
				inpackage >> client.Name >> client.Gate >> client.DNS >>client.MAC >> client.ProtInstall;
				TCHAR PartitionArea[MAX_DISKPARTION_LEN]={0};
				TCHAR AllArea[MAX_DISKPARTION_LEN]={0};
				TCHAR ClientDataVer[MAX_VERSTRING_LEN]={0};
				inpackage.PopString(PartitionArea);
				inpackage.PopString(AllArea);
				_stprintf(AllArea, _T("%s%s%s"), AllArea, _T("|"), PartitionArea );
				utility::Strcpy(client.Partition, AllArea);
				inpackage>> client.ProtVer >> client.MenuVer >> ClientDataVer >> client.CliSvrVer >> client.VDiskVer

				 >> client.DNS2 >> client.Mark >> client.Temperature;
			
				_stprintf(client.MenuVer, _T("%s%s%s"), client.MenuVer, _T("|"),ClientDataVer );
				utility::Strcpy(client.MenuVer, client.MenuVer);

			} 
			catch (...) 
			{
				NAK_RETURN(_T("错误的消息"));
			} 
		
		db::tClient c ;
		if (m_pTableMgr->GetClientTable()->GetData(client.Name, &c, 0) == 0 && !utility::IsEmpty(c.AID))
			utility::Strcpy(client.AID, c.AID);
		else 
			utility::Strcpy(client.AID, DEFAULT_AREA_GUID);

		client.Online = 1;

		//Log(LM_INFO, _T("Name = %s 的客户机上报信息内容  IP = %d  Gate = %d DNS = %d MAC = %s AID = %s  ProtInstall = %d   Partition = %s    ProtVer = %s    VDiskVer = %s MenuVer = %s CliSvrVer = %s DNS2 = %d Mark = %d, Online = %d .\n"), 
		//	client.Name , client.IP,client.Gate ,client.DNS, client.MAC,client.AID, client.ProtInstall, client.Partition,client.ProtVer,client.VDiskVer, client.MenuVer,
		//	client.CliSvrVer, client.DNS2,client.Mark,client.Online);

		m_pTableMgr->GetClientTable()->Ensure(&client, 
			MASK_TCLIENT_NAME
			| MASK_TCLIENT_IP 
			| MASK_TCLIENT_GATE 
			| MASK_TCLIENT_DNS 
			| MASK_TCLIENT_MAC 
			| MASK_TCLIENT_AID 
			| MASK_TCLIENT_PROTINSTALL 
			| MASK_TCLIENT_PARTITION 
			| MASK_TCLIENT_PROTVER 
			| MASK_TCLIENT_VDISKVER 
			| MASK_TCLIENT_MENUVER 
			| MASK_TCLIENT_CLISVRVER 
			| MASK_TCLIENT_DNS2
			| MASK_TCLIENT_MARK 
			| MASK_TCLIENT_ONLINE
			| MASK_TCLIENT_TEMPERATURE
			);

		{
			NetClient_Ptr pClient;
			if (m_ClientList.peek(sck, pClient))
			{
				if(utility::Strcmp(pClient->GetName(),client.Name)!= 0)
				pClient->SetName(client.Name);
			}

		}

		IVDiskStatusRecordset *pRecordset = 0;
		IVDiskStatusFilter *pFilter = 0;

		m_pTableMgr->GetVDiskStatusTable()->Select(&pRecordset, pFilter, 0);
		uint32 n = pRecordset->GetCount();

		const size_t bufsiz = 32*1024;
		LPSTR lpSendPackage = pNetLayer->GetIMemPool()->Alloc(bufsiz);
		CPkgHelper out(lpSendPackage, bufsiz, header.Cmd,OBJECT_UNKOWN);

		//游戏的版本使用游戏表和运行方式表中的大的一个
		//因为它们都是时间,大的表示最新的版本号
		ulong gver = std::max(m_pTableMgr->GetGameTable()->GetVersion(),
			m_pTableMgr->GetRunTypeTable()->GetVersion());
		ulong taskver = std::max(m_pTableMgr->GetCmpBootTaskTable()->GetVersion(),
			m_pTableMgr->GetBootTaskAreaTable()->GetVersion());

		DWORD BusinisssVer = 0;
		ISvrPlug *pBusiniss = m_pPlugMgr->FindPlug(PLUG_BUSINESS_NAME);
		if( pBusiniss == 0 )
			Log(LM_INFO, _T("没有找到Businiss 插件"));
		else
			BusinisssVer = pBusiniss->SendMessage(E_BSC_GET_GAMEEX_GLOBAL_VERSION,0,0);

		//Log(LM_DEBUG,_T("gver = %d, taskver = %d BusinisssVer = %d "),gver,taskver,BusinisssVer);


		out << 1L 
			<< gver
			<< m_pTableMgr->GetVDiskTable()->GetVersion()
			<< m_pIconTable->GetVersion()
			<< m_pClientSoftMonitor->GetVersion()
			<< m_pTableMgr->GetSysOptTable()->GetVersion()
			<< taskver
			<< m_pTableMgr->GetPlugToolTable()->GetVersion()
			<< BusinisssVer;

		out << n;
		for (uint32 i = 0; i < n; i++) 
		{
			db::tVDiskStatus *d = pRecordset->GetData(i);
			out << d->VID << d->Version;
		}
		pRecordset->Release();

		header.Length = out.GetLength();
		out.pushHeader(header);
		pNetLayer->SendPackage(sck, lpSendPackage, out.GetLength());
	}

	void CRTDataSvr::Cmd_Clirpt_Hardware(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
	{
		//TEST_THIS_SECTION(__FUNCTION__);

		db::tClient client , tmpclient;
		try {
			inpackage >> tmpclient.Name >> tmpclient.MAC
				>> tmpclient.CPU 
				>> tmpclient.Memory
				>> tmpclient.Disk
				>> tmpclient.Video
				>> tmpclient.Mainboard
				>> tmpclient.Network
				>> tmpclient.Camera
				>> tmpclient.System
				>> tmpclient.Audio;
			}
		 catch (...) {
			Log(LM_INFO, _T("Name = %s 的客户机上报硬件信息错误.\n"));
			NAK_RETURN(_T("错误的消息"));
		}

		 if (m_pTableMgr->GetClientTable()->GetData(tmpclient.Name, &client) != 0)
			 NAK_RETURN(_T("没有该客户机信息"));

		uint64 mask = 0;
		if( IsHardwareChange(tmpclient, client, mask) )
		{
			utility::Strcpy(tmpclient.AID,client.AID);
			utility::Strcpy(tmpclient.CliSvrVer,client.CliSvrVer);
			tmpclient.DNS = client.DNS;
			tmpclient.DNS2 = client.DNS2;
			tmpclient.Gate = client.Gate;
			tmpclient.IP = client.IP;
			tmpclient.Mark = client.Mark;
			utility::Strcpy(tmpclient.MenuVer,client.MenuVer);
			tmpclient.Online = client.Online;
			utility::Strcpy(tmpclient.Partition,client.Partition);
			tmpclient.ProtInstall = client.ProtInstall;
			utility::Strcpy(tmpclient.ProtVer,client.ProtVer);
			tmpclient.Socket = client.Socket;
			utility::Strcpy(tmpclient.VDiskVer,client.VDiskVer);

			ClientHDChanged_.push_back(tmpclient);
			Cmd_NotifyCon_HardwareChange(tmpclient, mask);

		}

		ACK();
	}

	bool CRTDataSvr::IsFirstReportHardware(const db::tClient &client)
	{
		//if(utility::IsEmpty(client.System))
		//	return true;
		if(utility::IsEmpty(client.MAC))
			return true;
		if(utility::IsEmpty(client.CPU))
			return true;
		if(utility::IsEmpty(client.Memory))
			return true;
		if(utility::IsEmpty(client.Mainboard))
			return true;
		//if(utility::IsEmpty(client.Network))
		//	return true;
		//if(utility::IsEmpty(client.Video))
		//	return true;

		return false;
	}

	bool CRTDataSvr::IsHardwareChange(const db::tClient &newclient, const db::tClient &oldclient, uint64 &mask)
	{
		bool bRet = false;
		if(IsFirstReportHardware(oldclient))
		{
			m_pTableMgr->GetClientTable()->Ensure(&newclient, 
				MASK_TCLIENT_MAC
				| MASK_TCLIENT_CPU 
				| MASK_TCLIENT_MEMORY
				| MASK_TCLIENT_DISK
				| MASK_TCLIENT_VIDEO
				| MASK_TCLIENT_MAINBOARD
				| MASK_TCLIENT_NETWORK
				| MASK_TCLIENT_CAMERA
				| MASK_TCLIENT_SYSTEM
				| MASK_TCLIENT_AUDIO
				);
		}
		else
		{
			if(0 != utility::Strcmp(newclient.MAC, oldclient.MAC))
			{
				Log(LM_INFO, _T("Name = %s 的客户机 MAC 地址由原来[%s]变化为[%s].\n"), 
					newclient.Name,oldclient.MAC,newclient.MAC);
				mask |= MASK_TCLIENT_MAC;
				bRet = true;
			}

			if(0 != utility::Strcmp(newclient.CPU, oldclient.CPU))
			{
				Log(LM_INFO, _T("Name = %s 的客户机 CPU 地址由原来[%s]变化为[%s].\n"), 
					newclient.Name,oldclient.CPU,newclient.CPU);
				mask |= MASK_TCLIENT_CPU;
				bRet = true;
			}

			if(0 != utility::Strcmp(newclient.Memory, oldclient.Memory))
			{
				Log(LM_INFO, _T("Name = %s 的客户机 MEMORY 地址由原来[%s]变化为[%s].\n"), 
					newclient.Name,oldclient.Memory,newclient.Memory);
				mask |= MASK_TCLIENT_MEMORY;
				bRet = true;
			}

			if(0 != utility::Strcmp(newclient.Video, oldclient.Video))
			{
				Log(LM_INFO, _T("Name = %s 的客户机 DISK 地址由原来[%s]变化为[%s].\n"), 
					newclient.Name,oldclient.Video,newclient.Video);
				mask |= MASK_TCLIENT_VIDEO;
				bRet = true;
			}

			if(0 != utility::Strcmp(newclient.Mainboard, oldclient.Mainboard))
			{
				Log(LM_INFO, _T("Name = %s 的客户机 MAINBOARD 地址由原来[%s]变化为[%s].\n"), 
					newclient.Name,oldclient.Mainboard,newclient.Mainboard);
				mask |= MASK_TCLIENT_MAINBOARD;
				bRet = true;
			}

			if(0 != utility::Strcmp(newclient.Network, oldclient.Network))
			{
				Log(LM_INFO, _T("Name = %s 的客户机 NETWORK 地址由原来[%s]变化为[%s].\n"), 
					newclient.Name,oldclient.Network,newclient.Network);
				mask |= MASK_TCLIENT_NETWORK;
				bRet = true;
			}

			if(0 != utility::Strcmp(newclient.Camera, oldclient.Camera))
			{
				Log(LM_INFO, _T("Name = %s 的客户机 CAMERA 地址由原来[%s]变化为[%s].\n"), 
					newclient.Name,oldclient.Camera,newclient.Camera);
				mask |= MASK_TCLIENT_CAMERA;
				bRet = true;
			}

			if(0 != utility::Strcmp(newclient.System, oldclient.System))
			{
				Log(LM_INFO, _T("Name = %s 的客户机 SYSTEM 地址由原来[%s]变化为[%s].\n"), 
					newclient.Name,oldclient.System,newclient.System);
				mask |= MASK_TCLIENT_SYSTEM;
				bRet = true;
			}

			if(0 != utility::Strcmp(newclient.Audio, oldclient.Audio))
			{
				Log(LM_INFO, _T("Name = %s 的客户机 AUDIO 地址由原来[%s]变化为[%s].\n"), 
					newclient.Name,oldclient.Audio,newclient.Audio);
				mask |= MASK_TCLIENT_AUDIO;
				bRet = true;
			}
		}

		return bRet;

	}

	void CRTDataSvr::Cmd_Vdsvr_GetList(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
	{
		//TEST_THIS_SECTION(__FUNCTION__);

		const size_t bufsiz = 1024*1024/2;
		LPSTR lpSendPackage = pNetLayer->GetIMemPool()->Alloc(bufsiz);
		CPkgHelper outpackage(lpSendPackage, bufsiz, header.Cmd,OBJECT_UNKOWN);

		struct FGetVDisk : public std::unary_function<VDiskCache_Ptr, void> {
			FGetVDisk(CPkgHelper& pkg, DWORD *pCount) 
				: outpackage(pkg)
				, m_pCount(pCount) 
			{
			}
			void operator()(VDiskCache_Ptr vdisk)
			{
				i8desk::db::tVDisk vd;
					if (vdisk->GetVDisk(vd)) {
						outpackage << vd.VID
							<< vd.SoucIP
							<< vd.Port
							<< vd.SvrDrv
							<< vd.CliDrv
							<< vd.Type
							<< vd.LoadType;
						(*m_pCount)++;
					}
			}
			CPkgHelper& outpackage;
			DWORD *m_pCount;
		};

		outpackage << 1L << 0L;
		DWORD *pCount = (DWORD *)(lpSendPackage + sizeof(pkgheader) + 4);

		m_pDataCache->GetVDisks().foreach_value(FGetVDisk(outpackage, pCount));

		outpackage.pushHeader(header);
		pNetLayer->SendPackage(sck, lpSendPackage, outpackage.GetLength());
	}


	struct ViewClient
	{
		SOCKET sock_;
		INetLayer *netLayer_;
		async::thread::AutoEvent event_;

		ViewClient(SOCKET sock, INetLayer *netLayer)
			: sock_(sock)
			, netLayer_(netLayer)
		{
			event_.Create();
		}

		void BegineWait()
		{
			if( !event_.WaitForEvent(30 * 1000) )
			{
				char buf[128] = {0};
				CPkgHelper out(buf);
				out << 0 << _T("获取客户机信息超时");
				netLayer_->SendPackage(sock_, (LPSTR)out.GetBuffer(), out.GetLength());

				return;
			}
		}

		void OnWait(LPSTR buf)
		{
			event_.SetEvent();

			CPkgHelper outpackage(buf);
			netLayer_->SendPackage(sock_, (LPSTR)outpackage.GetBuffer(), outpackage.GetLength()); 
			Log(LM_DEBUG, _T("收到客户端向控制台发出的查看命令成功!\n"));
		}
	};
	typedef std::tr1::shared_ptr<ViewClient> ViewClientPtr;
	ViewClientPtr g_View;

	//转发控制台向客户端发的命令
	void CRTDataSvr::Cmd_Game_ConToClient(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage, pkgheader &header)
	{
		Log(LM_DEBUG, _T("收到控制台向客户端发出的操作命令!\n"));

		DWORD IpArray[sizeof(DWORD)*1024*4]={0};

		DWORD dwDataLen;
		DWORD dwClientIpArraySize;
		DWORD dwClientCount;

		DWORD viewFlag = 0;
		BYTE data[1024] = {0};
		try 
		{
			inpackage >> dwDataLen >> viewFlag;
			inpackage.PopPointer(data, dwDataLen - 4);
			inpackage >> dwClientIpArraySize;
			dwClientCount = dwClientIpArraySize/sizeof(DWORD);
			inpackage.PopPointer(IpArray, dwClientCount);

			Log(LM_DEBUG, _T("收到控制台向客户端发出的操作或者查看命令!\n"));

		} 
		catch (...)
		{
			NAK_RETURN(_T("错误的命令格式"));
		} 

		//1.填写头信息
		char buf[2048]={0};
		CPkgHelper out(buf, header.Cmd, OBJECT_UNKOWN);
		out << utility::CalBufCRC32(buf, header.Length) 
			<< viewFlag;
		out.PushPointer(data, dwDataLen - 4);

		SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (s == INVALID_SOCKET)
		{
			Log(LM_WARNING, _T("UDP套接字打开失败!\n"));
			NAK_RETURN(_T("命令失败"));
		}

		int iSocketBufSize = 0x100000;
		if(setsockopt(s, SOL_SOCKET, SO_SNDBUF,
			(char *)&iSocketBufSize, sizeof(iSocketBufSize)) == SOCKET_ERROR)
		{
			Log(LM_WARNING, _T("UDP套接字设置选项失败!\n"));
			closesocket(s);
			NAK_RETURN(_T("命令失败"));
		}

		for(DWORD i = 0; i < dwClientCount; i++)
		{
			sockaddr_in address;
			int len					= sizeof(address);
			address.sin_family		= AF_INET;
			address.sin_addr.s_addr = IpArray[i];
			address.sin_port		= htons(CLIENT_UDP_PORT);
			int iret				= sendto(s, out.GetBuffer(), out.GetLength(), 0, (sockaddr*)&address, len);
			
			if (iret != out.GetLength()) 
			{
				stdex::tString Ip = CA2T(::inet_ntoa(address.sin_addr));
				Log(LM_WARNING, _T("发送UDP数据失败[TO:IP=%s]!\n"),Ip.c_str());
				NAK_RETURN(_T("命令失败"));
			}

			Log(LM_DEBUG, _T("发送控制台向客户端发出的操作命令成功!\n"));

		}

		closesocket(s);

		if( viewFlag == Ctrl_ViewProcInfo ||
			viewFlag == Ctrl_ViewSvrInfo ||
			viewFlag == Ctrl_ViewSysInfo )
		{
			g_View.reset(new ViewClient(sck, pNetLayer));
			g_View->BegineWait();
		}
		else
		{
			ACK();
		}
	}

	void CRTDataSvr::Cmd_Game_ClientToCon(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
	{			 
		g_View->OnWait((LPSTR)lpPackage);
	}


	void CRTDataSvr::Cmd_Vdsvr_ReqStatus(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
	{
		//TEST_THIS_SECTION(__FUNCTION__);

		try {
			sockaddr_in address;
			int len = sizeof(sockaddr_in);
			getpeername(sck, (sockaddr*)&address, &len);

			db::tVDiskStatus d;
			d.VDiskIP =  address.sin_addr.s_addr;

			DWORD dwVdiskCount;
			inpackage >> dwVdiskCount;

			std::vector<db::tVDiskStatus> VDiskStatusList;

			for (DWORD i = 0; i < dwVdiskCount; i++) {
				inpackage >> d.VID >> d.Version >> d.ClientCount ;
				VDiskStatusList.push_back(d);
			}
			
			memset(&d, 0, sizeof(d));
			inpackage >> d.HostName; 
			
			for (size_t i = 0; i < VDiskStatusList.size(); i++) {
				db::tVDiskStatus& vds = VDiskStatusList[i];
				utility::Strcpy(vds.HostName, d.HostName);
				m_pTableMgr->GetVDiskStatusTable()->Ensure(&vds, 
					MASK_TVDISKSTATUS_VERSION 
					| MASK_TVDISKSTATUS_CLIENTCOUNT 
					| MASK_TVDISKSTATUS_HOSTNAME
					);		
			}

			struct Filter : public IVDiskStatusFilter {
				Filter(std::vector<db::tVDiskStatus>& VDiskStatusList, ulong ip) 
					: m_VDiskStatusList(m_VDiskStatusList), m_ip(ip)
				{
				}
				bool bypass(const db::tVDiskStatus *const d)
				{
					if (d->VDiskIP != m_ip)
						return true;
					for (size_t i = 0; i < m_VDiskStatusList.size(); i++) {
						db::tVDiskStatus& vds = m_VDiskStatusList[i];
						if (_tcscmp(vds.VID, d->VID) == 0)
							return true;
					}
					return false; //不包含在上报的列表中，则删除
				}
			private:
				ulong m_ip;
				std::vector<db::tVDiskStatus>& m_VDiskStatusList;
			} filter(VDiskStatusList, d.VDiskIP);

			//删除多余的记录
			m_pTableMgr->GetVDiskStatusTable()->Delete(&filter); 

			ACK();
		} catch (...) {
			NAK_RETURN(_T("错误的命令格式"));
		} 
	}

	void CRTDataSvr::Cmd_Vdsvr_VDiskClient(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
	{
		try 
		{
			DWORD dwVdiskCount;
			inpackage >> dwVdiskCount;

			for (DWORD i = 0; i < dwVdiskCount; i++) 
			{
				db::tVDiskClient d ;
				inpackage	>> d.ClientIP;
				inpackage	>> d.VID;
				inpackage	>> d.VDiskIP;
				inpackage	>> d.ReadCount;
				inpackage	>> d.ReadSpeed;
				inpackage	>> d.ReadMax; 
				inpackage	>> d.ConnectTime;
				inpackage	>> d.CacheShooting;

				
				m_pTableMgr->GetVDiskClientTable()->Ensure(&d, ~0); 
			}
		} 
		catch (...) 
		{
			NAK_RETURN(_T("错误的命令格式"));
		} 
	
		ACK();
	}

	void CRTDataSvr::Cmd_CliUpt_DownFileInfo(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
	{
		//TEST_THIS_SECTION(__FUNCTION__);

		TCHAR FileName[MAX_PATH];//相对于data\Client文件夹的文件名
		DWORD dwFPType;
		DWORD dwProgramType;

		try {
			inpackage >> dwProgramType >> dwFPType >> FileName;
		} catch (...){
			NAK_RETURN(_T("错误的命令格式"));
		} 

		stdex::tString strUserDir = m_pTableMgr->GetOption(OPT_M_USERDATADIR, 
			utility::GetAppPath()+_T("ClientData"));

		TCHAR Path[MAX_PATH];

		if (dwFPType == FP_REALPATH) {
			//个人磁盘
			if (dwProgramType==0) 
				_stprintf(Path, _T("%s\\%s"), strUserDir.c_str(), FileName);
			else
				_stprintf(Path, _T("%s%s"), utility::GetAppPath().c_str(), FileName);
		} 
		else
			_stprintf(Path,_T("%s"),FileName);

		NetClient_Ptr pClient;
		if (!m_ClientList.peek(sck, pClient)) {
			return;
		}

		LPSTR pbuf = pNetLayer->GetIMemPool()->Alloc(1024);
		CPkgHelper out(pbuf, 1024,header.Cmd,OBJECT_UNKOWN);
		
		out << 1L
			<< pClient->GetFileSize(Path) 
			<< pClient->GetFileCrc(Path);

		header.Length = out.GetLength();
		out.pushHeader(header);
		pNetLayer->SendPackage(sck, pbuf, out.GetLength());
	}

	void CRTDataSvr::Cmd_CliUpt_DownFile(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
	{
		//TEST_THIS_SECTION(__FUNCTION__);

		TCHAR FileName[MAX_PATH];//相对于data\Client文件夹的文件名
		DWORD dwFPType;
		DWORD dwProgramType;

		try {
			inpackage >> dwProgramType >> dwFPType >> FileName;
		} catch (...){
			NAK_RETURN(_T("错误的命令格式"));
		} 

		stdex::tString strUserDir = m_pTableMgr->GetOption(OPT_M_USERDATADIR, 
			utility::GetAppPath()+_T("ClientData"));

		TCHAR Path[MAX_PATH];

		if (dwFPType == FP_REALPATH) {
			//个人磁盘
			if (dwProgramType==0) 
				_stprintf(Path, _T("%s\\%s"), strUserDir.c_str(), FileName);
			else
				_stprintf(Path, _T("%s%s"), utility::GetAppPath().c_str(), FileName);
		} 
		else
			_stprintf(Path,_T("%s"),FileName);

		NetClient_Ptr pClient;
		if (!m_ClientList.peek(sck, pClient)) {
			return;
		}

		TCHAR ErrBuf[MAX_PATH*2];
		const DWORD dwSize = pClient->GetFileSize(Path);

		pClient->SetBlockCount( dwSize / BLOCK_SIZE );
		pClient->SetRemainSize( dwSize % BLOCK_SIZE );
		pClient->ClearIndex();

		if (!pClient->OpenFile(Path,           // open MYFILE.TXT 
			GENERIC_READ,              // open for reading 
			FILE_SHARE_READ,           // share for reading 
			OPEN_EXISTING,             // existing file only 
			FILE_ATTRIBUTE_NORMAL))
		{
			_stprintf(ErrBuf, _T("打开文件%s时发生错误，错误代码%u"), Path, GetLastError());
			NAK_RETURN(ErrBuf);
		}


		const size_t bufsiz = 1024*64;
		LPSTR lpSendPackage = pNetLayer->GetIMemPool()->Alloc(bufsiz);
		CPkgHelper outpackage(lpSendPackage, bufsiz);

		const DWORD dwFirstBlockSize = pClient->GetBlockCount() == 0 ? dwSize : BLOCK_SIZE;
		if (!pClient->ReadFileData(
			((BYTE*)lpSendPackage) + sizeof(pkgheader) + sizeof(DWORD) * 2,
			dwFirstBlockSize))
		{
			_stprintf(ErrBuf,_T("读文件%s时发生错误，错误代码%u"),Path,GetLastError());
			pClient->CloseFile();
			pNetLayer->GetIMemPool()->Free(lpSendPackage);
			NAK_RETURN(ErrBuf);
		}

		pClient->IncreaseIndex();

		header.Length = sizeof(pkgheader) + sizeof(DWORD) * 2 + dwSize;
		//填写头信息
		memcpy(lpSendPackage, &header, sizeof(header));

		//返回值
		*((DWORD*)(lpSendPackage + sizeof(pkgheader))) = 1;
		//文件大小
		*((DWORD*)(lpSendPackage + sizeof(pkgheader) + sizeof(DWORD))) = dwSize;

		DWORD dwFirstPackageSize = sizeof(pkgheader);
		dwFirstPackageSize += sizeof(DWORD) * 2;
		dwFirstPackageSize += dwFirstBlockSize;

		pClient->SetCurrentCmd( CMD_CLIUPT_DOWNFILE );

		//如果只需要一帧就传送完成则清除记录，以能接收下一个文件传送请求
		if (dwFirstBlockSize == dwSize) {
			pClient->CloseFile();
			pClient->ClearBlockCount();
			pClient->ClearCurrentCmd();
		}

		pNetLayer->SendPackage(sck, lpSendPackage, dwFirstPackageSize);
	}

	void CRTDataSvr::Cmd_CliUpt_DownFile_Start(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
	{
		TCHAR FileName[MAX_PATH]={0};//相对于data\Client文件夹的文件名
		DWORD dwFPType;
		DWORD dwProgramType;

		try {
			inpackage >> dwProgramType >> dwFPType ;
			inpackage.PopString(FileName);
		} catch (...){
			NAK_RETURN(_T("错误的命令格式"));
		} 

		stdex::tString strUserDir = m_pTableMgr->GetOption(OPT_M_USERDATADIR, 
			utility::GetAppPath()+_T("ClientData"));

		TCHAR Path[MAX_PATH];

		if (dwFPType == FP_REALPATH) {
			//个人磁盘
			if (dwProgramType==0) 
				_stprintf(Path, _T("%s\\%s"), strUserDir.c_str(), FileName);
			else
				_stprintf(Path, _T("%s%s"), utility::GetAppPath().c_str(), FileName);
		} 
		else
			_stprintf(Path,_T("%s"),FileName);

		NetClient_Ptr pClient;
		if (!m_ClientList.peek(sck, pClient)) 
			return;

		TCHAR ErrBuf[MAX_PATH*2];
		const DWORD dwSize = pClient->GetFileSize(Path);
		if (dwSize != 0) 
		{
			if (!pClient->OpenFile(Path,           // open MYFILE.TXT 
				GENERIC_READ,              // open for reading 
				FILE_SHARE_READ,           // share for reading 
				OPEN_EXISTING,             // existing file only 
				FILE_ATTRIBUTE_NORMAL))
			{
				_stprintf(ErrBuf, _T("打开文件%s时发生错误，错误代码%u"), Path, GetLastError());
				NAK_RETURN(ErrBuf);
			}

			pClient->SetBlockCount( dwSize / BLOCK_SIZE );
			pClient->SetRemainSize( dwSize % BLOCK_SIZE );
			pClient->ClearIndex();
			pClient->ClearCurrentCmd();

			if (0 == dwSize) 
			{
				pClient->CloseFile();
				pClient->ClearBlockCount();
			}
		}

		const size_t bufsiz = 1024;
		LPSTR lpSendPackage = pNetLayer->GetIMemPool()->Alloc(bufsiz);

		CPkgHelper out(lpSendPackage, bufsiz);
		out << 1L << dwSize;

		header.Length = out.GetLength();
		out.pushHeader(header);
		pNetLayer->SendPackage(sck, lpSendPackage, out.GetLength());
	}

	void CRTDataSvr::Cmd_DownFile_Next(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
	{
		//TEST_THIS_SECTION(__FUNCTION__);

		NetClient_Ptr pClient;
		if (!m_ClientList.peek(sck, pClient)) 
			return;

		const size_t bufsiz = BLOCK_SIZE + 256;
		LPSTR pbuf = pNetLayer->GetIMemPool()->Alloc(bufsiz);
		CPkgHelper out(pbuf, bufsiz);
		BYTE *data = (BYTE*)(pbuf + sizeof(pkgheader) + 4 + 4 + 4);

		if (pClient->HasRemainBlock()) 
		{
			if (pClient->ReadFileData(data, BLOCK_SIZE)) 
			{
				pClient->IncreaseIndex();
				out << 1L;
				if (!pClient->HasRemainBlock() && pClient->GetRemainSize() == 0) 
				{
					pClient->CloseFile();
					pClient->ClearBlockCount();
					pClient->ClearCurrentCmd();
					out << 1L;
				}
				else 
				{
					out << 0L;
				}

				static const DWORD bsize = BLOCK_SIZE; 
				out << bsize;
				header.Length = out.GetLength() + bsize;
			}
			else
			{
				pClient->CloseFile();
				pClient->ClearCurrentCmd();

				out << 0L << _T("读取块数据错误失败");
				header.Length = out.GetLength();
			}
		
		}
		else if (DWORD dwRemainSize = pClient->GetRemainSize()) 
		{
			if (pClient->ReadFileData(data, dwRemainSize))
			{
				out << 1L << 1L << dwRemainSize;
				header.Length = out.GetLength() + dwRemainSize;
			}
			else
			{
				out << 0L << _T("读取最后的剩余数据失败");
				header.Length = out.GetLength();
			}

			//此包发送完成，立即清除记录，以能接收下一个文件传送请求
			pClient->ClearBlockCount();
			pClient->CloseFile();
			pClient->ClearCurrentCmd();
		}
		else
		{
			pClient->CloseFile();
			pClient->ClearBlockCount();
			pClient->ClearCurrentCmd();

			out << 0L << _T("读取数据失败");
			header.Length = out.GetLength();
		}

		out.pushHeader(header);
		pNetLayer->SendPackage(sck, pbuf, header.Length);	
	}

	void CRTDataSvr::Cmd_Boot_DelList(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
	{
		//TEST_THIS_SECTION(__FUNCTION__);

		try 
		{
			DWORD dwRsCount;
			inpackage >> dwRsCount;
			TCHAR TID[64] = {0};
			for (DWORD i = 0; i < dwRsCount; i++)
			{
				inpackage >> TID;

				struct Filter 
					: public ICmpBootTaskFilter 
				{
					Filter(const TCHAR *uid) : m_uid(uid) {}
					bool bypass(const db::tCmpBootTask *const d) 
					{
						return !(d->Flag == 1 && _tcsicmp (d->TID, m_uid) == 0);
					}
					const TCHAR *m_uid;
				};		
				m_pTableMgr->GetCmpBootTaskTable()->Delete(&Filter(TID));
			}
		} 
		catch (...) 
		{
			NAK_RETURN(_T("错误的命令格式"));
		}
	}

	void CRTDataSvr::Cmd_Nak(INetLayer *pNetLayer, SOCKET sck, pkgheader &header, const TCHAR *lpszError)
	{
		LPSTR pbuf = pNetLayer->GetIMemPool()->Alloc(1024);
		CPkgHelper out(pbuf, 1024, header.Cmd,OBJECT_UNKOWN);
		out << 0L << lpszError;
		header.Length = out.GetLength();
		out.pushHeader(header);
		pNetLayer->SendPackage(sck, pbuf, out.GetLength());
	}

void CRTDataSvr::Cmd_CliUpt_GetFileList(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
{
	//TEST_THIS_SECTION(__FUNCTION__);

	DWORD dwCmdType=0;//0-盘符，1-目录，2-文件
	DWORD dwDirType=0;//绝对、相对
	TCHAR Path[MAX_PATH] ={0};
	
	try {
		inpackage >> dwCmdType >> dwDirType >> Path;
	} catch (...) {
		NAK_RETURN(_T("错误的命令格式"));
	} 

	if (dwCmdType == 0) {
		LPSTR lpSendPackage = pNetLayer->GetIMemPool()->Alloc(1024);
		CPkgHelper outpackage(lpSendPackage, 1024, header.Cmd,OBJECT_UNKOWN);
		outpackage.Push(1);

		DWORD dwDrives = GetLogicalDrives();
		for (DWORD i = 0; i < 32; i++) {
			TCHAR buf[] = {_T("A:\\")};
			buf[0] = static_cast<TCHAR>('A' + i);
			if (GetDriveType(buf) != DRIVE_FIXED)
				dwDrives &= ~BitN(i);
				//ClearFlag(dwDrives, BitN(i));
		}
		outpackage.Push(dwDrives);
		//header.Length = outpackage.GetLength();
		outpackage.pushHeader(header);
		pNetLayer->SendPackage(sck, lpSendPackage, outpackage.GetLength());
		return;
	}

	stdex::tString strPath;
	if (dwDirType == FP_ABSPATH)
		strPath = Path;
	else 
		strPath += utility::GetAppPath() + Path;
	strPath += _T("\\*.*");

	TCHAR ErrorBuf[MAX_PATH]={0};
	WIN32_FIND_DATA FindFileData;

	HANDLE hFind = FindFirstFile(strPath.c_str(), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		_stprintf(ErrorBuf,_T("打开路径%s时发生错误，错误代码%u"),Path,GetLastError());
		NAK_RETURN(ErrorBuf);
	}

	std::vector<stdex::tString>	vFileList;
	if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
	{
		if (dwCmdType == 1) 
		{
			if (FindFileData.cFileName[0] == '.') 
			{
				if (FindFileData.cFileName[1] == '\0' ||
					(FindFileData.cFileName[1] == '.' &&
					FindFileData.cFileName[2] == '\0'))
				{
				}
				else
				{
					vFileList.push_back(FindFileData.cFileName);
				}
			}
			else {
				vFileList.push_back(FindFileData.cFileName);
			}
		}
	}
	else 
	{
		if (dwCmdType==2)
			vFileList.push_back(FindFileData.cFileName);
	}

	while (FindNextFile(hFind,&FindFileData))
	{
		if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			if (dwCmdType == 1)
			{
				if (FindFileData.cFileName[0] == '.')
				{
					if (FindFileData.cFileName[1] == '\0' ||
						(FindFileData.cFileName[1] == '.' &&
						FindFileData.cFileName[2] == '\0'))
					{
						continue;
					}
					else 
					{
						vFileList.push_back(FindFileData.cFileName);
					}
				}
				else
				{
					vFileList.push_back(FindFileData.cFileName);
				}
			}
		}
		else
		{
			if (dwCmdType==2)
				vFileList.push_back(FindFileData.cFileName);
		}
	}

	FindClose(hFind);

	DWORD dwStrSize=0;

	for (DWORD i = 0; i < vFileList.size(); i++)
	{
		dwStrSize += vFileList[i].size() + 1;
	}

	DWORD dwPacketSize = sizeof(pkgheader) 
		+ sizeof(DWORD) * 2 
		+ vFileList.size() * sizeof(DWORD) + dwStrSize 
		+ 256;
	LPSTR lpSendPackage = pNetLayer->GetIMemPool()->Alloc(dwPacketSize);
	CPkgHelper outpackage(lpSendPackage, dwPacketSize, header.Cmd,OBJECT_UNKOWN);
	outpackage.Push(1);
	outpackage.Push(vFileList.size());

	for (DWORD i=0;i<vFileList.size();i++)
	{
		outpackage.PushString(vFileList[i].c_str(),vFileList[i].size());
	}

	header.Length=outpackage.GetLength();
	outpackage.pushHeader(header);
	pNetLayer->SendPackage(sck, lpSendPackage, outpackage.GetLength());
}

void CRTDataSvr::Cmd_Con_SetClassIcon(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
{
	DWORD dwCmdType = 0 ,FileSize = 0;
	TCHAR FilePath[MAX_PATH] = {0};
	unsigned char data[MAX_ICON_DATA_SIZE] = {0};

	try 
	{
		inpackage >> dwCmdType >> FilePath >> FileSize ;
		inpackage.PopPointer(data,FileSize);
	}
	catch ( ...)
	{
		NAK_RETURN(_T("错误的命令格式"));
	}

	assert(FileSize);

	stdex::tString fileName;
	fileName = utility::GetAppPath() + FilePath;

	::DeleteFile(fileName.c_str());
	utility::CAutoFile hFile = CreateFile(fileName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if( !hFile.IsValid() )
	{
		DWORD dwError = GetLastError();
		TCHAR szError[MAX_PATH] = {0};
		_com_error Error(dwError);
		_stprintf(szError, _T("创建文件错:%d:%s"), dwError, Error.ErrorMessage());
		NAK_RETURN(szError);
	}

	WriteFileContent(hFile,data,FileSize);

	ACK();

}

void CRTDataSvr::Cmd_Game_Icon(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
{
	db::tIcon icon;
	BYTE  *pBufIcon = icon.data;

	DWORD dwCmdType;

	try {
		inpackage >> dwCmdType >> icon.gid;
		if (dwCmdType==ioAdd)
		{
			inpackage >> icon.size;
			inpackage.PopPointer(icon.data,icon.size);
		}
	} catch (...) {
		NAK_RETURN(_T("错误的命令格式"));
	}

	if(dwCmdType==ioAdd && (icon.size < 1 || icon.size > MAX_ICON_DATA_SIZE))
		NAK_RETURN(_T("错误的图标大小"));

	Log(LM_DEBUG,_T("Cmd_Game_Icon GID = %d , CmdType = %d Size = %d"),icon.gid,dwCmdType,icon.size);

	TCHAR strGID[64]={0};

	TCHAR ErrInfo[MAX_PATH]={0};

	switch (dwCmdType) {
	case ioAdd: {
		//保存到临时数据表中
		if( !m_pIconTable->AddCustomFile2(icon.gid,icon.size,icon.data) )
			assert(0);
		break; }
	case ioDel: {
		//1.删除ICON文件
		if( m_pIconTable->DeleteCustomFile((long)icon.gid) )
			assert(0);
		break; }
	case ioGet: {
		size_t size = 0;
		void *data;
		if ( !m_pIconTable->GetFileData(icon.gid,size,data) ) 
		{
			NAK_RETURN(_T("图标不存在"));
		}
		LPSTR lpSendPackage = pNetLayer->GetIMemPool()->Alloc(size);
		CPkgHelper outpackage(lpSendPackage, size,header.Cmd,OBJECT_UNKOWN);
		outpackage.Push(1);
		outpackage.Push(size);
		outpackage.PushPointer((unsigned char*)data, size);

		header.Length = outpackage.GetLength();
		pNetLayer->SendPackage(sck, lpSendPackage, header.Length);
		return; }
	default: {
		NAK_RETURN(_T("操作未支持"));
		return; }
	}

	ACK();
}

void CRTDataSvr::Cmd_Validate(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
{
	//TEST_THIS_SECTION(__FUNCTION__);
	stdex::tString username, pwd ;

	inpackage >> username >> pwd;

	ISvrPlug *pLogRpt = m_pPlugMgr->FindPlug(PLUG_LOGRPT_NAME);
	if (!pLogRpt) 
		NAK_RETURN(_T("没有找到上报插件"));

	pLogRpt->SendMessage(LOG_RPT_CHECK_USER, (DWORD)username.c_str(), (DWORD)pwd.c_str());
	ACK();
}


void CRTDataSvr::Cmd_Notify_Save_Data(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
{
	//TEST_THIS_SECTION(__FUNCTION__);
	Log(LM_DEBUG, _T("收到控制台保存命令\n"));

	m_pDBWriter->SaveData();

	ISvrPlug *GameMgrPlug  = m_pPlugMgr->FindPlug(PLUG_GAMEMGR_NAME) ;
	if( GameMgrPlug == NULL ) 
	{
		NAK_RETURN(_T("控制台保存命令不成功"));
	}

	GameMgrPlug->SendMessage(CON_SAVE_DATA, 0, 0 );

	ACK();
}

void CRTDataSvr::Cmd_Sysopt_Syntime(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
{
	//TEST_THIS_SECTION(__FUNCTION__);

	LPSTR lpSendPackage = pNetLayer->GetIMemPool()->Alloc(1024);
	CPkgHelper outpackage(lpSendPackage, 1024, header.Cmd,OBJECT_UNKOWN);

	SYSTEMTIME st;
	GetSystemTime(&st);

	outpackage << 1L 
		<< st.wYear
		<<st.wMonth 
		<<st.wDay 
		<<st.wHour 
		<<st.wMinute 
		<<st.wSecond;

	header.Length=outpackage.GetLength();
	outpackage.pushHeader(header);
	pNetLayer->SendPackage(sck, lpSendPackage, outpackage.GetLength());
}

void CRTDataSvr::GetBarSkins(std::vector<i8desk::db::tBarSkin>& BarSkins)
{
	// 检测皮肤文件
	stdex::tString skinPath = utility::GetAppPath() + _T("Skin\\");
	
	WIN32_FIND_DATA fd;
	utility::CAutoFindFile hFind = FindFirstFile((skinPath + _T("*.png")).c_str(), &fd);
	if (!hFind.IsValid()) 
		return ;

	do{
		i8desk::db::tBarSkin bs;
		_tcscpy(bs.PathName, _T("Skin\\"));
		_tcscat(bs.PathName, fd.cFileName);
		stdex::tString style = fd.cFileName;
		style = stdex::Split(style, _T('.'), 0);
		utility::Strcpy(bs.Style, style.c_str());
		BarSkins.push_back(bs);
	} 
	while (::FindNextFile(hFind, &fd));

}

void CRTDataSvr::Cmd_Get_BarSkins(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
{
	//TEST_THIS_SECTION(__FUNCTION__);

	int nBarSkin = 0; 
	BarSkins.clear();
	 GetBarSkins(BarSkins);
	nBarSkin = BarSkins.size();
	const size_t bufsiz = 0
		+ sizeof(pkgheader)
		+ nBarSkin * sizeof(i8desk::db::tBarSkin)
		+ 64;
	LPSTR pbuf = pNetLayer->GetIMemPool()->Alloc(bufsiz);
	CPkgHelper packet(pbuf, bufsiz);
	packet.GetHeader()->Cmd = CMD_REP_BARSKINS;
	try
	{
		packet << 1L;
		packet.Push(nBarSkin); 
		for (int i = 0; i < nBarSkin; i++) {
			packet.Push(BarSkins[i]);
		}
	}
	catch (...)
	{
		packet<<0L<<_T("得到BarSkin命令失败！");
	}
	
	packet.SetLength(packet.GetLength());

	pNetLayer->SendPackage(sck, (char*)packet.GetBuffer(), packet.GetLength());
}

void CRTDataSvr::Cmd_Update_Client_Index(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
{
	//TEST_THIS_SECTION(__FUNCTION__);

	const size_t bufsiz = sizeof(pkgheader) + 64;
	LPSTR pbuf = pNetLayer->GetIMemPool()->Alloc(bufsiz);
	CPkgHelper packet(pbuf, bufsiz);
	packet.GetHeader()->Cmd = CMD_UPT_CLIENT_INDEX;

	DWORD oldversion = m_pClientSoftMonitor->GetVersion();
	if (m_pClientSoftMonitor->MakeIdx()) {
		packet.Push(1); 
		packet.Push(oldversion); 
		m_pClientSoftMonitor->UpdateVersion(::_time32(0)); //强制更新一下版本
		packet.Push(m_pClientSoftMonitor->GetVersion()); 
	} 
	else {
		packet.Push(0); 
		const TCHAR *err = _T("更新索引失败");
		packet.PushString(err, _tcslen (err));
	}

	packet.SetLength(packet.GetLength());
	pNetLayer->SendPackage(sck, (char*)packet.GetBuffer(), packet.GetLength());
}

void CRTDataSvr::Cmd_Con_GetInfo_From_GameUpdateSvr(INetLayer*pNetLayer, SOCKET sck, LPCSTR /*lpPackage*/,CPkgHelper &inpackage,pkgheader &header)
{
	DWORD dwCmd;
	DWORD dwParam1;
	try {
		inpackage >> dwCmd;
		inpackage.Pop(dwParam1);
	} catch (...) {
		NAK_RETURN(_T("错误的命令格式"));
	} 

	ISvrPlug *pUpdateServer = m_pPlugMgr->FindPlug(PLUG_UPTSVR_NAME);
	
	if (!pUpdateServer) 
		NAK_RETURN(_T("没有找到游戏更新插件"));

	std::vector<db::tViewGame> ViewGames;
	std::vector<db::tViewClient> ViewClients;

	size_t index = 0;
	size_t ncount = 0; 
	switch( dwCmd )
	{
	case CMD_UPT_SM_IPHAVEGAME:  // 查看客户端游戏
		{
			char *tmp = reinterpret_cast<char *>(pUpdateServer->SendMessage(dwCmd, dwParam1, 0));
			
			if(tmp == NULL) return;

			int count = 0;
			memcpy(&count,tmp,sizeof(int));

			for (int i = 0; i < count; i++)
			{
				db::tViewGame ViewGame ;
				memcpy(&ViewGame,(tmp + sizeof(int))+( i * sizeof(db::tViewGame)),sizeof(db::tViewGame));

				db::tViewGame d;

				// GID 为0则结束
				if( ViewGame.GID == 0 )
					break;

				d.GID = ViewGame.GID;
				d.UpdateTime = ViewGame.UpdateTime;

				db::tGame game;
				if( m_pTableMgr->GetGameTable()->GetData(d.GID, &game) != 0)
					 continue;

				utility::Strcpy(d.GameName, game.Name);
				ViewGames.push_back(d);
			}
			CoTaskMemFree(tmp);
			ncount = ViewGames.size();
			break;
		}
	case CMD_UPT_SM_GAMEINIP:  // 查看游戏所在的客户端
		{
			char *tmpclient = reinterpret_cast<char *>(pUpdateServer->SendMessage(dwCmd, dwParam1, 0));
			if(tmpclient == NULL) return;
			int count = 0;
			memcpy(&count,tmpclient,sizeof(int));

			for (int i = 0; i < count; i++)
			{
				db::tViewClient ViewClient ;
				memcpy(&ViewClient,(tmpclient + sizeof(int))+( i * sizeof(db::tViewClient)),sizeof(db::tViewClient));

				db::tViewClient d;
				// GID 为0则结束
				if( ViewClient.Ip == 0 )
					break;

				d.Ip = ViewClient.Ip;
				d.UpdateTime = ViewClient.UpdateTime;

				struct Filter : public IClientFilter 
				{
					Filter(ulong IP) : m_IP(IP) {}
					bool bypass(const db::tClient *const d) 
					{
						return d->IP != m_IP ;
					}
				private:
					ulong m_IP;
				} filter(ViewClient.Ip);

				IClientRecordset *pRecordset = 0;

				m_pTableMgr->GetClientTable()->Select(&pRecordset,&filter,0);
				TCHAR ClientName[MAX_NAME_LEN] = {0};

				if( pRecordset->GetCount() == 0 )
					continue;

				for (uint32 i=0;i<pRecordset->GetCount();i++)
				{
					db::tClient *client = pRecordset->GetData(i);
					utility::Strcpy(d.ClientName,client->Name);
				}

				ViewClients.push_back(d);

				pRecordset->Release();
			}
			
			CoTaskMemFree(tmpclient);
			ncount = ViewClients.size();
			break;
		}
	}


	LPSTR pbuf = pNetLayer->GetIMemPool()->Alloc(sizeof(db::tViewClient)*(ncount + 1));
	CPkgHelper out(pbuf, sizeof(db::tViewClient)*(ncount + 1));
	out.GetHeader()->Cmd = header.Cmd;

	out << 1L << ncount ;
	switch( dwCmd )
	{
	case 3:
		for( size_t n = 0; n < ncount; n++ )
			out.Push(ViewGames[n]) ;
		break;
	case 4:
		for( size_t n = 0; n < ncount; n++ )
			out.Push(ViewClients[n]);
		break;
	}

	pNetLayer->SendPackage(sck, (char*)out.GetBuffer(), out.GetLength());
}


void CRTDataSvr::Cmd_Get_Server_Config(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
{
	TCHAR clientName[MAX_PATH] = {0};
	try {
		inpackage >> clientName;
	} catch (...) {
		NAK_RETURN(_T("错误的命令格式"));
	} 
	
	static std::vector<std::pair<ulong, std::set<ulong>>> svrsIP;
	svrsIP.clear();

	balance_->GetServersIPs(clientName, svrsIP);
	ulong cliIP = win32::system::GetDesIP(sck);
	balance_->UpdateConnect(UpdateIP, svrsIP[0].first, cliIP);

	size_t bufLen = 0;
	for(size_t i = 0; i != svrsIP.size(); ++i)
		bufLen += sizeof(ulong) + svrsIP[i].second.size() * sizeof(ulong);

	char *pBuf = pNetLayer->GetIMemPool()->Alloc(bufLen + 128);
	CPkgHelper out(pBuf, bufLen, header.Cmd, OBJECT_UNKOWN);
	out << 1L << svrsIP.size();
	for(size_t i = 0; i != svrsIP.size(); ++i)
	{
		out << svrsIP[i].first				// 内网更新IP
			<< svrsIP[i].second.size();		// 虚拟盘IP个数
		for(std::set<ulong>::const_iterator iter = svrsIP[i].second.begin(); iter != svrsIP[i].second.end(); ++iter)
		{
			out << *iter;					// 虚拟盘IP
		}
	}

	pNetLayer->SendPackage(sck, (LPSTR)out.GetBuffer(), out.GetLength());
}

void CRTDataSvr::Cmd_Get_VDisk_Config(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
{
	//TEST_THIS_SECTION(__FUNCTION__);

	const size_t bufsiz = 1024*1024/2;
	LPSTR lpSendPackage = pNetLayer->GetIMemPool()->Alloc(bufsiz);
	CPkgHelper outpackage(lpSendPackage, bufsiz,header.Cmd,OBJECT_UNKOWN);

	struct FGetVDisk : public std::unary_function<VDiskCache_Ptr, void> {
		FGetVDisk(CPkgHelper& pkg, DWORD *pCount) 
			: outpackage(pkg)
			, m_pCount(pCount) 
		{
		}
		void operator()(VDiskCache_Ptr vdisk)
		{
			i8desk::db::tVDisk vd;
			if (vdisk->GetVDisk(vd)) {
				outpackage << vd.VID
					<< vd.SoucIP
					<< vd.Port
					<< vd.SvrDrv
					<< vd.CliDrv
					<< vd.Type
					<< vd.LoadType
					<< vd.SvrMode
					<< vd.Size
					<< vd.SsdDrv;

				(*m_pCount)++;
			}
		}

		CPkgHelper& outpackage;
		DWORD *m_pCount;
	};

	outpackage << 1L << 0L;
	DWORD *pCount = (DWORD *)(lpSendPackage + sizeof(pkgheader) + 4);

	m_pDataCache->GetVDisks().foreach_value(FGetVDisk(outpackage, pCount));

	header.Length = outpackage.GetLength();
	outpackage.pushHeader(header);
	pNetLayer->SendPackage(sck, lpSendPackage, outpackage.GetLength());
}

void CRTDataSvr::Cmd_Reload_Cache_File(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
{
	TCHAR pathname[MAX_PATH]={0};
	try {
		inpackage >> pathname;
	} catch (...) {
		NAK_RETURN(_T("错误的命令格式"));
	} 

	if (!m_pFileCache->Reload(pathname))
	{
		TCHAR err[MAX_PATH*2]={0};
		_stprintf(err, _T("服务器重新加载缓存文件失败：%s\n"), pathname);
		NAK_RETURN(err);
	}

	Log(LM_DEBUG, _T("服务器重新加载缓存文件成功：%s\n"), pathname);
	ACK();
}

void CRTDataSvr::Cmd_GetClientVersion(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
{
	const size_t bufsiz = 1024;
	LPSTR lpSendPackage = pNetLayer->GetIMemPool()->Alloc(bufsiz);
	CPkgHelper out(lpSendPackage, bufsiz,header.Cmd,OBJECT_UNKOWN);

	DWORD ClientSvrVer = 0;

	out << 1L << m_pTableMgr->GetOption(OPT_M_AUTOUPT,ClientSvrVer) << m_pClientSoftMonitor->GetVersion();

	pNetLayer->SendPackage(sck, lpSendPackage, out.GetLength());

}

void CRTDataSvr::Cmd_Con_PlugToolIcon(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
{
	DWORD gid = 0;
	try 
	{
		inpackage >> gid;
	} 
	catch (...) 
	{
		NAK_RETURN(_T("错误的命令格式"));
	} 

	ISvrPlug *pPlugtoolPlug = m_pPlugMgr->FindPlug(PLUG_PLUGTOOL_NAME);
	if (!pPlugtoolPlug) 
		NAK_RETURN(_T("没有找到插件中心组件"));

	char* pData = reinterpret_cast<char *>(pPlugtoolPlug->SendMessage(Icon,gid,0));

	if ( pData == 0)
	{
		TCHAR ErrBuf[255] = {0};
		_stprintf(ErrBuf, _T("没有找到gid = %d 的游戏图标"), gid);
		NAK_RETURN(ErrBuf);
	}
	size_t size = 0;
	memcpy(&size, pData, sizeof(size_t));
	assert(size != 0 || size <= 1024*20);

	LPSTR lpSendPackage = pNetLayer->GetIMemPool()->Alloc(size+1024);
	CPkgHelper out(lpSendPackage, size,header.Cmd,OBJECT_UNKOWN);
	
	out.Push(1);
	out.Push(size);
	out.PushPointer(pData + sizeof(size_t), size);
	
	pNetLayer->SendPackage(sck, lpSendPackage, out.GetLength());

	::CoTaskMemFree(pData);
}

void CRTDataSvr::Cmd_Con_PlugToolOperate(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
{
	DWORD cmd,gid = 0;
	try 
	{
		inpackage >> gid >> cmd;
	} 
	catch (...) 
	{
		NAK_RETURN(_T("错误的命令格式"));
	} 

	ISvrPlug *pPlugtoolPlug = m_pPlugMgr->FindPlug(PLUG_PLUGTOOL_NAME);
	if (!pPlugtoolPlug) 
		NAK_RETURN(_T("没有找到插件中心组件"));

	DWORD data = pPlugtoolPlug->SendMessage(cmd,gid,0);

	const size_t bufsiz = 1024;
	LPSTR lpSendPackage = pNetLayer->GetIMemPool()->Alloc(bufsiz);
	CPkgHelper out(lpSendPackage, bufsiz,header.Cmd,OBJECT_UNKOWN);

	out << 1L << data;

	pNetLayer->SendPackage(sck, lpSendPackage, out.GetLength());
}


void CRTDataSvr::Cmd_Con_MonitorDeleteFile(INetLayer *pNetLayer, SOCKET sck,  LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
{
	ISvrPlug *pGameMgr = m_pPlugMgr->FindPlug(PLUG_GAMEMGR_NAME);
	if (!pGameMgr) 
		NAK_RETURN(_T("没有找到三层下载组件"));

	DWORD data = pGameMgr->SendMessage(DELETE_FILE,0,0);

	ACK();
	
}

void CRTDataSvr::Cmd_Con_MonitorStatus(INetLayer *pNetLayer, SOCKET sck,  LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
{

	const size_t bufsiz = 1024*2;
	LPSTR lpSendPackage = pNetLayer->GetIMemPool()->Alloc(bufsiz);
	CPkgHelper out(lpSendPackage, bufsiz,header.Cmd,OBJECT_UNKOWN);
	out << 1L << 1L;
	//　服务器统计
	StatisticServer_(out);

	//　三层更新统计
	StatisticTask_(out);
		
	//　内网更新统计
	StatisticUpdateGame_(out);
	
	//  同步任务统计
	StatisticSyncGame_(out);


	pNetLayer->SendPackage(sck, lpSendPackage, out.GetLength());
}

void CRTDataSvr::Cmd_Con_StatisticStatus(INetLayer *pNetLayer, SOCKET sck,  LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
{
	const size_t bufsiz = 1024;
	LPSTR lpSendPackage = pNetLayer->GetIMemPool()->Alloc(bufsiz);
	CPkgHelper out(lpSendPackage, bufsiz,header.Cmd,OBJECT_UNKOWN);
	out << 1L << 1L;

	//　本地资源与中心资源统计
	StatisticGame_(out);
	//　客户机在线统计
	StatisticClient_(out);
	//  上报插件验证状态
	StatisticLogRptStatus_(out);

	pNetLayer->SendPackage(sck, lpSendPackage, out.GetLength());

}


void CRTDataSvr::StatisticGame_( CPkgHelper &out )
{
	DWORD IdcCount = 0, LocalCount = 0; 
	unsigned long long LocalSize = 0 , IdcSize = 0;
	IGameRecordset *pRecordset = 0;
	m_pTableMgr->GetGameTable()->Select(&pRecordset,0,0);

	for (uint32 i = 0; i < pRecordset->GetCount(); i++)
	{
		db::tGame *game = pRecordset->GetData(i);
		//　中心资源个数与大小统计
		if( (game->GID) > MIN_IDC_GID ) 
		{
			IdcCount++;
			IdcSize += game->Size ;
		}
		//　本地资源个数与大小统计
		if( game->Status )
		{
			LocalCount++;
			LocalSize += game->Size ; 
		}
	}

	out << IdcCount << (IdcSize * 1024)  << LocalCount << (LocalSize * 1024);

	pRecordset->Release();
}

void CRTDataSvr::StatisticClient_( CPkgHelper &out )
{
	uint32  ClientOnline = 0,ClientSum = 0;

	IClientRecordset 	*pRecordset = 0;
	m_pTableMgr->GetClientTable()->Select(&pRecordset,0,0);

	ClientSum = pRecordset->GetCount();

	for ( uint32 i = 0; i < ClientSum; i++ )
	{
		db::tClient *client = pRecordset->GetData(i);
		if( client->Online ) ClientOnline++;
	}
	pRecordset->Release();

	out << ClientOnline << ClientSum;

}

void CRTDataSvr::StatisticUpdateGame_( CPkgHelper &out )
{
	DWORD UpdateClientSum = 0;
	DWORD UpdateSpeedSum = 0;
	EnsureStatus(FLAG_UPDATEGAMESTATUS_BIT);
	IUpdateGameStatusRecordset 	*pstatusRecordset = 0;
	m_pTableMgr->GetUpdateGameStatusTable()->Select(&pstatusRecordset,0,0);

	for (uint32 i = 0; i < pstatusRecordset->GetCount(); i++)
	{
		db::tUpdateGameStatus *updategamestatus = pstatusRecordset->GetData(i);
		// 内网更新统计
		if( updategamestatus->Type == 2 ) 
		{
			UpdateClientSum++;
			UpdateSpeedSum += updategamestatus->TransferRate;
		}

	}

	out << UpdateClientSum << UpdateSpeedSum ;
	pstatusRecordset->Release();

}

void CRTDataSvr::StatisticSyncGame_( CPkgHelper &out )
{
	DWORD SyncTaskSum = 0;
	DWORD SyncSpeedSum = 0;
	std::set<stdex::tString> TaskName;

	ISyncTaskRecordset *psynctaskRecordset = 0;

	m_pTableMgr->GetSyncTaskTable()->Select(&psynctaskRecordset,0,0);
	SyncTaskSum = psynctaskRecordset->GetCount();
	psynctaskRecordset->Release();


	ISyncTaskStatusRecordset 	*pstatusRecordset = 0;
	ISyncTaskStatusFilter *pFilter = 0;
	m_pTableMgr->GetSyncTaskStatusTable()->Select(&pstatusRecordset,pFilter,0);

	for (uint32 i = 0; i < pstatusRecordset->GetCount(); i++)
	{
		db::tSyncTaskStatus *synctaskstatus = pstatusRecordset->GetData(i);


		if( synctaskstatus->UpdateState == i8desk::GameRunning ) 
		{
			SyncSpeedSum += synctaskstatus->TransferRate;
		}
	}

	out << SyncTaskSum << SyncSpeedSum;
	pstatusRecordset->Release();

}

void CRTDataSvr::StatisticTask_( CPkgHelper &out )
{
	DWORD DownloadStatus = 0, DownloadingCount = 0, SelfQueueCount = 0, CpuUasge = 0;
	ushort ConnectSum = 0;
	ULONGLONG  TempDirSize = 0;
	long MaxSpeed = 0 ;
	ulong DownloadingSpeedSum = 0;

	EnsureStatus(FLAG_TASKSTATUS_BIT);

	ITaskStatusRecordset *pstatusRecordset = 0;
	m_pTableMgr->GetTaskStatusTable()->Select(&pstatusRecordset,0,0);

	for (uint32 i = 0; i < pstatusRecordset->GetCount(); i++)
	{
		db::tTaskStatus *taskstatus = pstatusRecordset->GetData(i);
		if ( taskstatus->State == tsDownloading )
		{
			DownloadingCount++;
			DownloadingSpeedSum += taskstatus->TransferRate;
			ConnectSum += taskstatus->Connect;
		}

		if ( taskstatus->State == tsSelfQueue )
			SelfQueueCount++;
	}

	pstatusRecordset->Release();

	// 正在下载个数不为０,则表示"正在下载",否则"正在等待"
	if( DownloadingCount != 0 ) 
		DownloadStatus = 1;

	// 统计三层临时目录大小
	ISvrPlug *pGamePlug = m_pPlugMgr->FindPlug(PLUG_GAMEMGR_NAME);
	if( pGamePlug == 0 )
		TempDirSize = 0;
	else
	{
		DWORD *ptr = (DWORD *)(pGamePlug->SendMessage(MSC_TMP_SIZE, 0, 0));
		TempDirSize = *(ulonglong *)(ptr);
	}

	
	//　三层下载限速
	MaxSpeed = m_pTableMgr->GetOption(OPT_D_SPEED,0);

	//　CPU 利用率
	stdex::tString FileName =  _T("DNAService.exe");
	int Pid = GetPIDByName(FileName.c_str());
	CpuUasge = i8desk_GetProcessCpuUsage(Pid);

	stdex::tOstringstream os;
	os << CpuUasge <<"  " << TempDirSize;
	//_stprintf_s(str, TEXT("CpuUasge = %d\r\n TempDirSize = %I64"), CpuUasge,TempDirSize);
	OutputDebugString(os.str().c_str());


	out << DownloadStatus << DownloadingCount << DownloadingSpeedSum
		<< SelfQueueCount << ConnectSum << TempDirSize << MaxSpeed << CpuUasge;
}

void CRTDataSvr::StatisticServer_( CPkgHelper &out )
{
	DWORD HostCount = 0;

	IServerRecordset *pserverRecordset = 0;
	
	m_pTableMgr->GetServerTable()->Select(&pserverRecordset,0,0);
	HostCount = pserverRecordset->GetCount();
	out << HostCount;
	for (uint32 i = 0; i < HostCount; i++)
	{
		// 取得服务器ID
		db::tServer *servers = pserverRecordset->GetData(i);
		out << servers->SvrName << servers->SvrType;
		DWORD  Status = 0;
		struct ServerFilter : public IServerStatusFilter 
		{
			ServerFilter(const TCHAR* svrID) : svrID_(svrID) {}
			bool bypass(const db::tServerStatus *const d) 
			{
				return utility::Strcmp(d->SvrID, svrID_) != 0;
			}
		private:
			const TCHAR* svrID_;
		} serverfilter( servers->SvrID);

		IServerStatusRecordset *pserverstatusRecordset = 0;
		m_pTableMgr->GetServerStatusTable()->Select(&pserverstatusRecordset,&serverfilter,0);
		for ( uint32 k = 0; k < pserverstatusRecordset->GetCount(); k++ )
		{
			db::tServerStatus *serverstatus = pserverstatusRecordset->GetData(k);
			Status = serverstatus->I8VDiskSvr;
		}
		pserverstatusRecordset->Release();

		// 通过SvrID 找到 VID 过滤VDiskClient表 得到总速度和已发送数据,以及链接数
		uint64  SendDataSum = 0; ulong SpeedSum = 0;
		DWORD ConnectSum = 0;

		IVDiskRecordset *pvdiskRecordset = 0;

		struct Filter2 : public IVDiskFilter 
		{
			Filter2(TCHAR* SvrID) : m_SvrID(SvrID) {}
			bool bypass(const db::tVDisk *const d) 
			{
				return utility::Stricmp(m_SvrID, d->SvrID) != 0 ;
			}
		private:
			TCHAR* m_SvrID;
		} filter2( servers->SvrID);

		m_pTableMgr->GetVDiskTable()->Select(&pvdiskRecordset,&filter2,0);

		for ( uint32 k = 0; k < pvdiskRecordset->GetCount(); k++ )
		{
			db::tVDisk *vdisk = pvdiskRecordset->GetData(k);

			IVDiskClientRecordset *pvdiskclientRecordset = 0;

			struct Filter1 : public IVDiskClientFilter 
			{
				Filter1(TCHAR* VID) : m_VID(VID) {}
				bool bypass(const db::tVDiskClient *const d) 
				{
					return utility::Stricmp(m_VID, d->VID) != 0 ;
				}
			private:
				TCHAR* m_VID;
			} filter1( vdisk->VID);

			m_pTableMgr->GetVDiskClientTable()->Select(&pvdiskclientRecordset,&filter1,0);

			for ( uint32 k = 0; k < pvdiskclientRecordset->GetCount(); k++ )
			{
				db::tVDiskClient *vdiskclient = pvdiskclientRecordset->GetData(k);
				SendDataSum += vdiskclient->ReadCount;
				SpeedSum	+= vdiskclient->ReadSpeed;
				//Status		= 1;
			}
			ConnectSum = pvdiskclientRecordset->GetCount();

			pvdiskclientRecordset->Release();
		}

		std::pair<size_t, size_t> loadCnt = balance_->GetLoadCount(servers->SvrID);
		out << Status << loadCnt.first << loadCnt.second << SpeedSum << SendDataSum;
		pvdiskRecordset->Release();
	
	}

	pserverRecordset->Release();
}

void CRTDataSvr::StatisticLogRptStatus_( CPkgHelper &out )
{
	DWORD ret  = 0;
	ISvrPlug *pLogRpt = m_pPlugMgr->FindPlug(PLUG_LOGRPT_NAME);
	if( pLogRpt == 0 )
		return;
	ret = pLogRpt->SendMessage(LOG_RPT_GET_LOGIN_STATE,0,0);

	out << ret;
}

void CRTDataSvr::Cmd_Business_InfoEx(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
{
	db::tClient client;
	DWORD dwCount = 0;
	std::list<DWORD> gids;
	DWORD cmdtype;
	stdex::tString Prefix;
	ULONGLONG filesizeSum = 0;

	size_t bufsize =  1024 * 10 * 10;
	LPSTR pbuf = pNetLayer->GetIMemPool()->Alloc(bufsize);
	CPkgHelper out(pbuf, bufsize, header.Cmd,OBJECT_UNKOWN);

	try 
	{
		inpackage >> client.Name >> cmdtype >> dwCount ;
	} 
	catch (...) 
	{
		NAK_RETURN(_T("错误的命令格式"));
	} 

	if (dwCount > MAX_COUNT_GAME_PER_PACKEG) 
		dwCount = MAX_COUNT_GAME_PER_PACKEG;

	ISvrPlug *business = m_pPlugMgr->FindPlug(PLUG_BUSINESS_NAME);
	if( business == 0 )
		NAK_RETURN(_T("没有找到也无插件"));

	if ( cmdtype == E_EXINFO_GAME )
	{
		out << 1 << cmdtype << 0;
		DWORD* pdwCount = (DWORD*)(pbuf + sizeof(pkgheader) + 8);
		
		for ( DWORD i = 0; i < dwCount; ++i )
		{
			// 分级菜单命令gid = 0, 其他命令gid > MIN_IDC_GID
			DWORD gid = 0;
			inpackage >> gid;
			if( gid < MIN_IDC_GID )
				continue;

			char *buf = (char *)business->SendMessage(E_BSC_GET_EX_INFO, GameXML, gid);
			if( buf == 0 )
				continue;

			DWORD size = 0;
			memcpy(&size, buf, sizeof(DWORD));

			out << gid << size;
			out.PushPointer(buf + sizeof(DWORD), size);
			(*pdwCount)++;

			::CoTaskMemFree(buf);
		}
	}
	else if ( cmdtype == E_EXINFO_MENU )
	{
		out << 1 << cmdtype << 1;
		ULONGLONG filesize = 0;
		DWORD gid = 0;
		
		char *buf = (char *)business->SendMessage(E_BSC_GET_EX_INFO, ClientXML, gid);
		if( buf != 0 )
		{
			DWORD size = 0;
			memcpy(&size, buf, sizeof(DWORD));

			out << gid << size;
			out.PushPointer(buf + sizeof(DWORD), size);

			::CoTaskMemFree(buf);
		}
		else
		{
			NAK_RETURN(_T("没有找到菜单分级信息"));
		}
	}

	header.Length = out.GetLength();
	out.pushHeader(header);
	pNetLayer->SendPackage(sck,(LPSTR)out.GetBuffer(), out.GetLength());
}

struct CheckT
{
	template<typename FileFindT>
	bool operator()(const FileFindT &filefind)
	{
		stdex::tString lpExt = filefind.GetFileName();
		return utility::Stricmp(_T("ico"), lpExt.substr(lpExt.length() - 3)) == 0 ;
	}
};

struct ScanFileT
{
	std::vector<stdex::tString> &paths_;
	ScanFileT(std::vector<stdex::tString> &paths)
		: paths_(paths)
	{}

	void operator()(const stdex::tString &path)
	{
		paths_.push_back(path);
	}
};

typedef win32::file::FileDepthSearch<CheckT, ScanFileT> ScanFile;

void CRTDataSvr::Cmd_Con_GetAll_ClassIcon(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
{
	DWORD dwCmdType = 0 ,FileSize = 0;
	stdex::tString path;
	std::vector<stdex::tString> paths;
	path = utility::GetAppPath() + _T("Data/ClassIcon");


	CheckT checkfile; 
	ScanFileT searchItem(paths);
	ScanFile scanDir(checkfile, searchItem);
	scanDir.Run(path);

	assert( paths.size() != 0 );

	size_t bufsize =   1024 * 64;
	LPSTR pbuf = pNetLayer->GetIMemPool()->Alloc(bufsize);
	CPkgHelper out(pbuf, bufsize, header.Cmd, OBJECT_CONSOLE);


	out << 1 << paths.size() ;
	for (std::vector<stdex::tString>::iterator iter = paths.begin(); iter != paths.end(); ++iter )
	{
		ULONGLONG filesize = 0;

		utility::CAutoFile handel = CreateFile((*iter).c_str(),           // open MYFILE.TXT 
			GENERIC_READ,              // open for reading 
			FILE_SHARE_READ,           // share for reading 
			NULL,                      // no security 
			OPEN_EXISTING,             // existing file only i
			FILE_ATTRIBUTE_NORMAL,     // normal file 
			NULL);                     // no attr. template 


		assert( handel != INVALID_HANDLE_VALUE );
		if( !utility::GetFileSize((*iter).c_str(),filesize) )
			continue;

		BYTE *data = I8MP.malloc<BYTE>((DWORD)filesize);

		if( !utility::ReadFileContent(handel,data,(DWORD)filesize))
		{
			continue;
		}
		else
		{
			out << (*iter) << filesize << data ;
		}

	}

	pNetLayer->SendPackage(sck, (LPSTR)out.GetBuffer(), out.GetLength());
}


void CRTDataSvr::Cmd_Con_UserInfoModify(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
{
	ISvrPlug *plug  = m_pPlugMgr->FindPlug(PLUG_LOGRPT_NAME);
	if( plug == NULL ) 
		return;

	if( plug->SendMessage( LOG_RPT_SAVE_DATA, 0, 0 ) )
		ACK();
	else
		NAK_RETURN(_T("保存网吧信息失败!"));
}

void CRTDataSvr::Cmd_Con_GetAll_GameIcon(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
{

	size_t nCount = 0;
	CustomFileInfoPtr pIcons ;
	size_t filesize = 0;
	filesize =  m_pIconTable->GetAllFileData(nCount,pIcons);

	size_t bufsize =  nCount * sizeof(CustomFileInfo) + filesize + 128 ;
	LPSTR pbuf = pNetLayer->GetIMemPool()->Alloc(bufsize);
	CPkgHelper out(pbuf, bufsize, header.Cmd, OBJECT_CONSOLE);


	if ( filesize != 0 )
	{
		out << 1L << nCount;
		CustomFileInfo* custominfo = pIcons.get();

		for( size_t i = 0; i!= nCount; ++i )
		{
			out << custominfo->gid << custominfo->size  ;
			out.PushPointer((const char*)custominfo->data,custominfo->size);
			custominfo += 1;
		}
	}
	else
	{
	//	assert(0);
		static stdex::tString text = _T("无法获取所有图标文件");
		NAK_RETURN(text.c_str());
	}


	pNetLayer->SendPackage(sck, (LPSTR)out.GetBuffer(), out.GetLength());
	
}


void CRTDataSvr::Cmd_GameMgr_TaskOperation(INetLayer *pNetLayer, SOCKET sck,  LPCSTR lpPackage, CPkgHelper &inpackage,pkgheader &header)
{
	ISvrPlug *GamePlug  = m_pPlugMgr->FindPlug(PLUG_GAMEMGR_NAME) ;
	if( GamePlug == NULL ) 
		return;

	if( GamePlug->SendMessage( CMD_TASK_OPERATION, (DWORD)inpackage.GetBuffer(), inpackage.GetLength() ) )
		ACK();
	else
		NAK_RETURN(_T("发送 CMD_TASK_OPERATION 命令失败"));
}


void CRTDataSvr::Cmd_GameMgr_NotifyRefreshPL(INetLayer *pNetLayer, SOCKET sck,  LPCSTR lpPackage, CPkgHelper &inpackage,pkgheader &header)
{
	ISvrPlug *GamePlug  = m_pPlugMgr->FindPlug(PLUG_GAMEMGR_NAME) ;
	if( GamePlug == NULL ) 
		return;

	if( GamePlug->SendMessage( CMD_CON_NOTIFY_REFRESH_PL,  (DWORD)inpackage.GetBuffer(), inpackage.GetLength() ) )
		ACK();
	else
		NAK_RETURN(_T(" 发送 CMD_CON_NOTIFY_REFRESH_PL 命令失败"));

}

void CRTDataSvr::Cmd_GameMgr_NotifyServerMedical(INetLayer *pNetLayer, SOCKET sck,  LPCSTR lpPackage, CPkgHelper &inpackage,pkgheader &header)
{
	ISvrPlug *GamePlug  = m_pPlugMgr->FindPlug(PLUG_GAMEMGR_NAME) ;
	if( GamePlug == NULL ) 
		return;

	if( GamePlug->SendMessage( CMD_NOTIFY_SERVER_MEDICAL,  (DWORD)inpackage.GetBuffer(), inpackage.GetLength() ) )
		ACK();
	else
		NAK_RETURN(_T(" 发送 CMD_NOTIFY_SERVER_MEDICAL 命令失败"));

}

void CRTDataSvr::Cmd_GameMgr_NotifyDeleteFile(INetLayer *pNetLayer, SOCKET sck,  LPCSTR lpPackage, CPkgHelper &inpackage,pkgheader &header)
{
	ISvrPlug *GamePlug  = m_pPlugMgr->FindPlug(PLUG_GAMEMGR_NAME) ;
	if( GamePlug == NULL ) 
		return;

	if( GamePlug->SendMessage( CMD_CON_NOTIFY_DELETEFILE,  (DWORD)inpackage.GetBuffer(), inpackage.GetLength() ) )
		ACK();
	else
		NAK_RETURN(_T(" 发送 CMD_CON_NOTIFY_DELETEFILE 命令失败"));

}


void CRTDataSvr::SendConsoleNotify(i8desk::TaskNotifyType taskType, i8desk::TaskNotify notify, long gid, size_t err, DWORD &ip)
{
	if( ip == 0 )
		return;

	SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s == INVALID_SOCKET)
	{
		Log(LM_WARNING, _T("UDP套接字打开失败!\n"));
		return;
	}

	try
	{
		sockaddr_in address		= {0};
		int len					= sizeof(address);
		address.sin_family		= AF_INET;
		address.sin_addr.s_addr	= ip;
		address.sin_port		= ::htons(CONTROL_UDP_PORT);

		char buf[512] = {0};
		i8desk::CPkgHelper out(buf, 512, taskType == i8desk::GamePlug ? CMD_TASK_STATE_CHANGED : CMD_PLUG_TASK_COMPLATE, i8desk::OBJECT_UNKOWN);
		out << 1L << (DWORD)notify << gid << err;

		sendto(s, out.GetBuffer(), out.GetLength(), 0, (sockaddr*)&address, len );
	}
	catch(std::exception &e)
	{
		e.what();
		Log(i8desk::LM_ERROR, _T("发送UDP数据失败[TO:IP=%d, err=%d]!"), ip, ::GetLastError());
	}

	closesocket(s);
}


void CRTDataSvr::Cmd_NotifyCon_HardwareChange(const db::tClient &client, uint64 mask)
{
	DWORD ip = GetConsoleIP();
	if(ip == 0)
		return;

	SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s == INVALID_SOCKET)
	{
		Log(LM_WARNING, _T("UDP套接字打开失败!\n"));
		return;
	}

	int iSocketBufSize = 0x10000;
	if(setsockopt(s, SOL_SOCKET, SO_SNDBUF,
		(char *)&iSocketBufSize, sizeof(iSocketBufSize)) == SOCKET_ERROR)
	{
		Log(LM_WARNING, _T("UDP套接字设置选项失败!\n"));

		closesocket(s);
		return;
	}

	try
	{
		sockaddr_in address		= {0};
		int len					= sizeof(address);
		address.sin_family		= AF_INET;
		address.sin_addr.s_addr	= ip;
		address.sin_port		= ::htons(CONTROL_UDP_PORT);
		
		DWORD size =  sizeof(db::tClient) + sizeof(pkgheader) + 256;
		char buf[1024 * 10] = {0};
		i8desk::CPkgHelper out(buf, size, CMD_CLIENT_HARDWARECHANGE, i8desk::OBJECT_UNKOWN);
		out << 1L <<  client << mask ;

		sendto(s, out.GetBuffer(), out.GetLength(), 0, (sockaddr*)&address, len );
	}
	catch(std::exception &e)
	{
		e.what();
		Log(i8desk::LM_ERROR, _T("发送UDP数据失败[TO:IP=%d, err=%d]!"), ip, ::GetLastError());
	}

	closesocket(s);
}

void CRTDataSvr::Cmd_Con_SaveHardWare(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
{
	uint64 mask = ~MASK_TCLIENT_ONLINE;
	for( std::vector<db::tClient>::const_iterator iter = ClientHDChanged_.begin(); iter != ClientHDChanged_.end(); ++iter )
		m_pTableMgr->GetClientTable()->Ensure(&(*iter), mask);

	ACK();
}

void CRTDataSvr::Cmd_Con_NotifySmartClean(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
{
	size_t cnt = 0;
	inpackage >> cnt;

	std::vector<DWORD> gids(cnt);
	inpackage.PopPointer(&gids[0], cnt);

	ISvrPlug *gamePlug  = m_pPlugMgr->FindPlug(PLUG_GAMEMGR_NAME);
	if( gamePlug == 0 ) 
		return;

	gamePlug->SendMessage(SMART_CLEAN, cnt, (DWORD)&gids[0]);
	ACK();
}

void CRTDataSvr::Cmd_Con_CheckSpace(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
{
	stdex::tString path;
	ulonglong totalSize = 0;

	inpackage >> path >> totalSize;
	path.resize(3);

	CPkgHelper out(pNetLayer->GetIMemPool()->Alloc(1024), 1024, header.Cmd, OBJECT_UNKOWN);
	out << 1L;

	stdex::tString info;
	if( !win32::file::CheckDiskSpace(path, totalSize, info) )
		out << 0;
	else
		out << 1;

	pNetLayer->SendPackage(sck, (LPSTR)out.GetBuffer(), out.GetLength());
}

void CRTDataSvr::Cmd_Businisss_GetGameExVer(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
{
	ISvrPlug *BusinissPlug  = m_pPlugMgr->FindPlug(PLUG_BUSINESS_NAME) ;
	if( BusinissPlug == NULL ) 
	{
		NAK_RETURN(_T("Businiss 插件启失败"));
	}
		

	GameExInfoVersionArray_st *tmp = reinterpret_cast<GameExInfoVersionArray_st *>(BusinissPlug->SendMessage( E_BSC_GET_GAMEEX_VERSION,  0, 0 ));

	size_t bufsize = tmp->dwCount * sizeof(GameExInfoVersion_st) + sizeof(DWORD) + 256;
	LPSTR pbuf = pNetLayer->GetIMemPool()->Alloc(bufsize);

	CPkgHelper out(pbuf, bufsize, header.Cmd, OBJECT_CONSOLE);
	out << 1L;
	out.PushPointer((char *)tmp, tmp->dwCount * sizeof(GameExInfoVersion_st) + sizeof(DWORD));
	pNetLayer->SendPackage(sck, (LPSTR)out.GetBuffer(), out.GetLength());
	::CoTaskMemFree(tmp);
}


	void CRTDataSvr::Cmd_UpdateSvr_Operate(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
	{
		ISvrPlug *updateSvr  = m_pPlugMgr->FindPlug(PLUG_UPTSVR_NAME);
		if( updateSvr == NULL ) 
		{
			NAK_RETURN(_T("Update Game Svr 插件启失败"));
		}

		int cmd = 0;
		long gid = 0;
		stdex::tString SID, taskName;
		bool IsDelFile = false;
		try
		{
			inpackage >> cmd >> gid >> SID >> taskName >> IsDelFile;
		}
		catch(std::exception &/*e*/)
		{
			Log(i8desk::LM_ERROR, _T("解析命令错误 %d"), CMD_CON_SYNCTASK_OPERATE);
			NAK_RETURN(_T("解析命令错误"));
		}

		Log(i8desk::LM_DEBUG, _T("同步任务操作 cmd = %d gid = %d SID = %s  taskName = %s , Isdel = %d"), cmd, gid, SID.c_str(), taskName.c_str(), IsDelFile);

		char dataBuf[4096] = {0};
		utility::serialize::Serialize os(dataBuf);
		os << gid << SID << taskName << IsDelFile;
		updateSvr->SendMessage(CMD_UPT_SM_OPERATE, cmd, (DWORD)&dataBuf);
		LPSTR pbuf = pNetLayer->GetIMemPool()->Alloc(64);
		CPkgHelper out(pbuf, 64, header.Cmd, OBJECT_CONSOLE);
		out << 1L;
		pNetLayer->SendPackage(sck, (LPSTR)out.GetBuffer(), out.GetLength());
	}

void CRTDataSvr::Cmd_Client_GetDelGame(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
{		
	TCHAR clientName[MAX_NAME_LEN] = {0};
	try 
	{
		inpackage >> clientName ;
	}
	catch(...)
	{
		NAK_RETURN(_T("错误的命令格式"));
	}
	
	ISvrPlug *GameMgrPlug  = m_pPlugMgr->FindPlug(PLUG_GAMEMGR_NAME) ;
	if( GameMgrPlug == NULL ) 
	{
		NAK_RETURN(_T("GameMgr 插件启失败"));
	}


	try
	{
		typedef std::vector<std::pair<long, stdex::tString>> DelGame;

		char *buf = reinterpret_cast<char *>(GameMgrPlug->SendMessage(CLEINT_DELETE_GAME, Find,  (DWORD)&clientName));
		size_t cnt = 0;
		memcpy(&cnt, buf, sizeof(size_t));

		utility::serialize::Serialize os(buf + sizeof(size_t), cnt);

		DelGame delGame;
		os >> delGame;

		size_t bufsize =  delGame.size() * ( MAX_PATH + sizeof(long) + sizeof(size_t))  + MAX_NAME_LEN + sizeof(size_t) * 3;
		LPSTR pbuf = pNetLayer->GetIMemPool()->Alloc(bufsize);
		CPkgHelper out(pbuf, bufsize, header.Cmd, OBJECT_I8DESKCLISVR);

		out << 1L << clientName <<  delGame.size();

		for(DelGame::iterator iter = delGame.begin(); iter != delGame.end(); ++iter )
		{
			out << iter->first << iter->second;
		}

		pNetLayer->SendPackage(sck,(LPSTR)out.GetBuffer(), out.GetLength());

		::CoTaskMemFree(buf);
	}
	catch (...)
	{
		NAK_RETURN(_T("GetDelGame时发生错误"));
	}

}


void CRTDataSvr::Cmd_Client_RptGameStatus(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
{
	TCHAR clientName[MAX_NAME_LEN] = {0};
	DWORD dwCount = 0;

	try
	{
		inpackage >> clientName >> dwCount ;
	}
	catch (...)
	{
		NAK_RETURN(_T("错误的命令格式"));
	}

	ISvrPlug *GameMgrPlug  = m_pPlugMgr->FindPlug(PLUG_GAMEMGR_NAME) ;
	if( GameMgrPlug == NULL ) 
	{
		NAK_RETURN(_T("GameMgr 插件启失败"));
	}

	try
	{
		std::vector<long> GIDS;
		for(size_t i = 0; i < dwCount; ++i)
		{
			DWORD gid = 0;
			inpackage >> gid;
			GIDS.push_back(gid);
		}

		const size_t cnt = GIDS.size() * sizeof(long) + MAX_NAME_LEN + sizeof(size_t);
		char *buf = (char *)::CoTaskMemAlloc(cnt);
		utility::serialize::Serialize os(buf, cnt);
		os << cnt << clientName << GIDS;

		GameMgrPlug->SendMessage(CLEINT_DELETE_GAME, Del, (DWORD)buf);	

		CoTaskMemFree(buf);

		ACK();
	}
	catch (...)
	{
		NAK_RETURN(_T("RptDelGameStatus时发生错误"));
	}

}

} //namespace i8desk

#include "stdafx.h"
#include "ControlMgr.h"

#include "../NetworkMgr.h"
#include "../Data/DataHelper.h"
#include "CustomDataType.h"


#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../../../../include/I8Type.hpp"
#include "../../../../include/Win32//System/DynamicFunc.hpp"


#include "../../../../include/UpdateGame.h"



namespace i8desk
{
	namespace manager
	{
		namespace detail
		{
			//template < typename NetworkT, typename VecT, typename ClientCtrlCmdT >
			data_helper::Buffer SendControlMsg(NetworkDataMgr *network, const std::vector<DWORD> &list, int cmd, DWORD flag)
			{
				const size_t bufSize = 64 + list.size() * sizeof(DWORD);
				std::vector<char> buf(bufSize);

				CPkgHelper out(&buf[0], bufSize,  CMD_GAME_CONTOCLIENT, OBJECT_CONSOLE);
				out << sizeof(DWORD) * 2 << cmd << flag << list.size() * sizeof(DWORD);
				for(size_t i = 0; i != list.size(); ++i)
					out << list[i];

				return data_helper::ExecuteMsg(network, out.GetBuffer(), out.GetLength());
			}
		}
	


		ControlMgr::ControlMgr(NetworkDataMgr *network)
			: network_(network)
			, errorCallback_(0)
		{}
		ControlMgr::~ControlMgr()
		{}

		void ControlMgr::RegisterError(const ErrorCallback &errorCallback)
		{
			errorCallback_ = errorCallback;
		}


		bool ControlMgr::InstallProt(const std::vector<DWORD>& list, bool bProtAllArea)
		{
			assert(list.size() != 0);

			AutoLock lock(network_->MutexData());

			return exception::ExceptHandle(std::tr1::bind(&detail::SendControlMsg, 
				network_, std::tr1::cref(list), Ctrl_InstallProt, (bProtAllArea ? 1 : 0)), errorCallback_);
		}

		bool ControlMgr::EnableSafeCenter(const std::vector<DWORD>& list)
		{
			assert(list.size() != 0);

			AutoLock lock(network_->MutexData());

			return exception::ExceptHandle(std::tr1::bind(&detail::SendControlMsg, 
				network_, std::tr1::cref(list), Ctrl_EnableSC, 0L), errorCallback_);
		}

		bool ControlMgr::EnableIEProt(const std::vector<DWORD>& list)
		{
			assert(list.size() != 0);
			AutoLock lock(network_->MutexData());

			return exception::ExceptHandle(std::tr1::bind(&detail::SendControlMsg, 
				network_, std::tr1::cref(list), Ctrl_EnableIEProt, 0L), errorCallback_);
		}

		bool ControlMgr::EnableDogProt(const std::vector<DWORD>& list)
		{
			assert(list.size() != 0);
			AutoLock lock(network_->MutexData());

			return exception::ExceptHandle(std::tr1::bind(&detail::SendControlMsg, 
				network_, std::tr1::cref(list), Ctrl_EnableDogProt, 0L), errorCallback_);
		}

		bool ControlMgr::RemoveProt(const std::vector<DWORD>& list)
		{
			assert(list.size() != 0);

			const size_t bufSize = 64 + list.size() * sizeof(DWORD);
			std::vector<char> buf(bufSize);

			CPkgHelper out(&buf[0], bufSize,  CMD_GAME_CONTOCLIENT, OBJECT_CONSOLE);
			out << sizeof(DWORD) * 1 << Ctrl_RemoveProt << list.size() * sizeof(DWORD);
			for(size_t i = 0; i != list.size(); ++i)
				out << list[i];

			AutoLock lock(network_->MutexData());

			return exception::ExceptHandle(std::tr1::bind(&data_helper::ExecuteMsg, 
				network_, out.GetBuffer(), out.GetLength()), errorCallback_);
		}

		bool ControlMgr::ProtArea(const std::vector<DWORD>& list, const stdex::tString &ProtArea, const stdex::tString &UnProtArea)
		{
			assert(list.size() != 0);
			const size_t length = sizeof(DWORD) + sizeof(TCHAR) * (ProtArea.length() +1 + UnProtArea.length() + 1) + 2 * sizeof(DWORD);
			const size_t bufSize =  64 + length + list.size() * sizeof(DWORD);
			std::vector<char> buf(bufSize);

			CPkgHelper out(&buf[0], bufSize,  CMD_GAME_CONTOCLIENT, OBJECT_CONSOLE);
			out << length << Ctrl_ProtArea
				<< ProtArea << UnProtArea << list.size() * sizeof(DWORD);
			for(size_t i = 0; i != list.size(); ++i)
				out << list[i];

			AutoLock lock(network_->MutexData());
			return exception::ExceptHandle(std::tr1::bind(&data_helper::ExecuteMsg, 
				network_, out.GetBuffer(), out.GetLength()), errorCallback_);
		}

		bool ControlMgr::ShutDown(const std::vector<DWORD>& list, bool bReBoot)
		{
			assert(list.size() != 0);

			AutoLock lock(network_->MutexData());

			return exception::ExceptHandle(std::tr1::bind(&detail::SendControlMsg, 
				network_, std::tr1::cref(list), Ctrl_ShutDown, (bReBoot ? 1L : 0L)), errorCallback_);
		}

		bool ControlMgr::RealPush(const std::vector<DWORD>& list, const stdex::tString &strGids, DWORD dwLimitSpeed, DWORD dwConstructWay)
		{
			assert(list.size() != 0);

			const size_t length = sizeof(DWORD) * 3 + (strGids.length()  + 1) * sizeof(TCHAR) + sizeof(DWORD);
			const size_t bufSize = 64 + list.size() * sizeof(DWORD) + strGids.length();
			std::vector<char> buf(bufSize);

			CPkgHelper out(&buf[0], bufSize,  CMD_GAME_CONTOCLIENT, OBJECT_CONSOLE);
			out << length << Ctrl_PushGameAtOnce
				<< dwConstructWay << dwLimitSpeed << strGids << list.size() * sizeof(DWORD);
			for(size_t i = 0; i != list.size(); ++i)
				out << list[i];

			AutoLock lock(network_->MutexData());
			return exception::ExceptHandle(std::tr1::bind(&data_helper::ExecuteMsg, 
				network_, out.GetBuffer(), out.GetLength()), errorCallback_);
		}

		bool ControlMgr::ExecExeFile(const std::vector<DWORD>& list, const stdex::tString &szFile)
		{
			assert(list.size() != 0);
			const size_t length = sizeof(DWORD) + (szFile.length()  + 1) * sizeof(TCHAR) + sizeof(DWORD);
			const size_t bufSize = 64 + list.size() * sizeof(DWORD) + (szFile.length()  + 1) * sizeof(TCHAR) ;
			std::vector<char> buf(bufSize);

			CPkgHelper out(&buf[0], bufSize,  CMD_GAME_CONTOCLIENT, OBJECT_CONSOLE);
			out << length << Ctrl_ExecFile
				<< szFile << list.size() * sizeof(DWORD);
			for(size_t i = 0; i != list.size(); ++i)
				out << list[i];

			AutoLock lock(network_->MutexData());
			return exception::ExceptHandle(std::tr1::bind(&data_helper::ExecuteMsg, 
				network_, out.GetBuffer(), out.GetLength()), errorCallback_);
		}

		bool ControlMgr::Chkdsk(const std::vector<DWORD>& list, const stdex::tString &strArea)
		{
			assert(list.size() != 0);

			const size_t length = sizeof(DWORD) + (strArea.length()  + 1) * sizeof(TCHAR) + sizeof(DWORD);
			const size_t bufSize = 64 + list.size() * sizeof(DWORD) + strArea.length();
			std::vector<char> buf(bufSize);

			CPkgHelper out(&buf[0], bufSize,  CMD_GAME_CONTOCLIENT, OBJECT_CONSOLE);
			out << length << Ctrl_Chkdsk
				<< strArea << list.size() * sizeof(DWORD);
			for(size_t i = 0; i != list.size(); ++i)
				out << list[i];

			AutoLock lock(network_->MutexData());
			return exception::ExceptHandle(std::tr1::bind(&data_helper::ExecuteMsg, 
				network_, out.GetBuffer(), out.GetLength()), errorCallback_);
		}

		bool ControlMgr::RemoteControl(DWORD ip, int bstart)
		{
			TCHAR szPath[MAX_PATH] = {0};
			GetModuleFileName(0, szPath, sizeof(szPath));
			PathRemoveFileSpec(szPath);

			PathAddBackslash(szPath);
			stdex::tOstringstream os;
			os << _T("tools\\vncviewer.exe ") <<  IP2String(ip) << _T("|") << bstart;

			STARTUPINFO si = {sizeof(si)};
			PROCESS_INFORMATION pi = {0};

			if (CreateProcess(0, (LPTSTR)os.str().c_str(), 0, 0, FALSE, 0, 0, 0, &si, &pi)) 
			{
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
				return true;
			}
			else
			{
				stdex::tOstringstream err;
				err << _T("远程控制工具[ ") << szPath << _T(" ]不存在!\n启动远程控制失败!");
				throw exception::Businuss(err.str());
				return false;
			}
		}

		bool ControlMgr::ViewClientInfo(const std::vector<DWORD>& list, DWORD flag, stdex::tString &text)
		{
			assert(list.size() != 0);

			const size_t bufSize = 64 + list.size() * sizeof(DWORD);
			std::vector<char> buf(bufSize);

			// 同时只能查看一个客户机信息
			CPkgHelper out(&buf[0], bufSize,  CMD_GAME_CONTOCLIENT, OBJECT_CONSOLE);
			out << sizeof(DWORD) * 1 << flag << /*list.size()*/ 1 * sizeof(DWORD);
			for(size_t i = 0; i != /*list.size()*/ 1; ++i)
				out << list[i];

			data_helper::Buffer buffer;
			{
				AutoLock lock(network_->MutexData());
				if( !exception::ExceptHandle(buffer,
					std::tr1::bind(&data_helper::ExecuteMsg, network_, out.GetBuffer(), out.GetLength()), errorCallback_) )
					return false;
			}

			CPkgHelper in(buffer.first.get(), buffer.second);
			DWORD ret = 0;
			try
			{
				in >> ret;
				if( ret != flag )
					return false;
				in >> text;
			}
			catch(std::exception &/*e*/)
			{
				stdex::tString error;
				in >> error;
				errorCallback_(std::tr1::ref(error));
			}
			
			return true;
		}

		bool ControlMgr::KillProcSvr(const stdex::tString &file,  DWORD ip, bool bIsProcess)
		{
			char buf[512] = {0};

			CPkgHelper out(buf, CMD_GAME_CONTOCLIENT, OBJECT_CONSOLE);
			out << (file.size()+1)*sizeof(TCHAR) + 4 + 4 + 4 << Ctrl_KillProcSvr
				<< (bIsProcess ? 1L : 0L) << file << 4L << ip;

			AutoLock lock(network_->MutexData());
			return exception::ExceptHandle(std::tr1::bind(&data_helper::ExecuteMsg, 
				network_, out.GetBuffer(), out.GetLength()), errorCallback_);

		}

		bool ControlMgr::GetFileOrDir(const stdex::tString& szDir, DWORD nType, bool IsAbsPath, 
						  std::vector<stdex::tString>& ListFiles)
		{
			char buf[1024*1] = {0};

			CPkgHelper out(buf, CMD_CLIUPT_GET_FILELIST, OBJECT_CONSOLE);
			out << nType << (IsAbsPath ? 0L : 1L) << szDir.c_str();

			data_helper::Buffer buffer;

			{
				AutoLock lock(network_->MutexData());

				if( !exception::ExceptHandle(buffer, 
					std::tr1::bind(&data_helper::ExecuteCmd, network_, out.GetBuffer(), out.GetLength()),
					errorCallback_) )
					return false;
			}

			CPkgHelper in(buffer.first.get(), buffer.second);

			DWORD ret = 0, nValue = 0, dwSize = 0;

			if (nType == data_helper::GET_DRVLIST)
			{
				in >> ret >>nValue;
				for (char chDrv='C'; chDrv<='Z'; chDrv++)
				{
					if ((1<< (chDrv-'A')) & nValue)
					{
						TCHAR buf1[20] = {0};
						_stprintf_s(buf1, _T("%C:"), chDrv);
						ListFiles.push_back(buf1);
					}
				}
			}
			else
			{
				
				in >> ret >> dwSize;
				if( ret != 1 )
					return false;

				for (DWORD idx=0; idx<dwSize; idx++)
				{
					TCHAR file[MAX_PATH] = {0};
					in >> file;
					ListFiles.push_back(file);
				}
			}

			return true;
		}

		bool ControlMgr::GetFileData(const stdex::tString &dir, data_helper::Buffer &data)
		{
			char buf[1024] = {0};
			CPkgHelper out(buf, CMD_CLIUPT_DOWNFILE_START, OBJECT_CONSOLE);
			out << 1L << static_cast<DWORD>(FP_REALPATH) << dir.c_str();

			bool suc = false;
			data_helper::Buffer outBuf;
			{
				AutoLock lock(network_->MutexData());
				suc = exception::ExceptHandle(outBuf, std::tr1::bind(&data_helper::ExecuteMsg, 
					network_, out.GetBuffer(), out.GetLength()), errorCallback_);
			}

			if( suc == false )
				return false;

			// 接受数据大小
			CPkgHelper in(outBuf.first.get(), outBuf.second);
			DWORD ret = 0;
			DWORD size = 0;
			in >> ret >> size;
		
			data.first = data_helper::BufferAllocator(size);
			data.second = size;

			// 接受数据
			DWORD remSize = 0;
			while(remSize < size)
			{
				CPkgHelper out(buf, CMD_DOWNFILE_NEXT, OBJECT_CONSOLE);

				data_helper::Buffer dataBuf;
				
				{
					AutoLock lock(network_->MutexData());
					suc = exception::ExceptHandle(dataBuf, std::tr1::bind(&data_helper::ExecuteMsg, 
						network_, out.GetBuffer(), out.GetLength()), errorCallback_);
				}

				assert(suc);
				if( !suc )
					break;

				CPkgHelper in(dataBuf.first.get(), dataBuf.second);
				DWORD ret = 0;
				DWORD size = 0;
				in >> ret >> ret >> size;

				in.PopPointer(data.first.get() + remSize, size);
			
				remSize += size;
			}

			assert(remSize == data.second);
			return suc;
		}

		bool ControlMgr::SaveFileData(const stdex::tString &dir, data_helper::Buffer &data)
		{
			char buf[8192 + 0x100] = {0};
			CPkgHelper out(buf, CMD_CON_SETFILE_CLASSICON, OBJECT_CONSOLE);
			out << static_cast<DWORD>(FP_REALPATH) << dir.c_str() << data.second;
			out.PushPointer(data.first.get(), data.second);

			bool suc = false;
			{
				AutoLock lock(network_->MutexData());
				suc = exception::ExceptHandle(std::tr1::bind(&data_helper::ExecuteMsg, 
					network_, out.GetBuffer(), out.GetLength()), errorCallback_);
			}

			return suc;
		}

		bool ControlMgr::GetGameIcons(data_helper::Buffer &data)
		{
			char buf[512] = {0};

			CPkgHelper out(buf, CMD_CON_GETALL_GAMEICONS, OBJECT_CONSOLE);

			AutoLock lock(network_->MutexData());
			return exception::ExceptHandle(data, std::tr1::bind(&data_helper::ExecuteMsg, 
				network_, out.GetBuffer(), out.GetLength()), errorCallback_);
		}

		bool ControlMgr::GetClassIcons(data_helper::Buffer &data)
		{
			char buf[512] = {0};

			CPkgHelper out(buf, CMD_CON_GETALL_CLASSICONS, OBJECT_CONSOLE);

			AutoLock lock(network_->MutexData());
			return exception::ExceptHandle(data, std::tr1::bind(&data_helper::ExecuteMsg, 
				network_, out.GetBuffer(), out.GetLength()), errorCallback_);
		}

		bool ControlMgr::AddIcon(DWORD gid, unsigned char* pData, size_t size)
		{
			static char buf[1024 * 20] = {0};
			CPkgHelper out(buf, CMD_CON_GAME_ICON, OBJECT_CONSOLE);
			out << static_cast<DWORD>(ioAdd) << gid << size;
			out.PushPointer(pData, size);

			AutoLock lock(network_->MutexData());
			return exception::ExceptHandle(std::tr1::bind(&data_helper::ExecuteMsg, 
				network_, out.GetBuffer(), out.GetLength()), errorCallback_);
		}

		bool ControlMgr::DelIcon(DWORD gid)
		{
			char buf[1024 * 1] = {0};
			CPkgHelper out(buf, CMD_CON_GAME_ICON, OBJECT_CONSOLE);
			out << static_cast<DWORD>(ioDel) << gid;

			AutoLock lock(network_->MutexData());
			return exception::ExceptHandle(std::tr1::bind(&data_helper::ExecuteMsg, 
				network_, out.GetBuffer(), out.GetLength()), errorCallback_);
		}

		bool ControlMgr::GetIcon(DWORD gid, unsigned char *data, size_t &size)
		{
			char buf[1024 * 1] = {0};
			CPkgHelper out(buf, CMD_CON_GAME_ICON, OBJECT_CONSOLE);
			out << static_cast<DWORD>(ioGet) << gid;

			data_helper::Buffer buffer;

			{
				AutoLock lock(network_->MutexData());

				if( !exception::ExceptHandle(buffer, 
					std::tr1::bind(&data_helper::ExecuteCmd, network_, out.GetBuffer(), out.GetLength()),
					errorCallback_) )
					return false;
			}

			CPkgHelper in(buffer.first.get(), buffer.second);
			DWORD ret = 0;
			in >> ret;
			if( ret == 0 )
			{
				stdex::tString error;
				in >> error;
				return false;
			}

			in >> size;
			in.PopPointer(data, size);

			return true;
		}

		bool ControlMgr::NotifySaveData()
		{
			char buf[1024 * 1] = {0};
			CPkgHelper out(buf, CMD_NOTIFY_SAVE_DATA, OBJECT_CONSOLE);

			AutoLock lock(network_->MutexData());
			return exception::ExceptHandle(std::tr1::bind(&data_helper::ExecuteMsg, 
				network_, out.GetBuffer(), out.GetLength()), errorCallback_);
		}

		bool ControlMgr::PlugToolOperate(long gid, DWORD op)
		{
			char buf[1024 * 1] = {0};
			CPkgHelper out(buf, CMD_CON_PLUGTOOL_OPERATE, OBJECT_CONSOLE);
			out << gid << op;

			data_helper::Buffer inBuf;
			{
				AutoLock lock(network_->MutexData());
				bool suc = exception::ExceptHandle(inBuf, std::tr1::bind(&data_helper::ExecuteMsg, 
					network_, out.GetBuffer(), out.GetLength()), errorCallback_);
				if( !suc )
					return false;
			}

			CPkgHelper in(inBuf.first.get(), inBuf.second);
			DWORD ret = 0, opFlag = 0;
			in >> ret >> opFlag;
			if( opFlag )
				return true;
			else
				return false;
		}

		bool ControlMgr::GetPlugIcon(long gid, unsigned char *data, size_t &size)
		{
			char buf[1024 * 1] = {0};
			CPkgHelper out(buf, CMD_CON_PLUGTOOL_ICON, OBJECT_CONSOLE);
			out << gid;

			data_helper::Buffer buffer;

			{
				AutoLock lock(network_->MutexData());

				if( !exception::ExceptHandle(buffer, 
					std::tr1::bind(&data_helper::ExecuteCmd, network_, out.GetBuffer(), out.GetLength()),
					errorCallback_) )
					return false;
			}

			CPkgHelper in(buffer.first.get(), buffer.second);
			DWORD ret = 0;
			in >> ret;
			if( ret == 0 )
			{
				stdex::tString error;
				in >> error;
				return false;
			}

			in >> size;
			in.PopPointer(data, size);
			return true;
		}

		bool ControlMgr::DeleteTmpFiles()
		{
			char buf[1024 * 1] = {0};
			CPkgHelper out(buf, CMD_CON_MONITOR_DELETEFILE, OBJECT_CONSOLE);

			AutoLock lock(network_->MutexData());
			return exception::ExceptHandle(std::tr1::bind(&data_helper::ExecuteMsg, 
				network_, out.GetBuffer(), out.GetLength()), errorCallback_);
		}


		bool ControlMgr::GetViewGame(DWORD param,data_helper::ViewGameTraits::VectorType &viewGames)
		{
			char buf[1024] = {0};
			CPkgHelper out(buf, CMD_CON_GETINFO_FROM_GAMEUPDATESVR, OBJECT_CONSOLE);
			out << CMD_UPT_SM_IPHAVEGAME << param;
			data_helper::Buffer buffer;
			// 发送数据并接收
			{
				AutoLock lock(network_->MutexData());
				if( !exception::ExceptHandle(buffer, 
					std::tr1::bind(&data_helper::ExecuteCmd, network_, out.GetBuffer(), out.GetLength()),
					errorCallback_) ) 
					return false;
			}


			CPkgHelper in(buffer.first.get(), buffer.second);
			DWORD ret = 0;
			DWORD cnt = 0;
			in >> ret;
			if( ret != 1 )
			{
				stdex::tString error;
				in >> error;
				throw exception::Businuss(error);
			}

			in >> cnt;
			viewGames.clear();

			for(size_t i = 0; i != cnt; ++i)
			{
				data_helper::ViewGameTraits::ValueType val(new data_helper::ViewGameTraits::ElementType);
				in >> *val;
				viewGames.push_back(val);
			}

			return true;
		}

		bool ControlMgr::GetViewClient( DWORD param, data_helper::ViewClientTraits::VectorType &viewClients)
		{
			char buf[1024] = {0};
			CPkgHelper out(buf, CMD_CON_GETINFO_FROM_GAMEUPDATESVR, OBJECT_CONSOLE);
			out << CMD_UPT_SM_GAMEINIP << param;

			data_helper::Buffer buffer;
			// 发送数据并接收
			{
				AutoLock lock(network_->MutexData());
				if( !exception::ExceptHandle(buffer, 
					std::tr1::bind(&data_helper::ExecuteCmd, network_, out.GetBuffer(), out.GetLength()),
					errorCallback_) ) 
					return false;
			}


			CPkgHelper in(buffer.first.get(), buffer.second);
			DWORD ret = 0;
			DWORD cnt = 0;
			in >> ret;
			if( ret != 1 )
			{
				stdex::tString error;
				in >> error;
				throw exception::Businuss(error);
			}

			in >> cnt;
			viewClients.clear();

			for(size_t i = 0; i != cnt; ++i)
			{
				data_helper::ViewClientTraits::ValueType val(new data_helper::ViewClientTraits::ElementType);
				in >> *val;
				viewClients.push_back(val);
			}

			return true;
		}


		bool ControlMgr::CheckUser(const stdex::tString &userName, const stdex::tString &psw)
		{
			char buf[1024] = {0};
			CPkgHelper out(buf, CMD_VALIDATE, OBJECT_CONSOLE);
			out << userName << psw;

			data_helper::Buffer buffer;
			// 发送数据并接收
			{
				AutoLock lock(network_->MutexData());
				if( !exception::ExceptHandle(buffer, 
					std::tr1::bind(&data_helper::ExecuteMsg, network_, out.GetBuffer(), out.GetLength()),
					errorCallback_) ) 
					return false;
			}

			return true;
		}

		bool ControlMgr::ModifyNetbarInfo()
		{
			char buf[1024] = {0};
			CPkgHelper out(buf, CMD_CON_REPORT_USERINFOMODIFY, OBJECT_CONSOLE);
			
			data_helper::Buffer buffer;
			// 发送数据并接收
			{
				AutoLock lock(network_->MutexData());
				if( !exception::ExceptHandle(buffer, 
					std::tr1::bind(&data_helper::ExecuteMsg, network_, out.GetBuffer(), out.GetLength()),
					errorCallback_) ) 
					return false;
			}

			return true;
		}

		bool ControlMgr::NotifyReloadFile(const stdex::tString &path)
		{
			char buf[1024] = {0};
			CPkgHelper out(buf, CMD_RELOAD_CACHE_FILE, OBJECT_CONSOLE);
			out << path ;

			data_helper::Buffer buffer;
			// 发送数据并接收
			{
				AutoLock lock(network_->MutexData());
				if( !exception::ExceptHandle(buffer, 
					std::tr1::bind(&data_helper::ExecuteMsg, network_, out.GetBuffer(), out.GetLength()),
					errorCallback_) ) 
					return false;
			}

			return true;
		}


		bool ControlMgr::SaveHardware()
		{
			char buf[1024] = {0};

			CPkgHelper out(buf, CMD_CON_SAVE_HARDWARE, OBJECT_CONSOLE);

			data_helper::Buffer buffer;
			// 发送数据并接收
			{
				AutoLock lock(network_->MutexData());
				if( !exception::ExceptHandle(buffer, 
					std::tr1::bind(&data_helper::ExecuteMsg, network_, out.GetBuffer(), out.GetLength()),
					errorCallback_) ) 
					return false;
			}

			return true;
		}

		bool ControlMgr::NotifySmartClean(const std::vector<long> &gids)
		{
			char buf[64 * 1024] = {0};

			CPkgHelper out(buf, CMD_CON_SMART_CLEAN, OBJECT_CONSOLE);

			out << gids.size();
			for(size_t i = 0; i != gids.size(); ++i)
				out << gids[i];

			data_helper::Buffer buffer;
			// 发送数据并接收
			{
				AutoLock lock(network_->MutexData());
				if( !exception::ExceptHandle(buffer, 
					std::tr1::bind(&data_helper::ExecuteMsg, network_, out.GetBuffer(), out.GetLength()),
					errorCallback_) ) 
					return false;
			}

			return true;
		}

		bool ControlMgr::NeedSmartClean(const stdex::tString &path, const ulonglong totalSize, bool &isEnough)
		{
			char buf[64 * 1024] = {0};

			CPkgHelper out(buf, CMD_CON_MONITOR_DISK_SIZE, OBJECT_CONSOLE);
			out << path << totalSize ;
			
			data_helper::Buffer buffer;
			// 发送数据并接收
			{
				AutoLock lock(network_->MutexData());
				if( !exception::ExceptHandle(buffer, 
					std::tr1::bind(&data_helper::ExecuteMsg, network_, out.GetBuffer(), out.GetLength()),
					errorCallback_) ) 
					return false;
			}

			CPkgHelper in(buffer.first.get(), buffer.second);
			DWORD ret = 0;
			in >> ret;
			if( ret != 1 )
			{
				stdex::tString error;
				in >> error;
				throw exception::Businuss(error);
			}

			DWORD enough = 0;
			in >> enough;

			isEnough = (enough == 1);
			return true;
		}


		bool ControlMgr::SyncTaskOperate(int cmd, long gid, const stdex::tString &SID, const stdex::tString &TaskName, const bool &IsDelFile )
		{
			char buf[64 * 1024] = {0};

			CPkgHelper out(buf, CMD_CON_SYNCTASK_OPERATE, OBJECT_CONSOLE);
			out << cmd << gid << SID << TaskName << IsDelFile;

			// 发送数据并接收
			{
				AutoLock lock(network_->MutexData());
				if( !exception::ExceptHandle(std::tr1::bind(&data_helper::ExecuteMsg, network_, out.GetBuffer(), out.GetLength()),
					errorCallback_) ) 
					return false;
			}

			return true;
		}
	}
}
#include "stdafx.h"
#include "VDiskBusiness.h"

#include "../Network/IOService.h"
#include "../ManagerInstance.h"

namespace i8desk
{
	namespace business
	{

		namespace VDisk
		{

			// 虚拟盘端口
			u_short VDiskPort = 17918;


			void RunTypeDesc(const data_helper::VDiskTraits::ValueType &val, stdex::tString &msg)
			{
				stdex::tOstringstream os;

				if( val->Type == data_helper::I8VDisk )
				{
					os << data_helper::RT_VDISK << (TCHAR)val->CliDrv 
						<< _T(" (") << IP2String(val->SoucIP).c_str()
						<< _T(":") << (TCHAR)val->SvrDrv
						<< _T(")");
				}
				else
				{
					os << data_helper::RT_VDISK << (TCHAR)val->CliDrv;
				}

				msg = os.str();
			}

			// 虚拟盘包头信息
			namespace vdisk
			{
				#pragma pack(push,8)
				#pragma warning(disable:4200)

				enum {
					DRP_MJ_REFRESH	 = 0x01,		//刷新虚拟为盘，上行只有一个命令，应答的数据空。只是成功。
					DRP_MJ_READ		 = 0X02,
					DRP_MJ_WRITE	 = 0x03,
					DRP_MJ_QUERY	 = 0x04,
				};

				typedef struct tagDRP
				{
					BYTE			 MajorFunction;//1: Get Size ;2: Read ; 3: Write; 4:get Info
					union
					{
						struct 
						{
							LARGE_INTEGER	Offset;
							ULONG			Length;
						}Read;
						struct 
						{
							LARGE_INTEGER   Offset;
							ULONG			Length;
						}Write;
						struct 
						{
							ULONG	DiskSize;
						}Query;
					};
					char buffer[0];
				} DRP, *LPDRP;

				//应答头
				typedef struct tagDSTATUS
				{
					WORD	Status;		//STATUS_
					DWORD	Information;
					DWORD	Length;
					char	buffer[0];
				} DSTATUS, *LPDSTATUS; //*/

				enum {
					STATUS_SUCCESS			= 0x0000,
					STATUS_INVALID_CALL 	= 0x2001,
					STATUS_READ_ERROR		= 0x2002,
					STATUS_WRITE_ERROR  	= 0x2003,
				};

				#pragma pack(pop)
			}

			namespace detail
			{
				using namespace async::network;
				using namespace async::timer;

				class VDiskHelper
					: public std::tr1::enable_shared_from_this<VDiskHelper>
				{
				private:
					Tcp::Socket sck_;
					Timer		timeOut_;

					volatile long connect_;
					async::thread::AutoEvent	complate_;

				public:
					VDiskHelper()
						: sck_(io::GetIODispatcher(), Tcp::V4())
						, timeOut_(io::GetIODispatcher())
						, connect_(0)
					{
						complate_.Create();
					}
					~VDiskHelper()
					{

					}

					void Connect(const std::string &ip, u_short port)
					{
						timeOut_.AsyncWait(std::tr1::bind(&VDiskHelper::_OnTimeout, this), 3 * 1000);
						sck_.AsyncConnect(String2IP(ip), port, 
							std::tr1::bind(&VDiskHelper::_OnConnect, shared_from_this(), _Error));
					}

					void Send(const char *buf, size_t sz)
					{
						sck_.Write(Buffer(buf, sz)); 
					}

					void Recv(char *buf, size_t sz)
					{
						sck_.Read(Buffer(buf, sz));
					}

					bool WaitSuccess()
					{
						complate_.WaitForEvent();
						return ::InterlockedExchangeAdd(&connect_, 0) == 1;
					}

				private:
					void _OnConnect(u_long err)
					{
						timeOut_.Cancel();
						::InterlockedExchange(&connect_, err == 0 ? 1 : 0);
						complate_.SetEvent();
					}

					void _OnTimeout()
					{
						sck_.Close();
					}
				};

				typedef std::tr1::shared_ptr<VDiskHelper> VDiskHelperPtr;
			}


			void Check(const std::string &ip, u_short port, CString &result)
			{
				using namespace vdisk;

				DRP drp = {0};
				drp.MajorFunction = DRP_MJ_QUERY;
				drp.Read.Offset.QuadPart = 0;
				drp.Read.Length = 512;

				try
				{
					detail::VDiskHelperPtr vDiskCheck(new detail::VDiskHelper);
					vDiskCheck->Connect(ip, port);

					if( vDiskCheck->WaitSuccess() )
						vDiskCheck->Send((const char *)&drp, sizeof(drp));
					else
						throw false;
				}
				catch(...)
				{
					result += _T("连接服务器失败");
					return;
				}

				result += _T("连接服务器成功");
			}


			void Refresh(const std::string &ip, u_short port, CString &result)
			{
				vdisk::DRP drp = {0};
				drp.MajorFunction = vdisk::DRP_MJ_REFRESH;

				
				detail::VDiskHelperPtr vDiskCheck(new detail::VDiskHelper);
				try
				{
					vDiskCheck->Connect(ip, port);

					if( vDiskCheck->WaitSuccess() )
						vDiskCheck->Send((const char *)&drp, sizeof(drp));
					else
						throw false;
				}
				catch(...)
				{
					result += _T("连接服务器失败");
					return;
				}


				vdisk::DSTATUS dstatus = { vdisk::STATUS_INVALID_CALL };

				try
				{
					vDiskCheck->Recv((char *)&dstatus, sizeof(dstatus));
				}
				catch(...)
				{
					result += _T("接收响应报文失败");
					return;
				}

				if( dstatus.Status != vdisk::STATUS_SUCCESS ) 
				{
					result += _T("服务端操作失败");
					return;
				}

				result += _T("刷盘成功");
			}


			bool IsValid(const stdex::tString &curVID, ulong ip, TCHAR svrDrv, TCHAR cliDrv, const std::tr1::function<void(LPCTSTR, LPCTSTR)> &errCallback)
			{
				const data_helper::VDiskTraits::MapType &vDisks = GetDataMgr().GetVDisks();
				
				for(data_helper::VDiskTraits::MapConstIterator iter = vDisks.begin(); iter != vDisks.end(); ++iter)
				{
					if( iter->second->SoucIP == ip && curVID != iter->second->VID )
					{
						if( iter->second->SvrDrv == svrDrv )
						{
							errCallback(_T("提示"), _T("已经有虚拟盘使用服务器盘符"));
							return false;
						}

						if( iter->second->CliDrv == cliDrv )
						{
							errCallback(_T("提示"),_T("己经使用了客户机的盘符"));
							return false;
						}
					}
				}

				return true;
			}

			stdex::tString GetVDiskName(const data_helper::VDiskTraits::ValueType &val)
			{
				stdex::tOstringstream os;
				os << _T("虚拟盘") 
					<< (TCHAR)val->SvrDrv 
					<< _T("(") 
					<< (TCHAR)val->CliDrv 
					<< _T(")");

				return os.str();
			}

			bool Delete(const stdex::tString &VID)
			{
				data_helper::VDiskTraits::MapType &vDisks = GetDataMgr().GetVDisks();
				
				return GetDataMgr().DelData(vDisks, VID);
			}

			namespace detail
			{
				struct
				{
					int type_;
					LPCTSTR desc_;
				}loadType[] = 
				{
					{ VD_BOOT,		_T("开机刷盘") },
					{ VD_STARTMENU, _T("运行菜单") },
					{ VD_STARTGAME, _T("运行游戏") }
				};
			}
			void GetLoadTypes(const std::tr1::function<void(int, LPCTSTR)> &callback)
			{
				for(size_t i = 0; i != _countof(detail::loadType); ++i)
					callback(detail::loadType[i].type_, detail::loadType[i].desc_);
			}

			LPCTSTR GetLoadTypeDescByType(int type)
			{
				for(size_t i = 0; i != _countof(detail::loadType); ++i)
				{
					if( detail::loadType[i].type_ == type )
						return detail::loadType[i].desc_;
				}

				return _T("");
			}

			namespace detail
			{
				struct
				{
					int type_;
					LPCTSTR desc_;
				}drvType[] = 
				{
					{ 0,		_T("不启用") },				{ 67,		_T("C") },
					{ 68,		_T("D") },					{ 69,		_T("E") },
					{ 70,		_T("F") },					{ 71,		_T("G") },
					{ 72,		_T("H") },					{ 73,		_T("I") },
					{ 74,		_T("J") },					{ 75,		_T("K") },
					{ 76,		_T("L") },					{ 77,		_T("M") },
					{ 78,		_T("N") },					{ 79,		_T("O") },
					{ 80,		_T("P") },					{ 81,		_T("Q") },
					{ 82,		_T("R") },					{ 83,		_T("S") },
					{ 84,		_T("T") },					{ 85,		_T("U") },
					{ 86,		_T("V") },					{ 87,		_T("W") },
					{ 88,		_T("X") },					{ 89,		_T("Y") },
					{ 90,		_T("Z") },
				};
			}
			void GetSsdDrv(const std::tr1::function<void(int, LPCTSTR)> &callback)
			{
				for(size_t i = 0; i != _countof(detail::drvType); ++i)
					callback(detail::drvType[i].type_, detail::drvType[i].desc_);
			}

			LPCTSTR GetSsdDescByType(int type)
			{
				for(size_t i = 0; i != _countof(detail::drvType); ++i)
				{
					if( detail::drvType[i].type_ == type )
						return detail::drvType[i].desc_;
				}

				return _T("");
			}
		}
	}
}
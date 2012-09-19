#include "stdafx.h"
#include "ClientBusiness.h"

#include "../../../../include/Utility/utility.h"
#include "../../../../include/Utility/Markup.h"
#include "../../../../include/Extend STL/StdEx.h"
#include "../../../../include/Extend STL/StringAlgorithm.h"

#include "../ManagerInstance.h"

#include "../../../../include/AsyncIO/Network/TCP.hpp"
#include "../../../../include/AsyncIO/Network/UDP.hpp"
#include "../Network/IOService.h"

#include "../Misc.h"
#include "../Data/RecvDataMgr.h"
#include "../MessageBox.h"

namespace i8desk
{
	namespace business
	{

		namespace Client
		{
			namespace detail
			{
				template < typename ContainerT, typename ClientsT >
				void Judge(ContainerT &clis, const ClientsT &clients)
				{
					for(size_t i = 0; i != clients.size(); ++i)
					{
						if( clients[i]->Online == 1 )
							clis.push_back(clients[i]->IP);
					}
				}
			}


			stdex::tString GetNameByIP(ulong ip)
			{
				const data_helper::ClientTraits::MapType &clients = GetDataMgr().GetClients();
				
				for(data_helper::ClientTraits::MapConstIterator iter = clients.begin();
					iter != clients.end(); ++iter)
				{
					if( iter->second->IP == ip )
						return iter->first;
				}
				
				return _T("");
			}

			void ControlStart(const data_helper::ClientTraits::VectorType &clients)
			{
				for(size_t i = 0; i != clients.size(); ++i)
				{
					using namespace async::network;

			
					Udp::Socket sock(i8desk::io::GetIODispatcher(), Udp::V4());
					if( !sock.IsOpen() )
						return;

					sock.SetOption(Broadcast(1));


					//网络唤醒协议：6个0xff, 16个重复的mac地址，6个字节的密码
					unsigned char buf[6 + 6 * 16 + 6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
					if( utility::IsEmpty(clients[i]->MAC) )
						return;
					

					unsigned char mac[6] = {0};
					if( !i8desk::StringMacToBinaryMac((const unsigned char *)(const char *)CT2A(clients[i]->MAC), mac) )
						return;
					

					for(int i = 0; i < 16; i++) 
						memcpy(buf+6+i*6, mac, 6);

					sockaddr_in address;
					int len = sizeof(address);

					address.sin_family = AF_INET;
					address.sin_addr.s_addr = 0xffffffff;
					address.sin_port = ::htons(5050);

					try
					{
						size_t len = sock.SendTo(async::iocp::Buffer((const char *)buf), &address);
						if( len != _countof(buf) )
							return;
					}
					catch(std::exception &e)
					{
						assert(e.what());
					}
				}
			}

			void ControlRestart(const data_helper::ClientTraits::VectorType &clients)
			{
				std::vector<DWORD> clis;

				detail::Judge(clis, clients);

				if( clis.empty() )
					return;

				GetControlMgr().ShutDown(clis, true);
			}


			void ControlShutdown(const data_helper::ClientTraits::VectorType &clients)
			{
				std::vector<DWORD> clis;

				detail::Judge(clis, clients);

				if( clis.empty() )
					return;

				GetControlMgr().ShutDown(clis, false);
			}


			void ControlDiskRecorver(const data_helper::ClientTraits::VectorType &clients, const stdex::tString &areas)
			{
				std::vector<DWORD> clis;

				detail::Judge(clis, clients);

				if( clis.empty() )
					return;

				GetControlMgr().Chkdsk(clis, areas);
			}

			void ControlRunClientFile(const data_helper::ClientTraits::VectorType &clients,  const stdex::tString &path)
			{
				std::vector<DWORD> clis;

				detail::Judge(clis, clients);

				if( clis.empty() )
					return;

				GetControlMgr().ExecExeFile(clis, path);
			}


			void ControlInstallAll(const data_helper::ClientTraits::VectorType &clients)
			{
				std::vector<DWORD> clis;

				detail::Judge(clis, clients);

				if( clis.empty() )
					return;

				GetControlMgr().InstallProt(clis, true);
			}

			void ControlInstall(const data_helper::ClientTraits::VectorType &clients)
			{
				std::vector<DWORD> clis;

				detail::Judge(clis, clients);
				
				if( clis.empty() )
					return;

				GetControlMgr().InstallProt(clis, false);
			}

			void ControlProtoArea(const data_helper::ClientTraits::VectorType &clients, const stdex::tString &protAreas, const stdex::tString &unProtAreas)
			{
				std::vector<DWORD> clis;

				detail::Judge(clis, clients);
				
				if( clis.empty() )
					return;

				GetControlMgr().ProtArea(clis, protAreas, unProtAreas);
			}

			void ControlRestore(const data_helper::ClientTraits::VectorType &clients)
			{
				std::vector<DWORD> clis;

				detail::Judge(clis, clients);

				if( clis.empty() )
					return;

				GetControlMgr().RemoveProt(clis);
			}


			void ControlViewSystem(const data_helper::ClientTraits::VectorType &clients, stdex::tString &text)
			{
				std::vector<DWORD> clis;

				detail::Judge(clis, clients);

				if( clis.empty() )
					return;

				GetControlMgr().ViewClientInfo(clis, Ctrl_ViewSysInfo, text);
			}

			void ControlViewProcess(const data_helper::ClientTraits::VectorType &clients, stdex::tString &text)
			{
				std::vector<DWORD> clis;

				detail::Judge(clis, clients);

				if( clis.empty() )
					return;

				GetControlMgr().ViewClientInfo(clis, Ctrl_ViewProcInfo, text);
			}

			void ControlViewService(const data_helper::ClientTraits::VectorType &clients, stdex::tString &text)
			{
				std::vector<DWORD> clis;

				detail::Judge(clis, clients);

				if( clis.empty() )
					return;

				GetControlMgr().ViewClientInfo(clis, Ctrl_ViewSvrInfo, text);
			}

			void ControlViewGames(const data_helper::ClientTraits::VectorType &clients)
			{

			}

			void ControlRemoteControl(const data_helper::ClientTraits::VectorType &clients)
			{
				if(clients.size() == 0)
				{
					CMessageBox msgDlg(_T("提示"),_T("请选中被控制的客户端!"));
					msgDlg.DoModal();
					return ;
				}

				stdex::tString path = utility::GetAppPath() + _T("tools\\vncviewer.exe ");
				if(PathFileExists(path.c_str()))
					GetControlMgr().RemoteControl(clients[0]->IP, true); 
				else
				{
					CMessageBox msgDlg(_T("提示"),_T("启动远程控制工具失败!"));
					msgDlg.DoModal();
					return ;
				}
			}

			void ControlRemoteView(const data_helper::ClientTraits::VectorType &clients)
			{
				stdex::tString path = utility::GetAppPath() + _T("tools\\vncviewer.exe ");
				if(PathFileExists(path.c_str()))
					GetControlMgr().RemoteControl(clients[0]->IP, false);
				else
				{
					CMessageBox msgDlg(_T("提示"),_T("启动远程控制工具失败!"));
					msgDlg.DoModal();
					return ;
				}
			}

			void ControlRefresh(const data_helper::ClientTraits::VectorType &clients)
			{

			}

			void ControlKillProcess(const stdex::tString &file,const DWORD clientIP, bool IsProcess )
			{
				GetControlMgr().KillProcSvr(file,clientIP,IsProcess);
			}

			namespace detail
			{
				struct Filter
				{
					typedef std::tr1::function<bool (const data_helper::ClientTraits::ElementType &)> Callback;

					data_helper::ClientTraits::VectorType &clients_;
					Callback callback_;
					Filter(data_helper::ClientTraits::VectorType &clients, const Callback &callback)
						: clients_(clients)
						, callback_(callback)
					{}
					void operator()(const data_helper::ClientTraits::MapType::value_type &val) const
					{ 
						if( callback_(*(val.second)) )
							clients_.push_back(val.second);
					}
				};
			}

			void AreaRealFilter(data_helper::ClientTraits::VectorType &sClients, data_helper::ClientTraits::VectorType &dClients, const stdex::tString &AID)
			{
				using namespace std::tr1::placeholders;

				// 如果本地没有，则新加
				// 如果本地已有，是选中区域，则更新
				// 如果本地已有，但是不是选中区域，则删除
				using data_helper::ClientStatusTraits;
				using data_helper::ClientTraits;

				ClientStatusTraits::MapType clientStatus = i8desk::GetRealDataMgr().GetClientStatus();

				for(ClientStatusTraits::MapConstIterator iter = clientStatus.begin();
					iter != clientStatus.end(); ++iter)
				{
					struct FindIF
					{
						const stdex::tString &name_;
						FindIF(const stdex::tString &name)
							: name_(name)
						{}

						bool operator()(const ClientTraits::VectorType::value_type &val) const
						{ return val->Name == name_; }
					};
					ClientTraits::VecIterator cliIter = std::find_if(sClients.begin(), sClients.end(), FindIF(iter->first));

					if( cliIter != sClients.end() )
					{
						if (utility::Strcmp(AID.c_str(), iter->second->AID) == 0)
							*cliIter =  iter->second;
						else
							if(!AID.empty())
								sClients.erase(cliIter);
					}
					else
					{
						if ( (AID.c_str() == iter->second->AID) )
							sClients.push_back(iter->second);
					}
				}

				// 设置离线状态
				for(ClientTraits::VecIterator iter = sClients.begin(); iter != sClients.end(); ++iter)
				{
					ClientStatusTraits::MapConstIterator statusIter = clientStatus.find((*iter)->Name);
					if( statusIter == clientStatus.end() )
						(*iter)->Online = 0;
				}

				if( AID.empty() )
				{
					dClients = sClients;
					return;
				}

				for(ClientTraits::VecConstIterator iter = sClients.begin(); iter != sClients.end(); ++iter)
				{
					if(utility::Strcmp(AID.c_str(), (*iter)->AID) == 0)
						dClients.push_back(*iter);
				}


			}

			void AreaFilter(data_helper::ClientTraits::VectorType &curClients, const stdex::tString &AID)
			{
				using namespace std::tr1::placeholders;

				// 如果本地没有，则新加
				// 如果本地已有，则更新
				using data_helper::ClientStatusTraits;
				using data_helper::ClientTraits;

				ClientTraits::MapType &clients = i8desk::GetDataMgr().GetClients();

				ClientStatusTraits::MapType clientStatus = GetRealDataMgr().GetClientStatus();
				for(ClientStatusTraits::MapConstIterator iter = clientStatus.begin();
					iter != clientStatus.end(); ++iter)
				{
					ClientTraits::MapIterator cliIter = clients.find(iter->second->Name);
					if( cliIter != clients.end() )
					{
						*(cliIter->second) = *(iter->second);
					}
					else
					{
						clients[iter->second->Name] = iter->second;
					}
				}

				// 设置离线状态
				for(ClientTraits::MapConstIterator iter = clients.begin(); iter != clients.end(); ++iter)
				{
					ClientStatusTraits::MapConstIterator statusIter = clientStatus.find(iter->second->Name);
					if( statusIter == clientStatus.end() )
						iter->second->Online = 0;
				}

				if( AID.empty() )
				{
					stdex::map2vector(clients, curClients);
				}
				else
				{
					std::for_each(clients.begin(), clients.end(), [&curClients, &AID](data_helper::ClientTraits::MapType::value_type &val)
					{
						if( val.second->AID == AID )
							curClients.push_back(val.second);
					});
				}
			}

			void PraseViewSystem( stdex::tString &text)
			{
	
			}
			
			void PraseViewProcess( std::vector<ListVector> &listVector ,const stdex::tString &Info)
			{
				CMarkup xml;
				ListVector list_;

				listVector.clear();

				xml.SetDoc(Info);
				if (xml.GetError().size() == 0)
				{
					xml.ResetPos();
					if (xml.FindElem(_T("ProcessLst")))
					{
						xml.IntoElem();
						while (xml.FindElem(_T("Process")))
						{
							stdex::tString szItem;
							stdex::ToString(_ttoi(xml.GetAttrib(_T("Memory")).c_str()) / 1024,szItem);
							list_.col0 = xml.GetAttrib(_T("Name")).c_str();
							list_.col1 = xml.GetAttrib(_T("Pid")).c_str();
							list_.col2 = szItem;

							listVector.push_back( list_ );
						}
					}				
				}
			}

			void PraseViewService( std::vector<ListVector> &listVector ,const stdex::tString &Info)
			{
				CMarkup xml;
				ListVector list_;

				listVector.clear();

				xml.SetDoc(Info);
				if (xml.GetError().size() == 0)
				{
					xml.ResetPos();
					if (xml.FindElem(_T("ServiceLst")))
					{
						xml.IntoElem();
						while (xml.FindElem(_T("Service")))
						{
							list_.col0 = xml.GetAttrib(_T("Name")).c_str();
							list_.col1 = xml.GetAttrib(_T("DisplayName")).c_str();
							list_.col2 = xml.GetAttrib(_T("Application")).c_str();

							listVector.push_back( list_ );

						}
					}				
				}
			}


			void GetUnProtArea(const CString &allAreas, const CString &protAreas, stdex::tString &unprotAreas)
			{
				stdex::tString allAreas_  = allAreas;
				stdex::tString protAreas_  = protAreas;

				struct Op
				{
					const stdex::tString &protAreas_;
					Op(stdex::tString &protAreas)
						: protAreas_(protAreas)
					{}
					bool operator ()(stdex::tString::value_type c)
					{
						return protAreas_.find(c) != stdex::tString::npos;
					}
				};
				std::remove_copy_if(allAreas_.begin(), allAreas_.end(), back_inserter(unprotAreas), Op(protAreas_));
			
			}


			void ParseProtArea(const stdex::tString &clientName,  std::vector<stdex::tString> &partiions)
			{
				using i8desk::data_helper::ClientTraits;
				const ClientTraits::MapType &allClients = i8desk::GetDataMgr().GetClients();

				ClientTraits::MapConstIterator iter = allClients.find(clientName.c_str());
				if( iter != allClients.end() )
				{
					const ClientTraits::ValueType &client = iter->second;
					stdex::tString partition = client->Partition; 
					stdex::Split(partiions, partition, _T('|'));

					// 如果无保护分区盘则为空
					if( partiions.size() != 2 )
						partiions.push_back(_T(""));
				}
			}

			void GetProtArea( const stdex::tString &Partition, stdex::tString &protarea)
			{
				std::vector<stdex::tString> Partitions; 
				stdex::Split(Partitions,Partition, _T('|'));
				if( Partitions.size() > 1)
					protarea = Partitions[1];
				else 
					protarea = _T("");
			}

		}
	}
}
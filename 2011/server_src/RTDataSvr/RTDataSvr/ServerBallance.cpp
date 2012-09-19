#include "stdafx.h"
#include "SeverBallance.hpp"

#include "tablemgr.h"

namespace i8desk
{
	namespace ballance
	{

		struct ServerInfo
		{
			stdex::tString svrID;		// 服务器ID
			ulong svrIP;				// 服务器默认IP

			std::set<ulong> updateLoadIP;	// 服务器负载客户机IP
			std::set<ulong> vDiskLoadIP;// 虚拟盘负载客户机IP

			ServerInfo()
				: svrIP(0)
			{}
		};
		bool operator<(const ServerInfoPtr &lhs, const ServerInfoPtr &rhs)
		{
			return lhs->updateLoadIP.size() < rhs->updateLoadIP.size();
		}


		namespace detail
		{
			struct FindSvrByIP
			{
				ulong ip_;
				FindSvrByIP(ulong ip)
					: ip_(ip)
				{}

				template < typename SvrsInfoT >
				bool operator()(const SvrsInfoT &svrs) const
				{
					return svrs->svrIP == ip_;
				}
			};

			struct FindSvrBySID
			{
				const stdex::tString &SID_;
				FindSvrBySID(const stdex::tString &SID)
					: SID_(SID)
				{}

				template < typename SvrsInfoT >
				bool operator()(const SvrsInfoT &svrs) const
				{
					return svrs->svrID == SID_;
				}
			};

		}
		

		// 服务器上线下线变化
		struct ServerStatusEventHandler
			: public i8desk::IServerStatusEventHandler
		{
			ServerLoadBalance::ServerInfos &svrsInfo_;
			ZTableMgr *rtData_;
			ServerStatusEventHandler(ServerLoadBalance::ServerInfos &svrsInfo, ZTableMgr *rtData)
				: svrsInfo_(svrsInfo)
				, rtData_(rtData)
			{}
			virtual uint32 GetEvents(void)
			{
				return SQL_OP_UPDATE | SQL_OP_ENSURE | SQL_OP_INSERT;
			}

			virtual void HandleEvent(uint32 optType, const TCHAR *key, uint64 mask) 
			{
				if( optType == SQL_OP_INSERT ||
					mask & MASK_TSERVERSTATUS_I8DESKSVR ||
					mask & MASK_TSERVERSTATUS_I8VDISKSVR )
				{
					if( svrsInfo_.find_if(detail::FindSvrBySID(key)) !=
						svrsInfo_.end() )
						return;

					db::tServerStatus svrStatus;
					rtData_->GetServerStatusTable()->GetData(key, &svrStatus);

					if( svrStatus.I8VDiskSvr == 1 ||
						svrStatus.I8DeskSvr == 1 )
					{
						db::tServer svr;
						if( rtData_->GetServerTable()->GetData(key, &svr) != 0 )
							return;

						ServerInfoPtr newSvr(new ServerInfo);
						newSvr->svrID		= svr.SvrID;
						newSvr->svrIP		= svr.Ip1;
						
						svrsInfo_.push_back(newSvr);
					}
					else
					{	
						svrsInfo_.erase(detail::FindSvrBySID(svrStatus.SvrID));
					}
				}
			}
		};

		ServerLoadBalance::ServerLoadBalance(ZTableMgr *rtData)
			: rtData_(rtData)
			, svrStatusHandler_(new ServerStatusEventHandler(svrInfos_, rtData_))
		{}


		void ServerLoadBalance::Start()
		{
			rtData_->GetServerStatusTable()->RegisterEventHandler(svrStatusHandler_.get());
		}
			

		void ServerLoadBalance::Stop()
		{
			rtData_->GetServerStatusTable()->UnregisterEventHandler(svrStatusHandler_.get());
		}


		void ServerLoadBalance::UpdateConnect(DWORD flag, ulong svrIP, ulong clientIP)
		{
			struct OP
			{
				ServerLoadBalance *svr_;
				DWORD flag_;
				ulong clientIP_;
				OP(ServerLoadBalance *svr, DWORD flag, ulong clientIP)
					: svr_(svr)
					, flag_(flag)
					, clientIP_(clientIP)
				{}
				void operator()(const ServerInfos::value_type &val) const
				{
					svr_->_EraseClientIP(flag_, clientIP_);

					if( flag_ == UpdateIP )
					{
						val->updateLoadIP.insert(clientIP_);
					}
					else if( flag_ == VDiskIP )
					{
						val->vDiskLoadIP.insert(clientIP_);
					}
				}
			};
			
			svrInfos_.op_if(detail::FindSvrByIP(svrIP), OP(this, flag, clientIP));
		}

		void ServerLoadBalance::UpdateDisconnect(ulong clientIP)
		{
			_EraseClientIP(UpdateIP, clientIP);
			_EraseClientIP(VDiskIP, clientIP);
		}

		std::pair<size_t, size_t> ServerLoadBalance::GetLoadCount(const stdex::tString &srvID) const
		{
			struct FindClientIPCount
			{
				size_t &updateCnt_;
				size_t &vDiskCnt_;
				const stdex::tString &svrID_;
				FindClientIPCount(size_t &updateCnt, size_t &vDiskCnt, const stdex::tString &svrID)
					: updateCnt_(updateCnt)
					, vDiskCnt_(vDiskCnt)
					, svrID_(svrID)
				{}

				bool operator()(const ServerInfoPtr &svr) const
				{
					if( svr->svrID == svrID_ )
					{
						updateCnt_ = svr->updateLoadIP.size();
						vDiskCnt_ = svr->vDiskLoadIP.size();
						return true;
					}

					return false;
				}
			};
			size_t updateCnt = 0, vDiskCnt = 0;
			svrInfos_.find_if(FindClientIPCount(updateCnt, vDiskCnt, srvID));

			return std::make_pair(updateCnt, vDiskCnt);
		}

		void ServerLoadBalance::GetServersIPs(const stdex::tString &cliName, ServersIP &serversIP)
		{
			// 先测试是否能区域优先
			ServerLoadBalance::ServerIP svrIPs = _AreaLoading(cliName);
			if( svrIPs.first == 0 )
			{
				// 平均分配
				svrIPs = _AvgLoading();
			}
			serversIP.push_back(svrIPs);

			// 动态负载
			std::tr1::shared_ptr<std::vector<ServerLoadBalance::ServerIP>> dynamicIPs = _DynamicLoading();
			for(size_t i = 0; i != dynamicIPs->size(); ++i)
			{
				if( (*dynamicIPs)[i].first != svrIPs.first )
					serversIP.push_back((*dynamicIPs)[i]);
			}
		}


		void ServerLoadBalance::_EraseClientIP(DWORD flag, ulong clientIP)
		{
			struct EraseClientIP
			{
				DWORD flag_;
				ulong clietnIP_;
				EraseClientIP(DWORD flag, ulong clientIP)
					: flag_(flag)
					, clietnIP_(clientIP)
				{}
				void operator()(ServerInfoPtr &svr)
				{
					if( flag_ == UpdateIP )
					{
						svr->updateLoadIP.erase(clietnIP_);
					}
					else if( flag_ == VDiskIP )
					{
						svr->vDiskLoadIP.erase(clietnIP_);
					}
				}
			};

			svrInfos_.for_each(EraseClientIP(flag, clientIP));
		}

		ServerLoadBalance::ServerIP ServerLoadBalance::_AvgLoading()
		{
			static size_t cnt = 0;
			size_t pos = ++cnt % svrInfos_.size();

			// 根据SID找出VDisk的IP
			const stdex::tString &svrSID = svrInfos_[pos]->svrID;
			
			IVDiskRecordset *vDiskRt = 0;
			struct Filter : public IVDiskFilter 
			{
				Filter(const stdex::tString &svrSID) : svrSID_(svrSID) {}
				bool bypass(const db::tVDisk *const d) 
				{ return svrSID_ != d->SvrID; }
			private:
				const stdex::tString &svrSID_;
			} filter(svrSID);


			std::set<ulong> vDisksIP;
			if( rtData_->GetVDiskTable()->Select(&vDiskRt, &filter, 0) == 0 )
			{
				for(size_t i = 0; i != vDiskRt->GetCount(); ++i)
				{
					const db::tVDisk *vDisk = vDiskRt->GetData(i);
					
					vDisksIP.insert(vDisk->SoucIP);
				}
				vDiskRt->Release();
			}

			db::tServer svr;
			if( rtData_->GetServerTable()->GetData(svrSID.c_str(), &svr) != 0 )
				return std::make_pair(0, vDisksIP);

			return std::make_pair(svr.SvrIP, vDisksIP);
		}

		ServerLoadBalance::ServerIP ServerLoadBalance::_AreaLoading(const stdex::tString &cliName)
		{
			// 先确定分区
			db::tClient client;
			if( rtData_->GetClientTable()->GetData(cliName.c_str(), &client) != 0 )
				utility::Strcpy(client.AID, DEFAULT_AREA_GUID);

			// 再取分区的优先服务器
			db::tArea area;
			rtData_->GetAreaTable()->GetData(client.AID, &area);

			
			ulong updateIP = 0;
			std::set<ulong> vDiskIPs;

			// 区域优先
			db::tServer areaSvr;
			if( !utility::IsEmpty(area.SvrID) ) 
			{
				rtData_->GetServerTable()->GetData(area.SvrID, &areaSvr);

				if( areaSvr.BalanceType != i8desk::BALANCE_AREAPRIORITY )
					return std::make_pair(updateIP, vDiskIPs);

				updateIP = areaSvr.SvrIP;

				IVDiskRecordset *vDiskRt = 0;
				struct Filter : public IVDiskFilter {
					Filter(TCHAR* SvrID) : m_SvrID(SvrID) {}
					bool bypass(const db::tVDisk *const d) {
						return _tcscmp(d->SvrID , m_SvrID) != 0;
					}
				private:
					TCHAR *m_SvrID;
				} filter(area.SvrID);

				if( rtData_->GetVDiskTable()->Select(&vDiskRt, &filter, 0) == 0 )
				{
					for(size_t i = 0; i != vDiskRt->GetCount(); ++i)
					{
						const db::tVDisk *vDisk = vDiskRt->GetData(i);
						if( utility::Strcmp(vDisk->SvrID, areaSvr.SvrID) == 0 )
							vDiskIPs.insert(vDisk->SoucIP);
					}
					vDiskRt->Release();
				}
			}

			return std::make_pair(updateIP, vDiskIPs);
		}

		std::tr1::shared_ptr<std::vector<ServerLoadBalance::ServerIP>> ServerLoadBalance::_DynamicLoading()
		{
			struct SortByCliCount
			{
				bool operator()(const ServerInfoPtr &lhs, const ServerInfoPtr &rhs) const
				{
					return lhs->updateLoadIP.size() + lhs->vDiskLoadIP.size() < 
						rhs->updateLoadIP.size() + rhs->vDiskLoadIP.size();
				}
			};
			svrInfos_.sort(SortByCliCount());
			

			std::tr1::shared_ptr<std::vector<ServerLoadBalance::ServerIP>> IPs(
				new std::vector<ServerLoadBalance::ServerIP>);
			struct CopyIP
			{
				ZTableMgr *rtData_;
				std::tr1::shared_ptr<std::vector<ServerLoadBalance::ServerIP>> &IPs_;
				CopyIP(ZTableMgr *rtData, std::tr1::shared_ptr<std::vector<ServerLoadBalance::ServerIP>> &ips)
					: rtData_(rtData)
					, IPs_(ips)
				{}

				void operator()(const ServerInfoPtr &lhs) const
				{
					db::tServer svr;
					if( rtData_->GetServerTable()->GetData(lhs->svrID.c_str(), &svr) != 0 )
						return;

					IVDiskRecordset *vDiskRt = 0;
					struct Filter : public IVDiskFilter {
						Filter(TCHAR* SvrID) : m_SvrID(SvrID) {}
						bool bypass(const db::tVDisk *const d) {
							return _tcscmp(d->SvrID , m_SvrID) != 0;
						}
					private:
						TCHAR *m_SvrID;
					} filter(svr.SvrID);

					ServerLoadBalance::ServerIP svrIPs;
					if( rtData_->GetVDiskTable()->Select(&vDiskRt, &filter, 0) == 0 )
					{
						for(size_t i = 0; i != vDiskRt->GetCount(); ++i)
						{
							const db::tVDisk *vDisk = vDiskRt->GetData(i);
							if( utility::Strcmp(vDisk->SvrID, svr.SvrID) == 0 )
								svrIPs.second.insert(vDisk->SoucIP);
						}
						vDiskRt->Release();
					}

					svrIPs.first = svr.SvrIP;
					IPs_->push_back(svrIPs);
				}
			};
			svrInfos_.for_each(CopyIP(rtData_, IPs));
			
			return IPs;
		}
	}
}

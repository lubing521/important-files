#include "stdafx.h"
#include "MonitorBusiness.h"

#include "../../../../include/frame.h"
#include "../../../../include/MultiThread/Lock.hpp"
#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../ManagerInstance.h"
#include "../Misc.h"
#include "../LogHelper.h"

#include <functional>
#include <algorithm>

namespace i8desk
{
	namespace business
	{
		namespace monitor
		{
			
			typedef std::vector<i8desk::LogRecordPtr> LogRecords;

			class LogRecordHelper
			{
				enum { MAX_SIZE = 5000 };

				typedef async::thread::AutoCriticalSection Mutex;
				typedef async::thread::AutoLock<Mutex> AutoLock;

				bool isDebug_;

				mutable Mutex mutex_;
				LogRecords totalLogs_;
				LogRecords errorLogs_;

			public:
				LogRecordHelper()
#ifdef _DEBUG
					: isDebug_(true)
#else
					:isDebug_(false)
#endif
				{
					totalLogs_.reserve(MAX_SIZE);
					errorLogs_.reserve(MAX_SIZE);
				}

			private:
				void _Adjust(LogRecords &logs)
				{
					AutoLock lock(mutex_);

					if( logs.size() > MAX_SIZE )
					{
						LogRecords::iterator iter = logs.begin();
						std::advance(iter , MAX_SIZE / 2);

						logs.erase(iter, logs.end());
					}
				}

			public:
				void PushRecord(const LogRecordPtr &log)
				{
					AutoLock lock(mutex_);

					if( log->type == i8desk::LM_WARNING )	
						log->clr_ = RGB(200, 200, 0);
					else if( log->type == i8desk::LM_ERROR )
						log->clr_ = RGB(255, 0, 0);
					else
						log->clr_ = RGB(0, 0, 0);

					totalLogs_.push_back(log);
					_Adjust(totalLogs_);

					
					if( log->type > i8desk::LM_DEBUG )
					{
						errorLogs_.push_back(log);
						_Adjust(errorLogs_);
					}
				}

				void ChangeDebugMode()
				{
					isDebug_ = !isDebug_;
				}

				void GetLog(size_t index, i8desk::LogRecord &log)
				{
					AutoLock lock(mutex_);
					if( !isDebug_ )
					{
						LogRecords::reverse_iterator iter = errorLogs_.rbegin();
						std::advance(iter, index);
						log = *(*iter);
					}
					else
					{
						LogRecords::reverse_iterator iter = totalLogs_.rbegin();
						std::advance(iter, index);
						log = *(*iter);
					}
				}

				size_t GetLogCount() const
				{
					AutoLock lock(mutex_);
					if( !isDebug_ )
					{
						return errorLogs_.size();
					}
					else
					{
						return totalLogs_.size();
					}
				}

			}logRecords;



			void PushLog(const LogRecordPtr &log)
			{
				logRecords.PushRecord(log);
			}


			// 改变Debug 模式
			void ChangeDebugMode()
			{
				logRecords.ChangeDebugMode();
			}

			// 获取出错消息
			void GetLogRecord(size_t index, i8desk::LogRecord &log)
			{
				logRecords.GetLog(index, log);
			}

			size_t GetLogCount()
			{
				return logRecords.GetLogCount();
			}


			struct ExceptInfo
			{
				UINT mask_;
				stdex::tString time_;
				db::TClientPtr info_;
				stdex::tString name_;
				stdex::tString ip_;
				stdex::tString desc_;
			};
			typedef std::vector<ExceptInfo> ExceptInfos;

			class ExceptInfoHelper
			{
				typedef async::thread::AutoCriticalSection Mutex;
				typedef async::thread::AutoLock<Mutex> AutoLock;

				mutable Mutex mutex_;
				ExceptInfos infos_;

				enum { MAX_COUNT = 2000 };

			public:
				ExceptInfoHelper()
				{}

			private:
				template < typename ClientT >
				stdex::tString GetValue(const ClientT &client, UINT srcMask, UINT destMask)
				{
					switch(srcMask & destMask)
					{
					case MASK_TCLIENT_SYSTEM:
						return client->System;
					case MASK_TCLIENT_CPU:
						return client->CPU;
					case MASK_TCLIENT_MAINBOARD:
						return client->Mainboard;
					case MASK_TCLIENT_MEMORY:
						return client->Memory;
					case MASK_TCLIENT_DISK:
						return client->Disk;
					case MASK_TCLIENT_VIDEO:
						return client->Video;
					case MASK_TCLIENT_AUDIO:
						return client->Audio;
					case MASK_TCLIENT_NETWORK:
						return client->Network;
					case MASK_TCLIENT_CAMERA:
						return client->Camera;
					case MASK_TCLIENT_TEMPERATURE:
						return stdex::ToString<stdex::tString>(client->Temperature);
					default:
						return _T("无效");
					}
				}

			public:
				void Push(const std::tr1::tuple<db::TClientPtr, DWORD> &except)
				{
					static struct
					{
						int mask_;
						LPCTSTR name_;
					}clientMask[] = 
					{
						{ MASK_TCLIENT_SYSTEM,		_T("[系统信息]") },
						{ MASK_TCLIENT_CPU,			_T("[CPU]") },
						{ MASK_TCLIENT_MAINBOARD,	_T("[主板]") },
						{ MASK_TCLIENT_MEMORY,		_T("[内存]") },
						{ MASK_TCLIENT_DISK,		_T("[硬盘]") },
						{ MASK_TCLIENT_VIDEO,		_T("[显卡]") },
						{ MASK_TCLIENT_AUDIO,		_T("[声卡]") },
						{ MASK_TCLIENT_NETWORK,		_T("[网卡]") },
						{ MASK_TCLIENT_CAMERA,		_T("[摄像机]") },
						{ MASK_TCLIENT_TEMPERATURE, _T("[CPU温度]")},
					};


					ExceptInfo info;
					info.time_ = i8desk::FormatTime(::time(0));
					info.mask_ = std::tr1::get<1>(except);
					info.info_ = std::tr1::get<0>(except);
					info.name_ = info.info_->Name;
					info.ip_   = i8desk::IP2String(info.info_->IP);	

					
					for(size_t i = 0; i != _countof(clientMask); ++i)
					{
						stdex::tOstringstream os;
						os << _T("客户机硬件异常: ");

						if( info.mask_ & clientMask[i].mask_ )
						{
							data_helper::ClientTraits::MapConstIterator iter = GetDataMgr().GetClients().find(info.info_->Name);
							if( iter != GetDataMgr().GetClients().end() )
							{
								os << clientMask[i].name_ << _T(" ")
									<< GetValue(iter->second, info.mask_, clientMask[i].mask_)
									<< _T("-->")
									<< GetValue(info.info_, info.mask_, clientMask[i].mask_);

								info.desc_   = os.str();

								AutoLock lock(mutex_);
								if( infos_.size() < MAX_COUNT )
									infos_.push_back(info);
							}
							else
							{
								Log() << _T("客户机硬件异常,没有找到对应客户机");
							}
						}
					}
					
				}

				std::tr1::tuple<stdex::tString, stdex::tString, stdex::tString, stdex::tString > Get(size_t index) const
				{
					AutoLock lock(mutex_);
					if( index > infos_.size() )
					{
						static stdex::tString err;
						return std::tr1::make_tuple(err, err, err, err);
					}

					return std::tr1::make_tuple(infos_[index].name_, infos_[index].ip_, infos_[index].time_, infos_[index].desc_);
				}

				size_t Size() const
				{
					AutoLock lock(mutex_);
					return infos_.size();
				}

				void Clear()
				{
					AutoLock lock(mutex_);
					infos_.clear();
				}

				std::pair<bool, UINT> Has(const stdex::tString &name, const stdex::tString &ip) const
				{
					AutoLock lock(mutex_);
					for(ExceptInfos::const_iterator iter = infos_.begin(); iter != infos_.end(); ++iter)
					{
						if( iter->name_ == name && iter->ip_ == ip )
							return std::make_pair(true, iter->mask_);
					}

					return std::make_pair(false, 0);
				}

			}exceptInfos;

			void PushExcept(const std::tr1::tuple<data_helper::ClientTraits::ValueType, DWORD> &exceptInfo)
			{
				exceptInfos.Push(exceptInfo);
			}

			// 获取异常信息
			std::tr1::tuple<stdex::tString, stdex::tString, stdex::tString, stdex::tString> GetExceptInfo(size_t index)
			{
				return exceptInfos.Get(index);
			}

			// 获取异常消息总个数
			size_t GetExceptCount()
			{
				return exceptInfos.Size();
			}

			std::pair<bool, UINT> IsException(const stdex::tString &name, const stdex::tString &ip)
			{
				return exceptInfos.Has(name, ip);
			}

			// 清除异常
			void ClearException()
			{
				return exceptInfos.Clear();
			}


		}
	}
}
#ifndef __TASK_ITEM_HELPER_HPP
#define __TASK_ITEM_HELPER_HPP




// 队列操作仿函数
namespace i8desk
{

	namespace task_helper
	{

		// --------------------------------------------
		// class ProductTaskItem

		// 产生任务后所需要的属性信息
		struct ProductTaskItem
		{
			long	gid_				;	// gid
			long	normalGID_			;	// 如果为强推压缩包，则此为正常游戏GID
			DWORD	idcClick_			;	// 中心点击率
			DWORD	idcVer_				;	// 中心版本号
			DWORD	svrVer_				;	// 网吧版本号
			DWORD	forceRepair_		;	// 强制修复
			DWORD	pushTask_			;	// 强推
			DWORD	sender_				;	// 强修发送者
			TaskType taskType_			;	// 任务类型

			TCHAR svrPath_[MAX_PATH]	;	// 保存在服务器路径
			TCHAR mscPath_[MAX_PATH]	;	// 三层临时目录

			ProductTaskItem()
			{
				memset(this, 0, sizeof(*this));
			}

			ProductTaskItem(long gid, long normalGID, DWORD click, DWORD idcVer, DWORD svrVer, DWORD force,
				DWORD push, DWORD sender, TaskType taskType
				, const TCHAR *svrPath, const TCHAR *mscPath)
				: gid_(gid), normalGID_(normalGID), idcClick_(click), idcVer_(idcVer)
				, svrVer_(svrVer), forceRepair_(force), pushTask_(push), sender_(sender), taskType_(taskType)
			{
				std::uninitialized_fill_n(svrPath_, _countof(svrPath_), 0);
				std::uninitialized_fill_n(mscPath_, _countof(mscPath_), 0);

				std::copy(svrPath, svrPath + _tcslen(svrPath), svrPath_);
				std::copy(mscPath, mscPath + _tcslen(mscPath), mscPath_);
			}
		};

		// 支持序列化
		inline std::ostream &operator<<(std::ostream &os, const ProductTaskItem &t)
		{
			os.write(reinterpret_cast<const char *>(&t), sizeof(ProductTaskItem));

			return os;
		}

		inline std::istream &operator>>(std::istream &in, ProductTaskItem &t)
		{
			in.read(reinterpret_cast<char *>(&t), sizeof(ProductTaskItem));

			return in;
		}

		

		// -------------------------------------------
		struct TaskFind
		{
			long gid_;

			TaskFind(long gid)
				: gid_(gid)
			{}


			// 实际使用时的比较
			template<typename TaskPtrT>
			bool operator()(const TaskPtrT &item) const
			{
				return item->GetGid() == gid_;
			}

			// 为了在debug模式下编译通过
			template<typename TaskPtrT>
			bool operator()(const TaskPtrT &item, const long gid) const
			{
				return item->GetGid() == gid;
			}

			// 为了在debug模式下编译通过
			template<typename TaskPtrT>
			bool operator()(const TaskPtrT &lhs, const TaskPtrT &rhs) const
			{
				return lhs->GetGid() == rhs->GetGid();
			}
		};



		// ---------------------------------------------
		struct TaskRemove
		{
			long gid_;

			TaskRemove(long gid)
				: gid_(gid)
			{}

			template<typename TaskPtrT>
			bool operator()(const TaskPtrT &rhs) const
			{
				if( gid_ == rhs->GetGid() )
				{
					rhs->SetDelete();
					return true;
				}
				else
					return false;
			}
		};


		// ---------------------------------------------

		struct Compare
		{
			template < typename ValT >
			bool operator()(const ValT &lhs, const ValT &rhs) const
			{
				return lhs->GetIdcClick() > rhs->GetIdcClick();
			}
		};

		struct TaskSort
		{
			// 如果手动更改下载优先级, 则按照中心点击率
			template< typename QueueT >
			void operator()(QueueT &queue, size_t max)
			{
				typename QueueT::iterator beg;
				typename QueueT::iterator iter = queue.begin();
				std::advance(iter, max);
				for( ; iter != queue.end(); ++iter)
				{
					if( (*iter)->GetManualAdjust() )
						continue;

					beg = iter;
					break;
				}
				
				std::stable_sort(beg, queue.end(), Compare());
			}
		};

		// -----------------------------------------------
		struct TaskStop
		{
			template<typename TaskPtrT>
			void operator()(const TaskPtrT &rhs) const
			{
				rhs->Stop();
			}
		};


		// --------------------------------------------
		template<typename FuncT>
		struct TaskOperate
		{
			long gid_;
			FuncT func_;

			TaskOperate(long gid, FuncT func)
				: gid_(gid)
				, func_(func)
			{}

			template<typename TaskPtrT>
			bool operator()(const TaskPtrT &rhs) const
			{
				if( gid_ == rhs->GetGid() )
				{
					(rhs.get()->*func_)();
					return true;
				}
				else
					return false;
			}
		};

		template<typename FuncT>
		inline TaskOperate<FuncT> MakeTaskOperate(long gid, FuncT func)
		{
			return TaskOperate<FuncT>(gid, func);
		}



		template<typename TaskPtrT, typename ContainerT>
		inline void PushTaskInfo(const TaskPtrT &taskItem, ContainerT *container)
		{
			TaskInfo info;
			info.gid_		= taskItem->GetGid();
			info.state_		= taskItem->GetTaskState();
			info.type_		= taskItem->GetTaskType();

			const MSC_DOWNLOAD_PROGRESS &progress = taskItem->GetPdp();
			info.updateSize_	= taskItem->GetUpdateSize();
			info.bytesTotal_	= progress.BytesTotal;
			info.bytesTransferred_	= progress.BytesTransferred;
			info.transferRate_	= progress.TransferRate;
			info.timeLeft_		= progress.TimeLeft;
			info.timeElapsed_	= progress.TimeElapsed;
			info.progress_		= progress.Progress;
			info.downloadState_	= progress.State;
			info.connections_	= progress.Connections;

			//Log(LM_INFO,_T("BytesTotal = %I64u , BytesTransferred = %I64u  "),progress.BytesTotal,progress.BytesTransferred);

			container->push_back(info);
		}
	}
	
}






#endif
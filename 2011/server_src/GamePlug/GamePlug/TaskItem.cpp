#include "stdafx.h"
#include "TaskItem.h"

#include "LogHelper.h"
#include "PushGamePolicy.hpp"


namespace i8desk
{


	int CALLBACK TaskItem::OnMscProgress(UINT step, UINT code, void *presult, void *puser)
	{
		TaskItem *item = reinterpret_cast<TaskItem *>(puser);
		return item->_OnMscProgress(step, code, presult);
	}

	int TaskItem::_OnMscProgress(UINT step, UINT code,void *presult)
	{
		_PROGRESS_RESULT *pr = reinterpret_cast<_PROGRESS_RESULT*>(presult);

		{
			AutoLock lock(mutex_);

			step_ = step;

			switch (step_) 
			{
			case MSCPROGRESS_COMPOSE: 
				{
					code_ = code;
					if ( code_ == MSCPROGRESS_STEP_SCAN ) 
					{
						ull0_ = pr->ull0;
						ull1_ = pr->ull1;
					}
					break; 
				}
			case MSCPROGRESS_HASHMAP: 
				{
					taskState_ = tsHashmap;
					downloadProgress_.Progress = code;
					break; 
				}
			case MSCPROGRESS_COPYFILE: 
				{
					taskState_ = tsCopyfile;
					break; 
				}
			case MSCPROGRESS_FINISHED: 
				{
					ull0_ = pr->ull0;
					ull1_ = pr->ull1;
					updateSize_ = ull1_ - ull0_;
					checkComplete_ = true;
					memset(&downloadProgress_, 0, sizeof(downloadProgress_));
					break; 
				}
			case MSCPROGRESS_COMMIT:
				{
					taskState_ = tsCommit;
					PushGameStatics pushGameStatics(rtSvr_);
					pushGameStatics.Run();
					break;

				}
			case MSCPROGRESS_ERROR: 
				{
					code_ = code;//错误代码
					Log(LM_ERROR, _T("Lava回调中返回错误,错误代码%d,%s\n"),
						code, CW2T(static_cast<wchar_t *>(presult)));
					break; 
				}
			default:
				return 0;
			}
		}

		return 1;
	}
}
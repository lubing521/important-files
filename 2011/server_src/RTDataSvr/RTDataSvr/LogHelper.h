#ifndef __I8DESK_LOG_HELPER_HPP
#define __I8DESK_LOG_HELPER_HPP

#include "../../../include/frame.h"

namespace i8desk
{
	
	namespace log
	{
		// ----------------------------------------------------
		// class LogHelper

		class LogHelper
		{
		public:
			i8desk::ILogger *log_;

			 LogHelper(i8desk::ISvrPlugMgr	*plugMgr);
				
			~LogHelper();
			
			i8desk::ILogger *GetLogger()
			{
				assert(log_!=0);
				return log_;
			}

			static LogHelper &GetInstance(i8desk::ISvrPlugMgr *plugMgr = 0)
			{
				static LogHelper log(plugMgr);
				return log;
			}

	/*	private:
			LogHelper();*/
		};

	}
	
	void Log(size_t priority, LPCTSTR lpszText, ...);
	
}



#endif
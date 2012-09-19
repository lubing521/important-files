#ifndef __CONSOLE_LOG_RECORD_HPP
#define __CONSOLE_LOG_RECORD_HPP


namespace i8desk
{
	struct LogRecord 
	{
		int time;
		int type;
		COLORREF clr_;
		TCHAR data[1024];
		TCHAR plugin[MAX_PATH];
	};

	typedef std::tr1::shared_ptr<LogRecord> LogRecordPtr;

}

#endif
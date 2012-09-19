#ifndef __PARSE_PL_HPP
#define __PARSE_PL_HPP

#include "ThreadPoolHelper.h"


namespace i8desk
{
	class IconHelper;

	// ---------------------------------------
	// class Parse PL

	class ParsePL
	{
	private:
		IRTDataSvr *rtData_;
		ulonglong version_;			// 版本号
		IconHelper *iconHelper_;	// 图标处理
		volatile long isExit_;

	public:
		ParsePL(IRTDataSvr *rtData, IconHelper *iconHelper);
	
	private:
		ParsePL(const ParsePL &);
		ParsePL &operator=(const ParsePL &);

	public:
		void Parse(LPCTSTR data);
		void Stop();

	private:
		void _ParseImpl(LPCTSTR data);
	};
}



#endif
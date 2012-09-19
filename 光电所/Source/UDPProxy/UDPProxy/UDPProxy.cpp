// UDPProxy.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "ProxySvr.hpp"
#include <iostream>

#include <atlbase.h>
#include <atlconv.h>

#include "../../Common/Utility.hpp"
#include "../../../include/Win32/System/except.h"

int _tmain(int argc, _TCHAR* argv[])
{
	except::MiniDump dump;

	proxy::ProxySvr svr;
	svr.ProcessCmdLine(argc, argv);

	return 0;
}


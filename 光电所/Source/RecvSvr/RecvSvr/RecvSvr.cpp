// RecvSvr.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "RecvSvr.hpp"


int _tmain(int argc, _TCHAR* argv[])
{
	recvSvr::RecvSvr recvSvr;

	recvSvr.ProcessCmdLine(argc, argv);

	//recvSvr.Start();

	//system("pause");	
	//recvSvr.Stop();

	return 0;
}


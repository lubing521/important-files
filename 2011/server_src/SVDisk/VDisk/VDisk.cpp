// VDisk.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "impl/NetworkLayer/NetworkService.h"



int _tmain(int argc, _TCHAR* argv[])
{
	i8desk::networkimpl::NetworkService::GetInstance();

	
	system("pause");
	return 0;
}


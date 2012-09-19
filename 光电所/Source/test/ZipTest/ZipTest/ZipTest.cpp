// ZipTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../../../../include/Zip/ZipHelper.hpp"

#include "../../../../include/Zip/ZipStream.hpp"
#pragma comment(lib, "zlib1.lib")

int _tmain(int argc, _TCHAR* argv[])
{
	/*HZIP hz = CreateZip(_T("bmp.zip"),0,ZIP_FILENAME);
	ZipAdd(hz, _T("test1.bmp"),  _T("test1.bmp"), 0, ZIP_FILENAME);
	CloseZip(hz);*/

	{
		zip::OZipStream out("test.zip");
		out << "chenyu" << std::endl;
	}

	{
		zip::IZipStream in("test.zip");
		char buf[64] = {0};
		in >> buf;
		std::cout << in.rdbuf() << std::endl;
	}


	return 0;
}


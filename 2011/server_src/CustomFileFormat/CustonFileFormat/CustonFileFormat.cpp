// CustonFileFormat.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "CustomFileImpl.h"
#include <iostream>


#include "../../../include/UnitTest/tut.hpp"
#include "../../../include/UnitTest/tut_console_reporter.hpp"
#include "../../../include/UnitTest/tut_cppunit_reporter.hpp"


namespace tut
{
	struct Test;

	typedef TestGroup<Test, 10> CustomGroup;
	typedef CustomGroup::ObjectType	TestType;
	CustomGroup group(_T("Custom Format"));


	struct Test
	{
		custom_file::CustomFileImpl file_;

		Test()
			: file_(_T("custom.dat"))
		{
			//file_.LoadFromFile();
		}
		~Test()
		{		
			//file_.SaveToFile();
		}
	};

	template<>
	template<>
	void TestType::Test<1>()
	{
		SetTestName(L"Test 1");


		file_.AddFile(10, _T("ReadMe.txt"));
		EnsureEqual(file_.GetIndex().size(), 1, L"ReadMe.txt");

		file_.AddFile(11, _T("CustomFileImpl.h"));
		EnsureEqual(file_.GetIndex().size(), 2, L"CustomFileImpl.h");

		file_.AddFile(12, _T("CustonFileFormat.vcproj"));
		EnsureEqual(file_.GetIndex().size(), 3, L"CustonFileFormat.vcproj");

		file_.AddFile(14, _T("CustonFileFormat.cpp"));
		EnsureEqual(file_.GetIndex().size(), 4, L"CustonFileFormat.cpp");

		file_.AddFile(13, _T("CustomFileSerialize.h"));	
		EnsureEqual(file_.GetIndex().size(), 5, L"CustomFileSerialize.h");
	}


	template<>
	template<>
	void TestType::Test<2>()
	{
		SetTestName(L"Test 2");

		file_.AddFile(10, _T("ReadMe.txt"));
		file_.AddFile(11, _T("CustomFileImpl.h"));
		file_.AddFile(12, _T("CustonFileFormat.vcproj"));

		file_.DeleteFile(10);
		EnsureLessEqual(file_.GetIndex().size(), 2, L"删除掉一个");

		file_.DeleteFile(11);
		EnsureLessEqual(file_.GetIndex().size(), 1, L"删除掉两个");

		file_.DeleteFile(12);
		EnsureLessEqual(file_.GetIndex().size(), 0, L"删除掉三个");
	}


	template<>
	template<>
	void TestType::Test<3>()
	{
		SetTestName(L"Test 3");

		file_.AddFile(10, _T("ReadMe.txt"));
		file_.AddFile(11, _T("CustomFileImpl.h"));
		file_.AddFile(12, _T("CustonFileFormat.vcproj"));

		file_.ModifyFile(10, _T("stdafx.cpp"));
		Ensure(file_.GetFilesInfo().find(10) != file_.GetFilesInfo().end());

		file_.ModifyFile(11, _T("ReadMe.txt"));
		Ensure(file_.GetFilesInfo().find(11) != file_.GetFilesInfo().end());
	
	}


	template<>
	template<>
	void TestType::Test<4>()
	{
		SetTestName(L"Test 4");

		file_.AddFile(10, _T("ReadMe.txt"));
		file_.AddFile(11, _T("CustomFileImpl.h"));

		const custom_file::FileInfoPtr &file = file_.GetData(10);
		Ensure(file->gid_ == 10, _T("GID"));

		const custom_file::FileInfoPtr &file1 = file_.GetData(11);
		Ensure(file1->gid_ == 11, _T("GID"));
	}	

}



int _tmain(int argc, TCHAR* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);  // _CRTDBG_REPORT_FLAG

	
	tut::ConsoleReporter reporter;
	tut::CPPUnitReporter cppReporter;

	tut::RunnerInstance().SetCallback(&reporter);
	tut::RunnerInstance().InsertCallback(&cppReporter);

	tut::RunnerInstance().RunTests();

	

	system("pause");
	return 0;
}


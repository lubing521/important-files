// DatabaseChange.cpp : 定义控制台应用程序的入口点。
//
#pragma once

#include "stdafx.h"
#include "../../../include/frame.h"
#include "replaceDatabase.h"
#include "IconChange.h"
#include "CreatDatabase.h"
#include "../../../include/Win32/System/except.h"

#include "../../../include/Utility/utility.h"
#include "../../../include/Extend STL/StringAlgorithm.h"
#include <vector>

#pragma comment(lib, "frame.lib")

using namespace i8desk;
int _tmain(int argc, _TCHAR* argv[])
{
	except::MiniDump miniDump;
	ILogger*	m_pLogger;

	CreateLogger(&m_pLogger);		
	m_pLogger->SetLogFileName(_T("DatabaseChang"));
	m_pLogger->SetAddDateBefore(false);
	m_pLogger->WriteLog(LM_INFO, TEXT("============================="));
	m_pLogger->SetAddDateBefore(true);

	m_pLogger->WriteLog(LM_INFO, TEXT("start create database."));
	stdex::tString filepath_ , newdatapath_, olddatapath_;
	std::vector<stdex::tString> vctParam;

	if(argv[1] != NULL)
	{
		filepath_ = argv[1];
		if(filepath_.empty()) 
			return false; 

		m_pLogger->WriteLog(LM_INFO, TEXT("filepath_ = %s."), filepath_.c_str());

		stdex::Split(vctParam, filepath_ , _T('|'));

		newdatapath_ = vctParam[1] + _T("\\data\\i8desk2011.mdb");
		m_pLogger->WriteLog(LM_INFO, TEXT("newdatapath_ = %s."), newdatapath_.c_str());

		olddatapath_ = vctParam[0] + _T("\\data\\i8desk.mdb");
		m_pLogger->WriteLog(LM_INFO, TEXT("olddatapath_ = %s."), olddatapath_.c_str());
	}
	else
	{	
		filepath_ = utility::GetAppPath();
		newdatapath_ = filepath_ + _T("data\\i8desk2011.mdb");
		m_pLogger->WriteLog(LM_INFO, TEXT("newdatapath_ = %s."), newdatapath_.c_str());

		olddatapath_ = filepath_ + _T("data\\i8desk.mdb");
		m_pLogger->WriteLog(LM_INFO, TEXT("olddatapath_ = %s."), olddatapath_.c_str());

	}
	

	if(PathFileExists(newdatapath_.c_str()))
	{ 
		m_pLogger->WriteLog(LM_INFO, TEXT("存在2011版的i8desk，不用做数据库升级转换."));
		m_pLogger->Release();
		m_pLogger = 0;
		delete m_pLogger;

		return true;
	}


	try
	{	
		if(PathFileExists(olddatapath_.c_str()))
		{
			m_pLogger->WriteLog(LM_INFO, TEXT("存在2010版的i8desk，做数据库升级转换."));
			try
			{
				CCreatDatabase creatdatabase(m_pLogger, newdatapath_);
				creatdatabase.start();
				creatdatabase.stop();
			}
			catch(...)
			{
				if(PathFileExists(newdatapath_.c_str()))
				{
					if(DeleteFile(newdatapath_.c_str()))
						m_pLogger->WriteLog(LM_INFO, TEXT("删除错误数据库成功."));
				}

				m_pLogger->WriteLog(LM_INFO, TEXT("新数据库未创建成功."));
				m_pLogger->Release();
				m_pLogger = 0;
				delete m_pLogger;


				return false;
			}

			m_pLogger->WriteLog(LM_INFO, TEXT("开始进行数据库升级转换."));
			{
				CReplaceDatabase replacedatabse(m_pLogger, newdatapath_, olddatapath_);
				replacedatabse.start();
				replacedatabse.stop();
			}
			stdex::tString newIconpath,oldIconpath;
			if(argv[1] != NULL)
			{
				newIconpath = vctParam[1] + _T("\\DATA\\Icon");
				oldIconpath = vctParam[0] + _T("\\DATA\\Icon");
			}
			else
			{
				newIconpath = filepath_ + _T("DATA\\Icon");
				oldIconpath = filepath_ + _T("DATA\\Icon");
			}


			{
				CReplaceIcon iconchange(m_pLogger,newIconpath, oldIconpath);
				iconchange.start();
				iconchange.stop();
			}

			m_pLogger->Release();
			m_pLogger = 0;
			delete m_pLogger;

			return true;
		}
	}
	catch (...)
	{
		m_pLogger->WriteLog(LM_INFO, TEXT("数据库升级转换错误."));
		m_pLogger->Release();
		m_pLogger = 0;
		delete m_pLogger;

		return false;
	}
	try
	{	
		CCreatDatabase creatdatabase(m_pLogger, newdatapath_);
		creatdatabase.start();
		creatdatabase.stop();
	}
	catch (...)
	{
		if(PathFileExists(newdatapath_.c_str()))
		{
			if(DeleteFile(newdatapath_.c_str()))
				m_pLogger->WriteLog(LM_INFO, TEXT("删除错误数据库成功."));
		}

		m_pLogger->WriteLog(LM_INFO, TEXT("新数据库未创建成功."));
		m_pLogger->Release();
		m_pLogger = 0;
		delete m_pLogger;


		return false;
	}

	m_pLogger->Release();
	m_pLogger = 0;
	delete m_pLogger;

	return true;
}



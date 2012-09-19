#include "stdafx.h"
#include "TaskCompletor.h"

#include "../../../include/UpdateGame.h"
#include "../../../include/Win32/FileSystem/FileHelper.hpp"

#include "TaskItem.h"
#include "LogHelper.h"
#include "DataSvrHelper.h"

#include "ZXML.h"
#include "GlobalFun.h"
#include "GameTableCache.h"
#include "PLInfo.h"


namespace i8desk
{
	
	namespace 
	{
		void GetFileListFromXml(I8XML::ZXml& xml, const stdex::tString &strGamePath, 
			utility::StringHashSet &vFileList, utility::StringHashSet &vDirList)
		{
			//1.处理文件
			xml.SetExpression(_T("./f"));

			//2.处理目录
			long lCount;

			lCount=xml.GetItemCount();
			for (long i=0;i<lCount;i++)
			{
				stdex::tString val;
				xml[i].GetAttribute(NULL, _T("n"), val);
				stdex::ToLower(val);

				val = strGamePath + _T("\\") + val;
				vFileList.insert(val);
			}

			xml.SetExpression(_T("./d"));

			lCount=xml.GetItemCount();
			for(long i=0; i<lCount; i++)
			{
				stdex::tString val;
				xml[i].GetAttribute(NULL, _T("n"), val);
				stdex::ToLower(val);

				val = strGamePath + _T("\\") + val;
				vDirList.insert(val);
				GetFileListFromXml(xml[i], val, vFileList, vDirList);
			}
		}
	}



	TaskCompletor::TaskCompletor(const TaskItemPtr &taskItem, 
		ISvrPlugMgr *plugMgr, IRTDataSvr *rtDatasvr, ISysOpt *sysOpt, async::thread::AutoEvent &exit)
		: taskItem_(taskItem)
		, plugMgr_(plugMgr)
		, rtDataSvr_(rtDatasvr)
		, sysOpt_(sysOpt)
		, exit_(exit)
	{
	}

	TaskCompletor::~TaskCompletor()
	{
	}


	void TaskCompletor::Run()
	{
		_RunImpl();	
	}

	void TaskCompletor::Clear()
	{
		_ClearImpl();
	}

	void TaskCompletor::Stop()
	{
	}


	void TaskCompletor::_ReplaceIndex(const stdex::tString &oldIndex)
	{
		// 复制新索引文件
		std::ofstream out(oldIndex.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
		if( !out )
		{
			std::string info = "复制索引文件[";
			info += CT2A(oldIndex.c_str());
			info += "]失败";

			throw std::runtime_error(info);
		}

		out.write((char *)taskItem_->GetSeed().first, taskItem_->GetSeed().second);
	}

	void TaskCompletor::_DeleteOldFiles(const stdex::tString &gamePath)
	{
		// 防止目录设置错误导致将其他游戏删除而做的防范
		int nPathLevel = std::count_if(gamePath.begin(), gamePath.end(), 
			std::bind2nd(std::equal_to<TCHAR>(), _T('\\')));
		if( nPathLevel < 3 ) 
		{
			Log(LM_WARNING, 
				_T("游戏[%u]下载目录[%s]小于两级, 禁止删除其他目录及文件!")
				_T("这通常意味着游戏下载目录不妥，请修改设置后重新下载.\n")
				, taskItem_->GetGid(), gamePath.c_str());
			return;
		}

		// 游戏的目录名等于类别名或游戏的路径与类别的路径设置相同，不删
		stdex::tString strGameDir = utility::ExtractLastDir(gamePath);

		struct Filter 
			: public IClassFilter 
		{
			Filter(const stdex::tString& strGameDir, const stdex::tString& strGamePath)
				: m_strGameDir(strGameDir)
				, m_strGamePath(strGamePath) 
			{}

			bool bypass(const db::tClass *const d)
			{
				return m_strGameDir != d->Name
					&& m_strGamePath != d->SvrPath;
			}
		private:
			const stdex::tString &m_strGameDir;
			const stdex::tString &m_strGamePath;
		} filter(strGameDir, gamePath);

		IClassRecordset *pRecordset = 0;
		rtDataSvr_->GetClassTable()->Select(&pRecordset, &filter, 0);

		uint32 n = pRecordset->GetCount();
		pRecordset->Release();

		if( n > 0 ) 
		{
			Log(LM_ERROR, _T("游戏路径与类别路径冲突，放弃删除多余文件.游戏:%u,路径:%s"), 
				taskItem_->GetGid(), gamePath.c_str());
			return;
		}
 
		char *indexData = (char *)taskItem_->GetSeed().first;

		utility::tagFileHeader *pfh=(utility::tagFileHeader *)(indexData);
		indexData[(pfh->CrcBlockNum+1)*32+pfh->dwXmlSize]	= 0;
		indexData[(pfh->CrcBlockNum+1)*32+pfh->dwXmlSize+1]	= 0;

		I8XML::ZXml xml,xmld;
		std::wstring strUnicode = CA2W(&indexData[(pfh->CrcBlockNum+1)*32], CP_UTF8);
		stdex::tString strUtf8 = CW2T(strUnicode.c_str());

		bool bret = xml.LoadFromMemory(strUtf8.c_str());
		xml.createProcessingInstruction();
		xml.GetValue(_T("d"), xmld);

		stdex::tString gamePathTmp = gamePath;
		gamePathTmp = gamePathTmp.substr(0, gamePathTmp.size()-1);
		stdex::tString strGamePath(gamePathTmp.length(), 0);
		std::transform(gamePathTmp.begin(), gamePathTmp.end(), strGamePath.begin(), ::tolower);

		utility::StringHashSet vXmlFileList;
		utility::StringHashSet vXmlDirList;
		GetFileListFromXml(xmld, strGamePath.c_str(), vXmlFileList,vXmlDirList);

		std::vector<stdex::tString> vFileList;
		std::vector<stdex::tString> vDirList;
		std::vector<ULONGLONG> vFileSizeList;
		if(utility::GetFileTreeList2(strGamePath.c_str(),vFileList,vDirList,vFileSizeList))
		{
			size_t nFileList = vFileList.size();
			for (size_t i = 0; i < nFileList; i++) 
			{
				const stdex::tString& file = vFileList[i];
				if (vXmlFileList.find(file) == vXmlFileList.end())
				{
					LPCTSTR lpFileName = file.c_str();
				
					if( !::SetFileAttributes(lpFileName,FILE_ATTRIBUTE_NORMAL) )
					{
						Log(LM_ERROR, _T("设置文件\"%s\"属性失败，错误代码%u\n"), lpFileName, GetLastError());
					}

					if( !::DeleteFile(lpFileName) )
					{
						Log(LM_ERROR, _T("删除文件\"%s\"失败，错误代码%u\n"), lpFileName, GetLastError());
					}
				}
			}

			size_t nDirList = vDirList.size();
			for (size_t i = 0; i < nDirList; i++) 
			{
				const stdex::tString &dir = vDirList[i];
				if( vXmlDirList.find(dir) == vXmlDirList.end() ) 
				{
					LPCTSTR lpPathName = dir.c_str();
					//Log(LM_INFO, _T("删除目录%s\n"), lpPathName);
					//utility::DeleteDirectory(lpPathName);
				}
			}
		}
	}



	void TaskCompletor::_RunImpl()
	{
		taskItem_->SetTaskState(tsDeleteOldFile);

		stdex::tString gidBuf;
		stdex::ToString(taskItem_->GetGid(), gidBuf);

		const stdex::tString oldIdx = taskItem_->GetServerPath() + _T("i8desk.idx");

		// 1. 删除多余文件
		Log(LM_DEBUG, _T("删除多余文件"));
		_DeleteOldFiles(taskItem_->GetServerPath());

		// 2. 删除原有索引，复制新索引
		Log(LM_DEBUG, _T("复制新索引"));
		_ReplaceIndex(oldIdx);
		
		// 3. 删除三层临时目录
		Log(LM_DEBUG, _T("删除临时目录 Path = %s\n"), taskItem_->GetMscTmpDir().c_str());
		win32::file::DeleteDirectory(taskItem_->GetMscTmpDir().c_str());


		// 4. 通知虚拟盘
		utility::CEvenyOneSD sd;
		async::thread::AutoEvent vDiskNotify;
		BOOL suc = vDiskNotify.Create(VDISK_EVENT_NAME, FALSE, FALSE, sd.GetSA());
		if( suc )
		{
			Log(LM_DEBUG, _T("通知虚拟盘刷新"));
			vDiskNotify.SetEvent();
		}

		taskItem_->SetTaskState(tsComplete);
	}


	void TaskCompletor::_ClearImpl()
	{
		
	}
}
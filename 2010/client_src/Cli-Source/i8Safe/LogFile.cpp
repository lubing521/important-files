// LogFile.cpp: implementation of the CLogFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogFile::CLogFile()
{

}

CLogFile::~CLogFile()
{

}
//获取文件名称
CString CLogFile::GetFileName()
{
	CString m_sFileName;

	m_sFileName = CTime::GetCurrentTime().Format("%Y-%m-%d") + ".log";
	
	return m_sFileName;
}
//获取应用程序所在路径
CString CLogFile::GetFilePath()
{   
	CString m_FilePath;

	GetModuleFileName(NULL,m_FilePath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);

	m_FilePath.ReleaseBuffer();

	int m_iPosIndex;

	m_iPosIndex = m_FilePath.ReverseFind('\\'); 

	m_FilePath = m_FilePath.Left(m_iPosIndex) + "\\i8SafeLog";

	return m_FilePath;
}

BOOL CLogFile::WriteLog(CString LogText)
{
	try
	{
		CFile m_File;
		CStdioFile m_SFile;
		CFileFind m_FileFind;
		CString m_sErrorMessage;
		CString m_sFileName = GetFileName();
		CString m_sFilePath = GetFilePath();
		CString m_sCurrentTime = CTime::GetCurrentTime().Format("%Y-%m-%d %X");
		
		m_sErrorMessage = "[" + m_sCurrentTime + "]";
		m_sErrorMessage += LogText + "\r\n";

		if(!m_FileFind.FindFile(m_sFilePath))
		{
			CreateDirectory(m_sFilePath,NULL);
		}
		
		if(!m_SFile.Open(m_sFilePath + "\\" +m_sFileName,CFile::modeReadWrite))
		{
			m_SFile.Open(m_sFilePath + "\\" + m_sFileName,CFile::modeCreate | CFile::modeReadWrite | CFile::typeText);
		}
		
		m_SFile.SeekToEnd(); 

		char* m_szMessage;

		m_szMessage=(LPTSTR)(LPCTSTR)m_sErrorMessage;

		m_SFile.Write(m_szMessage,lstrlen(m_szMessage));   

		m_SFile.Close();
	}
	catch(CFileException fileException)
	{
		return false;
	}

	return true;
}
// LogFile.h: interface for the CLogFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGFILE_H__288388CA_9A3E_4F3D_A2B8_F1078E1F6A6B__INCLUDED_)
#define AFX_LOGFILE_H__288388CA_9A3E_4F3D_A2B8_F1078E1F6A6B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLogFile  
{
public:
	CLogFile();
	virtual ~CLogFile();
	static CString GetFileName();
	static CString GetFilePath();
	static BOOL WriteLog(CString LogText);
};

#endif // !defined(AFX_LOGFILE_H__288388CA_9A3E_4F3D_A2B8_F1078E1F6A6B__INCLUDED_)

#pragma once
namespace HTTPFILE
{
	typedef void (WINAPI* PFNDOWNCALLBACK)(DWORD dwError, DWORD progress, LPVOID dwUser);
	DWORD CreateHttpFile(PULONGLONG handle, LPCTSTR url, LPCTSTR file, BOOL async, PFNDOWNCALLBACK pfnCallBack, LPVOID dwUser);
	void  DeleteHttpFile(ULONGLONG handle);
	void ReclaimHttpObjectPool(void);
}
#ifndef __i8desk_except_inc__
#define __i8desk_except_inc__

#pragma once

#include <DbgHelp.h>
#include <Shlwapi.h>

#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "shlwapi.lib")

#ifndef CRASH_DUMP_FILE
#define CRASH_DUMP_FILE "Crash.dmp"
#endif

static void DumpMiniDump(HANDLE hFile, PEXCEPTION_POINTERS excpInfo)
{
	if (excpInfo == NULL) 
	{
		__try 
		{
			RaiseException(EXCEPTION_BREAKPOINT, 0, 0, NULL);
		} 
		__except(DumpMiniDump(hFile, GetExceptionInformation()),
			EXCEPTION_CONTINUE_EXECUTION) 
		{
		}
	} 
	else
	{
		MINIDUMP_EXCEPTION_INFORMATION eInfo;
		eInfo.ThreadId = GetCurrentThreadId();
		eInfo.ExceptionPointers = excpInfo;
		eInfo.ClientPointers = FALSE;
		MiniDumpWriteDump(
			GetCurrentProcess(),
			GetCurrentProcessId(),
			hFile,
			MiniDumpNormal,
			excpInfo ? &eInfo : NULL,
			NULL,
			NULL);
	}
}

static LONG WINAPI Local_UnhandledExceptionFilter(PEXCEPTION_POINTERS pExcept)
{
	char szModuleName[MAX_PATH] = {0};
	GetModuleFileName(NULL, szModuleName, MAX_PATH);
	PathRemoveFileSpec(szModuleName);
	PathAddBackslash(szModuleName);
	lstrcat(szModuleName, "Dump\\");
	CreateDirectory(szModuleName, NULL);
	lstrcat(szModuleName, CRASH_DUMP_FILE);


	HANDLE hFile = CreateFile(szModuleName,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
		NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		DumpMiniDump(hFile, pExcept);
		CloseHandle(hFile);
	}
	return 0;
}
#endif
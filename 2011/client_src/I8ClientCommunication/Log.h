#pragma once

#include "Utility/utility.h"
#include "MultiThread/Lock.hpp"
#include <atlconv.h>

#define    KEY_I8DESK			      TEXT("SOFTWARE\\Goyoo\\i8desk")

class CLogger
{
    utility::CAutoFile clFile;
    async::thread::AutoCriticalSection  lock;
public:
    CLogger()
    {
        TCHAR szFile[MAX_PATH] = {0};
        GetRunPath(szFile);
        lstrcat(szFile, TEXT("I8ClientCommunication.log"));
        utility::MakeSureDirectoryPathExists(szFile);
        clFile = CreateFile(szFile, GENERIC_WRITE, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
        if (clFile.IsValid())
        {
            WriteLog(TEXT("启动客户通讯组件"));
        }
    }
    ~CLogger()
    {
        if (clFile.IsValid())
        {
            WriteLog(TEXT("关闭客户通讯组件\r\n\r\n"));
        }
    }
    void WriteLog(LPCTSTR lpszLog, bool bAddDate = true)
    {
        lock.Lock();
        if (clFile.IsValid())
        {
            char buf[4096] = {0};
            int nLen = 0;
            if (bAddDate)
            {
                SYSTEMTIME st = {0};
                GetLocalTime(&st);
                nLen = sprintf_s(buf, "%04d-%02d-%02d %02d:%02d:%02d ", 
                    st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
            }
            std::string sInfo = CT2A(lpszLog);
            nLen += sprintf_s(&buf[nLen], sizeof(buf) - nLen, "%s\r\n", sInfo.c_str());
            DWORD dwWriteBytes = 0;
            WriteFile(clFile, buf, nLen, &dwWriteBytes, NULL);
        }
        lock.Unlock();
    }
    void WriteLog2(LPCTSTR lpszLog, ...)
    {
        TCHAR szLog[4096] = {0};
        va_list marker;
        va_start(marker, lpszLog);
        _vstprintf_s(szLog, lpszLog, marker);
        WriteLog(szLog);
    }
private:
    template<int iCount>
    void GetRunPath(TCHAR(&atFilePath)[iCount])
    {
        DWORD dwSize = iCount, dwType = REG_SZ;
        DWORD dwRet = SHGetValue(HKEY_LOCAL_MACHINE, KEY_I8DESK, TEXT("exec"), &dwType, atFilePath, &dwSize);
        if (dwRet == ERROR_SUCCESS)
        {
            PathRemoveFileSpec(atFilePath);
            PathAddBackslash(atFilePath);
            lstrcat(atFilePath, TEXT("Log\\"));
        }
        else
        {
            GetModuleFileName(NULL, atFilePath, iCount);
            PathRemoveFileSpec(atFilePath);
            PathAddBackslash(atFilePath);
            lstrcat(atFilePath, TEXT("I8Log\\"));
        }
    }
};

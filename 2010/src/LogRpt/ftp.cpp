#include "stdafx.h"
#include "ftp.h"
#include <WinInet.h>
#pragma comment(lib, "Wininet.lib")

DWORD i8desk_ftpWriteFtp(LPCTSTR lpszFile, HANDLE hExit)
{
	HINTERNET hSession = NULL, hFtp = NULL, hFile = NULL;
	
	if (NULL == (hSession = InternetOpen("i8desk.", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0)))
		return GetLastError();

	if (NULL == (hFtp = InternetConnect(hSession, "123.196.115.69", 22121, "chengduftp", "chengdu@2009",
		INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, NULL)))
	{
		DWORD dwError = GetLastError();
		InternetCloseHandle(hSession);
		return dwError;
	}

	FtpSetCurrentDirectory(hFtp, "/");

	if (NULL == (hFile = FtpOpenFile(hFtp, strrchr(lpszFile, '\\')+1, GENERIC_WRITE, 
		FTP_TRANSFER_TYPE_BINARY, 0)))
	{
		DWORD dwError = GetLastError();
		InternetCloseHandle(hFtp);
		InternetCloseHandle(hSession);
		return dwError;
	}

	HANDLE hZipFile = CreateFile(lpszFile, GENERIC_READ, FILE_SHARE_WRITE|FILE_SHARE_READ,
		NULL, OPEN_EXISTING, 0, NULL);
	int dwSize = 0;
	if (hZipFile != INVALID_HANDLE_VALUE)
		dwSize = GetFileSize(hZipFile, NULL);

	char buf[0x1000] = {0};
	DWORD dwReadBytes = 0;

	while (dwSize && ReadFile(hZipFile, buf, sizeof(buf), &dwReadBytes, NULL) && dwReadBytes)
	{
		DWORD dwSendBytes = 0;
		if (!InternetWriteFile(hFile, buf, dwReadBytes, &dwSendBytes))
		{
			DWORD dwError = GetLastError();
			CloseHandle(hZipFile);
			InternetCloseHandle(hFile);
			InternetCloseHandle(hFtp);
			InternetCloseHandle(hSession);
			return dwError;
		}
		dwSize -= dwSendBytes;
		if (WAIT_OBJECT_0 == WaitForSingleObject(hExit, 0))
		{
			CloseHandle(hZipFile);
			InternetCloseHandle(hFile);
			InternetCloseHandle(hFtp);
			InternetCloseHandle(hSession);
			return 0;
		}
	}
	CloseHandle(hZipFile);
	InternetCloseHandle(hFile);
	InternetCloseHandle(hFtp);
	InternetCloseHandle(hSession);
	return 0;
}
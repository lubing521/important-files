#ifndef __CONSOLE_MISC_HPP
#define __CONSOLE_MISC_HPP



namespace i8desk
{
	// IP 相关
	stdex::tString IP2String(unsigned long ip);
	unsigned long String2IP(const std::string &ip);
	unsigned long String2IP(const std::wstring &ip);

	bool IsValidIP(LPCTSTR lpszIP);

	// 得到本机所有IP
	bool GetLocalIps(std::vector<std::string> &IPs);

	// 格式化时间
	stdex::tString FormatTime(time_t time);

	// 总时间
	stdex::tString TotalTime(time_t time);

	// 服务器判断
	bool IsLocalMachine(const std::string &ip);
	bool IsRunOnServer();


	//"00-12-EF-AC-0A-78" TO {0x00, 0x12, 0xef, 0xac, 0x0a, 0x78}
	bool StringMacToBinaryMac(const unsigned char *lpszMac, unsigned char *pbuf);

	bool IsValidDirName(const stdex::tString &dir);

	
	// 快速将16进制数转换为大写
	TCHAR *HexStrToUpper(TCHAR *str);

	// Hex 2 QWord
	QWORD HexstrToQword(const TCHAR *str);

	// 转换时文件时间
	DWORD MakeTimeFromFileTime(const FILETIME& ft);

	// Format Size
	void FormatSize(const unsigned long long &size, stdex::tString &text);

	// 根据汉字获取拼音
	void GetPY(const stdex::tString &sChinese, stdex::tString &result);
}


#endif
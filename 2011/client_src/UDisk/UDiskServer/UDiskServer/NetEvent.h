#pragma once

class CNetEvent :
	public INetLayerEvent
{
public:
	CNetEvent(void);
	~CNetEvent(void);


	virtual void Release();

	//当客户端连接上来时，产生该事件
	virtual void OnAccept(INetLayer*pNetLayer, SOCKET sck, DWORD param) ;

	//当服务端发客户端发送完成一个数据包时产生该事件
	virtual void OnSend(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param) ; 

	//当服务端接收到客户端一个完整命令包时，产生该事件给应用层处理一个命令
	virtual void OnRecv(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param) ;

	//当客户端断开时产生该事件.
	virtual void OnClose(INetLayer*pNetLayer, SOCKET sck, DWORD param) ;
protected:
	BOOL	RegisteClient(char* username,char* password,string& str);
	void	GetFilepath();
	int     WriteBlockFile(LPSTR lpackage);
	bool    LogVaild(char* pbuf);
	bool	SendData(SOCKET sck,INetLayer*pNetLayer,WORD cmd,const char* pdata= NULL,DWORD length= 0);
	bool    ScanFile(char* path,vector<FileInfo>& vecfileLst);
	void	SendBlcokFile(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage);
	bool    GetDiskFreeSize();
	bool	ReSetPassword(char* buf);
	void	ScanGameGid(const string& szfile,map<DWORD,FILETIME>& maplst);
private:
	CLock	m_optex;

	map<string,HANDLE> m_mapHandle; //文件名字，文件句柄。
	SrvConfigInfo* m_pConfigInfo;
};

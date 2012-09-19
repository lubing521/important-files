
// UDiskClientDlg.h : 头文件
//

#pragma once

#include "DropListCtrl.h"
#include "afxcmn.h"
#include "CustomTabCtrl.h"
// CUDiskClientDlg 对话框

class CFindFile
{
public:
	CFindFile(string str):m_file(str)
	{
	}
	bool operator() (FileInfo& s1)
	{
		return s1.filename == m_file;
	}
private:
	string m_file;
};



class CUDiskClientDlg : public CDialog
{
// 构造
public:
	CUDiskClientDlg(CWnd* pParent = NULL);	// 标准构造函数
	bool DrawFileList();
	void DrawGameLst();
	UINT m_threadID;
	CSocketEx* m_pSocket;
// 对话框数据
	enum { IDD = IDD_UDISKCLIENT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	static UINT _stdcall WorkThread(LPVOID lparam);
	bool SendBlockFile(char* pData,DWORD length);
	bool PostFileToServer(WPARAM wparam,LPARAM lparam);
	bool GetGameInfo(map<DWORD,tagGameInfo*> &mapGameInfoLst);
//	bool SaveGameRecord(DWORD gid);//游戏存档
	bool DowndGameRecord(DWORD gid,string& strError);
	bool ConvertPath(DWORD gid,vector<string>&veclst);
	HICON FindIconOfGame_(DWORD gid);
	void LoadVDisk();
	void LoadIconInfo();
// 实现
protected:
	HICON m_hIcon;
	HANDLE m_Thread;
	char* m_pFileBuf;
	char* m_pIconInfo;
	CCustomTabCtrl	m_ctrlTab;
	CImageList m_ImageList;
	bool init();
	DWORD m_dwFreeRoom,m_dwUsedRoom;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	string ZipFile(LPCSTR zipName,const char* szFile);
	
	DECLARE_MESSAGE_MAP()
public:
	CDropListCtrl m_wndListCtrl;
	CImageList  m_Imagelst;
	string m_UserName;
	vector<FileInfo> m_vecLstPersion;
	vector<FileInfo> m_vecLstComment;
	char m_szPath[MAX_PATH];
	vector<string>m_vecFileLst;
	SrvConfigInfo* m_pSrvConfigInfo;
	CProgressCtrl m_profress;
	CListCtrl m_lstComment;
	CListCtrl m_wndGameList;
	map<DWORD,tagGameInfo*> m_mapGameInfo;
	map<CString,tagVDiskInfo*> m_mapVDisk;
	map<DWORD,LPTSTR> m_mapIconInfo;
protected:
	bool SendFile(LPCSTR path,LPCSTR szfile);
	bool GetFileLst(WORD cmd,vector<FileInfo>& veclst);
	bool DowndFile(const char* szfile,char* SzDestDir);
	bool UnZipFile(const char* szfile,char* szDestDir,bool bDir = true);
	void UpdataStatic();
public:
	afx_msg void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg LRESULT DrawPorgress(WPARAM wparam,LPARAM lparam);
	afx_msg void OnShowlst();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnNMRClickList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDowndcommfile();
	afx_msg void OnBnClickedAbout();
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedInfo();
	afx_msg void OnNMRClickList3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeleteGameRecord();
	afx_msg void OnReadrecord();
	afx_msg LRESULT ReadGamerRecord(WPARAM wparam,LPARAM lparam);
	afx_msg LRESULT SaveGameRecord(WPARAM wparam,LPARAM lparam);
};

#ifndef _i8desk_makeidx_inc_
#define _i8desk_makeidx_inc_

#include <process.h>
#include <string>
#include <sstream>



#define MAKEQWORD(a, b)	((QWORD)( ((QWORD) ((DWORD) (a))) << 32 | ((DWORD) (b))))

#define LODWORD(l) ((DWORD)(l))
#define HIDWORD(l) ((DWORD)(((QWORD)(l) >> 32) & 0xFFFFFFFF))

enum IDX_STATUS_PROGRESS
{
	IDX_STATUS_ERROR,
	IDX_STATUS_SCAN,
	IDX_STATUS_GETSIZE,
	IDX_STATUS_CRC,
	IDX_STATUS_CHECK,
	IDX_STATUS_FINISH,
	IDX_STATUS_CRC_START,
	IDX_STATUS_CRC_END,
};

namespace i8desk
{
	static int g_nIdxMessage = RegisterWindowMessage("_i8desk_make_idx_message_");
	typedef struct tagFileHeader
	{
		DWORD dwGameSizeLow;
		DWORD dwGameSizeHigh;
		DWORD resv1;
		DWORD resv2;
		DWORD resv3;
		DWORD resv4;
		DWORD dwXmlSize;
		WORD  resv5;
		WORD  CrcBlockNum;
	}tagFileHeader;
	typedef struct tagCrcBlock
	{
		DWORD dwReseverd0;
		DWORD dwReseverd1;
		DWORD dwBlockSize;
		DWORD dwReseverd2;
		DWORD dwCrc32;
		DWORD dwReseverd3;
		DWORD dwReseverd4;
		DWORD dwReseverd5;
	}tagCrcBlock;

	typedef struct tagFileInfo
	{
		WCHAR FileName[MAX_PATH];
		QWORD qSize;
		QWORD qModifyTime;
		DWORD dwAttr;
		DWORD dwCrcOffset;
	} tagFileInfo;

	class CMakeIdx
	{
	public:
		CMakeIdx() 
			: m_hThread(NULL)
			, m_bExited(0)
			, m_hWnd(NULL)
			, m_bOnlyMakeIdx(true)
		{
			CoInitialize(NULL);
		}
		~CMakeIdx() 
		{
			StopMakeIdx();
			CoUninitialize();
		}
	public:		
		bool StartMakeIdx(LPCWSTR dir, bool bOnlyMakeIdx, HWND hWnd);
		void StopMakeIdx();

		std::string GetErrorString() { return m_strError; }
		std::string GetCheckLog()	 { return m_strLog;   }
		DWORD GetProgress(void)		 { return m_dwProgress; }
	private:
//*
		bool MakeIdxScanDir(MSXML2::IXMLDOMElementPtr& Parent,
			const std::wstring& Dir, 
			__int64& llTotalSize, 
			std::vector<tagFileInfo>& FileList);
/*/
		bool MakeIdxScanDir(
			MSXML2::IXMLDOMElementPtr& Parent, 
			std::stringstream& xml,
			const std::string& Dir,
			bool bNotRootDir,
			__int64& llTotalSize, 
			std::vector<tagFileInfo>& FileList);
//*/
		bool CheckIdxScanDir(const std::wstring& Dir,  __int64& llTotalSize, std::vector<tagFileInfo>& FileList);
		bool CalCrcBlock(std::vector<tagFileInfo>& FileList, __int64 qGameSize, std::vector<tagCrcBlock>& CrcBlock);
		bool CheckIdx(const std::vector<tagFileInfo>& FileList, const std::vector<tagFileInfo>& FileList2,
			tagCrcBlock* pCrcBlock, tagCrcBlock* pCrcBlock2);
		bool LoadOldIdxFile(std::vector<tagFileInfo>& FileList, tagCrcBlock* & pCrcBlock);
		void ParaseXMLFile(MSXML2::IXMLDOMNodePtr root, std::vector<tagFileInfo>& fileList, int& nOffset);
		_bstr_t GetNodeFullName(MSXML2::IXMLDOMNodePtr& node);
		MSXML2::IXMLDOMNodePtr CmpFile(MSXML2::IXMLDOMNodePtr& node, MSXML2::IXMLDOMNodeListPtr& childs);
	private:
		MSXML2::IXMLDOMDocumentPtr m_XmlDoc;
		MSXML2::IXMLDOMDocumentPtr m_XmlLocDoc;
		std::wstring m_strDir;
		std::wstring m_strIdxfile;
		std::string m_strError;
		std::string m_strLog;
		HWND m_hWnd;
		bool m_bOnlyMakeIdx;
		HANDLE m_hThread;
		int    m_bExited;
		DWORD m_dwProgress;
		static UINT __stdcall ThreadProc(LPVOID lpVoid);
	};
}

#endif
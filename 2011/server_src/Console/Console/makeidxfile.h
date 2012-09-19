#ifndef __MAKE_IDX_HPP
#define __MAKE_IDX_HPP

#include <string>
#include <vector>
#include <sstream>



enum { WM_MAKEIDX = WM_USER + 0x100 };

#define MAKEQWORD(a, b)	((QWORD)( ((QWORD) ((DWORD) (a))) << 32 | ((DWORD) (b))))

#ifndef LODWORD
    #define LODWORD(l) ((DWORD)(l))
#endif
#ifndef HIDWORD
    #define HIDWORD(l) ((DWORD)(((QWORD)(l) >> 32) & 0xFFFFFFFF))
#endif

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
	
	struct FileHeader
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
	};

	struct CrcBlock
	{
		DWORD dwReseverd0;
		DWORD dwReseverd1;
		DWORD dwBlockSize;
		DWORD dwReseverd2;
		DWORD dwCrc32;
		DWORD dwReseverd3;
		DWORD dwReseverd4;
		DWORD dwReseverd5;
	};

	struct FileInfo
	{
		WCHAR FileName[MAX_PATH];
		QWORD qSize;
		QWORD qModifyTime;
		DWORD dwAttr;
		DWORD dwCrcOffset;
	};

	// ---------------------------------
	class CMakeIdx
	{
	public:
		CMakeIdx() 
			: m_hThread(NULL)
			, m_bExited(0)
			, m_hWnd(NULL)
			, m_bOnlyMakeIdx(true)
			, m_bError(0)
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
		bool StopMakeIdx(bool wait = false);

		const stdex::tString &GetErrorString() const { return m_strError; }
		const stdex::tString &GetCheckLog() const	 { return m_strLog;   }
		DWORD GetProgress(void)		 { return m_dwProgress; }
	private:

		bool MakeIdxScanDir(MSXML2::IXMLDOMElementPtr& Parent, const std::wstring& Dir, 
			__int64& llTotalSize, 
			std::vector<FileInfo>& FileList);

		bool CheckIdxScanDir(const std::wstring& Dir,  __int64& llTotalSize, std::vector<FileInfo>& FileList);
		bool CalCrcBlock(std::vector<FileInfo>& FileList, __int64 qGameSize, std::vector<CrcBlock>& crcBlocks);
		bool CheckIdx(const std::vector<FileInfo>& FileList, const std::vector<FileInfo>& FileList2,
			CrcBlock* pCrcBlock, CrcBlock* pCrcBlock2);
		bool LoadOldIdxFile(std::vector<FileInfo>& FileList, CrcBlock* & pCrcBlock);
		void ParaseXMLFile(MSXML2::IXMLDOMNodePtr root, std::vector<FileInfo>& fileList, int& nOffset);
		_bstr_t GetNodeFullName(MSXML2::IXMLDOMNodePtr& node);
		MSXML2::IXMLDOMNodePtr CmpFile(MSXML2::IXMLDOMNodePtr& node, MSXML2::IXMLDOMNodeListPtr& childs);

	private:
		MSXML2::IXMLDOMDocumentPtr m_XmlDoc;
		MSXML2::IXMLDOMDocumentPtr m_XmlLocDoc;
		std::wstring m_strDir;
		std::wstring m_strIdxfile;
		stdex::tString m_strError;
		stdex::tString m_strLog;
		HWND m_hWnd;
		bool m_bOnlyMakeIdx;
		bool m_bError;
		HANDLE m_hThread;
		int    m_bExited;
		DWORD m_dwProgress;

		static UINT __stdcall ThreadProc(LPVOID lpVoid);
	};
}

#endif
#include "stdafx.h"
#include <math.h>
#include <time.h>
#include <Shlwapi.h>
#include "idxfile.h"
#include <stack>

#define USE_MSXML_SAX

#pragma comment(lib, "shlwapi.lib")

#import "msxml3.dll"

//把filetime转换成time_t时间。如果返回-1,表示转换失败(即time_t无法表示filetime)
DWORD MakeTimeFromFileTime(const FILETIME& ft)
{
	FILETIME localFileTime;
	FileTimeToLocalFileTime(&ft, &localFileTime);

	SYSTEMTIME st;		
	FileTimeToSystemTime(&localFileTime, &st);
	struct tm stm = {0};
	stm.tm_year = st.wYear - 1900;
	stm.tm_mon  = st.wMonth - 1;
	stm.tm_mday = st.wDay;
	stm.tm_hour = st.wHour;
	stm.tm_min  = st.wMinute;
	stm.tm_sec  = st.wSecond;
	stm.tm_isdst = -1;
	return _mktime32(&stm);
}

//把十六进制字符串转换成__int64.（字符串没有0x开头）
__int64 HexStr2UInt64(const _bstr_t& src)
{
	if (src.length() == 0)
		return 0LL;

	__int64 result = 0LL;
	__int64 number = 1LL;
	wchar_t* psrc = src;
	for (int idx=src.length()-1; idx>=0; idx--)
	{
		if (iswdigit(psrc[idx]))
		{
			result += ((psrc[idx]- L'0') * number);
		}
		else if (iswalpha(psrc[idx]))
		{
			result += ((towlower(psrc[idx]) - L'a' + 10) * number);
		}
		number *= 16;
	}
	return result;
}

inline DWORD HexStr2UInt32(const _bstr_t& src)
{
	return (DWORD)HexStr2UInt64(src);
}

//得到xml节点的文本
inline _bstr_t GetNodeAttr(const MSXML2::IXMLDOMNodePtr& node, LPCSTR name)
{
	MSXML2::IXMLDOMNamedNodeMapPtr attr = node->Getattributes();
	if (!attr)
		return _bstr_t("");
	MSXML2::IXMLDOMNodePtr node2 = attr->getNamedItem(_bstr_t(name));
	if (!node2)
		return _bstr_t("");
	return node2->text;
}

DWORD ScanDirectory(const std::wstring& dir, int baselen, MSXML2::IXMLDOMElementPtr& xml, 
	MSXML2::IXMLDOMDocumentPtr& xmlDoc, __int64& qGameSize)
{
	WIN32_FIND_DATAW wfd = {0};
	HANDLE hFinder = FindFirstFileW((dir + L"*.*").c_str(), &wfd);
	if (hFinder == INVALID_HANDLE_VALUE)
		return GetLastError();

	//插入目录节点
	MSXML2::IXMLDOMElementPtr DirNode = xmlDoc->createElement("d");
	{
		std::wstring name = dir;
		name.resize(name.size() - 1);
		name = name.substr(name.rfind(L'\\') + 1);
		DirNode->setAttribute("n", name.c_str());
		if (!xml)
			xml = DirNode;
		else
			xml->appendChild(DirNode);
	}

	std::vector <std::wstring> dirs;
	do 
	{
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (!((wfd.cFileName[0] == L'.') && 
				((wfd.cFileName[1] == 0) || (wfd.cFileName[1] == L'.' && wfd.cFileName[2] == 0))))
			{
				dirs.push_back(dir + wfd.cFileName);
			}
		}
		else	//file.
		{
			std::wstring file	= dir + wfd.cFileName;
			std::wstring subfile = file.substr(baselen);
			if (_wcsicmp(subfile.c_str(), INDEX_FILE_NAMEW) != 0)
			{
				//插入文件节点
				MSXML2::IXMLDOMElementPtr FileNode = xmlDoc->createElement("f");
				{
					wchar_t buf[MAX_PATH] = {0};
					FileNode->setAttribute("n", wfd.cFileName);
					__int64 qSize = MAKEQWORD(wfd.nFileSizeLow, wfd.nFileSizeHigh);
					qGameSize += qSize;
					if (qSize)
					{
						swprintf(buf, L"%I64X", qSize);
						FileNode->setAttribute("s", buf);
					}

					//修改时间用属性t(time_t)表示，有些时间不能用time_t表示,则用w(filetime)来表示
					DWORD t = MakeTimeFromFileTime(wfd.ftLastWriteTime);
					if (-1 != t)
					{
						swprintf(buf, L"%X", t);
						FileNode->setAttribute("t", buf);
					}
					else
					{
						swprintf(buf, L"I64X", MAKEQWORD(wfd.ftLastWriteTime.dwLowDateTime, 
							wfd.ftLastWriteTime.dwHighDateTime));
						FileNode->setAttribute("m", buf);
					}

					//如果属性等于32则属性为空。相当于默认是32.为了少占索引空间
					if (wfd.dwFileAttributes != 32)
					{
						swprintf(buf, L"%X", wfd.dwFileAttributes);
						FileNode->setAttribute("a", buf);
					}
					DirNode->appendChild(FileNode);
				}
			}
		}
	}while (FindNextFileW(hFinder, &wfd));
	FindClose(hFinder);

	for (size_t idx=0; idx<dirs.size(); idx++)
	{
		ScanDirectory(dirs[idx] + L"\\", baselen, DirNode, xmlDoc, qGameSize);
	}		
	return 0;
}

inline void CalcCrc32(BYTE byte, DWORD& dwCrc32);

int Idxfile_CalCrc(const MSXML2::IXMLDOMElementPtr& xml, std::vector<tagIdxBlock>& vcrcs, 
		   const std::wstring& dir, int blksize = INDEX_BLOCK_SIZE)
{
	PBYTE pdata = new BYTE[blksize];
	MSXML2::IXMLDOMNodeListPtr list = xml->GetchildNodes();
	for (long idx=0; idx<list->Getlength(); idx++)
	{
		MSXML2::IXMLDOMNodePtr	  node = list->Getitem(idx);
		MSXML2::IXMLDOMElementPtr element = node;
		if (node->GetnodeName() == _bstr_t("d"))
		{
			std::wstring newDir = dir + (LPCWSTR)GetNodeAttr(node, "n") + L"\\";
			Idxfile_CalCrc(element, vcrcs, newDir, blksize);
		}
		else
		{
			std::wstring file = dir + (LPCWSTR)GetNodeAttr(node, "n");
			HANDLE hFile = CreateFileW(file.c_str(), GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, 
				NULL, OPEN_EXISTING, 0, NULL);
			if (hFile == INVALID_HANDLE_VALUE)
			{
				DWORD dwError = GetLastError();
				delete []pdata;
				return dwError;
			}

			LARGE_INTEGER qFileSize;
			if (!GetFileSizeEx(hFile, &qFileSize))
			{
				DWORD dwError = GetLastError();
				delete []pdata;
				CloseHandle(hFile);
				return dwError;
			}

			__int64 qOffset = 0;
			DWORD dwFileCRC = -1;
			while (qOffset < qFileSize.QuadPart)
			{
				DWORD dwReadBytes = 0;
				if (!ReadFile(hFile, pdata, blksize, &dwReadBytes, NULL))
				{
					DWORD dwError = GetLastError();
					CloseHandle(hFile);
					delete []pdata;
					return dwError;
				}

				//cal block and file crc.
				DWORD blkCRC = -1;
				for (DWORD idx=0; idx<dwReadBytes; idx++)
				{
					CalcCrc32(pdata[idx], dwFileCRC);
					CalcCrc32(pdata[idx], blkCRC);
				}
				blkCRC = ~blkCRC;
				tagIdxBlock block = {0};
				block.blksize = dwReadBytes;
				block.crc     = blkCRC;
				vcrcs.push_back(block);
				qOffset += dwReadBytes;
			}
			dwFileCRC = ~dwFileCRC;

			//写文件CRC
			wchar_t buf[MAX_PATH] = {0};
			swprintf(buf, L"%X", dwFileCRC);
			element->setAttribute("x", buf);
			CloseHandle(hFile);
		}
	}
	delete []pdata;

	return 0;
}

int Idxfile_WriteFile(const std::wstring& IdxFile, MSXML2::IXMLDOMElementPtr& xml, 
	std::vector<tagIdxBlock>& vcrcs, __int64& qGameSize)
{
	//convert wstring to utf-8.
	std::wstring doc = xml->Getxml();
	int nXmlSize = WideCharToMultiByte(CP_UTF8, 0, doc.c_str(), doc.size(), NULL, 0, NULL, NULL);
	char* pXmlData = new char[nXmlSize+1];
	int len = WideCharToMultiByte(CP_UTF8, 0, doc.c_str(), doc.size(), pXmlData, nXmlSize+1, NULL, NULL);
	pXmlData[len] = 0;

	HANDLE hFile = CreateFileW(IdxFile.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		DWORD dwError = GetLastError();
		delete[]pXmlData;
		return dwError;
	}

	DWORD dwLength = sizeof(tagIdxHeahder) + nXmlSize + 32 + vcrcs.size() * 32;
	char* pIdxFile = new char[dwLength];
	tagIdxHeahder* pheader = (tagIdxHeahder* )pIdxFile;

	//write idxfile header.
	ZeroMemory(pheader, sizeof(tagIdxHeahder));
	pheader->lowsize	= LOWDWORD(qGameSize);
	pheader->highsize	= HIGHDWORD(qGameSize);
	pheader->blknum		= 0;	//lava crc block num.
	pheader->xmlsize	= nXmlSize;

	//write idxfile file list(xml).
	char* pData = pIdxFile + sizeof(tagIdxHeahder);
	if (nXmlSize)
	{
		memcpy(pData, pXmlData, nXmlSize);
	}
	pData += nXmlSize;
	ZeroMemory(pData, 32);
	*(DWORD*)pData = _time32(NULL);

	//write idxfile crc list.
	pData += 32;
	if (vcrcs.size())
	{
		memcpy(pData, &vcrcs[0], vcrcs.size() * 32);
	}

	//write data to file.
	DWORD dwWriteBytes = 0;
	if (!WriteFile(hFile, pIdxFile, dwLength, &dwWriteBytes, NULL))
	{
		DWORD dwError = GetLastError();
		CloseHandle(hFile);
		delete[]pXmlData;
		delete[]pIdxFile;
		return dwError;
	}
	CloseHandle(hFile);
	delete []pXmlData;
	delete []pIdxFile;
	return ERROR_SUCCESS;
}

DWORD Idxfile_Make(LPCWSTR dir, DWORD blksize /* = INDEX_BLOCK_SIZE */)
{
	CoInitialize(NULL);
	{
		MSXML2::IXMLDOMDocumentPtr xmlDoc(__uuidof(MSXML2::DOMDocument));
		MSXML2::IXMLDOMElementPtr  xml;
		std::vector<tagIdxBlock> vcrcs;

		xmlDoc->preserveWhiteSpace = VARIANT_TRUE;

		std::wstring sdir(dir);
		if (sdir.size() && *sdir.rbegin() != L'\\')
			sdir += L"\\";

		DWORD ret = ERROR_SUCCESS;
		__int64 qGameSize = 0;
		if ( (ret = ScanDirectory(sdir, sdir.size(), xml, xmlDoc, qGameSize)) != ERROR_SUCCESS)
			return ret;
		if (xml)	//设置块大小.
		{
			wchar_t buf[20] = {0};
			swprintf(buf, L"%X", blksize);
			xml->setAttribute("blksize", buf);
		}

		if ( (ret = Idxfile_CalCrc(xml, vcrcs , sdir, blksize)) != ERROR_SUCCESS)
			return ret;

		if ( (ret = Idxfile_WriteFile(sdir + INDEX_FILE_NAMEW, xml, vcrcs, qGameSize)) != ERROR_SUCCESS)
			return ret;
	}
	CoUninitialize();
	return ERROR_SUCCESS;
}

DWORD Idxfile_Parse(LPCWSTR filename, std::vector<tagIdxFile*>& vFiles, 
	std::vector<tagIdxBlock*>& vBlocks, std::map<std::wstring, tagIdxFile*>& mFiles)
{
	vFiles.clear(); vBlocks.clear(); mFiles.clear();
	HANDLE hFile = CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return GetLastError();

	DWORD dwSize = GetFileSize(hFile, NULL);
	char* pData = new char[dwSize+1];
	DWORD dwReadBytes = 0;
	if (!ReadFile(hFile, pData, dwSize, &dwReadBytes, NULL))
	{
		DWORD dwError = GetLastError();
		CloseHandle(hFile);
		delete[] pData;
		return dwError;
	}

	DWORD ret = Idxfile_Parse(pData, dwSize, vFiles, vBlocks, mFiles);
	CloseHandle(hFile);
	delete[] pData;

	return ret;
}

#ifdef USE_MSXML_SAX
class CPlParser : public MSXML2::ISAXContentHandler
{
public:
	CPlParser(std::map<std::wstring, tagIdxFile*>& mF, std::vector<tagIdxFile*>& vF, DWORD& crcoffset) 
		: mFiles_(mF), vFiles_(vF), crcoff_(crcoffset), brootelement_(true) {}
private:
	std::map<std::wstring, tagIdxFile*> &mFiles_;
	std::vector<tagIdxFile*> &vFiles_;
	DWORD  &crcoff_;

	std::wstring     dir_;
	std::stack <int> dirstack_;
	bool			 brootelement_;	
public:
	HRESULT __stdcall QueryInterface(const IID &, void **)
	{
		return S_OK;
	}
	ULONG __stdcall AddRef(void)
	{
		return 1;
	}
	ULONG __stdcall Release(void)
	{
		return 1;
	}
	HRESULT __stdcall raw_putDocumentLocator(struct MSXML2::ISAXLocator * pLocator)
	{
		return S_OK;
	}
	HRESULT __stdcall raw_startDocument( )
	{
		mFiles_.clear(); vFiles_.clear();
		crcoff_ = 0;
		dir_.clear();
		brootelement_ = true;		
		return S_OK;
	}
	HRESULT __stdcall raw_endDocument()
	{ 
		return S_OK;
	}
	HRESULT __stdcall raw_startPrefixMapping(unsigned short * pwchPrefix, int cchPrefix,
		unsigned short * pwchUri, int cchUri)
	{
		return S_OK;
	}
	HRESULT __stdcall raw_endPrefixMapping (unsigned short * pwchPrefix, int cchPrefix) { return S_OK; }

	inline void InitIdxFile(tagIdxFile* pFile, bool isdir, DWORD crcoff)
	{
		pFile->size = 0;
		pFile->time = 0;
		pFile->attr = isdir ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_ARCHIVE;
		pFile->blknum = 0;
		pFile->crc  = 0;
		pFile->crcoff = crcoff;
		pFile->update = 0;
	}

	HRESULT __stdcall raw_startElement (unsigned short * pwchNamespaceUri, int cchNamespaceUri,
		unsigned short * pwchLocalName, int cchLocalName,
		unsigned short * pwchQName, int cchQName, struct MSXML2::ISAXAttributes * pAttributes)
	{
		if (brootelement_)	//document root element.
		{
			dirstack_.push(0);
			brootelement_ = false;
			return S_OK;
		}
		int  nlen = 0, vlen = 0;
		wchar_t* pname = NULL, *pvalue = NULL;
		bool isdir = (cchLocalName == 1 && *pwchLocalName == L'd');
		tagIdxFile* pFile = new tagIdxFile;
		InitIdxFile(pFile, isdir, crcoff_);

		for (int i = 0; i < pAttributes->getLength(); i++)
		{
			if (S_OK != pAttributes->getLocalName(i, (unsigned short**)&pname, &nlen) ||
				S_OK != pAttributes->getValue(i, (unsigned short**)&pvalue, &vlen) || 
				nlen != 1)
				return S_FALSE;

			std::wstring attrValue(pvalue, vlen);				
			switch (*pname)
			{
			case L'n':
				if (isdir)
				{
					dir_ += attrValue + L"\\";
					pFile->name = dir_;
					dirstack_.push(vlen + 1);
				}
				else
				{
					pFile->name = dir_ + attrValue;
				}
				break;
			case L's':
				pFile->size = HexStr2UInt64(attrValue.c_str());
				break;
			case L'm':
				pFile->time = HexStr2UInt64(attrValue.c_str());
				break;
			case L't':
				{
					pFile->time = HexStr2UInt64(attrValue.c_str());
					pFile->time *= 0x989680;
					pFile->time += 0x19DB1DED53E8000LL;
				}
				break;
			case L'a':
				pFile->attr = HexStr2UInt32(attrValue.c_str());
				break;
			case L'x':
				pFile->crc  = HexStr2UInt32(attrValue.c_str());;
				break;
			}
		}
		pFile->blknum   = (DWORD)((pFile->size + INDEX_BLOCK_SIZE - 1) / INDEX_BLOCK_SIZE);
		crcoff_		   += pFile->blknum;
		//删除<f n="k" a="10">这种表示empty目录的文件。
		if (!isdir && pFile->attr == FILE_ATTRIBUTE_DIRECTORY)
			delete pFile;
		else
		{
			mFiles_.insert(std::make_pair(pFile->name, pFile));
			vFiles_.push_back(pFile);
		}
		return S_OK;
	}
	HRESULT __stdcall raw_endElement (unsigned short * pwchNamespaceUri, int cchNamespaceUri,
		unsigned short * pwchLocalName, int cchLocalName, unsigned short * pwchQName, int cchQName)
	{
		if (*pwchLocalName == L'd')
		{
			dir_.resize(dir_.size() - dirstack_.top());
			dirstack_.pop();
		}
		return S_OK;
	}
	HRESULT __stdcall raw_characters (unsigned short * pwchChars, int cchChars)
	{
		return S_OK;
	}
	HRESULT __stdcall raw_ignorableWhitespace (unsigned short * pwchChars, int cchChars)
	{
		return S_OK;
	}
	HRESULT __stdcall raw_processingInstruction (unsigned short * pwchTarget, int cchTarget,
		unsigned short * pwchData, int cchData)
	{
		return S_OK;
	}
	HRESULT __stdcall raw_skippedEntity (unsigned short * pwchName, int cchName)
	{
		return S_OK;
	}
};
#else
void LoadXml2FileList(const MSXML2::IXMLDOMElementPtr& xml, DWORD blksize, std::map<std::wstring, tagIdxFile*>& mFiles,
	std::vector<tagIdxFile*>& vFiles, const std::wstring& root, DWORD& crcoff)
{
	std::vector<std::wstring> SubDirs;
	MSXML2::IXMLDOMNodeListPtr list = xml->GetchildNodes();
	for (long idx=0; idx<list->Getlength(); idx++)
	{
		MSXML2::IXMLDOMNodePtr	  node = list->Getitem(idx);
		MSXML2::IXMLDOMElementPtr element = node;

		//解析文件节点
		if (node->GetnodeName() == _bstr_t("f"))
		{
			_bstr_t name = GetNodeAttr(node, "n");
			std::wstring file(root);
			file += name;

			if (!(name == _bstr_t(L"k") && GetNodeAttr(node, "a") == _bstr_t(L"10")))
			{
				tagIdxFile* pFile = new tagIdxFile;
				pFile->name		= file;
				pFile->size		= HexStr2UInt64(GetNodeAttr(node, "s"));
				pFile->time		= HexStr2UInt64(GetNodeAttr(node, "m"));
				if (pFile->time == 0)
				{
					pFile->time  = HexStr2UInt32(GetNodeAttr(node, "t"));
					pFile->time *= 0x989680;
					pFile->time += 0x19DB1DED53E8000LL;
				}
				pFile->attr		= HexStr2UInt32(GetNodeAttr(node, "a"));
				if (pFile->attr == 0)
					pFile->attr = 32;

				pFile->blknum   = (DWORD)((pFile->size + blksize - 1) / blksize);
				pFile->crc		= HexStr2UInt32(GetNodeAttr(node, "x"));
				pFile->crcoff	= crcoff;
				crcoff		   += pFile->blknum;

				mFiles.insert(std::make_pair(pFile->name, pFile));
				vFiles.push_back(pFile);
			}
		}
		//解析目录节点
		else
		{
			std::wstring dir(root);
			dir += (LPCWSTR)GetNodeAttr(node, "n");
			dir += L"\\";

			tagIdxFile* pFile = new tagIdxFile;
			pFile->name		= dir;
			pFile->size		= 0;
			pFile->time		= 0;
			pFile->attr		= FILE_ATTRIBUTE_DIRECTORY;
			pFile->blknum	= 0;
			pFile->crc		= 0;
			pFile->crcoff	= crcoff;
			mFiles.insert(std::make_pair(pFile->name, pFile));
			vFiles.push_back(pFile);
			LoadXml2FileList(element, blksize, mFiles, vFiles, dir, crcoff);
		}
	}
}
#endif

DWORD Idxfile_Parse(char* pfile, DWORD length, std::vector<tagIdxFile*>& vFiles, 
	std::vector<tagIdxBlock*>& vBlocks, std::map<std::wstring, tagIdxFile*>& mFiles)
{
	vFiles.clear(); vBlocks.clear(); mFiles.clear();

	//check file length.
	tagIdxHeahder* pheader = reinterpret_cast<tagIdxHeahder*>(pfile);
	if ( length < sizeof(tagIdxHeahder) ||	//file header.
		(length < sizeof(tagIdxHeahder) + pheader->blknum * 32) ||	//lava crc block
		(length < sizeof(tagIdxHeahder) + pheader->blknum * 32 + pheader->xmlsize) || //lava xml block.
	    pheader->xmlsize == 0)
	{
		return ERROR_INVALID_DATA;
	}

	std::string doc(pfile + sizeof(tagIdxHeahder) + pheader->blknum * 32, pheader->xmlsize);
	
	//convert utf8 to wstring.
	int nXmlSize = MultiByteToWideChar(CP_UTF8, 0, doc.c_str(), doc.size(), NULL, 0);
	wchar_t* pXmlData = new wchar_t[nXmlSize+1];
	int len = MultiByteToWideChar(CP_UTF8, 0, doc.c_str(), doc.size(), pXmlData, nXmlSize+1);
	pXmlData[len] = 0;

	//parse xml file.
	DWORD ret	 = ERROR_SUCCESS;
	DWORD crcoff = 0;
	CoInitialize(NULL);

#ifdef USE_MSXML_SAX	
	{
		try
		{
			CPlParser Parser(mFiles, vFiles, crcoff);
			MSXML2::ISAXXMLReaderPtr Reader(__uuidof(MSXML2::SAXXMLReader));
			Reader->putContentHandler(&Parser);
			if (S_OK != Reader->parse(_variant_t(pXmlData)))
				ret = ERROR_INVALID_DATA;
		}
		catch (_com_error& err)
		{
			ret = ERROR_INVALID_DATA;
			OutputDebugString(err.Description());
		}
	}
#else
	{
		MSXML2::IXMLDOMDocumentPtr xmlDoc(__uuidof(MSXML2::DOMDocument));
		xmlDoc->preserveWhiteSpace = VARIANT_TRUE;
		if (VARIANT_FALSE == xmlDoc->loadXML(_bstr_t(pXmlData)))
			ret = ERROR_INVALID_DATA;
		else
		{
			DWORD blksize = HexStr2UInt32(GetNodeAttr(xmlDoc->documentElement, "blksize"));
			if (blksize == 0)
				blksize = INDEX_BLOCK_SIZE;			
			LoadXml2FileList(xmlDoc->documentElement, blksize, mFiles, vFiles, L"", crcoff);
		}
	}
#endif

	//parse crc block.
	if (ret != ERROR_SUCCESS || 
		sizeof(tagIdxHeahder) + pheader->blknum * 32 + pheader->xmlsize + 32 + crcoff * 32 > length)
		ret = ERROR_INVALID_DATA;
	else
	{
		tagIdxBlock* pcrcstart = (tagIdxBlock*)(pfile + sizeof(tagIdxHeahder) + pheader->blknum * 32 +
			pheader->xmlsize + 32);
		DWORD filepos = 0;
		for (std::vector<tagIdxFile*>::iterator it = vFiles.begin(); it != vFiles.end(); it++)
		{
			(*it)->update = 0;	//设置成整个文件不需要更新		
			__int64 blkcount = (*it)->blknum;
			__int64 fileoffset = 0;
			for (__int64 idx=0; idx<blkcount; idx++)
			{
				tagIdxBlock* pBlock = new tagIdxBlock;
				pBlock->offset  = fileoffset;
				pBlock->blksize = pcrcstart->blksize;
				//先设置成不更新，通过对比来发现是否需要更新
				pBlock->update  = 0;
				pBlock->crc     = pcrcstart->crc;
				pBlock->resv6   = 0;
				pBlock->fileoff = filepos;
				pBlock->pdata   = 0;
				vBlocks.push_back(pBlock);
				fileoffset += pBlock->blksize;
				pcrcstart++;
			}
			filepos ++;
		}
	}
	delete[]pXmlData;

	//free memory if error.
	if (ret != ERROR_SUCCESS)
	{
		for (std::vector<tagIdxBlock*>::iterator it = vBlocks.begin(); it != vBlocks.end(); it++)
			delete (*it);
		for (std::vector<tagIdxFile*>::iterator it = vFiles.begin(); it != vFiles.end(); it++)
			delete (*it);

		vFiles.clear(); vBlocks.clear(); mFiles.clear();
	}
	return ret;
}

void Idxfile_Compare(LPCWSTR dir, std::vector<tagIdxFile*>& vFiles, std::vector<tagIdxBlock*>& vBlocks, __int64& qSize,
	IDXCALLBACK pfnCallback, void* pCookie)
{
	qSize = 0;
	wchar_t root[MAX_PATH] = {0};
	wcscpy(root, dir);
	PathAddBackslashW(root);

	for (std::vector<tagIdxFile*>::iterator it = vFiles.begin(); it != vFiles.end(); it ++)
	{
		tagIdxFile* pFile = (*it);
		if (!(pFile->attr & FILE_ATTRIBUTE_DIRECTORY))
		{
			wchar_t file[MAX_PATH] = {0}; 
			WIN32_FILE_ATTRIBUTE_DATA wfd = {0};
			swprintf(file, L"%s%s", root, pFile->name.c_str());
			BOOL b = GetFileAttributesExW(file, GetFileExInfoStandard, &wfd);
			__int64 qFileSize = MAKEQWORD(wfd.nFileSizeLow, wfd.nFileSizeHigh);
			__int64 qFileTime = MAKEQWORD(wfd.ftLastWriteTime.dwLowDateTime, wfd.ftLastWriteTime.dwHighDateTime);
			if (!b || pFile->size != qFileSize || _abs64(pFile->time - qFileTime) > 20000000)
			{
				pFile->update = 1;
				for (DWORD idx=0; idx<pFile->blknum; idx++)
				{
					vBlocks[pFile->crcoff+idx]->update = 1;
				}
				qSize += pFile->size;
			}
			if (pfnCallback != NULL && !pfnCallback(pCookie, pFile->name.c_str()))
				break;
		}
	}
}

void Idxfile_Compare(std::map<std::wstring, tagIdxFile*>& mNewFiles, std::vector<tagIdxBlock*>& vNewBlocks, 
	std::map<std::wstring, tagIdxFile*>& mOldFiles, std::vector<tagIdxBlock*>& vOldBlocks, __int64& qSize,
	IDXCALLBACK pfnCallback, void* pCookie)
{
	qSize = 0;
	for (std::map<std::wstring, tagIdxFile*>::iterator first = mNewFiles.begin(); first != mNewFiles.end(); first ++)
	{
		tagIdxFile* pFile = first->second;
		if (pfnCallback != NULL && !pfnCallback(pCookie, pFile->name.c_str()))
			break;
		std::map<std::wstring, tagIdxFile*>::iterator second = mOldFiles.find(first->first);
		if (second == mOldFiles.end())
		{
			pFile->update = 1;
			for (DWORD idx=0; idx<pFile->blknum; idx++)
			{
				vNewBlocks[pFile->crcoff+idx]->update = 1;
			}
			qSize += pFile->size;
		}
		else
		{
			if (pFile->crc != second->second->crc || pFile->time != second->second->time || 
				 pFile->attr != second->second->attr)
			{
				pFile->update = 1;
				for (DWORD idx=0; idx<pFile->blknum; idx++)
				{
					if (idx<second->second->blknum)
					{
						if (vNewBlocks[pFile->crcoff+idx]->crc != vOldBlocks[second->second->crcoff+idx]->crc)
						{
							vNewBlocks[pFile->crcoff+idx]->update = 1;
							qSize += vNewBlocks[pFile->crcoff + idx]->blksize;
						}
					}
					else
					{
						vNewBlocks[pFile->crcoff+idx]->update = 1;
						qSize += vNewBlocks[pFile->crcoff+idx]->blksize;
					}
				}
			}
		}
	}
}

DWORD Idxfile_GetVersion(LPCWSTR file)
{
	HANDLE hFile = CreateFileW(file, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return -1;

	DWORD dwSize = GetFileSize(hFile, NULL);
	tagIdxHeahder header = {0};
	DWORD dwReadBytes = 0;
	if (dwSize < sizeof(tagIdxHeahder) || !ReadFile(hFile, &header, sizeof(tagIdxHeahder), &dwReadBytes, NULL))
	{
		CloseHandle(hFile);
		return -1;
	}

	DWORD offset = sizeof(tagIdxHeahder) + header.blknum * 32 + header.xmlsize;
	if (dwSize < offset + 32)
	{
		CloseHandle(hFile);
		return -1;
	}

	DWORD dwVersion = 0;
	if (!SetFilePointer(hFile, offset, NULL, FILE_BEGIN) || 
		!ReadFile(hFile, &dwVersion, sizeof(DWORD), &dwReadBytes, NULL))
	{
		CloseHandle(hFile);
		return -1;
	}

	CloseHandle(hFile);
	return dwVersion;
}

DWORD Idxfile_GetVersion(char* pfile, DWORD length)
{
	tagIdxHeahder* pheader = reinterpret_cast<tagIdxHeahder*>(pfile);
	if (length < sizeof(tagIdxHeahder) ||
		length < sizeof(tagIdxHeahder) + pheader->blknum * 32 + pheader->xmlsize + 32)
		return -1;

	return *(DWORD*)(&pfile[sizeof(tagIdxHeahder) + pheader->blknum * 32 + pheader->xmlsize]);
}

void Idxfile_DeleteMorefile(const std::wstring& dir, int baselen, std::map<std::wstring, tagIdxFile*>& mFiles, IDXCALLBACK pfnCallback, void* pCookie)
{
	WIN32_FIND_DATAW wfd = {0};
	HANDLE hFinder = FindFirstFileW((dir + L"*.*").c_str(), &wfd);
	if (hFinder == INVALID_HANDLE_VALUE)
		return ;

	std::vector <std::wstring> subdirs;
	do 
	{
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//filter dot directory.
			if (!((wfd.cFileName[0] == L'.') && 
				   ((wfd.cFileName[1] == 0) || (wfd.cFileName[1] == L'.' && wfd.cFileName[2] == 0))))
			{
				std::wstring newdir = dir + wfd.cFileName + L"\\";
				Idxfile_DeleteMorefile(newdir, baselen, mFiles, pfnCallback, pCookie);

				//delete more directory.
				if (mFiles.end() == mFiles.find(newdir.substr(baselen)))
				{
					SetFileAttributesW(newdir.c_str(), FILE_ATTRIBUTE_NORMAL);
					RemoveDirectoryW(newdir.c_str());
				}
			}
		}
		else
		{
			std::wstring file	= dir + wfd.cFileName;
			//delte more file.
			if (mFiles.end() == mFiles.find(file.substr(baselen)) && wcsicmp(wfd.cFileName, L"i8desk.idx") != 0)
			{
				SetFileAttributesW(file.c_str(), FILE_ATTRIBUTE_NORMAL);
				DeleteFileW(file.c_str());
				if (pfnCallback != NULL && !pfnCallback(pCookie, file.substr(baselen).c_str()))
					break;
			}
		}
	}while (FindNextFileW(hFinder, &wfd));
	FindClose(hFinder);
}

void Idxfile_DeleteMorefile(LPCWSTR dir, std::map<std::wstring, tagIdxFile*>& mFiles, IDXCALLBACK pfnCallback, void* pCookie)
{
	wchar_t szDir[MAX_PATH] = {0};
	wcscpy(szDir, dir);
	PathAddBackslashW(szDir);

	std::wstring strdir(szDir);
	Idxfile_DeleteMorefile(strdir, strdir.size(), mFiles, pfnCallback, pCookie);
}


unsigned long g_Crc32Table[256] = {
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L, 
	0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
	0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
	0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
	0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
	0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
	0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
	0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
	0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
	0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
	0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
	0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
	0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
	0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
	0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
	0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
	0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
	0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
	0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
	0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
	0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
	0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
	0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
	0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
	0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
	0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
	0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
	0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
	0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
	0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
	0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
	0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
	0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
	0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
	0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
	0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
	0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
	0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
	0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
	0x2d02ef8dL
};

inline void CalcCrc32(BYTE byte, DWORD& dwCrc32)
{
	dwCrc32 = ((dwCrc32) >> 8) ^ g_Crc32Table[(byte) ^ ((dwCrc32) & 0x000000FF)];
}

DWORD Idxfile_BlockCrc(unsigned char* pdata, int length)
{
	DWORD  crc(0xffffffff);
	while(length--)
		crc = (crc >> 8) ^ g_Crc32Table[(crc & 0xFF) ^ *pdata++];
	return crc^0xffffffff;
}
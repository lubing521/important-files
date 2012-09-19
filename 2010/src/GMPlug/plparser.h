#ifndef PLPARSER_H
#define PLPARSER_H

using namespace MSXML2;

namespace i8desk {

class ZPLContentHandler : public ISAXContentHandler
{
	enum PL_ETYPE 
	{
		PL_ETYPE_MAIN = 1,
		PL_ETYPE_PRODUCTS,
		PL_ETYPE_PRODUCT,
		PL_ETYPE_BASE,
		PL_ETYPE_MATCH,
		PL_ETYPE_UPDATE,
		PL_ETYPE_SERVERLIST
	};
public:
	ZPLContentHandler()
	{
		ASSERT(m_RootLock.Create());

		m_ElementTypes[L"main"]			= PL_ETYPE_MAIN;
		m_ElementTypes[L"products"]		= PL_ETYPE_PRODUCTS;
		m_ElementTypes[L"product"]		= PL_ETYPE_PRODUCT;
		m_ElementTypes[L"base"]			= PL_ETYPE_BASE;
		m_ElementTypes[L"match"]		= PL_ETYPE_MATCH;
		m_ElementTypes[L"update"]		= PL_ETYPE_UPDATE;
		m_ElementTypes[L"serverlist"]	= PL_ETYPE_SERVERLIST;

		m_AttributeTypes[L"gid"]		  = MASK_TGAME_GID;
		m_AttributeTypes[L"pid"]		  = MASK_TGAME_PID;
		m_AttributeTypes[L"play"]		  = MASK_TGAME_I8PLAY;
		m_AttributeTypes[L"type"]		  = MASK_TGAME_DEFCLASS;
		m_AttributeTypes[L"name"]		  =	MASK_TGAME_NAME;
		m_AttributeTypes[L"exe"]		  =	MASK_TGAME_GAMEEXE;
		m_AttributeTypes[L"size"]		  =	MASK_TGAME_SIZE;
		m_AttributeTypes[L"SourceTypeName"]=MASK_TGAME_GAMESOURCE;
		m_AttributeTypes[L"priority"]	  =	MASK_TGAME_PRIORITY;
		m_AttributeTypes[L"DownPriority"] =	MASK_TGAME_DOWNPRIORITY;
		m_AttributeTypes[L"showpriority"] =	MASK_TGAME_SHOWPRIORITY;
		m_AttributeTypes[L"savefilter"]	  =	MASK_TGAME_SAVEFILTER;
		m_AttributeTypes[L"url"]		  =	MASK_TGAME_URL;
		m_AttributeTypes[L"toolbar"]	  =	MASK_TGAME_TOOLBAR;
		m_AttributeTypes[L"desklnk"]	  =	MASK_TGAME_DESKLNK;
		m_AttributeTypes[L"param"]		  =	MASK_TGAME_PARAM;
		m_AttributeTypes[L"memo"]		  =	MASK_TGAME_COMMENT;

		m_AttributeTypes[L"product_url"]  =	MASK_TGAME_URL1;

		m_AttributeTypes[L"miniorfile"]   = MASK_TGAME_MATCHFILE;
		m_AttributeTypes[L"idcver"]		  =	MASK_TGAME_IDCVER;
		m_AttributeTypes[L"IdcUptDate"]   =	MASK_TGAME_IDCUPTDATE;
		m_AttributeTypes[L"idcclick"]     =	MASK_TGAME_IDCCLICK;

		m_AttributeTypes[L"auto"]         =	MASK_TGAME_AUTO;
		m_AttributeTypes[L"declinectr"]   =	MASK_TGAME_DECLINECTR;
		m_AttributeTypes[L"hide"]         =	MASK_TGAME_HIDE;
		m_AttributeTypes[L"LocalPath"]    =	MASK_TGAME_LOCALPATH;

		m_AttributeTypes[L"lock"]		  =	MASK_TGAME_LOCK;
		m_AttributeTypes[L"idcName"]      =	MASK_TGAME_IDCNAME;
		m_AttributeTypes[L"agentSetUpt"]  =	MASK_TGAME_AGENTSETUPT;
		m_AttributeTypes[L"syncName"]	  =	MASK_TGAME_SYNCNAME;
		m_AttributeTypes[L"adddate"]	  =	MASK_TGAME_ADDDATE;

		m_bNewVersion = FALSE;
	}

	void Init(IGameTable *pGameTable, ISysOpt *pSysOpt)
	{
		m_pGameTable = pGameTable;
		m_pSysOpt = pSysOpt;
	}

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
	HRESULT __stdcall raw_putDocumentLocator (
	/*[in]*/ struct ISAXLocator * pLocator ) 
	{
		return S_OK;
	}
	HRESULT __stdcall raw_startDocument ( ) 
	{
		return S_OK;
	}
	HRESULT __stdcall raw_endDocument ( ) 
	{
		return S_OK;
	}
	HRESULT __stdcall raw_startPrefixMapping (
	/*[in]*/ unsigned short * pwchPrefix,
	/*[in]*/ int cchPrefix,
	/*[in]*/ unsigned short * pwchUri,
	/*[in]*/ int cchUri ) 
	{
		return S_OK;
	}
	HRESULT __stdcall raw_endPrefixMapping (
		/*[in]*/ unsigned short * pwchPrefix,
		/*[in]*/ int cchPrefix ) 
	{
		return S_OK;
	}
	HRESULT __stdcall raw_startElement (
		/*[in]*/ unsigned short * pwchNamespaceUri,
		/*[in]*/ int cchNamespaceUri,
		/*[in]*/ unsigned short * pwchLocalName,
		/*[in]*/ int cchLocalName,
		/*[in]*/ unsigned short * pwchQName,
		/*[in]*/ int cchQName,
	/*[in]*/ struct ISAXAttributes * pAttributes ) 
	{
		switch (m_ElementTypes[(wchar_t *)pwchLocalName]) {
		case PL_ETYPE_PRODUCT:
			ResetCurrentGame();
			GetProductAttr(pAttributes);
			break;
		case PL_ETYPE_BASE:
			GetProductAttr(pAttributes);
			break;
		case PL_ETYPE_MATCH:
			GetProductAttr(pAttributes);
			break;
		case PL_ETYPE_UPDATE:
			GetProductAttr(pAttributes);
			break;
		case PL_ETYPE_MAIN:
			return CheckVersion(pAttributes);
		case PL_ETYPE_PRODUCTS:
			break;
		case PL_ETYPE_SERVERLIST:
			return SetPLRoot(pAttributes);
		}
		return S_OK;
	}
	HRESULT __stdcall raw_endElement (
		/*[in]*/ unsigned short * pwchNamespaceUri,
		/*[in]*/ int cchNamespaceUri,
		/*[in]*/ unsigned short * pwchLocalName,
		/*[in]*/ int cchLocalName,
		/*[in]*/ unsigned short * pwchQName,
		/*[in]*/ int cchQName ) 
	{
		switch (m_ElementTypes[(wchar_t *)pwchLocalName]) {
		case PL_ETYPE_PRODUCT:
			SaveCurrentGame();
			break;
		case PL_ETYPE_BASE:
			break;
		case PL_ETYPE_MATCH:
			break;
		case PL_ETYPE_UPDATE:
			break;
		case PL_ETYPE_MAIN: 
			break; 
		case PL_ETYPE_PRODUCTS:
			break;
		}
		return S_OK;
	}
	HRESULT __stdcall raw_characters (
		/*[in]*/ unsigned short * pwchChars,
		/*[in]*/ int cchChars ) 
	{
		return S_OK;
	}
	HRESULT __stdcall raw_ignorableWhitespace (
		/*[in]*/ unsigned short * pwchChars,
		/*[in]*/ int cchChars ) 
	{
		return S_OK;
	}
	HRESULT __stdcall raw_processingInstruction (
		/*[in]*/ unsigned short * pwchTarget,
		/*[in]*/ int cchTarget,
		/*[in]*/ unsigned short * pwchData,
		/*[in]*/ int cchData ) 
	{
		return S_OK;
	}
	HRESULT __stdcall raw_skippedEntity (
		/*[in]*/ unsigned short * pwchName,
		/*[in]*/ int cchName ) 
	{
		return S_OK;
	}

	BOOL IsNewVersion(void)
	{
		return m_bNewVersion;
	}

	std::string GetPLVersion(void)
	{
		char ver[16] = {0};

		I8_GUARD(ZGuard, ZSync, m_VersionLock);

		if (WCToMB(m_wstrPLVersion, ver))
			return std::string(ver);

		return "";
	}

	std::string GetPLRoot(void)
	{
		I8_GUARD(ZGuard, ZSync, m_RootLock);

		return WCToMB(m_wstrPLRoot);
	}

	void SetForceUpdate(BOOL b) { m_bForceUpdate = b; }

private:
	HRESULT CheckVersion(struct ISAXAttributes * pAttributes)
	{
		I8_GUARD(ZGuard, ZSync, m_VersionLock);

		ushort *wcs = 0;
		int cchValue = 0;
		pAttributes->raw_getIndexFromName(0, 0, (ushort*)L"ver", 6, &cchValue);
		pAttributes->raw_getValue(cchValue, &wcs, &cchValue);
		std::wstring wstrPLVersion((wchar_t*)wcs, cchValue);
	
		//还回S_FALSE,让pl停止解析
		if ( wstrPLVersion < m_wstrPLVersion)
		{
			//表示过滤掉错误的pl.
			m_bNewVersion = FALSE;
			I8_INFOR((LM_INFO, I8_TEXT("接收到错误的PL！\n")));
			return S_FALSE;
		}
		else if (wstrPLVersion == m_wstrPLVersion)
		{
			//若版本相同，则要求强制解析才解析pl.
			m_bNewVersion = FALSE;
			return m_bForceUpdate ? S_OK : S_FALSE;
		}
		
		//若有新版本，则一定要解析pl.
		m_bNewVersion = TRUE;
		m_wstrPLVersion = wstrPLVersion;
		return S_OK;
 	}

	HRESULT SetPLRoot(struct ISAXAttributes * pAttributes)
	{
		I8_GUARD(ZGuard, ZSync, m_RootLock);

		ushort *wcs = 0;
		int cchValue = 0;

		if (m_pSysOpt->GetOpt(OPT_D_LINETYPE, 0L) != 0) 
			pAttributes->raw_getIndexFromName(0, 0, (ushort*)L"uni", 6, &cchValue);
		else
			pAttributes->raw_getIndexFromName(0, 0, (ushort*)L"tel", 6, &cchValue);

		if (SUCCEEDED(pAttributes->raw_getValue(cchValue, &wcs, &cchValue)) && cchValue > 0) {
			if ((wchar_t)wcs[cchValue - 1] == L'/')
				cchValue--;
			m_wstrPLRoot = std::wstring((wchar_t*)wcs, cchValue);
		}
		
		return S_OK;
	}

	void GetProductAttr(struct ISAXAttributes * pAttributes)
	{
		uint64 mask;
		ushort *wcs;
		int cchLocalName;

		int nLength;
		pAttributes->raw_getLength(&nLength);

		for (int i = 0; i < nLength; i++) 
		{
			pAttributes->raw_getLocalName(i, &wcs, &cchLocalName);
			mask = m_AttributeTypes[std::wstring((wchar_t*)wcs, cchLocalName)];
			m_mask |= mask;

			pAttributes->raw_getValue(i, &wcs, &cchLocalName);
			std::wstring value((wchar_t*)wcs, cchLocalName);

			switch (mask) 
			{
			case MASK_TGAME_GID:
				m_g.GID = ::_wtoi(value.c_str());
				break;
			case MASK_TGAME_PID:
				m_g.PID = ::_wtoi(value.c_str());
				break;
			case MASK_TGAME_I8PLAY:
				m_g.I8Play = ::_wtoi(value.c_str());
				break;
			case MASK_TGAME_DEFCLASS:
				WCToMB(value, m_g.DefClass);
				break;
			case MASK_TGAME_NAME:
				WCToMB(value, m_g.Name);
				break;
			case MASK_TGAME_GAMEEXE:
				WCToMB(value, m_g.GameExe);
				WCToMB(value, m_g.GameExe2);
				m_mask &= ~mask;
				m_mask |= MASK_TGAME_GAMEEXE2;
				break;
			case MASK_TGAME_SIZE:
				m_g.Size = (int)(::_wtoi64(value.c_str()) / 1024);
				break;
			case MASK_TGAME_PRIORITY:
				m_g.Priority = ::_wtoi(value.c_str());
				m_mask &= ~mask;
				break;
			case MASK_TGAME_DOWNPRIORITY:
				m_g.DownPriority = ::_wtoi(value.c_str());
				break;
			case MASK_TGAME_SHOWPRIORITY:
				m_g.ShowPriority = ::_wtoi(value.c_str());
				break;
			case MASK_TGAME_SAVEFILTER:
				WCToMB(value, m_g.SaveFilter);
				WCToMB(value, m_g.SaveFilter2);
				m_mask &= ~mask;
				if (m_g.SaveFilter2[0])
					m_mask |= MASK_TGAME_SAVEFILTER2;
				break;
			case MASK_TGAME_URL:
				WCToMB(value, m_g.URL);
				break;
			case MASK_TGAME_TOOLBAR:
				m_g.Toolbar = ::_wtoi(value.c_str());
				if (!m_g.Toolbar)m_mask &= ~mask;
				break;
			case MASK_TGAME_DESKLNK:
				m_g.DeskLnk = ::_wtoi(value.c_str());
				if (m_g.DeskLnk == 0)
					m_mask &= ~mask;
				break;
			case MASK_TGAME_PARAM:
				WCToMB(value, m_g.Param);
				WCToMB(value, m_g.Param2);
				m_mask &= ~mask;
				m_mask |= MASK_TGAME_PARAM2;
				break;
			case MASK_TGAME_COMMENT:
				WCToMB(value, m_g.Comment);
				break;
			case MASK_TGAME_GAMESOURCE:
				WCToMB(value, m_g.GameSource);
				break;
			case MASK_TGAME_MATCHFILE:
				WCToMB(value, m_g.MatchFile);
				break;
			case MASK_TGAME_IDCVER:
				m_g.IdcVer = ::_wtoi(value.c_str());
				break;
			case MASK_TGAME_IDCUPTDATE:
				m_g.IdcUptDate = ::_wtoi(value.c_str());
				break;
			case MASK_TGAME_IDCCLICK:
				m_g.IdcClick = ::_wtoi(value.c_str());
				break;
			case MASK_TGAME_AUTO:
				m_g.Auto = ::_wtoi(value.c_str());
				break;
			case MASK_TGAME_DECLINECTR:
				m_g.Declinectr = ::_wtoi(value.c_str());
				break;
			case MASK_TGAME_HIDE:
				m_g.hide = ::_wtoi(value.c_str());
				break;
			case MASK_TGAME_LOCALPATH:
				WCToMB(value, m_g.LocalPath);
				break;
			case MASK_TGAME_LOCK:
				m_bLockGame = ::_wtoi(value.c_str());
				m_mask &= ~mask;
				break;
			case MASK_TGAME_SYNCNAME:
				m_bSyncName = ::_wtoi(value.c_str());
				m_mask &= ~mask;
				break;

			// ur1~url4
			case MASK_TGAME_URL1:
				{
					// 分解为4个url
					int nIndex = 0;
					int nCount = 0;
					std::wstring strUrl1;
					std::wstring strUrl2;
					std::wstring strUrl3;
					std::wstring strUrl4;

					if( nIndex != -1 )
					{
						nCount	= value.find_first_of(_T('|'));
						strUrl1 = value.substr(nIndex, nCount - nIndex - 1);
						nIndex	= nCount;
					}

					if( nIndex != -1 )
					{
						nCount	= value.find_first_of(_T('|'), nIndex + 1);
						strUrl2 = value.substr(nIndex + 1, nCount - nIndex - 1);
						nIndex	= nCount;
					}

					if( nIndex != -1 )
					{
						nCount	= value.find_first_of(_T('|'), nIndex + 1);
						strUrl3 = value.substr(nIndex + 1, nCount - nIndex - 1);
						nIndex	= nCount;
					}

					if( nIndex != -1 )
					{
						nCount	= value.find_first_of(_T('|'), nIndex + 1);
						strUrl4 = value.substr(nIndex + 1, nCount - nIndex - 1);
						nIndex	= nCount;
					}
	
					WCToMB(strUrl1, m_g.url1);
					WCToMB(strUrl2, m_g.url2);
					WCToMB(strUrl3, m_g.url3);
					WCToMB(strUrl4, m_g.url4);
				}
				
				break;

			case MASK_TGAME_AGENTSETUPT:
				m_g.AutoUpt = ::_wtoi(value.c_str());
				if (m_g.AutoUpt == 1)
					m_mask |= MASK_TGAME_AUTOUPT;
				m_mask &= ~mask;
				break;
			case MASK_TGAME_ADDDATE:
				m_g.AddDate = ::_wtoi(value.c_str());
				break;
			default:
				m_mask &= ~mask;
				break;
			}
		}
	}

	void ResetCurrentGame(void)
	{
		memset(&m_g, 0, sizeof(m_g));
		m_mask = 0;
		m_bLockGame = 0;
		m_bSyncName = 0;
	}

	void SaveCurrentGame(void)
	{
		//检查是否有锁定游戏的属性
		if (m_bLockGame) 
		{
			m_pGameTable->Delete(m_g.GID);
			return;
		}

		//检查是否有强制改名的属性
		if (!m_bSyncName) 
			m_mask &= ~MASK_TGAME_NAME; 

		// 已经设置过则取消设置下载优先级
		if (m_pGameTable->GetData(m_g.GID, &g, 0) == 0 && g.DownPriority != 0) 
		{
			m_mask &= ~MASK_TGAME_DOWNPRIORITY; 
		}

		m_pGameTable->Ensure(&m_g, m_mask);
	}

private:
	int m_bLockGame;
	int m_bSyncName;
	db::tGame g; //临时用的变量
	db::tGame m_g; //当前游戏变量
	uint64 m_mask; //当前游戏字段
	IGameTable *m_pGameTable;
	std::map<std::wstring, uint64> m_ElementTypes; 
	std::map<std::wstring, uint64> m_AttributeTypes;

	ZSync m_VersionLock;
	BOOL m_bNewVersion;
	std::wstring m_wstrPLVersion;

	ISysOpt *m_pSysOpt;
	ZSync m_RootLock;
	std::wstring m_wstrPLRoot;

	BOOL m_bForceUpdate;
};


} // namespace i8desk

#endif //PLPARSER_H
#include "stdafx.h"
#include "plparser.h"
#include "LogHelper.h"
#include "PLInfo.h"
#include "GameTableCache.h"
#include "ForceGameZip.h"

#include "../../../include/tablestruct.h"
#include "../../../include/I8Type.hpp"



namespace i8desk 
{

	namespace detail
	{
		// ------------------------------------------
		// 映射游戏类别
		struct ClassMap
		{
			stdex::tString classMap_[256];

			ClassMap()
			{
				classMap_['a'] = CLASS_WL_GUID;  
				classMap_['b'] = CLASS_DJ_GUID;
				classMap_['c'] = CLASS_XX_GUID;
				classMap_['d'] = CLASS_DZ_GUID;
				classMap_['e'] = CLASS_QP_GUID;
				classMap_['f'] = CLASS_PL_GUID;
				classMap_['g'] = CLASS_LT_GUID;
				classMap_['h'] = CLASS_CY_GUID;
				classMap_['i'] = CLASS_YY_GUID;
				classMap_['j'] = CLASS_GP_GUID;
				classMap_['k'] = CLASS_WY_GUID;
			}

			const stdex::tString &operator[](size_t index) const
			{
				if( index < 0 || index >= 256 )
					throw std::out_of_range("游戏类别超出类别区间");

				return classMap_[index];
			}
		};

		static ClassMap classMap;


		// -----------------------------------
		// 映射PL版本字段类别

		enum PL_ETYPE 
		{
			PL_ETYPE_DOCUMENT = 1,
			PL_ETYPE_GAMES,
			PL_ETYPE_GAME,
			PL_ETYPE_BASE,
			PL_ETYPE_PROPERTY,
			PL_ETYPE_UPDATE
		};

		struct ElementType
		{
			std::map<std::wstring, uint64> elementTypes_;

			ElementType()
			{
				elementTypes_[L"document"]	=	PL_ETYPE_DOCUMENT;
				elementTypes_[L"games"]		=	PL_ETYPE_GAMES;
				elementTypes_[L"game"]		=	PL_ETYPE_GAME;
				elementTypes_[L"base"]		=	PL_ETYPE_BASE;
				elementTypes_[L"property"]	=	PL_ETYPE_PROPERTY;
				elementTypes_[L"update"]	=	PL_ETYPE_UPDATE;
			}

			uint64 operator[](const std::wstring &key)
			{
				return elementTypes_[key];
			}
		};

		static ElementType elementTypes;
	}



	// -------------------------------------------------

	PLParser::PLParser(IGameTable *pGameTable, ISysOpt *pSysOpt, const PLComplate &complate)
		: gameTable_(pGameTable)
		, sysOpt_(pSysOpt)
		, idcRename_(0)
		, newVersion_(0)
		, forceUpdate_(0)
		, delFlag_(0)
		, complate_(complate)
	{
		assert(gameTable_);
		assert(sysOpt_);
	}


	HRESULT  PLParser::my_raw_startElement (
		/*[in]*/ unsigned short * pwchNamespaceUri,
		/*[in]*/ int cchNamespaceUri,
		/*[in]*/ unsigned short * pwchLocalName,
		/*[in]*/ int cchLocalName,
		/*[in]*/ unsigned short * pwchQName,
		/*[in]*/ int cchQName,
	/*[in]*/ struct ISAXAttributes * pAttributes ) 
	{
		switch (detail::elementTypes[(wchar_t *)pwchLocalName]) 
		{
		case detail::PL_ETYPE_GAME:
			{
				_ResetCurrentGame();
				_GetGameAttr(pAttributes);
			}
			break;
		case detail::PL_ETYPE_BASE:
			_GetBaseAttr(pAttributes);
			break;
		case detail::PL_ETYPE_PROPERTY:
			_GetPropertyAttr(pAttributes);
			break;
		case detail::PL_ETYPE_UPDATE:
			_GetUpdataAttr(pAttributes);
			break;
		case detail::PL_ETYPE_DOCUMENT:
			_GetDocumentAttr(pAttributes);
			break;
		}
		return S_OK;
	}
	HRESULT  PLParser::my_raw_endElement (
		/*[in]*/ unsigned short * pwchNamespaceUri,
		/*[in]*/ int cchNamespaceUri,
		/*[in]*/ unsigned short * pwchLocalName,
		/*[in]*/ int cchLocalName,
		/*[in]*/ unsigned short * pwchQName,
		/*[in]*/ int cchQName ) 
	{
		switch (detail::elementTypes[(wchar_t *)pwchLocalName]) 
		{
		case detail::PL_ETYPE_GAME:
			_SaveCurrentGame();
			break;
		case detail::PL_ETYPE_BASE:
			break;
		case detail::PL_ETYPE_PROPERTY:
			break;
		case detail::PL_ETYPE_UPDATE:
			break;
		case detail::PL_ETYPE_DOCUMENT: 
			break; 
		case detail::PL_ETYPE_GAMES:
			break;
		}
		return S_OK;
	}


	HRESULT PLParser::_CheckVersion(const std::wstring &version)
	{
		long long plOldVer = stdex::ToNumber<long long>(PLInfo::GetInstance().PLVersion());
		long long plNewVer = stdex::ToNumber<long long>(version);


		// PL当前版本比前一版本旧,错误!
		if( plNewVer < plOldVer )
		{
			// 表示过滤掉错误的pl.
			::InterlockedExchange(&newVersion_, 0);

			Log(LM_INFO, _T("接收到错误的PL！\n"));
			return S_FALSE;
		}
		else if( plNewVer == plOldVer )
		{
			// 若版本相同，则要求强制解析才解析pl.
			::InterlockedExchange(&newVersion_, 0);
			return forceUpdate_ ? S_OK : S_FALSE;
		}
		else
		{
			// 若有新版本，则一定要解析pl.
			::InterlockedExchange(&newVersion_, 1);
			PLInfo::GetInstance().PLVersion(version);

			return S_OK;
		}
		
		return S_FALSE;
 	}

	void PLParser::_SetPLRoot(ushort *wcs,int cchValue)
	{
		if( cchValue >= 1 && (wchar_t)wcs[cchValue - 1] == L'/' )
			cchValue--;
		PLInfo::GetInstance().PLRoot(std::wstring((wchar_t*)wcs, cchValue));
	}

	HRESULT PLParser::_GetDocumentAttr(struct ISAXAttributes * pAttributes)
	{
		ushort *wcs = 0;
		ushort wcsl = 0;
		int cchLocalName = 0;

		int nLength = 0;
		pAttributes->raw_getLength(&nLength);

		for(int i = 0; i < nLength; i++) 
		{
			pAttributes->raw_getLocalName(i, &wcs, &cchLocalName);
			wcsl = wcs[cchLocalName - 1];

			pAttributes->raw_getValue(i, &wcs, &cchLocalName);
			std::wstring value((wchar_t*)wcs, cchLocalName);

			switch (wcsl) 
			{
			case 'v'://PL版本号
				if( _CheckVersion(value) != S_OK ) 
					return S_FALSE;

				break;
			case 't'://电信网址
				if( sysOpt_->GetOpt(OPT_D_LINETYPE, 0L) == 0 ) 
					_SetPLRoot(wcs, cchLocalName);
				break;
			case 'n'://网通网址
				if( sysOpt_->GetOpt(OPT_D_LINETYPE, 0L) == 1 ) 
					_SetPLRoot(wcs, cchLocalName);
				break;
			}
		}
		return S_OK;
	}


	void PLParser::_GetGameAttr(struct ISAXAttributes * pAttributes)
	{
		ushort *wcs = 0;
		ushort wcsl = 0;
		int cchLocalName = 0;
		int  deltype = 0;
		int nLength = 0;
		pAttributes->raw_getLength(&nLength);

		for(int i = 0; i < nLength; i++) 
		{
			pAttributes->raw_getLocalName(i, &wcs, &cchLocalName);
			if( cchLocalName < 1 )
			{
				Log(LM_ERROR, _T("Game Attribute 配置信息出错\n"));
				return;
			}

			wcsl = wcs[cchLocalName - 1];

			pAttributes->raw_getValue(i, &wcs, &cchLocalName);
			std::wstring value((wchar_t*)wcs, cchLocalName);
			switch (wcsl) 
			{
			case 'i'://游戏Gid
				curGame_.GID = ::_wtoi(value.c_str());
				zipGame_.gid_ = curGame_.GID;
				mask_ |= MASK_TGAME_GID;
				break;
			case 'a'://游戏关联ID
				curGame_.PID = ::_wtoi(value.c_str());
				mask_ |= MASK_TGAME_PID;
				break;
			case 'v'://增值游戏
				curGame_.I8Play = ::_wtoi(value.c_str());
				mask_ |= MASK_TGAME_I8PLAY;
				break;
			case 't'://游戏类别
				{
					assert(value.length() > 0);
					const TCHAR index = value[0];
					
					try
					{
						utility::Strcpy(curGame_.CID, detail::classMap[index]);
					}
					catch(std::exception &e)
					{
						Log(LM_ERROR, CA2T(e.what()));
						break;
					}
					
					mask_ |= MASK_TGAME_CID;
				}
				break;
			case 'd'://删除游戏的方式
				deltype = ::_wtoi(value.c_str());
				if( deltype == 1 )	
				{
					delFlag_ = DelIdc;
					curGame_.Hide = 1;
					mask_ |= MASK_TGAME_HIDE;
				}
				if( deltype == 2 ) 
				{
					delFlag_ = DelAll;
					curGame_.Hide = 1;
					mask_ |= MASK_TGAME_HIDE;
				}
				if( deltype == 3 )  
				{
					delFlag_ = DelNone;
					curGame_.StopRun = DelLock;
				}
				mask_ |= MASK_TGAME_STOPRUN;
				break;
			case 'f':
				zipGame_.forceGid_ = ::_wtoi(value.c_str());
				break;
			case 'm':
				zipGame_.unZipTime_ = ::_wtoi(value.c_str());
				break;
			default:
				break;
			}
		}
	}

	void PLParser::_GetBaseAttr(struct ISAXAttributes * pAttributes)
	{
		ushort *wcs = 0;
		ushort wcsl = 0;
		int cchLocalName = 0;

		int nLength = 0;
		pAttributes->raw_getLength(&nLength);

		for(int i = 0; i < nLength; i++) 
		{
			pAttributes->raw_getLocalName(i, &wcs, &cchLocalName);

			if( cchLocalName < 1 )
			{
				Log(LM_ERROR, _T("Base Attribute 配置信息出错\n"));
				return;
			}
			wcsl = wcs[cchLocalName - 1];

			pAttributes->raw_getValue(i, &wcs, &cchLocalName);
			std::wstring value((wchar_t*)wcs, cchLocalName);

			switch (wcsl) 
			{
			case 'n'://游戏名
				utility::Strcpy(curGame_.Name, value.c_str());
				mask_ |= MASK_TGAME_NAME;
				break;
			case 'e'://游戏的启动程序文件
				utility::Strcpy(curGame_.Exe, value.c_str());
				utility::Strcpy(GameTableCache::Instance(curGame_.GID).Exe, curGame_.Exe);
				break;
			case 'w':
				GameTableCache::Instance(curGame_.GID).GameExVersion = _wtoi(value.c_str());
				break;
			case 's'://游戏大小
				curGame_.Size = (int)(::_wtoi64(value.c_str())/1024);
				mask_ |= MASK_TGAME_SIZE;
				break;
			case 'm'://游戏备注
				utility::Strcpy(curGame_.Memo, value.c_str());
				mask_ |= MASK_TGAME_MEMO;
				break;
			case 'f'://游戏来源
				utility::Strcpy(curGame_.GameSource, value.c_str());
				mask_ |= MASK_TGAME_GAMESOURCE;
				break;
			case 't'://第二特征文件
				utility::Strcpy(curGame_.TraitFile, value.c_str());
				mask_ |= MASK_TGAME_TRAITFILE;
				break;
			case 'd'://中心添加日期
				curGame_.IdcAddDate = ::_wtoi(value.c_str());
				mask_ |= MASK_TGAME_IDCADDDATE;
				break;
			case 'c'://中心点击数
				curGame_.IdcClick = ::_wtoi(value.c_str());
				mask_ |= MASK_TGAME_IDCCLICK;
				break;
			default:
				break;
			}
		}
	}

	void PLParser::_GetPropertyAttr(struct ISAXAttributes * pAttributes)
	{
		ushort *wcs = 0;
		ushort wcsl = 0;
		int cchLocalName = 0;

		int nLength = 0;
		pAttributes->raw_getLength(&nLength);

		for(int i = 0; i < nLength; i++) 
		{
			pAttributes->raw_getLocalName(i, &wcs, &cchLocalName);
			if( cchLocalName < 1 )
			{
				Log(LM_ERROR, _T("Property Attribute 配置信息出错\n"));
				return;
			}

			wcsl = wcs[cchLocalName - 1];
			pAttributes->raw_getValue(i, &wcs, &cchLocalName);
			std::wstring value((wchar_t*)wcs, cchLocalName);

			switch (wcsl) 
			{
			case 'f'://游戏的存档文件
				utility::Strcpy(curGame_.SaveFilter, value);
				mask_ |= MASK_TGAME_SAVEFILTER;
				break;
			case 't'://中心要显示到菜单的工具栏
				curGame_.Toolbar = ::_wtoi(value.c_str());
				if( curGame_.Toolbar )
					mask_ |= MASK_TGAME_TOOLBAR;
				break;
			case 'l'://客户端在桌面创建该游戏的快速方式
				curGame_.DeskLink = ::_wtoi(value.c_str());
				if( curGame_.DeskLink )
					mask_ |= MASK_TGAME_DESKLINK;
				break;
			case 'p'://游戏运行参数
				utility::Strcpy(curGame_.Param, value);
				utility::Strcpy(GameTableCache::Instance(curGame_.GID).Param, curGame_.Param);
				break;
			case 'r'://表示中心是否要改名
				idcRename_ = ::_wtoi(value.c_str());
				break;
			case 'd'://客户机内网更新空间不足时，可删除该游戏
				curGame_.EnDel = ::_wtoi(value.c_str());
				mask_ |= MASK_TGAME_ENDEL;
				break;
			case 'u'://强制更新
				curGame_.Repair = ::_wtoi(value.c_str());
				mask_ |= MASK_TGAME_REPAIR;
				break;
			case 'h': // 是否显示游戏
				curGame_.Hide = ::_wtoi(value.c_str());
				mask_ |= MASK_TGAME_HIDE;

			default:
				break;
			}
		}
	}

	void PLParser::_GetUpdataAttr(struct ISAXAttributes * pAttributes)
	{
		ushort *wcs = 0;
		ushort wcsl = 0;
		int cchLocalName = 0;

		int nLength = 0;
		pAttributes->raw_getLength(&nLength);

		for(int i = 0; i < nLength; i++) 
		{
			pAttributes->raw_getLocalName(i, &wcs, &cchLocalName);
			if( cchLocalName < 1 )
			{
				Log(LM_ERROR, _T("Updata Attribute 配置信息出错\n"));
				return;
			}

			wcsl = wcs[cchLocalName - 1];

			pAttributes->raw_getValue(i, &wcs, &cchLocalName);
			std::wstring value((wchar_t*)wcs, cchLocalName);

			switch(wcsl) 
			{
			case 'v'://表示中心版本号
				curGame_.IdcVer = ::_wtoi(value.c_str());
				mask_ |= MASK_TGAME_IDCVER;
				break;
			case 'f'://表示强推逻辑
				curGame_.Force = ::_wtoi(value.c_str());
				mask_ |= MASK_TGAME_FORCE;
				break;
			case 'd'://表示中心强推到网吧服务器上的目录
				utility::Strcpy(curGame_.ForceDir, value);
				mask_ |= MASK_TGAME_FORCEDIR;
				break;
			default:
				break;
			}
		}
	}
	void PLParser::_ResetCurrentGame()
	{
		memset(&curGame_, 0, sizeof(curGame_));
		memset(&zipGame_, 0, sizeof(zipGame_));

		mask_		= 0;
		idcRename_	= 0;
		delFlag_	= DelNone;
	}

	void PLParser::_SaveCurrentGame()
	{
		DeleteInfo::GetInstance().PushGame(curGame_.GID, delFlag_);

		db::tGame tmpGame;
		if( gameTable_->GetData(curGame_.GID, &tmpGame, 0) != 0 )
		{
			// 如果本地有此游戏且中心没有设置恢复名字,则取消更改名字
			if( tmpGame.Status == 1 && idcRename_ != 1 )
				mask_ &= ~MASK_TGAME_NAME;

			// 已经设置过则取消设置下载优先级
			if( tmpGame.Priority != 0 ) 
				mask_ &= ~MASK_TGAME_PRIORITY; 
		}
		gameTable_->Ensure(&curGame_, mask_);


		if( complate_ != 0 && 
			zipGame_.forceGid_ != 0 &&
			zipGame_.unZipTime_ != 0 )
		{
			complate_(zipGame_.gid_, zipGame_.forceGid_, zipGame_.unZipTime_);
		}
	}

}


#ifndef __PL_PARSER_H
#define __PL_PARSER_H


#import <msxml3.dll>
using namespace MSXML2;



namespace i8desk 
{

	class PLParser 
		: public ISAXContentHandler
	{
	public:
		typedef std::tr1::function<void(long, long, time_t)> PLComplate;

	private:
		IGameTable *gameTable_;
		ISysOpt *sysOpt_;

		int idcRename_;					// 中心是否改名
		DWORD delFlag_;					// 删除游戏方式
		volatile LONG newVersion_;		// 新版本
		volatile LONG forceUpdate_;		// 强制更新

		db::tGame curGame_;				// 当前游戏变量

		struct ZipGame
		{
			long gid_;
			long forceGid_;
			time_t unZipTime_;
		}zipGame_;						// 当前压缩包信息	

		uint64 mask_;					// 当前游戏字段

		PLComplate complate_;

	public:
		PLParser(IGameTable *pGameTable, ISysOpt *pSysOpt, const PLComplate &complate);

	public:
		HRESULT __stdcall QueryInterface(const IID &, void **)
		{
			return S_OK;
		};
		ULONG __stdcall AddRef(void)
		{
			return 1;
		};
		ULONG __stdcall Release(void)
		{
			return 1;
		};
		HRESULT __stdcall raw_putDocumentLocator (
		/*[in]*/ struct ISAXLocator * pLocator ) 
		{
			return S_OK;
		};
		HRESULT __stdcall raw_startDocument ( ) 
		{
			return S_OK;
		};
		HRESULT __stdcall raw_endDocument ( ) 
		{
			return S_OK;
		};
		HRESULT __stdcall raw_startPrefixMapping (
			/*[in]*/ unsigned short * pwchPrefix,
			/*[in]*/ int cchPrefix,
			/*[in]*/ unsigned short * pwchUri,
			/*[in]*/ int cchUri ) 
		{
			return S_OK;
		};
		HRESULT __stdcall raw_endPrefixMapping (
			/*[in]*/ unsigned short * pwchPrefix,
			/*[in]*/ int cchPrefix ) 
		{
			return S_OK;
		};
		HRESULT __stdcall raw_startElement (
			/*[in]*/ unsigned short * pwchNamespaceUri,
			/*[in]*/ int cchNamespaceUri,
			/*[in]*/ unsigned short * pwchLocalName,
			/*[in]*/ int cchLocalName,
			/*[in]*/ unsigned short * pwchQName,
			/*[in]*/ int cchQName,
		/*[in]*/ struct ISAXAttributes * pAttributes ) 
		{
			return my_raw_startElement ( pwchNamespaceUri,
				cchNamespaceUri,
				pwchLocalName,
				cchLocalName,
				pwchQName,
				cchQName,
				pAttributes ) ;
		};
		HRESULT __stdcall raw_endElement (/*[in]*/ unsigned short * pwchNamespaceUri,
			/*[in]*/ int cchNamespaceUri,
			/*[in]*/ unsigned short * pwchLocalName,
			/*[in]*/ int cchLocalName,
			/*[in]*/ unsigned short * pwchQName,
			/*[in]*/ int cchQName ) 
		{
			return my_raw_endElement (
				pwchNamespaceUri,
				cchNamespaceUri,
				pwchLocalName,
				cchLocalName,
				pwchQName,
				cchQName ) ;
		};
		HRESULT __stdcall raw_characters (
			/*[in]*/ unsigned short * pwchChars,
			/*[in]*/ int cchChars ) 
		{
			return S_OK;
		};
		HRESULT __stdcall raw_ignorableWhitespace (
			/*[in]*/ unsigned short * pwchChars,
			/*[in]*/ int cchChars ) 
		{
			return S_OK;
		};
		HRESULT __stdcall raw_processingInstruction (
			/*[in]*/ unsigned short * pwchTarget,
			/*[in]*/ int cchTarget,
			/*[in]*/ unsigned short * pwchData,
			/*[in]*/ int cchData ) 
		{
			return S_OK;
		};
		HRESULT __stdcall raw_skippedEntity (
			/*[in]*/ unsigned short * pwchName,
			/*[in]*/ int cchName ) 
		{
			return S_OK;
		};


		HRESULT my_raw_startElement (
			/*[in]*/ unsigned short * pwchNamespaceUri,
			/*[in]*/ int cchNamespaceUri,
			/*[in]*/ unsigned short * pwchLocalName,
			/*[in]*/ int cchLocalName,
			/*[in]*/ unsigned short * pwchQName,
			/*[in]*/ int cchQName,
		/*[in]*/ struct ISAXAttributes * pAttributes ) ;

		HRESULT my_raw_endElement (
			/*[in]*/ unsigned short * pwchNamespaceUri,
			/*[in]*/ int cchNamespaceUri,
			/*[in]*/ unsigned short * pwchLocalName,
			/*[in]*/ int cchLocalName,
			/*[in]*/ unsigned short * pwchQName,
			/*[in]*/ int cchQName ) ;


	public:
		// 检测是否为新版本
		bool IsNewVersion()
		{
			return ::InterlockedCompareExchange(&newVersion_, 0, 1) == 1;
		}

		// 设置强制更新
		void SetForceUpdate(bool force) 
		{ 
			::InterlockedExchange(&forceUpdate_, force ? 1 : 0);
		}

	private:
		HRESULT _CheckVersion(const std::wstring &version);
		void _SetPLRoot(ushort *wcs,int cchValue);
		HRESULT _GetDocumentAttr(struct ISAXAttributes * pAttributes);
		void _GetGameAttr(struct ISAXAttributes * pAttributes);
		void _GetBaseAttr(struct ISAXAttributes * pAttributes);
		void _GetPropertyAttr(struct ISAXAttributes * pAttributes);
		void _GetUpdataAttr(struct ISAXAttributes * pAttributes);


		void _ResetCurrentGame(void);
		void _SaveCurrentGame(void);

	};


} // namespace i8desk

#endif //PLPARSER_H
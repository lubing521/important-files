#include "stdafx.h"
#include "PLControl.h"


#include "../../../include/Zip/XUnzip.h"
#include "../../../include/Utility/utility.h"
#include "../../../include/Extend STL/StringAlgorithm.h"
#include "../../../include/Utility/ScopeExit.hpp"
#include "../../../include/Utility/MD5.h"

#include "LogHelper.h"
#include "GlobalFun.h"
#pragma comment(lib,"Frame.lib")




namespace i8desk
{

	namespace 
	{
		template<typename EventT>
		inline bool IsStop(EventT &event)
		{
			if( ::WaitForSingleObject(event, 0) == WAIT_OBJECT_0 )
				return true;

			return false;
		}


		template<typename ContainerT>
		inline void CallbackAll(const ContainerT &container)
		{
			struct CallbackFunctor
			{
				void operator()(const typename ContainerT::value_type &callback)
				{
					callback();
				}
			};

			std::for_each(container.begin(), container.end(), CallbackFunctor());
		}
	}


	PLControl::PLControl(ISysOpt *sysOpt, IRTDataSvr *rtDataSvr)
		: pSysOpt_(sysOpt) 
		, pRTDataSvr_(rtDataSvr)
		, rdr_(0)
	{
		assert(pSysOpt_);
		assert(pRTDataSvr_);
	}

	PLControl::~PLControl()
	{
	}

	void PLControl::Start(const PLComplate &plComplate)
	{
		if( !_Init(plComplate) )
		{
			std::ostringstream os;
			os << "初始化PLControl时创建事件失败! error = " << ::GetLastError();
			throw std::runtime_error(os.str());
		}
		refreshEvent_.SetEvent();
	}

	void PLControl::Stop()
	{
		exitEvent_.SetEvent();

		thread_.Stop();

		Log(LM_DEBUG, _T("PLControl 退出成功"));
	}

	void PLControl::SetForceRefresh()
	{
		refreshEvent_.SetEvent();
	}

	void PLControl::RegisterCallback(const Callback &callback)
	{
		callbacks_.push_back(callback);
	}

	bool PLControl::_Init(const PLComplate &plComplate)
	{
		if( !exitEvent_.Create() )
			return false;
		if( !refreshEvent_.Create() )
			return false;

 		//HRESULT hr =CoCreateInstance(__uuidof(SAXXMLReader), NULL, CLSCTX_ALL, __uuidof(ISAXXMLReader), (void **)&rdr_);
		HRESULT hr = rdr_.CreateInstance(__uuidof(SAXXMLReader));
		if( hr != S_OK )
			return false;

		plHandler_.reset(new PLParser(pRTDataSvr_->GetGameTable(), pSysOpt_, plComplate));
		rdr_->raw_putContentHandler(plHandler_.get());


		thread_.RegisterFunc(std::tr1::bind(&PLControl::_Thread, this));
		thread_.Start();

		return true;
	}


	http::Buffer PLControl::_DownLoadCGI()
	{
		static const stdex::tString urls[] = 
		{ 
			_T("http://3up.i8desk.com:2821/getpl2011/GetPL.aspx"), 
			_T("http://118.102.24.149:2821/getpl2011/GetPL.aspx")
		};


		http::Buffer buf;
		for(size_t i = 0; i != _countof(urls); ++i)
		{
			try
			{
				const stdex::tString url = urls[i] + _GetCGIParam();

				// 从域名得到CGI串
				using namespace std::tr1::placeholders;
				buf = http::DownloadFile2(url, 
					std::tr1::bind(&PLControl::_DownloadFileCallback, this));
				
				break;
			}
			catch(std::exception &e)
			{
				if( i == _countof(urls) - 1 )
					throw e;
				else
				{
					stdex::tString error = _T("通过域名下载CGI失败: ");
					error += CA2T(e.what());
					Log(LM_DEBUG, error);
				}
			}
		}
		
		return buf;
	}

	bool PLControl::_DownloadFileCallback()
	{
		// 快速退出
		return IsStop(exitEvent_);
	}

	stdex::tString PLControl::_ParserCGI(stdex::tString &buf)
	{
		std::vector<stdex::tString> vParameters;
		std::vector<stdex::tString> vSubParameters;

		std::string error = "无效的CGI内容";

		stdex::Split(vParameters, buf, _T('&'));
		if( vParameters.size() < 3 ) 
			throw std::logic_error(error);
			

		stdex::Split(vSubParameters, vParameters[0], _T('='));
		if( vSubParameters.size() != 2 ) 
			throw std::logic_error(error);

		if( vSubParameters[1] != _T("1") ) 
		{
			vSubParameters.clear();
			stdex::Split(vSubParameters, vParameters[1], _T('='));

			if( vSubParameters.size() != 2 ) 
				throw std::logic_error(error);

			std::string cgiError = "请求PL CGI失败:";
			cgiError += CT2A(vSubParameters[1].c_str());
				throw std::logic_error(cgiError);
		}

		vSubParameters.clear();
		stdex::Split(vSubParameters, vParameters[1], _T('='));

		if( vSubParameters.size() != 2 )
			throw std::logic_error(error);

		return vSubParameters[1];	
	}

	http::Buffer PLControl::_DownLoadPl(const stdex::tString &plUrl)
	{
		using namespace std::tr1::placeholders;

		http::Buffer buf;
		
		try
		{
			if( plUrl.empty() )
				throw std::runtime_error("PL url 为空");

			Log(LM_DEBUG, plUrl);
			buf = http::DownloadFile2(plUrl,
				std::tr1::bind(&PLControl::_DownloadFileCallback, this));
		}
		catch(std::exception &e)
		{
			std::string error = "下载PL文件失败: ";
			error += e.what();

			throw std::runtime_error(error);
		}

		return buf;
	}

	http::Buffer PLControl::_UnZipPL(const http::Buffer &inBuf)
	{
		http::Buffer outBuf;
		try
		{
			outBuf = http::UnZip(inBuf); 
		}
		catch(std::exception &e)
		{
			std::string error = "解压PL失败: ";
			error += e.what();

			throw std::logic_error(error);
		}

#ifdef _DEBUG
		// 测试使用
		stdex::tString path = utility::GetAppPath() + _T("tmp/textPl.xml");
		std::ofstream os(CT2A(path.c_str()), std::ios::binary | std::ios::out);
		os.write(outBuf.first.get(), outBuf.second);
		os.close();
#endif

		return outBuf;
	}
		

	stdex::tString PLControl::_GetCGIParam() const
	{
		stdex::tChar nid[128] = {0};
		stdex::tChar bindoemid[128] = {0};
		stdex::tChar rid[128] = {0};
		stdex::tChar MD5[40] = {0};

		stdex::tString strParam,strsver;
		
		stdex::tString key = _T("Woijfw935@!40-s");
		strParam += _T("?");

		pSysOpt_->GetOpt(OPT_U_NID,nid);
		strParam += _T("nid=");
		strParam += nid;

		strParam += _T("&");

		strsver = utility::GetAppPath()+_T("I8DeskSvr.exe");
		strsver = utility::GetFileVersion(strsver);
		strParam +=_T("sver=");
		strParam += strsver;

		strParam += _T("&");

		pSysOpt_->GetOpt(OPT_U_OEMID, bindoemid);
		strParam += _T("bindoemid=");
		strParam += bindoemid;

		strParam += _T("&");

		pSysOpt_->GetOpt(OPT_U_RID, rid);
		strParam += _T("rid=");
		strParam += rid;

		strParam += _T("&");
		std::ostringstream os;
		os << CT2A(nid) << CT2A(bindoemid) << CT2A(rid) << CT2A(strsver.c_str()) << CT2A(key.c_str());
		
		strParam += _T("safecode=");
		const std::string buf = os.str();
		MD5Encode(buf.c_str(), buf.length(), MD5);
		strParam += MD5;	

		stdex::tString retParam;
		for(size_t i = 0; i != strParam.size(); ++i)
		{
			if( strParam[i] != 32 )
			{
				retParam += strParam[i];
			}
		}

		return retParam;
	}


	namespace detail
	{
		template < typename T >
		bool IsExit(T &exit)
		{
			if( ::WaitForSingleObject(exit, 0) == WAIT_OBJECT_0 )
			{
				return true;
			}
			else
				return false;
		}
	}

	DWORD PLControl::_Thread()
	{
		HANDLE handle[] = { refreshEvent_, exitEvent_ };

		while( !thread_.IsAborted() )
		{
			DWORD ret = ::WaitForMultipleObjects(_countof(handle), handle, FALSE, REFRESH_PL_INTERVAL);
			if( ret == WAIT_OBJECT_0 || ret == WAIT_TIMEOUT )
			{
				try
				{
					Log(LM_DEBUG, _T("开始下载CGI文件..."));

					stdex::tString plUrl;
					
					size_t cnt = 0;		// 失败循环次数
					while(1)
					{
						try
						{
							++cnt;
							if( exitEvent_.WaitForEvent(cnt * 1000) )
								break;

							if( cnt > 10 )
								break;

							http::Buffer cgiBufTmp = _DownLoadCGI();
							Log(LM_DEBUG, _T("下载CGI成功 "));

							
							stdex::tString cgiBuf = CA2T(cgiBufTmp.first.get());
							if( cgiBuf.empty() )
								continue;

							Log(LM_DEBUG, _T("开始解析CGI文件..."));
							
							plUrl = _ParserCGI(cgiBuf);
							break;
						}
						catch(std::exception &e)
						{
							stdex::tString error = _T("下载CGI失败: ");
							error += CA2T(e.what());
							Log(LM_ERROR, error);
						}
						catch(_com_error &e)
						{
							stdex::tString error = _T("下载CGI失败: ");
							error += e.ErrorMessage();
							Log(LM_ERROR, error);
						}
					}

					if( detail::IsExit(exitEvent_) )
						break;

					if( cnt > 10 )
						continue;

					Log(LM_DEBUG, _T("下载PL文件..."));

					if( detail::IsExit(exitEvent_) )
						break;

					http::Buffer plContent = _DownLoadPl(plUrl);
		
					if( detail::IsExit(exitEvent_) )
						break;

					Log(LM_DEBUG, _T("解压PL文件..."));
					http::Buffer plBuf = _UnZipPL(plContent);

					if( detail::IsExit(exitEvent_) )
						break;

					
					std::wstring msg(CA2W(plBuf.first.get() + 3, CP_UTF8));
					assert(rdr_);
					HRESULT res = S_OK;

					if( detail::IsExit(exitEvent_) )
						break;
					Log(LM_DEBUG, _T("开始解析PL..."));
					if( SUCCEEDED(res = rdr_->raw_parse(_variant_t(msg.c_str()))) )
					{
						if( plHandler_->IsNewVersion() )
						{
							Log(LM_INFO, _T("PL有更新并更新数据库..."));
							if( detail::IsExit(exitEvent_) )
								break;

							// 通知所有注册回调
							CallbackAll(callbacks_);
						}
					}
					else
					{
						Log(LM_ERROR, _T("解析PL产生错误!"));
					}
				}
				catch(std::exception &e)
				{
					Log(LM_ERROR, CA2T(e.what()));
				}
				catch(_com_error &e)
				{
					Log(LM_ERROR, e.ErrorMessage());
				}
				catch(...)
				{
					Log(LM_ERROR, _T("刷新PL发生未知错误!"));
				}
			}
			else if( ret == WAIT_OBJECT_0 + 1 )
			{
				break;
			}
			else
			{
				assert(0);
			}
		}

		return 0;
	}

	

	
}
#pragma once
#import <msxml3.dll>
#include <string>
#include <tchar.h>
#ifndef _AFXDLL
#include <assert.h>
#endif _AFXDLL
#include "../../../include/tablestruct.h"

#pragma warning(disable:4100)	// 消除未引用形参警告
#pragma warning(disable:4786)	// 消除模板函数长度警告
#pragma warning(disable:4996)	// 消除VC7/8中运行时函数过期警告

// 我们只需要小数点后 6 位有效数字
// 这样可以防止将 "123.456" _tstof 为 123.45599999999
namespace I8XML
{
	static const double Const_XML_Precision = 0.0000001;
#define INITXML() { bool bRet2= InitXMLObject(); if( !bRet2 ) return bRet2; }
	class ZXml
	{
	public:
		ZXml(void)
		{
			m_pnl=NULL;
			m_pXMLDoc = NULL;
			m_pCurNode = NULL;
			m_strError = _T("操作成功完成");
			m_bOriginal = false;
		}
		// 拷贝构造
		ZXml( const ZXml& rhs )
		{
			m_pnl=NULL;
			m_pXMLDoc = rhs.m_pXMLDoc;
			m_pCurNode = rhs.m_pCurNode;
			m_strError = rhs.m_strError;
			m_bOriginal = false;
		}
		~ZXml(void)
		{
			//if (m_bOriginal)
			//{
			//	if (m_pXMLDoc!=NULL)
			//	{
			//		m_pXMLDoc.Release();
			//	    m_pXMLDoc=NULL;
			//	}
			//}
		}
	protected:
		MSXML2::IXMLDOMDocumentPtr m_pXMLDoc;	// 文档对象
		MSXML2::IXMLDOMNodePtr m_pCurNode;		// 当前节点
		bool m_bOriginal;						// 是否原生对象？只有原生对象才在析构时 Release XML COM对象
		stdex::tString m_strError;
		MSXML2::IXMLDOMNodeListPtr m_pnl;
	public:
		stdex::tString m_strExpression;             //得到子节点时的表达式
	protected:
		// 创建 MSXML 组件对象
		bool InitXMLObject()
		{
			try
			{
				if( m_pXMLDoc != NULL )
				{
					return true;
				}
				HRESULT hr = m_pXMLDoc.CreateInstance( __uuidof(MSXML2::FreeThreadedDOMDocument) );
				if( FAILED(hr) )
				{
					if (hr==REGDB_E_CLASSNOTREG)
						m_strError = _T("创建 XML 组件对象失败！请检查 MSXML 是否正确安装。");
					else 
						m_strError = _T("创建 XML 组件对象失败！");

					return false;
				}
				m_pXMLDoc->PutpreserveWhiteSpace(VARIANT_TRUE);
				m_bOriginal=true;

				// 添加 XML 描述头（带有 encoding='GB2312' 指令）
				//
				m_pXMLDoc->appendChild( m_pXMLDoc->createProcessingInstruction( "xml", " version='1.0' encoding='GB2312'" ) );
			}
			catch(std::exception &e)
			{
				m_strError = _T("InitXMLObject: ");
				m_strError += CA2T(e.what());
				return false;
			}
			catch(...)
			{
				if( m_pXMLDoc->parseError->errorCode != 0 )
				{
					m_strError = stdex::tString(_T("InitXMLObject: ")) + (LPCTSTR)m_pXMLDoc->parseError->Getreason();
				}
				else
				{
					m_strError = _T("InitXMLObject Error!");
				}
				return false;
			}


			return true;
		}

		//-------------------
		MSXML2::IXMLDOMNodePtr _AddNode( LPCTSTR lpTagName, MSXML2::IXMLDOMNodePtr pnCurrent = NULL)
		{
			INITXML();
			try
			{
				// 抽取第一个标签名称，即 "Node.child.subtext" 中的 "Node"
				stdex::tString strTagName( lpTagName ), strFirst, strRemain;
				stdex::tString::size_type pos = strTagName.find( '.' );
				if( pos == stdex::tString::npos )
				{
					// 没有找到节点分隔符 '.',表明只有一级
					strFirst = strTagName;
					strRemain = _T("");
				}
				else
				{
					strFirst = strTagName.substr( 0, pos );				// 第一级别的标签名
					strRemain = strTagName.substr( pos + 1 );			// 剩余的标签名
				}



				//if (pnCurrent==NULL)//从顶层开始查找节点

				MSXML2::IXMLDOMNodeListPtr pRootNodes;
				bool bRoot=pnCurrent==NULL;
				if (bRoot)
				{
					pRootNodes=m_pXMLDoc->GetchildNodes();
				}
				else
				{
					pRootNodes=pnCurrent->GetchildNodes();
				}
				_bstr_t bstrTagName( strFirst.c_str() );


				MSXML2::IXMLDOMElementPtr pNewNode=m_pXMLDoc->createElement(bstrTagName);
				if (pnCurrent==NULL)
					m_pXMLDoc->appendChild(pNewNode);
				else
					pnCurrent->appendChild(pNewNode);
				if (strRemain.empty())
				{
					//最后从这里返回
					return pNewNode;
				}
				else
				{
					return _AddNode( strRemain.c_str(), pNewNode);
				}
			}
			catch(std::exception &e)
			{
				m_strError = _T("FindNodeEx: ");
				m_strError += CA2T(e.what());
				return NULL;
			}
			catch(...)
			{
				m_strError = stdex::tString(_T("FindNodeEx: ")) + (LPCTSTR)m_pXMLDoc->parseError->Getreason();
				return NULL;
			}

			return NULL;
		}




		//-------------------
	public:

		bool SaveToFile(LPCTSTR lpFileName)
		{
			INITXML();
			try
			{
				HRESULT  hr=m_pXMLDoc->save(lpFileName);
				if (FAILED(hr))
				{
					m_strError=_T("保存文件失败");
					return false;
				}
			}
			catch (std::exception &e)
			{
				m_strError = _T("保存文件失败: ");
				m_strError += CA2T(e.what());
				return false;
			}


			return true;
		}

		bool LoadFromFile(LPCTSTR lpFileName)
		{
			INITXML();
			try
			{
				m_pXMLDoc->async = VARIANT_FALSE;
				if (VARIANT_TRUE!=m_pXMLDoc->load(lpFileName))
				{
					m_strError = _T("读取文件失败");
					return false;
				}
				m_pnl=NULL;
			}
			catch (std::exception &e)
			{
				m_strError = _T("读取文件失败: ");
				m_strError += CA2T(e.what());
				return false;
			}


			return true;
		}

		bool LoadFromMemory(LPCTSTR lpContent)
		{
			INITXML();
			try
			{
				if (VARIANT_TRUE!=m_pXMLDoc->loadXML(lpContent))
				{
					m_strError=_T("保存文件失败");
					return false;
				}
				m_pnl=NULL;
			}
			catch (std::exception &e)
			{
				m_strError = _T("读取文件失败: ");
				m_strError += CA2T(e.what());
				return false;
			}


			return true;
		}
		bool AddNode(LPCTSTR lpTagName,ZXml &xml,bool bFromCurrent=false)
		{
			bool bRet=true;

			try
			{
				INITXML();
				assert( lpTagName!= NULL && lpTagName[0]!= NULL );

				// 查找标签，不存在则创建它
				MSXML2::IXMLDOMNodePtr pn = _AddNode( lpTagName, bFromCurrent?m_pCurNode:NULL);
				if( pn == NULL )
				{
					TCHAR szTemp[MAX_PATH];
					_stprintf( szTemp, _T("创建XML节点[%s]失败：%s"), lpTagName, (LPCTSTR)m_pXMLDoc->parseError->Getreason() );
					m_strError = szTemp;
					return false;
				}

				xml.m_pXMLDoc=m_pXMLDoc;
				xml.m_pCurNode=pn;
				m_pnl=NULL;

			}
			catch(std::exception &e)
			{
				m_strError = _T("SetValueEx: ");
				m_strError += CA2T(e.what());
				bRet = false;
			}
			catch(...)
			{
				if( m_pXMLDoc->parseError->errorCode != 0 )
				{
					m_strError = stdex::tString(_T("SetValueEx: ")) + (LPCTSTR)m_pXMLDoc->parseError->Getreason();
				}
				else
				{
					m_strError = _T("SetValueEx Error!");
				}
				bRet = false;
			}


			return bRet;
		}

		template <class T>
		bool GetValue( LPCTSTR lpTagName, T& value,bool bFromCurrent=false)
		{
			bool bRet;

			try
			{
				INITXML();
				assert( lpTagName != NULL && lpTagName[0] != NULL );

				MSXML2::IXMLDOMNodePtr pn = FindNodeEx(lpTagName,bFromCurrent?m_pCurNode:NULL);//m_pXMLDoc->selectSingleNode(lpTagName);
				if( pn == NULL )
				{
					m_strError = stdex::tString( _T("没有找到指定的标签名称：") ) + lpTagName;
					return false;
				}

				bRet =XML2T( value, pn);
			}
			catch(std::exception &e)
			{
				m_strError = stdex::tString(_T("GetValue: "));
				m_strError += CA2T(e.what());
				return false;
			}
			catch(...)
			{
				if( m_pXMLDoc->parseError->errorCode != 0 )
				{
					m_strError = stdex::tString(_T("GetValue: ")) + (LPCTSTR)m_pXMLDoc->parseError->Getreason();
				}
				else
				{
					m_strError = _T("得到值时发生错误!");
				}
				bRet =false;
			}


			return bRet;
		}

		template <class T>
		bool SetValueEx( LPCTSTR lpTagName/*为空NULL时设置当前节点的值*/, const T& value,bool bFromCurrent=false)
		{
			bool bRet;

			try
			{
				INITXML();
				//assert( lpTagName!= NULL && lpTagName[0]!= NULL );

				// 查找标签，不存在则创建它
				//MSXML2::IXMLDOMNodePtr pn = FindNodeEx( lpTagName, bFromCurrent?m_pCurNode:NULL, true );
				MSXML2::IXMLDOMNodePtr pn = lpTagName!=NULL?FindNodeEx( lpTagName, bFromCurrent?m_pCurNode:NULL, true ):m_pCurNode;
				if( pn == NULL )
				{
					TCHAR szTemp[MAX_PATH];
					_stprintf( szTemp, _T("创建XML节点[%s]失败：%s"), lpTagName, (LPCTSTR)m_pXMLDoc->parseError->Getreason() );
					m_strError = szTemp;
					return false;
				}

				bRet = T2XML( value, pn);
				m_pnl=NULL;
			}
			catch(std::exception &e)
			{
				m_strError = stdex::tString(_T("SetValueEx: "));
				m_strError += CA2T(e.what());
				bRet = false;
			}
			catch(...)
			{
				if( m_pXMLDoc->parseError->errorCode != 0 )
				{
					m_strError = stdex::tString(_T("SetValueEx: ")) + (LPCTSTR)m_pXMLDoc->parseError->Getreason();
				}
				else
				{
					m_strError = _T("SetValueEx Error!");
				}
				bRet = false;
			}


			return bRet;
		}


		template <class T>
		bool SetAttribute( LPCTSTR lpTagName/*如果lpTagName==NULL则设置当前结点属性*/, LPCTSTR lpAttributeName,const T& value)
		{
			bool bRet;

			try
			{
				INITXML();
				assert( /*lpTagName!= NULL && lpTagName[0]!= NULL && */lpAttributeName!=NULL && lpAttributeName[0]!=NULL);

				// 查找标签，不存在则创建它
				MSXML2::IXMLDOMNodePtr pn =lpTagName!=NULL? FindNodeEx( lpTagName, NULL, true ):m_pCurNode;
				if( pn == NULL )
				{
					TCHAR szTemp[MAX_PATH];
					_stprintf( szTemp, _T("创建XML节点[%s]失败：%s"), lpTagName, (LPCTSTR)m_pXMLDoc->parseError->Getreason() );
					m_strError = szTemp;
					return false;
				}

				MSXML2::IXMLDOMNamedNodeMapPtr pAttrMap = pn->Getattributes();
				_bstr_t bstrAttributeName(lpAttributeName);
				bool bFind=false;
				MSXML2::IXMLDOMAttributePtr Attributer=NULL;
				for (long i=0;i<pAttrMap->length;i++)
				{
					if (pAttrMap->item[i]->nodeName==bstrAttributeName)
					{
						bFind=true;
						Attributer=pAttrMap->item[i];
						break;
					}
				}
				if (!bFind)
				{
					Attributer=m_pXMLDoc->createAttribute(lpAttributeName);
					pAttrMap->setNamedItem(Attributer);
				}

				bRet = T2XML( value, (MSXML2::IXMLDOMNodePtr&)Attributer);
				m_pnl=NULL;
			}
			catch(std::exception &e)
			{
				m_strError = stdex::tString(_T("SetAttribute: "));
				m_strError += CA2T(e.what());
				bRet = false;
			}
			catch(...)
			{
				if( m_pXMLDoc->parseError->errorCode != 0 )
				{
					m_strError = stdex::tString(_T("SetAttribute: ")) + (LPCTSTR)m_pXMLDoc->parseError->Getreason();
				}
				else
				{
					m_strError = _T("SetAttribute Error!");
				}
				bRet = false;
			}


			return bRet;
		}

		template <class T>
		bool GetAttribute( LPCTSTR lpTagName, LPCTSTR lpAttributeName, T& value)
		{
			bool bRet;

			try
			{
				INITXML();
				assert(lpAttributeName && lpAttributeName[0]);

				// 查找标签，不存在则创建它
				MSXML2::IXMLDOMNodePtr pn = lpTagName 
					? FindNodeEx(lpTagName, NULL, false ) : m_pCurNode;
				if( pn == NULL )
				{
					TCHAR szTemp[MAX_PATH];
					_stprintf( szTemp, _T("创建XML节点[%s]失败：%s"), 
						lpTagName, (LPCTSTR)m_pXMLDoc->parseError->Getreason() );
					m_strError = szTemp;
					return false;
				}

				bool bFind = false;
				_bstr_t bstrAttributeName(lpAttributeName);
				MSXML2::IXMLDOMAttributePtr Attributer = NULL;

				MSXML2::IXMLDOMNamedNodeMapPtr pAttrMap = pn->Getattributes();
				for (long i = 0; i < pAttrMap->length; i++)
				{
					if (pAttrMap->item[i]->nodeName == bstrAttributeName)
					{
						bFind = true;
						Attributer = pAttrMap->item[i];
						break;
					}
				}

				if (!bFind)
				{
					m_strError = _T("没找到指定的属性");
					return false;
				}

				bRet = XML2T( value, (MSXML2::IXMLDOMNodePtr&)Attributer);
			}
			catch(std::exception &e)
			{
				m_strError = stdex::tString(_T("SetAttribute: "));
				m_strError += CA2T(e.what());
				bRet = false;
			}
			catch(...)
			{
				if( m_pXMLDoc->parseError->errorCode != 0 )
				{
					m_strError = stdex::tString(_T("SetAttribute: ")) 
						+ (LPCTSTR)m_pXMLDoc->parseError->Getreason();
				}
				else
				{
					m_strError = _T("SetAttribute Error!");
				}
				bRet = false;
			}


			return bRet;
		}
		long GetItemCount() const
		{
			if( m_pCurNode == NULL )
			{
				return 0;
			}

			MSXML2::IXMLDOMNodeListPtr items=NULL;
			if (m_strExpression.empty())
				items= m_pCurNode->childNodes;
			else
				items=m_pCurNode->selectNodes(m_strExpression.c_str());

			if( items == NULL )
			{
				return 0;
			}

			long lRet=items->length;
			return lRet;
		}
	protected:
		// 在 pnCurrent 节点级别下递归查找标签 szTagName，szTagName 具有 "A.B.C" 类似的形式，每一个 '.' 表示要一个层次级别
		// 如果 pnCurrent == NULL 则查找整个文档，bCreate 表示是否创建不存在的节点
		// 查找失败则返回 NULL
		MSXML2::IXMLDOMNodePtr FindNodeEx( LPCTSTR lpTagName, MSXML2::IXMLDOMNodePtr pnCurrent = NULL, bool bCreate = false)
		{
			INITXML();
			try
			{
				// 抽取第一个标签名称，即 "Node.child.subtext" 中的 "Node"
				stdex::tString strTagName( lpTagName ), strFirst, strRemain;
				stdex::tString::size_type pos = strTagName.find( '.' );
				if( pos == stdex::tString::npos )
				{
					// 没有找到节点分隔符 '.',表明只有一级
					strFirst = strTagName;
					strRemain = _T("");
				}
				else
				{
					strFirst = strTagName.substr( 0, pos );				// 第一级别的标签名
					strRemain = strTagName.substr( pos + 1 );			// 剩余的标签名
				}


				bool bFind=false;//是否找到了顶层标签
				//if (pnCurrent==NULL)//从顶层开始查找节点

				MSXML2::IXMLDOMNodeListPtr pRootNodes;
				bool bRoot=pnCurrent==NULL;
				if (bRoot)
				{
					pRootNodes=m_pXMLDoc->GetchildNodes();
				}
				else
				{
					pRootNodes=pnCurrent->GetchildNodes();
				}
				_bstr_t bstrTagName( strFirst.c_str() );


				MSXML2::IXMLDOMNodeListPtr pNodes=pRootNodes;
				if (pNodes!=NULL)
					for (long i=0;i<pNodes->length;i++)
					{
						//if (bRoot && i==0)
						//	continue;
						if(pNodes->item[i]->nodeType==MSXML2::NODE_PROCESSING_INSTRUCTION)
							continue;

						if (pNodes->item[i]->nodeName==bstrTagName)
						{
							bFind=true;
							if (strRemain.empty())
							{
								return pNodes->item[i];
							}
							else
							{
								return FindNodeEx( strRemain.c_str(),  pNodes->item[i], bCreate);
							}
						}
					}

					if (!bFind)
					{
						if (bCreate)
						{
							MSXML2::IXMLDOMElementPtr pNewNode=m_pXMLDoc->createElement(bstrTagName);
							if (pnCurrent==NULL)
								m_pXMLDoc->appendChild(pNewNode);
							else
								pnCurrent->appendChild(pNewNode);
							if (strRemain.empty())
							{
								return pNewNode;
							}
							else
							{
								return FindNodeEx( strRemain.c_str(), pNewNode, bCreate);
							}
						}
					}
			}
			catch(std::exception &e)
			{
				m_strError = _T("FindNodeEx: ");
				m_strError += CA2T(e.what());
				return NULL;
			}
			catch(...)
			{
				m_strError = stdex::tString(_T("FindNodeEx: ")) + (LPCTSTR)m_pXMLDoc->parseError->Getreason();
				return NULL;
			}


			return NULL;
		}

		template<typename CharT>
		bool XML2T( std::basic_string<CharT> & value, const MSXML2::IXMLDOMNodePtr& pn)
		{
			typedef std::tr1::add_pointer<CharT>::type PType;
			typedef std::tr1::add_const<PType>::type CPType;
			value = (CPType)pn->text;

			return true;
		}	

		template<size_t _Size>
		bool XML2T(char (&_Desc)[_Size], const MSXML2::IXMLDOMNodePtr& pn)
		{
			strncpy(_Desc, (LPCTSTR)pn->text, _Size - 1);
			return true;
		}

		bool XML2T( bool& value, const MSXML2::IXMLDOMNodePtr& pn)
		{
			stdex::tString str = (LPCTSTR)pn->text;
			value = ( str.compare( _T("true" )) == 0) ? true : false;
			return true;
		}

		//bool XML2T( TCHAR& value, const MSXML2::IXMLDOMNodePtr& pn)
		//{
		//	value = (TCHAR)_ttoi( (LPCTSTR)pn->text );
		//	return true;
		//}

		bool XML2T( int& value, const MSXML2::IXMLDOMNodePtr& pn)
		{
			value = _ttoi( (LPCTSTR)pn->text );
			return true;
		}

		// 长整形值 long
		bool XML2T( long& value, const MSXML2::IXMLDOMNodePtr& pn)
		{
			value = _ttol( (LPCTSTR)pn->text );
			return true;
		}

		// 短整形值 short
		bool XML2T( short& value, const MSXML2::IXMLDOMNodePtr& pn)
		{
			value = (short)_ttoi( (LPCTSTR)pn->text );
			return true;
		}

		// 无符号字符值 unsigned TCHAR
		bool XML2T( unsigned char& value, const MSXML2::IXMLDOMNodePtr& pn)
		{
			value = (unsigned char)_ttoi( (LPCTSTR)pn->text );
			return true;
		}

		bool XML2T(char& value, const MSXML2::IXMLDOMNodePtr& pn)
		{
			value = (char)_ttoi( (LPCTSTR)pn->text );
			return true;
		}

		// 无符号整形值 unsigned int
		bool XML2T( unsigned int& value, const MSXML2::IXMLDOMNodePtr& pn)
		{
			value = (unsigned int)_ttoi64( (LPCTSTR)pn->text );
			return true;
		}

		// 无符号长整形值 unsigned long
		bool XML2T( unsigned long& value, const MSXML2::IXMLDOMNodePtr& pn)
		{
			value = (unsigned long)_ttoi64( (LPCTSTR)pn->text );
			return true;
		}

		// 无符号短整形值 unsigned short
		bool XML2T( unsigned short& value, const MSXML2::IXMLDOMNodePtr& pn)
		{
			value = (unsigned short)_ttoi( (LPCTSTR)pn->text );
			return true;
		}

		bool XML2T( double& value, const MSXML2::IXMLDOMNodePtr& pn)
		{
			value = _tstof( (LPCTSTR)pn->text );
			if( value != 0 )
			{
				value = value + ( value > 0  ?	Const_XML_Precision : -Const_XML_Precision );
			}
			return true;
		}

		bool XML2T( __int64& value, const MSXML2::IXMLDOMNodePtr& pn)
		{
			value = _ttoi64( (LPCTSTR)pn->text );
			return true;
		}

		bool XML2T( unsigned __int64& value, const MSXML2::IXMLDOMNodePtr& pn)
		{
			value = (unsigned __int64)_ttoi64( (LPCTSTR)pn->text );
			return true;
		}

		bool XML2T( float& value, const MSXML2::IXMLDOMNodePtr& pn)
		{
			value = (float)_tstof( (LPCTSTR)pn->text );
			if( value != 0 )
			{
				value = value + ( value > 0  ?	(float)Const_XML_Precision : (float)-Const_XML_Precision );
			}
			return true;
		}

		bool XML2T( long double& value, const MSXML2::IXMLDOMNodePtr& pn)
		{
			value = (long double)_tstof( (LPCTSTR)pn->text );
			if( value != 0 )
			{
				value = value + ( value > 0  ?	Const_XML_Precision : -Const_XML_Precision );
			}
			return true;
		}

		bool XML2T( ZXml& value, const MSXML2::IXMLDOMNodePtr& pn)
		{
			if( &value != this )
			{
				value.m_pXMLDoc = m_pXMLDoc;
				value.m_pCurNode = pn;
				value.m_strError = m_strError;
				value.m_bOriginal = false;
				value.m_strExpression=m_strExpression;
			}
			return true;
		}


		// 去掉字符串表示的浮点数值尾部的0
		void TrimBackZero( TCHAR* szNumber )
		{
			int len = (int)lstrlen( szNumber );
			for( int i = len - 1; i > 0; i-- )
			{
				if( szNumber[i] == '0' )
				{
					szNumber[i] = 0;
				}
				else if( szNumber[i] == '.' )
				{
					szNumber[i] = 0;
					break;
				}
				else
				{
					break;
				}
			}
		}

		bool T2XML( const char* value, MSXML2::IXMLDOMNodePtr& pn)
		{
			pn->text = value;
			return true;
		}

		bool T2XML( const wchar_t* value, MSXML2::IXMLDOMNodePtr& pn)
		{
			pn->text = value;
			return true;
		}


		bool T2XML( const bool value, MSXML2::IXMLDOMNodePtr& pn)
		{
			pn->text = ( value ? _T("true") : _T("false") );
			return true;
		}

		bool T2XML( const char value, MSXML2::IXMLDOMNodePtr& pn)
		{
			return T2XML( (int)value, pn);
		}

		bool T2XML( const unsigned char value, MSXML2::IXMLDOMNodePtr& pn)
		{
			return T2XML( (int)value, pn);
		}

		bool T2XML( const int value, MSXML2::IXMLDOMNodePtr& pn)
		{
			TCHAR buffer[256];
			pn->text = _itot( value, buffer, 10 );
			return true;
		}	

		bool T2XML( const unsigned int value, MSXML2::IXMLDOMNodePtr& pn)
		{
			TCHAR buffer[256];
			pn->text = ::_ultot( value, buffer, 10 );
			return true;
		}

		bool T2XML( const long value, MSXML2::IXMLDOMNodePtr& pn)
		{
			TCHAR buffer[256];
			pn->text = _ltot( value, buffer, 10 );
			return true;
		}

		bool T2XML(const unsigned long value,MSXML2::IXMLDOMNodePtr& pn)
		{
			TCHAR buffer[256];
			pn->text =_ultot(value,buffer,10);
			return true;
		}

		bool T2XML( const short value, MSXML2::IXMLDOMNodePtr& pn)
		{
			return T2XML( (int)value, pn );
		}

		// 双精度浮点值 double
		bool T2XML( const double value, MSXML2::IXMLDOMNodePtr& pn)
		{
			TCHAR sz[MAX_PATH];
			_stprintf( sz, _T("%.6f"), value );
			TrimBackZero( sz );
			pn->text = (LPCTSTR)sz;
			return true;
		}

		bool T2XML( const stdex::tString& value, MSXML2::IXMLDOMNodePtr& pn)
		{
			pn->text = value.c_str();
			return true;
		}

		bool T2XML( const unsigned __int64 value, MSXML2::IXMLDOMNodePtr& pn)
		{
			TCHAR buffer[256];
			pn->text = _ui64tot( value, buffer, 10 );
			return true;
		}

		bool T2XML( const __int64 value, MSXML2::IXMLDOMNodePtr& pn)
		{
			TCHAR buffer[256];
			pn->text = _i64tot( value, buffer, 10 );
			return true;
		}

	public:
		ZXml CreateArray( LPCTSTR lpTagName, size_t sLen)
		{
			InitXMLObject();

			// 创建接口
			ZXml _xml( *this );

			try
			{
				// 创建节点
				MSXML2::IXMLDOMNodePtr pnAdd = FindNodeEx( lpTagName, NULL, true);
				_xml.m_pCurNode = pnAdd;

				TCHAR temp[20];
				for( size_t i = 0; i < sLen; i++ )
				{
					// 创建节点
					_stprintf( temp, _T("_%d"), i );
					MSXML2::IXMLDOMElementPtr peChild1 = m_pXMLDoc->createElement( temp );
					MSXML2::IXMLDOMNodePtr pnChild1 = pnAdd->appendChild( peChild1 );
				}
				m_pnl=NULL;
			}
			catch(std::exception &e)
			{
				m_strError = _T("CreateArray: ");
				m_strError += CA2T(e.what());
				_xml.m_pCurNode = NULL;
			}
			catch(...)
			{
				m_strError = stdex::tString(_T("CreateArray: ")) + (LPCTSTR)m_pXMLDoc->parseError->Getreason();
				_xml.m_pCurNode = NULL;
			}


			return _xml;
		}

		//ZXml operator [] ( long index )
		//{
		//	InitXMLObject();
		//	assert( m_pXMLDoc != NULL && m_pCurNode != NULL );

		//	ZXml _xml;
		//	_xml.m_pXMLDoc = m_pXMLDoc;

		//	MSXML2::IXMLDOMNodeListPtr pnl =NULL;
		//	if (m_strExpression.empty())
		//		pnl=m_pCurNode->childNodes;
		//	else
		//		pnl=m_pCurNode->selectNodes(m_strExpression.c_str());
		//
		//	assert( pnl->length >= index );
		//	if( pnl->length < index )
		//	{
		//		_xml.m_pCurNode = m_pCurNode;
		//	}
		//	else
		//	{
		//		_xml.m_pCurNode = pnl->item[index];
		//	}

		//	return _xml;
		//}
		ZXml operator [] ( long index )
		{
			InitXMLObject();
			assert( m_pXMLDoc != NULL && m_pCurNode != NULL );

			ZXml _xml;
			_xml.m_pXMLDoc = m_pXMLDoc;

			if (m_pnl==NULL)
			{
				if (m_strExpression.empty())
					m_pnl=m_pCurNode->childNodes;
				else
					m_pnl=m_pCurNode->selectNodes(m_strExpression.c_str());
			}

			assert( m_pnl->length >= index );
			if( m_pnl->length < index )
			{
				_xml.m_pCurNode = m_pCurNode;
			}
			else
			{
				_xml.m_pCurNode = m_pnl->item[index];
			}

			return _xml;
		}

		ZXml operator [] ( unsigned long index )
		{
			return this->operator []( (long)index );
		}
		ZXml operator [] ( int index )
		{
			return this->operator []( (long)index );
		}
		ZXml operator [] ( unsigned int index )
		{
			return this->operator []( (long)index );
		}
		ZXml operator [] ( short index )
		{
			return this->operator []( (long)index );
		}
		ZXml operator [] ( unsigned short index )
		{
			return this->operator []( (long)index );
		}
		ZXml operator [] ( char index )
		{
			return this->operator []( (long)index );
		}
		ZXml operator [] ( unsigned char index )
		{
			return this->operator []( (long)index );
		}

		template <class T>
		ZXml& operator = ( const T& rhs )
		{
			// 原生对象不允许直接进行赋值操作
			if( !m_bOriginal )
			{
				try
				{
					long lRet = T2XML( rhs, m_pCurNode );
				}
				catch(...)
				{
				}

			}

			return *this;
		}
		//------------------------
		operator bool ()			{ bool value = bool();							XML2T( value, m_pCurNode ); return value; }
		operator char ()			{ char value = char();							XML2T( value, m_pCurNode ); return value; }
		operator unsigned char ()	{ unsigned char value = unsigned char();		XML2T( value, m_pCurNode ); return value; }
		operator short ()			{ short value = short();						XML2T( value, m_pCurNode ); return value; }
		operator unsigned short ()	{ unsigned short value = unsigned short();		XML2T( value, m_pCurNode ); return value; }
		operator int ()				{ int value = int();							XML2T( value, m_pCurNode ); return value; }
		operator unsigned int ()	{ unsigned int value = unsigned int();			XML2T( value, m_pCurNode ); return value; }
		operator long ()			{ long value = long();							XML2T( value, m_pCurNode ); return value; }
		operator unsigned long ()	{ unsigned long value = unsigned long();		XML2T( value, m_pCurNode ); return value; }
		operator __int64 ()			{ __int64 value = __int64();					XML2T( value, m_pCurNode ); return value; }
		operator unsigned __int64 (){ unsigned __int64 value = unsigned __int64();	XML2T( value, m_pCurNode ); return value; }
		operator float ()			{ float value = float();						XML2T( value, m_pCurNode ); return value; }
		operator double ()			{ double value = double();						XML2T( value, m_pCurNode ); return value; }
		operator long double ()		{ long double value = long double();			XML2T( value, m_pCurNode ); return value; }
		operator const stdex::tString(){ stdex::tString value;							XML2T( value, m_pCurNode ); return value; }

		void SaveToMemory(stdex::tString& strXml)
		{
			InitXMLObject();
			try
			{
				strXml=m_pXMLDoc->xml;
				stdex::tString::size_type pos;
				pos=strXml.find_first_of('>');
				if (pos!=stdex::tString::npos)
				{
					strXml.replace(0,++pos,_T("<?xml version=\"1.0\" encoding=\"GB2312\"?>"));
				}
			}
			catch(...)
			{

			}

		}

		bool RemoveNode(LPCTSTR lpTagName, bool bFromCurrent=false)
		{
			bool bRet=true;

			try
			{
				INITXML();
				MSXML2::IXMLDOMNodePtr pn = bFromCurrent?m_pCurNode:m_pXMLDoc;
				MSXML2::IXMLDOMNodeListPtr items=pn->selectNodes(lpTagName);
				if (items==NULL)
					return true;

				long lCount=items->length;
				while (lCount--)
					pn->removeChild(items->item[0]);
			}
			catch(std::exception &e)
			{
				m_strError = _T("RemoveNode: ");
				m_strError += CA2T(e.what());
				bRet = false;
			}
			catch(...)
			{
				if( m_pXMLDoc->parseError->errorCode != 0 )
				{
					m_strError = stdex::tString(_T("RemoveNode: ")) + (LPCTSTR)m_pXMLDoc->parseError->Getreason();
				}
				else
				{
					m_strError = _T("RemoveNode Error!");
				}
				bRet = false;
			}


			return bRet;
		}

		void SetExpression(const stdex::tString& strExpression)
		{
			m_pnl=NULL;
			m_strExpression=strExpression;
		}

		bool createProcessingInstruction(LPCTSTR target=_T("xml"), LPCTSTR data=_T(" version='1.0' encoding='GB2312'"))
		{
			try
			{
				//MSXML2::IXMLDOMNodePtr  pn=;
				m_pXMLDoc->insertBefore(
					m_pXMLDoc->createProcessingInstruction( target, data ) ,
					_variant_t(m_pXMLDoc->GetchildNodes()->Getitem(0).GetInterfacePtr())
					);
			}
			catch(std::exception &e)
			{
				m_strError = _T("createProcessingInstruction: ");
				m_strError += CA2T(e.what());
				return false;
			}
			catch(...)
			{
				if( m_pXMLDoc->parseError->errorCode != 0 )
				{
					m_strError = stdex::tString(_T("InitXMLObject: ")) + (LPCTSTR)m_pXMLDoc->parseError->Getreason();
				}
				else
				{
					m_strError = _T("InitXMLObject Error!");
				}
				return false;
			}


			return true;
		}
	};

}
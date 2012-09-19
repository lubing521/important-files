/*
 * Copyright (c) 2003, 新浪网研发中心
 * All rights reserved.
 * 
 * 文件名称：LibCode.h
 * 文件标识：WEB开发库编码,加解密函数头文件
 * 摘    要：BASE64编码,字符串加密解密,URI编码函数
 * 作    者：高岩
 * 版    本：030101
 */
 
/// \file LibCode.h
/// 编码,加解密函数头文件
/// BASE64编码,字符串加密解密,URI编码函数
   
#ifndef _WEBDEVLIB_CODE_H_
#define _WEBDEVLIB_CODE_H_ 

#include <wtypes.h>
#include <string>

using namespace std;

// WEB Application Develop Library namaspace
namespace webdevlib
{
	/// Base64编码
	string base64_encode( const string &source );
	
	/// Base64解码
	string base64_decode( const string &source );
	
	/// Sina字符串加密
	string sina_encrypt( const string &source );
	
	/// Sina字符串解密
	string sina_decrypt( const string &source );

	/// URI编码
	//string uri_encode( const string &source );
	string uri_encode( LPCTSTR lpszSouce);
	
	
	/// URI解码
	string uri_decode( const string &source );
}

#endif //_WEBDEVLIB_CODE_H_

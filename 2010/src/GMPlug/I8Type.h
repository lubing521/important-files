#ifndef __I8TYPE_H__
#define __I8TYPE_H__
#include <string>
#ifdef _UNICODE
typedef std::wstring std_string; 
#else
typedef std::string  std_string; 
#endif
#endif
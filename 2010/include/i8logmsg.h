/**
 * i8logmsg.h
 * 用于日志与调试信息的处理
 * 2009-07-02  v1.0
 * author:liuyongqing@i8mail.com
 * copyrihgt@sogo 
 * 消息的严重级别定义，
   /// Shutdown the logger (decimal 1).
   LM_SHUTDOWN,

   /// Messages indicating function-calling sequence (decimal 2).
   LM_TRACE,

  /// Messages that contain information normally of use only when
  /// debugging a program (decimal 4).
  LM_DEBUG,

  /// Informational messages (decimal 8).
  LM_INFO,

  /// Conditions that are not error conditions, but that may require
  /// special handling (decimal 16).
  LM_NOTICE,

  /// Warning messages (decimal 32).
  LM_WARNING,

  /// Initialize the logger (decimal 64).
  LM_STARTUP,

  /// Error messages (decimal 128).
  LM_ERROR,

  /// Critical conditions, such as hard device errors (decimal 256).
  LM_CRITICAL,

  /// A condition that should be corrected immediately, such as a
  /// corrupted system database (decimal 512).
  LM_ALERT,

  /// A panic condition.  This is normally broadcast to all users
  /// (decimal 1024).
  LM_EMERGENCY,

 * 格式串支持的形式，取自"ace/Log_Msg.h"
   * Format a message to the thread-safe ACE logging mechanism.  Valid
   * options (prefixed by '%', as in printf format strings) include:
   *  - 'A': print an ACE_timer_t value (which could be either double
   *         or ACE_UINT32.)
   *  - 'a': abort the program at this point abruptly.
   *  - 'b': print a ssize_t value
   *  - 'B': print a size_t value
   *  - 'c': print a character
   *  - 'C': print a character string
   *  - 'i', 'd': print a decimal number
   *  - 'I': indent according to nesting depth (obtained from
   *         ACE_Trace::get_nesting_indent()).
   *  - 'e', 'E', 'f', 'F', 'g', 'G': print a double
   *  - 'l': print line number where an error occurred.
   *  - 'M': print the name of the priority of the message.
   *  - 'm': return the message corresponding to errno value, e.g., as
   *         done by strerror()
   *  - 'N': print file name where the error occurred.
   *  - 'n': print the name of the program (or "<unknown>" if not set)
   *  - 'o': print as an octal number
   *  - 'P': print out the current process id
   *  - 'p': print out the appropriate errno message from sys_errlist,
   *         e.g., as done by perror()
   *  - 'Q': print out the uint64 number
   *  - 'q': print out the int64 number
   *  - '@': print a void* pointer (in hexadecimal)
   *  - 'r': call the function pointed to by the corresponding argument
   *  - 'R': print return status
   *  - 'S': print out the appropriate _sys_siglist entry corresponding
   *         to var-argument.
   *  - 's': print out a character string
   *  - 'T': print timestamp in hour:minute:sec:usec format.
   *  - 'D': print timestamp as Weekday Month day year hour:minute:sec.usec
   *  - 't': print thread id (1 if single-threaded)
   *  - 'u': print as unsigned int
   *  - 'w': prints a wide character
   *  - 'W': print a wide character string
   *  - 'x': print as a hex number
   *  - 'X': print as a hex number
   *  - 'z': print an ACE_OS::WChar character
   *  - 'Z': print an ACE_OS::WChar character string
   *  - ':': print a time_t value as an integral number
   *  - '%': print out a single percent sign, '%
   * 日志宏的调用格式(注意有两层括符)
   I8_DEBUG, I8_INFOR, I8_ERROR:
   I8_DEBUG((LM_XXXX, I8_TEXT("..."))); //无可变参数
   I8_DEBUG((LM_XXXX, I8_TEXT("..."), ...)); //有可变参数
   注：
   I8_DEBUG不保证非调试版本会起作用
   I8_INFOR和I8_ERROR会在任何启用日志功能的版本起作用
*/

#ifndef I8LOGMSG_H
#define I8LOGMSG_H

#include <windows.h>

#ifdef I8LOGMSG
#	ifdef USE_ACE_LIB
#		include "ace/Log_Msg.h"
#		include "ace/Log_Record.h"
#		include "ace/Log_Msg_Backend.h"

#		ifndef NDEBUG
#			pragma comment(lib, "ACEd.lib")
#		else
#			pragma comment(lib, "ACE.lib")
#		endif //#		ifndef NDEBUG

#		define I8_ASSERT(X) ACE_ASSERT(X)

#		define I8_TEXT					ACE_TEXT

#		define RESET_COLOR		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
#		define DEBUG_COLOR		FOREGROUND_RED | FOREGROUND_GREEN  //brown
#		define INFOR_COLOR	    FOREGROUND_GREEN | FOREGROUND_BLUE //cyan
#		define ERROR_COLOR		FOREGROUND_RED

#		define I8_HEX_DUMP(X)			ACE_HEX_DUMP(X)

#		define I8_DEBUG(X)	do { \
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DEBUG_COLOR); \
			ACE_DEBUG(X); \
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), RESET_COLOR); \
			} while (0)

#		define I8_INFOR(X)	do { \
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), INFOR_COLOR); \
			ACE_DEBUG(X); \
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), RESET_COLOR); \
			} while (0)

#		define I8_ERROR(X)	do { \
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), ERROR_COLOR); \
			ACE_ERROR(X); \
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), RESET_COLOR); \
			} while (0)

#		define I8_ERROR_RETURN(X, Y) do { \
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), ERROR_COLOR); \
			ACE_ERROR_RETURN(X, Y); \
			} while (0)

#	else
#		include <assert.h>

#		define I8_ASSERT(X) assert(X)

//i8 log msg code

#		define I8_HEX_DUMP(X)	do {} while (0)
#		define I8_DEBUG(X)		do {} while (0)
#		define I8_INFOR(X)		do {} while (0)
#		define I8_ERROR(X)		do {} while (0)
#		define I8_ERROR_RETURN(X, Y) return (Y)
#	endif

#else //#ifdef I8LOGMSG
#	include <assert.h>

#	define I8_ASSERT(X)  assert(X)

enum I8_Log_Priority
{
  // = Note, this first argument *must* start at 1!

  /// Shutdown the logger (decimal 1).
  LM_SHUTDOWN = 01,

  /// Messages indicating function-calling sequence (decimal 2).
  LM_TRACE = 02,

  /// Messages that contain information normally of use only when
  /// debugging a program (decimal 4).
  LM_DEBUG = 04,

  /// Informational messages (decimal 8).
  LM_INFO = 010,

  /// Conditions that are not error conditions, but that may require
  /// special handling (decimal 16).
  LM_NOTICE = 020,

  /// Warning messages (decimal 32).
  LM_WARNING = 040,

  /// Initialize the logger (decimal 64).
  LM_STARTUP = 0100,

  /// Error messages (decimal 128).
  LM_ERROR = 0200,

  /// Critical conditions, such as hard device errors (decimal 256).
  LM_CRITICAL = 0400,

  /// A condition that should be corrected immediately, such as a
  /// corrupted system database (decimal 512).
  LM_ALERT = 01000,

  /// A panic condition.  This is normally broadcast to all users
  /// (decimal 1024).
  LM_EMERGENCY = 02000,

  /// The maximum logging priority.
  LM_MAX = LM_EMERGENCY,

  /// Do not use!!  This enum value ensures that the underlying
  /// integral type for this enum is at least 32 bits.
  LM_ENSURE_32_BITS = 0x7FFFFFFF
};

#	define I8_TCHAR		TCHAR
#	define I8_TEXT		TEXT

namespace i8desk {
	class CI8Log 
	{
	public:
		virtual void i8log(I8_Log_Priority priority, const I8_TCHAR *format, ...) = 0;
	};
	CI8Log *GetI8Log(void);
}

#	define I8_HEX_DUMP(X)	do { \
		CI8Log *pLog = GetI8Log(); \
		pLog->i8log X; \
		} while (0)

#	define I8_DEBUG(X)		do { \
		CI8Log *pLog = GetI8Log(); \
		pLog->i8log X; \
		} while (0)

//仅在调试版才生效的日志
#	if !defined(NDEBUG)
#		define I8_DEBUG2(X)		I8_DEBUG(X) //仅在调试版才生效的日志
#	else
#		define I8_DEBUG2(X)					//仅在调试版才生效的日志
#	endif

//仅在调试版且手动打开才生效的日志
#	if 0
#		define I8_DEBUG3(X)		I8_DEBUG2(X) //仅在调试版且手动打开才生效的日志
#	else
#		define I8_DEBUG3(X)					//仅在调试版且手动打开才生效的日志
#	endif

#	define I8_INFOR(X)		do { \
		CI8Log *pLog = GetI8Log(); \
		pLog->i8log X; \
		} while (0)

#	define I8_ERROR(X)		do { \
		CI8Log *pLog = GetI8Log(); \
		pLog->i8log X; \
		} while (0)

#	define I8_ERROR_RETURN(X, Y) do { \
		CI8Log *pLog = GetI8Log(); \
		pLog->i8log X; \
		return (Y); \
		} while (0)

#include <ctime>

namespace i8desk 
{
	class CSectionMonitor 
	{
	public:
		CSectionMonitor(const I8_TCHAR *id, time_t ht = 0, bool b = true) 
			: m_id(id)
			, m_ht(ht)
			, m_st(time(0))
			, m_b(b)
		{
			if (m_b)
			{
				SYSTEMTIME t;
				::GetLocalTime(&t);
				I8_DEBUG((LM_DEBUG, I8_TEXT("%02u:%02u:%02u.%03u:ENTER:%s\n"), 
					t.wHour, t.wMinute, t.wSecond, t.wMilliseconds, m_id));
			}
		}		
		~CSectionMonitor()
		{
			SYSTEMTIME t;
			::GetLocalTime(&t);
			time_t ut = time(0) - m_st;

			if (m_b) 
			{
				I8_DEBUG(((m_ht == 0 || ut < m_ht) ? LM_DEBUG : LM_WARNING, 
					I8_TEXT("%02u:%02u:%02u.%03u:LEAVE:%s(用时:%d秒)\n"), 
					t.wHour, t.wMinute, t.wSecond, t.wMilliseconds, m_id, (int)ut));
			} 
			else if (ut >= m_ht)
			{
				I8_DEBUG((LM_WARNING, 
					I8_TEXT("%02u:%02u:%02u.%03u:LEAVE:%s(用时:%d秒)\n"), 
					t.wHour, t.wMinute, t.wSecond, t.wMilliseconds, m_id, (int)ut));
			}
		}
	private:
		const I8_TCHAR *m_id;
		time_t m_st;
		time_t m_ht;
		bool m_b;
	};
}

#	if !defined(NDEBUG) && defined(TEST_SECTION_TIMECOST)
//X:代码段的标识，Y:执行时间水位设定
//测试代码的执行时间并发送消息到日志系统，只输出消息不检查
#		define TEST_THIS_SECTION(X)			i8desk::CSectionMonitor i8sm(X)
//检查代码的执行时间，超时会有一条警告消息，此宏可避免消息过多
#		define CHECK_THIS_SECTION(X, Y)		i8desk::CSectionMonitor i8sm(X, Y, false)
//监视代码的执行时间，又检查又输出，此宏超时时有警告标志
#		define MONITOR_THIS_SECTION(X, Y)	i8desk::CSectionMonitor i8sm(X, Y)
#	else
#		define TEST_THIS_SECTION(X)			do {} while (0)
#		define CHECK_THIS_SECTION(X, Y)		do {} while (0)
#		define MONITOR_THIS_SECTION(X, Y)	do {} while (0)
#	endif

#endif //#ifdef I8LOGMSG



#endif //#ifndef I8LOGMSG_H
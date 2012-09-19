#ifndef _i8desk_dbengine_h_
#define _i8desk_dbengine_h_

#include <wctype.h>
#include <tchar.h>
#import "c:\\program Files\\Common Files\\System\\ADO\\msado15.dll" no_namespace rename("EOF","adoEOF")

namespace i8desk
{
	//统一的数据操作接口.
	struct IDatabase 
	{
		//释放接口
		virtual void Release()			= 0;

		//打开数据库
		virtual bool Open(LPCTSTR lpszConnString, LPCTSTR lpszUser = TEXT(""), LPCTSTR lpszPassword = TEXT("")) = 0;

		//关闭数据库
		virtual void Close()			= 0;

		//得到数据库操作错误的信息
		virtual LPCTSTR GetErrInfo()	= 0;

		//得到对应该的错误代码
		virtual long GetErrCode()		= 0;

		//锁住数据库组件
		virtual void Lock()				= 0;

		//解锁数据库组件
		virtual void UnLock()			= 0;

		//事务管理，开始，提交，回滚事务
		virtual void BeginTrans()		= 0;
		virtual void CommitTrans()		= 0;
		virtual void RollbackTrans()	= 0;

		//执行SQL语句.不需要得到返回结果，如果是插件，删除，修改，可以通过第二个参数得到影响的行。
		//如果不需要第二个参数的值，可以设置空，或者不填（有默认值）
		virtual bool ExecSql(LPCTSTR lpszSql, PLONG RecordsAffected = NULL) = 0;

		//执行SQL语句.需要得到返回的结果集.返回的是ＡＤＯ的Recordset接口接口.如果成功需要自己执行prcd->Close();
		//ex. 
		//	CAutoLock<IDatabase> lock(m_pDatabase);
		//  _RecordsetPrt prcd;
		//  if (m_pDatabase->ExecSql(lpszSql, prcd))
		//	{
		//		while (prcd->IsEof() == VARIANT_FALSE)
		//		{
		//			//get line value.	
		//		}
		//		prcd->Close();
		//		prcd.Release();	//可以不需要，出了作用域，智能指针会自己调用释放接口。
		//	}
		//	else
		//		OutputDebugString(m_pDatabase->GetErrInfo());
		
		virtual bool ExecSql(LPCTSTR lpszSql, _RecordsetPtr& prcd)			= 0;

		//工具函数:压缩数据库.只能要没有打开数据时才能压缩.
		virtual bool CompactDatabase(LPCTSTR lpszDbFile) = 0;
	};

	//系统统一的选项操作。（他是存诸在数据库里的一个表里）
	struct ISysOpt
	{
		//释放接口
		virtual void	Release()										= 0;

		//09是设置成数据库的组件，10是设置成数据库缓存组件返回的选项表指针。
		virtual void	SetIDatabase(void* pDatabase)					= 0;

		//得到系统选项.如果表里没有这个选项。会自动插入，并写上默认值
		virtual bool	GetOpt(LPCTSTR optName, bool defValue = true)	= 0;
		virtual long	GetOpt(LPCTSTR optName, long defValue = 0)		= 0;
		virtual __int64 GetOpt(LPCTSTR optName, __int64 defValue = 0)	= 0;
		virtual void	GetOpt(LPCTSTR optName, LPTSTR lpszValue, LPCTSTR defValue = TEXT("")) = 0;

		//设置系统选项.如果表里没有这个选项。会自动插入，并写上设置的值
		virtual void	SetOpt(LPCTSTR optName, bool optValue)			= 0;
		virtual void	SetOpt(LPCTSTR optName, long optValue)			= 0;
		virtual void	SetOpt(LPCTSTR optName, __int64 optValue)		= 0;
		virtual void	SetOpt(LPCTSTR optName, LPCTSTR optValue)		= 0;
	};

	//系统通用写日志的组件。创建组件后，一定要设置自己的日志文件名（真正的文件名是在（设置文件名）-yyyymmdd.log）
	enum I8_Log_Priority
	{
		LM_SHUTDOWN		= 01,
		LM_TRACE		= 02,
		LM_DEBUG		= 04,			//大于他的级别才写向日志文件。
		LM_INFO			= 010,
		LM_NOTICE		= 020,
		LM_WARNING		= 040,
		LM_STARTUP		= 0100,
		LM_ERROR		= 0200,
		LM_CRITICAL		= 0400,
		LM_ALERT		= 01000,
		LM_EMERGENCY	= 02000,
		LM_MAX			= LM_EMERGENCY,
		LM_ENSURE_32_BITS = 0x7FFFFFFF
	};

	struct ILogger
	{
		virtual void Release()	= 0;

		//设置日志文件名.仅需要文件名(不要带路径以及扩展名).比如log,最后生成的文件名是\log\log-yyymmdd.log.
		//内部会自动根据当天的日期生成日志。（即会自动在文件名后添加当天的日期，并加上扩展名.log,并统一放到log目录）
		//建立以插件名字为日志文件名.这样，所有的日志文件名不会重复，也方便查看
		virtual void SetLogFileName(LPCTSTR lpszLogFile)	= 0;

		//设置日志文件最大的大小。(字节为单位)
		virtual void SetMaxLogSize(DWORD dwMaxSize)			= 0;

		//在每条日志前添加日志输出的日期
		virtual void SetAddDateBefore(bool bAddDate)		= 0;

		//在每条日志后添加一个回车换行符
		virtual void SetAddCrLfAfter(bool bAddCrLf)			= 0;

		//设置多个内部属性。
		virtual void SetProptery(LPCTSTR lpszLogFile, DWORD dwMaxLogSize, bool bAddDate) = 0;

		//设置日志输出到控制台的ip,port.内部通过udp发给控制台。//网络序
		virtual void SetConsole(DWORD ip, DWORD port)		= 0;

		//得到组件的属性
		virtual LPCTSTR GetLogFileName()					= 0;
		virtual DWORD	GetMaxLogSize()						= 0;
		virtual bool	GetAddCrLfAfter()					= 0;
		virtual bool	GetAddDateBefore()					= 0;

		//写一条日志到数据
		virtual void WriteLog(DWORD level, LPCTSTR lpszFormater, ...)		= 0;
		virtual void WriteLog2(DWORD level, LPCTSTR lpszLog, int nLen = -1) = 0;

		//清空所有的日志
		virtual void CleanLog()								= 0;
	};

	BOOL WINAPI CreateDatabase(IDatabase** ppIDatabase);
	BOOL WINAPI CreateSysOpt(ISysOpt** ppISysOpt);
	BOOL WINAPI CreateLogger(ILogger** ppILogger);
}


#endif
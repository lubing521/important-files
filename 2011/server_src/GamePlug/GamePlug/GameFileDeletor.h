#ifndef __I8DESK_GAME_FILE_DELETOR_HPP
#define __I8DESK_GAME_FILE_DELETOR_HPP


#include "../../../include/MultiThread/Lock.hpp"
#include "../../../include/MultiThread/Thread.hpp"


namespace i8desk
{

	class DownloadEngine;
	class DelClientGame;

	// ----------------------------------------------
	// class GameFileDeletor

	// 中心指定删除冗余游戏
	class GameFileDeletor
	{
		enum { MAX_DELETE_GAME = 10 };											// 最大删除游戏数
		static const size_t DELETE_FILES_TIMEOUT = 1 * 10 * 1000;	


	private:
		IRTDataSvr *rtDataSvr_			;	// 数据缓存接口
		ISvrPlugMgr *plugMgr_			;	// 插件管理器
		ISysOpt *sysOpt_				;	// 选项表
		DownloadEngine *downEngine_		;	// 下载引擎
		DelClientGame *delClient_		;	// 删除客户机游戏

		async::thread::AutoEvent exit_;			// 快速退出事件
		async::thread::AutoEvent waitHandle_;	// 等待事件
		async::thread::AutoEvent delTmpFiles_;	// 删除临时目录文件事件
		async::thread::ThreadImplEx thread_;

	public:
		GameFileDeletor(IRTDataSvr *rtDataSvr,  ISvrPlugMgr *plugMgr, ISysOpt *sysOpt, DownloadEngine *downEngine, DelClientGame *delClient);
		~GameFileDeletor();

	private:
		GameFileDeletor(const GameFileDeletor &);
		GameFileDeletor &operator=(const GameFileDeletor &);

		// 对外接口
	public:
		void Start();					// 失败抛出异常
		void Stop();

		void Run();
		void DeleteTmpFiles();			// 删除临时文件

	private:
		void _DeleteGame();				// 删除操作
		void _DeleteGameImpl(const std::tr1::tuple<long, stdex::tString, DWORD> &);
		void _DeleteTmpFiles();			// 删除三层下载临时文件
		void _Init();
		DWORD _Thread();
	};
}





#endif
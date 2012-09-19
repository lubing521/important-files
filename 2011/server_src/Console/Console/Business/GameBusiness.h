#ifndef __BUSINESS_GAME_HPP
#define __BUSINESS_GAME_HPP


#include "../Data/DataHelper.h"
#include "../../../../include/TaskInfo.h"
#include <set>

namespace i8desk
{
	namespace business
	{
		// -----------------------------
		// Game

		

		namespace Game
		{
			const extern stdex::tString AutoUpdate;
			const extern stdex::tString ManualUpdate;


			// 处理服务端、客户端路径
			void HandlePath(data_helper::GameTraits::ValueType &val);

			// 处理游戏运行方式描述
			void HandleRunTypeDesc(const data_helper::RunTypeTraits::VectorType &runTpes, stdex::tString &desc);

			// 删除游戏
			void Delete(const data_helper::GameTraits::ValueType &val);

			// 设置游戏运行方式
			bool SetRunType(const data_helper::GameTraits::ValueType &game, const data_helper::RunTypeTraits::VectorType &runType);

			// 所有游戏添加指定分区
			void AddRunType(const stdex::tString &AID);

			// 删除所有游戏指定分区
			void EraseRunType(const stdex::tString &AID);
				 

			// 是否需要下载
			bool CouldDownload(const data_helper::GameTraits::ValueType &val);


			// 得到用户自定义添加游戏GID, 不可重入
			unsigned long GetCustomGID();

			// 通过Name查找游戏
			bool CheckByName(const stdex::tString &name, i8desk::data_helper::GameTraits::ValueType &findGame);

			// 得到优下载先级描述
			LPCTSTR GePriorityDesc(DWORD priority);

			// 得到状态描述
			LPCTSTR GetStatusDesc(const data_helper::GameTraits::ValueType &game);

			// 获取更新方式
			void GetUpdateDesc(const std::tr1::function<int(LPCTSTR)> &callback);

			// 设置更新方式
			void SetUpdate(const data_helper::GameTraits::ValueType &game, const stdex::tString &updateDesc);

			// 获取下载优先级描述
			void GetPriorityDesc(const std::tr1::function<int(LPCTSTR)> &callback);

			// 设置状态描述
			void SetStatusDesc(const data_helper::GameTraits::ValueType &game, const stdex::tString &desc);

			// 得到运行方式描述
			const stdex::tString &GetRunTypeDesc(const data_helper::GameTraits::ValueType &game);

			// 得到游戏图标
			const utility::ICONPtr &GetIcon(const data_helper::GameTraits::ValueType &game);

			// 设置游戏ICON
			bool SetIcon(const data_helper::GameTraits::ValueType &game, const utility::ICONPtr &bmp);

			// 类别过滤游戏
			void ClassFilter(const data_helper::GameTraits::VectorType &games, data_helper::GameTraits::VectorType &filterGames, const stdex::tString &CID);

			// 中心游戏
			void CenterFilter(const data_helper::GameTraits::MapType &games, data_helper::GameTraits::VectorType &curGames);


			// 中心增值游戏
			void CenterI8PlayFilter(const data_helper::GameTraits::MapType &games, data_helper::GameTraits::VectorType &centerGames);

			// 未下载游戏
			void NoDownloadFilter(const data_helper::GameTraits::MapType &games, data_helper::GameTraits::VectorType &noDownloadGames);

			// 近期新增游戏
			void RecentFilter(const data_helper::GameTraits::MapType &games, data_helper::GameTraits::VectorType &recentGames);
		
			// 中心删除游戏
			void DeleteFilter( data_helper::GameTraits::VectorType &recentGames);

			// 需要更新的游戏
			void UpdateFilter(const data_helper::GameTraits::MapType &games, data_helper::GameTraits::VectorType &updateGames);
		
			// 本地游戏
			void LocalAllFilter(const data_helper::GameTraits::MapType &games, data_helper::GameTraits::VectorType &curGames);
			void LocalFilter(const data_helper::GameTraits::MapType &games, data_helper::GameTraits::VectorType &localGames, const stdex::tString &CID);
		

			// 查找游戏
			void FindGame(LPCTSTR destText, const data_helper::GameTraits::VectorType &games, data_helper::GameTraits::VectorType &values);
		
			// 游戏操作
			bool TaskOperate(long gid, TaskOperation op);

			// 本地游戏操作
			bool LocalTaskOperate(const data_helper::GameTraits::ValueType &game, TaskOperation op, bool isForce);
	
			// 获取本地游戏的所有盘符
			void GetLocalDrv(std::set<TCHAR> &drvs);

			// 获取可删除游戏列表
			void GetCleanGames(data_helper::GameTraits::VectorType &games, TCHAR drv, ulonglong size, int iKeepDay, bool greateHundred);
		
			// 判断硬盘空间是否足够所选择下载游戏
			bool IsEnoughSpace(const stdex::tString &path, const ulonglong totalSize);

			// 得到所有游戏大小
			ulonglong GetSize(data_helper::GameTraits::VectorType &games);

			// 获取游戏存档路径
			stdex::tString GetSavePath(const data_helper::GameTraits::ValueType &val);
		
			// 设置存档路径
			void SetSavePath(data_helper::GameTraits::ValueType &val, const stdex::tString &path);
		
			// 统计中心资源个数
			void CummulateCenterGame(std::map<int, size_t> &centerGames);

			// 统计本地游戏个数
			void CummulateLocalGame(std::map<stdex::tString, size_t> &centerGames);

			// 保留天数描述信息回调
			typedef std::tr1::function<int (LPCTSTR)> KeepDayDescCallback;
			typedef std::tr1::function<int (int, DWORD_PTR)> KeepDayDataCallback;
			void KeepDayDesc(const KeepDayDescCallback &descCallback, const KeepDayDataCallback &dataCallback);

			// 通过GID得到游戏名称
			stdex::tString  GetGameName(const long gid);

			// 通过GID得到游戏服务端路径
			stdex::tString  GetSvrPath(const long gid);
			
			// 通过GID得到游戏类别
			stdex::tString  GetClass(const long gid);

			// 通过GID得到游戏大小
			ulonglong GetGameSize(const long gid);

            // 统计游戏大小
            ulonglong GetGameSize(const std::vector<ulong>& gid);

            // 通过GID得到游戏信息
            db::TCustomGamePtr GetGameInfo(const long gid);

		}
	}
}



#endif
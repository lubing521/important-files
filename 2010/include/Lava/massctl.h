#ifdef MASSCTL_EXPORTS
#define MASSCTL_API __declspec(dllexport)
#else
#define MASSCTL_API __declspec(dllimport)
#endif

#pragma once

#include <shlobj.h>

#ifdef  __cplusplus
extern "C" {
#endif

MASSCTL_API int __stdcall msc_init(const wchar_t *ownername,const wchar_t *password);
/*
msc_init 必须最先调用一次进行初始化；
	ownername: 调用者标识
	password: 自定的口令
	这两个值必须记住，以后每次初始化调用应保持不变；否则以前加入的任务就不再可以获取
*/

typedef int (CALLBACK *MSCPROGRESS)(UINT step,UINT code,void *presult,void *puser);
/*
回调返回：	1 正常
			0 build / rebuid 不再调用回调函数报告上层应用进度；但出错停止和完成结果仍然会通过回调报告给上层应用
			-1 build / rebuid 将停止并返回用户取消错误
*/

struct _PROGRESS_RESULT {
	unsigned long long ull0;
	unsigned long long ull1;
	const wchar_t *wmsg;
};

#define MSCPROGRESS_START		0
// 开始: MSCPROGRESS(MSCPROGRESS_START,0,0,puser)

#define MSCPROGRESS_COMPOSE		1
// 扫描目录/文件 MSCPROGRESS(MSCPROGRESS_COMPOSE,code,_PROGRESS_RESULT *pr,puser), 只在 build 时调用
// code: 分别为
	#define MSCPROGRESS_STEP_SCAN		0	// 扫描路径
	#define MSCPROGRESS_STEP_COMPARE	1	// 文件增量比较
// pr: MSCPROGRESS_STEP_SCAN 时, ull0/ull1 分别为 已扫描文件总数/已扫描总长度, wmsg 为正在扫描的路径
//     MSCPROGRESS_STEP_COMPARE 时, ull0/ull1 不用, wmsg 为正在计算的文件

#define MSCPROGRESS_HASHMAP		2
// 计算/比较 hsahmap 块: MSCPROGRESS(MSCPROGRESS_HASHMAP,code,0,puser)
// code 为进度百分比值

#define MSCPROGRESS_COPYFILE	5
// 拷贝文件: MSCPROGRESS(MSCPROGRESS_COPYFILE,0,wchar_t *filename,puser)
//

#define MSCPROGRESS_COMMIT		0xFF

#define MSCPROGRESS_FINISHED	0x100
// 成功: MSCPROGRESS(MSCPROGRESS_FINISHED,0,_PROGRESS_RESULT *pr,puser)
// pr: 当 build 调用时, ull0/ull1 分别为 文件总数/总长度
//	   当 rebuild 调用时, ull0/ull1 分别为 已下载长度/总长度

#define MSCPROGRESS_ERROR			0x80000000
// 出错停止: MSCPROGRESS(MSCPROGRESS_ERROR,errno,wchar_t *errmsg,puser)
// errno: 错误码
// errmsg: 错误信息

#define BUILD_MODE_INPLACE	0
#define BUILD_MODE_IMPORT	1
#define BUILD_MODE_PARTIAL	2
MASSCTL_API int __stdcall msc_build(const wchar_t *taskname,const wchar_t *src,UINT mode,MSCPROGRESS progressfn,void *pvoid);
/*
msc_build 发布新的或更新目录文件
	taskname: 任务(游戏)id，为了更新，建立以后不能改变，不同的任务此值一定不能相同
	src: 任务文件根(顶层)目录。当 mode 为 BUILD_MODE_INPLACE，表明发布的游戏就在此目录。当
		mode 为 BUILD_MODE_IMPORT 时，表明从此目录导入到发布目录中，并计算增量
	mode: 可以是 BUILD_MODE_INPLACE 或 BUILD_MODE_IMPORT，分别表示是原地更新还是从其他
		目录导入更新。当是导入更新时，可以位或 BUILD_MODE_PARTIAL 表明导入源是否完整游戏
*/

MASSCTL_API int __stdcall msc_getseed(const wchar_t *taskname,__out BSTR *seed);
/*
msc_getseed: 获取目前版本的种子
	taskname: 游戏id
	seed: 种子数据，调用成功后返回
*/
MASSCTL_API int __stdcall msc_getseed2(const wchar_t *seedid,__out BSTR *seed);

MASSCTL_API int __stdcall msc_getseedex(const wchar_t *taskname,__out wchar_t *version,__out BSTR *seed,int &itype);

MASSCTL_API int __stdcall msc_createseedid(const wchar_t *taskname,__out GUID &sid,__inout BSTR *ptracklist);
/*
msc_createseedid: 获取目前版本的种子id
	taskname: 游戏id
	sid: 种子id，调用成功后返回
	ptracklist: in/out. 输入 保留，务必传入NULL.
						输出 trackerlist, shall use SysFreeString to free.
*/


// 以上函数为发布端调用

#define REBUILD_NO_TOPDIR	1	// 不生成任务名称子目录
#define REBUILD_NO_QUICKCHK	2	// 强制块比较
#define REBUILD_IN_TEMPDIR	4	// 下载到临时目录，此时 des 为 multi-string，第一个为目标目录，第二个为临时目录，中间以0间隔；最后双零结尾。
MASSCTL_API int __stdcall msc_rebuild(__inout BSTR seed,const wchar_t *des,UINT mode,__out wchar_t *taskname,MSCPROGRESS progressfn,void *pvoid);
/*
msc_rebuild: 下载目录文件
	seed: 要下载的目录文件的种子
	des: 下载目标目录
	mode: REBUILD_NO_TOPDIR/REBUILD_NO_QUICKCHK/REBUILD_IN_TEMPDIR
	taskname: 
*/

MASSCTL_API int __stdcall msc_rebuild2(const wchar_t *taskname,const GUID& sid,LPCSTR ptracklist,const wchar_t *des,UINT mode,MSCPROGRESS progressfn,void *pvoid);
/*
msc_rebuild2: 下载目录文件
	taskname: 
	sid: 要下载的目录文件的种子id
	ptracklist: 
	des: 下载目标目录(same as msc_rebuild)
	mode: same as msc_rebuild
返回值: <0 出错
		>0 下载种子
		=0 下载目录文件
*/

MASSCTL_API int __stdcall msc_addfile(const wchar_t *taskname,LPCSTR ptracklist,PVOID ptorrent,const wchar_t *szdes,const wchar_t *szsrc);

MASSCTL_API int __stdcall msc_import(const wchar_t *szseedfile,const wchar_t *des,UINT mode,wchar_t *taskname,MSCPROGRESS progressfn,void *pvoid);

MASSCTL_API unsigned __int64 __stdcall msc_getownerid(void);
/*
msc_getownerid: 获取用户id；通过用户id和任务名可以调用dna服务获取任务的状况。
*/

MASSCTL_API int __stdcall msc_commit(const wchar_t *taskname);
/*
msc_commit: 下载完成后，必须调用此函数进行封口
	taskname: 游戏id
*/

MASSCTL_API int __stdcall msc_commitex(const wchar_t *taskname,const wchar_t *des,const wchar_t *src,MSCPROGRESS progressfn,void *pvoid);
/*
msc_commitex: 封口
	taskname: 游戏id
	des：游戏目录
	src：下载文件临时目录
*/

MASSCTL_API int __stdcall msc_verify(const wchar_t *taskname,MSCPROGRESS progressfn,void *pvoid);

MASSCTL_API int __stdcall msc_reset(const wchar_t *taskname,const wchar_t *des=NULL);


// 以上函数为下载端调用

#define MSC_JOB_STATE_QUEUED		0
#define MSC_JOB_STATE_DOWNLOADING	1
#define MSC_JOB_STATE_SUSPENDED		2
#define MSC_JOB_STATE_COMPLETE		3

#define MSC_JOB_STATE_ERROR			0x80

struct MSC_DOWNLOAD_PROGRESS
{
    unsigned __int64 BytesTotal;
    unsigned __int64 BytesTransferred;
    unsigned int TransferRate;
    unsigned int TimeLeft;
    unsigned int TimeElapsed;
    unsigned char Progress;
    unsigned char State;
    unsigned short Connections;
};

MASSCTL_API int __stdcall msc_delete(const wchar_t *taskname);
MASSCTL_API int __stdcall msc_deleteex(const wchar_t *taskname,int flag);
MASSCTL_API int __stdcall msc_stop(const wchar_t *taskname);
MASSCTL_API int __stdcall msc_stopex(const wchar_t *taskname,int flag);
/*
	msc_stopex
		taskname:	
		flag:	1 停止下载
				2 停止上传
				3 全部停止
*/
MASSCTL_API int __stdcall msc_restart(const wchar_t *taskname);
MASSCTL_API int __stdcall msc_gettasklist(__out BSTR *tasklist,__out UINT *cnt);
MASSCTL_API int __stdcall msc_gettaskinfo(BSTR seed,__out wchar_t *taskname,__out wchar_t *version,__out wchar_t *homedir,__out char *trackerlist);
MASSCTL_API int __stdcall msc_getprogress(const wchar_t *taskname,__out MSC_DOWNLOAD_PROGRESS *progress);
/*
返回值: <0 出错
		>0 下载种子
		=0 下载目录文件
	progress: 下载进度及状况
*/
MASSCTL_API int __stdcall msc_getmaxtransferrate(__out UINT *downdloadrate,__out UINT *uploadrate);
MASSCTL_API int __stdcall msc_setmaxtransferrate(UINT downdloadrate,UINT uploadrate);

MASSCTL_API int __stdcall msc_GetTransferRate (__out unsigned int *pRecvRate,__out unsigned int *pSendRate );
MASSCTL_API int __stdcall msc_GetBytesTransferred (__out unsigned __int64 *pBytesRecvd,__out unsigned __int64 *pBytesSent );

#ifdef  __cplusplus
}
#endif

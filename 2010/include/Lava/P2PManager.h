//
// Created by chenzhi@dianji.com 02/16/2009
//

#pragma once
#pragma pack(push, 8)

#include <comdef.h>

//
// Forward references and typedefs
//

struct __declspec(uuid("76f49648-4e44-5341-6572-76696365544c"))
/* LIBID */ __DNAServiceLib;
struct /* coclass */ P2PCopyFile;
struct __declspec(uuid("76f49648-5049-5032-436f-707946696c65"))
/* interface */ IP2PCopyFile;
struct P2P_DOWNLOAD_PROGRESS;
enum P2P_ERROR_CODE;
enum P2P_JOB_TYPE;
enum P2P_JOB_FLAG;
enum P2P_JOB_PRIORITY;
enum P2P_JOB_STATE;
struct /* coclass */ P2PCopyJob;
struct __declspec(uuid("76f49648-5049-5032-436f-70794a6f6243"))
/* interface */ IP2PCopyJob;
struct /* coclass */ P2PCopyManager;
struct __declspec(uuid("76f49648-5049-5032-436f-70794d677243"))
/* interface */ IP2PCopyManager;
struct /* coclass */ P2PAgentManager;
struct __declspec(uuid("41503250-6567-746e-494d-616e61676572"))
/* interface */ IP2PAgentManager;

//
// Smart pointer typedef declarations
//

_COM_SMARTPTR_TYPEDEF(IP2PCopyFile, __uuidof(IP2PCopyFile));
_COM_SMARTPTR_TYPEDEF(IP2PCopyJob, __uuidof(IP2PCopyJob));
_COM_SMARTPTR_TYPEDEF(IP2PCopyManager, __uuidof(IP2PCopyManager));
_COM_SMARTPTR_TYPEDEF(IP2PAgentManager, __uuidof(IP2PAgentManager));

//
// Type library items
//

struct __declspec(uuid("76f49648-3250-4350-6f70-7946696c6543"))
P2PCopyFile;
    // [ default ] interface IP2PCopyFile

#pragma pack(push, 8)

struct P2P_DOWNLOAD_PROGRESS
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

#pragma pack(pop)

enum P2P_ERROR_CODE
{
    P2P_OK = 0,
    P2P_ERROR_OWNER_EXIST = 1,
    P2P_ERROR_JOB_EXIST = 2,
    P2P_ERROR_FILE_EXIST = 3,
    P2P_ERROR_SERVER_NOTREADY = -2138570751,
    P2P_ERROR_DATABASE_OPERATION = -2138570750,
    P2P_ERROR_OWNER_NOTEXIST = -2138570749,
    P2P_ERROR_JOB_NOTEXIST = -2138570748,
    P2P_ERROR_FILE_NOTEXIST = -2138570747,
    P2P_ERROR_LOCALNAME_EXIST = -2138570746,
    P2P_ERROR_INVALID_LOCALNAME = -2138570745,
    P2P_ERROR_INVALID_FILE = -2138570744,
    P2P_ERROR_INVALID_ARG = -2138570743,
    P2P_ERROR_JOB_TYPE = -2138570742,
    P2P_ERROR_COPY_FILE = -2138570741
};

enum P2P_JOB_TYPE
{
    P2P_JOB_TYPE_SHARE = 0,
    P2P_JOB_TYPE_DOWNLOAD = 1
};

enum P2P_JOB_FLAG
{
    P2P_DOWNLOAD_FLAG_RANDOM = 1,
    P2P_DOWNLOAD_FLAG_SEQUENTIAL = 2,
    P2P_DOWNLOAD_FLAG_HTTPP2P = 4,
    P2P_DOWNLOAD_FLAG_P2PHTTP = 8,
    P2P_DOWNLOAD_FLAG_HTTPONLY = 16,
    P2P_JOB_FLAG_TEMP = 32,
    P2P_JOB_FLAG_ADDNEW = 64
};

enum P2P_JOB_PRIORITY
{
    P2P_JOB_PRIORITY_FOREGROUND = 0,
    P2P_JOB_PRIORITY_HIGH = 1,
    P2P_JOB_PRIORITY_NORMAL = 2,
    P2P_JOB_PRIORITY_LOW = 3
};

enum P2P_JOB_STATE
{
    P2P_JOB_STATE_QUEUED = 0,
    P2P_JOB_STATE_DOWNLOADING = 1,
    P2P_JOB_STATE_SUSPENDED = 2,
    P2P_JOB_STATE_COMPLETE = 3
};

struct __declspec(uuid("76f49648-5049-5032-436f-707946696c65"))
IP2PCopyFile : IUnknown
{
    //
    // Raw methods provided by interface
    //

      virtual HRESULT __stdcall GetId (
        /*[out]*/ GUID * pFileID ) = 0;
      virtual HRESULT __stdcall GetTrackerList (
        /*[out]*/ LPSTR * ppTrackerList ) = 0;
      virtual HRESULT __stdcall GetUrlList (
        /*[out]*/ LPSTR * ppUrlList ) = 0;
      virtual HRESULT __stdcall GetLocalName (
        /*[out]*/ LPWSTR * ppLocalName ) = 0;
      virtual HRESULT __stdcall GetAddTime (
        /*[out]*/ unsigned int * pAddTime ) = 0;
      virtual HRESULT __stdcall GetPeers (
        /*[out]*/ unsigned int * pCompletePeers,
        /*[out]*/ unsigned int * pIncompletePeers ) = 0;
      virtual HRESULT __stdcall GetAvailable (
        /*[out]*/ unsigned int * pAvailable ) = 0;
      virtual HRESULT __stdcall GetProgress (
        /*[out]*/ struct P2P_DOWNLOAD_PROGRESS * pProgress ) = 0;
      virtual HRESULT __stdcall GetFinishedBlockSet (
        /*[out]*/ BSTR * ppFinishedSet,
        /*[out]*/ unsigned int * pFinishedCount,
        /*[out]*/ unsigned int * pTotalCount ) = 0;
      virtual HRESULT __stdcall Suspend ( ) = 0;
      virtual HRESULT __stdcall Resume ( ) = 0;
      virtual HRESULT __stdcall GetFileMap (
        /*[out]*/ BSTR * ppFileMap ) = 0;
      virtual HRESULT __stdcall Chenzhi (
        /*[in]*/ enum P2P_ERROR_CODE _error,
        /*[in]*/ enum P2P_JOB_TYPE _type,
        /*[in]*/ enum P2P_JOB_FLAG _flag,
        /*[in]*/ enum P2P_JOB_PRIORITY _priority,
        /*[in]*/ enum P2P_JOB_STATE _state ) = 0;
};

struct __declspec(uuid("76f49648-3250-4350-6f70-794a6f62435a"))
P2PCopyJob;
    // [ default ] interface IP2PCopyJob

struct __declspec(uuid("76f49648-5049-5032-436f-70794a6f6243"))
IP2PCopyJob : IUnknown
{
    //
    // Raw methods provided by interface
    //

      virtual HRESULT __stdcall GetId (
        /*[out]*/ unsigned __int64 * ppJobID ) = 0;
      virtual HRESULT __stdcall GetOwnerName (
        /*[out]*/ BSTR * ppOwnerName ) = 0;
      virtual HRESULT __stdcall GetJobName (
        /*[out]*/ LPWSTR * ppJobName ) = 0;
      virtual HRESULT __stdcall GetType (
        /*[out]*/ unsigned int * pJobType ) = 0;
      virtual HRESULT __stdcall GetPriority (
        /*[out]*/ unsigned int * pPriority ) = 0;
      virtual HRESULT __stdcall GetPeers (
        /*[out]*/ unsigned int * pCompletePeers,
        /*[out]*/ unsigned int * pIncompletePeers ) = 0;
      virtual HRESULT __stdcall GetProgress (
        /*[out]*/ struct P2P_DOWNLOAD_PROGRESS * pProgress ) = 0;
      virtual HRESULT __stdcall GetState (
        /*[out]*/ enum P2P_JOB_STATE * pJobState ) = 0;
      virtual HRESULT __stdcall GetCreationTime (
        /*[out]*/ unsigned int * pCreationTime ) = 0;
      virtual HRESULT __stdcall GetTotalFileCount (
        /*[out]*/ unsigned int * pTotalCount ) = 0;
      virtual HRESULT __stdcall GetFileList (
        /*[in]*/ unsigned int Offset,
        /*[in]*/ unsigned int Limit,
        /*[out]*/ BSTR * ppLocalNameList,
        /*[out]*/ unsigned int * pFileCount ) = 0;
      virtual HRESULT __stdcall GetFile (
        /*[in]*/ LPWSTR pLocalName,
        /*[out]*/ struct IP2PCopyFile * * ppFile ) = 0;
      virtual HRESULT __stdcall AddDownloadFile (
        /*[in,out]*/ GUID * pFileID,
        /*[in]*/ LPSTR pTrackerList,
        /*[in]*/ LPSTR pTorrent,
        /*[in]*/ LPSTR pUrlList,
        /*[in]*/ LPWSTR pLocalName,
        /*[in]*/ LPWSTR pExistingFile,
        /*[in]*/ unsigned int ReplaceExisting ) = 0;
      virtual HRESULT __stdcall AddShareFile (
        /*[out]*/ GUID * pFileID,
        /*[out]*/ BSTR * ppTorrent,
        /*[in]*/ LPSTR pTrackerList,
        /*[in]*/ LPWSTR pLocalName,
        /*[in]*/ LPWSTR pExistingFile ) = 0;
      virtual HRESULT __stdcall RemoveFile (
        /*[in]*/ LPWSTR pLocalName,
        /*[in]*/ unsigned int DeleteLocalFile ) = 0;
      virtual HRESULT __stdcall RenameFile (
        /*[in]*/ LPWSTR pOldLocalName,
        /*[in]*/ LPWSTR pNewLocalName,
        /*[in]*/ unsigned int ReplaceExisting ) = 0;
      virtual HRESULT __stdcall SetPriority (
        /*[in]*/ unsigned int JobPriority ) = 0;
      virtual HRESULT __stdcall Suspend ( ) = 0;
      virtual HRESULT __stdcall Resume ( ) = 0;
      virtual HRESULT __stdcall AddCustomFile (
        /*[in]*/ GUID * pFileID,
        /*[in]*/ BSTR pFileMap,
        /*[in]*/ LPSTR pTrackerList,
        /*[in]*/ unsigned int IsDownload ) = 0;
      virtual HRESULT __stdcall RemoveCustomFile (
        /*[in]*/ GUID * pFileID ) = 0;
      virtual HRESULT __stdcall GetCustomFile (
        /*[in]*/ GUID * pFileID,
        /*[out]*/ struct IP2PCopyFile * * ppFile ) = 0;
};

struct __declspec(uuid("76f49648-3250-4350-6f70-794d6772435a"))
P2PCopyManager;
    // [ default ] interface IP2PCopyManager

struct __declspec(uuid("76f49648-5049-5032-436f-70794d677243"))
IP2PCopyManager : IUnknown
{
    //
    // Raw methods provided by interface
    //

      virtual HRESULT __stdcall CreateOwnerId (
        /*[in]*/ LPWSTR pOwnerName,
        /*[in]*/ LPWSTR pPassword,
        /*[out]*/ unsigned __int64 * pOwnerID ) = 0;
      virtual HRESULT __stdcall GetOwnerId (
        /*[in]*/ LPWSTR pOwnerName,
        /*[in]*/ LPWSTR pPassword,
        /*[out]*/ unsigned __int64 * pOwnerID ) = 0;
      virtual HRESULT __stdcall CreateJob (
        /*[in]*/ LPWSTR pJobName,
        /*[in]*/ unsigned int JobType,
        /*[in]*/ enum P2P_JOB_PRIORITY Priority,
        /*[in]*/ unsigned __int64 OwnerID,
        /*[in]*/ LPSTR pReferrer,
        /*[out]*/ unsigned __int64 * pJobID,
        /*[out]*/ struct IP2PCopyJob * * ppJob ) = 0;
      virtual HRESULT __stdcall GetTotalJobCount (
        /*[in]*/ unsigned __int64 OwnerID,
        /*[out]*/ unsigned int * pTotalCount ) = 0;
      virtual HRESULT __stdcall GetJobList (
        /*[in]*/ unsigned __int64 OwnerID,
        /*[in]*/ unsigned int Offset,
        /*[in]*/ unsigned int Limit,
        /*[out]*/ BSTR * ppJobList,
        /*[out]*/ unsigned int * pJobCount ) = 0;
      virtual HRESULT __stdcall GetJob (
        /*[in]*/ unsigned __int64 JobID,
        /*[out]*/ struct IP2PCopyJob * * ppJob ) = 0;
      virtual HRESULT __stdcall DeleteJob (
        /*[in]*/ unsigned __int64 JobID,
        /*[in]*/ unsigned int DeleteLocalFile ) = 0;
      virtual HRESULT __stdcall GetMaxTransferRate (
        /*[out]*/ unsigned int * pMaxRecvRate,
        /*[out]*/ unsigned int * pMaxSendRate ) = 0;
      virtual HRESULT __stdcall SetMaxTransferRate (
        /*[in]*/ unsigned int MaxRecvRate,
        /*[in]*/ unsigned int MaxSendRate ) = 0;
      virtual HRESULT __stdcall GetTransferRate (
        /*[out]*/ unsigned int * pRecvRate,
        /*[out]*/ unsigned int * pSendRate ) = 0;
      virtual HRESULT __stdcall GetBytesTransferred (
        /*[out]*/ unsigned __int64 * pBytesRecvd,
        /*[out]*/ unsigned __int64 * pBytesSent ) = 0;
};

struct __declspec(uuid("41503250-6567-746e-434d-616e61676572"))
P2PAgentManager;
    // [ default ] interface IP2PAgentManager

struct __declspec(uuid("41503250-6567-746e-494d-616e61676572"))
IP2PAgentManager : IUnknown
{
    //
    // Raw methods provided by interface
    //

      virtual HRESULT __stdcall AddJob (
        /*[in,out]*/ GUID * pFileID,
        /*[in]*/ LPWSTR pUrlList,
        /*[in]*/ LPWSTR pTrackerList,
        /*[in]*/ LPWSTR pLocalName,
        /*[in]*/ unsigned long PriorityAndTypes ) = 0;
      virtual HRESULT __stdcall AddJob1 (
        /*[in]*/ LPSTR pszFileSeed,
        /*[in]*/ LPWSTR pUrlList,
        /*[in]*/ LPWSTR pLocalName,
        /*[in]*/ unsigned long PriorityAndTypes ) = 0;
      virtual HRESULT __stdcall DeleteJob (
        /*[in]*/ LPWSTR pLocalName,
        /*[in]*/ int DeleteFlags ) = 0;
      virtual HRESULT __stdcall DeleteAllJobs (
        /*[in]*/ int DeleteFlags ) = 0;
      virtual HRESULT __stdcall RenameJob (
        /*[in]*/ LPWSTR lpOldLocalName,
        /*[in]*/ LPWSTR lpNewLocalName ) = 0;
      virtual HRESULT __stdcall UpdateJob (
        /*[in]*/ GUID * pOldFileId,
        /*[in]*/ LPSTR lpNewFileSeed,
        /*[in]*/ LPWSTR lpNewLocalName ) = 0;
      virtual HRESULT __stdcall GetJobList (
        /*[in]*/ unsigned long JobState,
        /*[out]*/ BSTR * ppJobList,
        /*[out]*/ unsigned long * pJobCount ) = 0;
      virtual HRESULT __stdcall GetJobInfo (
        /*[in]*/ LPWSTR pLocalName,
        /*[out]*/ GUID * pFileID,
        /*[out]*/ BSTR * ppUrlList,
        /*[out]*/ BSTR * ppTrackerList,
        /*[out]*/ unsigned long * pPriority,
        /*[out]*/ unsigned long * pCreateTime,
        /*[out]*/ unsigned long * pState ) = 0;
      virtual HRESULT __stdcall GetFileSeed (
        /*[in]*/ GUID * pFileID,
        /*[out]*/ BSTR * ppszFileSeed,
        /*[out]*/ unsigned long * pLength ) = 0;
      virtual HRESULT __stdcall GetFilePeers (
        /*[in]*/ GUID * pFileID,
        /*[out]*/ unsigned long * pCompletePeers,
        /*[out]*/ unsigned long * pIncompletePeers ) = 0;
      virtual HRESULT __stdcall GetFileAvailableAmount (
        /*[in]*/ GUID * pFileID,
        /*[out]*/ unsigned long * pAvailable ) = 0;
      virtual HRESULT __stdcall GetFileProgress (
        /*[in]*/ GUID * pFileID,
        /*[out]*/ unsigned __int64 * pFileSize,
        /*[out]*/ unsigned long * pState,
        /*[out]*/ unsigned long * pProgress,
        /*[out]*/ unsigned long * pTransferRate,
        /*[out]*/ unsigned long * pTimeLeft,
        /*[out]*/ unsigned long * pConnections ) = 0;
      virtual HRESULT __stdcall SuspendFile (
        /*[in]*/ GUID * pFileID ) = 0;
      virtual HRESULT __stdcall ResumeFile (
        /*[in]*/ GUID * pFileID ) = 0;
      virtual HRESULT __stdcall SetFilePriority (
        /*[in]*/ GUID * pFileID,
        /*[in]*/ unsigned long Priority ) = 0;
      virtual HRESULT __stdcall GetMaxTransferRate (
        /*[out]*/ unsigned long * pMaxRecvRate,
        /*[out]*/ unsigned long * pMaxSendRate ) = 0;
      virtual HRESULT __stdcall SetMaxTransferRate (
        /*[in]*/ unsigned long MaxRecvRate,
        /*[in]*/ unsigned long MaxSendRate ) = 0;
      virtual HRESULT __stdcall GetTransferRate (
        /*[out]*/ unsigned long * pRecvRate,
        /*[out]*/ unsigned long * pSendRate ) = 0;
      virtual HRESULT __stdcall GetBytesTransferred (
        /*[out]*/ unsigned __int64 * pBytesRecvd,
        /*[out]*/ unsigned __int64 * pBytesSent ) = 0;
      virtual HRESULT __stdcall GetFinishedBlockIds (
        /*[in]*/ GUID * pFileID,
        /*[out]*/ BSTR * ppBlockIds,
        /*[out]*/ unsigned long * pBlockCount,
        /*[out]*/ unsigned long * pMaxBlockCount ) = 0;
};

#pragma pack(pop)


//
// 以下三个类是为了方便界面显示开发者所做的范例，如果直接使用请仔细阅读注意事项。
//
// class CP2PFile; 本文件的信息。
//
// class CP2PJob; 本任务的信息和本任务的（部分或全部）文件列表（动态创建，不建议为非当前任务创建文件列表）。
//
// class CP2PJobList; （部分或全部）任务的列表。
//
//
//
// 1、在使用CP2PJobList时，一定注意只有当前任务才创建文件列表，当前任务切换时一定要先清除旧任务的文件列表，再为新的当前任务创建文件列表。避免消耗过多的内存。
//
// 2、如果能做到每次只取部分任务到任务列表中则更好，DNA接口和CP2PJobList都支持并建议这样使用。如果过于麻烦，可以一次取所有任务，但永远不应该也没有必要同时为所有的任务创建文件列表。
//
// 3、同样道理：任务的文件列表也最好每次只取部分，如果过于麻烦可以一次全取。
//
// 4、在使用以下函数时一定要先创建列表并保证索引"i"小于列表中元素总数，函数内部为了效率不判越界。
//
// CP2PJob *pJob = JobList.GetJob(i);
//
// CP2PJob &_Job = JobList[i];
//
// CP2PFile *pFile = Job.GetFle(i);
//
// CP2PFile &_File = Job[i];
//
// chenzhi@dianji.com
//
#include <algorithm>

class CP2PFile
{
public:
	IP2PCopyFile *m_pIFile;
	LPCWSTR m_pFileName;
	LPCSTR m_pTrackerList;
	LPCSTR m_pUrlList;
	P2P_DOWNLOAD_PROGRESS m_Progress;
	UINT32 m_AddTime;
	UINT32 m_Available;
	UINT16 m_CompletePeers;
	UINT16 m_IncompletePeers;
private:
	UINT32 m_LastUpdateTime;
public:
	CP2PFile(IP2PCopyFile *pIFile, LPCWSTR lpFileName) : m_pIFile(pIFile), m_pFileName(lpFileName)
	{
		m_pTrackerList = 0;
		pIFile->GetTrackerList((LPSTR*)&m_pTrackerList);
		m_pUrlList = 0;
		pIFile->GetUrlList((LPSTR*)&m_pUrlList);
		m_pIFile->GetAddTime(&m_AddTime);
		m_LastUpdateTime = 0;
		UpdateFileInfo();
	}
	~CP2PFile(void)
	{
		if(m_pTrackerList)
			CoTaskMemFree((void*)m_pTrackerList);
		if(m_pUrlList)
			CoTaskMemFree((void*)m_pUrlList);
		m_pIFile->Release();
	}
	HRESULT UpdateFileInfo(UINT32 CurrentTime = GetTickCount()) // Milliseconds
	{
		if(CurrentTime - m_LastUpdateTime < 1000)
			return S_OK;
		m_LastUpdateTime = CurrentTime;
		HRESULT hr = m_pIFile->GetProgress(&m_Progress);
		if(S_OK != hr)
			return hr;
		if(100 == m_Progress.Progress){
			m_CompletePeers = 1;
			m_IncompletePeers = 0;
			m_Available = 100;
		}else{
			m_pIFile->GetPeers((UINT32*)&m_CompletePeers, &CurrentTime);
			m_IncompletePeers = (UINT16)CurrentTime;
			m_pIFile->GetAvailable(&m_Available);
		}
		return hr;
	}
	void *operator new(size_t _Size, void *_Where){return _Where;}
	void operator delete(void *_Memory, void *_Where){}
	void operator delete(void *_Memory){}
};

class CP2PJob
{
	struct SSITEM {char m_buf[sizeof(CP2PFile)];};
public:
	IP2PCopyJob *m_pIJob;
	LPCWSTR m_pJobName;
	LPCWSTR m_pOwnerName;
	UINT32 m_CreationTime;
	P2P_DOWNLOAD_PROGRESS m_Progress;
	UINT8 m_Type;
	UINT8 m_Priority;
	UINT16 m_Available; // 如果使用必须自己累加本任务所有文件的饱和度。
	UINT16 m_CompletePeers;
	UINT16 m_IncompletePeers;
private:
	UINT32 m_LastUpdateTime;
	void *m_pbuf;
	UINT32 m_FileCount;
	BSTR m_bstr;
public:
	CP2PJob(IP2PCopyJob *pIJob) : m_pIJob(pIJob), m_pbuf(0), m_FileCount(0), m_bstr(0)
	{
		m_pJobName = 0;
		m_pIJob->GetJobName((LPWSTR*)&m_pJobName);
		m_pOwnerName = 0;
		m_pIJob->GetOwnerName((BSTR*)&m_pOwnerName);
		m_pIJob->GetCreationTime(&m_CreationTime);
		m_pIJob->GetType(&m_LastUpdateTime);
		m_Type = (UINT8)m_LastUpdateTime;
		m_pIJob->GetPriority(&m_LastUpdateTime);
		m_Priority = (UINT8)m_LastUpdateTime;
		m_LastUpdateTime = 0;
		UpdateJobInfo();
	}
	~CP2PJob(void)
	{
		if(m_pJobName)
			CoTaskMemFree((void*)m_pJobName);
		if(m_pOwnerName)
			SysFreeString((BSTR)m_pOwnerName);
		ClearFileList();
		m_pIJob->Release();
	}
	HRESULT UpdateJobInfo(UINT32 CurrentTime = GetTickCount()) // Milliseconds
	{
		if(CurrentTime - m_LastUpdateTime < 1000)
			return S_OK;
		m_LastUpdateTime = CurrentTime;
		HRESULT hr = m_pIJob->GetProgress(&m_Progress);
		if(S_OK != hr)
			return hr;
		if(100 == m_Progress.Progress){
			m_CompletePeers = 1;
			m_IncompletePeers = 0;
			m_Available = 100;
		}else{
			m_pIJob->GetPeers((UINT32*)&m_CompletePeers, &CurrentTime);
			m_IncompletePeers = (UINT16)CurrentTime;
			m_Available = 0; // You must calculate this value from each files of this job.
		}
		return hr;
	}
	void ClearFileList(void)
	{
		if(0==m_pbuf)
			return;
		while(m_FileCount){
			CP2PFile *pFile = GetFile(--m_FileCount);
			delete pFile;
		}
		free(m_pbuf);
		m_pbuf = 0;
		SysFreeString(m_bstr);
		m_bstr = 0;
	}
	UINT32 GetFileList(UINT32 Start=0, UINT32 Count=1234)
	{
		ClearFileList();
		m_pIJob->GetFileList(Start, Count, &m_bstr, &m_FileCount);
		if(0 == m_FileCount)
			return 0;
		LPWSTR lpFileName = m_bstr;
		m_pbuf = malloc(m_FileCount * sizeof(CP2PFile));
		for(UINT32 i=0; i<m_FileCount; ++i){
			IP2PCopyFile *pIFile = 0;
			m_pIJob->GetFile(lpFileName, &pIFile);
			if(0 == pIFile){
				m_FileCount = i;
				break;
			}
			CP2PFile * pFile = new(GetFile(i)) CP2PFile(pIFile, lpFileName);
			lpFileName += wcslen(lpFileName) + 1;
		}
		return m_FileCount;
	}
	void Sort(int (__cdecl *compare)(const CP2PFile *, const CP2PFile *))
	{
		qsort(m_pbuf, m_FileCount, sizeof(CP2PFile), (int (__cdecl *)(const void *,const void *))compare);
	}
	void StableSort(bool (__cdecl *compare)(const CP2PFile &, const CP2PFile &))
	{
		SSITEM *_First = (SSITEM*)GetFile(0);
		SSITEM *_Last = (SSITEM*)GetFile(m_FileCount);
		std::stable_sort(_First, _Last, (bool (__cdecl *)(const SSITEM &, const SSITEM &))compare);
	}
	UINT32 GetFileCount(void){return m_FileCount;}
	CP2PFile *GetFile(UINT32 i){return (CP2PFile*)m_pbuf + i;}
	operator CP2PFile *(void){return (CP2PFile*)m_pbuf;}
	void *operator new(size_t _Size, void *_Where){return _Where;}
	void operator delete(void *_Memory, void *_Where){}
	void operator delete(void *_Memory){}
};

class CP2PJobList
{
	struct SSITEM {char m_buf[sizeof(CP2PJob)];};
	void *m_pbuf;
	UINT32 m_JobCount;
public:
	CP2PJobList(void) : m_pbuf(0), m_JobCount(0)
	{
	}
	~CP2PJobList(void)
	{
		Clear();
	}
	void Clear(void)
	{
		if(0==m_pbuf)
			return;
		while(m_JobCount){
			CP2PJob *pJob = GetJob(--m_JobCount);
			delete pJob;
		}
		free(m_pbuf);
		m_pbuf = 0;
	}
	UINT32 Create(IP2PCopyManager *pIManager, UINT64 OwnerID, UINT32 Start=0, UINT32 Count=123)
	{
		Clear();
		union{BSTR bstr; UINT64 *_JobID;};
		pIManager->GetJobList(OwnerID, Start, Count, &bstr, &m_JobCount);
		if(0 == m_JobCount)
			return 0;
		m_pbuf = malloc(m_JobCount * sizeof(CP2PJob));
		for(UINT32 i=0; i<m_JobCount; ++i){
			IP2PCopyJob *pIJob = 0;
			pIManager->GetJob(_JobID[i], &pIJob);
			if(0 == pIJob){
				m_JobCount = i;
				break;
			}
			CP2PJob * pJob = new(GetJob(i)) CP2PJob(pIJob);
		}
		SysFreeString(bstr);
		return m_JobCount;
	}
	void Sort(int (__cdecl *compare)(const CP2PJob *, const CP2PJob *))
	{
		qsort(m_pbuf, m_JobCount, sizeof(CP2PJob), (int (__cdecl *)(const void *,const void *))compare);
	}
	void StableSort(bool (__cdecl *compare)(const CP2PJob &, const CP2PJob &))
	{
		SSITEM *_First = (SSITEM*)GetJob(0);
		SSITEM *_Last = (SSITEM*)GetJob(m_JobCount);
		std::stable_sort(_First, _Last, (bool (__cdecl *)(const SSITEM &, const SSITEM &))compare);
	}
	UINT32 GetJobCount(void){return m_JobCount;}
	CP2PJob *GetJob(UINT32 i){return (CP2PJob*)m_pbuf + i;}
	operator CP2PJob *(void){return (CP2PJob*)m_pbuf;}
};

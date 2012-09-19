#pragma once 

namespace CommunicationData_ns
{
    struct PlugIn_st //插件信息
    {
        //Attribuild
        DWORD    gid;                           //工具ID
        TCHAR    cid[ MAX_GUID_LEN ];           //工具类别 
        DWORD    dwIdcVer;                      //中心版本号
        DWORD    dwSvrVer;                      //本地版本号
        //Base Elenment
        TCHAR    atName[ MAX_NAME_LEN ];        //工具名称
        DWORD    dwSize;                        //工具大小    
        DWORD    dwIdcClick;                    //热门度
        DWORD    dwDownloadType;                //下载类型    enum{
                                                //1：表示Server Element;
                                                //2：表示Client Element;
                                                //4：表示Config Element;}
        DWORD    dwDownloadStatus;              // 0:未下载；1：已下载
        DWORD    dwStatus;                      // 0：未启用;1：已启用；
        //Client Element
        TCHAR    atCliName[ MAX_NAME_LEN ];
        TCHAR    atCliExe[ MAX_PATH ];
        TCHAR    atCliParam[ MAX_PARAM_LEN ];
        TCHAR    atCliPath[ MAX_PATH ];
        DWORD    dwCliRunType;
    
        template<class STREAM>
        void Read(STREAM& clStream)
        {
            clStream>>gid>>cid>>dwIdcVer>>dwSvrVer>>atName>>dwSize>>dwIdcClick>>dwDownloadType>>dwDownloadStatus>>dwStatus>>atCliName>>atCliExe>>atCliParam>>atCliPath>>dwCliRunType;
        }
        template<class STREAM>
        void Write(STREAM& clStream) const
        {
            clStream<<gid<<cid<<dwIdcVer<<dwSvrVer<<atName<<dwSize<<dwIdcClick<<dwDownloadType<<dwDownloadStatus<<dwStatus<<atCliName<<atCliExe<<atCliParam<<atCliPath<<dwCliRunType;
        }
        BOOL IsEnable() const
        {
            return dwDownloadStatus && (dwDownloadType & 0x2) && dwStatus;
        }
    };
}

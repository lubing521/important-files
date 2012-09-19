#include "stdafx.h"

#include "hy.h"
#include "Extend STL/StringAlgorithm.h"
#include "Win32/FileSystem/FileHelper.hpp"


CHyOperator::CHyOperator(BOOL bDisableUnloadVDisk)
    : m_hModule(NULL)
    , pfnInstall4DosChar(NULL)
    , pfnGetDriveState(NULL)
    , pfnUnInstallDriver(NULL)
    , pfnDriverIsProtected(NULL)
    , pfnCheckPassword(NULL)
    , pfnChangePassword(NULL)
    , pfnRWAPData(NULL)
    , pfnProtArea(NULL)
    , pfnGetProtectDrivers(NULL)
    , pfnMountDosChar(NULL)
    , pfnUMountDosChar(NULL)
    , pfnGetMountInfo(NULL)
    , pfnGetProtectArea(NULL)
    , pfnGetHDInfo(NULL)
    , pfnUpdateDir(NULL)
    , m_bDisableUnloadVDisk(bDisableUnloadVDisk)
{
    TCHAR szModule[MAX_PATH] = {0};
    GetModuleFileName(NULL, szModule, MAX_PATH);
    PathRemoveFileSpec(szModule);
    PathAddBackslash(szModule);
    _tcscat_s(szModule, PROT_DLL_NAME);
    m_hModule = LoadLibrary(szModule);
    if (m_hModule == NULL)
    {
        assert(0); //加载wxProt.dll失败
        return;
    }

    pfnInstall4DosChar        = (PFNINSTALL4DOSCHAR)GetProcAddress(m_hModule, "Install4DosChar");
    pfnGetDriveState        = (PFNGETDRIVESTATE)GetProcAddress(m_hModule, "GetDriveState");
    pfnUnInstallDriver        = (PFNUNINSTALLDRIVER)GetProcAddress(m_hModule, "UnInstallDriver");
    pfnDriverIsProtected    = (PFNDRIVERISPROTECTED)GetProcAddress(m_hModule, "DriverIsProtected");       
    pfnCheckPassword        = (PFNCHECKPASSWORD)GetProcAddress(m_hModule, "CheckPassWord");
    pfnChangePassword        = (PFNCHANGEPASSWORD)GetProcAddress(m_hModule, "ChangePassWord");
    pfnRWAPData                = (PFNRWAPDATA)GetProcAddress(m_hModule, "RWAPData");
    pfnProtArea                = (PFNSETPROTECTAREAFORDOSCHAR)GetProcAddress(m_hModule, "SetProtectAreaForDosChar");
    pfnGetProtectDrivers    = (PFNGETPROTECTEDDRIVES)GetProcAddress(m_hModule, "GetProtectedDrives");
    pfnMountDosChar            = (PFNMOUNTPTDOSCHAR)GetProcAddress(m_hModule, "MountPtDosChar");
    pfnUMountDosChar        = (PFNUMOUNTPTDOSCHAR)GetProcAddress(m_hModule, "UMountPtDosChar");
    pfnGetMountInfo            = (PFNGETMOUNTINFO)GetProcAddress(m_hModule, "GetMountInfo");
    pfnGetProtectArea        = (PFNGETPROTECTAREA)GetProcAddress(m_hModule, "GetProtectArea");
    pfnGetHDInfo            = (PFNGETHDINFO)GetProcAddress(m_hModule, "GetHDInfo");
    pfnUpdateDir            = (PFNUPDATEDIR)GetProcAddress(m_hModule,"UpDateDir");
}

CHyOperator::~CHyOperator()
{
    if (m_hModule != NULL)
        FreeLibrary(m_hModule);

    pfnInstall4DosChar = NULL;
    pfnGetDriveState = NULL;
    pfnUnInstallDriver = NULL;
    pfnDriverIsProtected = NULL;       
    pfnCheckPassword = NULL;
    pfnChangePassword = NULL;
    pfnRWAPData = NULL;
    pfnProtArea = NULL;
    pfnGetProtectDrivers = NULL;
    pfnMountDosChar = NULL;                
    pfnUMountDosChar = NULL;
    pfnGetMountInfo  = NULL;
    pfnGetProtectArea = NULL;
    pfnGetHDInfo = NULL;
}

BOOL CHyOperator::IsInitialize()
{
    return m_hModule != NULL;
}

BOOL CHyOperator::IsInstallHy()
{
    return (m_hModule != NULL && pfnGetDriveState() == 0) ? TRUE : FALSE;
}

BOOL CHyOperator::ShutdownComputer(BOOL bReboot)
{
    HANDLE hToken; 
    TOKEN_PRIVILEGES tkp; 

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
        return false; 

    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid); 

    tkp.PrivilegeCount = 1; 
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;     
    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);     

    ExitWindowsEx(bReboot ? EWX_REBOOT|EWX_FORCE : EWX_SHUTDOWN|EWX_FORCE, 0);
    return true;
}
BOOL CHyOperator::InstallHy(LPCTSTR szProtectList)
{            
    if (IsInstallHy())
        return TRUE;
    if (pfnInstall4DosChar(CT2A(szProtectList), PROT_PASSWORD) == 0)
    {
        CreateProtDiskService(TEXT("ProtDisk"));
        ShutdownComputer(TRUE);
        return TRUE;
    }
    return FALSE;            
}

BOOL CHyOperator::UnInstallHy()
{
    if (IsInstallHy() && pfnUnInstallDriver() == 0)
    {
        ShutdownComputer(TRUE);
        return TRUE;
    }
    return FALSE;
}

BOOL CHyOperator::GetHDInfo(LPHD lphd )
{
    return pfnGetHDInfo(lphd);
}

BOOL CHyOperator::IsDriverProtected(LPCTSTR lpszArea)
{
    std::string sArea = CT2A(lpszArea);
    stdex::ToUpper(sArea);
    return pfnDriverIsProtected(sArea.c_str()) == 0 ? TRUE : FALSE;
}

BOOL CHyOperator::IsDriverProtected(TCHAR chDriver)
{
    char szDriver[2] = "X";
    szDriver[0] = char(towupper(chDriver));
    return pfnDriverIsProtected(szDriver) == 0 ? TRUE : FALSE;
}

//保护分区
BOOL CHyOperator::ProtectDriver(LPCTSTR lpszArea, BOOL* pbNeedRestart)
{
    assert(lpszArea != NULL && "空指针");
    assert(IsInstallHy() && "没有安装还原保护");
    if (pbNeedRestart != NULL)
    {
        *pbNeedRestart = FALSE;
    }
    if (pfnProtArea(CT2A(lpszArea), PROT_DRIVER, 0, PROT_PASSWORD, FLAG_IMME) != 0)
    {
        if (pfnProtArea(CT2A(lpszArea), PROT_DRIVER, 0, PROT_PASSWORD, FLAG_REBOOT) == 0)
        {
            if (pbNeedRestart != NULL)
            {
                *pbNeedRestart = TRUE;
            }
            else
            {
                ShutdownComputer(TRUE);
            }
            return TRUE;
        }
        return FALSE;
    }
    return TRUE;
}

//取消分区保护
BOOL CHyOperator::UnProtectDriver(LPCTSTR lpszArea, BOOL* pbNeedRestart)
{
    assert(lpszArea != NULL && "空指针");
    assert(IsInstallHy() && "没有安装还原保护");
    if (pbNeedRestart != NULL)
    {
        *pbNeedRestart = FALSE;
    }
    if (pfnProtArea(CT2A(lpszArea), UNPROT_DRIVER, 0, PROT_PASSWORD, FLAG_IMME) != 0)
    {
        if (pfnProtArea(CT2A(lpszArea), UNPROT_DRIVER, 0, PROT_PASSWORD, FLAG_REBOOT) == 0)
        {
            if (pbNeedRestart != NULL)
            {
                *pbNeedRestart = TRUE;
            }
            else
            {
                ShutdownComputer(TRUE);
            }
            return TRUE;
        }
        return FALSE;
    }
    return TRUE;
}

void CHyOperator::CreateProtDiskService(LPCTSTR lpszSvrName)
{
#define PROTDISKFILE    TEXT("ProtDisk.sys")
    SC_HANDLE   schSCManager;
    schSCManager = OpenSCManager(
        NULL,                   // machine (NULL == local)
        NULL,                   // database (NULL == default)
        SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE  // access required
        );
    if (schSCManager == NULL)
        return ;

    TCHAR szModule[MAX_PATH] = {0};
    GetSystemDirectory(szModule, MAX_PATH);
    PathAddBackslash(szModule);
    _tcscat_s(szModule, MAX_PATH, TEXT("drivers\\ProtDisk.sys"));    

    SC_HANDLE schService = CreateService(schSCManager,lpszSvrName,lpszSvrName,SERVICE_ALL_ACCESS,SERVICE_KERNEL_DRIVER,SERVICE_SYSTEM_START,SERVICE_ERROR_IGNORE,szModule,NULL,NULL,NULL,NULL,NULL);
    if (schService != NULL)
        ::CloseServiceHandle(schService);

    ::CloseServiceHandle(schSCManager); 
}

BOOL CHyOperator::IsAnyDriverProt()
{
    if (IsInstallHy())
    {
        const int HD_MAX_NUM = 10; //假设用户最多有10个硬盘
        for (int i = 0;i < HD_MAX_NUM; ++i)
        {
            tag_HDInfo hdInfo = {0};
            hdInfo.dwHdNumber = i;
            if (0 != GetHDInfo(&hdInfo))
                continue;

            for (DWORD loop = 0; loop < hdInfo.dwPtNums; loop++)
            {
                if (IsDriverProtected((TCHAR)hdInfo.pt[loop].bDosChar))
                {                    
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

stdex::tString CHyOperator::ConvertPath(LPCTSTR lpszPath)
{
    stdex::tString file(lpszPath);
    if (!(file.size() == 0 || (file.size() >= 1 && !IsDriverProtected(file[0])) || (file.size() >= 3 && file[2] != L'\\')))
    {
        stdex::tString pre = _T("\\\\.\\I8Desk");
        pre += towupper(file[0]);
        file.replace(0, 2, pre);
    }
    return file;
}

std::string CHyOperator::GetVirtualDeviceName(TCHAR drv)
{
    char device[20] = {0};
    sprintf_s(device, "I8Desk%C", towupper(drv));
    return device;
}

std::string CHyOperator::GetDriverName( TCHAR drv )
{
    char driver[20] = {0};
    sprintf_s(driver, "%C", towupper(drv));
    return driver;
}

BOOL CHyOperator::LoadVirtualDevice(TCHAR drv)
{
    if (pfnMountDosChar != NULL)
    {
        if (pfnMountDosChar(GetDriverName(drv).c_str(), GetVirtualDeviceName(drv).c_str(), PROT_PASSWORD) == 0)
        {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL CHyOperator::UnloadVirtualDevice(TCHAR drv)
{
    if (pfnUMountDosChar != NULL && !m_bDisableUnloadVDisk)
    {
        if (pfnUMountDosChar(GetVirtualDeviceName(drv).c_str(), PROT_PASSWORD) == 0)
        {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL CHyOperator::LoadVirtualDeviceEx(TCHAR drv, LPCTSTR pVirtualDeviceName)
{
    if (pfnMountDosChar != NULL)
    {
        if (pfnMountDosChar(GetDriverName(drv).c_str(), CT2A(pVirtualDeviceName), PROT_PASSWORD) == 0)
        {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL CHyOperator::UnloadVirtualDeviceEx(TCHAR drv, LPCTSTR pVirtualDeviceName)
{
    if (pfnUMountDosChar != NULL)
    {
        if (pfnUMountDosChar(CT2A(pVirtualDeviceName), PROT_PASSWORD) == 0)
        {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL CHyOperator::LoadVDevice(TCHAR tDevice)
{
    if (IsDriverProtected(tDevice))
    {
        return FALSE;
    }
    if (LoadVirtualDevice(tDevice))
    {
        return TRUE;
    }
    else
    {
        if (IsVirtualDeviceExist(tDevice))
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
}

BOOL CHyOperator::UnloadVDevice(TCHAR drv)
{
    if (IsDriverProtected(drv))
    {
        return UnloadVirtualDevice(drv);
    }
    else
    {
        return TRUE;
    }
}

BOOL CHyOperator::GetDiskRoomSize(TCHAR drv, PLARGE_INTEGER pliSize)
{
    if (pliSize == NULL)
        return false;

    char driver = (char)towupper(drv);
    pliSize->QuadPart = 0;
    if (IsDriverProtected(drv))
    {
        for (DWORD idx=0; idx<5; idx++)
        {
            tag_HDInfo hdInfo = {0};
            hdInfo.dwHdNumber = idx;
            if (0 != pfnGetHDInfo(&hdInfo))
                continue;

            for (DWORD loop=0; loop<hdInfo.dwPtNums; loop++)
            {
                if (hdInfo.pt[loop].bDosChar == driver)
                {
                    DWORD dwStartSec = hdInfo.pt[loop].dwStartSec;
                    tag_PROTECTAREA_PACK Areainfo = {0};
                    if (0 != pfnGetProtectArea(&Areainfo))
                        continue;
                    for (DWORD iArea=0; iArea<Areainfo.dwNums; iArea++)
                    {
                        if (Areainfo.Area[iArea].dwStartSec == dwStartSec)
                        {
                            pliSize->QuadPart = (__int64)Areainfo.Area[iArea].dwBitmapNotUsedBits * 512;
                            return true;
                        }
                    }
                }
            }
        }
    }

    TCHAR sdriver[] = _T("X:\\");
    sdriver[0] = driver;
    return (TRUE == GetDiskFreeSpaceExW(sdriver, (PULARGE_INTEGER)pliSize, NULL, NULL));
}
//刷新被保护的磁盘中的目录，使用该功能时必须先关闭虚拟盘
BOOL CHyOperator::RefreshDirectory(LPCTSTR dir)
{
    if (lstrlen(dir) <= 3 && lstrlen(dir) >= 1)
    {
        return RefreshDriver(dir[0]);
    }
    if (dir != NULL && lstrlen(dir) >= 3 && dir[1] == ':' && 
        IsDriverProtected(dir[0]) && pfnUpdateDir != NULL)
    {
        char path[MAX_PATH] = {0};
        WideCharToMultiByte(CP_ACP, 0, dir, -1, path, sizeof(path), NULL, NULL);

        return 0 == pfnUpdateDir(path);
    }
    return FALSE;
}

//刷新被保护的磁盘，使用该功能时必须先关闭虚拟盘
BOOL CHyOperator::RefreshDriver(TCHAR drv)
{
    if (IsDriverProtected(drv) && pfnProtArea != NULL)
    {
        if (pfnProtArea(GetDriverName(drv).c_str(), 1, 1, PROT_PASSWORD, FLAG_IMME) != 0)
        {
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

BOOL CHyOperator::IsVirtualDeviceExist(TCHAR drv)
{
    TCHAR atDriver[20] = {0};
    _stprintf_s(atDriver, _T("%C"), towupper(drv));
    HANDLE hDevHandle = ::CreateFile(ConvertPath(atDriver).c_str(), GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (INVALID_HANDLE_VALUE != hDevHandle)
    {
        ::CloseHandle( hDevHandle );
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL CHyOperator::ProtectFile(LPCTSTR lpszFile)
{
    if (lpszFile == NULL || lstrlen(lpszFile) == 0)
    {
        assert(0 && "错误的文件路径");
        return FALSE;
    }
    if (IsDriverProtected(lpszFile[0]))
    {
        if (LoadVDevice(lpszFile[0]))
        {
            stdex::tString sDesVPath = ConvertPath(lpszFile);
            if (CopyFile(lpszFile, sDesVPath.c_str(), FALSE))
            {
                if (!UnloadVirtualDevice(lpszFile[0]))
                {
                    return FALSE;
                }
            }
            else
            {
                UnloadVirtualDevice(lpszFile[0]);
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }
    }
    return TRUE;
}

BOOL CHyOperator::DeleteFile(LPCTSTR lpszFile)
{
    if (lpszFile == NULL || lstrlen(lpszFile) == 0)
    {
        assert(0 && "错误的文件路径");
        return FALSE;
    }
    if (IsDriverProtected(lpszFile[0]))
    {
        if (LoadVDevice(lpszFile[0]))
        {
            stdex::tString sDesVPath = ConvertPath(lpszFile);
            if (DeleteFile(sDesVPath.c_str()))
            {
                UnloadVirtualDevice(lpszFile[0]);
            }
            else
            {
                UnloadVirtualDevice(lpszFile[0]);
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }
    }
    DeleteFile(lpszFile);
    return TRUE;
}

BOOL CHyOperator::DeleteDirectory(LPCTSTR pPath)
{
    if (pPath == NULL || lstrlen(pPath) == 0)
    {
        assert(0 && "错误的文件路径");
        return FALSE;
    }
    if (IsDriverProtected(pPath[0]))
    {
        if (LoadVDevice(pPath[0]))
        {
            stdex::tString sVPath = ConvertPath(pPath).c_str();
            if (win32::file::DeleteDirectory(sVPath.c_str()))
            {
                UnloadVirtualDevice(pPath[0]);
            }
            else
            {
                UnloadVirtualDevice(pPath[0]);
                return FALSE;
            }
        }
    }
    win32::file::DeleteDirectory(pPath);
    return TRUE;
}

BOOL CHyOperator::MakeSureDirectoryPathExists(const std::wstring& dir)
{
    std::wstring::size_type pos = 3;	//X:\.(不能创建根目录.比如C:\)
    std::wstring::size_type len = wcslen(L"\\\\.\\I8Desk");
    if (dir.size() >= len && _wcsnicmp(dir.c_str(), L"\\\\.\\I8Desk", len) == 0)
        pos = len + 2;

    pos = dir.find(L'\\', pos);
    while (pos != dir.npos)
    {
        std::wstring sub = dir.substr(0, pos+1);
        if (!::PathFileExists(sub.c_str()))
        {
            if (!CreateDirectoryW(sub.c_str(), NULL))
            {
                if (!CreateDirectoryW(sub.c_str(), NULL))
                {
                    if (!CreateDirectoryW(sub.c_str(), NULL))
                    {
                        return FALSE;
                    }
                }
            }
        }
        pos = dir.find(L'\\', pos+1);
    }
    return TRUE;
}
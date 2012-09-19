#ifndef __i8desk_hook_inc__
#define __i8desk_hook_inc__

DWORD InstallHook(void);
DWORD StopHook(void);
void  InjectToWinlogon();
void  UnInjectToWinlogon();

BOOL  SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege);

#endif
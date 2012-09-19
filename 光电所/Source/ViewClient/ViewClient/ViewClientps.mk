
ViewClientps.dll: dlldata.obj ViewClient_p.obj ViewClient_i.obj
	link /dll /out:ViewClientps.dll /def:ViewClientps.def /entry:DllMain dlldata.obj ViewClient_p.obj ViewClient_i.obj \
		kernel32.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \
.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del ViewClientps.dll
	@del ViewClientps.lib
	@del ViewClientps.exp
	@del dlldata.obj
	@del ViewClient_p.obj
	@del ViewClient_i.obj

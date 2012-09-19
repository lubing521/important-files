// dllmain.h : 模块类的声明。

class CI8ClientCommunicationModule : public CAtlDllModuleT< CI8ClientCommunicationModule >
{
public :
	DECLARE_LIBID(LIBID_I8ClientCommunicationLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_I8CLIENTCOMMUNICATION, "{C401F946-6F53-4CA0-ABAA-5F91334A0D2B}")
};

extern class CI8ClientCommunicationModule _AtlModule;

// dllmain.h : Declaration of module class.

class CeparakstspluginieModule : public ATL::CAtlDllModuleT< CeparakstspluginieModule >
{
public :
	DECLARE_LIBID(LIBID_eparakstspluginieLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ESTEIDPLUGINIE, "{B1C669E7-2A5B-4E51-922B-DB5891485F93}")
};

extern class CeparakstspluginieModule _AtlModule;

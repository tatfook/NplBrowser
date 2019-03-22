#include "stdafx.h"

#include <string>
#include <thread>
#include <sstream>
#include <fstream>

#include "INPLRuntimeState.h"
#include "NPLInterface.hpp"
#include "json.hpp"
using namespace ParaEngine;
using namespace nlohmann;

struct BrowserParams
{
	std::string cmd;
	std::string subProcessName;
	intptr_t parentHandle;
	std::string id;
	std::string url;
	bool showTitleBar = false;
	bool withControl = false;
	int x = 0;
	int y = 0;
	int width = 800;
	int height = 600;
	bool visible = true;
	bool resize = true;
	bool enabled = true;


};
json ToJson(BrowserParams p)
{
	json out;
	out["cmd"] = p.cmd;
	out["subProcessName"] = p.subProcessName;
	out["parentHandle"] = p.parentHandle;
	out["id"] = p.id;
	out["url"] = p.url;
	out["showTitleBar"] = p.showTitleBar;
	out["withControl"] = p.withControl;
	out["x"] = p.x;
	out["y"] = p.y;
	out["width"] = p.width;
	out["height"] = p.height;
	out["visible"] = p.visible;
	out["resize"] = p.resize;
	out["enabled"] = p.enabled;
	return out;
}
json Read(NPLInterface::NPLObjectProxy tabMsg)
{
	BrowserParams params;
	params.cmd = tabMsg["cmd"];
	params.subProcessName = tabMsg["subProcessName"];
	double parentHandle = tabMsg["parentHandle"];
	params.parentHandle = (intptr_t)parentHandle;
	params.id = tabMsg["id"];
	params.url = tabMsg["url"];
	params.showTitleBar = tabMsg["showTitleBar"];
	params.withControl = tabMsg["withControl"];
	params.resize = tabMsg["resize"];
	params.visible = tabMsg["visible"];
	params.enabled = tabMsg["enabled"];
	double x = tabMsg["x"];
	double y = tabMsg["y"];
	double width = tabMsg["width"];
	double height = tabMsg["height"];
	params.x = x;
	params.y = y;
	params.width = width;
	params.height = height;

	return ToJson(params);
}
#pragma region PE_DLL 

#ifdef WIN32
#define CORE_EXPORT_DECL    __declspec(dllexport)
#else
#define CORE_EXPORT_DECL
#endif

// forware declare of exported functions. 
#ifdef __cplusplus
extern "C" {
#endif
	CORE_EXPORT_DECL const char* LibDescription();
	CORE_EXPORT_DECL int LibNumberClasses();
	CORE_EXPORT_DECL unsigned long LibVersion();
	CORE_EXPORT_DECL ParaEngine::ClassDescriptor* LibClassDesc(int i);
	CORE_EXPORT_DECL void LibInit();
	CORE_EXPORT_DECL void LibActivate(int nType, void* pVoid);
#ifdef __cplusplus
}   /* extern "C" */
#endif

HINSTANCE Instance = NULL;



ClassDescriptor* NplCefBrowser_GetClassDesc();
typedef ClassDescriptor* (*GetClassDescMethod)();

GetClassDescMethod Plugins[] =
{
	NplCefBrowser_GetClassDesc,
};

#define NplCefBrowser_CLASS_ID Class_ID(0x3b305a29, 0x47a409ce)

class NplCefBrowserDesc :public ClassDescriptor
{
public:

	void *	Create(bool loading)
	{
		return NULL;
	}
	const char* ClassName()
	{
		return "INplCefBrowser";
	}

	SClass_ID SuperClassID()
	{
		return OBJECT_MODIFIER_CLASS_ID;
	}

	Class_ID ClassID()
	{
		return NplCefBrowser_CLASS_ID;
	}

	const char* Category()
	{
		return "NplCefBrowser Category";
	}

	const char* InternalName()
	{
		return "NplCefBrowser InternalName";
	}

	HINSTANCE HInstance()
	{
		extern HINSTANCE Instance;
		return Instance;
	}
};

ClassDescriptor* NplCefBrowser_GetClassDesc()
{
	static NplCefBrowserDesc s_desc;
	return &s_desc;
}

CORE_EXPORT_DECL const char* LibDescription()
{
	return "ParaEngine NplCefBrowserImporter Ver 1.0.0";
}

CORE_EXPORT_DECL unsigned long LibVersion()
{
	return 1;
}

CORE_EXPORT_DECL int LibNumberClasses()
{
	return sizeof(Plugins) / sizeof(Plugins[0]);
}

CORE_EXPORT_DECL ClassDescriptor* LibClassDesc(int i)
{
	if (i < LibNumberClasses() && Plugins[i])
	{
		return Plugins[i]();
	}
	else
	{
		return NULL;
	}
}

CORE_EXPORT_DECL void LibInit()
{
}

#ifdef WIN32
BOOL WINAPI DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID lpvReserved)
#else
void __attribute__((constructor)) DllMain()
#endif
{
	// TODO: dll start up code here
#ifdef WIN32
	Instance = hinstDLL;				// Hang on to this DLL's instance handle.
	return (TRUE);
#endif
}
#pragma endregion PE_DLL 
CORE_EXPORT_DECL void LibActivate(int nType, void* pVoid)
{
	if (nType == ParaEngine::PluginActType_STATE)
	{
		NPL::INPLRuntimeState* pState = (NPL::INPLRuntimeState*)pVoid;
		const char* sMsg = pState->GetCurrentMsg();
		int nMsgLength = pState->GetCurrentMsgLength();

		NPLInterface::NPLObjectProxy tabMsg = NPLInterface::NPLHelper::MsgStringToNPLTable(sMsg);
		json input = Read(tabMsg);
		std::string id = input["id"];
		if (id.empty()) {
			id = "CEFCLIENT";
		}
		HWND hwnd = FindWindowA(id.c_str(), NULL);
		if (hwnd)
		{
			std::string json_str = input.dump();
			LPSTR s = const_cast<char*>(json_str.c_str());
			strcpy(s, input.dump().c_str());
			COPYDATASTRUCT MyCDS;
			MyCDS.dwData = 1; // function identifier
			MyCDS.cbData = strnlen(s, 4096) + 1; // size of data
			MyCDS.lpData = s;           // data structure

			SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)(LPVOID)&MyCDS);

		}
	}
}




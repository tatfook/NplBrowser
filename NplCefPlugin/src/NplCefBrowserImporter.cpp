#include "stdafx.h"

#include <tlhelp32.h>

#include "INPLRuntime.h"
#include "INPLRuntimeState.h"
#include "IParaEngineCore.h"
#include "IParaEngineApp.h"
#include "IAttributeFields.h"
#include "NPLInterface.hpp"

#include "json.hpp"

using namespace ParaEngine;
using namespace nlohmann;

json global_wnd_map;

struct BrowserParams
{
	std::string cmd;
    std::string callback_file;
    std::string cmdline;
	std::string client_name;
	std::string parent_handle;
    std::string cefclient_config_filename;
    std::string pid; //process id
    std::string id;
    std::string url;
	int x = 0;
	int y = 0;
	int width = 800;
	int height = 600;
	bool visible = true;
	bool resize = true;
	bool enabled = true;
	double zoom = 0.0;

    
};
json ToJson(BrowserParams p)
{
	json out;
	out["cmd"] = p.cmd;
    out["callback_file"] = p.callback_file;
    out["cmdline"] = p.cmdline;
	out["client_name"] = p.client_name;
    out["parent_handle"] = p.parent_handle;
    out["cefclient_config_filename"] = p.cefclient_config_filename;
    out["pid"] = p.pid;
	out["id"] = p.id;
	out["url"] = p.url;
	out["x"] = p.x;
	out["y"] = p.y;
	out["width"] = p.width;
	out["height"] = p.height;
	out["visible"] = p.visible;
	out["resize"] = p.resize;
	out["enabled"] = p.enabled;
	out["zoom"] = p.zoom;
	return out;
}
json Read(NPLInterface::NPLObjectProxy tabMsg)
{
	BrowserParams params;
	params.cmd = tabMsg["cmd"];
    params.callback_file = tabMsg["callback_file"];
    params.cmdline = tabMsg["cmdline"];
	params.client_name = tabMsg["client_name"];
    params.parent_handle = tabMsg["parent_handle"];
    params.cefclient_config_filename = tabMsg["cefclient_config_filename"];
    params.pid = tabMsg["pid"];
	params.id = tabMsg["id"];
	params.url = tabMsg["url"];
	params.resize = tabMsg["resize"];
	params.visible = tabMsg["visible"];
	params.enabled = tabMsg["enabled"];
	double x = tabMsg["x"];
	double y = tabMsg["y"];
	double width = tabMsg["width"];
	double height = tabMsg["height"];
	double zoom = tabMsg["zoom"];
	params.x = x;
	params.y = y;
	params.width = width;
	params.height = height;
	params.zoom = zoom;

	return ToJson(params);
}
void NPL_Activate(NPL::INPLRuntimeState* pState, std::string activateFile, NPLInterface::NPLObjectProxy& data) {

    if (!pState || activateFile.empty())
    {
        return;
    }
    std::string data_string;
    NPLInterface::NPLHelper::NPLTableToString("msg", data, data_string);
    pState->activate(activateFile.c_str(), data_string.c_str(), data_string.length());

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
    CORE_EXPORT_DECL void LibInitParaEngine(ParaEngine::IParaEngineCore* pCoreInterface);
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

ParaEngine::IParaEngineCore* g_pCoreInterface = NULL;
ParaEngine::IParaEngineCore* GetCoreInterface()
{
    return g_pCoreInterface;
}

CORE_EXPORT_DECL void LibInitParaEngine(IParaEngineCore* pCoreInterface)
{
    g_pCoreInterface = pCoreInterface;
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

DWORD FindProcessId(const char* processName)
{
    // strip path

    const char* p = strrchr(processName, '\\');
    if (p)
        processName = p + 1;

    PROCESSENTRY32 processInfo;
    processInfo.dwSize = sizeof(processInfo);

    HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (processesSnapshot == INVALID_HANDLE_VALUE)
        return 0;

    Process32First(processesSnapshot, &processInfo);
    if (!strcmp(processName, processInfo.szExeFile))
    {
        CloseHandle(processesSnapshot);
        return processInfo.th32ProcessID;
    }

    while (Process32Next(processesSnapshot, &processInfo))
    {
        if (!strcmp(processName, processInfo.szExeFile))
        {
            CloseHandle(processesSnapshot);
            return processInfo.th32ProcessID;
        }
    }

    CloseHandle(processesSnapshot);
    return 0;
}
extern "C" {
    void WriteLog(const char* sFormat, ...) {
        if (GetCoreInterface() && GetCoreInterface()->GetAppInterface()) {
            char buf_[1024 + 1];
            va_list args;
            va_start(args, sFormat);
            vsnprintf(buf_, 1024, sFormat, args);
            GetCoreInterface()->GetAppInterface()->WriteToLog(buf_);
            va_end(args);
        }
    }
}
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
		std::string cmd = input["cmd"];
		std::string callback_file = input["callback_file"];
        std::string parent_handle_s = input["parent_handle"];
        HWND parent_handle = parent_handle_s.empty() ? NULL : (HWND)std::stoull(parent_handle_s.c_str());
        WriteLog("running id:%s,cmd:%s\n", id.c_str(),cmd.c_str());
        std::string client_name = input["client_name"];
        client_name = client_name.empty() ? "cefcleint.exe" : client_name;
		id = id.empty() ? "CEFCLIENT" : id;
		if (cmd == "Start") {
			std::string cmdline = input["cmdline"];

            if (global_wnd_map.find(id) == global_wnd_map.end()) {
                global_wnd_map[id] = true;
                WriteLog("create the window:%s\ncmdline:%s\n", id.c_str(), cmdline.c_str());
                ShellExecute(NULL, "open", client_name.c_str(), cmdline.c_str(), NULL, SW_SHOWDEFAULT);
            }
            else {
                WriteLog("the window is existed:%s\n", id.c_str());
            }

		} 
		else 
		{
            if (cmd == "Quit")
            {
                if (global_wnd_map.find(id) != global_wnd_map.end()) {
                    global_wnd_map.erase(id);
                    WriteLog("erase the map of window:%s\n", id.c_str());
                }
            }
            HWND hwnd = FindWindowEx(parent_handle, NULL, id.c_str(), NULL);
            if (hwnd)
            {
                std::string json_str = input.dump();
                LPSTR s = const_cast<char*>(json_str.c_str());
                strcpy(s, input.dump().c_str());
                COPYDATASTRUCT MyCDS;
                MyCDS.dwData = 1; // function identifierz
                MyCDS.cbData = strnlen(s, 4096) + 1; // size of data
                MyCDS.lpData = s;           // data structure

                SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)(LPVOID)& MyCDS);

                NPLInterface::NPLObjectProxy data;
                data["cmd"] = cmd;
                data["id"] = id;
                data["parent_handle"] = parent_handle_s;

                NPL_Activate(pState, callback_file, data);
            }
            else {
                WriteLog("can't find the window:%s\n", id.c_str());
            }

          
			
		}

		
	}
}




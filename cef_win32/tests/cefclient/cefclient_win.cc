// Copyright (c) 2015 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <windows.h>

#include "include/base/cef_scoped_ptr.h"
#include "include/cef_command_line.h"
#include "include/cef_sandbox_win.h"
#include "tests/cefclient/browser/main_context_impl.h"
#include "tests/cefclient/browser/main_message_loop_multithreaded_win.h"
#include "tests/cefclient/browser/root_window_manager.h"
#include "tests/cefclient/browser/test_runner.h"
#include "tests/shared/browser/client_app_browser.h"
#include "tests/shared/browser/main_message_loop_external_pump.h"
#include "tests/shared/browser/main_message_loop_std.h"
#include "tests/shared/common/client_app_other.h"
#include "tests/shared/common/client_switches.h"
#include "tests/shared/renderer/client_app_renderer.h"

// When generating projects with CMake the CEF_USE_SANDBOX value will be defined
// automatically if using the required compiler version. Pass -DUSE_SANDBOX=OFF
// to the CMake command-line to disable use of the sandbox.
// Uncomment this line to manually enable sandbox support.
// #define CEF_USE_SANDBOX 1

#if defined(CEF_USE_SANDBOX)
// The cef_sandbox.lib static library may not link successfully with all VS
// versions.
#pragma comment(lib, "cef_sandbox.lib")
#endif

#include <sstream>
#include <iostream>
#include <vector>
#include "tests/cefclient/CefClientConfig.h"
#include <ShlObj.h>
#include <string.h>
using namespace std;
namespace client {
namespace {
	std::wstring CharToWchar(const std::string& orig)
	{
		size_t origsize = orig.length() + 1;
		size_t convertedChars = 0;
		int len = sizeof(wchar_t) * (orig.length() - 1);
		wchar_t* wcstring = new wchar_t[len + 1];
		mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);
		wcstring[len] = '\0';
		std::wstring wstr = wcstring;
		delete wcstring;
		return wstr;
	}

	//utf-8

	std::wstring GetLocalAppDataPath()
	{
		wchar_t m_lpszDefaultDir[MAX_PATH];
		wchar_t szDocument[MAX_PATH] = { 0 };
		memset(m_lpszDefaultDir, 0, _MAX_PATH);

		LPITEMIDLIST pidl = NULL;
		SHGetSpecialFolderLocation(NULL, CSIDL_LOCAL_APPDATA, &pidl);
		if (pidl && SHGetPathFromIDList(pidl, szDocument))
		{
			GetShortPathName(szDocument, m_lpszDefaultDir, _MAX_PATH);
		}

		std::wstring wsR = m_lpszDefaultDir;

		return wsR;
	}

	std::wstring GetLocalAppDataPath(std::wstring appName,std::wstring strBasePath)
	{
		std::wstring path = strBasePath != L"" ? strBasePath :GetLocalAppDataPath();
		path.append(L"\\");
		path.append(appName);

		if (_waccess(path.c_str(), 0) == -1)
		{
			int ret = _wmkdir(path.c_str());
			if (ret != 0)
			{
				LOG(INFO) << "create cache path err code===="<< ret;
			}
		}

		return path;
	}

	string TCHAT_to_string(TCHAR* STR)
	{
		int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, nullptr, 0, nullptr, nullptr);
		char* chRtn = new char[iLen * sizeof(char)];
		WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, nullptr, nullptr);
		string str(chRtn);

		int pos = str.find_last_of("\\", str.length());
		string exePath = str.substr(0, pos);

		return exePath;
	}

	string getExePath()
	{
		TCHAR szExePath[1024];
		GetModuleFileName(nullptr, szExePath, sizeof(szExePath));

		string exePath = TCHAT_to_string(szExePath);

		return exePath;
	}

int RunMain(HINSTANCE hInstance, int nCmdShow) {
  // Enable High-DPI support on Windows 7 or newer.
  //CefEnableHighDPISupport();

  CefMainArgs main_args(hInstance);
  void* sandbox_info = NULL;

#if defined(CEF_USE_SANDBOX)
  // Manage the life span of the sandbox information object. This is necessary
  // for sandbox support on Windows. See cef_sandbox_win.h for complete details.
  CefScopedSandboxInfo scoped_sandbox;
  sandbox_info = scoped_sandbox.sandbox_info();
#endif

  // Parse command-line arguments.
  CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
  command_line->InitFromString(::GetCommandLineW());

  // Create a ClientApp of the correct type.
  CefRefPtr<CefApp> app;
  ClientApp::ProcessType process_type = ClientApp::GetProcessType(command_line);
  if (process_type == ClientApp::BrowserProcess)
    app = new ClientAppBrowser();
  else if (process_type == ClientApp::RendererProcess)
    app = new ClientAppRenderer();
  else if (process_type == ClientApp::OtherProcess)
    app = new ClientAppOther();

  // Execute the secondary process, if any.
  int exit_code = CefExecuteProcess(main_args, app, sandbox_info);
  if (exit_code >= 0)
    return exit_code;

  // Create the main context object.
  scoped_ptr<MainContextImpl> context(new MainContextImpl(command_line, true));

  CefSettings settings;

#if !defined(CEF_USE_SANDBOX)
  settings.no_sandbox = true;
#endif
  std::string parent_handle_s = command_line->GetSwitchValue("parent_handle").ToString();
  HWND parent_handle = parent_handle_s.empty() ? NULL : (HWND)std::stoull(parent_handle_s.c_str());
  // Populate the settings based on command line arguments.
  context->PopulateSettings(&settings);
  settings.multi_threaded_message_loop = true;
  // add cache path
  //TCHAR fullPath[MAX_PATH];
  //::GetModuleHandleEx(DWORD(parent_handle), fullPath, MAX_PATH);//context->GetAppWorkingDirectory();
  //strPath += "temp/";
  //std::wstring wStrpath = CharToWchar(strPath);'
  std::wstring wStrpath = CharToWchar(getExePath());
  const std::wstring AppName = L"paracraftCef";
  CefString(&settings.cache_path).FromWString(GetLocalAppDataPath(AppName, wStrpath));
  // Create the main message loop object.
  scoped_ptr<MainMessageLoop> message_loop;
  if (settings.multi_threaded_message_loop)
    message_loop.reset(new MainMessageLoopMultithreadedWin);
  else if (settings.external_message_pump)
    message_loop = MainMessageLoopExternalPump::Create();
  else
    message_loop.reset(new MainMessageLoopStd);

  // Initialize CEF.
  context->Initialize(main_args, settings, app, sandbox_info);

  // Register scheme handlers.
  test_runner::RegisterSchemeHandlers();

  RootWindowConfig window_config;
  window_config.always_on_top = command_line->HasSwitch(switches::kAlwaysOnTop);
  window_config.with_controls =
      !command_line->HasSwitch(switches::kHideControls);
  window_config.with_osr = settings.windowless_rendering_enabled ? true : false;
  // set window bounds
  if (command_line->HasSwitch("bounds"))
  {
	  CefString bounds_s = command_line->GetSwitchValue("bounds");
	  std::vector<int> rect;
	  istringstream f(bounds_s.ToString());
	  string s;
	  while (getline(f, s, ',')) {
		  int v = std::stoul(s);
		  rect.push_back(v);
	  }
	  if (rect.size() == 4)
	  {
		  CefRect window_rect(rect[0], rect[1], rect[2], rect[3]);
		  window_config.bounds = window_rect;
	  }
  }
  window_config.url = command_line->GetSwitchValue("url").ToString();

  // Create the first window.
  context->GetRootWindowManager()->CreateRootWindow(window_config);

  std::string cefclient_config_filename = command_line->GetSwitchValue("cefclient_config_filename").ToString();
  std::string pid = command_line->GetSwitchValue("pid").ToString();
  std::string id = command_line->GetSwitchValue("window_name").ToString();

  std::string key = id + "_" + parent_handle_s;
  LOG(INFO) << "cefclient_config_filename is:" << cefclient_config_filename;
  LOG(INFO) << "pid is:" << pid;


  // Run the message loop. This will block until Quit() is called by the
  // RootWindowManager after all windows have been destroyed.
  int result = message_loop->Run();

  // Shut down CEF.
  context->Shutdown();

  // Release objects in reverse order of creation.
  message_loop.reset();
  context.reset();

  //clear config
  if (!cefclient_config_filename.empty() && !pid.empty())
  {
      LOG(INFO) << "erase config by pid:" << pid << " key:" << key;
      CefClientConfig::GetInstance(cefclient_config_filename, pid)->EraseJsonValue(key);
  }

  return result;
}

}  // namespace
}  // namespace client

// Program entry point function.
int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPTSTR lpCmdLine,
                      int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);
  return client::RunMain(hInstance, nCmdShow);
}

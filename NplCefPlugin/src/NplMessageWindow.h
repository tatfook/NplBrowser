#ifndef MESSAGE_WINDOW_WIN_H_
#define MESSAGE_WINDOW_WIN_H_
#pragma once
#include "stdafx.h"
#include "stdio.h"
#include "INPLRuntime.h"
#include "INPLRuntimeState.h"
#include "IParaEngineCore.h"
#include "IParaEngineApp.h"
#include "IAttributeFields.h"
#include "NPLInterface.hpp"
#include <tlhelp32.h>
#include <queue>
#include <windows.h>
#include "json.hpp"

//这个是
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
	std::string message_content;
	std::string message_to;
};

class NplMessageWindow
{
public:
	NplMessageWindow(std::string strHandle);
	~NplMessageWindow();
	void createWindow();
	void createWindowThread();
	static void HandleCustomMsg(HWND hWnd, WPARAM wParam, LPARAM lParam);
	std::string strWindowName;
private:
	//window相关
	HWND m_hwnd;
	std::string m_strHandle;
	void closeWindow();
};


#endif

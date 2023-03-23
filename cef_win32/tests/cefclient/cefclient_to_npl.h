#pragma once
#include "stdio.h"
#include <string>
#include <windows.h>
#include <string.h>
#include "json.hpp"

using namespace std;
class CefClientToNpl
{
public:
	CefClientToNpl();
	~CefClientToNpl();
	static CefClientToNpl* GetInstance();
	void SendMessageToWindow(std::string strMessage);
	bool ResolveMessageFromJs(std::string strMessage);
	void SetParentHandle(std::string strHandle);
	void SendMessageByWindow(std::string strMessage);
	void SendMessageByPipe(std::string strMessage);
	void CloseLink();
private:
	static CefClientToNpl* m_pInstance;
	std::string m_strHandle;
	HWND m_pHwnd;
	HANDLE m_pNamedPipe;
};


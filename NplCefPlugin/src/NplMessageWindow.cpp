#include "NplMessageWindow.h"
#include "NPLInterface.hpp"
using namespace nlohmann;
using namespace std;

static std::string m_strDefaultCallFile = "";
extern "C" {
	void WriteLog(const char* sFormat, ...);
	json Read(NPLInterface::NPLObjectProxy tabMsg);
	json ToJson(BrowserParams p);
	void SendMessageToNpl(std::string callback_file, NPLInterface::NPLObjectProxy data);


	LRESULT CALLBACK NplMessageWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_QUERYENDSESSION:
		{
			
			return TRUE; 
		}
		case WM_ENDSESSION:
		{
			
			return 0; 
		}
		case WM_COPYDATA:
		{
			NplMessageWindow::HandleCustomMsg(hwnd, wParam, lParam);
			return 0;
		}
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
}

NplMessageWindow::NplMessageWindow(std::string strHandle)
{
	strWindowName = "";
	m_hwnd = nullptr;
	m_strHandle = strHandle;
	m_strDefaultCallFile = "script/apps/Aries/Creator/Game/NplBrowser/NplBrowserPlugin.lua";
}

NplMessageWindow::~NplMessageWindow()
{
	this->closeWindow();
}

void NplMessageWindow::createWindow()
{
	WNDCLASS messageClass{ 0 };
	string str1 = "MessageWindow";
	str1 += m_strHandle;
	messageClass.lpszClassName = str1.c_str();
	messageClass.lpfnWndProc = NplMessageWindowProc;

	if (!RegisterClass(&messageClass))
	{
		return ;
	}
	m_hwnd = CreateWindowEx(
		0,
		messageClass.lpszClassName, 
		messageClass.lpszClassName, 
		0,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		nullptr,
		nullptr,
		nullptr,
		nullptr
	);
	if (!m_hwnd)
	{
		return ;
	}
	strWindowName = str1;

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void NplMessageWindow::createWindowThread()
{
	std::thread trMessage([this]() {
		this->createWindow();
	});
	//trMessage.join(); 
	trMessage.detach();
}

std::string GetMessageFile(std::string strMessage) {
	std::string strTemp = strMessage;
	std::string navigateionStr = "paracraft://sendMsg?";
	if (strTemp.find(navigateionStr) != strTemp.npos)
	{
		strTemp = strTemp.replace(strTemp.find(navigateionStr), navigateionStr.length(), "");
		if (json::accept(strTemp))
		{
			json jData = json::parse(strTemp);
			try
			{
				std::string callFile = jData["filename"];
				return callFile;
			}
			catch (const std::exception&)
			{
				WriteLog("call file isNull");
			}			
		}
	}
	return "";

}

void NplMessageWindow::HandleCustomMsg(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;
	string s = (const char*)(pcds->lpData);
	json input = nlohmann::json::parse(s);
	string message_to = input["message_to"];
	string message_content = input["message_content"];
	std::string callFile1 = GetMessageFile(message_content);
	if (message_to == "npl")
	{
		std::string defaultStr = "test message window";
		NPLInterface::NPLObjectProxy data;
		std::string navigateionStr = "paracraft://sendMsg?";
		if (message_content.find(navigateionStr) != navigateionStr.npos)
		{
			message_content = message_content.replace(message_content.find(navigateionStr), navigateionStr.length(), "");
		}
		data["msg"] = message_content != "" ? message_content : defaultStr;
		string callFile = callFile1!= "" ? callFile1:m_strDefaultCallFile;
		SendMessageToNpl(callFile, data);
	}
}

void NplMessageWindow::closeWindow()
{
	if (m_hwnd)
	{
		DestroyWindow(m_hwnd);
		delete m_hwnd;
		m_hwnd = nullptr;
	}
}


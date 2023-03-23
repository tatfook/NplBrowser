#include "NplMessageWindow.h"
#include "NPLInterface.hpp"
using namespace nlohmann;
using namespace std;

//npl相关
//static NPL::INPLRuntimeState* m_pState = nullptr;
static std::string m_strDefaultCallFile = "";
//引入c方法
extern "C" {
	void WriteLog(const char* sFormat, ...);
	json Read(NPLInterface::NPLObjectProxy tabMsg);
	json ToJson(BrowserParams p);
	void SendMessageToNpl(std::string callback_file, NPLInterface::NPLObjectProxy data);

	//MsgWaitForMultipleObjects

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
	string str1 = "NplMessageWindow";
	str1 += m_strHandle;
	messageClass.lpszClassName = str1.c_str();
	messageClass.lpfnWndProc = NplMessageWindowProc;

	if (!RegisterClass(&messageClass))
	{
		return ;
	}
	m_hwnd = CreateWindowEx(
		0,
		messageClass.lpszClassName, //窗口类名
		messageClass.lpszClassName, //窗口名
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
	//下面的代码不能调用，ShowWindow会发消息，此时messageloop还没有
	/*ShowWindow(m_hwnd, SW_HIDE);
	UpdateWindow(m_hwnd);*/

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);//翻译消息，如把WM_KEYDOWN和WM_KEYUP翻译成一个WM_CHAR消息
		DispatchMessage(&msg);//派发消息
	}
}

void NplMessageWindow::createWindowThread()
{
	std::thread trMessage([this]() {
		this->createWindow();
	});
	//trMessage.join(); //这个会阻断主线程
	trMessage.detach();
}


void NplMessageWindow::HandleCustomMsg(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;
	string s = (const char*)(pcds->lpData);
	json input = nlohmann::json::parse(s);
	string message_to = input["message_to"];
	string message_content = input["message_content"];

	//这种消息是从broswer发出来的
	if (message_to == "npl")
	{
		WriteLog("recieve mssage from broswer %s,content=====%s\n", message_to.c_str(), message_content.c_str());
		std::string defaultStr = "test message window";
		//发送数据给broswer后返回结果给npl
		NPLInterface::NPLObjectProxy data;
		data["otherMsg"] = message_content != "" ? message_content : defaultStr;
		string callFile = m_strDefaultCallFile;
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


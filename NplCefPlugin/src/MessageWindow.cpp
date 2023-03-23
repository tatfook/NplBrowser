#include "MessageWindow.h"
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

	LRESULT CALLBACK MessageWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
			MessageWindow::HandleCustomMsg(hwnd, wParam, lParam);
		}
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
}

MessageWindow::MessageWindow(std::string strHandle)
{
	m_pNamedPipe = nullptr;
	strWindowName = "";
	m_hwnd = nullptr;
	m_strHandle = strHandle;
	m_strDefaultCallFile = "script/apps/Aries/Creator/Game/NplBrowser/NplBrowserPlugin.lua";
}

MessageWindow::~MessageWindow()
{
	this->closeWindow();
}

//ParaEngine::CGUIWebBrowser::DefaultWinThreadProc()
//{
//	CoInitialize(NULL);
//	//
//	// create a window to render into. Register the windows class
//	// 
//	{
//		ParaEngine::Lock lock_(m_win_thread_mutex);
//
//		m_hBrowserHostWnd = CreateWindowExW(WS_EX_NOPARENTNOTIFY, L"static", NULL, WS_CHILD,
//			m_nWndX, m_nWndY, m_nWndWidth, m_nWndHeight,
//			CGlobals::GetAppHWND(), (HMENU)GetID(), CGlobals::GetApp()->GetModuleHandle(), NULL);
//
//		if (m_hBrowserHostWnd != 0)
//		{
//			m_dwWinThreadID = ::GetWindowThreadProcessId(m_hBrowserHostWnd, NULL);
//
//			// inform window creation
//			m_bWindowCreated = true;
//			PostWinThreadMessage(PE_WM_WEBBROWSER_CREATE_WINDOW, 1, 0);
//		}
//		else
//		{
//			OUTPUT_LOG("error: CreateWindow web browser failed\n");
//			m_bWindowCreated = true;
//			return;
//		}
//	}
//
//	//
//	// Dispatching window messages in this window thread. 
//	//
//
//	// Now we're ready to receive and process Windows messages.
//	MSG  msg;
//	msg.message = WM_NULL;
//	PeekMessageW(&msg, NULL, 0U, 0U, PM_NOREMOVE);
//
//	while (WM_QUIT != msg.message)
//	{
//		if (GetMessageW(&msg, NULL, 0U, 0U) != 0)
//		{
//			if (MsgProcCustom(msg.message, msg.wParam, msg.lParam) == 0)
//			{
//				// Translate and dispatch the message
//				TranslateMessage(&msg);
//				DispatchMessageW(&msg);
//			}
//		}
//	}
//	OUTPUT_LOG("web browser window thread exit\n");
//	m_dwWinThreadID = 0;
//}

void MessageWindow::createWindow()
{
	WNDCLASS messageClass{ 0 };
	string str1 = "MessageWindow";
	str1 += m_strHandle;
	messageClass.lpszClassName = str1.c_str();
	messageClass.lpfnWndProc = MessageWindowProc;

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
	ShowWindow(m_hwnd, SW_HIDE);
	UpdateWindow(m_hwnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);//翻译消息，如把WM_KEYDOWN和WM_KEYUP翻译成一个WM_CHAR消息
		DispatchMessage(&msg);//派发消息
	}
}

void MessageWindow::createWindowThread()
{
	std::thread trMessage(&MessageWindow::createWindow);
	trMessage.join();
}


void MessageWindow::HandleCustomMsg(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;
	string s = (const char*)(pcds->lpData);
	json input = nlohmann::json::parse(s);
	string message_to = input["message_to"];
	string message_content = input["message_content"];
	WriteLog("MessageWindow::HandleCustomMsg=================%s====%s\n", message_to.c_str(), message_content.c_str());
	
	//if (message_to == "browser")
	//{
	//	string cmd = input["cmd"];
	//	if (cmd == "Start")
	//	{
	//		return;
	//	}
	//	string id = input["id"];
	//	string parent_handle_s = input["parent_handle"];
	//	string cefclient_config_filename = input["cefclient_config_filename"];
	//	string pid = input["pid"];
	//	string callback_file = input["callback_file"];
	//	string key = id + "_" + parent_handle_s;
	//	HWND parent_handle = parent_handle_s.empty() ? NULL : (HWND)std::stoull(parent_handle_s.c_str());
	//	WriteLog("MessageWindow::HandleCustomMsg parent_handle_s=================%s\n", parent_handle_s.c_str());
	//	HWND hwnd = FindWindowEx(parent_handle, NULL, id.c_str(), NULL);
	//	if (hwnd)
	//	{
	//		//发送数据给broswer
	//		std::string json_str = input.dump();
	//		//LPSTR s = const_cast<char*>(json_str.c_str());
	//		//strcpy(s, input.dump().c_str());
	//		//COPYDATASTRUCT MyCDS;
	//		//MyCDS.dwData = 1; // function identifierz
	//		//MyCDS.cbData = strnlen(s, 4096) + 1; // size of data
	//		//MyCDS.lpData = s;           // data structure

	//		COPYDATASTRUCT MyCDS;
	//		MyCDS.dwData = 1; // function identifierz
	//		MyCDS.cbData = json_str.size() + 1; // size of data
	//		MyCDS.lpData = &(json_str[0]);

	//		LRESULT re = SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)(LPVOID)& MyCDS);
	//		if (re == 0)
	//		{
	//			WriteLog("MessageWindow::HandleCustomMsg sendMessage toBrowser=======%s\n", parent_handle_s.c_str());
	//		}

	//		//发送数据给broswer后返回结果给npl
	//		NPLInterface::NPLObjectProxy data;
	//		data["cmd"] = cmd;
	//		data["id"] = id;
	//		data["parent_handle"] = parent_handle_s;
	//		std::string defaultStr = "test message window";
	//		data["otherMsg"] = message_content != ""? message_content:defaultStr;
	//		if (m_pState)
	//		{
	//			string callFile = callback_file != "" ? callback_file : m_strDefaultCallFile;
	//			SendMessageToNpl(m_pState, callFile, data);
	//		}
	//	}
	//	else {
	//		WriteLog("can't find the window:%s\n", id.c_str());
	//	}
	//	return;
	//}

	//这种消息是从broswer发出来的
	if (message_to == "npl")
	{
		
		if (message_content != "" && message_content.find("paracraft") != message_content.npos) //console.log发出的消息
		{
			WriteLog("dddddddddddddd");
		}
		WriteLog("recieve mssage from broswer %s,content=====%s\n", message_to.c_str(), message_content.c_str());
		std::string defaultStr = "test message window";
		//发送数据给broswer后返回结果给npl
		NPLInterface::NPLObjectProxy data;
		data["otherMsg"] = message_content != "" ? message_content : defaultStr;
		string callFile = m_strDefaultCallFile;
		SendMessageToNpl(callFile, data);
	}
}
// h_pipe = ::CreateNamedPipe("\\\\.\\pipe\\MyPipe", PIPE_ACCESS_INBOUND, PIPE_READMODE_BYTE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, BUF_SIZE, BUF_SIZE, 0, nullptr);

#define  BUF_SIZE 327680  //最大32K数据
HANDLE MessageWindow::createNamedPipe()
{
	string str1 = "MessageWindow";
	str1 += m_strHandle;
	char buf_msg[BUF_SIZE];
	DWORD num_rcv; //实际接收到的字节数
	m_pNamedPipe = CreateNamedPipe("\\\\.\\Pipe\\mypipe", PIPE_ACCESS_INBOUND, PIPE_READMODE_BYTE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, BUF_SIZE, BUF_SIZE, 0, nullptr);
	if (m_pNamedPipe == INVALID_HANDLE_VALUE)
	{
		WriteLog("Failed to create named pipe!Error code\n");
		return nullptr;
	}
	else
	{
		WriteLog("Named pipe created successfully: %s\n", str1.c_str());
	}
	//等待命名管道客户端连接
	if (ConnectNamedPipe(m_pNamedPipe, nullptr))
	{
		WriteLog("A client connected...%s\n", str1.c_str());
		memset(buf_msg, 0, BUF_SIZE);
		//读取数据
		if (ReadFile(m_pNamedPipe, buf_msg, BUF_SIZE, &num_rcv, nullptr))
		{
			WriteLog("Message received: %s\n", &buf_msg);
		}
		else
		{
			WriteLog("Failed to receive message!Error code\n");
			return nullptr;
		}
	}
	return m_pNamedPipe;
}

void MessageWindow::closeWindow()
{
	if (m_hwnd)
	{
		DestroyWindow(m_hwnd);
		delete m_hwnd;
		m_hwnd = nullptr;
	}
	if (m_pNamedPipe)
	{
		CloseHandle(m_pNamedPipe);
		delete m_pNamedPipe;
		m_pNamedPipe = nullptr;
	}
}


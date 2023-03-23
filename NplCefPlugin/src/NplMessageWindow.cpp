#include "NplMessageWindow.h"
#include "NPLInterface.hpp"
using namespace nlohmann;
using namespace std;

//npl���
//static NPL::INPLRuntimeState* m_pState = nullptr;
static std::string m_strDefaultCallFile = "";
//����c����
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
		messageClass.lpszClassName, //��������
		messageClass.lpszClassName, //������
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
	//����Ĵ��벻�ܵ��ã�ShowWindow�ᷢ��Ϣ����ʱmessageloop��û��
	/*ShowWindow(m_hwnd, SW_HIDE);
	UpdateWindow(m_hwnd);*/

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);//������Ϣ�����WM_KEYDOWN��WM_KEYUP�����һ��WM_CHAR��Ϣ
		DispatchMessage(&msg);//�ɷ���Ϣ
	}
}

void NplMessageWindow::createWindowThread()
{
	std::thread trMessage([this]() {
		this->createWindow();
	});
	//trMessage.join(); //�����������߳�
	trMessage.detach();
}


void NplMessageWindow::HandleCustomMsg(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;
	string s = (const char*)(pcds->lpData);
	json input = nlohmann::json::parse(s);
	string message_to = input["message_to"];
	string message_content = input["message_content"];

	//������Ϣ�Ǵ�broswer��������
	if (message_to == "npl")
	{
		WriteLog("recieve mssage from broswer %s,content=====%s\n", message_to.c_str(), message_content.c_str());
		std::string defaultStr = "test message window";
		//�������ݸ�broswer�󷵻ؽ����npl
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


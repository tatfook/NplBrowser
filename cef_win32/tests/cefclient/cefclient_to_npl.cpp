#include "cefclient_to_npl.h"
#include "include/base/cef_logging.h"
#include <sstream>
using std::stringstream;
using namespace nlohmann;

json GenerateMessage(string str)
{
	json out;
	out["message_to"] = "npl";
	out["message_content"] = str;
	if (json::accept(str))
	{
		json s = json::parse(str);
		string callback_file = s["callback_file"];
		string browserName = s["broswer_name"];
		if (callback_file != "")
		{
			out["callback_file"] = callback_file;
		}
		if (browserName != "")
		{
			out["broswer_name"] = browserName;
		}
	}

	return out;
}
CefClientToNpl* CefClientToNpl::m_pInstance = nullptr;
CefClientToNpl::CefClientToNpl()
{
	m_pHwnd = nullptr;
}

CefClientToNpl::~CefClientToNpl()
{
}

CefClientToNpl* CefClientToNpl::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new CefClientToNpl();
	}
	return m_pInstance;
}

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

void createWindow(){}

void CefClientToNpl::SendMessageToWindow(std::string strMessage)
{
	this->SendMessageByWindow(strMessage);
}

bool CefClientToNpl::ResolveMessageFromJs(std::string strMessage)
{
	if (strMessage != "" && strMessage.find("paracraft:",0) == 0)
	{
		LOG(INFO) << "ResolveMessageFromJs Find the paracraft";
		std::string strJs = strMessage.replace(0,10,"paracraft:");
		if (strJs != "")
		{
			LOG(INFO) << "ResolveMessageFromJs send message";
			this->SendMessageToWindow(strJs);
			return true;
		}
		LOG(INFO) << "ResolveMessageFromJs send message fail";
		return false;
	}
	LOG(INFO) << "ResolveMessageFromJs send message fail1";
	return false;
}

void CefClientToNpl::SetParentHandle(std::string strHandle)
{
	m_strHandle = strHandle;
}

void CefClientToNpl::SendMessageByWindow(std::string strMessage)
{
	json message = GenerateMessage(strMessage);
	std::string strName = "MessageWindow";
	strName += m_strHandle;
	std::wstring wStrName = CharToWchar(strName.c_str());
	LOG(INFO) << "SendMessageToWindow Find the paracraft : " << strName << wStrName;
	if (!m_pHwnd)
	{
		m_pHwnd = FindWindow(wStrName.c_str(), wStrName.c_str());
	}
	if (m_pHwnd && IsWindow(m_pHwnd))
	{
		LOG(INFO) << "SendMessageToWindow Find the window : " << strName;
		std::string json_str = message.dump();

		COPYDATASTRUCT MyCDS;
		MyCDS.dwData = 1; // function identifierz
		MyCDS.cbData = json_str.size() + 1; // size of data
		MyCDS.lpData = &(json_str[0]);           // data structure
		LOG(INFO) << "SendMessageToWindow Find the window2 : " << json_str.c_str();
		LRESULT re = SendMessage(m_pHwnd, WM_COPYDATA, 0, (LPARAM)(LPVOID)& MyCDS);
		if (re == 0)
		{
			LOG(INFO) << "SendMessageToWindow failed : " << strName << strMessage;
		}
		else
		{
			LOG(INFO) << "SendMessageToWindow success : " << strName;
		}
	}
}

void CefClientToNpl::CloseLink()
{
	if (m_pHwnd)
	{
		delete m_pHwnd;
		m_pHwnd = nullptr;
	}
}







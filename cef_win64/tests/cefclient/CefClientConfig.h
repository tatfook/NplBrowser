#ifndef CefClientConfig_H
#define CefClientConfig_H
#include <string>
#include <map>
#include <vector>
#include "json.hpp"
#include <sstream>
using namespace nlohmann;
namespace client {

    class CefClientConfig
    {
    public:
        CefClientConfig(std::string filename, std::string pid);
        ~CefClientConfig();
        static CefClientConfig* GetInstance(std::string filename, std::string pid);
        // write value to file and close file 
        void WriteJsonValue(std::string key, bool value);
        void ReadJsonValue(std::string key, bool& value);
        void EraseJsonValue(std::string key);
        void Clear();

        template <class T>
        static inline std::string ToString(const T& t)
        {
            std::stringstream ss;
            ss << t;
            return ss.str();
        }
    private:
        
        nlohmann::json& ReadRoot();
        void WriteRoot(nlohmann::json& root);
        nlohmann::json& CreateOrGetConfig(nlohmann::json& root, std::string pid);
        void SetConfigValue(nlohmann::json& root, std::string pid, std::string key, bool value);
        json mValue;
        std::string mPid;
        std::string mFileName;
        nlohmann::json mRoot;
    };
}
#endif
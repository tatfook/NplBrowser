#include "CefClientConfig.h"
#include "include/base/cef_logging.h"
#include <fstream> 
#include <iomanip>
#include <algorithm>
#include "json.hpp"
namespace client
{
    static CefClientConfig* instance = NULL;
    CefClientConfig::CefClientConfig(std::string filename, std::string pid)
        : mFileName(filename)
        , mPid(pid)
    {

    }

    CefClientConfig::~CefClientConfig()
    {
    }

    CefClientConfig* CefClientConfig::GetInstance(std::string filename, std::string pid)
    {
        if (!instance)
        {
            instance = new CefClientConfig(filename,pid);
        }
        return instance;
    }

    void CefClientConfig::WriteJsonValue(std::string key, bool value)
    {
        nlohmann::json& root = ReadRoot();
        // set value
        SetConfigValue(root, mPid, key, value);
        //save value
        WriteRoot(root);
    }


    void CefClientConfig::ReadJsonValue(std::string key, bool& value)
    {
        nlohmann::json& config = CreateOrGetConfig(ReadRoot(), mPid);
        if (config.find(key) != config.end()) {
            if (config[key].is_boolean())
            {
                value = config[key];
            }
        }
    }

    void CefClientConfig::EraseJsonValue(std::string key)
    {
        nlohmann::json& root = ReadRoot();
        nlohmann::json& config = CreateOrGetConfig(ReadRoot(), mPid);
        config.erase(key);
        if (config.empty())
        {
            root.erase(mPid);
        }
        //save value
        WriteRoot(root);
    }

    void CefClientConfig::Clear()
    {
        LOG(INFO) << "clear CefClientConfig with pid:" << mPid << " in file " << mFileName;
        nlohmann::json& root = ReadRoot();
        if (!mRoot[mPid].is_null())
        {
            root.erase(mPid);
            WriteRoot(root);
        }
    }

    nlohmann::json& CefClientConfig::CreateOrGetConfig(nlohmann::json& root, std::string pid)
    {
        if (root.find(pid) == root.end())
        {
            nlohmann::json value;
            root[pid] = value;
        }
        return root[pid];
    }
    
    void CefClientConfig::SetConfigValue(nlohmann::json& root, std::string pid, std::string key, bool value)
    {
        nlohmann::json& config = CreateOrGetConfig(root,pid);
        config[key] = value;
    }

    nlohmann::json& CefClientConfig::ReadRoot()
    {
        mRoot.clear();
        try
        {
            std::ifstream input_json_file(mFileName);
            if (input_json_file.is_open())
            {
                // read 
                input_json_file >> mRoot;
                input_json_file.close();
            }
            else
            {
                WriteRoot(mRoot);
                LOG(INFO) << "create empty CefClientConfig:" << mFileName;
            }
        }
        catch (const std::exception&)
        {

        }
        
        LOG(INFO) << "read root:" << mRoot;
        return mRoot;
    }

    void CefClientConfig::WriteRoot(nlohmann::json& root)
    {
        try
        {
            std::ofstream out_json_file(mFileName);
            if (out_json_file.is_open())
            {
                out_json_file << std::setw(4) << root << std::endl;
                out_json_file.close();
            }
            else
            {
                LOG(INFO) << "can't write CefClientConfig:" << mFileName;
            }
        }
        catch (const std::exception&)
        {

        }
        
    }

}



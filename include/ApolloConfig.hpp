#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <shared_mutex>

/*
* 接口文档参考官方
* https://www.apolloconfig.com/#/zh/client/other-language-client-user-guide
*/

namespace apollocpp{

enum class HttpApiType: uint8_t{
    RETURNJSON_CACHE = 1,   // 这是http缓存的接口返回为json数据
    RETURNSTR_CACHE,        // 而这是http带缓存的st数据
    RETURN_WITHOUT_CACHE_DATABASES
};
struct EvnData
{
    std::string host;
    std::string appid;
    std::string cluster;
    std::string namespaceName;
};

class ApolloConfig {
private:
std::shared_mutex dataMutex;

public:
    ApolloConfig(const std::string& host,const std::string& appId, const std::string& cluster, const std::string& namespaceName);
    ApolloConfig(const EvnData& evn);
    ~ApolloConfig();

    void fetchConfigJsonCache();
    void fetchConfigStrCache();
    void fetchConfigDatabases();
    // 这里实时的更新，加下锁
    std::unordered_map<std::string, std::string> getConfigData()const;
    std::string getValue(const std::string& key);
    void viewDataJson();
    std::string getDataString();
private:
    std::string host;
    std::string appId;
    std::string cluster;
    std::string namespaceName;
    std::string urlJsonCache;
    std::string urlStrCache;
    std::string urlDatabases;
    std::string releaseKey;
    std::string messages;
    std::string label;
    std::string clientip;
    // std::mutex mut;
    std::unordered_map<std::string, std::string> configData;
    std::string configDataStr;
    void parseConfigJsonCache(const std::string& response);
    void parseConfigStrCache(const std::string& response);
    void parseConfigDatabases(const std::string& response);

    void cleanCache();
};
}

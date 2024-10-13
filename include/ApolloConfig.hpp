#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <shared_mutex>
#include <log4cpp/Category.hh>
#include <thread>
#include <condition_variable>
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
    std::string host; // 必填
    std::string appid; // 必填
    std::string cluster; // 必填
    std::string namespaceName; // 必填
    std::string secret;
    std::string releaseKey;
    std::string ip;
    std::string label;
    std::string message;
    uint8_t sleepsecondtime= static_cast<uint8_t>(600);
};

// class RemoteConfigLongPollService;

class ApolloConfig {
private:
log4cpp::Category& log = log4cpp::Category::getRoot(); 
public:
    // friend RemoteConfigLongPollService;
    ApolloConfig(
        const std::string& host,
        const std::string& appId, 
        const std::string& cluster, 
        const std::string& namespaceName, 
        const std::string& secret="",
        const std::string& releaseKey= "",
        const std::string& ip="", 
        const std::string& label="" ,
        const std::string& message="", 
        const uint8_t& sleepsecondtime=(uint8_t)600);
    ApolloConfig(const EvnData& evn);
    ~ApolloConfig();

    void fetchConfigJsonCache();
    void fetchConfigPropertiesCache();
    void fetchConfigDatabases();

    std::string getUrlJson() const ;
    std::string getUrlProperties() const;
    std::string getUrlDatabases() const;

    // 这里实时的更新，加下锁
    std::unordered_map<std::string, std::string> getConfigData();
    std::string getValue(const std::string& key);
    void viewDataJson();
    std::string getDataString();
    void stopBeat(); // 关闭心跳
    void startBeat();// 开启心跳
private:
    std::string host;
    std::string appId;
    std::string cluster;
    std::string namespaceName;
    std::string secret;
    std::string releaseKey;
    std::string clientip;
    std::string label;
    std::string messages;
    uint8_t sleepSecondTime;//s为单位

    bool apolloconfigstop = false;
    std::shared_mutex dataMutex;
    std::mutex beatmutex; 
    std::thread th;
    bool startheatbeat = false; // 心跳默认不开启，当你进行灰度的时候，将会自动开启心跳保证能够正常灰度，后面可手动关闭灰度
    std::condition_variable cond;

    std::string urlJsonCache;
    std::string urlStrCache;
    std::string urlDatabases;

    // std::mutex mut;
    std::unordered_map<std::string, std::string> configData;
    std::string configDataStr;
    void parseConfigJsonCache(const std::string& response);
    void parseConfigPropertiesCache(const std::string& response);
    void parseConfigDatabases(const std::string& response);
    void cleanCache();
    void headbeat();
};
}

#include "ApolloConfig.hpp"
#include "HttpClient.hpp"
#include<iostream>
#include<nlohmann/json.hpp>
#include<sstream>
#include"common.hpp"
// 就是需要加入ip参数，手动加，默认是不加的。
// 由于如果直接使用aolloconfig的功能的时候，那么就是说明不适用长轮训，那么就是可以进行

namespace apollocpp{
ApolloConfig::ApolloConfig(
    const std::string& host, 
    const std::string& appId, 
    const std::string& cluster, 
    const std::string& namespaceName, 
    const std::string& secret,
    const std::string& releaseKey,
    const std::string& ip , 
    const std::string& label,
    const std::string& message,
    const uint8_t& sleepsecondtime
    )
    :host(host), 
    appId(appId),
    cluster(cluster), 
    namespaceName(namespaceName), 
    secret(secret), 
    releaseKey(releaseKey), 
    clientip(ip), 
    label(label), 
    messages(message),
    sleepSecondTime(sleepsecondtime) 
    {
        urlJsonCache = "http://"+host+"/configfiles/json/"+appId+"/"+cluster+"/"+namespaceName;
        urlStrCache = "http://"+host+"/configfiles/"+appId+"/"+cluster+"/"+namespaceName;
        urlDatabases = "http://"+host+"/configs/"+appId+"/"+cluster+"/"+namespaceName;
        // // 通过这里组装三种url
        startheatbeat = false; // 开启心跳
        // 首先对1.2 通过带缓存的Http接口从Apollo读取配置
        if (!ip.empty()){
            urlJsonCache+="?ip="+ip;
            startheatbeat = true;
        }

        // 对1.3后面进行加入
        if (!releaseKey.empty() or !messages.empty() or !label.empty() or !clientip.empty()){
            urlStrCache+="?";
            if (!releaseKey.empty()){
                if (urlStrCache.back()!='?') urlStrCache.push_back('&');
                urlStrCache+="releaseKey="+releaseKey;
            }

            if (!messages.empty()){
                if (urlStrCache.back() != '?') urlStrCache.push_back('&');
                urlStrCache+="messages="+url_encode(messages);
            }

            if (!label.empty()){
                startheatbeat = true;
                if (urlStrCache.back() != '?') urlStrCache.push_back('&');
                urlStrCache+="label="+label;
            }

            if (!ip.empty()){
                if (urlStrCache.back() != '?') urlStrCache.push_back('&');
                urlStrCache+="ip="+ip;
            }

        }

        // 开启心跳
        if (startheatbeat){
            startBeat(); // 开启心跳
        }
        log.info("apollo config initialization completed.");
    }

ApolloConfig::ApolloConfig(const EvnData& evn):ApolloConfig(
        evn.host, 
        evn.appid, 
        evn.cluster, 
        evn.namespaceName, 
        evn.secret, 
        evn.releaseKey,
        evn.ip, 
        evn.label,
        evn.message,
        evn.sleepsecondtime
        ){

}

ApolloConfig::~ApolloConfig() {
    // 需要关闭心跳线程
    if(th.joinable()){
        stopBeat();
        th.join();// 等待心跳线程退出
    }
}

// 带缓存的"http://localhost:8080/configfiles/json/aldd/arvintzhang/RJDLOG";
// http://localhost:8070/config.html#/appid=aldd&env=LOCAL&cluster=arvintzhang
// 该接口会直接从数据库中获取配置，可以配合配置推送通知实现实时更新配置。
// URL: {config_server_url}/configs/{appId}/{clusterName}/{namespaceName}?releaseKey={releaseKey}&messages={messages}&label={label}&ip={clientIp}
void ApolloConfig::fetchConfigJsonCache() {
    // HttpClient client;    
    std::string response = HttpClient::get(urlJsonCache, appId, secret);
    if (response.empty()) log.info("response is empty.");
    parseConfigJsonCache(response);
}

void ApolloConfig::fetchConfigPropertiesCache() {
    // HttpClient client;    
    std::string response = HttpClient::get(urlStrCache,appId, secret);
    if (response.empty()) log.info("response is empty.");
    parseConfigPropertiesCache(response);
}

void ApolloConfig::fetchConfigDatabases()
{
    // HttpClient client;    
    std::string response = HttpClient::get(urlDatabases,appId, secret);
    if (response.empty()) log.info("response is empty.");
    parseConfigDatabases(response);
}

std::string ApolloConfig::getUrlJson() const
{
    return urlJsonCache;
}

std::string ApolloConfig::getUrlProperties() const
{
    return urlStrCache;
}

std::string ApolloConfig::getUrlDatabases() const
{
    return urlDatabases;
}

std::string ApolloConfig::getValue(const std::string& key) {
    if (configData.count(key)) return configData[key];
    return "";
}

void ApolloConfig::parseConfigJsonCache(const std::string& response) {
    nlohmann::json j2;
    std::lock_guard<std::shared_mutex> lock(dataMutex);
    cleanCache();
    try
    {
        j2 = nlohmann::json::parse(response.c_str());
        for (auto&[key, value]:j2.items()){
            configData[key] = value;
        }
    }
    catch (nlohmann::json::parse_error& ex)
    {
        // std::cerr << "parse error " << ex.what() << std::endl;
        log.error("parse error"+std::string(ex.what()));
        return ;
    }
    configDataStr = j2.dump();
}

void ApolloConfig::parseConfigPropertiesCache(const std::string& response) {
    std::stringstream ss(response);
    std::string s;
    std::lock_guard<std::shared_mutex> lock(dataMutex);
    cleanCache();
    while (std::getline(ss, s))
    {
        int pos  = s.find('=');
        configData[s.substr(0,pos)] = s.substr(pos+1, s.size()-pos-1);
    }
    configDataStr = response;
}

void ApolloConfig::parseConfigDatabases(const std::string& response) {
    nlohmann::json j2;
    std::lock_guard<std::shared_mutex> lock(dataMutex);
    cleanCache();
    try
    {
        j2 = nlohmann::json::parse(response.c_str());
        releaseKey = j2["releaseKey"];
        for (auto&[key, value]:j2["configurations"].items()){
            configData[key] = value;
        }
    }
    catch (nlohmann::json::parse_error& ex)
    {
        std::cerr << "parse error " << ex.what() << std::endl;
        return ;
    }
    configDataStr = j2.dump();
}

void ApolloConfig::cleanCache()
{
    configData.clear();
}

void ApolloConfig::headbeat()
{
    log.info("start heart beat.");
    while (true)
    {
        {
            std::unique_lock<std::mutex> lock(beatmutex);
            cond.wait_for(lock, std::chrono::seconds(sleepSecondTime), [this](){return !this->startheatbeat;});
            if(!this->startheatbeat){
                break;
            }
        }
        log.info("send heart beat.");
        HttpClient::get(urlStrCache, appId, secret);
    }
    log.info("head beat exit."); // 心跳退出
}

void ApolloConfig::stopBeat()
{
    std::this_thread::sleep_for(std::chrono::seconds(2)); // 防止唤醒失效
    {
        std::lock_guard<std::mutex> lock(beatmutex);
        if (!startheatbeat){
            log.info("stop heart beat.");
            return;
        }
        startheatbeat = false; // 心跳关闭
        cond.notify_one(); // 通知 但是可能会陷入唤醒失效，这个为了防止在线程中进行startheatbeat判断进行防止
    }
    log.info("stop heart beat.");
}

void ApolloConfig::startBeat()
{
    log.info("head beat start.");
    th = std::thread(&ApolloConfig::headbeat, this);
}

// 对于定时的感悟，如果说线程直接sleep这里，那么就没办法及时的处理其他任务。再者如果说sleep10分钟，那么当服务器退出的时候，这个心跳或者定时那么将直接定死在这里了。
// 需要等待10分钟，服务器才能正常的退出，这将是个失败的设计。

void ApolloConfig::viewDataJson()
{
    nlohmann::json j2;
    {
        std::shared_lock<std::shared_mutex> lock(dataMutex);
        j2 = configData;
    }
    std::cout<<j2<<std::endl;
}

std::string ApolloConfig::getDataString()
{
    std::shared_lock<std::shared_mutex> lock(dataMutex);
    return configDataStr;
}

std::unordered_map<std::string, std::string> ApolloConfig::getConfigData(){
    std::shared_lock<std::shared_mutex> lock(dataMutex);
    return configData;
}

}
#include "ApolloConfig.hpp"
#include "HttpClient.hpp"
#include<iostream>
#include<nlohmann/json.hpp>
#include<sstream>

namespace apollocpp{
ApolloConfig::ApolloConfig(const std::string& host, const std::string& appId, const std::string& cluster, const std::string& namespaceName)
    :host(host), appId(appId), cluster(cluster), namespaceName(namespaceName) {
        // 通过这里组装三种url
        urlJsonCache = "http://"+host+"/configfiles/json/"+appId+"/"+cluster+"/"+namespaceName;
        urlStrCache = "http://"+host+"/configfiles/"+appId+"/"+cluster+"/"+namespaceName;
        urlDatabases = "http://"+host+"/configs/"+appId+"/"+cluster+"/"+namespaceName;
        bool flag = true;
        if (!releaseKey.empty()){
            urlDatabases.push_back('?');
            flag = false;
            urlDatabases.append("releaseKey=");
            urlDatabases.append(releaseKey);
        }
        if (!messages.empty()){
            if (flag) urlDatabases.append("?");
            flag = false;
            urlDatabases.append("&messages=");
            urlDatabases.append(messages);
        }
        if (!label.empty()){
            if (flag) urlDatabases.append("?");
            flag = false;
            urlDatabases.append("&label=");
            urlDatabases.append(label);
        }
        if (!clientip.empty()){
            if (flag) urlDatabases.append("?");
            flag = false;
            urlDatabases.append("&ip=");
            urlDatabases.append(clientip);
        }
    }

ApolloConfig::ApolloConfig(const EvnData& evn){
    ApolloConfig(evn.host, evn.appid, evn.cluster, evn.namespaceName);
}

ApolloConfig::~ApolloConfig() {}

// 带缓存的"http://localhost:8080/configfiles/json/aldd/arvintzhang/RJDLOG";
// http://localhost:8070/config.html#/appid=aldd&env=LOCAL&cluster=arvintzhang
// 该接口会直接从数据库中获取配置，可以配合配置推送通知实现实时更新配置。
// URL: {config_server_url}/configs/{appId}/{clusterName}/{namespaceName}?releaseKey={releaseKey}&messages={messages}&label={label}&ip={clientIp}
void ApolloConfig::fetchConfigJsonCache() {
    // HttpClient client;    
    std::string response = HttpClient::get(urlJsonCache);
    parseConfigJsonCache(response);
}

void ApolloConfig::fetchConfigStrCache() {
    // HttpClient client;    
    std::string response = HttpClient::get(urlStrCache);
    parseConfigStrCache(response);
}
void ApolloConfig::fetchConfigDatabases() {
    // HttpClient client;    
    std::string response = HttpClient::get(urlDatabases);
    parseConfigDatabases(response);
}

std::string ApolloConfig::getValue(const std::string& key) {
    if (configData.count(key)) return configData[key];
    return "";
}

void ApolloConfig::parseConfigJsonCache(const std::string& response) {
    nlohmann::json j2;
    try
    {
        j2 = nlohmann::json::parse(response.c_str());
        for (auto&[key, value]:j2.items()){
            configData[key] = value;
        }
    }
    catch (nlohmann::json::parse_error& ex)
    {
        std::cerr << "parse error " << ex.what() << std::endl;
        return ;
    }
}

void ApolloConfig::parseConfigStrCache(const std::string& response) {
    std::stringstream ss(response);
    std::string s;
    while (std::getline(ss, s))
    {
        int pos  = s.find('=');
        configData[s.substr(0,pos)] = s.substr(pos+1, s.size()-pos-1);
    }
    for (auto&[key, val]:configData){
        std::cout<<key<< " "<<val<<std::endl;
    }
}

void ApolloConfig::parseConfigDatabases(const std::string& response) {
    nlohmann::json j2;
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
}

std::unordered_map<std::string, std::string> ApolloConfig::getConfigData() const{
    return configData;
}

}
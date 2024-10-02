#pragma once
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <mutex>
#include <shared_mutex>
#include <random>
#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include "ApolloConfig.hpp"
#include "HttpClient.hpp"
#define APOLLOCONFIGCACHESIZE 2000 // 保存的是2000个

// 此为单应用单集群多命名空间配置感知
// 后序在这个基础上开发多apollo多应用多集群，多命名空间配置感知系统

namespace apollocpp {


class RemoteConfigLongPollService {
private:
    std::atomic<bool> longPollStarted;
    std::atomic<bool> longPollingStopped;
    std::thread longPollingThread;
    std::unordered_map<std::string, int>  namespaceToNotificationsId;
    std::vector<std::unordered_map<std::string, std::string>> notifications;
    std::shared_mutex notificationsMutex;
    std::string appId;
    std::string cluster;
    std::string host;
    long longPollingTimeout;
    std::unordered_map<std::string, ApolloConfig> apolloconfig; // 保存配置结果
    size_t apolloconfigSize; //配置结果的大小控制防止内存爆掉 // 初步想法是lru
    bool notificationsUpdateflag = false;

    std::string configJsonFile;
    // Simulated function to perform HTTP GET request
    void httpGet(const std::string& url);

    // Callback function for libcurl to write response data
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* s);
    // Long polling logic
    void doLongPollingRefresh();

    std::string assembleLongPollRefreshUrl();
    void processResponse(const std::string& response);
public:
    RemoteConfigLongPollService(const std::string& host, const std::string& appId, const std::string& cluster, const std::string& namesapceName="", long longPollingTimeout=60L)
        : longPollStarted(false), longPollingStopped(false), appId(appId), cluster(cluster), host(host) {
        if (!namesapceName.empty()){
            addNotifications(namesapceName); // 增加命名空间
        }
    }
    
    RemoteConfigLongPollService(const std::string& configJsonFile);
    ~RemoteConfigLongPollService() {
        stopLongPolling();
        if (longPollingThread.joinable()) {
            longPollingThread.join();
        }
        std::cout<<"[info] long poll service stoped."<<std::endl;
    }

    void startLongPolling();
    void stopLongPolling();
    // 增加命名空间
    void addNotifications(const std::string& namespaceName);
    void addNotifications(const std::vector<std::string>& vec);
    void addNotifications(const std::unordered_map<std::string, std::string> &maps);

    // 删除某个命名空间
    void deleteNotifications(const std::string& namespaceName);

    // 改命名空间的值
    void modifyNotifitionId(const std::string& namespaceName, const std::string& NotifitionIdStr);
    void modifyNotifitionId(const std::string& namespaceName, int NotifitionIdINT);
    void modifyNotifitionId(const std::string& namespaceName, const std::string& NotifitionIdStr ,int NotifitionId);

    // 控制配置数据size
    void setApolloconfigSzie(size_t size){apolloconfigSize = (size>APOLLOCONFIGCACHESIZE?APOLLOCONFIGCACHESIZE:size);}
    size_t getApolloconfigSzie()const{return apolloconfigSize;}
    
    // 刷新配置
    void refreshConnectApolloConfig();
    void notifyClients(const std::string& reNamespaceName);
};
}  // namespace apollocpp


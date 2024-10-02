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

namespace apollocpp {

class apolloNode{
public:
    int namespaceName; // 命名空间
    ApolloConfig apolloc; // 所保存的数据
    apolloNode* pre;
    apolloNode* next;
};


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
    int longPollingTimeout;
    std::unordered_map<std::string, ApolloConfig> apolloconfig;
    size_t apolloconfigSize;
    bool notificationsUpdateflag = false;
    // Simulated function to perform HTTP GET request
    // 长连接请求
    void httpGet(const std::string& url);

    // Callback function for libcurl to write response data
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* s);
    // Long polling logic
    void doLongPollingRefresh();

    std::string assembleLongPollRefreshUrl();
    void processResponse(const std::string& response);

    void notifyClients();

public:
    RemoteConfigLongPollService(const std::string& host, const std::string& appId, const std::string& cluster, const std::string& namesapceName="")
        : longPollStarted(false), longPollingStopped(false), appId(appId), cluster(cluster), host(host) {
        longPollingTimeout = 90000; // 90 seconds
        if (!namesapceName.empty()){
            addNotifications(namesapceName); // 增加命名空间
        }
    }

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
    void addNotifications(const std::string& namespaceName,const std::string& notificationId="-1");
    void addNotifications(const std::unordered_map<std::string, std::string>& maps);

    // 删除某个命名空间
    void deleteNotifications(const std::string& namespaceName);

    // 改命名空间的值
    void modifyNotifitionId(const std::string& namespaceName, const std::string& NotifitionIdStr);
    void modifyNotifitionId(const std::string& namespaceName, int NotifitionIdINT);
    void modifyNotifitionId(const std::string& namespaceName, const std::string& NotifitionIdStr ,int NotifitionId);

    // 控制配置数据size
    void setApolloconfigSzie(size_t size){apolloconfigSize = (size>APOLLOCONFIGCACHESIZE?APOLLOCONFIGCACHESIZE:size);}
    size_t getApolloconfigSzie()const{return apolloconfigSize;}
};
}  // namespace apollocpp


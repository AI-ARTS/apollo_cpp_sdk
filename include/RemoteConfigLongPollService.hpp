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
#include <random>
#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include "HttpClient.hpp"
#include "common.hpp"

namespace apollocpp {

class RemoteConfigLongPollService {
private:
    std::atomic<bool> longPollStarted;
    std::atomic<bool> longPollingStopped;
    std::thread longPollingThread;
    std::vector<std::unordered_map<std::string, long>> notifications;
    std::mutex notificationsMutex;
    std::string appId;
    std::string cluster;
    std::string host;
    int longPollingTimeout;

    // Simulated function to perform HTTP GET request
    std::string httpGet(const std::string& url);

    // Callback function for libcurl to write response data
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* s);
    // Long polling logic
    void doLongPollingRefresh();

    std::string assembleLongPollRefreshUrl();
    void processResponse(const std::string& response);

    void notifyClients();

public:
    RemoteConfigLongPollService(const std::string& host, const std::string& appId, const std::string& cluster)
        : longPollStarted(false), longPollingStopped(false), appId(appId), cluster(cluster), host(host) {
        longPollingTimeout = 90000; // 90 seconds
    }

    ~RemoteConfigLongPollService() {
        stopLongPolling();
        if (longPollingThread.joinable()) {
            longPollingThread.join();
        }
    }

    void startLongPolling();
    void stopLongPolling();
};
}  // namespace apollocpp

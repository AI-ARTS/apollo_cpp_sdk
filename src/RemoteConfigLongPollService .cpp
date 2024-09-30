
#include"RemoteConfigLongPollService.hpp"
#include<iostream>
namespace apollocpp{

std::string RemoteConfigLongPollService::httpGet(const std::string& url) {
    return HttpClient::get(url);
}

size_t RemoteConfigLongPollService::writeCallback(void* contents, size_t size, size_t nmemb, std::string* s){
    size_t newLength = size * nmemb;
    s->append((char*)contents, newLength);
    return newLength;
}

void RemoteConfigLongPollService::doLongPollingRefresh(){
    while (!longPollingStopped) {
        // Simulate building the polling URL
        std::string url = assembleLongPollRefreshUrl();

        try {
            std::string response = httpGet(url);
            processResponse(response);
            std::this_thread::sleep_for(std::chrono::seconds(5));
        } catch (const std::exception& e) {
            std::cerr << "Error during long polling: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));  // Retry with delay
        }
    }
}

std::string RemoteConfigLongPollService::assembleLongPollRefreshUrl(){
    std::lock_guard<std::mutex> lock(notificationsMutex);
    // Simulate the building of URL
    // 这里重新组装url。然后长连接轮训
    nlohmann::json j = notifications;
    std::string jData = j.dump();
    std::string encoded_notifications = url_encode(jData);
    std::string url = "http://"+host+"/notifications/v2?appId=" + appId + "&cluster=" + cluster+"&notifications="+encoded_notifications;
    std::cout<<"URL:"<<url<<std::endl;
    return url;
}

    void RemoteConfigLongPollService::processResponse(const std::string& response) {
        if (response.empty()) {
            return;
        }

        // Parse JSON response (simulated as JSON string)
        nlohmann::json responseJson = nlohmann::json::parse(response);
        std::vector<std::unordered_map<std::string, std::string>> maps;
        for (const auto& notification : responseJson) {
            std::lock_guard<std::mutex> lock(notificationsMutex);
            maps.push_back(notification);
        }

        // Simulate notification to other components
        notifyClients();
    }

    void RemoteConfigLongPollService::notifyClients() {
        std::cout << "Notifying clients of configuration changes..." << std::endl;
    }

    void RemoteConfigLongPollService::startLongPolling() {
        if (longPollStarted.exchange(true)) {
            // Already started
            return;
        }

        longPollingStopped = false;
        longPollingThread = std::thread(&RemoteConfigLongPollService::doLongPollingRefresh, this);
    }
    void RemoteConfigLongPollService::stopLongPolling() {
        longPollingStopped = true;
    }
    
}




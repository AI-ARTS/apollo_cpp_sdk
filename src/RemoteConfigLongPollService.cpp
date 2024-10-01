
#include"RemoteConfigLongPollService.hpp"
#include<iostream>
#include "common.hpp"
#include "ApolloConfig.hpp"

namespace apollocpp{
void RemoteConfigLongPollService::httpGet(const std::string& url) {
 CURL* curl = curl_easy_init();
    std::string readBuffer;

    if (curl) {
        // 设置长连接的选项
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);

        // 启用 TCP KeepAlive
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
        curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);

        while (!longPollingStopped) {
            readBuffer.clear();  // 清空读取缓冲区
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);  // 超时设置
            CURLcode res = curl_easy_perform(curl);

            if (res != CURLE_OK) {
                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
                readBuffer.clear();  // 清空读取缓冲区
                break;  // 出错则退出循环
            }
            // 处理数据
            processResponse(readBuffer);
            {
                // 读锁
                std::shared_lock<std::shared_mutex> lock(notificationsMutex);
                if (notificationsUpdateflag){
                    // url需要更新
                    break;
                }
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        curl_easy_cleanup(curl);  // 关闭连接
    }
}

size_t RemoteConfigLongPollService::writeCallback(void* contents, size_t size, size_t nmemb, std::string* s){
    size_t newLength = size * nmemb;
    s->append((char*)contents, newLength);
    return newLength;
}

void RemoteConfigLongPollService::doLongPollingRefresh(){
    
    // Simulate building the polling URL
    while (!longPollingStopped)
    {
        std::string url = assembleLongPollRefreshUrl();
        {
            std::lock_guard<std::shared_mutex> lock(notificationsMutex);
            notificationsUpdateflag = false; // 更新
        }
        httpGet(url);
    }
}

std::string RemoteConfigLongPollService::assembleLongPollRefreshUrl(){
    nlohmann::json j;
    {
        std::lock_guard<std::shared_mutex> lock(notificationsMutex);
        j = notifications;
    }
    // Simulate the building of URL
    // 这里重新组装url。然后长连接轮训
    std::string jData = j.dump();
    std::cout<<"jData: "<<jData<<std::endl;
    std::string encoded_notifications = apollocpp::url_encode(jData);
    std::string url = "http://"+host+"/notifications/v2?appId=" + appId + "&cluster=" + cluster+"&notifications="+encoded_notifications;
    // std::cout<<"URL:"<<url<<std::endl;
    return url;
}

    void RemoteConfigLongPollService::processResponse(const std::string& response) {
        if (response.empty()) {
            return;
        }
        std::cout<< response<<std::endl;
        // Parse JSON response (simulated as JSON string)
        nlohmann::json responseJson = nlohmann::json::parse(response);
        std::vector<std::unordered_map<std::string, std::string>> maps;
        for (const auto& notification : responseJson) {
            std::string reNamespaceName = notification["namespaceName"];
            int reNotificationId =  notification["notificationId"];
            // 拿到通知的结果
            std::cout<<"reNamespaceName: "<<reNamespaceName<<" reNotificationId: "<<reNotificationId<<std::endl;
            // 现在就是要通知某个namespace的值更新了
            if (namespaceToNotificationsId[reNamespaceName]!=reNotificationId){
                modifyNotifitionId(reNamespaceName,reNotificationId);
            }
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

    void RemoteConfigLongPollService::addNotifications(const std::string& namespaceName,const std::string& notificationId){
        std::unordered_map<std::string, std::string> maps{{"namespaceName",namespaceName}, {"notificationId",notificationId}};
        std::lock_guard<std::shared_mutex> lock(notificationsMutex);
        try
        {
            namespaceToNotificationsId[namespaceName] = std::stoi(notificationId);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return; // 增加失败
        }
        notifications.push_back(maps);
        {
            std::lock_guard<std::shared_mutex> lock(notificationsMutex);
            notificationsUpdateflag = true; // 更新
        }
    }

    void RemoteConfigLongPollService::addNotifications(const std::unordered_map<std::string, std::string> &maps)
    {
        std::lock_guard<std::shared_mutex> lock(notificationsMutex);
        std::unordered_map<std::string, int> temp;
        try
        {
            for (const auto& [key, val]:maps){
                temp[key] = std::stoi(val);
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return; // 增加失败
        }
        {
            std::lock_guard<std::shared_mutex> lock(notificationsMutex);
            notifications.push_back(maps);
            for (const auto&[key, val]:temp){
                namespaceToNotificationsId[key] = val;
            }
            notificationsUpdateflag = true; // 更新
        }
    }

void RemoteConfigLongPollService::deleteNotifications(const std::string &namespaceName){
    std::lock_guard<std::shared_mutex> lock(notificationsMutex);
    if (namespaceToNotificationsId.count(namespaceName)){
        namespaceToNotificationsId.erase(namespaceName);
        for (auto it = notifications.begin();it!=notifications.end();it=std::next(it)){
            if ((*it).count(namespaceName)){
                notifications.erase(it);
                break;
            }
        }
    }
}

void RemoteConfigLongPollService::modifyNotifitionId(const std::string &namespaceName, const std::string &NotifitionId){
    // 需要对这个进行修改
    modifyNotifitionId(namespaceName, std::stoi(NotifitionId));

}
void RemoteConfigLongPollService::modifyNotifitionId(const std::string& namespaceName, int NotifitionId){
    {
        std::lock_guard<std::shared_mutex> lock(notificationsMutex);
        namespaceToNotificationsId[namespaceName] = NotifitionId;
        for (auto&it:notifications){
            if (it.count(namespaceName)){
                it[namespaceName] = std::to_string(NotifitionId);
                break;
            }
        }
    }
}
}


#include"RemoteConfigLongPollService.hpp"
#include<iostream>
#include<fstream>
#include "common.hpp"
#include "ApolloConfig.hpp"

namespace apollocpp{

RemoteConfigLongPollService::RemoteConfigLongPollService(const std::string& configJsonFile){
    // 使用json读取文件
    this->configJsonFile = configJsonFile;
    this->refreshConnectApolloConfig();
}

void RemoteConfigLongPollService::refreshConnectApolloConfig(){
    // 重置polling配置
    nlohmann::json j;
    std::ifstream input(configJsonFile);
    if (!input.is_open() || input.peek() == std::ifstream::traits_type::eof()) {
        std::cerr << "Failed to open config file or file is empty." << std::endl;
        return; // 或者采取其他适当的处理
    }
    input>>j;
    std::string namesapceName;
    try
    {
        appId = j.at("appId");
        host = j.at("host");
        cluster = j.at("cluster");
        if (j.contains("namesapceName")){
            namesapceName = j.at("namesapceName");
            addNotifications(namesapceName);
        }else{
            namesapceName = "";
        }
        longPollingTimeout = static_cast<long>(j.at("longPollingTimeout"))<60L?(60L):static_cast<long>(j.at("longPollingTimeout"));        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

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
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, longPollingTimeout);  // 超时设置
            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                if (res == CURLE_COULDNT_CONNECT) {
                    std::cerr << "Could not connect to server." << std::endl;
                    // continue;
                } else if (res == CURLE_HTTP_RETURNED_ERROR) {
                    std::cerr << "HTTP error occurred." << std::endl;
                    // 这里可根据状态码进行操作
                    break;
                } else if (res == CURLE_OPERATION_TIMEDOUT) {
                    std::cerr << "Operation timed out. " << "long poll continue." <<std::endl;
                    {
                    // 读锁
                        std::shared_lock<std::shared_mutex> lock(notificationsMutex);
                        if (notificationsUpdateflag){
                            // url需要更新
                            break;
                        }
                    }
                    // continue;
                } else {
                    std::cerr << "Curl error: " << curl_easy_strerror(res) << std::endl;
                }
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
        std::string url;
        {
            std::lock_guard<std::shared_mutex> lock(notificationsMutex);
            url = assembleLongPollRefreshUrl();
            notificationsUpdateflag = false; // 完成了本轮的数据更新后的URL
        }
        httpGet(url);
    }
}

std::string RemoteConfigLongPollService::assembleLongPollRefreshUrl(){
    nlohmann::json j = notifications;
    std::string jData = j.dump();
    std::cout<<"[info] url parameter: "<<jData<<std::endl;
    std::string encoded_notifications = apollocpp::url_encode(jData);
    std::string url = "http://"+host+"/notifications/v2?appId=" + appId + "&cluster=" + cluster+"&notifications="+encoded_notifications;
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
        // 加读锁
        for (const auto& notification : responseJson) {
            std::string reNamespaceName = notification["namespaceName"];
            int reNotificationId =  notification["notificationId"];
            // 拿到通知的结果
            // 现在就是要通知某个namespace的值更新了
            int tempNotifitionId;
            {
                std::shared_lock<std::shared_mutex> lock(notificationsMutex);
                tempNotifitionId = namespaceToNotificationsId[reNamespaceName];
            }
            if (tempNotifitionId != reNotificationId){
                // 配置进行更新，进行配置的通知：所监听的命名空间的版本进行更新了，进行通知
                modifyNotifitionId(reNamespaceName,reNotificationId);
                notifyClients(reNamespaceName);
            }
        }
    }

    void RemoteConfigLongPollService::notifyClients(const std::string& reNamespaceName) {
        std::cout << "Notifying clients of configuration changes..." << std::endl;
        std::cout<<"[info] need update namespaceName: "<<reNamespaceName<<std::endl;
        // 这里就是需要将配置信息落盘还是什么
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

    void RemoteConfigLongPollService::addNotifications(const std::string& namespaceName){
        const std::string notificationId = "-1";
        std::unordered_map<std::string, std::string> maps{{"namespaceName",namespaceName}, {"notificationId",notificationId}};
        std::lock_guard<std::shared_mutex> lock(notificationsMutex);
        try
        {
            std::cout<<"[info] add namespace: "<<namespaceName<<std::endl;
            namespaceToNotificationsId[namespaceName] = std::stoi(notificationId);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return; // 增加失败
        }
        notifications.push_back(maps);
        notificationsUpdateflag = true; // 数据标志位更新
    }

    void RemoteConfigLongPollService::addNotifications(const std::unordered_map<std::string, std::string> &maps)
    {
        // 增加一个maps。
        std::unordered_map<std::string, int> temp;
        std::lock_guard<std::shared_mutex> lock(notificationsMutex);
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
            notifications.push_back(maps);
            for (const auto&[key, val]:temp){
                namespaceToNotificationsId[key] = val;
            }
            notificationsUpdateflag = true; // 更新
        }
    }
void RemoteConfigLongPollService::addNotifications(const std::vector<std::string>& vec){
    // 多命名空间增加
    std::lock_guard<std::shared_mutex> lock(notificationsMutex);
    for (auto&it:vec){
        namespaceToNotificationsId[it] = -1;
        notifications.push_back({{it,"-1"}});
    }
}


void RemoteConfigLongPollService::deleteNotifications(const std::string &namespaceName){
    std::lock_guard<std::shared_mutex> lock(notificationsMutex);
    notificationsUpdateflag = true;
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

void RemoteConfigLongPollService::modifyNotifitionId(const std::string &namespaceName, const std::string &NotifitionIdStr){
    modifyNotifitionId(namespaceName, NotifitionIdStr,std::stoi(NotifitionIdStr));
}

void RemoteConfigLongPollService::modifyNotifitionId(const std::string& namespaceName, int NotifitionIdINT){
    modifyNotifitionId(namespaceName,std::to_string(NotifitionIdINT), NotifitionIdINT);
}

void RemoteConfigLongPollService::modifyNotifitionId(const std::string& namespaceName,const std::string& NotifitionStr ,int NotifitionId){
    {
        // 修改数据进行加锁：数据的修改两个地方
        std::lock_guard<std::shared_mutex> lock(notificationsMutex);
        namespaceToNotificationsId[namespaceName] = NotifitionId;
        // 这里不用修改
        for (auto&it:notifications){
            if (it["namespaceName"] == namespaceName){
                it["notificationId"] = NotifitionStr;
            }
        }
        // 修改后的结果 
        notificationsUpdateflag = true;
    }
}
}
// 初始化-1，访问之后就会立即更新通知一下配置，因为初始的apollo 可能不是-1；
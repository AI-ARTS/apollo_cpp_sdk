
#include"RemoteConfigLongPollService.hpp"
#include<iostream>
#include<fstream>
#include "common.hpp"
#include "ApolloConfig.hpp"
#include "Executor.hpp"
#include <fstream>
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
        // std::cerr << "Failed to open config file or file is empty." << std::endl;
        log.error("Failed to open config file or file is empty.");
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
        longPollingTimeout = static_cast<long>(j.at("longPollingTimeout"))>20L?(20L):static_cast<long>(j.at("longPollingTimeout"));       
        if (j.contains("configfiledir")){
            configconfigdir = j.at("configfiledir");
        }else{
            configconfigdir = "./";
        }

        if (j.contains("singleConfigCacheSize")){
            apolloconfigSize = static_cast<size_t>(j.at("singleConfigCacheSize"))>100?100:static_cast<size_t>(j.at("singleConfigCacheSize"));
        }else{
            apolloconfigSize = 100;
        }
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
                    // std::cerr <<  << std::endl;
                    log.error("Could not connect to server.");
                    // break; // 出现错误进行重试，
                } else if (res == CURLE_HTTP_RETURNED_ERROR) {
                    // std::cerr <<  << std::endl;
                    log.error("HTTP error occurred.");
                    // 这里可根据状态码进行操作
                    break;
                } else if (res == CURLE_OPERATION_TIMEDOUT) {
                    // std::cerr << "Operation timed out. " << "long poll continue." <<std::endl;
                    log.info("Operation timed out. long poll continue.");
                    {
                    // 读锁
                        std::shared_lock<std::shared_mutex> lock(notificationsMutex);
                        if (notificationsUpdateflag){
                            // url需要更新
                            break;
                        }   
                    }
                    continue; // 如果超时但是url不需要更新那么就再次发起请求
                } else {
                     log.error("Curl error: " + std::string(curl_easy_strerror(res)));
                }
                std::this_thread::sleep_for(std::chrono::seconds(1));
                break;
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
    // std::cout<<<<jData<<std::endl;
    log.info("url parameter: "+jData);
    std::string encoded_notifications = apollocpp::url_encode(jData);
    std::string url = "http://"+host+"/notifications/v2?appId=" + appId + "&cluster=" + cluster+"&notifications="+encoded_notifications;
    return url;
}

    void RemoteConfigLongPollService::processResponse(const std::string& response) {
        if (response.empty()) {
            return;
        }
        // std::cout<< response<<std::endl;
        log.info("response: "+response);
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
                // std::thread th(&RemoteConfigLongPollService::notifyClients,this, reNamespaceName);
                // Executor::submit(&RemoteConfigLongPollService::notifyClients,this, reNamespaceName);
                notifyClients(reNamespaceName);
            }
        }
    }

    void RemoteConfigLongPollService::notifyClients(const std::string& reNamespaceName) {
        log.info("need update namespaceName: "+reNamespaceName);
        std::unique_lock<std::shared_mutex> lock(apolloconfigMutex);
        if (apolloconfig.count(reNamespaceName)){
            lock.unlock(); // 加锁的意义，防止频繁的发布导致数据的混乱
            auto apolloconfigPtr = apolloconfig[reNamespaceName];
            apolloconfigPtr->fetchConfigJsonCache();
        }else{
            apolloconfig[reNamespaceName] = std::make_shared<ApolloConfig>(host, appId, cluster, reNamespaceName);
            apolloconfig[reNamespaceName]->fetchConfigJsonCache();
        }
        log.info(reNamespaceName+" update: "+ apolloconfig[reNamespaceName]->getDataString());
        flushdisk(apolloconfig[reNamespaceName],reNamespaceName);
    }

    void RemoteConfigLongPollService::flushdisk(const std::shared_ptr<ApolloConfig> &apolloc, const std::string namespaceName)
    {
        // 就是写文件
        nlohmann::json j = apolloc->getConfigData();// 拿到这个数据
        std::string place = configconfigdir+"/"+namespaceName+".json";
        std::ofstream output(place,std::ios_base::out);
        if (output.is_open()){
            output<<j.dump();
            output.flush();
            output.close();
            log.info("flush disk success: "+place);
        }else{
            log.error("open file faild:"+place);
        }
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
            // std::cout<<<<namespaceName<<std::endl;
            log.info("add namespace: "+namespaceName);
            namespaceToNotificationsId[namespaceName] = std::stoi(notificationId);
        }
        catch(const std::exception& e)
        {
            log.error("addNotifications error:"+std::string(e.what()));
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
            log.error("addNotifications error:"+std::string(e.what()));
            return; // 增加失败
        }
        notifications.push_back(maps);
        for (const auto&[key, val]:temp){
            namespaceToNotificationsId[key] = val;
        }
        notificationsUpdateflag = true; // 更新
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
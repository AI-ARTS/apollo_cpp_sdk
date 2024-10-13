
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
    dummy = new Node;tail = new Node;
    dummy->next = tail; tail->pre = dummy;
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
            addNotifications(namesapceName, false);
        }else{
            namesapceName = "";
        }
        longPollingTimeout = static_cast<long>(j.at("longPollingTimeout"))<=20L?(20L):static_cast<long>(j.at("longPollingTimeout"));       
        if (j.contains("configfiledir")){
            configconfigdir = j.at("configfiledir");
        }else{
            configconfigdir = "./";
        }

        if (j.contains("singleConfigCacheSize")){
            apolloconfigSize = static_cast<size_t>(j.at("singleConfigCacheSize"))>APOLLOCONFIGCACHESIZE?APOLLOCONFIGCACHESIZE:static_cast<size_t>(j.at("singleConfigCacheSize"));
        }else{
            apolloconfigSize = 100;
        }
        if (j.contains("secret")){
            secretKey = j.at("secret");
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
        // 添加请求头

    // 转换为时间戳（自1970年1月1日以来的秒数）
        log.info("URL: "+url);
        while (!longPollingStopped) {
            readBuffer.clear();  // 清空读取缓冲区
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, longPollingTimeout);  // 超时设置
            if (!secretKey.empty()){
                // 秘钥不等于空那就进行加密操作
                struct curl_slist* headers = NULL;
                auto now = std::chrono::system_clock::now();
                auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();  // 毫秒级时间戳
                std::string sign = signature(std::to_string(timestamp), uri, secretKey);
                std::string Authorization = "Authorization: Apollo " + appId + ":" + sign;
                std::string Timestamp = "Timestamp: " + std::to_string(timestamp);
                log.info(Authorization);
                log.info(Timestamp);
                headers = curl_slist_append(headers,Authorization.c_str());
                headers = curl_slist_append(headers,Timestamp.c_str());
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            }

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
        url = assembleLongPollRefreshUrl();
        notificationsUpdateflag = false; // 完成了本轮的数据更新后的URL
        httpGet(url);
    }
}

/*
@ 线程安全
*/
std::string RemoteConfigLongPollService::assembleLongPollRefreshUrl(){
    std::shared_lock<std::shared_mutex> lock(notificationsMutex); 
    nlohmann::json j = notifications;
    lock.unlock();
    std::string jData = j.dump();
    log.info("url parameter: "+jData);
    std::string encoded_notifications = apollocpp::url_encode(jData);
    std::string query = "appId=" + appId + "&cluster=" + cluster+"&notifications="+encoded_notifications;
    std::string path = "/notifications/v2";
    uri = path+"?"+query;
    std::string url = "http://"+host+path+"?"+query;
    return url;
}

void RemoteConfigLongPollService::processResponse(const std::string& response) {
    if (response.empty()) {
        return;
    }
    log.info("response: "+response);
    // 首先解析出状态码
    // Parse JSON response (simulated as JSON string)
    nlohmann::json responseJson = nlohmann::json::parse(response);
    if (responseJson.contains("status")){
        // 包含状态码
        int status = responseJson["status"].get<int>();
        if (status == 400){
            log.error("Error in client input parameters.");
        }else if (status == 401){
            log.error("Client not authorized.");
        }else if (status == 404){
            log.error("The resource that the interface wants to access does not exist.");
        }else if (status == 405){
            log.error("The method used to access the interface is incorrect.");
        }else if (status == 500){
            log.error("Unknown error.");
        }else{
            log.error("Unknown error.");
        }
        return;
    }
    std::vector<std::unordered_map<std::string, std::string>> maps;
    for (const auto& notification : responseJson) {
        std::string reNamespaceName = notification["namespaceName"];
        int reNotificationId =  notification["notificationId"];
        int tempNotifitionId;
        {
            std::shared_lock<std::shared_mutex> lock(notificationsMutex);
            tempNotifitionId = namespaceToNotificationsId[reNamespaceName];
        }
        if (tempNotifitionId != reNotificationId){
            modifyNotifitionId(reNamespaceName,reNotificationId);
            notifyClients(reNamespaceName);
        }
    }
}

void RemoteConfigLongPollService::notifyClients(const std::string& reNamespaceName) {
    log.info("need update namespaceName: "+reNamespaceName);
    std::shared_lock<std::shared_mutex> lock(apolloconfigMutex);
    if (apolloconfig.count(reNamespaceName)){
        lock.unlock(); 
        auto apolloconfigPtr = apolloconfig[reNamespaceName];
        apolloconfigPtr->data->fetchConfigJsonCache();
        moveNodeHead(apolloconfig[reNamespaceName]);
    }else{
        // 不存在
        if (apolloconfig.size()>=apolloconfigSize){
            Node* deleteNode = tail->pre;
            deleteNode->pre->next = deleteNode->next;
            deleteNode->next->pre = deleteNode->pre;
            apolloconfig.erase(deleteNode->namespaceName);
            delete deleteNode; // 删除此Node
        }
        Node* newNode = new Node(reNamespaceName);
        apolloconfig[reNamespaceName] = newNode;
        // 需要验证就需要加秘钥，不需要验证就不加
        newNode->data = std::make_shared<ApolloConfig>(host, appId, cluster, reNamespaceName,secretKey);
        insertNodeHead(newNode);
        newNode->data->fetchConfigDatabases(); // 更新从数据库中拿数据，配合配置推送进行更新配置
    }
    log.info(reNamespaceName+" update: "+ apolloconfig[reNamespaceName]->data->getDataString());
    auto apolloval = apolloconfig[reNamespaceName];
    Executor::submit([this, apolloval, reNamespaceName](){
        log.info("flushdisk:"+reNamespaceName);
        this->flushdisk(apolloval->data, reNamespaceName);
    });
}

void RemoteConfigLongPollService::flushdisk(const std::shared_ptr<ApolloConfig> &apolloc, const std::string namespaceName)
{
    // 就是写文件
    nlohmann::json j = apolloc->getConfigData();// 拿到这个数据
    std::string place = configconfigdir+"/"+host+"_"+appId+"_"+cluster+"_"+namespaceName+".json";
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

void RemoteConfigLongPollService::moveNodeHead(Node *node)
{
    node->pre->next = node->next;
    node->next->pre = node->pre;
    
    node->next = dummy->next;
    node->pre = dummy;
    dummy->next = node;
    node->next->pre = node;
}

void RemoteConfigLongPollService::insertNodeHead(Node *node)
{
    node->next = dummy->next;
    node->pre = dummy;
    dummy->next = node;
    node->next->pre = node;
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

void RemoteConfigLongPollService::addNotifications(const std::string& namespaceName, bool flag){
    const std::string notificationId = "-1";
    std::unordered_map<std::string, std::string> maps{{"namespaceName",namespaceName}, {"notificationId",notificationId}};
    {
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
    // 进行及时更新数据
    if (flag){
        auto url = assembleLongPollRefreshUrl();
        httpGet(url);
    }
}

void RemoteConfigLongPollService::addNotifications(const std::unordered_map<std::string, std::string> &maps, bool flag)
{
    // 增加一个maps。
    std::unordered_map<std::string, int> temp;
    {
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
    // 进行及时更新数据
    if (flag){
        auto url = assembleLongPollRefreshUrl();
        httpGet(url);
    }
}

void RemoteConfigLongPollService::addNotifications(const std::vector<std::string>& vec, bool flag){
    // 多命名空间增加    
    {
        std::lock_guard<std::shared_mutex> lock(notificationsMutex);
        for (auto&it:vec){
            namespaceToNotificationsId[it] = -1;
            notifications.push_back({{it,"-1"}});
        }
        notificationsUpdateflag = true;
    }
     // 进行及时更新数据
    if (flag){
        auto url = assembleLongPollRefreshUrl();
        httpGet(url);
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
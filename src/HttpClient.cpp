#include "HttpClient.hpp"
#include <curl/curl.h>
#include<iostream>
#include <unistd.h>
#include <chrono>
#include "common.hpp"
namespace apollocpp{
log4cpp::Category& HttpClient::log = log4cpp::Category::getRoot();

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

 std::string HttpClient::getURI(const std::string& url){
    size_t pos = url.find(':',6);
    while (pos<url.size() and url[pos]!='/')
    {
        pos++;
    }
    std::string uri = url.substr(pos, url.size()-pos);
    log.info("uri:"+uri);
    return uri;
 }


std::string HttpClient::get(const std::string& url, const std::string& appId,const std::string& secret) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    int temp = 0;
    while (true){
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            // 如果说secret存在，那么说明需要身份验证
            if (!secret.empty()){
                // 身份验证
                std::string uri = getURI(url);
                struct curl_slist* headers = NULL;
                auto now = std::chrono::system_clock::now();
                auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();  // 毫秒级时间戳
                std::string sign = signature(std::to_string(timestamp), uri, secret);
                std::string Authorization = "Authorization: Apollo " + appId + ":" + sign;
                std::string Timestamp = "Timestamp: " + std::to_string(timestamp);
                log.info(Authorization);
                log.info(Timestamp);
                headers = curl_slist_append(headers,Authorization.c_str());
                headers = curl_slist_append(headers,Timestamp.c_str());
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            }

            res = curl_easy_perform(curl);
            temp++;
            // 检查 res 是否成功
            if (res != CURLE_OK) {
                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
                readBuffer.clear();  // 清空读取缓冲区
                if (temp!=5){
                    sleep(1); // 短连接容灾重试
                    log.info("Disaster recovery retry.");
                    continue;
                }
            }
        }
        break;
    }
    curl_easy_cleanup(curl);
    return readBuffer;
}

}

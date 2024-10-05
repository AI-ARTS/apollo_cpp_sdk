#pragma once
#include <string>
#include<vector>
#include<unordered_map>
#include<memory>
namespace apollocpp{
    class APP{ // 单app多集群
    private:
        std::string appid;
        // key：集群，val：集群命名空间
        std::unordered_map<std::string, std::vector<std::string>> cluster;
        int longPollingTimeout;
        std::string configfiledir; // 配置信息的记录地点
        int singleConfigCacheSize;
    public:
        std::string getAppId()const{return appid;}
        std::vector<std::string> getclusterNamespace(const std::string& clustername){
            if (cluster.count(clustername)) return cluster[clustername];
            return{};
        }
        int getlongPollingTimeout()const { return longPollingTimeout;}
        std::string getconfigfiledir()const{return configfiledir;}
        int getsingleConfigCacheSize()const{return singleConfigCacheSize;}
    };
    // host(host), appId(appId), cluster(cluster), namespace 对应一组配置
    class MultiServer{
    private:
    std::unordered_map<std::string, std::vector<std::shared_ptr<APP>>> server;
    bool parseJsongConfig();
    public:
        MultiServer(const std::string& configfilename);
        MultiServer();
        bool addServer();
        bool addApp();
        bool addCluster();
        bool addNameSpaceName();
    };
}
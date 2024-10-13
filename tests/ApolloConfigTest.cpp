#include "ApolloConfig.hpp"
#include "HttpClient.hpp"
#include "RemoteConfigLongPollService.hpp"
#include <log4cpp/Category.hh>
#include "common.hpp"
#include <log4cpp/PropertyConfigurator.hh>

int main(){
    // apollocpp::RemoteConfigLongPollService longPollService("/home/aldd/JD/apollo_cpp_sdk/configfile/singleapolloserve/config.json");
    log4cpp::PropertyConfigurator::configure("/home/aldd/JD/apollo_cpp_sdk/configfile/logconfig/log4cpp.properties");
    // longPollService.startLongPolling();
    // std::this_thread::sleep_for(std::chrono::seconds(4));
    // longPollService.addNotifications("application");
    // std::this_thread::sleep_for(std::chrono::seconds(200000));
    // longPollService.stopLongPolling();
    struct apollocpp::EvnData env;
    env.host = "localhost:8080";
    env.appid = "aldd"; env.cluster = "arvintzhang";
    env.namespaceName = "RJDLOG"; 
    env.ip = apollocpp::getLocalIP();

    // apollocpp::ApolloConfig apolloclient(env.host,env.appid,env.cluster,env.namespaceName, env.secret, env.releaseKey, env.ip, env.label, env.message,env.sleepsecondtime);
    apollocpp::ApolloConfig apolloclient(env);
    // apolloclient.fetchConfigJsonCache();
    // apolloclient.fetchConfigDatabases();
    apolloclient.fetchConfigPropertiesCache();
    auto it = apolloclient.getConfigData();
    for (auto&[key, val]:it){
        std::cout<<key<<" "<<val<<std::endl;
    }
    std::this_thread::sleep_for(std::chrono::seconds(3));
    return 0;
}
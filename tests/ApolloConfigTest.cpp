#include "ApolloConfig.hpp"
#include "HttpClient.hpp"
#include "RemoteConfigLongPollService.hpp"
#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>

int main(){
    apollocpp::RemoteConfigLongPollService longPollService("/home/aldd/JD/apollo_cpp_sdk/configfile/singleapolloserve/config.json");
    log4cpp::PropertyConfigurator::configure("/home/aldd/JD/apollo_cpp_sdk/configfile/logconfig/log4cpp.properties");
    longPollService.startLongPolling();
    std::this_thread::sleep_for(std::chrono::seconds(4));
    longPollService.addNotifications("application");
    std::this_thread::sleep_for(std::chrono::seconds(200000));
    longPollService.stopLongPolling();
    return 0;
}
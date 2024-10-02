#include "ApolloConfig.hpp"
#include "HttpClient.hpp"
#include "RemoteConfigLongPollService.hpp"
int main(){
    apollocpp::RemoteConfigLongPollService longPollService("/home/aldd/JD/apollo_cpp_sdk/config.json");
    longPollService.startLongPolling();
    std::this_thread::sleep_for(std::chrono::seconds(4));
    longPollService.addNotifications("application");
    std::this_thread::sleep_for(std::chrono::seconds(20000));
    longPollService.stopLongPolling();
    return 0;
}
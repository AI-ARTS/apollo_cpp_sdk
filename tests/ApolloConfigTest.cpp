#include "ApolloConfig.hpp"
#include "HttpClient.hpp"
#include "RemoteConfigLongPollService.hpp"
int main(){
    apollocpp::RemoteConfigLongPollService longPollService("localhost:8080","aldd","arvintzhang","RJDLOG");
    longPollService.startLongPolling();
    std::this_thread::sleep_for(std::chrono::seconds(10));
    longPollService.addNotifications("application");
    std::this_thread::sleep_for(std::chrono::seconds(20000));
    longPollService.stopLongPolling();
    return 0;
}
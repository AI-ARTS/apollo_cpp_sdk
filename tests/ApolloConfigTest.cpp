#include "ApolloConfig.hpp"
#include "HttpClient.hpp"
#include "RemoteConfigLongPollService.hpp"
int main(){
    apollocpp::RemoteConfigLongPollService longPollService("localhost:8080","aldd","arvintzhang","RJDLOG");
    longPollService.startLongPolling();
    // longPollService.addNotifications("RJDLOG");
    std::this_thread::sleep_for(std::chrono::seconds(200));
    longPollService.stopLongPolling();
    return 0;
}
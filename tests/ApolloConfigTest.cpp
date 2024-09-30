#include "ApolloConfig.hpp"
#include "HttpClient.hpp"

int main(){

    apollocpp::ApolloConfig config("localhost:8080","aldd", "arvintzhang", "RJDLOG");
    // config.fetchConfig();
    config.fetchConfigJsonCache();
    config.fetchConfigStrCache();
    config.fetchConfigDatabases();
    auto res = config.getConfigData();
    return 0;
}
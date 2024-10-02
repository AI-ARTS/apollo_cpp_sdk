# Apollo-client-C++
Apollo配置中心C++客户端
## 测试实例
这个地方传入地址，APPID，集群，命名空间就可访问到配置结果。
下面对应了官方文档的访问方式，不同的访问方式的访问，我都写了API

```cpp
#include "ApolloConfig.hpp"
#include "HttpClient.hpp"

int main(){

    apollocpp::ApolloConfig config("localhost:8080","aldd", "arvintzhang", "RJDLOG");
    // config.fetchConfig();
    config.fetchConfigJsonCache();
    config.fetchConfigStrCache();
    config.fetchConfigDatabases();
    return 0;
}
```
## 长轮训测试示例
```cpp
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
```
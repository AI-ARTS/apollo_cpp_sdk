# Apollo-client-C++
Apollo配置中心C++客户端
## 架构以及功能介绍
此为apollo客户端，当前功能包括获取拉取apollo配置功能所拥有的接口介绍如下：
`void fetchConfigJsonCache();`：返回json格式的配置数据
`void fetchConfigStrCache();` ：返回 string类型的配置数据
`void fetchConfigDatabases();`：通过不带缓存的Http接口从Apollo读取配置，该接口会直接从数据库中获取配置，可以配合配置推送通知实现实时更新配置。
应用实时感知配置更新，使用长轮训进行配置的感应，首次进行访问，会将namespaceName进行注册，在长轮训返回结果等待的情况下，如果apollo有命名空间的配置更新了，那么会将信息返回。
结果示例
```txt
[{"namespaceName":"application","notificationId":29,"messages":{"details":{"aldd+arvintzhang+application":29,"aldd+default+application":2}}}]
```
从其中解析出notificationId进行本地的notificationId的配置更新推送等操作。
```cpp
    // 长轮训启动停止
    void startLongPolling();
    void stopLongPolling();
    // 增加命名空间
    void addNotifications(const std::string& namespaceName);
    void addNotifications(const std::vector<std::string>& vec);
    void addNotifications(const std::unordered_map<std::string, std::string> &maps);

    // 删除某个命名空间
    void deleteNotifications(const std::string& namespaceName);

    // 改命名空间的值
    void modifyNotifitionId(const std::string& namespaceName, const std::string& NotifitionIdStr);
    void modifyNotifitionId(const std::string& namespaceName, int NotifitionIdINT);
    void modifyNotifitionId(const std::string& namespaceName, const std::string& NotifitionIdStr ,int NotifitionId);

    // 控制配置数据size
    void setApolloconfigSzie(size_t size){apolloconfigSize = (size>APOLLOCONFIGCACHESIZE?APOLLOCONFIGCACHESIZE:size);}
    size_t getApolloconfigSzie()const{return apolloconfigSize;}
    
    // 刷新配置
    void refreshConnectApolloConfig();
```
后续增加更多api接口完善其客户端，将其打造的尽善尽美。
后续增加功能多apollo，多应用，多集群，多命名空间的配置感知，提供相应接口。
主动推送接口，被动拉取接口等
## 测试实例
这个地方传入地址，APPID，集群，命名空间就可访问到配置结果。
根据官方文档进行设计客户端。

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

## json文件初始化配置长轮训
还可书写配置文件进行配置感知系统的初始化,例如
```json
{
    "host" :"localhost:8080",
    "appId" :"aldd",
    "cluster" :"arvintzhang", 
    "longPollingTimeout" :90,
    "namesapceName":"RJDLOG"
}
```
示例代码
```cpp
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
```

## 项目疑问解答
如果现在在轮训。增加了命名空间，而此时配置进行了更新怎么办，处理逻辑是什么？
那么由于初始化值为-1，因此得到的最新的配置的id时会发现不一样，此时就会进行数据的更新


## 接口
```cpp
void RemoteConfigLongPollService::notifyClients(const std::string& reNamespaceName);
```

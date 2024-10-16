# Apollo-SDK-CPP: Apollo配置中心C++客户端

Apollo-SDK-CPP 是一个基于 Apollo 配置中心的 C++ 客户端，用于方便地获取、管理和动态更新配置。它通过灵活的接口和高效的系统架构，满足多场景下的配置管理需求。

## 功能概览与架构介绍

Apollo-SDK-CPP 客户端提供了一系列接口以获取和管理配置，支持多种数据格式并具备高效的配置感知功能。

### 基础功能

- **配置获取接口**
  - `fetchConfigJsonCache()`: 获取 JSON 格式的配置数据。
  - `fetchConfigPropertiesCache()`: 获取Properties格式的配置数据。
  - `fetchConfigDatabases()`: 通过不带缓存的 HTTP 接口直接从 Apollo 获取配置，支持实时更新。

### 长轮询机制

该客户端通过长轮询实现配置更新的实时感知。当应用首次访问时，将注册 `namespaceName`，并在等待配置更新的过程中，长轮询机制可感知配置的变更并返回最新信息。

### 客户端容灾机制

- **长轮询重试机制**：在请求失败时，暂停 1 秒后重新启动长轮询，每次重试间隔时间逐步翻倍，最多尝试 5 次。
- **短轮询容灾**：在短轮询过程中发生失败时，也会进行 5 次重试，每次间隔时间逐步翻倍。
- **数据持久化**：当获取到最新数据时，将热数据缓存在内存中，同时由执行器将任务持久化存储到磁盘，确保数据的安全性。

### 灰度发布支持

客户端支持灰度发布，只需在传入参数时加入 `ip` 或 `label` 参数即可。默认情况下，心跳机制开启，心跳时间间隔为 10 分钟，具体间隔时间可通过参数调整。

### 热数据与冷数据管理

- **热数据**：使用 LRU 算法缓存热数据，实时向集群报告最新的状态。
- **冷数据**：将冷数据持久化存储，以保证配置数据的持久性和安全性。

# 安装

1. 直接将头文件和lib库里面的文件拷贝到你的项目中
2.
    ```bash
    >cd build
    >sudo make install
    ```
## 使用示例

### 配置获取结果示例

```json
[{
  "namespaceName": "application",
  "notificationId": 29,
  "messages": {
    "details": {
      "aldd+arvintzhang+application": 29,
      "aldd+default+application": 2
    }
  }
}]
```

从该结果中可以解析 `notificationId`，并进行本地通知更新等操作。

### 接口代码示例

```cpp
// 长轮询启动与停止
void startLongPolling();
void stopLongPolling();

// 增加命名空间
void addNotifications(const std::string& namespaceName);
void addNotifications(const std::vector<std::string>& vec);
void addNotifications(const std::unordered_map<std::string, std::string> &maps);

// 删除命名空间
void deleteNotifications(const std::string& namespaceName);

// 修改命名空间通知 ID
void modifyNotifitionId(const std::string& namespaceName, const std::string& NotifitionIdStr);
void modifyNotifitionId(const std::string& namespaceName, int NotifitionIdINT);
void modifyNotifitionId(const std::string& namespaceName, const std::string& NotifitionIdStr, int NotifitionId);

// 控制配置数据大小
void setApolloconfigSize(size_t size);
size_t getApolloconfigSize() const;

// 刷新 Apollo 配置连接
void refreshConnectApolloConfig();
```

未来版本将增加更多 API 以完善客户端功能，包括多 Apollo 实例、多应用、多集群、多命名空间的配置感知接口，以及主动推送与被动拉取的机制。

## 测试用例

### 配置获取示例

以下代码演示了如何使用 Apollo-SDK-CPP 客户端来获取配置数据。

```cpp
#include "ApolloConfig.hpp"
#include "HttpClient.hpp"

int main() {
    apollocpp::ApolloConfig config("localhost:8080", "aldd", "arvintzhang", "RJDLOG");
    config.fetchConfigJsonCache();
    config.fetchConfigStrCache();
    config.fetchConfigDatabases();
    return 0;
}
```

### 长轮询测试用例

```cpp
#include "ApolloConfig.hpp"
#include "HttpClient.hpp"
#include "RemoteConfigLongPollService.hpp"

int main() {
    apollocpp::RemoteConfigLongPollService longPollService("localhost:8080", "aldd", "arvintzhang", "RJDLOG");
    longPollService.startLongPolling();
    std::this_thread::sleep_for(std::chrono::seconds(10));
    longPollService.addNotifications("application");
    std::this_thread::sleep_for(std::chrono::seconds(20000));
    longPollService.stopLongPolling();
    return 0;
}
```

### 使用 JSON 文件初始化配置长轮询

还可以通过 JSON 配置文件进行初始化，以便更灵活地配置长轮询系统。

```json
{
    "host": "localhost:8080",
    "appId": "aldd_test",
    "cluster": "default",
    "namespaceName": "application",
    "longPollingTimeout": 60,
    "configfiledir": "/home/aldd/JD/apollo_cpp_sdk/configfile/source",
    "singleConfigCacheSize": 200,
    "secret": "b22b96b1025f4feba1c3cbaacbb91d0a"
}
```

对应的 C++ 代码如下：

```cpp
#include "ApolloConfig.hpp"
#include "HttpClient.hpp"
#include "RemoteConfigLongPollService.hpp"

int main() {
    apollocpp::RemoteConfigLongPollService longPollService("/home/aldd/JD/apollo_cpp_sdk/config.json");
    longPollService.startLongPolling();
    std::this_thread::sleep_for(std::chrono::seconds(4));
    longPollService.addNotifications("application");
    std::this_thread::sleep_for(std::chrono::seconds(20000));
    longPollService.stopLongPolling();
    return 0;
}
```

## 常见问题解答

- **如果在长轮询过程中新增了命名空间，同时该配置被更新，系统如何处理？**
  - 在初始化时，命名空间的通知 ID 为 `-1`，当系统检测到新的配置更新时，会发现当前 ID 与最新配置的 ID 不一致，从而触发数据的更新。

## 核心接口

```cpp
void RemoteConfigLongPollService::notifyClients(const std::string& reNamespaceName);
```

该接口用于在配置变更时通知相应的客户端更新其缓存。

## 依赖项

- **nlohmann_json**：用于 JSON 解析。
- **CURL**：用于 HTTP 请求。
- **log4cpp**：用于日志记录。
- **Threads**：用于多线程支持。
- **OpenSSL**：用于加密和安全通信。

## 未来计划

- 支持多 Apollo 实例、多集群与命名空间的扩展感知。
- 增加更多 API 接口，如主动推送与被动拉取，完善配置管理体系。
- 优化客户端的性能，提升容灾处理的效率与可用性。

通过 Apollo-SDK-CPP，开发者可以轻松地集成 Apollo 配置中心的强大功能，确保应用在分布式环境下的稳定运行与动态配置的高效管理。


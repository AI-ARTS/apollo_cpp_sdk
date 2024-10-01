#pragma once
#include <string>

namespace apollocpp{
// 短连接
class HttpClient {
public:
    static std::string get(const std::string& url);
};

}

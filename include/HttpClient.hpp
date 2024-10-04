#pragma once
#include <string>
#include<log4cpp/Category.hh>

namespace apollocpp{
// 短连接
class HttpClient {
private:
    static log4cpp::Category& log;
public:
    static std::string get(const std::string& url);
};

}

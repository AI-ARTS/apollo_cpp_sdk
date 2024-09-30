#pragma once
#include <string>

namespace apollocpp{
class HttpClient {
public:
    static std::string get(const std::string& url);
};

}

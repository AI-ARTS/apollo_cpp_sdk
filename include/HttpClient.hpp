#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <string>

namespace apollocpp{
class HttpClient {
public:
    std::string get(const std::string& url);
};

}
#endif // HTTP_CLIENT_H

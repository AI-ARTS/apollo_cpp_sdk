#pragma once
#include <iostream>
#include <string>
#include <cassert>

namespace apollocpp{
// 将字符转换为16进制
char ToHex(unsigned char x);

// 将16进制字符转换为十进制
unsigned char FromHex(unsigned char x);

// URL 编码函数
std::string url_encode(const std::string& str);

// URL 解码函数
std::string url_decode(const std::string& str);

std::string base64_encode(const unsigned char* input, int length);

std::string signature(const std::string& timestamp, const std::string& uri, const std::string& secret) ;

}

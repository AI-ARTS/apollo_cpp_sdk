#include"common.hpp"
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

namespace apollocpp{

char ToHex(unsigned char x){
    return x > 9 ? x + 55 : x + 48;  // 大于9的数转换为A-F，小于9的转换为0-9
}

unsigned char FromHex(unsigned char x){
    if (x >= 'A' && x <= 'F') return x - 'A' + 10;
    if (x >= 'a' && x <= 'f') return x - 'a' + 10;
    if (x >= '0' && x <= '9') return x - '0';
    assert(false);
    return 0;
}

std::string url_encode(const std::string& str)
{
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        if (isalnum((unsigned char)str[i]) || 
            (str[i] == '-') ||
            (str[i] == '_') || 
            (str[i] == '.') || 
            (str[i] == '~'))
        {
            strTemp += str[i];
        }
        else if (str[i] == ' ')
        {
            strTemp += "+";
        }
        else
        {
            strTemp += '%';
            strTemp += ToHex((unsigned char)str[i] >> 4);
            strTemp += ToHex((unsigned char)str[i] % 16);
        }
    }
    return strTemp;
}

std::string url_decode(const std::string& str)
{
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        if (str[i] == '+') 
        {
            strTemp += ' ';
        }
        else if (str[i] == '%')
        {
            assert(i + 2 < length);
            unsigned char high = FromHex((unsigned char)str[++i]);
            unsigned char low = FromHex((unsigned char)str[++i]);
            strTemp += high * 16 + low;
        }
        else 
        {
            strTemp += str[i];
        }
    }
    return strTemp;
}
std::string base64_encode(const unsigned char *input, int length)
{
    BIO* bio;
    BIO* b64;
    BUF_MEM* bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); // 不使用换行符
    BIO_write(bio, input, length);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    std::string result(bufferPtr->data, bufferPtr->length);
    BIO_free_all(bio);
    return result;
}

std::string signature(const std::string &timestamp, const std::string &uri, const std::string &secret) {
    std::string stringToSign = timestamp + "\n" + uri;
    unsigned char* hmacResult;
    unsigned int hmacLength;

    hmacResult = HMAC(EVP_sha1(), secret.c_str(), secret.length(),
                      reinterpret_cast<const unsigned char*>(stringToSign.c_str()),
                      stringToSign.length(), nullptr, &hmacLength);

    return base64_encode(hmacResult, hmacLength);
}

};
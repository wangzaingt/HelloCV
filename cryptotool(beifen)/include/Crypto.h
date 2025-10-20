#ifndef CRYPTO_H
#define CRYPTO_H

#include <string>

class Crypto {
public:
    // 加密函数
    static std::string encrypt(const std::string& text, int key);
    
    // 解密函数
    static std::string decrypt(const std::string& text, int key);
};

#endif
